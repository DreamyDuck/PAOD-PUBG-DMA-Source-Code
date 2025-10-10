#include "PhysicStruct.h"
#include "../Visuals.h"
#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H
//#define DebugTrace
//#define pvdConnect
#define SaveFileToPath

namespace IronMan::Core
{
	using namespace physx;

	class PhysicsManager {
	public:
		PhysicsManager(bool Open);
		~PhysicsManager();
		//场景对象是否有效
		bool IsRigidValid(ptr_t Rigid);
		bool validateIndices(const PxTriangleMeshDesc& meshDesc, const GuTriangleMesh& TriangleMesh);
		//初始化Physx
		void InitPhysX();
		//销毁所有成员
		bool DestroyAllRigidBodies();
		//从场景中移除对象
		void RemoveActorFromScene(PxRigidActor* Actor);
		// 保存三角网格数据到文件
		void saveTriangleMeshToFile(PxTriangleMesh* triangleMesh, const char* filename);
		// 从文件加载三角网格数据
		std::pair<PxTriangleMesh*, void*> loadTriangleMeshFromFile(const char* filename);
		// 保存高度场数据到文件
		void saveHeightFieldToFile(PxHeightField* heightField, const char* filename);
		// 从文件加载高度场数据
		std::pair <PxHeightField*, void*> loadHeightFieldFromFile(const char* filename);

		//Math
		IronMan::Core::SDK::FVector PxVec3ToFVector(const PxVec3& vec);
		IronMan::Core::SDK::FQuat PxQuatToFQuat(const PxQuat& Quat);
		PxVec3 FVectorToPxVec3(const IronMan::Core::SDK::FVector& vec);
		PxQuat FQuatToPxQuat(const IronMan::Core::SDK::FQuat& Quat);
		PxQuat PxQuat_Multy(physx::PxQuat _this, const physx::PxQuat q);
		PxTransform CalulateDynamicGlobalPose2(PxTransform p_body2Actor, PxTransform p_mBufferedBody2World);
		int computeCellCoordinates(float rowLimit, float colLimit, float nbColumns, float x, float z, float* fracX, float* fracZ);
		float getHeightInternal2(ptr_t Samples, float NumCols, unsigned int vertexIndex, float fracX, float fracZ);
		float getHeightInternal2Cache(VMMDLL_SCATTER_HANDLE handle, ptr_t Samples, float NumCols, unsigned int vertexIndex, float fracX, float fracZ);
		//获取当前世界场景
		void* GetPxScene(__int64 a1, uint32_t* a2, __int16 a3);
		ptr_t GetPhysxSceneFromUWorld(ptr_t world);

		//从场景中筛选需要添加到镜像的刚体
		RigidFreshType FilterDynamicRigid(PxFilterData SimData, int VerticesSize);
		bool checkTransformIsSame(PxRigidActor* Actor, PxTransform Transform);
		//RayCast调用光线投射
		bool WeaponCanHit(FVector& origin, FVector& direction, float maxDistance = 1000000.f, bool showDebug = false, bool bTrigger = false, bool CarFilter = false);
		//更新场景对象的坐标
		void setGlobalPoseInternal(PxRigidActor* RigidActor, PxTransform Transform);
		//线程1
		void InsertTriangleMap(std::shared_ptr<RigiBodyStruct> RigidBody);
		bool UpdateInfoAndCheckPath();
		void ClearInValidRigid(std::unordered_map<ptr_t, std::shared_ptr<RigiBodyStruct>>& RIGIDARRAY, std::unordered_map<ptr_t, std::shared_ptr<int>>& EraseVector);
		void AddRigidBodiesToScene(std::unordered_map<ptr_t, std::shared_ptr<RigiBodyStruct>>& RIGIDARRAY, std::unordered_map<ptr_t, std::shared_ptr<int>>& UpdateArray, std::unordered_map<ptr_t, std::shared_ptr<int>>& EraseVector);
		void UpdateRigidData(VMMDLL_SCATTER_HANDLE ScatterHandle);
		void asyncScenes();
		//线程2
		//更新玩家坐标形态的坐标
		void setActorGlobalPoseInternal(PxRigidDynamic* RigidActor, PxTransform Transform, float radiu, float halfhigh);
		//更新线程
		void UpdateAlwaysUpdatedRigid(VMMDLL_SCATTER_HANDLE ScatterHandle);
		void UpdateActorRigid();
		void syncScenes();
		//绘制相关
		void DrawRigidBox();
		void DrawSingleRigid(PxRigidActor* Actor);
		//设置所有需要测试光线追踪的玩家和对应坐标
		inline void SetRayCast(std::unordered_map <ptr_t, std::vector<FVector>> BoneArray)
		{
			unique_writeguard<WfirstRWLock> wlock(RayCastArrayLock);
			std::vector<ptr_t>EraseVec;

			for (auto iter : RayCastArray)
			{
				try
				{
					auto FindResult = BoneArray.find(iter.first);
					if (FindResult == BoneArray.end() && FindResult->first != iter.first)
					{
						EraseVec.push_back(iter.first);
					}
				}
				catch (...)
				{

				}
			}
			for (auto iter : EraseVec)
			{
				auto FindResult = RayCastArray.find(iter);
				if (FindResult != RayCastArray.end())
					RayCastArray.erase(FindResult);
			}
			for (auto iter : BoneArray)
				RayCastArray[iter.first] = std::make_shared<std::vector<FVector>>(iter.second);
		}
		//获取所有需要测试光线追踪的玩家和对应坐标
		inline std::vector<std::pair<ptr_t, std::shared_ptr<std::vector<FVector>>>> GetRayCastPtr()
		{
			unique_readguard<WfirstRWLock> rlock(RayCastArrayLock);
			std::vector<std::pair<ptr_t, std::shared_ptr<std::vector<FVector>>>> RetArray;
			if (RayCastArray.size())
				for (auto iter : RayCastArray)
					RetArray.push_back(std::pair(iter.first, iter.second));
			return RetArray;
		}
		//清除光线追踪玩家的坐标
		inline void ClearRayCastArray()
		{
			unique_writeguard<WfirstRWLock> wlock(RayCastArrayLock);
			if (RayCastArray.size())
				RayCastArray.clear();
		}
		//添加玩家结果到映射表
		inline void AddActorResult(ptr_t Actor)
		{
			if (!IsAddrValid(Actor))
				return;
			unique_writeguard<WfirstRWLock> wlock(RayCastResultArrayLock);
			auto FindResult = RayCastResultArray.find(Actor);
			if (FindResult == RayCastResultArray.end())
			{
				RayCastResultArray.emplace(Actor, std::make_shared<std::vector<bool>>());
				RayCastResultArray[Actor]->resize(24);
			}
			else if (FindResult->second && FindResult->second->size() != 24)
				FindResult->second->resize(24);
		}
		//单次设置光线追踪结果
		inline void SetActorVisible(ptr_t Actor, int Bone, bool Visible)
		{
			if (!IsAddrValid(Actor))
				return;
			if (Bone >= 24)
				return;
			unique_writeguard<WfirstRWLock> wlock(RayCastResultArrayLock);
			auto FindResult = RayCastResultArray.find(Actor);
			if (FindResult != RayCastResultArray.end() && FindResult->second)
				(*FindResult->second)[Bone] = Visible;
		}
		//批量设置光线追踪结果
		inline void SetActorVisibleAll(std::vector<std::pair<ptr_t, std::vector<bool>>>& result)
		{
			unique_writeguard<WfirstRWLock> wlock(RayCastResultArrayLock);
			try
			{
				for (auto iter : result)
				{
					auto FindResult = RayCastResultArray.find(iter.first);
					if (FindResult != RayCastResultArray.end() && FindResult->second)
					{
						if (iter.second.size() > 24)
							continue;
						for (int i = 0; i < FindResult->second->size(); i++)
							(*FindResult->second)[i] = iter.second[i];
					}
				}
			}
			catch (...)
			{

			}
		}
		//清除光线追踪结果数组
		inline void ClearActorVisible()
		{
			unique_writeguard<WfirstRWLock> wlock(RayCastResultArrayLock);
			if (RayCastResultArray.size())
				RayCastResultArray.clear();
		}
		//查询光线追踪结果
		inline bool GetActorVisible(ptr_t Actor, int Bone)
		{
			if (!IsAddrValid(Actor))
				return true;
			if (Bone >= 24)
				return true;
			unique_readguard<WfirstRWLock> rlock(RayCastResultArrayLock);
			auto FindResult = RayCastResultArray.find(Actor);
			if (FindResult != RayCastResultArray.end())
				return (*FindResult->second)[Bone];
			return false;
		}
		//获取全部玩家的形态刚体、坐标
		inline std::unordered_map<ptr_t, std::shared_ptr<CapSuleInfo>> GetRigidActor()
		{
			std::unordered_map<ptr_t, std::shared_ptr<CapSuleInfo>> Ret;
			unique_readguard<WfirstRWLock> rlock(RigidActorLock);
			for (auto iter : RigidActor)
				Ret.emplace(iter.first, iter.second);
			return Ret;
		}
		//主线程设置玩家的形态刚体和坐标
		inline void SetRigidActor(std::unordered_map<ptr_t, std::shared_ptr<CapSuleInfo>>& objects)
		{
			unique_writeguard<WfirstRWLock> wlock(RigidActorLock);
			if (objects.empty())
			{
				RigidActor.clear();
				return;
			}
			RigidActor = objects;
			objects.clear();
		}
		//查询玩家的形态刚体和坐标
		inline auto FindRigidActor(ptr_t Actor)
		{
			if (!IsAddrValid(Actor))
				return std::shared_ptr<CapSuleInfo>(new CapSuleInfo(0.f, 0.f, PxTransform(), 0));
			unique_readguard<WfirstRWLock> rlock(RigidActorLock);
			auto FindResult = RigidActor.find(Actor);
			if (FindResult != RigidActor.end())
				return FindResult->second;
			return std::shared_ptr<CapSuleInfo>(new CapSuleInfo(0.f, 0.f, PxTransform(), 0));
		}
		//获取EraseMap
		inline std::vector<ptr_t> GetEraseMap()
		{
			unique_readguard<WfirstRWLock> rlock(EraseFromMapArrayLock);
			std::vector<ptr_t>Ret;
			for (auto iter : EraseFromMapArray)
				Ret.push_back(iter.first);
			return Ret;
		}
		//不添加的东西的过滤
		inline void AddtoErase(ptr_t point)
		{
			unique_writeguard<WfirstRWLock> wlock(EraseFromMapArrayLock);
			EraseFromMapArray.emplace(point, std::make_shared<int>(1));
		}
		//清除EraseMap
		inline void ClearEraseMap()
		{
			unique_writeguard<WfirstRWLock> wlock(EraseFromMapArrayLock);
			EraseFromMapArray.clear();
		}
		//查询OutOfMap
		inline bool FindOutOfMap(ptr_t point)
		{
			if (!IsAddrValid(point))
				return true;
			unique_readguard<WfirstRWLock> rlock(OutOfMapLock);
			return OutOfMapWithLock.find(point) != OutOfMapWithLock.end();
		}
		inline void InsertOutOfMap(ptr_t point, std::shared_ptr<RigiBodyStruct> body)
		{
			if (!IsAddrValid(point))
				return;
			unique_writeguard<WfirstRWLock> wlock(OutOfMapLock);
			OutOfMapWithLock.emplace(point, body);
		}
		inline void EraseOutOfMap(ptr_t point)
		{
			if (!IsAddrValid(point))
				return;
			unique_writeguard<WfirstRWLock> wlock(OutOfMapLock);
			auto FindResult = OutOfMapWithLock.find(point);
			if (FindResult != OutOfMapWithLock.end())
				OutOfMapWithLock.erase(FindResult);
		}
		inline void ClearOutOfMap()
		{
			unique_writeguard<WfirstRWLock> wlock(OutOfMapLock);
			if (OutOfMapWithLock.size())
				OutOfMapWithLock.clear();
		}
		//查找场景中已存在的动态刚体
		inline PxRigidDynamic* FindDynamicRigidBody(ptr_t point) {
			if (!IsAddrValid(point))
				return nullptr;
			unique_readguard<WfirstRWLock> rlock(dynamicBodiesMapLock);
			auto it = dynamicBodiesMapWithLock.find(point);
			if (it != dynamicBodiesMapWithLock.end()) {
				return it->second;
			}
			return nullptr;
		}
		//移除场景中已存在的动态刚体
		inline void EraseDynamicRigidBody(ptr_t point) {
			if (!IsAddrValid(point))
				return;
			unique_writeguard<WfirstRWLock> wlock(dynamicBodiesMapLock);
			auto FindResult = dynamicBodiesMapWithLock.find(point);
			if (FindResult != dynamicBodiesMapWithLock.end())
				dynamicBodiesMapWithLock.erase(FindResult);
		}
		//添加动态刚体到映射表
		inline void InsertDynamicRigidBody(ptr_t Point, PxRigidDynamic* Actor)
		{
			if (!IsAddrValid(Point) || Utils::IsSafeReadPtr(Actor, 8))
				return;
			unique_writeguard<WfirstRWLock> wlock(dynamicBodiesMapLock);
			dynamicBodiesMapWithLock.emplace(Point, Actor);
		}
		//清除动态刚体映射表
		inline void ClearDynamicRigidBody()
		{
			unique_writeguard<WfirstRWLock> wlock(dynamicBodiesMapLock);
			if (dynamicBodiesMapWithLock.size())
				dynamicBodiesMapWithLock.clear();
		}
		//查找场景中已存在的静态刚体
		inline PxRigidStatic* FindStaticRigidBody(ptr_t point) {
			if (!IsAddrValid(point))
				return false;
			unique_readguard<WfirstRWLock> rlock(staticBodiesMapLock);
			auto it = staticBodiesMapWithLock.find(point);
			if (it != staticBodiesMapWithLock.end()) {
				return it->second;
			}
			return nullptr;
		}
		//移除场景中已存在的静态刚体
		inline void EraseStaticRigidBody(ptr_t point) {
			if (!IsAddrValid(point))
				return;
			unique_writeguard<WfirstRWLock> wlock(staticBodiesMapLock);
			auto FindResult = staticBodiesMapWithLock.find(point);
			if (FindResult != staticBodiesMapWithLock.end())
				staticBodiesMapWithLock.erase(FindResult);
		}
		//添加静态刚体到映射表
		inline void InsertStaticRigidBody(ptr_t Point, PxRigidStatic* Actor)
		{
			if (!IsAddrValid(Point) || Utils::IsSafeReadPtr(Actor, 8))
				return;
			unique_writeguard<WfirstRWLock> wlock(staticBodiesMapLock);
			staticBodiesMapWithLock.emplace(Point, Actor);
		}
		//清除静态刚体映射表
		inline void ClearStaticRigidBody()
		{
			unique_writeguard<WfirstRWLock> wlock(staticBodiesMapLock);
			if (staticBodiesMapWithLock.size())
				staticBodiesMapWithLock.clear();
		}

		//单线程线程添加的动态刚体 只有胶囊和圆形
		inline std::shared_ptr<PxRigidDynamic*> FindDynamicActorRigid(ptr_t point) {
			if (!IsAddrValid(point))
				return std::make_shared<PxRigidDynamic*>(nullptr);
			unique_readguard<WfirstRWLock> rlock(dynamicActorMapLock);
			auto it = dynamicActorMap.find(point);
			if (it != dynamicActorMap.end()) {
				return it->second;
			}
			return std::make_shared<PxRigidDynamic*>(nullptr);
		}
		//添加胶囊和圆形到映射表
		inline void InsertDynamicActorRigid(ptr_t point, std::shared_ptr<PxRigidDynamic*> Actor)
		{
			if (!IsAddrValid(point) || !Actor || Utils::IsSafeReadPtr(*Actor, 8))
				return;
			unique_writeguard<WfirstRWLock> wlock(dynamicActorMapLock);
			dynamicActorMap.emplace(point, Actor);
		}

		inline void ClearDynamicActorRigid()
		{
			unique_writeguard<WfirstRWLock> wlock(dynamicActorMapLock);
			if (dynamicActorMap.size())
				dynamicActorMap.clear();
		}

		inline auto GetAlreadyLoadedTriangle(hash_t Name)
		{
			unique_readguard<WfirstRWLock> rlock(AlreadyLoadedTriangleLock);
			auto FindResult = AlreadyLoadedTriangle.find(Name);
			if (FindResult != AlreadyLoadedTriangle.end())
				return FindResult->second;
			return std::make_shared<std::pair<PxTriangleMesh*, void*>>(std::make_pair(nullptr, nullptr));
		}

		inline void InsertAlreadyLoadedTriangle(hash_t Name, std::pair<PxTriangleMesh*, void*> Triangle)
		{
			unique_writeguard<WfirstRWLock> wlock(AlreadyLoadedTriangleLock);
			AlreadyLoadedTriangle.emplace(Name, std::make_shared<std::pair<PxTriangleMesh*, void*>>(Triangle));
		}

		inline void ClearAlreadyLoadedTriangle()
		{
			unique_writeguard<WfirstRWLock> wlock(AlreadyLoadedTriangleLock);
			if (AlreadyLoadedTriangle.size())
			{
				for (auto iter : AlreadyLoadedTriangle)
				{
					if (!iter.second)
						continue;
					if (!IsRigidValid((ptr_t)iter.second->first) || IsBadReadPtr((void*)iter.second->second, 8))
						continue;
					if (IsRigidValid((ptr_t)iter.second->first))
					{
						ptr_t VTABLEADDR = *(ptr_t*)iter.second->first;
						if (IsBadReadPtr((void*)VTABLEADDR, 8))
						{
							free(iter.second->second);
							continue;
						}
						MEMORY_BASIC_INFORMATION mbi;
						if (!VirtualQuery((LPVOID)VTABLEADDR, &mbi, sizeof(mbi)))
						{
							free(iter.second->second);
							continue;
						}
						if (mbi.Protect != PAGE_READONLY)
						{
							free(iter.second->second);
							continue;
						}
						VTABLEADDR = *(ptr_t*)VTABLEADDR;
						if (IsBadReadPtr((void*)VTABLEADDR, 8))
						{
							free(iter.second->second);
							continue;
						}
						if (!VirtualQuery((LPVOID)VTABLEADDR, &mbi, sizeof(mbi)))
						{
							free(iter.second->second);
							continue;
						}
						if (mbi.Protect != PAGE_EXECUTE_READ)
						{
							free(iter.second->second);
							continue;
						}
						if (VTABLEADDR == PxCommonBase + 0xB1770)
						{
							iter.second->first->release();
							if (!Utils::IsSafeReadPtr(iter.second->second, 8))
								free(iter.second->second);
						}

					}
				}
				AlreadyLoadedTriangle.clear();
			}
		}

		inline auto GetAlreadyLoadedHeightField(hash_t Name)
		{
			unique_readguard<WfirstRWLock> rlock(AlreadyLoadedHeightFieldLock);
			auto FindResult = AlreadyLoadedHeightField.find(Name);
			if (FindResult != AlreadyLoadedHeightField.end())
				return FindResult->second;
			return std::make_shared<std::pair<PxHeightField*, void*>>(std::make_pair(nullptr, nullptr));
		}

		inline void InsertAlreadyLoadedHeightField(hash_t Name, std::pair<PxHeightField*, void*> HeightField)
		{
			unique_writeguard<WfirstRWLock> wlock(AlreadyLoadedHeightFieldLock);
			AlreadyLoadedHeightField.emplace(Name, std::make_shared<std::pair<PxHeightField*, void*>>(HeightField));
		}

		inline void ClearAlreadyLoadedHeightField()
		{
			unique_writeguard<WfirstRWLock> wlock(AlreadyLoadedHeightFieldLock);
			if (AlreadyLoadedHeightField.size())
			{
				for (auto iter : AlreadyLoadedHeightField)
				{
					if (!IsRigidValid((ptr_t)iter.second->first) || IsBadReadPtr((void*)iter.second->second, 8))
						continue;
					if (IsRigidValid((ptr_t)iter.second->first))
					{
						ptr_t VTABLEADDR = *(ptr_t*)iter.second->first;
						if (IsBadReadPtr((void*)VTABLEADDR, 8))
						{
							free(iter.second->second);
							continue;
						}
						MEMORY_BASIC_INFORMATION mbi;
						if (!VirtualQuery((LPVOID)VTABLEADDR, &mbi, sizeof(mbi)))
						{
							free(iter.second->second);
							continue;
						}
						if (mbi.Protect != PAGE_READONLY)
						{
							free(iter.second->second);
							continue;
						}
						VTABLEADDR = *(ptr_t*)VTABLEADDR;
						if (IsBadReadPtr((void*)VTABLEADDR, 8))
						{
							free(iter.second->second);
							continue;
						}
						if (!VirtualQuery((LPVOID)VTABLEADDR, &mbi, sizeof(mbi)))
						{
							free(iter.second->second);
							continue;
						}
						if (mbi.Protect != PAGE_EXECUTE_READ)
						{
							free(iter.second->second);
							continue;
						}
						if (VTABLEADDR == PxCommonBase + 0xB3190)
						{
							iter.second->first->release();
							if (!Utils::IsSafeReadPtr(iter.second->second, 8))
								free(iter.second->second);
						}
					}
				}
				AlreadyLoadedHeightField.clear();
			}
		}

		inline ptr_t FindCurrentSceneActor(ptr_t point) {
			if (!IsAddrValid(point))
				return 0;
			unique_readguard<WfirstRWLock> rlock(CurrentSceneActorLock);
			auto it = CurrentSceneActorWithLock.find(point);
			if (it != CurrentSceneActorWithLock.end()) {
				return it->first;
			}

			return 0;
		}
		inline void InsertCurrentSceneActor(ptr_t point) {
			if (!IsAddrValid(point))
				return;
			unique_writeguard<WfirstRWLock> wlock(CurrentSceneActorLock);
			CurrentSceneActorWithLock.emplace(point, std::make_shared<int>(1));
		}
		inline void ClearCurrentSceneActor() {
			unique_writeguard<WfirstRWLock> wlock(CurrentSceneActorLock);
			CurrentSceneActorWithLock.clear();
		}

		inline void InsertAllcateMemory(void* Memory)
		{
			unique_writeguard<WfirstRWLock> wlock(CurrentAllocateMemoryLock);
			CurrentAllocateMemory.emplace(Memory, std::make_shared<int>(1));
		}

		inline bool IsInAllocateMemory(void* Memory)
		{
			unique_readguard<WfirstRWLock> rlock(CurrentAllocateMemoryLock);
			return CurrentAllocateMemory.find(Memory) != CurrentAllocateMemory.end();
		}

		inline bool EraseAllocateMemory(void* Memory)
		{
			unique_writeguard<WfirstRWLock> wlock(CurrentAllocateMemoryLock);
			auto FindResult = CurrentAllocateMemory.find(Memory);
			if (FindResult != CurrentAllocateMemory.end())
			{
				if (!Utils::IsSafeReadPtr(FindResult->first, 8))
					delete FindResult->first;
				CurrentAllocateMemory.erase(FindResult);
				return true;
			}
			return false;
		}

		inline void ClearAllocateMemrory()
		{
			unique_writeguard<WfirstRWLock> wlock(CurrentAllocateMemoryLock);
			for (auto iter : CurrentAllocateMemory)
			{
				if (!Utils::IsSafeReadPtr(iter.first, 8))
					delete iter.first;
			}
			CurrentAllocateMemory.clear();
		}

		inline auto GetAllocateMemory()
		{
			unique_readguard<WfirstRWLock> rlock(CurrentAllocateMemoryLock);
			return CurrentAllocateMemory;
		}

		inline void InsertDeleteMemory(void* Memory)
		{
			unique_writeguard<WfirstRWLock> wlock(CurrentDeleteMemoryLock);
			CurrentDeleteMemory.emplace(Memory, std::make_shared<int>(1));
		}

		inline bool IsInDeleteMemory(void* Memory)
		{
			unique_readguard<WfirstRWLock> rlock(CurrentDeleteMemoryLock);
			return CurrentDeleteMemory.find(Memory) != CurrentDeleteMemory.end();
		}

		inline void EraseDeleteMemory(void* Memory)
		{
			unique_writeguard<WfirstRWLock> wlock(CurrentDeleteMemoryLock);
			auto FindResult = CurrentDeleteMemory.find(Memory);
			if (FindResult != CurrentDeleteMemory.end())
				CurrentDeleteMemory.erase(FindResult);
			return;
		}

		inline void ClearDeleteMemrory()
		{
			unique_writeguard<WfirstRWLock> wlock(CurrentDeleteMemoryLock);
			CurrentDeleteMemory.clear();
		}

		inline auto GetDeleteMemory()
		{
			unique_readguard<WfirstRWLock> rlock(CurrentDeleteMemoryLock);
			return CurrentDeleteMemory;
		}

		bool IsSceneCreate();
		inline auto GetPhysx() { return this; };
	private:
		// Global PhysX objects
		PxDefaultAllocator gAllocator;
		PxDefaultErrorCallback gErrorCallback;
		PxFoundation* gFoundation;
		PxPhysics* gPhysics;
		PxCooking* gCooking;
		PxDefaultCpuDispatcher* gDispatcher;
		WfirstRWLock gSceneLock;
		physx::PxScene* gScene;
		PxMaterial* gMaterial;
		PxPvd* gPvd;
		//场景是否创建
		bool bSceneCreate;
		//Px64模块头地址和尾地址
		ptr_t Px64Base = 0;
		ptr_t Px64End = 0;
		ptr_t PxCommonBase = 0;
		ptr_t PxCommonEnd = 0;
		ptr_t PxCookingBase = 0;
		ptr_t PxCookingEnd = 0;
		//绘制线程传递RigidActor的形态坐标存储的数组
		WfirstRWLock RigidActorLock;
		std::unordered_map<ptr_t, std::shared_ptr<CapSuleInfo>>RigidActor;
		//多线程使用锁
		WfirstRWLock dynamicActorMapLock;
		std::unordered_map<ptr_t, std::shared_ptr<PxRigidDynamic*>> dynamicActorMap;
		//多线程使用锁
		WfirstRWLock dynamicBodiesMapLock;
		std::unordered_map<ptr_t, PxRigidDynamic*> dynamicBodiesMapWithLock;
		//多线程使用锁
		WfirstRWLock staticBodiesMapLock;
		std::unordered_map<ptr_t, PxRigidStatic*> staticBodiesMapWithLock;
		//多线程使用锁
		WfirstRWLock OutOfMapLock;
		std::unordered_map<ptr_t, std::shared_ptr<RigiBodyStruct>> OutOfMapWithLock;
		//多线程使用锁
		WfirstRWLock AlreadyLoadedTriangleLock;
		std::unordered_map<hash_t, std::shared_ptr<std::pair<PxTriangleMesh*, void*>>>AlreadyLoadedTriangle;
		//多线程使用锁
		WfirstRWLock AlreadyLoadedHeightFieldLock;
		std::unordered_map<hash_t, std::shared_ptr<std::pair<PxHeightField*, void*>>>AlreadyLoadedHeightField;
		//多线程使用需要使用锁
		WfirstRWLock EraseFromMapArrayLock;
		std::unordered_map<ptr_t, std::shared_ptr<int>> EraseFromMapArray;
		//多线程使用需要使用锁
		WfirstRWLock RayCastArrayLock;
		std::unordered_map<ptr_t, std::shared_ptr<std::vector<FVector>>> RayCastArray;
		//多线程使用需要使用锁
		WfirstRWLock RayCastResultArrayLock;
		std::unordered_map<ptr_t, std::shared_ptr<std::vector<bool>>> RayCastResultArray;
		//将每次循环更新的指针存入数组并在下一轮开启的时候与总数组进行比较、移除
		WfirstRWLock CurrentSceneActorLock;
		std::unordered_map<ptr_t, std::shared_ptr<int>> CurrentSceneActorWithLock;
		//所有申请的内存保存在这个数组
		WfirstRWLock CurrentAllocateMemoryLock;
		std::unordered_map<void*, std::shared_ptr<int>> CurrentAllocateMemory;
		//将要销毁的内存保存在这个数组
		WfirstRWLock CurrentDeleteMemoryLock;
		std::unordered_map<void*, std::shared_ptr<int>> CurrentDeleteMemory;

	};
}


#endif // PHYSICS_MANAGER_H