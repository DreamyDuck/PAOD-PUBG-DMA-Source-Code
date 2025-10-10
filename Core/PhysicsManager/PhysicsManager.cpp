#include "PhysicsManager.h"
#include <future>
#include <execution>
#include <extensions/PxSerialization.h>
#include <extensions/PxDefaultStreams.h>
#include "../config.h"
namespace IronMan::Core
{
	//全局场景变量
	extern PhysicsManager* g_physic;
	//功能变量
	extern Variables Vars;
	ptr_t TestPtr = 0;
	std::string Path = "C:\\PaodCache\\";
	//World的SCENE
	ptr_t SyncScene = 0;
	//每次循环记录的Scene
	ptr_t SyncRigidArray = 0;
	//创建的处理线程同步异步的线程句柄
	std::thread asyncUpdateThread;
	std::thread syncUpdateThread;
	//当前地图名称
	std::string CurrentMapName;
	//存储Scene Rigid的数组
	std::unordered_map<ptr_t, std::shared_ptr<RigiBodyStruct>> RIGID_ARRAY;
	//存储Scene HeightField的数组
	std::unordered_map<ptr_t, std::shared_ptr<RigiBodyStruct>> HeightFieldUpdateArray;
	//备份的 SceneHeightField数组进行循环检查
	std::unordered_map<ptr_t, std::shared_ptr<RigiBodyStruct>> BackUpHeightFieldUpdateArray;
	//已经更新过的数组存储
	std::vector<ptr_t> ReadedHeightFieldUpdateArray;
	//进入慢速缓存的Rigid数组 只更新Shapes结构！！！
	std::unordered_map<ptr_t, std::shared_ptr<RigiBodyStruct>> LowSpeedCache;
	//每次更新heightField的变量
	int HeightFieldUpdateRigid = 32;
	//int HeightFieldUpdateRigid = 1000;
	int HeightFieldLoopCount = 0;
	int HeightFieldSubCount = 0;
	int HeightFieldCurrentLoopCount = 0;

	bool FirstLoadAllDataSet = false;
	//相机视角变量
	FVector LocalPos = FVector();
	//更新的动态刚体的最大距离
	float MaxDynamicRigidDistance = 500.f;
	//最大更新刚体的数量
	int MaxUpdateRigid = 1000;
	int RigidLoopCount = 0;
	int RigidSubCount = 0;
	int CurrentLoopCount = 0;
	int RenderCount = 0;
	//更新10次之后启动线程进行动态刚体位置更新
	int RunTimeStart = 0;

	int CurrentReadTime = 0;

	std::vector<int> SceneActorType;
	std::vector<TArray<ptr_t>> ShapeManager;
	std::vector<std::vector<ptr_t>> ShapeArray;
	// 全局的
	std::vector<ptr_t> StaticActorStreamPtr;
	std::vector<ptr_t> ActorSceneTransformPtr;
	std::vector<PxTransform> ActorSceneTransform;
	// 静态的
	std::vector<int> StaticActorControlState;
	// 动态的
	std::vector<ptr_t> DynamicBufferedBody2WorldPtr;
	std::vector<PxTransform> DynamicBufferedBody2World;
	std::vector<DWORD> DynamicBodyBufferFlags;
	// Shape 形态
	std::vector<std::vector<BYTE>> g_ShapeControlState;;
	std::vector<std::vector<ptr_t>> g_ShapeGeometry;
	std::vector<std::vector<ptr_t>> g_ShapeTransformPtr;
	std::vector<std::vector<PxTransform>> g_ShapeTransform;
	std::vector<std::vector<ptr_t>> g_ShapeGeometryHolderPtr;
	std::vector<std::vector<PxGeometryHolder>> g_ShapeGeometryHolder;
	std::vector<std::vector<PxShapeFlags>> g_ShapeFlags;
	std::vector<std::vector<ptr_t>> g_SimulationFilterDataPtr;
	std::vector<std::vector<PxFilterData>> g_SimulationFilterData;
	// TRIANGLEMESH
	std::vector<std::vector<GuTriangleMeshDataStruct>> g_GuTriangleMeshData;
	std::vector<std::vector<GuTriangleMesh>> g_TriangleMesh;
	// HEIGHTFIELD
	std::vector<std::vector<GuHeightFieldDataStruct>> g_GuHeightFieldData;
	std::vector<std::vector<GuHeightField>> g_HeightField;
	bool OtherThreadIsEnd = false;
	bool SyncThreadCreated = false;
	bool bRayCastValid = false;
	bool fileExists(const std::string& filename) {
		std::ifstream file(filename);
		return file.good();
	}

	// 检查是否有效的通用函数
	bool IsValidPtr(void* ptr, size_t size = sizeof(void*)) {
		return ptr != nullptr && !Utils::IsSafeReadPtr(ptr, size);
	}

	const float MIN_VALID_COORD = -1000000.0f;
	const float MAX_VALID_COORD = 1000000.0f;

	bool isValidVertex(const FVector vertex) {
		return
			std::isfinite(vertex.X) && !std::isnan(vertex.X) &&
			std::isfinite(vertex.Y) && !std::isnan(vertex.Y) &&
			std::isfinite(vertex.Z) && !std::isnan(vertex.Z) &&
			vertex.X >= MIN_VALID_COORD && vertex.X <= MAX_VALID_COORD &&
			vertex.Y >= MIN_VALID_COORD && vertex.Y <= MAX_VALID_COORD &&
			vertex.Z >= MIN_VALID_COORD && vertex.Z <= MAX_VALID_COORD;
	}

	bool isValidFloat(const float ft)
	{
		return
			std::isfinite(ft) && !std::isnan(ft);
	}

	bool hasMoreThanTenPercentZeroHeights(PxHeightField* heightField) {
		if (!heightField) {
			return false;
		}

		int sampleCount = heightField->getNbColumns() * heightField->getNbRows();
		if (sampleCount == 0) {
			return false;
		}

		int zeroHeightCount = 0;

		PxHeightFieldSample* sample = (PxHeightFieldSample*)(*(ptr_t*)((ptr_t)heightField + 0x50));
		for (int i = 0; i < sampleCount; ++i) {
			PxU16 height = sample[i].height;
			if (height == 0.0f) {
				++zeroHeightCount;
			}
			else
			{
				zeroHeightCount = 0;
			}
		}

		float zeroHeightPercentage = static_cast<float>(zeroHeightCount) / (float)sampleCount;
		return zeroHeightPercentage > 0.25f;
	}

	// 自定义过滤器回调函数
	PxFilterFlags CustomSimulationFilterShader(
		PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{
		return PxFilterFlag::eDEFAULT;
	}

	struct MyFilter :public PxQueryFilterCallback
	{
		virtual PxQueryHitType::Enum preFilter(
			const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
		{
			(void)shape; (void)actor; (void)queryFlags; (void)filterData;
			if (Utils::IsSafeReadPtr((void*)shape, 8))
			{
				return PxQueryHitType::eNONE;
			}

			PxU32 stWord3 = shape->getSimulationFilterData().word3;
			PxFilterData queryFiter = shape->getQueryFilterData();
			auto Holder = shape->getGeometry();
			if (Holder.getType() == PxGeometryType::eTRIANGLEMESH)
			{
				auto NbVertices = Holder.triangleMesh().triangleMesh->getNbVertices();

				queryFlags |= PxHitFlag::eMESH_BOTH_SIDES;
				if (queryFlags & PxHitFlag::eMESH_ANY && (stWord3 == 50331650 || stWord3 == 50331651))
				{
					return PxQueryHitType::eNONE;
				}
				if (stWord3 == 327155713 || stWord3 == 327155714)
				{
					return PxQueryHitType::eNONE;
				}
				else if (stWord3 == 2)
				{
					if (NbVertices == 428 || NbVertices == 449 || NbVertices == 443 || NbVertices == 666 || NbVertices == 651 || NbVertices == 664
						|| NbVertices == 604 || NbVertices == 244 || NbVertices == 642 || NbVertices == 534 || NbVertices == 541 || NbVertices == 532
						|| NbVertices == 709 || NbVertices == 136 || NbVertices == 368 || NbVertices == 388 || NbVertices == 364)
						return PxQueryHitType::eNONE;
				}
				else if (stWord3 == 18)
				{
					if (NbVertices == 1259 || NbVertices == 1761 || NbVertices == 1999 || NbVertices == 1345 || NbVertices == 1070 || NbVertices == 618
						|| NbVertices == 914 || NbVertices == 1468 || NbVertices == 1253 || NbVertices == 709 || NbVertices == 136 || NbVertices == 428)
						return PxQueryHitType::eNONE;
				}
				else if (stWord3 == 150994946)
				{
					if (hash_(CurrentMapName) == "DihorOtok_Main"_hash)
					{
						if (NbVertices == 531)
							return PxQueryHitType::eTOUCH;
					}
				}
			}
			else
			{
				//if (queryFlags & PxHitFlag::eUV)
				//{
				//	Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, ImVec2(300, 100 + RenderCount * 14.f), 0xFFFFFFFF, 0xff000000, 0, "hit:%d", stWord3);
				//	RenderCount++;
				//}
			}
			// 否则返回 eTOUCH，继续进行 raycast
			return PxQueryHitType::eBLOCK;
		}

		virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
		{
			(void)filterData; (void)hit;
			return PxQueryHitType::eNONE;
		}
	};

	bool PhysicsManager::IsRigidValid(ptr_t Rigid)
	{
		if (!Utils::IsSafeReadPtr((void*)Rigid, 1))
		{
			auto VTable = *(ptr_t*)Rigid;
			return ((VTable > Px64Base && VTable < Px64End) || (VTable > PxCommonBase && VTable < PxCommonEnd) || (VTable > PxCookingBase && VTable < PxCookingEnd)) && !Utils::IsSafeReadPtr((void*)VTable, 1);
		}
		return false;
	}

	bool PhysicsManager::validateIndices(const PxTriangleMeshDesc& meshDesc, const GuTriangleMesh& TriangleMesh) {
		PxU32 nbVertices = TriangleMesh.mNbVertices;
		if (meshDesc.flags & PxMeshFlag::e16_BIT_INDICES) {
			const PxU16* indices = static_cast<const PxU16*>(meshDesc.triangles.data);
			for (PxU32 i = 0; i < meshDesc.triangles.count * 3; ++i) {
				if (indices[i] >= nbVertices) {
					return false; // Invalid index found
				}
			}
		}
		else {
			const PxU32* indices = static_cast<const PxU32*>(meshDesc.triangles.data);
			for (PxU32 i = 0; i < meshDesc.triangles.count * 3; ++i) {
				if (indices[i] >= nbVertices) {
					return false; // Invalid index found
				}
			}
		}
		return true;
	}

	bool PhysicsManager::IsSceneCreate()
	{
		return bSceneCreate && RunTimeStart >= 10;
	};

	void PhysicsManager::RemoveActorFromScene(PxRigidActor* Actor)
	{
		if (!IsRigidValid((ptr_t)Actor))
			return;
		auto StaticActor = Actor->is<PxRigidStatic>();
		if (IsRigidValid((ptr_t)StaticActor)) {
			auto numShapes = StaticActor->getNbShapes();
			std::vector<PxShape*> Shapes(numShapes);
			StaticActor->getShapes(Shapes.data(), Shapes.size());
			for (auto& shape : Shapes) {
				if (IsRigidValid((ptr_t)shape)) {
					if (IsValidPtr(shape->userData))
					{
						InsertDeleteMemory(shape->userData);
						shape->userData = nullptr;
					}
					shape->release();
				}
			}
			if (IsValidPtr(StaticActor->userData)) {
				InsertDeleteMemory(StaticActor->userData);
				StaticActor->userData = nullptr;
				gScene->removeActor(*StaticActor);
				StaticActor->release();
			}
		}
		else {
			auto DynamicActor = Actor->is<PxRigidDynamic>();
			if (IsRigidValid((ptr_t)DynamicActor)) {
				auto numShapes = DynamicActor->getNbShapes();
				if (numShapes)
				{
					std::vector<PxShape*> Shapes(numShapes);
					DynamicActor->getShapes(Shapes.data(), Shapes.size());
					for (auto& shape : Shapes) {
						if (IsRigidValid((ptr_t)shape)) {
							if (IsValidPtr(shape->userData))
							{
								InsertDeleteMemory(shape->userData);
								shape->userData = nullptr;
							}
							shape->release();
						}
					}
				}
				if (IsValidPtr(DynamicActor->userData)) {
					InsertDeleteMemory(DynamicActor->userData);
					DynamicActor->userData = nullptr;
					gScene->removeActor(*DynamicActor);
					DynamicActor->release();
				}
			}
			else
			{
				Sleep(0);
			}
		}
	}

	bool PhysicsManager::DestroyAllRigidBodies() {
		bRayCastValid = false;
		bSceneCreate = false;
		do
		{
			Sleep(100);
		} while (!OtherThreadIsEnd);
		unique_writeguard<WfirstRWLock> wlock(gSceneLock);
		if (!IsRigidValid((ptr_t)gScene))
			return false;
		gScene->lockWrite();
		try
		{
			PxU32 actorCount = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
			std::vector<PxActor*> actors(actorCount);
			gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, actors.data(), actorCount);
			for (PxActor* actor : actors) {
				if (!Utils::IsSafeReadPtr(actor, 1)/* && IsRigidValid((ptr_t)actor)*/) {
					if (gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, &actor, 1) == 1) {
						auto RigidActor = actor->is<PxRigidActor>();
						if (RigidActor)
						{
							auto numShapes = RigidActor->getNbShapes();
							std::vector<PxShape*>Shapes(numShapes);
							RigidActor->getShapes(Shapes.data(), Shapes.size());
							for (auto& shape : Shapes)
							{
								if (IsRigidValid((ptr_t)shape))
								{
									auto UserData = shape->userData;
									if (!Utils::IsSafeReadPtr(UserData, 8))
									{
										if (!EraseAllocateMemory(UserData))
											delete UserData;
										shape->userData = nullptr;
									}
									shape->release();
								}
							}
							if (IsRigidValid((ptr_t)RigidActor))
							{
								auto Userdata = RigidActor->userData;
								if (!Utils::IsSafeReadPtr(Userdata, 8))
								{
									if (!EraseAllocateMemory(Userdata))
										delete Userdata;
									RigidActor->userData = nullptr;
									gScene->removeActor(*actor);
									actor->release();
								}
							}
							else
							{
								Sleep(0);
							}
						}
					}
				}
			}
			ClearAllocateMemrory();
			ClearDeleteMemrory();
			actorCount = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
			actors.resize(actorCount);
			gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, actors.data(), actorCount);
			Sleep(0);
		}
		catch (...)
		{
			CONSOLE_INFO("DestoryRigid Cause Unkonw Exception!");
		}

		gScene->unlockWrite();
		CurrentReadTime = 0;
		SyncThreadCreated = false;
		RunTimeStart = 0;
		FirstLoadAllDataSet = false;
		RigidLoopCount = 0;
		RigidSubCount = 0;
		HeightFieldLoopCount = 0;
		HeightFieldSubCount = 0;
		CurrentLoopCount = 0;
		SyncScene = 0;
		SyncRigidArray = 0;
		CurrentMapName = "None";
		FirstLoadAllDataSet = false;
		LocalPos = FVector();
		if (RIGID_ARRAY.size())
			RIGID_ARRAY.clear();
		if (HeightFieldUpdateArray.size())
			HeightFieldUpdateArray.clear();
		if (BackUpHeightFieldUpdateArray.size())
			BackUpHeightFieldUpdateArray.clear();
		if (ReadedHeightFieldUpdateArray.size())
			ReadedHeightFieldUpdateArray.clear();
		if (LowSpeedCache.size())
			LowSpeedCache.clear();
		try
		{
			ClearStaticRigidBody();
			ClearDynamicRigidBody();
			ClearAlreadyLoadedTriangle();
			ClearAlreadyLoadedHeightField();
			ClearOutOfMap();
			ClearDynamicActorRigid();
			ClearRayCastArray();
			ClearActorVisible();
			SetRigidActor(std::unordered_map<ptr_t, std::shared_ptr<CapSuleInfo>>());
			ClearCurrentSceneActor();
			//gScene->release();
			//gScene = nullptr;
		}
		catch (...)
		{
			//gScene = nullptr;
			CONSOLE_INFO2("DestoryAllScene Exception!");
		}
		//if (!gScene)
		//{
		//	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
		//	sceneDesc.gravity = PxVec3(0.f, 0.f, -980.f);
		//	gDispatcher = PxDefaultCpuDispatcherCreate(2);
		//	sceneDesc.cpuDispatcher = gDispatcher;
		//	gScene = gPhysics->createScene(sceneDesc);
		//}
		return true;
	}

	void PhysicsManager::InitPhysX() {
		gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);
		if (!gFoundation) throw std::runtime_error("PxCreateFoundation failed!");

#ifdef pvdConnectPxDefaultCpuDispatcherCreate
		gPvd = PxCreatePvd(*gFoundation);
		PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#else
		gPvd = nullptr;
#endif
		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
		if (!gPhysics) throw std::runtime_error("PxCreatePhysics failed!");

		PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.f, 0.f, -980.f);
		gDispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = gDispatcher;
		//sceneDesc.filterShader = PxDefaultSimulationFilterShader;
#ifdef pvdConnect
		sceneDesc.filterShader = CustomSimulationFilterShader;
#endif
		gScene = gPhysics->createScene(sceneDesc);
		if (!gScene) throw std::runtime_error("createScene failed!");
#ifdef pvdConnect
		PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
#endif
		gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
		if (!gMaterial) throw std::runtime_error("createMaterial failed!");

		PxCookingParams cookingParams(gPhysics->getTolerancesScale());
		cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
		cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
		cookingParams.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;

		gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, cookingParams);
		if (!gCooking) throw std::runtime_error("PxCreateCooking failed!");
	}

	PhysicsManager::PhysicsManager(bool Open) {
		gFoundation = nullptr;
		gPhysics = nullptr;
		gCooking = nullptr;
		gDispatcher = nullptr;
		gScene = nullptr;
		gMaterial = nullptr;
		gPvd = nullptr;
		bSceneCreate = false;
		if (Open)
		{
			Px64Base = (ptr_t)GetModuleHandleA("PhysX3_x64.dll");
			Px64End = Px64Base + Utils::GetModuleLen((HMODULE)Px64Base);
			PxCommonBase = (ptr_t)GetModuleHandleA("PhysX3Common_x64.dll");
			PxCommonEnd = PxCommonBase + Utils::GetModuleLen((HMODULE)PxCommonBase);
			PxCookingBase = (ptr_t)GetModuleHandleA("PhysX3Cooking_x64.dll");
			PxCookingEnd = PxCommonBase + Utils::GetModuleLen((HMODULE)PxCookingBase);
			InitPhysX();
			asyncUpdateThread = std::thread(&PhysicsManager::asyncScenes, this);
			syncUpdateThread = std::thread(&PhysicsManager::syncScenes, this);
		}
	}

	PhysicsManager::~PhysicsManager() {
		if (asyncUpdateThread.joinable()) {
			asyncUpdateThread.join();
		}
		if (syncUpdateThread.joinable()) {
			syncUpdateThread.join();
		}
	}
	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 保存三角网格数据到文件
	void PhysicsManager::saveTriangleMeshToFile(PxTriangleMesh* triangleMesh, const char* filename)
	{
#ifndef SaveFileToPath
		return;
#else
		if (!IsValidPtr(triangleMesh)) return;

		// 创建输出流
		std::ofstream file(filename, std::ios::binary);
		if (!file.is_open()) return;

		// 创建PhysX的二进制流
		PxDefaultMemoryOutputStream writeBuffer;
		PxSerializationRegistry* registry = PxSerialization::createSerializationRegistry(*gPhysics);

		// 序列化三角形网格
		PxCollection* collection = PxCreateCollection();
		collection->add(*triangleMesh);
		PxSerialization::serializeCollectionToBinary(writeBuffer, *collection, *registry);

		// 将二进制流的数据写入文件
		file.write((const char*)writeBuffer.getData(), writeBuffer.getSize());

		// 清理
		collection->release();
		registry->release();

		// 关闭文件
		file.close();
#endif
	}

	// 从文件加载三角网格数据
	std::pair<PxTriangleMesh*, void*> PhysicsManager::loadTriangleMeshFromFile(const char* filename)
	{
		// 打开文件并获取文件大小
		FILE* fp = fopen(filename, "rb");
		if (!fp) return std::pair(nullptr, nullptr);

		fseek(fp, 0, SEEK_END);
		unsigned fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// 分配对齐内存，加载数据并反序列化
		void* memory = malloc(fileSize + PX_SERIAL_FILE_ALIGN);
		void* memory128 = (void*)((size_t(memory) + PX_SERIAL_FILE_ALIGN) & ~(PX_SERIAL_FILE_ALIGN - 1));
		fread(memory128, 1, fileSize, fp);
		fclose(fp);

		PxSerializationRegistry* registry = PxSerialization::createSerializationRegistry(*gPhysics);
		PxCollection* collection = PxSerialization::createCollectionFromBinary(memory128, *registry);
		if (Utils::IsSafeReadPtr(collection, 8)) {
			registry->release();
			free(memory); // 不要忘记释放分配的内存
			return std::pair(nullptr, nullptr);
		}

		PxTriangleMesh* triangleMesh = nullptr;
		for (PxU32 i = 0; i < collection->getNbObjects(); ++i) {
			PxBase& object = collection->getObject(i);
			if (object.is<PxTriangleMesh>()) {
				triangleMesh = static_cast<PxTriangleMesh*>(&object);
				triangleMesh->acquireReference();
				break;
			}
		}

		// 清理
		collection->release();
		registry->release();
		// 注意：不能立即释放memory，因为它还被PxTriangleMesh使用
		// free(memory); // 释放分配的内存

		return std::pair(triangleMesh, memory);
	}

	// 保存高度场数据到文件
	void PhysicsManager::saveHeightFieldToFile(PxHeightField* heightField, const char* filename)
	{
#ifndef SaveFileToPath
		return;
#else
		if (!IsValidPtr(heightField)) return;
		// 创建输出流
		std::ofstream file(filename, std::ios::binary);
		if (!file.is_open()) return;

		// 创建PhysX的二进制流
		PxDefaultMemoryOutputStream writeBuffer;
		PxSerializationRegistry* registry = PxSerialization::createSerializationRegistry(*gPhysics);

		// 序列化高度场
		PxCollection* collection = PxCreateCollection();
		collection->add(*heightField);
		PxSerialization::serializeCollectionToBinary(writeBuffer, *collection, *registry);

		// 将二进制流的数据写入文件
		file.write((const char*)writeBuffer.getData(), writeBuffer.getSize());

		// 清理
		collection->release();
		registry->release();

		// 关闭文件
		file.close();
#endif
	}

	// 从文件加载高度场数据
	std::pair <PxHeightField*, void*> PhysicsManager::loadHeightFieldFromFile(const char* filename)
	{
		// 打开文件并获取文件大小
		FILE* fp = fopen(filename, "rb");
		if (!fp) return std::pair(nullptr, nullptr);

		fseek(fp, 0, SEEK_END);
		unsigned fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// 分配对齐的内存，加载数据并反序列化
		void* memory = malloc(fileSize + PX_SERIAL_FILE_ALIGN);
		void* memory128 = (void*)((size_t(memory) + PX_SERIAL_FILE_ALIGN) & ~(PX_SERIAL_FILE_ALIGN - 1));
		fread(memory128, 1, fileSize, fp);
		fclose(fp);

		// 使用PhysX创建高度场
		PxSerializationRegistry* registry = PxSerialization::createSerializationRegistry(*gPhysics);
		PxCollection* collection = PxSerialization::createCollectionFromBinary(memory128, *registry);
		if (!collection) {
			free(memory);
			registry->release();
			return std::pair(nullptr, nullptr);
		}

		PxHeightField* heightField = nullptr;
		for (PxU32 i = 0; i < collection->getNbObjects(); ++i) {
			PxBase& object = collection->getObject(i);
			if (object.is<PxHeightField>()) {
				heightField = static_cast<PxHeightField*>(&object);
				heightField->acquireReference();
				break;
			}
		}

		// 清理
		collection->release();
		registry->release();
		// 注意：不能立即释放memory，因为它还被PxTriangleMesh使用
		// free(memory); // 释放分配的内存
		return std::pair(heightField, memory);
	}
	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Math
	FVector PhysicsManager::PxVec3ToFVector(const PxVec3& vec) {
		return FVector(vec.x, vec.y, vec.z);
	}

	FQuat PhysicsManager::PxQuatToFQuat(const PxQuat& Quat) {
		return FQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	}

	PxVec3 PhysicsManager::FVectorToPxVec3(const FVector& vec) {
		return PxVec3(vec.X, vec.Y, vec.Z);
	}

	PxQuat PhysicsManager::FQuatToPxQuat(const FQuat& Quat) {
		return PxQuat(Quat.X, Quat.Y, Quat.Z, Quat.W);
	}

	PxQuat PhysicsManager::PxQuat_Multy(PxQuat _this, const PxQuat q)
	{
		float x; // xmm5_4
		physx::PxQuat v4; // rax
		float w; // xmm4_4
		float y; // xmm6_4
		float v7; // xmm7_4
		float v8; // xmm8_4
		float z; // xmm9_4
		float v10; // xmm10_4
		float v11; // xmm11_4
		float v12; // xmm1_4

		x = q.x;
		w = _this.w;
		y = _this.y;
		v7 = _this.x;
		v8 = q.y;
		z = _this.z;
		v10 = q.z;
		v11 = q.w;
		v12 = z * q.x;
		v4.x = (float)((float)((float)(_this.x * v11) + (float)(q.x * w)) + (float)(y * v10)) - (float)(z * v8);
		v4.y = (float)((float)((float)(y * v11) + (float)(v8 * w)) + v12) - (float)(v10 * v7);
		v4.w = (float)((float)((float)(v11 * w) - (float)(v7 * x)) - (float)(v8 * y)) - (float)(z * v10);
		v4.z = (float)((float)((float)(z * v11) + (float)(v10 * w)) + (float)(v8 * v7)) - (float)(y * x);
		return v4;
	}

	PxTransform PhysicsManager::CalulateDynamicGlobalPose2(PxTransform p_body2Actor, PxTransform p_mBufferedBody2World)
	{
		bool awe = false;
		PxQuat v37;
		PxTransform a2;
		float v4 = p_body2Actor.q.x;
		float v6 = p_body2Actor.q.y;
		float v7 = p_body2Actor.q.x;
		float v8 = p_body2Actor.q.z;
		float v38 = p_body2Actor.q.w;
		float v9 = p_body2Actor.p.x + p_body2Actor.p.x;
		float v10 = p_body2Actor.p.y + p_body2Actor.p.y;
		float v11 = p_body2Actor.p.z + p_body2Actor.p.z;
		float v12 = -v9;
		float v13 = (float)(v38 * v38) - 0.5;
		float v14 = -v10;
		float v15 = -v11;
		float v16 = (float)((float)(v6 * v14) + (float)(v7 * v12)) + (float)(v8 * v15);
		float v39 = (float)((float)(v13 * v12) - (float)((float)((float)(v6 * v15) - (float)(v8 * v14)) * v38)) + (float)(v4 * v16);
		float v17 = (float)(v13 * v14) - (float)((float)((float)(v8 * v12) - (float)(v4 * v15)) * v38);
		float v18 = v4 * v14;
		float v19 = p_mBufferedBody2World.q.w;

		v37.x = -v4;
		float v20 = v17 + (float)(v6 * v16);
		float v21 = v6 * v12;
		float v22 = (float)(v19 * v19) - 0.5;
		float v23 = p_mBufferedBody2World.q.z;
		float v24 = v18 - v21;
		v37.y = -v6;
		float v25 = p_mBufferedBody2World.q.x;
		float v26 = v8 * v16;
		float v27 = p_mBufferedBody2World.q.y;
		float v40 = v20;
		float v28 = v20 + v20;
		v37.z = -v8;
		v37.w = 1.f;
		float v41 = (float)((float)(v13 * v15) - (float)(v24 * v38)) + v26;
		float v29 = (float)((float)(v25 * (float)(v39 + v39)) + (float)(v27 * v28)) + (float)(v23 * (float)(v41 + v41));
		float v30 = (float)(v23 * (float)(v39 + v39)) - (float)(v25 * (float)(v41 + v41));
		float v31 = (float)((float)((float)((float)((float)(v27 * (float)(v41 + v41)) - (float)(v23 * v28)) * v19)
			+ (float)(v22 * (float)(v39 + v39)))
			+ (float)(v25 * v29))
			+ p_mBufferedBody2World.p.x;
		float v32 = (float)((float)((float)((float)((float)(v25 * v28) - (float)(v27 * (float)(v39 + v39))) * v19)
			+ (float)(v22 * (float)(v41 + v41)))
			+ (float)(v23 * v29))
			+ p_mBufferedBody2World.p.z;
		float v33 = (float)((float)((float)(v30 * v19) + (float)(v22 * v28)) + (float)(v27 * v29)) + p_mBufferedBody2World.p.y;
		PxQuat v34 = PxQuat_Multy(p_mBufferedBody2World.q, v37);

		a2.q.x = v34.x;
		a2.q.y = v34.y;
		a2.q.z = v34.z;
		a2.q.w = v34.w;
		a2.p.x = v31;
		a2.p.y = v33;
		a2.p.z = v32;
		return a2;
	}

	int PhysicsManager::computeCellCoordinates(float rowLimit, float colLimit, float nbColumns, float x, float z, float* fracX, float* fracZ)
	{
		//	float v5; // xmm4_4
		//	float v6; // xmm5_4
		//	__m128 v8; // xmm7
		//	__m128 v9; // xmm6
		//	float v10; // xmm1_4
		//	int v11; // ecx
		//	float v12; // xmm2_4
		//	int v13; // ecx
		//	unsigned int _xmm = 0xFFFFFFFF; // 示例掩码值
		//	v9 = _mm_set1_ps(1.0f);
		//	v5 = fmaxf(x, 0.0);
		//	v8 = _mm_set1_ps(1.0f);
		//	v6 = fmaxf(z, 0.0);
		//	v8.m128_f32[0] = fminf(
		//		(float)(1.0 - (float)(COERCE_FLOAT(COERCE_UNSIGNED_INT(v5 + 1.0) & _xmm) * 0.000001)),
		//		v5 // 另一个要比较的浮点值
		//	);
		//	v9.m128_f32[0] = fminf(
		//		(float)(1.0f - (float)(COERCE_FLOAT(COERCE_UNSIGNED_INT(v6 + 1.0f) & _xmm) * 0.000001f)) + rowLimit,
		//		v6
		//	);
		//	v10 = v8.m128_f32[0];
		//	v11 = (int)v8.m128_f32[0];
		//	if ((int)v8.m128_f32[0] != 0x80000000 && (float)v11 != v8.m128_f32[0])
		//		v10 = (float)(v11 - (_mm_movemask_ps(_mm_unpacklo_ps(v8, v8)) & 1));
		//	v12 = v9.m128_f32[0];
		//	v13 = (int)v9.m128_f32[0];
		//	*fracX = v8.m128_f32[0] - v10;
		//	if ((int)v9.m128_f32[0] != 0x80000000 && (float)v13 != v9.m128_f32[0])
		//		v12 = (float)(v13 - (_mm_movemask_ps(_mm_unpacklo_ps(v9, v9)) & 1));
		//	*fracZ = v9.m128_f32[0] - v12;
		//	return (unsigned int)(int)(float)((float)(v10 * nbColumns) + v12);
		return 0;
	};

	float PhysicsManager::getHeightInternal2(ptr_t Samples, float NumCols, unsigned int vertexIndex, float fracX, float fracZ)
	{
		__int64 v7; // r9
		float v8; // xmm3_4
		float v9; // xmm2_4
		float v10; // xmm0_4
		float v12; // xmm0_4
		__int64 v13; // r10
		float v14; // xmm0_4
		float height; // xmm3_4
		float v16; // xmm4_4
		float v17; // xmm1_4
		int v18; // eax
		auto vertexIndexSample = GetDMA().ReadCache<PxHeightFieldSample>(Samples + vertexIndex * sizeof(PxHeightFieldSample));
		if ((vertexIndexSample.materialIndex0 & 0x80u) == 0)
		{
			v13 = vertexIndex + NumCols;
			v14 = fracX + fracZ;
			auto vertexIndexSample1 = GetDMA().ReadCache<PxHeightFieldSample>(Samples + (vertexIndex + 1) * sizeof(PxHeightFieldSample));
			auto vertexIndexSamplev13 = GetDMA().ReadCache<PxHeightFieldSample>(Samples + v13 * sizeof(PxHeightFieldSample));
			height = (float)vertexIndexSample1.height;
			v16 = (float)vertexIndexSamplev13.height;
			if (v14 >= 1.0)
			{
				auto vertexIndexSamplev131 = GetDMA().ReadCache<PxHeightFieldSample>(Samples + (v13 + 1) * sizeof(PxHeightFieldSample));
				v18 = vertexIndexSamplev131.height;
				return (float)((float)((float)(v16 - (float)v18) * (float)(1.0 - fracZ)) + (float)v18)
					+ (float)((float)(height - (float)v18) * (float)(1.0 - fracX));
			}
			else
			{
				v17 = (float)vertexIndexSample.height;
				return (float)((float)((float)(height - v17) * fracZ) + v17) + (float)((float)(v16 - v17) * fracX);
			}
		}
		else
		{
			v7 = vertexIndex + NumCols;
			v8 = (float)vertexIndexSample.height;
			auto vertexIndexSamplev71 = GetDMA().ReadCache<PxHeightFieldSample>(Samples + (v7 + 1) * sizeof(PxHeightFieldSample));
			v9 = (float)vertexIndexSamplev71.height;
			if (fracZ <= fracX)
			{
				auto vertexIndexSamplev7 = GetDMA().ReadCache<PxHeightFieldSample>(Samples + v7 * sizeof(PxHeightFieldSample));
				v12 = (float)vertexIndexSamplev7.height;
				return (float)((float)((float)(v12 - v8) * fracX) + v8) + (float)((float)(v9 - v12) * fracZ);
			}
			else
			{
				auto vertexIndexSample1 = GetDMA().ReadCache<PxHeightFieldSample>(Samples + (vertexIndex + 1) * sizeof(PxHeightFieldSample));
				v10 = (float)vertexIndexSample1.height;
				return (float)((float)((float)(v10 - v8) * fracZ) + v8) + (float)((float)(v9 - v10) * fracX);
			}
		}
	}

	float PhysicsManager::getHeightInternal2Cache(VMMDLL_SCATTER_HANDLE handle, ptr_t Samples, float NumCols, unsigned int vertexIndex, float fracX, float fracZ)
	{
		// 获取顶点索引对应的样本
		unsigned int nextRowIndex = vertexIndex + NumCols;
		PxHeightFieldSample vertexSample[2];
		GetDMA().ReadCache(Samples + vertexIndex * 4, (ptr_t)&vertexSample, sizeof(vertexSample));

		PxHeightFieldSample nextRowIndexSample[2];
		GetDMA().ReadCache(Samples + nextRowIndex * 4, (ptr_t)&nextRowIndexSample, sizeof(nextRowIndexSample));
		return 0;
	}

	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//获取当前世界场景
	void* PhysicsManager::GetPxScene(__int64 a1, uint32_t* a2, __int16 a3)
	{
		unsigned __int64 v3 = 0; // rax
		int i = 0; // eax

		if (GetDMA().Read<int>(g_PatternData.offset_PxScene + 8) != GetDMA().Read<int>(g_PatternData.offset_PxScene + 0x34))
		{
			v3 = 0i64;
			if (!GetDMA().Read<ptr_t>(g_PatternData.offset_PxScene + 0x40))
				v3 = (unsigned __int64)g_PatternData.offset_PxScene + 0x38;
			for (i = GetDMA().Read<DWORD>((GetDMA().Read<ptr_t>(g_PatternData.offset_PxScene + 0x40) | v3) + 4 * (a3 & (unsigned __int64)(GetDMA().Read<int>(g_PatternData.offset_PxScene + 0x48) - 1i64)));
				i != -1;
				i = GetDMA().Read<DWORD>(GetDMA().Read<ptr_t>(g_PatternData.offset_PxScene) + 24i64 * i + 16))
			{
				if (GetDMA().Read<WORD>(GetDMA().Read<ptr_t>(g_PatternData.offset_PxScene) + 24i64 * i) == a3)
				{
					*a2 = i;
					return a2;
				}
			}
		}
		*a2 = -1;
		return a2;
	};

	ptr_t PhysicsManager::GetPhysxSceneFromUWorld(ptr_t world)
	{
		if (!IsAddrValid(world))
			return 0;
		// 获取物理场景并更新 SyncScene
		__int64 v1 = 0;
		uint32_t v6 = 0;
		ptr_t m_PhysScene = GetDMA().Read<ptr_t>(world + g_PatternData.offset_PhysScene);
		uint32_t ecx = GetDMA().Read<uint32_t>(m_PhysScene + 0xE0);
		auto Ret = GetPxScene(ecx, &v6, ecx);
		if (v6 == -1)
			v1 = 0i64;
		else
			v1 = GetDMA().Read<ptr_t>(g_PatternData.offset_PxScene) + 24i64 * v6;
		ptr_t v2 = v1 + 8;
		ptr_t v3 = -v1;
		if ((v2 & -(__int64)(v3 != 0)) == 0)
			return 0i64;
		else
		{
			ptr_t v4 = GetDMA().Read<ptr_t>(v2 & -(__int64)(v3 != 0));
			return GetDMA().Read<ptr_t>(v4 + 0xD0);
		}
	}

	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//过滤有用的刚体
	RigidFreshType PhysicsManager::FilterDynamicRigid(PxFilterData SimData, int VerticesSize)
	{
		static bool DebugFilter = false;
		auto MapNameHash = hash_(CurrentMapName);
		if (SimData.word3 == 8388610 || SimData.word3 == 8388611)
		{
			if (MapNameHash == "Range_Main"_hash)
				return RigidFreshType::RAlways;
			else
				return RigidFreshType::RSlow;
		}
		//载具
		if (SimData.word3 == 50331650 || SimData.word3 == 50331651)
			return RigidFreshType::RAlways;
		if (SimData.word3 == 50331658 || SimData.word3 == 50331662)
			return RigidFreshType::RAlways;
		//窗户
		//if (SimData.word3 == 327155713 || SimData.word3 == 327155714)
		//	return  RigidFreshType::RNotAdd;
		if (SimData.word3 == 327155713 || SimData.word3 == 327155714)
			return  RigidFreshType::RSlow;
		//if (SimData.word3 == 18 || SimData.word3 == 17)
		//	return RigidFreshType::RNotAdd;
		if (SimData.word3 == 2)
			return  RigidFreshType::RSlow;
		if ((SimData.word3 == 150994946 || SimData.word3 == 150994950) && (VerticesSize == 2356 || VerticesSize == 1104 || VerticesSize == 2554 || VerticesSize == 4147))
			return RigidFreshType::RSlow;
		switch (MapNameHash)
		{
		case "Range_Main"_hash://训练场
		{
			switch (SimData.word3)
			{
				//半破坏 完整
			case 58720527:
			case 58720259:
			case 150994945:
			case 150994946:
			{
				switch (VerticesSize)
				{
					//训练场木门
				case 756:
				case 401:
					//训练场铁门
				case 218:
				case 262:
				case 324:
				case 360:
					return  RigidFreshType::RSlow;
				default:
					break;
				}
				if (DebugFilter)
					return RigidFreshType::RNone;
				else
					return RigidFreshType::RNotAdd;
			}
			default:
				break;
			}
			if (DebugFilter)
				return RigidFreshType::RNone;
			else
				return RigidFreshType::RNotAdd;
		}
		case "Kiki_Main"_hash://帝斯顿
		{
			switch (SimData.word3)
			{
				//半破坏 完整
			case 58720527:
			case 58720259:
			case 150994945:
			case 150994946:
			{
				switch (VerticesSize)
				{
					//帝斯顿木门
				case 741:
				case 330:
				case 756:
					//帝斯顿铁门
				case 660:
				case 604:
				case 650:
				case 817:
				case 508:
					//帝斯顿玻璃门
				case 494:
					return  RigidFreshType::RSlow;
				default:
					break;
				}
				break;
			}
			//可破坏的掩体
			case 2:
			{
				switch (VerticesSize)
				{
					//帝斯顿木墙
				case 588:
				case 408:
					//帝斯顿铁墙
				case 261:
					//帝斯顿铁丝网可穿透的
				case 718:
				case 555:
					//帝斯顿木珊栏
				case 283:
				case 302:
				case 380:
					//帝斯顿铁珊栏
				case 308:
				case 228:
				case 348:
					//信号发生器
				case 3156:
					//加油站
				case 7085:
					return  RigidFreshType::RSlow;
				default:
					break;
				}

				break;
			}

			default:
				break;
			}
			if (DebugFilter)
				return RigidFreshType::RNone;
			else
				return RigidFreshType::RNotAdd;
		}
		case "Tiger_Main"_hash://泰戈
		{
			switch (SimData.word3)
			{
				//半破坏 完整
			case 58720527:
			case 58720259:
			case 150994945:
			case 150994946:
			{
				switch (VerticesSize)
				{
					//铁门
				case 198:
				case 206:
				case 220:
				case 312:
				case 318:
				case 320:
					//木门
				case 188:
				case 596:
				case 552:
				case 492:
					return  RigidFreshType::RSlow;
				default:
					break;
				}
				if (DebugFilter)
					return RigidFreshType::RNone;
				else
					return RigidFreshType::RNotAdd;
			}
			default:
				break;
			}
			if (DebugFilter)
				return RigidFreshType::RNone;
			else
				return RigidFreshType::RNotAdd;
		}
		case "Desert_Main"_hash://米拉玛
		{
			switch (SimData.word3)
			{
				//半破坏 完整
			case 58720527:
			case 58720259:
			case 150994945:
			case 150994946:
			{
				switch (VerticesSize)
				{
				default:
					//铁门
				case 324:
				case 360:
				case 508:
					//木门
				case 329:
					return  RigidFreshType::RSlow;
				}
				break;
			}
			default:
				break;
			}
			if (DebugFilter)
				return RigidFreshType::RNone;
			else
				return RigidFreshType::RNotAdd;
		}
		case "Summerland_Main"_hash://卡拉金
		{
			switch (SimData.word3)
			{
				//半破坏 完整
			case 58720527:
			case 58720259:
			case 150994945:
			case 150994946:
			{
				switch (VerticesSize)
				{
					//铁门
				case 817:
				case 90:
				case 516:
				case 380:
					return RigidFreshType::RSlow;
				default:
					break;
				}
				break;
			}
			default:
				break;
			}
			if (DebugFilter)
				return RigidFreshType::RNone;
			else
				return RigidFreshType::RNotAdd;
		}
		case "Baltic_Main"_hash://艾伦格
		{
			switch (SimData.word3)
			{
				//半破坏 完整
			case 58720527:
			case 58720259:
			case 150994945:
			case 150994946:
			{
				switch (VerticesSize)
				{
					//木门
				case 546:
				case 596:
				case 356:
				case 936:
				case 820:
					//铁门
				case 312:
				case 252:

					return RigidFreshType::RSlow;
				default:
					break;
				}
				break;
			}
			default:
				break;
			}
			if (DebugFilter)
				return RigidFreshType::RNone;
			else
				return RigidFreshType::RNotAdd;
		}
		case "Savage_Main"_hash://萨诺
		{
			switch (SimData.word3)
			{
				//半破坏 完整
			case 58720527:
			case 58720259:
			case 150994945:
			case 150994946:
			{
				switch (VerticesSize)
				{
					//萨诺木门
				case 342:
				case 634:
				case 498:
				case 809:
				case 500:
				case 797:
					//萨诺铁门
				case 218:
				case 262:
					return  RigidFreshType::RSlow;
				default:
					break;
				}
				break;
			}
			default:
				break;
			}
			if (DebugFilter)
				return RigidFreshType::RNone;
			else
				return RigidFreshType::RNotAdd;
		}
		case "DihorOtok_Main"_hash://维寒迪
		{
			switch (SimData.word3)
			{
				//半破坏 完整
			case 58720527:
			case 58720259:
			case 150994945:
			case 150994946:
			{
				switch (VerticesSize)
				{
					//维寒迪木门
				case 341:
				case 89:
				case 204:
				case 58:
				case 400:
				case 239:
					//维寒迪铁门
				case 508:
				case 308:
				case 294:
				case 531:
				case 455:
				case 166:
				case 57:
				case 106:
					//维寒迪物资箱
				case 1472:
					return  RigidFreshType::RSlow;
				default:
					break;
				}
				break;
			}
			default:
				break;
			}
			if (DebugFilter)
				return RigidFreshType::RNone;
			else
				return RigidFreshType::RNotAdd;
		}
		case "Chimera_Main"_hash://帕拉莫
		{
			switch (SimData.word3)
			{
				//半破坏 完整
			case 58720527:
			case 58720259:
			case 150994945:
			case 150994946:
			{
				switch (VerticesSize)
				{
					//木门
				case 298:
					//密室门
				case 1011:
					return RigidFreshType::RSlow;
				default:
					break;
				}
				break;
			}
			default:
				break;
			}
			if (DebugFilter)
				return RigidFreshType::RNone;
			else
				return RigidFreshType::RNotAdd;
		}
		case "Neon_Main"_hash://荣都
		{
			switch (SimData.word3)
			{
				//半破坏 完整
			case 58720527:
			case 58720259:
			case 150994945:
			case 150994946:
			{
				switch (VerticesSize)
				{
					//荣都木门
				case 228:
				case 148:
				case 396:
					//荣都铁门
				case 280:
				case 494:
				case 244:
				case 276:
				case 650:
					//草垛
				case 2350:
					return  RigidFreshType::RSlow;
				default:
					break;
				}
				break;
			}
			default:
				break;
			}
			if (DebugFilter)
				return RigidFreshType::RNone;
			else
				return RigidFreshType::RNotAdd;
		}
		case "Heaven_Main"_hash://褐湾
		{
			switch (SimData.word3)
			{
				//半破坏 完整
			case 58720527:
			case 58720259:
			case 150994945:
			case 150994946:
			{
				//switch (VerticesSize)
				//{

				//default:
				//	break;
				//}
				break;
			}
			default:
				break;
			}
			if (DebugFilter)
				return RigidFreshType::RNone;
			else
				return RigidFreshType::RNotAdd;
		}
		default:
			break;
		}

		if (DebugFilter)
			return RigidFreshType::RNone;
		else
			return RigidFreshType::RNotAdd;
	}
	//检查对象的坐标、形态属性是否一致是否需要更新
	bool PhysicsManager::checkTransformIsSame(PxRigidActor* Actor, PxTransform Transform)
	{
		if (!IsRigidValid((ptr_t)Actor))
			return true;
		if (Actor->is<PxRigidDynamic>())
		{
			auto ShapeNum = Actor->getNbShapes();
			std::vector<PxShape*>Shapes(ShapeNum);
			Actor->getShapes(Shapes.data(), Shapes.size());
			for (auto Shape : Shapes)
			{
				if (!IsRigidValid((ptr_t)Shape))
					continue;
				int ShapeGetType = Shape->getGeometry().getType();
				if (ShapeGetType == PxGeometryType::eTRIANGLEMESH)
				{
					ShapeUserData* userdata = (ShapeUserData*)Shape->userData;
					if (Utils::IsSafeReadPtr(userdata, 8))
						continue;
					if (userdata->SimulationFilterData == PxFilterData() || userdata->LogTArray->Data != userdata->WriteTArray->Data || (WORD)userdata->LogTArray->Num() != (WORD)userdata->WriteTArray->Num())
						return false;
				}
			}
			auto ActorTransform = Actor->getGlobalPose();
			if (Transform == ActorTransform)
				return true;
			return false;
		}
		else if (Actor->is<PxRigidStatic>())
		{
			auto ShapeNum = Actor->getNbShapes();
			std::vector<PxShape*>Shapes(ShapeNum);
			Actor->getShapes(Shapes.data(), Shapes.size());
			for (auto Shape : Shapes)
			{
				if (!IsRigidValid((ptr_t)Shape))
					continue;
				int ShapeGetType = Shape->getGeometry().getType();
				if (ShapeGetType == PxGeometryType::eTRIANGLEMESH)
				{
					ShapeUserData* userdata = (ShapeUserData*)Shape->userData;
					if (Utils::IsSafeReadPtr(userdata, 8))
						continue;
					if (userdata->SimulationFilterData != Shape->getSimulationFilterData() || userdata->LogTArray->Data != userdata->WriteTArray->Data || (WORD)userdata->LogTArray->Num() != (WORD)userdata->WriteTArray->Num())
						return false;
				}
			}
			auto ActorTransform = Actor->getGlobalPose();
			if (Transform == ActorTransform)
				return true;
			return false;
		}
		return true;
	}
	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//RayTrace
	bool PhysicsManager::WeaponCanHit(FVector& origin, FVector& direction, float maxDistance, bool showDebug, bool bTrigger, bool CarFilter)
	{
		if (!bRayCastValid)
			return false;
		auto varidist = (direction - origin).Size() / 100.f;
		if (varidist > 1000.f)
			return false;
		bool status = false;
		if (!IsSceneCreate())
			return false;
		// 检查输入参数是否有效
		if (Utils::IsSafeReadPtr(&origin, sizeof(FVector)) || Utils::IsSafeReadPtr(&direction, sizeof(FVector)) || Utils::IsSafeReadPtr(&maxDistance, sizeof(float))
			|| Utils::IsSafeReadPtr(&showDebug, sizeof(bool)) || Utils::IsSafeReadPtr(&bTrigger, sizeof(bool)))
			return false;
		if (origin.IsZero() || direction.IsZero())
			return false;
		PxRigidActor* HitActor = nullptr;
		auto ViewPos = origin;
		PxRaycastBuffer hit;
		FVector ueDirection = (direction - ViewPos).GetSafeNormal();
		PxVec3 Direction = FVectorToPxVec3(ueDirection);
		ImU32 Color = 0xEE0000FF;
		FVector Target;
		PxQueryFilterData stFilterData;
		stFilterData.flags = PxQueryFlag::ePREFILTER | PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC;
		PxHitFlags HitFlag = PxHitFlag::eDEFAULT | PxHitFlag::eMESH_BOTH_SIDES;
		if (showDebug)
			HitFlag |= PxHitFlag::eUV;
		if (CarFilter)
			HitFlag |= PxHitFlag::eMESH_ANY;
		MyFilter stFilterCallback;
		int TimeStep = 0;
		std::unique_ptr<unique_readguard<WfirstRWLock>> rlock(new unique_readguard<WfirstRWLock>(gSceneLock));
		if (!IsRigidValid((ptr_t)gScene))
			return false;
		gScene->lockRead();
		try
		{
			TimeStep = 1;
			// 检查全局场景指针是否有效
			status = gScene->raycast(FVectorToPxVec3(ViewPos), Direction, maxDistance, hit, HitFlag, stFilterData, &stFilterCallback);
			TimeStep = 2;
			if (status)
			{
				HitActor = hit.block.actor;
				TimeStep = 3;
				if (!IsRigidValid((ptr_t)HitActor))
				{
					gScene->unlockRead();
					return false;
				}
				TimeStep = 4;
				PxFilterData SF = PxFilterData();
				float Newdist_Test = 0;
				float Newdist_Hit = 0;
				Target = PxVec3ToFVector(hit.block.position);
				TimeStep = 5;
				if (HitActor->is<PxRigidStatic>())
				{
					TimeStep = 6;
					Newdist_Hit = (Target - ViewPos).Size() / 100.f;
					if (isinf(Newdist_Hit))
					{
						status = true;
						Color = 0xEE0000FF;
						goto Render;
					}
					Newdist_Test = (direction - ViewPos).Size() / 100.f;
					if (Newdist_Hit > Newdist_Test)
					{
						status = false;
						Color = 0xEE00FF00;
						goto Render;
					}

					//if (showDebug)
					//	DrawSingleRigid(HitActor);
					TimeStep = 7;
					auto ShapeNum = HitActor->getNbShapes();
					if (ShapeNum)
					{
						std::vector<PxShape*> Shapes(ShapeNum);
						HitActor->getShapes(Shapes.data(), Shapes.size());
						for (auto shape : Shapes)
						{
							if (!IsRigidValid((ptr_t)shape))
								continue;
							auto Type = shape->getGeometry().getType();
							if (Type == PxGeometryType::eTRIANGLEMESH)
							{
								auto nbv = shape->getGeometry().triangleMesh().triangleMesh->getNbVertices();
								auto nbt = shape->getGeometry().triangleMesh().triangleMesh->getNbTriangles();
								auto Sim = shape->getSimulationFilterData();
								PxU16* Triangle = (PxU16*)shape->getGeometry().triangleMesh().triangleMesh->getTriangles();
								auto FirstV = Triangle[0];
								auto FirstF = shape->getGeometry().triangleMesh().triangleMesh->getVertices()[0].x;
								constexpr size_t BUFFER_SIZE = 256;
								char buffer[BUFFER_SIZE];
								ZeroMemory(buffer, BUFFER_SIZE);
								int ret = sprintf_s(buffer, BUFFER_SIZE, "%s_%u_%u_%u_%d_%f",
									CurrentMapName.c_str(),
									Sim.word3,
									nbt,
									nbv,
									FirstV,
									FirstF);
								std::string FileNameLocation = std::string(buffer);
								auto hash_file = hash_(FileNameLocation);
								std::string FileNamePath = Path + CurrentMapName + "\\" + std::to_string(hash_file) + ".dat";
								Sleep(0);
							}
						}
					}
					TimeStep = 8;
					Color = 0xEE0000FF;
				}
				else if (HitActor->is<PxRigidDynamic>())
				{
					TimeStep = 9;
					auto ShapeNum = HitActor->getNbShapes();
					if (ShapeNum)
					{
						std::vector<PxShape*> Shapes(ShapeNum);
						HitActor->getShapes(Shapes.data(), Shapes.size());
						TimeStep = 10;
						for (auto& shape : Shapes)
						{
							if (!IsRigidValid((ptr_t)shape))
								continue;
							PxGeometryHolder Holder = shape->getGeometry();
							if (Holder.getType() == PxGeometryType::eCAPSULE || Holder.getType() == PxGeometryType::eSPHERE)
							{
								SF = shape->getSimulationFilterData();
								if (SF.word0 == 1 && SF.word1 == 1 && SF.word2 == 1 && SF.word3 == 1)
								{
									Newdist_Hit = (Target - ViewPos).Size() / 100.f;
									Newdist_Test = (direction - ViewPos).Size() / 100.f;
									if (abs(Newdist_Hit - Newdist_Test) < 10.f)
									{
										status = false;
										Color = 0xEE00FF00;
										goto Render;
									}
								}
								break;
							}
						}
						TimeStep = 11;
					}
				}
			}
			else
			{
				Target = direction;
				if (bTrigger)
				{
					Color = 0xEE0000FF;
					status = true;
				}
				else
				{
					Color = 0xEE00FF00;
				}
				TimeStep = 12;
			}

		}
		catch (const std::exception& e)
		{
			CONSOLE_INFO2("Weapon Can Hit Cause Exception: %s", e.what());
		}
		catch (...)
		{
			CONSOLE_INFO2("Weapon Can Hit Cause Unknown Exception![%d]", TimeStep);
			CONSOLE_INFO2("View:%f|%f|%f", ViewPos.X, ViewPos.Y, ViewPos.Z);
			CONSOLE_INFO2("Dir:%f|%f|%f", direction.X, direction.Y, direction.Z);
		}
	Render:
		gScene->unlockRead();
		rlock.reset(); // 确保写锁在渲染之前释放
		if (showDebug)
		{
			FVector2D screen1;
			FVector2D screen2;
			if (sdk::WorldToScreen2(Target, screen2))
			{
				Drawing::DrawCircleFilled(screen2, 3.5f, Color, 2.f);
				if (sdk::WorldToScreen2(origin, screen1))
				{
#ifdef DebugTrace
					Drawing::DrawLine(OverlayEngine::GetViewPort() / 2.f, screen2, Color, 2.f);
#else
					Drawing::DrawLine(screen1, screen2, Color, 2.f);
					//CONSOLE_INFO("%.0f|%.0f", screen2.X, screen2.Y);
#endif
				}
			}
		}
		return !status;
	}
	//更新场景对象的坐标
	void PhysicsManager::setGlobalPoseInternal(PxRigidActor* RigidActor, PxTransform Transform)
	{
		if (!IsRigidValid((ptr_t)RigidActor))
			return;
		//if (checkTransformIsSame(RigidActor, Transform))
		//	return;
		RigidActor->setGlobalPose(Transform);
		PxU32 shapeCount = RigidActor->getNbShapes();
		std::vector<PxShape*> shapes(shapeCount);
		RigidActor->getShapes(shapes.data(), shapeCount);
		for (PxShape* shape : shapes)
		{
			if (!IsRigidValid((ptr_t)shape))
				continue;
			auto Type = shape->getGeometry().getType();
			if (Type == PxGeometryType::eTRIANGLEMESH)
			{
				ShapeUserData* userdata = (ShapeUserData*)shape->userData;
				if (Utils::IsSafeReadPtr(userdata, 8))
					continue;

				if ((int)Type != userdata->ShapeType)
				{
					AddtoErase(userdata->RigidPoint);
					return;
				}

				if (RigidActor->is<PxRigidStatic>())
				{
					if (userdata->SimulationFilterData != shape->getSimulationFilterData() || userdata->LogTArray->Data != userdata->WriteTArray->Data || (WORD)userdata->LogTArray->Num() != (WORD)userdata->WriteTArray->Num())
					{
						AddtoErase(userdata->RigidPoint);
						return;
					}
				}
				else if (RigidActor->is<PxRigidDynamic>())
				{
					if (userdata->SimulationFilterData == PxFilterData() || userdata->LogTArray->Data != userdata->WriteTArray->Data || (WORD)userdata->LogTArray->Num() != (WORD)userdata->WriteTArray->Num())
					{
						AddtoErase(userdata->RigidPoint);
						return;
					}
				}
				PxTransform ShapeTransform = userdata->ShapeTransform;
				PxTransform CmpTransform = shape->getLocalPose();
				if (!(ShapeTransform == CmpTransform))
					shape->setLocalPose(userdata->ShapeTransform);
				PxFilterData CmpFilterData = shape->getSimulationFilterData();
				if (userdata->SimulationFilterData != CmpFilterData)
					shape->setSimulationFilterData(userdata->SimulationFilterData);
				//PxTransform ShapeTransform = userdata->ShapeTransform;
				//PxTransform CmpTransform = PxTransform(PxVec3(0, 0, 0), PxQuat(0, 0, 0, 0));
				//if (!(ShapeTransform == CmpTransform))
				//	shape->setLocalPose(userdata->ShapeTransform);
				//shape->setSimulationFilterData(userdata->SimulationFilterData);
			}
		}
	}
	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//线程1
	//如果不需要则只加载骨骼到内存防止下次重复读取
	void PhysicsManager::InsertTriangleMap(std::shared_ptr<RigiBodyStruct> RigidBody)
	{
		for (auto& shape : RigidBody->Shapes) {
			switch (shape.ShapeGeometry.getType()) {
			case PxGeometryType::eTRIANGLEMESH: {

				if (shape.TriangleMesh.mNbVertices <= 0 || shape.TriangleMesh.triangleCount <= 0)
					break;
				auto Scale = shape.ShapeGeometry.triangleMesh().scale;
				auto flags = shape.ShapeGeometry.triangleMesh().meshFlags;
				float FirstF = shape.TriangleMesh.FirstF;
				if (!isValidFloat(FirstF))
				{
					FirstF = shape.TriangleMesh.FirstF2;
					if (!isValidFloat(FirstF))
					{
						FirstF = shape.TriangleMesh.FirstF3;
						if (!isValidFloat(FirstF))
							break;
					}
				}
				int FirstV = shape.TriangleMesh.FirstV;
				if (FirstV < 0 || FirstV >= shape.TriangleMesh.mNbVertices)
					break;
				std::string CacheTypeName = RigidBody->Type == 0 ? "_" : "DM_";

				constexpr size_t BUFFER_SIZE = 256;
				char buffer[BUFFER_SIZE];
				ZeroMemory(buffer, BUFFER_SIZE);
				// 使用sprintf_s进行字符串格式化
				int ret = sprintf_s(buffer, BUFFER_SIZE, "%s%s%u_%u_%u_%d_%f",
					CurrentMapName.c_str(),
					RigidBody->Type == 0 ? "_" : "DM_",
					shape.SimulationFilterData.word3,
					shape.TriangleMesh.triangleCount,
					shape.TriangleMesh.mNbVertices,
					FirstV,
					FirstF);
				std::string	FileNameLocation = std::string(buffer);

				auto hash_file = hash_(FileNameLocation);
				std::string FileNamePath = Path + CurrentMapName + "\\" + std::to_string(hash_file) + ".dat";
				//std::string FileNamePath = Path + CurrentMapName + "\\" + FileNameLocation + ".dat";

				auto triangleMesh = GetAlreadyLoadedTriangle(hash_file);
				if (triangleMesh && !IsValidPtr(triangleMesh->first) && !IsValidPtr(triangleMesh->second))
				{
					if (Vars.Menu.UseCacheToTrace)
					{
						if (!fileExists(FileNamePath))
						{
							PxTriangleMeshDesc meshDesc;
							bool bIsZero = false;
							for (int vi = 0; vi < shape.TriangleMesh.Vertices.size(); vi++)
							{
								if (shape.TriangleMesh.Vertices[vi].isZero())
								{
									bIsZero = false;
									break;
								}
							}
							if (bIsZero)
								break;
							meshDesc.points.count = static_cast<PxU32>(shape.TriangleMesh.Vertices.size());
							meshDesc.points.stride = sizeof(PxVec3);
							meshDesc.points.data = shape.TriangleMesh.Vertices.data();
							if (shape.TriangleMesh.mTriangleMeshFlags & 2) {
								if (shape.TriangleMesh.Indices16.size() <= 0 || (shape.TriangleMesh.Indices16[0] == 0 && shape.TriangleMesh.Indices16[1] == 0 && shape.TriangleMesh.Indices16[2] == 0))
									break;
								if (shape.TriangleMesh.Indices16[0] > meshDesc.points.count || shape.TriangleMesh.Indices16[1] > meshDesc.points.count || shape.TriangleMesh.Indices16[2] > meshDesc.points.count)
									break;
								meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices16.size() / 3);
								meshDesc.triangles.stride = 3 * sizeof(PxU16);
								meshDesc.triangles.data = shape.TriangleMesh.Indices16.data();
								meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
							}
							else {
								if (shape.TriangleMesh.Indices32.size() <= 0 || (shape.TriangleMesh.Indices32[0] == 0 && shape.TriangleMesh.Indices32[1] == 0 && shape.TriangleMesh.Indices32[2] == 0))
									break;
								if (shape.TriangleMesh.Indices32[0] > meshDesc.points.count || shape.TriangleMesh.Indices32[1] > meshDesc.points.count || shape.TriangleMesh.Indices32[2] > meshDesc.points.count)
									break;
								meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices32.size() / 3);
								meshDesc.triangles.stride = 3 * sizeof(PxU32);
								meshDesc.triangles.data = shape.TriangleMesh.Indices32.data();
							}
							if (!validateIndices(meshDesc, shape.TriangleMesh))
								break;
							PxDefaultMemoryOutputStream writeBuffer;
							PxTriangleMeshCookingResult::Enum result;
							if (!gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result)) {
								break;
							}
							PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
							triangleMesh->first = gPhysics->createTriangleMesh(readBuffer);
							saveTriangleMeshToFile(triangleMesh->first, FileNamePath.c_str());
						}
						else
						{
							bool isInValidModel = false;
							*triangleMesh = loadTriangleMeshFromFile(FileNamePath.c_str());
							if (!Utils::IsSafeReadPtr(triangleMesh->first, 8))
							{
								auto nbVertices = triangleMesh->first->getNbVertices();
								auto nbTriangles = triangleMesh->first->getNbTriangles();
								auto TrianglesPtr = triangleMesh->first->getTriangles();
								auto VerticesPtr = triangleMesh->first->getVertices();
								auto TriangleMeshFlags = triangleMesh->first->getTriangleMeshFlags();
								if (TrianglesPtr == nullptr || VerticesPtr == nullptr) {
									isInValidModel = true;
									break;
								}
								else {
									for (PxU32 i = 0; i < nbVertices; ++i) {
										if (!isValidVertex(PxVec3ToFVector(VerticesPtr[i]))) {
											isInValidModel = true;
											break;
										}
									}
									if (!isInValidModel)
									{
										for (PxU32 jj = 0; jj < nbTriangles; jj++)
										{
											if (TriangleMeshFlags & PxTriangleMeshFlag::e16_BIT_INDICES)
											{
												PxU16* mTriangles = (PxU16*)TrianglesPtr;
												PxU16 idx1 = mTriangles[jj * 3];
												PxU16 idx2 = mTriangles[jj * 3 + 1];
												PxU16 idx3 = mTriangles[jj * 3 + 2];

												if (idx1 >= nbVertices || idx2 >= nbVertices || idx3 >= nbVertices)
												{
													isInValidModel = true;
													break;
												}
											}
											else
											{
												PxU32* mTriangles = (PxU32*)TrianglesPtr;
												PxU32 idx1 = mTriangles[jj * 3];
												PxU32 idx2 = mTriangles[jj * 3 + 1];
												PxU32 idx3 = mTriangles[jj * 3 + 2];

												if (idx1 >= nbVertices || idx2 >= nbVertices || idx3 >= nbVertices)
												{
													isInValidModel = true;
													break;
												}
											}
										}
									}
								}
								if (isInValidModel)
								{
									if (std::remove(FileNamePath.c_str()) == 0) {
										CONSOLE_INFO("Remove An InValid Module Cache : %s", FileNamePath.c_str());
										break;
									}
								}
							}
							else
							{
								PxTriangleMeshDesc meshDesc;
								bool bIsZero = false;
								for (int vi = 0; vi < shape.TriangleMesh.Vertices.size(); vi++)
								{
									if (shape.TriangleMesh.Vertices[vi].isZero())
									{
										bIsZero = false;
										break;
									}
								}
								if (bIsZero)
									break;
								meshDesc.points.count = static_cast<PxU32>(shape.TriangleMesh.Vertices.size());
								meshDesc.points.stride = sizeof(PxVec3);
								meshDesc.points.data = shape.TriangleMesh.Vertices.data();
								if (shape.TriangleMesh.mTriangleMeshFlags & 2) {
									if (shape.TriangleMesh.Indices16.size() <= 0 || (shape.TriangleMesh.Indices16[0] == 0 && shape.TriangleMesh.Indices16[1] == 0 && shape.TriangleMesh.Indices16[2] == 0))
										break;
									if (shape.TriangleMesh.Indices16[0] > meshDesc.points.count || shape.TriangleMesh.Indices16[1] > meshDesc.points.count || shape.TriangleMesh.Indices16[2] > meshDesc.points.count)
										break;
									meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices16.size() / 3);
									meshDesc.triangles.stride = 3 * sizeof(PxU16);
									meshDesc.triangles.data = shape.TriangleMesh.Indices16.data();
									meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
								}
								else {
									if (shape.TriangleMesh.Indices32.size() <= 0 || (shape.TriangleMesh.Indices32[0] == 0 && shape.TriangleMesh.Indices32[1] == 0 && shape.TriangleMesh.Indices32[2] == 0))
										break;
									if (shape.TriangleMesh.Indices32[0] > meshDesc.points.count || shape.TriangleMesh.Indices32[1] > meshDesc.points.count || shape.TriangleMesh.Indices32[2] > meshDesc.points.count)
										break;
									meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices32.size() / 3);
									meshDesc.triangles.stride = 3 * sizeof(PxU32);
									meshDesc.triangles.data = shape.TriangleMesh.Indices32.data();
								}
								if (!validateIndices(meshDesc, shape.TriangleMesh))
									break;
								PxDefaultMemoryOutputStream writeBuffer;
								PxTriangleMeshCookingResult::Enum result;
								if (!gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result)) {
									break;
								}
								PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
								triangleMesh->first = gPhysics->createTriangleMesh(readBuffer);
								saveTriangleMeshToFile(triangleMesh->first, FileNamePath.c_str());
							}
						}
					}
					else
					{
						PxTriangleMeshDesc meshDesc;
						bool bIsZero = false;
						for (int vi = 0; vi < shape.TriangleMesh.Vertices.size(); vi++)
						{
							if (shape.TriangleMesh.Vertices[vi].isZero())
							{
								bIsZero = false;
								break;
							}
						}
						if (bIsZero)
							break;
						meshDesc.points.count = static_cast<PxU32>(shape.TriangleMesh.Vertices.size());
						meshDesc.points.stride = sizeof(PxVec3);
						meshDesc.points.data = shape.TriangleMesh.Vertices.data();
						if (shape.TriangleMesh.mTriangleMeshFlags & 2) {
							if (shape.TriangleMesh.Indices16.size() <= 0 || (shape.TriangleMesh.Indices16[0] == 0 && shape.TriangleMesh.Indices16[1] == 0 && shape.TriangleMesh.Indices16[2] == 0))
								break;
							if (shape.TriangleMesh.Indices16[0] > meshDesc.points.count || shape.TriangleMesh.Indices16[1] > meshDesc.points.count || shape.TriangleMesh.Indices16[2] > meshDesc.points.count)
								break;
							meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices16.size() / 3);
							meshDesc.triangles.stride = 3 * sizeof(PxU16);
							meshDesc.triangles.data = shape.TriangleMesh.Indices16.data();
							meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
						}
						else {
							if (shape.TriangleMesh.Indices32.size() <= 0 || (shape.TriangleMesh.Indices32[0] == 0 && shape.TriangleMesh.Indices32[1] == 0 && shape.TriangleMesh.Indices32[2] == 0))
								break;
							if (shape.TriangleMesh.Indices32[0] > meshDesc.points.count || shape.TriangleMesh.Indices32[1] > meshDesc.points.count || shape.TriangleMesh.Indices32[2] > meshDesc.points.count)
								break;
							meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices32.size() / 3);
							meshDesc.triangles.stride = 3 * sizeof(PxU32);
							meshDesc.triangles.data = shape.TriangleMesh.Indices32.data();
						}
						if (!validateIndices(meshDesc, shape.TriangleMesh))
							break;
						PxDefaultMemoryOutputStream writeBuffer;
						PxTriangleMeshCookingResult::Enum result;
						if (!gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result)) {
							break;
						}
						PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
						triangleMesh->first = gPhysics->createTriangleMesh(readBuffer);
					}
					if (!Utils::IsSafeReadPtr(triangleMesh->first, 8) && (ptr_t)triangleMesh->second <= (ptr_t)triangleMesh->first)
					{
						if (IsValidPtr((void*)triangleMesh->first->getTriangles(), 8) && IsValidPtr((void*)triangleMesh->first->getVertices(), 0xC))
							InsertAlreadyLoadedTriangle(hash_file, std::make_pair(triangleMesh->first, triangleMesh->second));
						else
						{
							free(triangleMesh->second);
							break;
						}
					}
				}
				break;
			}
			default:
				break;
			}
		}
		return;
	}
	//更新相机坐标、地图名称、创建目录
	bool PhysicsManager::UpdateInfoAndCheckPath()
	{
		//每次循环将需要移除的刚体的指针传入过滤器
		//每次运行时将当前相机坐标赋值
		LocalPos = sdk::GetLocalPos();
		if (LocalPos.IsZero())
			return false;
		//每次运行时将当前地图名字
		CurrentMapName = sdk::GetWorldName();
		if (CurrentMapName.empty())
			return false;
		switch (hash_(CurrentMapName))
		{
		case "Tiger_Main"_hash:
		case "Desert_Main"_hash:
		case "Kiki_Main"_hash:
		case "Range_Main"_hash:
		case "Summerland_Main"_hash:
		case "Baltic_Main"_hash:
		case "Savage_Main"_hash:
		case "DihorOtok_Main"_hash:
		case "Chimera_Main"_hash:
		case "Neon_Main"_hash:
		case "Heaven_Main"_hash:
		case "Italy_Main"_hash:
		case "Italy_TDM_Main"_hash:
		case "Boardwalk_Main"_hash:
		case "PillarCompound_Main"_hash:
			break;
		default:
			return false;
		}
		if (!CurrentMapName.empty())
		{
			//检查设置路径目录是否存在，不存在则创建
			Utils::hasNullTerminator(Vars.Aimbot.txt_CacheSave, 32);
			std::string SetPath = std::string(Vars.Aimbot.txt_CacheSave);
			if (Utils::checkAndCreateDirectory(SetPath))
			{
				if (Utils::checkAndCreateDirectory(SetPath + CurrentMapName))
				{
					Path = SetPath + "\\";
				}
			}
		}
		return true;
	}
	//移除掉非法场景对象
	void PhysicsManager::ClearInValidRigid(std::unordered_map<ptr_t, std::shared_ptr<RigiBodyStruct>>& RIGIDARRAY, std::unordered_map<ptr_t, std::shared_ptr<int>>& EraseVector)
	{
		std::unordered_map<ptr_t, int> bodiesToRemove;
		//检查是否有SimulationFilterData全为0的刚体，并将其加入到删除队列
		for (auto& rigidBody : RIGIDARRAY) {
			if (!IsAddrValid(rigidBody.first) || !rigidBody.second)
				continue;
			bool shouldRemove = false;
			for (auto& shape : rigidBody.second->Shapes) {
				PxFilterData filterData = shape.SimulationFilterData;
				if (rigidBody.second->Type == 1)
				{
					if (filterData == PxFilterData() || filterData.word3 == 17 || filterData.word3 == 18) {
						InsertOutOfMap(rigidBody.first, std::make_shared<RigiBodyStruct>(*(rigidBody.second)));
						shouldRemove = true;
						break;
					}
				}
				else if (filterData == PxFilterData()) {
					shouldRemove = true;
					break;
				}

			}
			if (shouldRemove) {
				bodiesToRemove.emplace(rigidBody.first, 1);
				rigidBody.second->Point = 0;
			}
		}
		for (auto iter : EraseVector)
			bodiesToRemove.emplace(iter.first, 1);
		for (auto iter : GetEraseMap())
			bodiesToRemove.emplace(iter, 1);
		ClearEraseMap();
		unique_writeguard<WfirstRWLock> wlock(gSceneLock);
		if (!IsRigidValid((ptr_t)gScene))
			return;
		gScene->lockWrite();
		try
		{
			// 删除不再存在于RIGID_ARRAY中的刚体及其形状
			for (auto body : bodiesToRemove) {
				PxRigidDynamic* rigidDynamic = FindDynamicRigidBody(body.first);
				if (IsRigidValid((IronMan::ptr_t)rigidDynamic)) {
					RemoveActorFromScene(rigidDynamic);
					EraseDynamicRigidBody(body.first);
				}
				//else {
				//	EraseDynamicRigidBody(body.first);
				//}

				PxRigidStatic* rigidStatic = FindStaticRigidBody(body.first);
				if (IsRigidValid((IronMan::ptr_t)rigidStatic)) {
					RemoveActorFromScene(rigidStatic);
					EraseStaticRigidBody(body.first);
				}
				//else {
				//	EraseStaticRigidBody(body.first);
				//}
				auto FindResult = RIGIDARRAY.find(body.first);
				if (FindResult != RIGIDARRAY.end())
					RIGIDARRAY.erase(FindResult);
			}
		}
		catch (...)
		{
			CONSOLE_INFO("Clear InValid Cause Exception3!");
		}
		gScene->unlockWrite();
		bodiesToRemove.clear();
	}
	//添加对象到场景中
	void PhysicsManager::AddRigidBodiesToScene(std::unordered_map<ptr_t, std::shared_ptr<RigiBodyStruct>>& RIGIDARRAY, std::unordered_map<ptr_t, std::shared_ptr<int>>& UpdateArray, std::unordered_map<ptr_t, std::shared_ptr<int>>& EraseVector)
	{
		if (!sdk::InGame())
			return;
		ClearInValidRigid(RIGIDARRAY, EraseVector);
		unique_writeguard<WfirstRWLock> wlock(gSceneLock);
		if (!IsRigidValid((ptr_t)gScene))
			return;
		gScene->lockWrite();
		try
		{
			for (auto UpdateObject : UpdateArray)
			{
				if (UpdateObject.first == TestPtr)
					Sleep(0);
				auto FindResult = RIGIDARRAY.find(UpdateObject.first);
				if (FindResult != RIGIDARRAY.end())
				{
					auto rigidBody = FindResult->second;
					bool SuccessAddRigid = false;
					if (!IsAddrValid(rigidBody->Point))
					{
						RIGIDARRAY.erase(FindResult);
						continue;
					}
					if (rigidBody->Type == 0) {
						PxRigidStatic* rigidStatic = FindStaticRigidBody(rigidBody->Point);
						if (rigidStatic && IsRigidValid((IronMan::ptr_t)rigidStatic)) {
							float dis = (PxVec3ToFVector(rigidBody->Transform.p) - LocalPos).Size() / 100.f;
							if (dis > 1200.f && rigidBody->MeshType == 0)
							{
								AddtoErase(rigidBody->Point);
								continue;
							}
							setGlobalPoseInternal(rigidStatic, rigidBody->Transform);
							continue;
						}
						else
						{
							float dis = (PxVec3ToFVector(rigidBody->Transform.p) - LocalPos).Size() / 100.f;
							if (rigidBody->MeshType == 0 && dis > 1200.f)
							{
								InsertTriangleMap(rigidBody);
								continue;
							}
							rigidStatic = gPhysics->createRigidStatic(rigidBody->Transform);
							if (!rigidStatic || !IsRigidValid((IronMan::ptr_t)rigidStatic))
							{
								InsertTriangleMap(rigidBody);
								continue;
							}
							for (auto& shape : rigidBody->Shapes) {
								bool isInValidModel = false;
								PxShape* pxShape = nullptr;
								switch (shape.ShapeGeometry.getType()) {
								case PxGeometryType::eCAPSULE:
								{
									auto CapSule = shape.ShapeGeometry.capsule();
									if (!isValidFloat(CapSule.halfHeight) || !isValidFloat(CapSule.radius))
										break;
									auto Userdata = new RigidUserData();
									if (!Userdata)
										break;
									InsertAllcateMemory(Userdata);
									Userdata->DataType = RigidFreshType::RStatic;
									Userdata->RigidPoint = rigidBody->Point;
									Userdata->TransformPtr = rigidBody->TransformPtr;
									rigidStatic->userData = (void*)Userdata;
									pxShape = gPhysics->createShape(CapSule, *gMaterial);
									ShapeUserData* InsertUserData = new ShapeUserData();
									if (!InsertUserData)
										break;
									InsertAllcateMemory(InsertUserData);
									InsertUserData->ShapePtr = shape.Shape;
									InsertUserData->ShapeTransformPtr = shape.ShapeTransformPtr;
									InsertUserData->SimulationFilterDataPtr = shape.SimulationFilterDataPtr;
									InsertUserData->ShapeTransform = shape.ShapeTransform;
									InsertUserData->SimulationFilterData = shape.SimulationFilterData;
									InsertUserData->LogTArray = rigidBody->ShapeTArray;
									InsertUserData->RigidPoint = rigidBody->Point;
									InsertUserData->ShapeType = (int)PxGeometryType::eCAPSULE;
									pxShape->userData = (void*)InsertUserData;
									break;
								}
								case PxGeometryType::eTRIANGLEMESH: {
									if (shape.TriangleMesh.mNbVertices <= 0 || shape.TriangleMesh.triangleCount <= 0)
										break;
									if (shape.SimulationFilterData.word3 == 17)
									{
										InsertTriangleMap(rigidBody);
										continue;
									}

									auto Scale = shape.ShapeGeometry.triangleMesh().scale;
									auto flags = shape.ShapeGeometry.triangleMesh().meshFlags;
									float FirstF = shape.TriangleMesh.FirstF;
									if (!isValidFloat(FirstF))
									{
										FirstF = shape.TriangleMesh.FirstF2;
										if (!isValidFloat(FirstF))
										{
											FirstF = shape.TriangleMesh.FirstF3;
											if (!isValidFloat(FirstF))
												break;
										}
									}
									int FirstV = shape.TriangleMesh.FirstV;
									if (FirstV < 0 || FirstV >= shape.TriangleMesh.mNbVertices)
										break;
									constexpr size_t BUFFER_SIZE = 256;
									char buffer[BUFFER_SIZE];
									ZeroMemory(buffer, BUFFER_SIZE);
									// 使用sprintf_s进行字符串格式化
									int ret = sprintf_s(buffer, BUFFER_SIZE, "%s_%u_%u_%u_%d_%f",
										CurrentMapName.c_str(),
										shape.SimulationFilterData.word3,
										shape.TriangleMesh.triangleCount,
										shape.TriangleMesh.mNbVertices,
										FirstV,
										FirstF);
									std::string FileNameLocation = std::string(buffer);
									auto hash_file = hash_(FileNameLocation);
									std::string FileNamePath = Path + CurrentMapName + "\\" + std::to_string(hash_file) + ".dat";
									//std::string FileNamePath = Path + CurrentMapName + "\\" + FileNameLocation + ".dat";

									auto triangleMesh = GetAlreadyLoadedTriangle(hash_file);
									if (triangleMesh && !IsValidPtr(triangleMesh->first) && !IsValidPtr(triangleMesh->second))
									{
										if (Vars.Menu.UseCacheToTrace)
										{
											if (!fileExists(FileNamePath))
											{
												PxTriangleMeshDesc meshDesc;
												bool bIsZero = false;
												for (int vi = 0; vi < shape.TriangleMesh.Vertices.size(); vi++)
												{
													if (shape.TriangleMesh.Vertices[vi].isZero())
													{
														bIsZero = false;
														break;
													}
												}
												if (bIsZero)
													break;
												meshDesc.points.count = static_cast<PxU32>(shape.TriangleMesh.Vertices.size());
												meshDesc.points.stride = sizeof(PxVec3);
												meshDesc.points.data = shape.TriangleMesh.Vertices.data();
												if (shape.TriangleMesh.mTriangleMeshFlags & 2) {
													if (shape.TriangleMesh.Indices16.size() <= 0 || (shape.TriangleMesh.Indices16[0] == 0 && shape.TriangleMesh.Indices16[1] == 0 && shape.TriangleMesh.Indices16[2] == 0))
														break;
													if (shape.TriangleMesh.Indices16[0] > meshDesc.points.count || shape.TriangleMesh.Indices16[1] > meshDesc.points.count || shape.TriangleMesh.Indices16[2] > meshDesc.points.count)
														break;
													meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices16.size() / 3);
													meshDesc.triangles.stride = 3 * sizeof(PxU16);
													meshDesc.triangles.data = shape.TriangleMesh.Indices16.data();
													meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
												}
												else {
													if (shape.TriangleMesh.Indices32.size() <= 0 || (shape.TriangleMesh.Indices32[0] == 0 && shape.TriangleMesh.Indices32[1] == 0 && shape.TriangleMesh.Indices32[2] == 0))
														break;
													if (shape.TriangleMesh.Indices32[0] > meshDesc.points.count || shape.TriangleMesh.Indices32[1] > meshDesc.points.count || shape.TriangleMesh.Indices32[2] > meshDesc.points.count)
														break;
													meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices32.size() / 3);
													meshDesc.triangles.stride = 3 * sizeof(PxU32);
													meshDesc.triangles.data = shape.TriangleMesh.Indices32.data();
												}
												if (!validateIndices(meshDesc, shape.TriangleMesh))
													break;
												PxDefaultMemoryOutputStream writeBuffer;
												PxTriangleMeshCookingResult::Enum result;
												if (!gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result)) {
													break;
												}
												PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
												triangleMesh->first = gPhysics->createTriangleMesh(readBuffer);
												saveTriangleMeshToFile(triangleMesh->first, FileNamePath.c_str());
												CONSOLE_INFO("Add Object : %s", FileNamePath.c_str());
											}
											else
											{
												*triangleMesh = loadTriangleMeshFromFile(FileNamePath.c_str());
												if (!Utils::IsSafeReadPtr(triangleMesh->first, 8))
												{
													auto nbVertices = triangleMesh->first->getNbVertices();
													auto nbTriangles = triangleMesh->first->getNbTriangles();
													auto TrianglesPtr = triangleMesh->first->getTriangles();
													auto VerticesPtr = triangleMesh->first->getVertices();
													auto TriangleMeshFlags = triangleMesh->first->getTriangleMeshFlags();
													if (TrianglesPtr == nullptr || VerticesPtr == nullptr) {
														isInValidModel = true;
														break;
													}
													else {
														for (PxU32 i = 0; i < nbVertices; ++i) {
															if (!isValidVertex(PxVec3ToFVector(VerticesPtr[i]))) {
																isInValidModel = true;
																break;
															}
														}
														if (!isInValidModel)
														{
															for (PxU32 jj = 0; jj < nbTriangles; jj++)
															{
																if (TriangleMeshFlags & PxTriangleMeshFlag::e16_BIT_INDICES)
																{
																	PxU16* mTriangles = (PxU16*)TrianglesPtr;
																	PxU16 idx1 = mTriangles[jj * 3];
																	PxU16 idx2 = mTriangles[jj * 3 + 1];
																	PxU16 idx3 = mTriangles[jj * 3 + 2];

																	if (idx1 >= nbVertices || idx2 >= nbVertices || idx3 >= nbVertices)
																	{
																		isInValidModel = true;
																		break;
																	}
																}
																else
																{
																	PxU32* mTriangles = (PxU32*)TrianglesPtr;
																	PxU32 idx1 = mTriangles[jj * 3];
																	PxU32 idx2 = mTriangles[jj * 3 + 1];
																	PxU32 idx3 = mTriangles[jj * 3 + 2];

																	if (idx1 >= nbVertices || idx2 >= nbVertices || idx3 >= nbVertices)
																	{
																		isInValidModel = true;
																		break;
																	}
																}
															}
														}
													}
													if (isInValidModel)
													{
														if (std::remove(FileNamePath.c_str()) == 0) {
															CONSOLE_INFO("Remove An InValid Module Cache : %s", FileNamePath.c_str());
															RIGIDARRAY.erase(FindResult);
															break;
														}
													}
												}
												else
												{
													PxTriangleMeshDesc meshDesc;
													bool bIsZero = false;
													for (int vi = 0; vi < shape.TriangleMesh.Vertices.size(); vi++)
													{
														if (shape.TriangleMesh.Vertices[vi].isZero())
														{
															bIsZero = false;
															break;
														}
													}
													if (bIsZero)
														break;
													meshDesc.points.count = static_cast<PxU32>(shape.TriangleMesh.Vertices.size());
													meshDesc.points.stride = sizeof(PxVec3);
													meshDesc.points.data = shape.TriangleMesh.Vertices.data();
													if (shape.TriangleMesh.mTriangleMeshFlags & 2) {
														if (shape.TriangleMesh.Indices16.size() <= 0 || (shape.TriangleMesh.Indices16[0] == 0 && shape.TriangleMesh.Indices16[1] == 0 && shape.TriangleMesh.Indices16[2] == 0))
															break;
														if (shape.TriangleMesh.Indices16[0] > meshDesc.points.count || shape.TriangleMesh.Indices16[1] > meshDesc.points.count || shape.TriangleMesh.Indices16[2] > meshDesc.points.count)
															break;
														meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices16.size() / 3);
														meshDesc.triangles.stride = 3 * sizeof(PxU16);
														meshDesc.triangles.data = shape.TriangleMesh.Indices16.data();
														meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
													}
													else {
														if (shape.TriangleMesh.Indices32.size() <= 0 || (shape.TriangleMesh.Indices32[0] == 0 && shape.TriangleMesh.Indices32[1] == 0 && shape.TriangleMesh.Indices32[2] == 0))
															break;
														if (shape.TriangleMesh.Indices32[0] > meshDesc.points.count || shape.TriangleMesh.Indices32[1] > meshDesc.points.count || shape.TriangleMesh.Indices32[2] > meshDesc.points.count)
															break;
														meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices32.size() / 3);
														meshDesc.triangles.stride = 3 * sizeof(PxU32);
														meshDesc.triangles.data = shape.TriangleMesh.Indices32.data();
													}
													if (!validateIndices(meshDesc, shape.TriangleMesh))
														break;
													PxDefaultMemoryOutputStream writeBuffer;
													PxTriangleMeshCookingResult::Enum result;
													if (!gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result)) {
														break;
													}
													PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
													triangleMesh->first = gPhysics->createTriangleMesh(readBuffer);
													saveTriangleMeshToFile(triangleMesh->first, FileNamePath.c_str());
													CONSOLE_INFO("Add Object : %s", FileNamePath.c_str());
												}
											}
										}
										else
										{
											PxTriangleMeshDesc meshDesc;
											bool bIsZero = false;
											for (int vi = 0; vi < shape.TriangleMesh.Vertices.size(); vi++)
											{
												if (shape.TriangleMesh.Vertices[vi].isZero())
												{
													bIsZero = false;
													break;
												}
											}
											if (bIsZero)
												break;
											meshDesc.points.count = static_cast<PxU32>(shape.TriangleMesh.Vertices.size());
											meshDesc.points.stride = sizeof(PxVec3);
											meshDesc.points.data = shape.TriangleMesh.Vertices.data();
											if (shape.TriangleMesh.mTriangleMeshFlags & 2) {
												if (shape.TriangleMesh.Indices16.size() <= 0 || (shape.TriangleMesh.Indices16[0] == 0 && shape.TriangleMesh.Indices16[1] == 0 && shape.TriangleMesh.Indices16[2] == 0))
													break;
												if (shape.TriangleMesh.Indices16[0] > meshDesc.points.count || shape.TriangleMesh.Indices16[1] > meshDesc.points.count || shape.TriangleMesh.Indices16[2] > meshDesc.points.count)
													break;
												meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices16.size() / 3);
												meshDesc.triangles.stride = 3 * sizeof(PxU16);
												meshDesc.triangles.data = shape.TriangleMesh.Indices16.data();
												meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
											}
											else {
												if (shape.TriangleMesh.Indices32.size() <= 0 || (shape.TriangleMesh.Indices32[0] == 0 && shape.TriangleMesh.Indices32[1] == 0 && shape.TriangleMesh.Indices32[2] == 0))
													break;
												if (shape.TriangleMesh.Indices32[0] > meshDesc.points.count || shape.TriangleMesh.Indices32[1] > meshDesc.points.count || shape.TriangleMesh.Indices32[2] > meshDesc.points.count)
													break;
												meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices32.size() / 3);
												meshDesc.triangles.stride = 3 * sizeof(PxU32);
												meshDesc.triangles.data = shape.TriangleMesh.Indices32.data();
											}
											if (!validateIndices(meshDesc, shape.TriangleMesh))
												break;
											PxDefaultMemoryOutputStream writeBuffer;
											PxTriangleMeshCookingResult::Enum result;
											if (!gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result)) {
												break;
											}
											PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
											triangleMesh->first = gPhysics->createTriangleMesh(readBuffer);
										}
										if (!Utils::IsSafeReadPtr(triangleMesh->first, 8) && (ptr_t)triangleMesh->second <= (ptr_t)triangleMesh->first)
										{
											if (IsValidPtr((void*)triangleMesh->first->getTriangles(), 8) && IsValidPtr((void*)triangleMesh->first->getVertices(), 0xC))
												InsertAlreadyLoadedTriangle(hash_file, std::make_pair(triangleMesh->first, triangleMesh->second));
											else
											{
												free(triangleMesh->second);
												break;
											}
										}
									}

									auto Userdata = new RigidUserData();
									if (!Userdata)
										break;
									InsertAllcateMemory(Userdata);
									Userdata->DataType = RigidFreshType::RStatic;
									Userdata->RigidPoint = rigidBody->Point;
									Userdata->TransformPtr = rigidBody->TransformPtr;
									Userdata->DynamicBufferedBody2WorldPtr = 0;
									rigidStatic->userData = (void*)Userdata;
									PxTriangleMeshGeometry triangleGeometry(triangleMesh->first, Scale, flags);
									pxShape = gPhysics->createShape(triangleGeometry, *gMaterial);
									ShapeUserData* InsertUserData = new ShapeUserData();
									if (!InsertUserData)
										break;
									InsertAllcateMemory(InsertUserData);
									InsertUserData->ShapePtr = shape.Shape;
									InsertUserData->NbTriangles = pxShape->getGeometry().triangleMesh().triangleMesh->getNbTriangles();
									InsertUserData->NbVertices = pxShape->getGeometry().triangleMesh().triangleMesh->getNbVertices();
									InsertUserData->ShapeTransformPtr = shape.ShapeTransformPtr;
									InsertUserData->SimulationFilterDataPtr = shape.SimulationFilterDataPtr;
									InsertUserData->BoxVecPtr = 0;
									InsertUserData->ScalePtr = shape.ShapeGeometryPtr + 4;
									InsertUserData->ShapeTransform = shape.ShapeTransform;
									InsertUserData->SimulationFilterData = shape.SimulationFilterData;
									InsertUserData->LogTArray = rigidBody->ShapeTArray;
									InsertUserData->BoxVec = PxVec3(0, 0, 0);
									InsertUserData->MeshScale = Scale;
									InsertUserData->RigidPoint = rigidBody->Point;
									InsertUserData->ShapeType = (int)PxGeometryType::eTRIANGLEMESH;
									pxShape->userData = (void*)InsertUserData;
									break;
								}
								case PxGeometryType::eHEIGHTFIELD: {
									// 确保高度场数据有效
									if (shape.HeightField.HeightData.empty() || shape.HeightField.NumRows <= 0 || shape.HeightField.NumCols <= 0) {
										break;
									}
									int* HeightData = (int*)shape.HeightField.HeightData.data();
									constexpr size_t BUFFER_SIZE = 256;
									char buffer[BUFFER_SIZE];
									// 使用sprintf_s进行字符串格式化
									int ret = sprintf_s(buffer, BUFFER_SIZE, "%u_%u_%.0f_%.0f_%.0f_%.0f_%.0f_%.0f_%d_%d_%d_%d_%d_%d_%d_%d_%d",
										shape.HeightField.NumRows,
										shape.HeightField.NumCols,
										shape.HeightField.rowLimit,
										shape.HeightField.colLimit,
										shape.HeightField.nbColumns,
										shape.HeightField.HeightScale,
										shape.HeightField.RowScale,
										shape.HeightField.ColScale,
										HeightData[0],
										HeightData[1],
										HeightData[2],
										HeightData[3],
										HeightData[4],
										HeightData[5],
										HeightData[6],
										HeightData[7],
										HeightData[8]);
									std::string FileNameLocation(buffer);
									auto hash_file = hash_(FileNameLocation);
									std::string FileNamePath = Path + CurrentMapName + "\\HF" + std::to_string(hash_file) + ".dat";
									//std::string FileNamePath = Path + CurrentMapName + "\\HF" + FileNameLocation + ".dat";
									auto heightField = GetAlreadyLoadedHeightField(hash_file);
									if (heightField && !IsValidPtr(heightField->first) && !IsValidPtr(heightField->second))
									{
										if (Vars.Menu.UseCacheToTrace)
										{
											if (!fileExists(FileNamePath))
											{
												PxHeightFieldDesc hfDesc;
												hfDesc.format = PxHeightFieldFormat::eS16_TM;
												hfDesc.nbRows = shape.HeightField.NumRows;
												hfDesc.nbColumns = shape.HeightField.NumCols;
												hfDesc.samples.data = shape.HeightField.HeightData.data();
												hfDesc.samples.stride = sizeof(PxHeightFieldSample);
												// 烹饪高度场
												PxDefaultMemoryOutputStream writeBuffer;
												if (!gCooking->cookHeightField(hfDesc, writeBuffer)) {
													break;
												}
												// 创建高度场
												PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
												heightField->first = gPhysics->createHeightField(readBuffer);
												saveHeightFieldToFile(heightField->first, FileNamePath.c_str());
												CONSOLE_INFO("Add HeightField Object : %s", FileNamePath.c_str());
											}
											else
											{
												*heightField = loadHeightFieldFromFile(FileNamePath.c_str());
												if (!Utils::IsSafeReadPtr(heightField->first, 8))
												{
													//if (hasMoreThanTenPercentZeroHeights(heightField.first)) {
													//	CONSOLE_INFO("More than 10% of the height samples are zero.");
													//	Sleep(0);
													//	// Perform necessary actions
													//}
												}
												else
												{
													PxHeightFieldDesc hfDesc;
													hfDesc.format = PxHeightFieldFormat::eS16_TM;
													hfDesc.nbRows = shape.HeightField.NumRows;
													hfDesc.nbColumns = shape.HeightField.NumCols;
													hfDesc.samples.data = shape.HeightField.HeightData.data();
													hfDesc.samples.stride = sizeof(PxHeightFieldSample);
													// 烹饪高度场
													PxDefaultMemoryOutputStream writeBuffer;
													if (!gCooking->cookHeightField(hfDesc, writeBuffer)) {
														break;
													}
													// 创建高度场
													PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
													heightField->first = gPhysics->createHeightField(readBuffer);
													saveHeightFieldToFile(heightField->first, FileNamePath.c_str());
													CONSOLE_INFO("Add HeightField Object : %s", FileNamePath.c_str());
												}
											}
										}
										else
										{
											PxHeightFieldDesc hfDesc;
											hfDesc.format = PxHeightFieldFormat::eS16_TM;
											hfDesc.nbRows = shape.HeightField.NumRows;
											hfDesc.nbColumns = shape.HeightField.NumCols;
											hfDesc.samples.data = shape.HeightField.HeightData.data();
											hfDesc.samples.stride = sizeof(PxHeightFieldSample);
											// 烹饪高度场
											PxDefaultMemoryOutputStream writeBuffer;
											if (!gCooking->cookHeightField(hfDesc, writeBuffer)) {
												break;
											}
											// 创建高度场
											PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
											heightField->first = gPhysics->createHeightField(readBuffer);
										}
										if (!Utils::IsSafeReadPtr(heightField->first, 8) && (ptr_t)heightField->second <= (ptr_t)heightField->first)
										{
											if (IsValidPtr((void*)((ptr_t)heightField->first + 0x50), 8))
											{
												ptr_t Sample = *(ptr_t*)((ptr_t)heightField->first + 0x50);
												if (IsValidPtr((void*)Sample, 8))
												{
													InsertAlreadyLoadedHeightField(hash_file, std::make_pair(heightField->first, heightField->second));
												}
												else
												{
													free(heightField->second);
													break;
												}
											}
											else
											{
												free(heightField->second);
												break;
											}
										}
									}
									if (Utils::IsSafeReadPtr(heightField->first, 8))
										break;
									PxHeightFieldGeometry heightFieldGeometry(heightField->first, shape.HeightField.Flags, shape.HeightField.HeightScale, shape.HeightField.RowScale, shape.HeightField.ColScale);
									pxShape = gPhysics->createShape(heightFieldGeometry, *gMaterial);
									auto Geometry = pxShape->getGeometry().heightField();
									if (Utils::IsSafeReadPtr(Geometry.heightField, 8))
									{
										pxShape->release();
										break;
									}
									auto Userdata = new RigidUserData();
									if (!Userdata)
										break;
									InsertAllcateMemory(Userdata);
									Userdata->DataType = RigidFreshType::RStatic;
									Userdata->RigidPoint = rigidBody->Point;
									Userdata->TransformPtr = rigidBody->TransformPtr;
									Userdata->DynamicBufferedBody2WorldPtr = 0;
									rigidStatic->userData = (void*)Userdata;
									ShapeUserDataHeightField* InsertUserData = new ShapeUserDataHeightField();
									if (!InsertUserData)
										break;
									InsertAllcateMemory(InsertUserData);
									InsertUserData->ShapePtr = shape.Shape;
									InsertUserData->RigidPoint = rigidBody->Point;
									InsertUserData->LogTArray = rigidBody->ShapeTArray;
									InsertUserData->NumRows = shape.HeightField.NumRows;
									InsertUserData->NumCols = shape.HeightField.NumCols;
									InsertUserData->rowLimit = shape.HeightField.rowLimit;
									InsertUserData->colLimit = shape.HeightField.colLimit;
									InsertUserData->nbColumns = shape.HeightField.nbColumns;
									InsertUserData->RowScale = shape.HeightField.RowScale;
									InsertUserData->ColScale = shape.HeightField.ColScale;
									InsertUserData->HeightScale = shape.HeightField.HeightScale;
									InsertUserData->HeightField = shape.HeightField.HeightField;
									InsertUserData->Samples = shape.HeightField.Samples;
									pxShape->userData = (void*)InsertUserData;
									break;
								}
								default:
									break;
								}
								if (isInValidModel)
									break;
								if (IsRigidValid((IronMan::ptr_t)rigidStatic) && IsRigidValid((IronMan::ptr_t)pxShape)) {
									pxShape->setSimulationFilterData(shape.SimulationFilterData);
									pxShape->setLocalPose(shape.ShapeTransform);
									pxShape->setFlags(shape.ShapeFlags);
									rigidStatic->attachShape(*pxShape);
									pxShape->release();
									pxShape = nullptr;
									SuccessAddRigid = true;
								}
							}

							if (SuccessAddRigid && IsRigidValid((IronMan::ptr_t)rigidStatic)) {
								gScene->addActor(*rigidStatic);
								InsertStaticRigidBody(rigidBody->Point, rigidStatic);
							}
							else
							{
								InsertOutOfMap(rigidBody->Point, std::make_shared<RigiBodyStruct>(*rigidBody));
								rigidStatic->release();
								rigidStatic = nullptr;
							}
						}
					}
					else {

						PxRigidDynamic* rigidDynamic = FindDynamicRigidBody(rigidBody->Point);
						if (rigidDynamic && IsRigidValid((IronMan::ptr_t)rigidDynamic)) {
							continue;
						}
						else {
							if (FindOutOfMap(rigidBody->Point))
							{
								InsertTriangleMap(rigidBody);
								continue;
							}
							// 添加新的动态刚体
							float dis = (PxVec3ToFVector(rigidBody->Transform.p) - LocalPos).Size() / 100.f;
							if (dis > MaxDynamicRigidDistance)
							{
								InsertTriangleMap(rigidBody);
								continue;
							}
							if (dis < 5.f)
								Sleep(0);
							PxTransform transform(rigidBody->Transform.p, rigidBody->Transform.q);
							rigidDynamic = gPhysics->createRigidDynamic(transform);
							if (!rigidDynamic || !IsRigidValid((IronMan::ptr_t)rigidDynamic))
							{
								InsertTriangleMap(rigidBody);
								continue;
							}
							RigidFreshType UpdateType = RigidFreshType::RNotAdd;
							for (auto& shape : rigidBody->Shapes) {
								if (shape.SimulationFilterData == PxFilterData())
									continue;
								bool isInValidModel = false;
								PxShape* pxShape = nullptr;
								switch (shape.ShapeGeometry.getType()) {
								case PxGeometryType::eTRIANGLEMESH: {

									if (shape.TriangleMesh.mNbVertices <= 0 || shape.TriangleMesh.triangleCount <= 0)
										break;
									UpdateType = FilterDynamicRigid(shape.SimulationFilterData, shape.TriangleMesh.mNbVertices);
									if (UpdateType == RigidFreshType::RNotAdd)
									{
										InsertTriangleMap(rigidBody);
										break;
									}

									auto Scale = shape.ShapeGeometry.triangleMesh().scale;
									auto flags = shape.ShapeGeometry.triangleMesh().meshFlags;

									float FirstF = shape.TriangleMesh.FirstF;
									if (!isValidFloat(FirstF))
									{
										FirstF = shape.TriangleMesh.FirstF2;
										if (!isValidFloat(FirstF))
										{
											FirstF = shape.TriangleMesh.FirstF3;
											if (!isValidFloat(FirstF))
												break;
										}
									}
									int FirstV = shape.TriangleMesh.FirstV;
									if (FirstV < 0 || FirstV >= shape.TriangleMesh.mNbVertices)
										break;
									constexpr size_t BUFFER_SIZE = 256;
									char buffer[BUFFER_SIZE];
									ZeroMemory(buffer, BUFFER_SIZE);
									// 使用sprintf_s进行字符串格式化
									int ret = sprintf_s(buffer, BUFFER_SIZE, "%sDM_%u_%u_%u_%d_%f",
										CurrentMapName.c_str(),
										shape.SimulationFilterData.word3,
										shape.TriangleMesh.triangleCount,
										shape.TriangleMesh.mNbVertices,
										FirstV,
										FirstF);
									std::string	FileNameLocation = std::string(buffer);
									auto hash_file = hash_(FileNameLocation);
									std::string FileNamePath = Path + CurrentMapName + "\\" + std::to_string(hash_file) + ".dat";
									//std::string FileNamePath = Path + CurrentMapName + "\\" + FileNameLocation + ".dat";

									auto triangleMesh = GetAlreadyLoadedTriangle(hash_file);
									if (triangleMesh && !IsValidPtr(triangleMesh->first) && !IsValidPtr(triangleMesh->second))
									{
										if (Vars.Menu.UseCacheToTrace)
										{
											if (!fileExists(FileNamePath))
											{
												PxTriangleMeshDesc meshDesc;
												bool bIsZero = false;
												for (int vi = 0; vi < shape.TriangleMesh.Vertices.size(); vi++)
												{
													if (shape.TriangleMesh.Vertices[vi].isZero())
													{
														bIsZero = false;
														break;
													}
												}
												if (bIsZero)
													break;
												meshDesc.points.count = static_cast<PxU32>(shape.TriangleMesh.Vertices.size());
												meshDesc.points.stride = sizeof(PxVec3);
												meshDesc.points.data = shape.TriangleMesh.Vertices.data();
												if (shape.TriangleMesh.mTriangleMeshFlags & 2) {
													if (shape.TriangleMesh.Indices16.size() <= 0 || (shape.TriangleMesh.Indices16[0] == 0 && shape.TriangleMesh.Indices16[1] == 0 && shape.TriangleMesh.Indices16[2] == 0))
														break;
													if (shape.TriangleMesh.Indices16[0] > meshDesc.points.count || shape.TriangleMesh.Indices16[1] > meshDesc.points.count || shape.TriangleMesh.Indices16[2] > meshDesc.points.count)
														break;
													meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices16.size() / 3);
													meshDesc.triangles.stride = 3 * sizeof(PxU16);
													meshDesc.triangles.data = shape.TriangleMesh.Indices16.data();
													meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
												}
												else {
													if (shape.TriangleMesh.Indices32.size() <= 0 || (shape.TriangleMesh.Indices32[0] == 0 && shape.TriangleMesh.Indices32[1] == 0 && shape.TriangleMesh.Indices32[2] == 0))
														break;
													if (shape.TriangleMesh.Indices32[0] > meshDesc.points.count || shape.TriangleMesh.Indices32[1] > meshDesc.points.count || shape.TriangleMesh.Indices32[2] > meshDesc.points.count)
														break;
													meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices32.size() / 3);
													meshDesc.triangles.stride = 3 * sizeof(PxU32);
													meshDesc.triangles.data = shape.TriangleMesh.Indices32.data();
												}
												if (!validateIndices(meshDesc, shape.TriangleMesh))
													break;
												PxDefaultMemoryOutputStream writeBuffer;
												PxTriangleMeshCookingResult::Enum result;
												if (!gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result)) {
													break;
												}
												PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
												triangleMesh->first = gPhysics->createTriangleMesh(readBuffer);
												saveTriangleMeshToFile(triangleMesh->first, FileNamePath.c_str());
												CONSOLE_INFO("Add Object : %s", FileNamePath.c_str());
											}
											else
											{
												*triangleMesh = loadTriangleMeshFromFile(FileNamePath.c_str());
												if (!Utils::IsSafeReadPtr(triangleMesh->first, 8))
												{
													auto nbVertices = triangleMesh->first->getNbVertices();
													auto nbTriangles = triangleMesh->first->getNbTriangles();
													auto TrianglesPtr = triangleMesh->first->getTriangles();
													auto VerticesPtr = triangleMesh->first->getVertices();
													auto TriangleMeshFlags = triangleMesh->first->getTriangleMeshFlags();
													if (TrianglesPtr == nullptr || VerticesPtr == nullptr) {
														isInValidModel = true;
														break;
													}
													else {
														for (PxU32 i = 0; i < nbVertices; ++i) {
															if (!isValidVertex(PxVec3ToFVector(VerticesPtr[i]))) {
																isInValidModel = true;
																break;
															}
														}
														if (!isInValidModel)
														{
															for (PxU32 jj = 0; jj < nbTriangles; jj++)
															{
																if (TriangleMeshFlags & PxTriangleMeshFlag::e16_BIT_INDICES)
																{
																	PxU16* mTriangles = (PxU16*)TrianglesPtr;
																	PxU16 idx1 = mTriangles[jj * 3];
																	PxU16 idx2 = mTriangles[jj * 3 + 1];
																	PxU16 idx3 = mTriangles[jj * 3 + 2];

																	if (idx1 >= nbVertices || idx2 >= nbVertices || idx3 >= nbVertices)
																	{
																		isInValidModel = true;
																		break;
																	}
																}
																else
																{
																	PxU32* mTriangles = (PxU32*)TrianglesPtr;
																	PxU32 idx1 = mTriangles[jj * 3];
																	PxU32 idx2 = mTriangles[jj * 3 + 1];
																	PxU32 idx3 = mTriangles[jj * 3 + 2];

																	if (idx1 >= nbVertices || idx2 >= nbVertices || idx3 >= nbVertices)
																	{
																		isInValidModel = true;
																		break;
																	}
																}
															}
														}

													}
													if (isInValidModel)
													{
														if (std::remove(FileNamePath.c_str()) == 0) {
															CONSOLE_INFO("Remove An InValid Module Cache : %s", FileNamePath.c_str());
															RIGIDARRAY.erase(FindResult);
															break;
														}
													}
												}
												else
												{
													PxTriangleMeshDesc meshDesc;
													bool bIsZero = false;
													for (int vi = 0; vi < shape.TriangleMesh.Vertices.size(); vi++)
													{
														if (shape.TriangleMesh.Vertices[vi].isZero())
														{
															bIsZero = false;
															break;
														}
													}
													if (bIsZero)
														break;
													meshDesc.points.count = static_cast<PxU32>(shape.TriangleMesh.Vertices.size());
													meshDesc.points.stride = sizeof(PxVec3);
													meshDesc.points.data = shape.TriangleMesh.Vertices.data();
													if (shape.TriangleMesh.mTriangleMeshFlags & 2) {
														if (shape.TriangleMesh.Indices16.size() <= 0 || (shape.TriangleMesh.Indices16[0] == 0 && shape.TriangleMesh.Indices16[1] == 0 && shape.TriangleMesh.Indices16[2] == 0))
															break;
														if (shape.TriangleMesh.Indices16[0] > meshDesc.points.count || shape.TriangleMesh.Indices16[1] > meshDesc.points.count || shape.TriangleMesh.Indices16[2] > meshDesc.points.count)
															break;
														meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices16.size() / 3);
														meshDesc.triangles.stride = 3 * sizeof(PxU16);
														meshDesc.triangles.data = shape.TriangleMesh.Indices16.data();
														meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
													}
													else {
														if (shape.TriangleMesh.Indices32.size() <= 0 || (shape.TriangleMesh.Indices32[0] == 0 && shape.TriangleMesh.Indices32[1] == 0 && shape.TriangleMesh.Indices32[2] == 0))
															break;
														if (shape.TriangleMesh.Indices32[0] > meshDesc.points.count || shape.TriangleMesh.Indices32[1] > meshDesc.points.count || shape.TriangleMesh.Indices32[2] > meshDesc.points.count)
															break;
														meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices32.size() / 3);
														meshDesc.triangles.stride = 3 * sizeof(PxU32);
														meshDesc.triangles.data = shape.TriangleMesh.Indices32.data();
													}
													if (!validateIndices(meshDesc, shape.TriangleMesh))
														break;
													PxDefaultMemoryOutputStream writeBuffer;
													PxTriangleMeshCookingResult::Enum result;
													if (!gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result)) {
														break;
													}
													PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
													triangleMesh->first = gPhysics->createTriangleMesh(readBuffer);
													saveTriangleMeshToFile(triangleMesh->first, FileNamePath.c_str());
													CONSOLE_INFO("Add Object : %s", FileNamePath.c_str());
												}
											}
										}
										else
										{
											PxTriangleMeshDesc meshDesc;
											bool bIsZero = false;
											for (int vi = 0; vi < shape.TriangleMesh.Vertices.size(); vi++)
											{
												if (shape.TriangleMesh.Vertices[vi].isZero())
												{
													bIsZero = false;
													break;
												}
											}
											if (bIsZero)
												break;
											meshDesc.points.count = static_cast<PxU32>(shape.TriangleMesh.Vertices.size());
											meshDesc.points.stride = sizeof(PxVec3);
											meshDesc.points.data = shape.TriangleMesh.Vertices.data();
											if (shape.TriangleMesh.mTriangleMeshFlags & 2) {
												if (shape.TriangleMesh.Indices16.size() <= 0 || (shape.TriangleMesh.Indices16[0] == 0 && shape.TriangleMesh.Indices16[1] == 0 && shape.TriangleMesh.Indices16[2] == 0))
													break;
												if (shape.TriangleMesh.Indices16[0] > meshDesc.points.count || shape.TriangleMesh.Indices16[1] > meshDesc.points.count || shape.TriangleMesh.Indices16[2] > meshDesc.points.count)
													break;
												meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices16.size() / 3);
												meshDesc.triangles.stride = 3 * sizeof(PxU16);
												meshDesc.triangles.data = shape.TriangleMesh.Indices16.data();
												meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
											}
											else {
												if (shape.TriangleMesh.Indices32.size() <= 0 || (shape.TriangleMesh.Indices32[0] == 0 && shape.TriangleMesh.Indices32[1] == 0 && shape.TriangleMesh.Indices32[2] == 0))
													break;
												if (shape.TriangleMesh.Indices32[0] > meshDesc.points.count || shape.TriangleMesh.Indices32[1] > meshDesc.points.count || shape.TriangleMesh.Indices32[2] > meshDesc.points.count)
													break;
												meshDesc.triangles.count = static_cast<PxU32>(shape.TriangleMesh.Indices32.size() / 3);
												meshDesc.triangles.stride = 3 * sizeof(PxU32);
												meshDesc.triangles.data = shape.TriangleMesh.Indices32.data();
											}
											if (!validateIndices(meshDesc, shape.TriangleMesh))
												break;
											PxDefaultMemoryOutputStream writeBuffer;
											PxTriangleMeshCookingResult::Enum result;
											if (!gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result)) {
												break;
											}
											PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
											triangleMesh->first = gPhysics->createTriangleMesh(readBuffer);
										}
										if (!Utils::IsSafeReadPtr(triangleMesh->first, 8) && (ptr_t)triangleMesh->second <= (ptr_t)triangleMesh->first)
										{
											if (IsValidPtr((void*)triangleMesh->first->getTriangles(), 8) && IsValidPtr((void*)triangleMesh->first->getVertices(), 0xC))
												InsertAlreadyLoadedTriangle(hash_file, std::make_pair(triangleMesh->first, triangleMesh->second));
											else
											{
												free(triangleMesh->second);
												break;
											}
										}
									}

									auto Userdata = new RigidUserData();
									if (!Userdata)
										break;
									InsertAllcateMemory(Userdata);
									Userdata->DataType = UpdateType;
									Userdata->RigidPoint = rigidBody->Point;
									Userdata->TransformPtr = rigidBody->TransformPtr;
									Userdata->DynamicBufferedBody2WorldPtr = rigidBody->DynamicBufferedBody2WorldPtr;
									rigidDynamic->userData = (void*)Userdata;
									PxTriangleMeshGeometry triangleGeometry(triangleMesh->first, Scale, flags);
									pxShape = gPhysics->createShape(triangleGeometry, *gMaterial);
									ShapeUserData* InsertUserData = new ShapeUserData();
									if (!InsertUserData)
										break;
									InsertAllcateMemory(InsertUserData);
									InsertUserData->ShapePtr = shape.Shape;
									InsertUserData->NbTriangles = pxShape->getGeometry().triangleMesh().triangleMesh->getNbTriangles();
									InsertUserData->NbVertices = pxShape->getGeometry().triangleMesh().triangleMesh->getNbVertices();
									InsertUserData->ShapeTransformPtr = shape.ShapeTransformPtr;
									InsertUserData->SimulationFilterDataPtr = shape.SimulationFilterDataPtr;
									InsertUserData->BoxVecPtr = 0;
									InsertUserData->ScalePtr = shape.ShapeGeometryPtr + 4;
									InsertUserData->ShapeTransform = shape.ShapeTransform;
									InsertUserData->SimulationFilterData = shape.SimulationFilterData;
									InsertUserData->LogTArray = rigidBody->ShapeTArray;
									InsertUserData->BoxVec = PxVec3(0, 0, 0);
									InsertUserData->MeshScale = Scale;
									InsertUserData->RigidPoint = rigidBody->Point;
									InsertUserData->ShapeType = (int)PxGeometryType::eTRIANGLEMESH;
									pxShape->userData = (void*)InsertUserData;
									break;
								}
								default:
									break;
								}
								if (isInValidModel)
									break;
								if (IsRigidValid((IronMan::ptr_t)rigidDynamic) && IsRigidValid((IronMan::ptr_t)pxShape)) {
									pxShape->setSimulationFilterData(shape.SimulationFilterData);
									pxShape->setLocalPose(shape.ShapeTransform);
									pxShape->setFlags(shape.ShapeFlags);
									rigidDynamic->attachShape(*pxShape);
									pxShape->release();
									SuccessAddRigid = true;
								}
							}
							if (SuccessAddRigid && IsRigidValid((IronMan::ptr_t)rigidDynamic)) {
								gScene->addActor(*rigidDynamic);
								InsertDynamicRigidBody(rigidBody->Point, rigidDynamic);
							}
							else
							{
								InsertOutOfMap(rigidBody->Point, std::make_shared<RigiBodyStruct>(*rigidBody));
								rigidDynamic->release();
								rigidDynamic = nullptr;
							}
						}
					}
				}
			}
		}
		catch (...)
		{
			CONSOLE_INFO("AddRigid Cause Exception!");
		}
		gScene->unlockWrite();
		UpdateArray.clear();
		EraseVector.clear();
	}
	//循环遍历场景对象、筛选有用的对象
	void PhysicsManager::UpdateRigidData(VMMDLL_SCATTER_HANDLE ScatterHandle)
	{
		std::unordered_map<ptr_t, std::shared_ptr<int>>CurrentUpdateArray;
		std::unordered_map<ptr_t, std::shared_ptr<int>>NeedClearRigid;
		if (!UpdateInfoAndCheckPath())
			return;
		//100ms更新一次数据
		static ULONGLONG RunTick = GetTickCount64();
		if (GetTickCount64() - RunTick >= 50)
		{
			//未获取UWorld的Scene则获取一次
			auto StartTime = GetTickCount64();
			RunTick = GetTickCount64();
			if (SyncScene == 0)
			{
				Sleep(5000);
				auto& World = sdk::GetWorld();
				SyncScene = GetPhysxSceneFromUWorld(World);
			}
			if (SyncScene == 0)
				return;
			int TotleReadByte = 0;
			//每次更新SceneAcotr的数量变量
			int ReadCount = 0;
			//每次更新HeightField的数量变量
			int HeightFieldReadCount = 0;
			//当前需要更新的高度场数组
			std::unique_ptr<std::unordered_map<ptr_t, std::shared_ptr<int>>> CurrentHeightFieldArray(new std::unordered_map<ptr_t, std::shared_ptr<int>>());
			//存储Scene里循环数量的Actor
			std::vector<ptr_t> SceneActor;
			//判断是第一次读取则读取数量为全部
			if (!FirstLoadAllDataSet)
			{
				//读取全部数据
				TArray<void*> SceneArray = GetDMA().Read<TArray<void*>>(SyncScene + 0x2618);
				if (SceneArray->Count <= 0 || SceneArray->Count >= 100000)
					return;
				SyncRigidArray = (ptr_t)SceneArray->GetData();
				ReadCount = SceneArray->Count;
				SceneActor.resize(ReadCount);
				GetDMA().Read(SyncRigidArray, (ptr_t)SceneActor.data(), ReadCount * 8);
				//for (int i = 0; i < SceneActor.size(); i++) {
				//	InsertCurrentSceneActor(SceneActor[i]);
				//}
				TotleReadByte += ReadCount * 8;
			}
			else
			{
				//第一次加载完之后判断RigidLoopCount是否为0或者等于RigidLoopCount进行重置读取
				if (RigidLoopCount == 0 || CurrentLoopCount >= RigidLoopCount)
				{
					TArray<void*> SceneArray = GetDMA().Read<TArray<void*>>(SyncScene + 0x2618);
					if (SceneArray->Count <= 0 || SceneArray->Count >= 200000)
						return;
					//std::unique_ptr<unique_writeguard<WfirstRWLock>> wlock(new unique_writeguard<WfirstRWLock>(gSceneLock));
					//if (!IsRigidValid((ptr_t)gScene))
					//	return;
					//gScene->lockWrite();
					//PxU32 numActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);
					//gScene->unlockWrite();
					//wlock.reset();
					//float PtrPercent = (float)numActors / (float)SceneArray->Count;
					//if (PtrPercent < 0.3f)
					//{
					//	DestroyAllRigidBodies();
					//	return;
					//}
					//CONSOLE_INFO("Totle : %d | %d", SceneArray->Count, RIGID_ARRAY.size());
					//MaxUpdateRigid = 100000;
					MaxUpdateRigid = Vars.Aimbot.MaxUpdateRigid;
					MaxDynamicRigidDistance = Vars.Aimbot.MaxDynamicRigidDistance;
					SyncRigidArray = (ptr_t)SceneArray->GetData();
					RigidLoopCount = SceneArray->Count / MaxUpdateRigid;
					RigidSubCount = SceneArray->Count % MaxUpdateRigid;
					ReadCount = RigidLoopCount != 0 ? MaxUpdateRigid : RigidSubCount;
					//重置CurrentLoopCount为0
					CurrentLoopCount = 0;

					// 查找并移除RIGID_ARRAY中存在但CurrentScene中不存在的指针
					for (auto iter : RIGID_ARRAY) {
						if (iter.second->CurrentReadTime != CurrentReadTime) {
							NeedClearRigid.emplace(iter.first, std::make_shared<int>(1));
						}
					}

					std::vector<ptr_t>EraseLowSpeedCache;
					for (auto iter : LowSpeedCache)
					{
						if (!IsAddrValid(iter.first))
						{
							EraseLowSpeedCache.push_back(iter.first);
							continue;
						}
						if (iter.second->CurrentReadTime != CurrentReadTime) {
							EraseLowSpeedCache.push_back(iter.first);
						}
					}
					for (auto iter : EraseLowSpeedCache)
					{
						auto FindResult = LowSpeedCache.find(iter);
						if (FindResult != LowSpeedCache.end())
							LowSpeedCache.erase(FindResult);
					}
					std::unique_ptr<unique_writeguard<WfirstRWLock>> OutOfMaprlock(new unique_writeguard(OutOfMapLock));
					std::vector<ptr_t>EraseOutOfMap;
					for (auto iter : OutOfMapWithLock)
					{
						if (!IsAddrValid(iter.first))
						{
							EraseOutOfMap.push_back(iter.first);
							continue;
						}
						if (iter.second->CurrentReadTime != CurrentReadTime) {
							EraseOutOfMap.push_back(iter.first);
						}
					}
					for (auto iter : EraseOutOfMap)
					{
						auto FindResult = OutOfMapWithLock.find(iter);
						if (FindResult != OutOfMapWithLock.end())
							OutOfMapWithLock.erase(FindResult);
					}
					OutOfMaprlock.reset();

					std::vector<ptr_t>NeedClearHeight;
					if (HeightFieldUpdateArray.size())
					{
						//查找是否有HeightFieldUpdateArray不存在于RIGID_ARRAY
						//如果有则添加入NeedClearHeight
						for (auto iter : HeightFieldUpdateArray) {
							if (RIGID_ARRAY.find(iter.first) == RIGID_ARRAY.end()) {
								NeedClearHeight.push_back(iter.first);
							}
						}
						//进行从HeightFieldUpdateArray中移除不存在的对象
						for (auto iter : NeedClearHeight)
						{
							auto FindResult = HeightFieldUpdateArray.find(iter);
							if (FindResult != HeightFieldUpdateArray.end())
								HeightFieldUpdateArray.erase(FindResult);
						}
						////debug
						//RigiBodyStruct NearlyRigid;
						//float NearlyDistance = 1000000.f;
						//for (auto iter : HeightFieldUpdateArray) {
						//	auto LocalPostion = sdk::GetLocalPos();
						//	auto HeightFieldPos = PxVec3ToFVector(iter.second.Transform.p);
						//	auto dis = (HeightFieldPos - LocalPostion).Size() / 100.f;
						//	if (dis < NearlyDistance)
						//	{
						//		NearlyDistance = dis;
						//		NearlyRigid = iter.second;
						//	}
						//}
						//Sleep(0);
						//for (auto iter : NearlyRigid.Shapes)
						//{
						//	auto FilePath = "\\HF" + std::to_string(hash_(iter.HeightField.HeightFieldName)) + ".dat";
						//	Sleep(0);
						//}
					}
					CurrentReadTime++;
					gScene->lockWrite();
					try
					{
						auto DeleteMemory = GetDeleteMemory();
						//CONSOLE_INFO("DeleteMemory:%d", DeleteMemory.size());
						for (auto iter : DeleteMemory)
						{
							if (IsInAllocateMemory(iter.first))
							{
								EraseDeleteMemory(iter.first);
								EraseAllocateMemory(iter.first);
							}
							else
							{
								EraseDeleteMemory(iter.first);
							}
						}
						DeleteMemory.clear();
					}
					catch (...)
					{

					}
					gScene->unlockWrite();
				}
				else
				{
					//递增 进行下一轮
					CurrentLoopCount++;
					//选择读取数量
					ReadCount = CurrentLoopCount != RigidLoopCount ? MaxUpdateRigid : RigidSubCount;
				}
				if (HeightFieldUpdateArray.size())
				{
					//第一次加载完之后判断HeightFieldLoopCount是否为0或者等于HeightFieldLoopCount进行重置读取
					if (HeightFieldLoopCount == 0 || HeightFieldCurrentLoopCount >= HeightFieldLoopCount)
					{
						HeightFieldLoopCount = HeightFieldUpdateArray.size() / HeightFieldUpdateRigid;
						HeightFieldSubCount = HeightFieldUpdateArray.size() % HeightFieldUpdateRigid;
						HeightFieldReadCount = HeightFieldLoopCount != 0 ? HeightFieldUpdateRigid : HeightFieldSubCount;
						HeightFieldCurrentLoopCount = 0;
						if (ReadedHeightFieldUpdateArray.size())
							ReadedHeightFieldUpdateArray.clear();
						if (BackUpHeightFieldUpdateArray.size())
							BackUpHeightFieldUpdateArray.clear();
						//将HeightFieldUpdateArray拷贝到BackUpHeightFieldUpdateArray
						BackUpHeightFieldUpdateArray = HeightFieldUpdateArray;
					}
					else
					{
						//递增 进行下一轮
						HeightFieldCurrentLoopCount++;
						//选择读取数量
						HeightFieldReadCount = HeightFieldCurrentLoopCount != HeightFieldLoopCount ? HeightFieldUpdateRigid : HeightFieldSubCount;
					}
					//遍历BackUpHeightFieldUpdateArray将未更新的对象添加入CurrentHeightFieldArray并将已添加的添加到ReadedHeightFieldUpdateArray
					for (auto iter : BackUpHeightFieldUpdateArray)
					{
						bool bFindInArray = false;
						for (int k = 0; k < ReadedHeightFieldUpdateArray.size(); k++)
						{
							if (ReadedHeightFieldUpdateArray[k] == iter.first)
							{
								bFindInArray = true;
								break;
							}
						}
						if (!bFindInArray)
						{
							ReadedHeightFieldUpdateArray.push_back(iter.first);
							CurrentHeightFieldArray->emplace(iter.first, std::make_shared<int>(1));
							if (CurrentHeightFieldArray->size() == HeightFieldReadCount)
								break;
						}
						continue;
					}

					if (CurrentHeightFieldArray->size() == HeightFieldReadCount)
					{
						for (auto iter : *CurrentHeightFieldArray)
						{
							auto FindResult = BackUpHeightFieldUpdateArray.find(iter.first);
							if (FindResult != BackUpHeightFieldUpdateArray.end())
							{
								for (auto& shape : FindResult->second->Shapes)
								{
									auto HeightField = shape.HeightField;
									if (IsAddrValid(HeightField.HolderPtr) && IsAddrValid(HeightField.HeightField) && IsAddrValid(HeightField.Samples))
									{
										GetDMA().queueScatterReadEx(ScatterHandle, HeightField.HolderPtr + 0x8, &shape.HeightField.HeightField, 8);
										GetDMA().queueScatterReadEx(ScatterHandle, HeightField.HeightField + 0x50, &shape.HeightField.Samples, 8);
										GetDMA().queueScatterReadEx(ScatterHandle, HeightField.Samples, shape.HeightField.HeightData.data(), 5 * sizeof(PxHeightFieldSample));
									}
								}
							}
						}
					}
				}

				SceneActor.resize(ReadCount);
				for (int i = 0; i < SceneActor.size(); i++) {
					GetDMA().queueScatterReadEx(ScatterHandle, SyncRigidArray + (CurrentLoopCount * MaxUpdateRigid + i) * 8, &SceneActor[i], 8);
				}
				TotleReadByte += GetDMA().executeScatterRead2(ScatterHandle);
				for (int i = 0; i < SceneActor.size(); i++) {
					if (!IsAddrValid(SceneActor[i]))
						continue;
					auto FindResult = RIGID_ARRAY.find(SceneActor[i]);
					if (FindResult != RIGID_ARRAY.end())
						FindResult->second->CurrentReadTime = CurrentReadTime;
				}
				//如果高度场数量匹配则进行查找非法的高度场并进行移除
				if (CurrentHeightFieldArray->size() == HeightFieldReadCount)
				{
					for (auto iter : *CurrentHeightFieldArray)
					{
						if (!IsAddrValid(iter.first))
							continue;
						//存在于BackUpHeightFieldUpdateArray则继续
						auto FindResult = BackUpHeightFieldUpdateArray.find(iter.first);
						if (FindResult != BackUpHeightFieldUpdateArray.end())
						{
							//存在于RIGID_ARRAY则继续
							auto FindResult2 = RIGID_ARRAY.find(iter.first);
							if (FindResult2 != RIGID_ARRAY.end())
							{
								if (!IsAddrValid(FindResult2->second->Point) || FindResult2->second->Shapes.size() != FindResult->second->Shapes.size())
									continue;
								//匹配层次数量
								int FindCount = 0;
								int ToTleCount = FindResult->second->Shapes.size();
								for (auto shape1 : FindResult->second->Shapes)
								{
									for (auto shape2 : FindResult2->second->Shapes)
									{
										bool ValueIsOk = false;
										if (shape1.HeightField.HeightField == shape2.HeightField.HeightField
											&& shape1.HeightField.Samples == shape2.HeightField.Samples)
										{
											ValueIsOk = true;
											bool HeightSamepleSame = true;
											for (int i = 0; i < 5; i++)
											{
												if (shape1.HeightField.HeightData.size() < 5 || shape2.HeightField.HeightData.size() < 5
													|| !IsValidPtr(shape1.HeightField.HeightData.data()) || !IsValidPtr(shape2.HeightField.HeightData.data()))
												{
													HeightSamepleSame = false;
													break;
												}
												if (shape1.HeightField.HeightData[i].height != shape2.HeightField.HeightData[i].height
													|| shape1.HeightField.HeightData[i].materialIndex0 != shape2.HeightField.HeightData[i].materialIndex0
													|| shape1.HeightField.HeightData[i].materialIndex1 != shape2.HeightField.HeightData[i].materialIndex1)
												{
													HeightSamepleSame = false;
													break;
												}
											}
											if (HeightSamepleSame)
												FindCount++;
										}
									}
								}
								if (ToTleCount != FindCount)
								{
									CONSOLE_INFO("HeightField Change! %p", FindResult->first);
									auto FindToErase = HeightFieldUpdateArray.find(FindResult->first);
									if (FindToErase != HeightFieldUpdateArray.end())
									{
										if (!IsAddrValid(FindToErase->first) || !IsAddrValid(FindToErase->second->Point))
											continue;
										auto FindHeightField = FindStaticRigidBody(FindToErase->second->Point);
										if (FindHeightField)
										{
											if (!sdk::InGame())
												return;
											std::unique_ptr<unique_writeguard<WfirstRWLock>> wlock(new unique_writeguard<WfirstRWLock>(gSceneLock));
											gScene->lockWrite();
											try
											{
												RemoveActorFromScene(FindHeightField);
											}
											catch (...)
											{
												CONSOLE_INFO("Remove HF Exception!");
											}
											gScene->unlockWrite();
											wlock.reset();
											EraseStaticRigidBody(FindToErase->second->Point);
											SceneActor.push_back(FindResult->first);
											HeightFieldUpdateArray.erase(FindToErase);
											RIGID_ARRAY.erase(FindResult2);
											continue;
										}
									}
								}
							}
						}
					}
				}
			}

			if (FirstLoadAllDataSet)
			{
				//将当前存在于LowSpeedCache、OutOfMapWithLock的对象读取其shape结构
				for (int i = 0; i < SceneActor.size(); i++) {
					if (!IsAddrValid(SceneActor[i]))
						continue;
					if (SceneActor[i] == TestPtr)
						Sleep(0);
					auto FindResult = LowSpeedCache.find(SceneActor[i]);
					if (FindResult != LowSpeedCache.end())
					{
						if (FindResult->second->CmpShapeTArray->Data == 0)
							FindResult->second->CmpShapeTArray = FindResult->second->ShapeTArray;
						if (FindResult->second->ReadTimeCount < 4)
						{
							GetDMA().queueScatterReadEx(ScatterHandle, FindResult->first + 0x28 + FindResult->second->ReadTimeCount * 2,
								(void*)((ptr_t)&FindResult->second->CmpShapeTArray + FindResult->second->ReadTimeCount * 2), sizeof(WORD));
							WORD NumOfShapes = (WORD)FindResult->second->CmpShapeTArray->Count;
							if (NumOfShapes > 0 && NumOfShapes < 20)
							{
								if (FindResult->second->Shapes.size() != NumOfShapes)
									FindResult->second->Shapes.resize(NumOfShapes);
								if (NumOfShapes == 1)
								{
									if (FindResult->second->Shapes[0].Shape == 0 || FindResult->second->Shapes[0].Shape != (ptr_t)FindResult->second->CmpShapeTArray.GetData())
									{
										FindResult->second->Shapes[0].Shape = (ptr_t)FindResult->second->CmpShapeTArray.GetData();
									}
									else
									{
										FindResult->second->Shapes[0].Shape = (ptr_t)FindResult->second->CmpShapeTArray.GetData();
										GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->Shapes[0].Shape + 0x98,
											(void*)&FindResult->second->Shapes[0].GeoType, sizeof(WORD));
										if (!FindResult->second->Shapes[0].FirstInitGeo)
										{
											GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->Shapes[0].Shape + 0x98,
												(void*)&FindResult->second->Shapes[0].CmpGeoType, sizeof(WORD));
											FindResult->second->Shapes[0].FirstInitGeo = true;
										}
									}
								}
								else
								{
									for (int iop = 0; iop < FindResult->second->Shapes.size(); iop++)
									{
										if (!FindResult->second->Shapes[iop].FirstInitPtr)
										{
											GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)FindResult->second->CmpShapeTArray.GetData() + iop * 8,
												&FindResult->second->Shapes[iop].Shape, sizeof(ptr_t));
											FindResult->second->Shapes[iop].FirstInitPtr = true;
										}
										else
										{
											if (FindResult->second->LowSpeedReadTime == iop)
											{
												GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)FindResult->second->CmpShapeTArray.GetData() + iop * 8 + FindResult->second->LowSpeedReadTime * 1,
													(void*)((ptr_t)&FindResult->second->Shapes[iop].Shape + FindResult->second->LowSpeedReadTime * 1), sizeof(BYTE));
												GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->Shapes[iop].Shape + 0x98,
													(void*)&FindResult->second->Shapes[iop].GeoType, sizeof(WORD));
											}
											if (!FindResult->second->Shapes[iop].FirstInitGeo)
											{
												GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->Shapes[iop].Shape + 0x98,
													(void*)&FindResult->second->Shapes[iop].GeoType, sizeof(WORD));
												GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->Shapes[iop].Shape + 0x98,
													(void*)&FindResult->second->Shapes[iop].CmpGeoType, sizeof(WORD));
												FindResult->second->Shapes[iop].FirstInitGeo = true;
											}
										}
									}
									if (FindResult->second->LowSpeedReadTime < 8)
									{
										FindResult->second->LowSpeedReadTime++;
									}
									else
									{
										FindResult->second->LowSpeedReadTime = 0;
									}
								}
							}
							FindResult->second->ReadTimeCount++;
							if (FindResult->second->ReadTimeCount >= 4)
							{
								GetDMA().queueScatterReadEx(ScatterHandle, FindResult->first + 0x30, &FindResult->second->CmpShapeTArray->Count, sizeof(WORD));
								FindResult->second->ReadTimeCount = 0;
							}
						}
						//GetDMA().queueScatterReadEx(ScatterHandle, FindResult->first + 0x28, &FindResult->second.CmpShapeTArray->Data, sizeof(ptr_t));
						//GetDMA().queueScatterReadEx(ScatterHandle, FindResult->first + 0x30, &FindResult->second.CmpShapeTArray->Count, sizeof(WORD));
					}
					std::unique_ptr<unique_writeguard<WfirstRWLock>> OutOfMaprlock(new unique_writeguard(OutOfMapLock));
					auto FindResult2 = OutOfMapWithLock.find(SceneActor[i]);
					if (FindResult2 != OutOfMapWithLock.end())
					{
						if (FindResult2->second->CmpShapeTArray->Data == 0)
							FindResult2->second->CmpShapeTArray = FindResult2->second->ShapeTArray;
						if (FindResult2->second->ReadTimeCount < 4)
						{
							GetDMA().queueScatterReadEx(ScatterHandle, FindResult2->first + 0x28 + FindResult2->second->ReadTimeCount * 2,
								(void*)((ptr_t)&FindResult2->second->CmpShapeTArray + FindResult2->second->ReadTimeCount * 2), sizeof(WORD));
							WORD NumOfShapes = (WORD)FindResult2->second->CmpShapeTArray->Count;
							if (NumOfShapes > 0 && NumOfShapes < 20)
							{
								if (FindResult2->second->Shapes.size() != NumOfShapes)
									FindResult2->second->Shapes.resize(NumOfShapes);
								if (NumOfShapes == 1)
								{
									if (FindResult2->second->Shapes[0].Shape == 0 || FindResult2->second->Shapes[0].Shape != (ptr_t)FindResult2->second->CmpShapeTArray.GetData())
									{
										FindResult2->second->Shapes[0].Shape = (ptr_t)FindResult2->second->CmpShapeTArray.GetData();
									}
									else
									{
										FindResult2->second->Shapes[0].Shape = (ptr_t)FindResult2->second->CmpShapeTArray.GetData();
										GetDMA().queueScatterReadEx(ScatterHandle, FindResult2->second->Shapes[0].Shape + 0x98,
											(void*)&FindResult2->second->Shapes[0].GeoType, sizeof(WORD));
										if (!FindResult2->second->Shapes[0].FirstInitGeo)
										{
											GetDMA().queueScatterReadEx(ScatterHandle, FindResult2->second->Shapes[0].Shape + 0x98,
												(void*)&FindResult2->second->Shapes[0].CmpGeoType, sizeof(WORD));
											FindResult2->second->Shapes[0].FirstInitGeo = true;
										}
									}
								}
								else
								{
									for (int iop = 0; iop < FindResult2->second->Shapes.size(); iop++)
									{
										if (!FindResult2->second->Shapes[iop].FirstInitPtr)
										{
											GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)FindResult2->second->CmpShapeTArray.GetData() + iop * 8,
												&FindResult2->second->Shapes[iop].Shape, sizeof(ptr_t));
											FindResult2->second->Shapes[iop].FirstInitPtr = true;
										}
										else
										{
											if (FindResult2->second->LowSpeedReadTime == iop)
											{
												GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)FindResult2->second->CmpShapeTArray.GetData() + iop * 8 + FindResult2->second->LowSpeedReadTime * 1,
													(void*)((ptr_t)&FindResult2->second->Shapes[iop].Shape + FindResult2->second->LowSpeedReadTime * 1), sizeof(WORD));
												GetDMA().queueScatterReadEx(ScatterHandle, FindResult2->second->Shapes[iop].Shape + 0x98,
													(void*)&FindResult2->second->Shapes[iop].GeoType, sizeof(WORD));
											}
											if (!FindResult2->second->Shapes[iop].FirstInitGeo)
											{
												GetDMA().queueScatterReadEx(ScatterHandle, FindResult2->second->Shapes[iop].Shape + 0x98,
													(void*)&FindResult2->second->Shapes[iop].GeoType, sizeof(WORD));
												GetDMA().queueScatterReadEx(ScatterHandle, FindResult2->second->Shapes[iop].Shape + 0x98,
													(void*)&FindResult2->second->Shapes[iop].CmpGeoType, sizeof(WORD));
												FindResult2->second->Shapes[iop].FirstInitGeo = true;
											}
										}
									}
									if (FindResult2->second->LowSpeedReadTime < 8)
									{
										FindResult2->second->LowSpeedReadTime++;
									}
									else
									{
										FindResult2->second->LowSpeedReadTime = 0;
									}
								}
							}
							FindResult2->second->ReadTimeCount++;
							if (FindResult2->second->ReadTimeCount >= 4)
							{
								GetDMA().queueScatterReadEx(ScatterHandle, FindResult2->first + 0x30, &FindResult2->second->CmpShapeTArray->Count, sizeof(WORD));
								FindResult2->second->ReadTimeCount = 0;
							}
						}
						//GetDMA().queueScatterReadEx(ScatterHandle, FindResult2->first + 0x28, &FindResult2->second.CmpShapeTArray->Data, sizeof(ptr_t));
						//GetDMA().queueScatterReadEx(ScatterHandle, FindResult2->first + 0x30, &FindResult2->second.CmpShapeTArray->Count, sizeof(WORD));
					}
					OutOfMaprlock.reset();
				}

				TotleReadByte += GetDMA().executeScatterRead2(ScatterHandle);
				//将当前存在于LowSpeedCache、OutOfMapWithLock的对象对比上一次的Shape结构如果有变更则从数组中移除
				for (int i = 0; i < SceneActor.size(); i++) {
					if (!IsAddrValid(SceneActor[i]))
						continue;
					if (SceneActor[i] == TestPtr)
						Sleep(0);
					auto FindResult = LowSpeedCache.find(SceneActor[i]);
					if (FindResult != LowSpeedCache.end())
					{
						if (FindResult->second->CmpShapeTArray.Data != FindResult->second->ShapeTArray.Data
							|| (WORD)FindResult->second->CmpShapeTArray.Count != (WORD)FindResult->second->ShapeTArray.Count)
						{
							LowSpeedCache.erase(FindResult);
						}
						else
						{
							bool bNeedEraseMap = false;
							for (int i = 0; i < FindResult->second->Shapes.size(); i++)
							{
								if (FindResult->second->Shapes[i].GeoType != FindResult->second->Shapes[i].CmpGeoType)
								{
									bNeedEraseMap = true;
									break;
								}
							}
							if (bNeedEraseMap)
							{
								LowSpeedCache.erase(FindResult);
								continue;
							}
							FindResult->second->CurrentReadTime = CurrentReadTime;
						}
					}
					std::unique_ptr<unique_writeguard<WfirstRWLock>> OutOfMaprlock(new unique_writeguard(OutOfMapLock));
					auto FindResult2 = OutOfMapWithLock.find(SceneActor[i]);
					if (FindResult2 != OutOfMapWithLock.end())
					{
						if (FindResult2->second->CmpShapeTArray->Data != FindResult2->second->ShapeTArray.Data
							|| (WORD)FindResult2->second->CmpShapeTArray.Count != (WORD)FindResult2->second->ShapeTArray.Count)
						{
							OutOfMapWithLock.erase(FindResult2);
						}
						else
						{
							bool bNeedEraseMap = false;
							for (int i = 0; i < FindResult2->second->Shapes.size(); i++)
							{
								if (FindResult2->second->Shapes[i].GeoType != FindResult2->second->Shapes[i].CmpGeoType)
								{
									bNeedEraseMap = true;
									break;
								}
							}
							if (bNeedEraseMap)
							{
								OutOfMapWithLock.erase(FindResult2);
								continue;
							}
							FindResult2->second->CurrentReadTime = CurrentReadTime;
						}
					}
					OutOfMaprlock.reset();
				}

				std::vector<ptr_t>ListUpdateDynamic;
				for (int i = 0; i < SceneActor.size(); i++) {
					if (!IsAddrValid(SceneActor[i]))
						continue;
					if (SceneActor[i] == TestPtr)
						Sleep(0);
					//将当前需要更新的对象加入更新列表
					//如果已经存在于RIGID_ARRAY则进行比较移除
					//不存在则读全部数据
					auto FindResult = RIGID_ARRAY.find(SceneActor[i]);
					if (FindResult != RIGID_ARRAY.end())
					{
						//如果是静态刚体
						if (FindResult->second->Type == 0)
						{
							////读取最新坐标
							//if (FindResult->second->ReadTimeCount < 1)
							//{
							//	FindResult->second->ReadTimeCount++;
							//	GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->TransformPtr + 0x10, &FindResult->second->Transform.p, sizeof(PxVec3));
							//}
							//else
							//{
							//	FindResult->second->ReadTimeCount = 0;
							//	GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->TransformPtr, &FindResult->second->Transform.q, sizeof(PxQuat));
							//	//GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second.TransformPtr, &FindResult->second.Transform, sizeof(PxTransform));
							//}
							GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->TransformPtr + 0x10, &FindResult->second->Transform.p, sizeof(PxVec3));


							//GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second.TransformPtr, &FindResult->second.Transform, sizeof(PxTransform));
							//FVector2D screen;
							//auto pos = PxVec3ToFVector(FindResult->second.Transform.p);
							//if (sdk::WorldToScreen(pos, screen))
							//{
							//	Sleep(0);
							//}
							//auto dissize = (pos - LocalPos).Size() / 100.f;
							//if (dissize <= 5)
							//{
							//	auto RigidActor = FindStaticRigidBody(FindResult->second.Point);
							//	if (!RigidActor)
							//		Sleep(0);
							//}
							//如果存在于已添加的场景
							auto RigidActor = FindStaticRigidBody(FindResult->first);
							if (IsRigidValid((ptr_t)RigidActor))
							{
								auto NumShapes = RigidActor->getNbShapes();
								std::vector<PxShape*>Shapes(NumShapes);
								RigidActor->getShapes(Shapes.data(), Shapes.size());
								for (auto Shape : Shapes)
								{
									if (!IsRigidValid((ptr_t)Shape))
										continue;
									auto ShapeMeshType = Shape->getGeometry().getType();
									if (ShapeMeshType != PxGeometryType::eHEIGHTFIELD)
									{
										ShapeUserData* ShapeUserdata = (ShapeUserData*)Shape->userData;
										if (Utils::IsSafeReadPtr(ShapeUserdata, 8))
											continue;
										if (ShapeUserdata->WriteTArray->Data == 0)
										{
											GetDMA().queueScatterReadEx(ScatterHandle, FindResult->first + 0x28, (void*)&ShapeUserdata->WriteTArray->Data, sizeof(TArray<ptr_t>));
										}
										else
										{
											if (ShapeUserdata->ReadCount < 4)
											{
												GetDMA().queueScatterReadEx(ScatterHandle, ShapeUserdata->SimulationFilterDataPtr + ShapeUserdata->ReadCount * 4, (void*)((ptr_t)&ShapeUserdata->SimulationFilterData + ShapeUserdata->ReadCount * 4), sizeof(int));
												GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->Point + 0x28 + ShapeUserdata->ReadCount * 2, (void*)((ptr_t)&ShapeUserdata->WriteTArray + ShapeUserdata->ReadCount * 2), 2);
												ShapeUserdata->ReadCount++;
												if (ShapeUserdata->ReadCount >= 4)
												{
													ShapeUserdata->ReadCount = 0;
													GetDMA().queueScatterReadEx(ScatterHandle, ShapeUserdata->ShapePtr + 0x98, &ShapeUserdata->ShapeType, sizeof(BYTE));
													GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->Point + 0x30, (void*)&ShapeUserdata->WriteTArray->Count, sizeof(WORD));
												}
											}
										}
									}
									else
									{
										ShapeUserDataHeightField* ShapeUserdata = (ShapeUserDataHeightField*)Shape->userData;
										if (Utils::IsSafeReadPtr(ShapeUserdata, 8))
											continue;
										if (!IsAddrValid(ShapeUserdata->ShapePtr) || !IsAddrValid(ShapeUserdata->RigidPoint))
											continue;
										GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->Point + 0x28, (void*)&ShapeUserdata->WriteTArray->Data, sizeof(ptr_t));
										GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->Point + 0x30, (void*)&ShapeUserdata->WriteTArray->Count, sizeof(WORD));
										//GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second.Point + 0x28, (void*)&ShapeUserdata->WriteTArray, sizeof(TArray<ptr_t>));
										if (HeightFieldUpdateArray.find(SceneActor[i]) == HeightFieldUpdateArray.end())
										{
											CurrentUpdateArray.emplace(SceneActor[i], std::make_shared<int>(i));
											continue;
										}
									}
								}
							}
							else
							{
								//不存在于已添加的场景则加入更新列表
								CurrentUpdateArray.emplace(SceneActor[i], std::make_shared<int>(i));
								//如果存在于慢速缓存则不更新数据
								if (LowSpeedCache.find(SceneActor[i]) != LowSpeedCache.end())
									SceneActor[i] = 0;
								continue;
							}
							//存在于场景添加入更新列表但不更新数据
							CurrentUpdateArray.emplace(SceneActor[i], std::make_shared<int>(i));
							SceneActor[i] = 0;
							continue;
						}
						else
						{
							//如果是动态刚体

							//GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second.TransformPtr, &FindResult->second.Transform, sizeof(PxTransform));
							//FVector2D screen;
							//auto pos = PxVec3ToFVector(FindResult->second.Transform.p);
							//if (sdk::WorldToScreen(pos, screen))
							//{
							//	Sleep(0);
							//}
							//auto dissize = (pos - LocalPos).Size() / 100.f;
							//if (dissize <= 5)
							//{
							//	auto RigidActor = FindDynamicRigidBody(FindResult->second.Point);
							//	if (!RigidActor)
							//		Sleep(0);
							//}

							//如果不存在于场景且不存在与排除数组
							if (!FindDynamicRigidBody(SceneActor[i]) && !FindOutOfMap(SceneActor[i]))
							{
								//读取坐标信息到RIGID_ARRAY数组存储中
								if (FindResult->second->ReadTimeCount < 1)
								{
									FindResult->second->ReadTimeCount++;
									GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->TransformPtr + 0x10, &FindResult->second->CaculdateTransform.p, sizeof(PxVec3));
									GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->DynamicBufferedBody2WorldPtr + 0x10, &FindResult->second->DynamicBufferedBody2World.p, sizeof(PxVec3));
								}
								else
								{
									FindResult->second->ReadTimeCount = 0;
									GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->TransformPtr, &FindResult->second->CaculdateTransform.q, sizeof(PxQuat));
									GetDMA().queueScatterReadEx(ScatterHandle, FindResult->second->DynamicBufferedBody2WorldPtr, &FindResult->second->DynamicBufferedBody2World.q, sizeof(PxQuat));
								}

								//添加到ListUpdateDynamic进行位置更新
								ListUpdateDynamic.push_back(SceneActor[i]);
								//添加到需要更新的场景
								CurrentUpdateArray.emplace(SceneActor[i], std::make_shared<int>(i));
								//如果存在于LowSpeedCache则不进行更新
								if (LowSpeedCache.find(SceneActor[i]) != LowSpeedCache.end())
									SceneActor[i] = 0;
							}
							else
							{
								//存在 则不进行更新
								SceneActor[i] = 0;
							}
							continue;
						}
					}
				}


				TotleReadByte += GetDMA().executeScatterRead2(ScatterHandle);

				for (auto iter : ListUpdateDynamic)
				{
					auto FindResult = RIGID_ARRAY.find(iter);
					if (FindResult != RIGID_ARRAY.end())
					{
						FindResult->second->Transform = CalulateDynamicGlobalPose2(FindResult->second->CaculdateTransform, FindResult->second->DynamicBufferedBody2World);
						Sleep(0);
					}
				}
				ListUpdateDynamic.clear();
			}



			auto ET = GetTickCount64();
			auto AllocateSize = SceneActor.size();
			if (AllocateSize < MaxUpdateRigid)
				AllocateSize = MaxUpdateRigid;
			if (SceneActorType.size() != AllocateSize)
				SceneActorType.resize(AllocateSize);
			if (ShapeManager.size() != AllocateSize)
				ShapeManager.resize(AllocateSize);
			if (ShapeArray.size() != AllocateSize)
				ShapeArray.resize(AllocateSize);
			if (StaticActorStreamPtr.size() != AllocateSize)
				StaticActorStreamPtr.resize(AllocateSize);
			if (ActorSceneTransformPtr.size() != AllocateSize)
				ActorSceneTransformPtr.resize(AllocateSize);
			if (ActorSceneTransform.size() != AllocateSize)
				ActorSceneTransform.resize(AllocateSize);
			if (StaticActorControlState.size() != AllocateSize)
				StaticActorControlState.resize(AllocateSize);
			if (DynamicBufferedBody2WorldPtr.size() != AllocateSize)
				DynamicBufferedBody2WorldPtr.resize(AllocateSize);
			if (DynamicBufferedBody2World.size() != AllocateSize)
				DynamicBufferedBody2World.resize(AllocateSize);
			if (DynamicBodyBufferFlags.size() != AllocateSize)
				DynamicBodyBufferFlags.resize(AllocateSize);
			if (g_ShapeControlState.size() != AllocateSize)
				g_ShapeControlState.resize(AllocateSize);
			if (g_ShapeGeometry.size() != AllocateSize)
				g_ShapeGeometry.resize(AllocateSize);
			if (g_ShapeTransformPtr.size() != AllocateSize)
				g_ShapeTransformPtr.resize(AllocateSize);
			if (g_ShapeTransform.size() != AllocateSize)
				g_ShapeTransform.resize(AllocateSize);
			if (g_ShapeGeometryHolderPtr.size() != AllocateSize)
				g_ShapeGeometryHolderPtr.resize(AllocateSize);
			if (g_ShapeGeometryHolder.size() != AllocateSize)
				g_ShapeGeometryHolder.resize(AllocateSize);
			if (g_ShapeFlags.size() != AllocateSize)
				g_ShapeFlags.resize(AllocateSize);
			if (g_SimulationFilterDataPtr.size() != AllocateSize)
				g_SimulationFilterDataPtr.resize(AllocateSize);
			if (g_SimulationFilterData.size() != AllocateSize)
				g_SimulationFilterData.resize(AllocateSize);
			if (g_GuTriangleMeshData.size() != AllocateSize)
				g_GuTriangleMeshData.resize(AllocateSize);
			if (g_TriangleMesh.size() != AllocateSize)
				g_TriangleMesh.resize(AllocateSize);
			if (g_GuHeightFieldData.size() != AllocateSize)
				g_GuHeightFieldData.resize(AllocateSize);
			if (g_HeightField.size() != AllocateSize)
				g_HeightField.resize(AllocateSize);
			AllocateSize = SceneActor.size();
			// 7=Static 6=Dynamic
			for (int i = 0; i < AllocateSize; i++) {
				if (!IsAddrValid(SceneActor[i]))
					continue;
				CurrentUpdateArray.emplace(SceneActor[i], std::make_shared<int>(i));
				GetDMA().queueScatterReadEx(ScatterHandle, SceneActor[i] + 8, &SceneActorType[i], 2);
				GetDMA().queueScatterReadEx(ScatterHandle, SceneActor[i] + 0x28, &ShapeManager[i], sizeof(TArray<ptr_t>));
			}
			TotleReadByte += GetDMA().executeScatterRead2(ScatterHandle);

			for (int i = 0; i < AllocateSize; i++) {
				if (!IsAddrValid(SceneActor[i]))
					continue;
				// Transform
				GetDMA().queueScatterReadEx(ScatterHandle, SceneActor[i] + 0x70, &StaticActorStreamPtr[i], 8);
				if (SceneActorType[i] == 7) {
					GetDMA().queueScatterReadEx(ScatterHandle, SceneActor[i] + 0x68, &StaticActorControlState[i], 1);
				}
				else if (SceneActorType[i] == 6) {
					DynamicBufferedBody2WorldPtr[i] = SceneActor[i] + 0x140;
					GetDMA().queueScatterReadEx(ScatterHandle, SceneActor[i] + 0x140, &DynamicBufferedBody2World[i], sizeof(PxTransform));
					GetDMA().queueScatterReadEx(ScatterHandle, SceneActor[i] + 0x17C, &DynamicBodyBufferFlags[i], 4);
				}
				// Shape
				WORD ResizeCount = (WORD)ShapeManager[i]->Count;
				if (IsAddrValid(ShapeManager[i]->Data) && ResizeCount < 500)
				{
					ShapeArray[i].resize(ResizeCount);
					g_ShapeControlState[i].resize(ResizeCount);
					g_ShapeGeometry[i].resize(ResizeCount);
					g_ShapeTransformPtr[i].resize(ResizeCount);
					g_ShapeTransform[i].resize(ResizeCount);
					g_ShapeGeometryHolderPtr[i].resize(ResizeCount);
					g_ShapeGeometryHolder[i].resize(ResizeCount);
					g_ShapeFlags[i].resize(ResizeCount);
					g_SimulationFilterDataPtr[i].resize(ResizeCount);
					g_SimulationFilterData[i].resize(ResizeCount);
					g_GuTriangleMeshData[i].resize(ResizeCount);
					g_TriangleMesh[i].resize(ResizeCount);
					g_GuHeightFieldData[i].resize(ResizeCount);
					g_HeightField[i].resize(ResizeCount);
					if (ResizeCount == 1) {
						ShapeArray[i][0] = (ptr_t)ShapeManager[i]->GetData();
					}
					else {
						for (int j = 0; j < ResizeCount; j++) {
							GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)ShapeManager[i]->GetData() + j * 8, &ShapeArray[i][j], 8);
						}
					}
				}
				else
				{
					SceneActor[i] = 0;
				}
			}
			TotleReadByte += GetDMA().executeScatterRead2(ScatterHandle);

			for (int i = 0; i < AllocateSize; i++) {
				if (!IsAddrValid(SceneActor[i]))
					continue;
				// Transform
				if (SceneActorType[i] == 7) {
					if (StaticActorControlState[i] != 0) {
						ActorSceneTransformPtr[i] = StaticActorStreamPtr[i] + 0xB0;
						GetDMA().queueScatterReadEx(ScatterHandle, StaticActorStreamPtr[i] + 0xB0, &ActorSceneTransform[i], sizeof(PxTransform));
					}
					else {
						ActorSceneTransformPtr[i] = SceneActor[i] + 0x90;
						GetDMA().queueScatterReadEx(ScatterHandle, SceneActor[i] + 0x90, &ActorSceneTransform[i], sizeof(PxTransform));
					}
				}
				else if (SceneActorType[i] == 6) {
					if ((DynamicBodyBufferFlags[i] & 0x200) != 0) {
						ActorSceneTransformPtr[i] = StaticActorStreamPtr[i] + 0xE0;
						GetDMA().queueScatterReadEx(ScatterHandle, StaticActorStreamPtr[i] + 0xE0, &ActorSceneTransform[i], sizeof(PxTransform));
					}
					else {
						ActorSceneTransformPtr[i] = SceneActor[i] + 0xB0;
						GetDMA().queueScatterReadEx(ScatterHandle, SceneActor[i] + 0xB0, &ActorSceneTransform[i], sizeof(PxTransform));
					}
				}
				// Shape
				for (int j = 0; j < ShapeArray[i].size(); j++) {
					if (!IsAddrValid(ShapeArray[i][j]))
						continue;
					GetDMA().queueScatterReadEx(ScatterHandle, ShapeArray[i][j] + 0x38, &g_ShapeControlState[i][j], 1);
					GetDMA().queueScatterReadEx(ScatterHandle, ShapeArray[i][j] + 0x40, &g_ShapeGeometry[i][j], 8);
				}
			}
			TotleReadByte += GetDMA().executeScatterRead2(ScatterHandle);

			for (int i = 0; i < AllocateSize; i++) {
				if (!IsAddrValid(SceneActor[i]))
					continue;
				// Transform
				if (SceneActorType[i] == 6) {
					ActorSceneTransform[i] = CalulateDynamicGlobalPose2(ActorSceneTransform[i], DynamicBufferedBody2World[i]);
				}
				// Shape
				for (int j = 0; j < ShapeArray[i].size(); j++) {
					if (!IsAddrValid(ShapeArray[i][j]))
						continue;
					if ((g_ShapeControlState[i][j] & 0x4) != 0)
					{
						g_ShapeTransformPtr[i][j] = g_ShapeGeometry[i][j];
						GetDMA().queueScatterReadEx(ScatterHandle, g_ShapeGeometry[i][j], &g_ShapeTransform[i][j], sizeof(PxTransform));
					}
					else
					{
						g_ShapeTransformPtr[i][j] = ShapeArray[i][j] + 0x70;
						GetDMA().queueScatterReadEx(ScatterHandle, ShapeArray[i][j] + 0x70, &g_ShapeTransform[i][j], sizeof(PxTransform));
					}
					if ((g_ShapeControlState[i][j] & 0x1) != 0)
					{
						g_ShapeGeometryHolderPtr[i][j] = g_ShapeGeometry[i][j] + 0x38;
						GetDMA().queueScatterReadEx(ScatterHandle, g_ShapeGeometry[i][j] + 0x38, &g_ShapeGeometryHolder[i][j], sizeof(PxGeometryHolder));
					}
					else
					{
						g_ShapeGeometryHolderPtr[i][j] = ShapeArray[i][j] + 0x98;
						GetDMA().queueScatterReadEx(ScatterHandle, ShapeArray[i][j] + 0x98, &g_ShapeGeometryHolder[i][j], sizeof(PxGeometryHolder));
					}
					if ((g_ShapeControlState[i][j] & 0x40) != 0)
						GetDMA().queueScatterReadEx(ScatterHandle, g_ShapeGeometry[i][j] + 0x34, &g_ShapeFlags[i][j], sizeof(PxShapeFlags));
					else
						GetDMA().queueScatterReadEx(ScatterHandle, ShapeArray[i][j] + 0x90, &g_ShapeFlags[i][j], sizeof(PxShapeFlags));
					if ((g_ShapeControlState[i][j] & 0x08) != 0)
					{
						g_SimulationFilterDataPtr[i][j] = g_ShapeGeometry[i][j] + 0x1C;
						GetDMA().queueScatterReadEx(ScatterHandle, g_ShapeGeometry[i][j] + 0x1C, &g_SimulationFilterData[i][j], sizeof(PxFilterData));
					}
					else
					{
						g_SimulationFilterDataPtr[i][j] = ShapeArray[i][j] + 0x60;
						GetDMA().queueScatterReadEx(ScatterHandle, ShapeArray[i][j] + 0x60, &g_SimulationFilterData[i][j], sizeof(PxFilterData));
					}

					//(*g_QueryFilterDataPtr)[i][j] = ShapeArray[i][j] + 0x50;
					//GetDMA().queueScatterReadEx(ScatterHandle, ShapeArray[i][j] + 0x50, &(*g_QueryFilterData)[i][j], sizeof(PxFilterData));
				}
			}
			TotleReadByte += GetDMA().executeScatterRead2(ScatterHandle);

			for (int i = 0; i < AllocateSize; i++) {
				if (!IsAddrValid(SceneActor[i]))
					continue;
				// Shape
				for (int j = 0; j < ShapeArray[i].size(); j++) {
					if (!IsAddrValid(ShapeArray[i][j]))
						continue;
					auto ShapeGeometryType = g_ShapeGeometryHolder[i][j].getType();
					if (ShapeGeometryType == PxGeometryType::eTRIANGLEMESH)
					{
						PxTriangleMeshGeometry triangleMesh = g_ShapeGeometryHolder[i][j].triangleMesh();
						if (!IsAddrValid(triangleMesh.triangleMesh))
							continue;
						ptr_t GutriangleMesh = (ptr_t)triangleMesh.triangleMesh;
						GetDMA().queueScatterReadEx(ScatterHandle, GutriangleMesh + 0x20, &g_GuTriangleMeshData[i][j], sizeof(GuTriangleMeshDataStruct) - 1);
						GetDMA().queueScatterReadEx(ScatterHandle, GutriangleMesh + 0x5C, &g_GuTriangleMeshData[i][j].mTriangleMeshFlags, 1);
						continue;
					}
					else if (ShapeGeometryType == PxGeometryType::eHEIGHTFIELD)
					{
						PxHeightFieldGeometry heightField = g_ShapeGeometryHolder[i][j].heightField();
						if (!IsAddrValid(heightField.heightField))
							continue;
						auto Type = SceneActorType[i];
						ptr_t GuheightField = (ptr_t)heightField.heightField;
						GetDMA().queueScatterReadEx(ScatterHandle, GuheightField + 0x38, &g_GuHeightFieldData[i][j], sizeof(GuHeightFieldDataStruct));
					}
				}
			}
			TotleReadByte += GetDMA().executeScatterRead2(ScatterHandle);

			for (int i = 0; i < AllocateSize; i++) {
				if (!IsAddrValid(SceneActor[i]))
					continue;
				// Shape
				for (int j = 0; j < ShapeArray[i].size(); j++) {
					if (!IsAddrValid(ShapeArray[i][j]))
						continue;
					auto ShapeGeometryType = g_ShapeGeometryHolder[i][j].getType();
					if (ShapeGeometryType == PxGeometryType::eTRIANGLEMESH)
					{
						PxTriangleMeshGeometry triangleMesh = g_ShapeGeometryHolder[i][j].triangleMesh();
						if (!IsAddrValid(triangleMesh.triangleMesh))
							continue;
						ptr_t GutriangleMesh = (ptr_t)triangleMesh.triangleMesh;
						auto TraingleMeshData = g_GuTriangleMeshData[i][j];
						if (TraingleMeshData.mTriangleMeshFlags & 2)
						{
							GetDMA().queueScatterReadEx(ScatterHandle, TraingleMeshData.mPolygons, &g_TriangleMesh[i][j].FirstV, sizeof(short));
							GetDMA().queueScatterReadEx(ScatterHandle, TraingleMeshData.mPolygons + 2, &g_TriangleMesh[i][j].FirstV2, sizeof(short));
							GetDMA().queueScatterReadEx(ScatterHandle, TraingleMeshData.mPolygons + 4, &g_TriangleMesh[i][j].FirstV3, sizeof(short));
						}
						else
						{
							GetDMA().queueScatterReadEx(ScatterHandle, TraingleMeshData.mPolygons, &g_TriangleMesh[i][j].FirstV, sizeof(int));
							GetDMA().queueScatterReadEx(ScatterHandle, TraingleMeshData.mPolygons + 4, &g_TriangleMesh[i][j].FirstV2, sizeof(int));
							GetDMA().queueScatterReadEx(ScatterHandle, TraingleMeshData.mPolygons + 8, &g_TriangleMesh[i][j].FirstV3, sizeof(int));
						}
						continue;
					}
					else if (ShapeGeometryType == PxGeometryType::eHEIGHTFIELD)
					{
						PxHeightFieldGeometry heightField = g_ShapeGeometryHolder[i][j].heightField();
						if (!IsAddrValid(heightField.heightField))
							continue;
						auto Type = SceneActorType[i];
						ptr_t GuheightField = (ptr_t)heightField.heightField;
						auto HeightFieldData = g_GuHeightFieldData[i][j];
						g_HeightField[i][j].HeightData.resize(9);
						GetDMA().queueScatterReadEx(ScatterHandle, HeightFieldData.Samples, g_HeightField[i][j].HeightData.data(), 9 * sizeof(int));
					}
				}
			}
			TotleReadByte += GetDMA().executeScatterRead2(ScatterHandle);
			for (int i = 0; i < AllocateSize; i++) {
				if (!IsAddrValid(SceneActor[i]))
					continue;
				// Shape
				for (int j = 0; j < ShapeArray[i].size(); j++) {
					if (!IsAddrValid(ShapeArray[i][j]))
						continue;
					auto ShapeGeometryType = g_ShapeGeometryHolder[i][j].getType();
					if (ShapeGeometryType == PxGeometryType::eTRIANGLEMESH)
					{
						PxTriangleMeshGeometry triangleMesh = g_ShapeGeometryHolder[i][j].triangleMesh();
						if (!IsAddrValid(triangleMesh.triangleMesh))
							continue;
						ptr_t GutriangleMesh = (ptr_t)triangleMesh.triangleMesh;
						auto TraingleMeshData = g_GuTriangleMeshData[i][j];
						GetDMA().queueScatterReadEx(ScatterHandle, TraingleMeshData.mVertices + g_TriangleMesh[i][j].FirstV, &g_TriangleMesh[i][j].FirstF, sizeof(float));
						GetDMA().queueScatterReadEx(ScatterHandle, TraingleMeshData.mVertices + g_TriangleMesh[i][j].FirstV2, &g_TriangleMesh[i][j].FirstF2, sizeof(float));
						GetDMA().queueScatterReadEx(ScatterHandle, TraingleMeshData.mVertices + g_TriangleMesh[i][j].FirstV3, &g_TriangleMesh[i][j].FirstF3, sizeof(float));
						//GetDMA().queueScatterReadEx(ScatterHandle, TraingleMeshData.mVertices + g_TriangleMesh[i][j].FirstV * sizeof(PxVec3), &g_TriangleMesh[i][j].FirstF, sizeof(float));
						//GetDMA().queueScatterReadEx(ScatterHandle, TraingleMeshData.mVertices + g_TriangleMesh[i][j].FirstV2 * sizeof(PxVec3), &g_TriangleMesh[i][j].FirstF2, sizeof(float));
						//GetDMA().queueScatterReadEx(ScatterHandle, TraingleMeshData.mVertices + g_TriangleMesh[i][j].FirstV3 * sizeof(PxVec3), &g_TriangleMesh[i][j].FirstF3, sizeof(float));
						continue;
					}

				}
			}
			TotleReadByte += GetDMA().executeScatterRead2(ScatterHandle);

			for (int i = 0; i < AllocateSize; i++) {
				if (!IsAddrValid(SceneActor[i]))
					continue;
				// Shape
				for (int j = 0; j < ShapeArray[i].size(); j++) {
					if (!IsAddrValid(ShapeArray[i][j]))
						continue;
					auto ShapeGeometryType = g_ShapeGeometryHolder[i][j].getType();
					if (ShapeGeometryType == PxGeometryType::eTRIANGLEMESH)
					{
						PxTriangleMeshGeometry triangleMesh = g_ShapeGeometryHolder[i][j].triangleMesh();
						if (!IsAddrValid(triangleMesh.triangleMesh))
							continue;
						ptr_t GutriangleMesh = (ptr_t)triangleMesh.triangleMesh;
						GuTriangleMeshDataStruct Data = g_GuTriangleMeshData[i][j];
						g_TriangleMesh[i][j].mPolygons = Data.mPolygons;
						g_TriangleMesh[i][j].mVertices = Data.mVertices;
						g_TriangleMesh[i][j].mNbVertices = Data.mNbVertices;
						g_TriangleMesh[i][j].triangleCount = Data.triangleCount;
						g_TriangleMesh[i][j].mTriangleMeshFlags = Data.mTriangleMeshFlags;
						g_TriangleMesh[i][j].NbVerticesPtr = GutriangleMesh + 0x20;
						g_TriangleMesh[i][j].NbtrianglePtr = GutriangleMesh + 0x24;
						g_TriangleMesh[i][j].TriangleMeshPtr = GutriangleMesh;
						if (SceneActorType[i] == 7)
						{
							auto Scale = triangleMesh.scale;
							auto flags = triangleMesh.meshFlags;

							float FirstF = g_TriangleMesh[i][j].FirstF;
							if (!isValidFloat(FirstF))
							{
								FirstF = g_TriangleMesh[i][j].FirstF2;
								if (!isValidFloat(FirstF))
								{
									FirstF = g_TriangleMesh[i][j].FirstF3;
									if (!isValidFloat(FirstF))
										break;
								}
							}
							int FirstV = g_TriangleMesh[i][j].FirstV;
							if (FirstV < 0 || FirstV >= Data.mNbVertices)
								continue;
							constexpr size_t BUFFER_SIZE = 256;	
							char buffer[BUFFER_SIZE];
							ZeroMemory(buffer, BUFFER_SIZE);
							// 使用sprintf_s进行字符串格式化
							int ret = sprintf_s(buffer, BUFFER_SIZE, "%s_%u_%u_%u_%d_%f",
								CurrentMapName.c_str(),
								g_SimulationFilterData[i][j].word3,
								Data.triangleCount,
								Data.mNbVertices,
								FirstV,
								FirstF);
							std::string FileNameLocation = std::string(buffer);
							auto hash_file = hash_(FileNameLocation);
							auto triangleMesh = GetAlreadyLoadedTriangle(hash_file);
							if (triangleMesh && IsValidPtr(triangleMesh->first) && IsValidPtr(triangleMesh->second))
								continue;
							if (Vars.Menu.UseCacheToTrace)
							{
								std::string FileNamePath = Path + CurrentMapName + "\\" + std::to_string(hash_file) + ".dat";
								//std::string FileNamePath = Path + CurrentMapName + "\\" + FileNameLocation + ".dat";
								if (fileExists(FileNamePath))
									continue;
							}
						}
						else
						{
							auto UpdateType = FilterDynamicRigid(g_SimulationFilterData[i][j], Data.mNbVertices);
							if (UpdateType == RigidFreshType::RNotAdd)
								continue;
							auto Scale = triangleMesh.scale;
							auto flags = triangleMesh.meshFlags;

							float FirstF = g_TriangleMesh[i][j].FirstF;
							if (!isValidFloat(FirstF))
							{
								FirstF = g_TriangleMesh[i][j].FirstF2;
								if (!isValidFloat(FirstF))
								{
									FirstF = g_TriangleMesh[i][j].FirstF3;
									if (!isValidFloat(FirstF))
										break;
								}
							}
							int FirstV = g_TriangleMesh[i][j].FirstV;
							if (FirstV < 0 || FirstV >= Data.mNbVertices)
								continue;
							constexpr size_t BUFFER_SIZE = 256;
							char buffer[BUFFER_SIZE];
							ZeroMemory(buffer, BUFFER_SIZE);
							// 使用sprintf_s进行字符串格式化
							int ret = sprintf_s(buffer, BUFFER_SIZE, "%sDM_%u_%u_%u_%d_%f",
								CurrentMapName.c_str(),
								g_SimulationFilterData[i][j].word3,
								Data.triangleCount,
								Data.mNbVertices,
								FirstV,
								FirstF);
							std::string	FileNameLocation = std::string(buffer);
							auto hash_file = hash_(FileNameLocation);
							auto triangleMesh = GetAlreadyLoadedTriangle(hash_file);
							if (triangleMesh && IsValidPtr(triangleMesh->first) && IsValidPtr(triangleMesh->second))
								continue;
							if (Vars.Menu.UseCacheToTrace)
							{
								std::string FileNamePath = Path + CurrentMapName + "\\" + std::to_string(hash_file) + ".dat";
								//std::string FileNamePath = Path + CurrentMapName + "\\" + FileNameLocation + ".dat";
								if (fileExists(FileNamePath))
									continue;
							}
							if (FindOutOfMap(SceneActor[i]))
								continue;
						}

						g_TriangleMesh[i][j].Vertices.resize(Data.mNbVertices);
						GetDMA().queueScatterReadEx(ScatterHandle, Data.mVertices, g_TriangleMesh[i][j].Vertices.data(), Data.mNbVertices * sizeof(PxVec3));



						if (Data.mTriangleMeshFlags & 2) {
							g_TriangleMesh[i][j].Indices16.resize(Data.triangleCount * 3);
							GetDMA().queueScatterReadEx(ScatterHandle, Data.mPolygons, g_TriangleMesh[i][j].Indices16.data(), Data.triangleCount * 3 * sizeof(PxU16));
						}
						else {
							g_TriangleMesh[i][j].Indices32.resize(Data.triangleCount * 3);
							GetDMA().queueScatterReadEx(ScatterHandle, Data.mPolygons, g_TriangleMesh[i][j].Indices32.data(), Data.triangleCount * 3 * sizeof(PxU32));
						}
					}
					else if (ShapeGeometryType == PxGeometryType::eHEIGHTFIELD)
					{
						PxHeightFieldGeometry heightField = g_ShapeGeometryHolder[i][j].heightField();
						if (!IsAddrValid(heightField.heightField))
							continue;
						ptr_t GuheightField = (ptr_t)heightField.heightField;
						GuHeightFieldDataStruct Data = g_GuHeightFieldData[i][j];
						if (Data.NumRows >= 2000 || Data.NumCols >= 2000)
							continue;
						g_HeightField[i][j].NumRows = Data.NumRows;
						g_HeightField[i][j].NumCols = Data.NumCols;
						g_HeightField[i][j].rowLimit = Data.rowLimit;
						g_HeightField[i][j].colLimit = Data.colLimit;
						g_HeightField[i][j].nbColumns = Data.nbColumns;
						g_HeightField[i][j].unknown = Data.UnknownData;
						g_HeightField[i][j].HeightField = GuheightField;
						g_HeightField[i][j].HolderPtr = g_ShapeGeometryHolderPtr[i][j];
						g_HeightField[i][j].Samples = Data.Samples;
						g_HeightField[i][j].RowScale = heightField.rowScale;
						g_HeightField[i][j].ColScale = heightField.columnScale;
						g_HeightField[i][j].HeightScale = heightField.heightScale;
						g_HeightField[i][j].Flags = heightField.heightFieldFlags;

						if (SceneActorType[i] == 7)
						{
							constexpr size_t BUFFER_SIZE = 256;
							char buffer[BUFFER_SIZE];
							int* HeightData = (int*)g_HeightField[i][j].HeightData.data();
							// 使用sprintf_s进行字符串格式化
							int ret = sprintf_s(buffer, BUFFER_SIZE, "%u_%u_%.0f_%.0f_%.0f_%.0f_%.0f_%.0f_%d_%d_%d_%d_%d_%d_%d_%d_%d",
								Data.NumRows,
								static_cast<PxU32>(Data.NumCols),
								Data.rowLimit,
								Data.colLimit,
								Data.nbColumns,
								heightField.heightScale,
								heightField.rowScale,
								heightField.columnScale,
								HeightData[0],
								HeightData[1],
								HeightData[2],
								HeightData[3],
								HeightData[4],
								HeightData[5],
								HeightData[6],
								HeightData[7],
								HeightData[8]);
							std::string FileNameLocation(buffer);
							auto hash_file = hash_(FileNameLocation);
							g_HeightField[i][j].HeightFieldName = FileNameLocation;

							auto heightField = GetAlreadyLoadedHeightField(hash_file);
							if (heightField && IsValidPtr(heightField->first) && IsValidPtr(heightField->second))
								continue;
							if (Vars.Menu.UseCacheToTrace)
							{
								std::string FileNamePath = Path + CurrentMapName + "\\HF" + std::to_string(hash_file) + ".dat";
								if (fileExists(FileNamePath))
								{
									*heightField = loadHeightFieldFromFile(FileNamePath.c_str());
									if (!Utils::IsSafeReadPtr(heightField->first, 8))
									{
										if (!hasMoreThanTenPercentZeroHeights(heightField->first)) {
											free(heightField->second);
											continue;
										}
										else
										{
											free(heightField->second);
											if (std::remove(FileNamePath.c_str()) == 0) {
												CONSOLE_INFO("Remove An InValid Module Cache : %s", FileNamePath.c_str());
											}
										}
									}
									else
									{
										if (std::remove(FileNamePath.c_str()) == 0) {
											CONSOLE_INFO("Remove An InValid Module Cache : %s", FileNamePath.c_str());
										}
									}
								}
							}
						}
						g_HeightField[i][j].HeightData.resize(Data.NumRows * Data.NumCols);
						GetDMA().queueScatterReadEx(ScatterHandle, Data.Samples, g_HeightField[i][j].HeightData.data(), Data.NumRows * Data.NumCols * sizeof(PxHeightFieldSample));
					}
				}
			}
			TotleReadByte += GetDMA().executeScatterRead(ScatterHandle);

			for (int i = 0; i < AllocateSize; i++)
			{
				if (!IsAddrValid(SceneActor[i]))
					continue;
				if (SceneActor[i] == TestPtr)
					Sleep(0);
				RigiBodyStruct NeedPush;
				auto FindResult = RIGID_ARRAY.find(SceneActor[i]);
				if (FindResult == RIGID_ARRAY.end())
				{
					NeedPush = RigiBodyStruct(SceneActorType[i] == 7 ? 0 : 1,
						SceneActor[i], ActorSceneTransformPtr[i], ActorSceneTransform[i], DynamicBufferedBody2WorldPtr[i],
						DynamicBufferedBody2World[i], ShapeManager[i], 0, CurrentReadTime);
					NeedPush.Shapes.resize(ShapeArray[i].size());
					int loopCount = 0;
					for (int j = 0; j < NeedPush.Shapes.size();)
					{
						if (g_SimulationFilterData[i][loopCount] == PxFilterData())
						{
							NeedPush.Shapes.erase(NeedPush.Shapes.begin() + j);
							loopCount++;
							continue;
						}
						auto GeoType = g_ShapeGeometryHolder[i][loopCount].any().getType();
						if ((GeoType == PxGeometryType::eTRIANGLEMESH && g_TriangleMesh[i][loopCount].mNbVertices && g_TriangleMesh[i][loopCount].triangleCount)
							|| (GeoType == PxGeometryType::eHEIGHTFIELD && g_HeightField[i][loopCount].colLimit && g_HeightField[i][loopCount].ColScale
								&& g_HeightField[i][loopCount].HeightScale && g_HeightField[i][loopCount].nbColumns && g_HeightField[i][loopCount].NumCols
								&& g_HeightField[i][loopCount].rowLimit && g_HeightField[i][loopCount].RowScale && g_HeightField[i][loopCount].Samples)
							|| (hash_(CurrentMapName) == "Savage_Main"_hash && GeoType == PxGeometryType::eCAPSULE && SceneActorType[i] == 7))
						{
							//if (GeoType == PxGeometryType::eCAPSULE && SceneActorType[i] == 7)
							//{
							//	auto dissize = (PxVec3ToFVector(ActorSceneTransform[i].p) - LocalPos).Size() / 100.f;
							//	if (dissize <= 5)
							//	{
							//		auto RigidActor = FindStaticRigidBody(FindResult->second.Point);
							//		if (!RigidActor)
							//			Sleep(0);
							//	}
							//}
							NeedPush.Shapes[j].Shape = ShapeArray[i][loopCount];
							NeedPush.Shapes[j].ShapeTransformPtr = g_ShapeTransformPtr[i][loopCount];
							NeedPush.Shapes[j].ShapeTransform = g_ShapeTransform[i][loopCount];
							NeedPush.Shapes[j].ShapeFlags = g_ShapeFlags[i][loopCount];
							NeedPush.Shapes[j].ShapeGeometryPtr = g_ShapeGeometryHolderPtr[i][loopCount];
							NeedPush.Shapes[j].ShapeGeometry = g_ShapeGeometryHolder[i][loopCount];
							NeedPush.Shapes[j].SimulationFilterDataPtr = g_SimulationFilterDataPtr[i][loopCount];
							NeedPush.Shapes[j].SimulationFilterData = g_SimulationFilterData[i][loopCount];
							NeedPush.Shapes[j].TriangleMesh = g_TriangleMesh[i][loopCount];
							NeedPush.Shapes[j].HeightField = g_HeightField[i][loopCount];
							if (NeedPush.Shapes[j].HeightField.Samples)
							{
								HeightFieldUpdateArray.emplace(SceneActor[i], std::make_shared<RigiBodyStruct>(NeedPush));
								NeedPush.MeshType = 1;
							}
							j++;
							loopCount++;
						}
						else
						{
							NeedPush.Shapes.erase(NeedPush.Shapes.begin() + j);
							loopCount++;
						}
					}
					//不存在则添加
					if (FindOutOfMap(SceneActor[i]))
						EraseOutOfMap(SceneActor[i]);
					RIGID_ARRAY.emplace(SceneActor[i], std::make_shared<RigiBodyStruct>(NeedPush));
				}
				else
				{
					//存在则更新其坐标形态等数据
					FindResult->second->Transform = ActorSceneTransform[i];
					FindResult->second->Shapes.resize(ShapeArray[i].size());
					int loopCount = 0;
					for (int j = 0; j < FindResult->second->Shapes.size();)
					{
						if (g_SimulationFilterData[i][loopCount] == PxFilterData())
						{
							FindResult->second->Shapes.erase(FindResult->second->Shapes.begin() + j);
							loopCount++;
							continue;
						}
						auto GeoType = g_ShapeGeometryHolder[i][loopCount].any().getType();
						if ((GeoType == PxGeometryType::eTRIANGLEMESH && g_TriangleMesh[i][loopCount].mNbVertices && g_TriangleMesh[i][loopCount].triangleCount)
							|| (GeoType == PxGeometryType::eHEIGHTFIELD && g_HeightField[i][loopCount].colLimit && g_HeightField[i][loopCount].ColScale
								&& g_HeightField[i][loopCount].HeightScale && g_HeightField[i][loopCount].nbColumns && g_HeightField[i][loopCount].NumCols
								&& g_HeightField[i][loopCount].rowLimit && g_HeightField[i][loopCount].RowScale && g_HeightField[i][loopCount].Samples)
							|| (hash_(CurrentMapName) == "Savage_Main"_hash && GeoType == PxGeometryType::eCAPSULE && SceneActorType[i] == 7))
						{
							FindResult->second->Shapes[j].Shape = ShapeArray[i][loopCount];
							FindResult->second->Shapes[j].ShapeTransformPtr = g_ShapeTransformPtr[i][loopCount];
							FindResult->second->Shapes[j].ShapeTransform = g_ShapeTransform[i][loopCount];
							FindResult->second->Shapes[j].ShapeFlags = g_ShapeFlags[i][loopCount];
							FindResult->second->Shapes[j].ShapeGeometryPtr = g_ShapeGeometryHolderPtr[i][loopCount];
							FindResult->second->Shapes[j].ShapeGeometry = g_ShapeGeometryHolder[i][loopCount];
							FindResult->second->Shapes[j].SimulationFilterDataPtr = g_SimulationFilterDataPtr[i][loopCount];
							FindResult->second->Shapes[j].SimulationFilterData = g_SimulationFilterData[i][loopCount];
							FindResult->second->Shapes[j].TriangleMesh = g_TriangleMesh[i][loopCount];
							FindResult->second->Shapes[j].HeightField = g_HeightField[i][loopCount];
							if (FindResult->second->Shapes[j].HeightField.Samples)
							{
								HeightFieldUpdateArray.emplace(SceneActor[i], std::make_shared<RigiBodyStruct>(*FindResult->second));
								FindResult->second->MeshType = 1;
							}
							j++;
							loopCount++;
						}
						else
						{
							FindResult->second->Shapes.erase(FindResult->second->Shapes.begin() + j);
							loopCount++;
						}
					}
					LowSpeedCache.emplace(FindResult->first, std::make_shared<RigiBodyStruct>(*FindResult->second));
				}
			}

			try
			{
				AddRigidBodiesToScene(RIGID_ARRAY, CurrentUpdateArray, NeedClearRigid);
			}
			catch (...)
			{
				CONSOLE_INFO2("AddRigidBodiesToScene Exception!");
				return;
			}
			if (!bSceneCreate)
			{
				if (!FirstLoadAllDataSet)
				{
					FirstLoadAllDataSet = true;
				}
				bSceneCreate = true;
			}
			else
			{
				if (RunTimeStart != 10)
				{
					RunTimeStart++;
				}
				else
				{
					if (RunTimeStart == 10)
					{
						RunTimeStart++;
						bRayCastValid = true;
					}
				}
			}
			//CONSOLE_INFO("【%d|%d】Run Update CastTime:%d|%d", CurrentLoopCount, RigidLoopCount, GetTickCount64() - StartTime, TotleReadByte);

			if (SceneActorType.size() != MaxUpdateRigid)
				SceneActorType.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				SceneActorType[i] = 0;

			if (ShapeManager.size() != MaxUpdateRigid)
				ShapeManager.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				ShapeManager[i] = 0;

			if (StaticActorStreamPtr.size() != MaxUpdateRigid)
				StaticActorStreamPtr.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				StaticActorStreamPtr[i] = 0;

			if (ActorSceneTransformPtr.size() != MaxUpdateRigid)
				ActorSceneTransformPtr.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				ActorSceneTransformPtr[i] = 0;

			if (ActorSceneTransform.size() != MaxUpdateRigid)
				ActorSceneTransform.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				ZeroMemory(&ActorSceneTransform[i], sizeof(PxTransform));

			if (StaticActorControlState.size() != MaxUpdateRigid)
				StaticActorControlState.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				StaticActorControlState[i] = 0;

			if (DynamicBufferedBody2WorldPtr.size() != MaxUpdateRigid)
				DynamicBufferedBody2WorldPtr.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				DynamicBufferedBody2WorldPtr[i] = 0;

			if (DynamicBufferedBody2World.size() != MaxUpdateRigid)
				DynamicBufferedBody2World.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				ZeroMemory(&DynamicBufferedBody2World[i], sizeof(PxTransform));

			if (DynamicBodyBufferFlags.size() != MaxUpdateRigid)
				DynamicBodyBufferFlags.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				DynamicBodyBufferFlags[i] = 0;

			if (ShapeArray.size() != MaxUpdateRigid)
				ShapeArray.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				if (ShapeArray[i].size())
					ShapeArray[i].clear();

			if (g_ShapeControlState.size() != MaxUpdateRigid)
				g_ShapeControlState.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				if (g_ShapeControlState[i].size())
					g_ShapeControlState[i].clear();

			if (g_ShapeGeometry.size() != MaxUpdateRigid)
				g_ShapeGeometry.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				if (g_ShapeGeometry[i].size())
					g_ShapeGeometry[i].clear();

			if (g_ShapeTransformPtr.size() != MaxUpdateRigid)
				g_ShapeTransformPtr.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				if (g_ShapeTransformPtr[i].size())
					g_ShapeTransformPtr[i].clear();

			if (g_ShapeTransform.size() != MaxUpdateRigid)
				g_ShapeTransform.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				if (g_ShapeTransform[i].size())
					g_ShapeTransform[i].clear();

			if (g_ShapeGeometryHolderPtr.size() != MaxUpdateRigid)
				g_ShapeGeometryHolderPtr.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				if (g_ShapeGeometryHolderPtr[i].size())
					g_ShapeGeometryHolderPtr[i].clear();

			if (g_ShapeGeometryHolder.size() != MaxUpdateRigid)
				g_ShapeGeometryHolder.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				if (g_ShapeGeometryHolder[i].size())
					g_ShapeGeometryHolder[i].clear();

			if (g_ShapeFlags.size() != MaxUpdateRigid)
				g_ShapeFlags.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				if (g_ShapeFlags[i].size())
					g_ShapeFlags[i].clear();

			if (g_SimulationFilterDataPtr.size() != MaxUpdateRigid)
				g_SimulationFilterDataPtr.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				if (g_SimulationFilterDataPtr[i].size())
					g_SimulationFilterDataPtr[i].clear();

			if (g_SimulationFilterData.size() != MaxUpdateRigid)
				g_SimulationFilterData.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				if (g_SimulationFilterData[i].size())
					g_SimulationFilterData[i].clear();

			if (g_GuTriangleMeshData.size() != MaxUpdateRigid)
				g_GuTriangleMeshData.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				if (g_GuTriangleMeshData[i].size())
					g_GuTriangleMeshData[i].clear();

			if (g_TriangleMesh.size() != MaxUpdateRigid)
				g_TriangleMesh.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				if (g_TriangleMesh[i].size())
					g_TriangleMesh[i].clear();

			if (g_GuHeightFieldData.size() != MaxUpdateRigid)
				g_GuHeightFieldData.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				if (g_GuHeightFieldData[i].size())
					g_GuHeightFieldData[i].clear();

			if (g_HeightField.size() != MaxUpdateRigid)
				g_HeightField.resize(MaxUpdateRigid);
			for (int i = 0; i < MaxUpdateRigid; i++)
				if (g_HeightField[i].size())
					g_HeightField[i].clear();

#ifdef pvdConnect
			if (gScene)
			{
				gScene->simulate(1.0f / 60.0f);
				gScene->fetchResults(true);
			}
#endif
			return;
		}
	}
	//启动的逻辑线程
	void PhysicsManager::asyncScenes()
	{
		SyncThreadCreated = true;
		auto ScatterHandle = GetDMA().createScatterHandle();
		using framerate = std::chrono::duration<int, std::ratio<1, 20>>;
		auto tp = std::chrono::system_clock::now() + framerate{ 1 };
		do
		{
			try
			{
				int count = 0;
				auto start = std::chrono::high_resolution_clock::now();
				auto end = start + std::chrono::seconds(1);
				while (std::chrono::high_resolution_clock::now() < end) {
					if (!sdk::InGame() || !Vars.Menu.PhysxTrace)
					{
						if (SyncScene != 0)
						{
							DestroyAllRigidBodies();
						}
						Sleep(2000);
						continue;
					}
					if (Vars.Menu.FreshTrace != 27 && OverlayEngine::WasKeyPressed(Vars.Menu.FreshTrace))
					{
						if (SyncScene != 0)
						{
							DestroyAllRigidBodies();
						}
						Sleep(2000);
					}
					UpdateRigidData(ScatterHandle);
					count++;
					std::this_thread::sleep_until(tp);
					tp += framerate{ 1 };
				}
			}
			catch (...)
			{
				CONSOLE_INFO("asyncScenes Exception!");
			}

		} while (true);
		return;
	}

	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//线程2
	//更新玩家坐标形态的坐标
	void PhysicsManager::setActorGlobalPoseInternal(PxRigidDynamic* RigidActor, PxTransform Transform, float radiu, float halfhigh)
	{
		if (!IsRigidValid((ptr_t)RigidActor))
			return;
		RigidActor->setGlobalPose(Transform);
		PxU32 shapeCount = RigidActor->getNbShapes();
		std::vector<PxShape*> shapes(shapeCount);
		RigidActor->getShapes(shapes.data(), shapeCount);
		for (PxShape* shape : shapes)
		{
			if (!IsRigidValid((ptr_t)shape))
				continue;
			auto Type = shape->getGeometry().getType();
			if (Type == PxGeometryType::eCAPSULE)
			{
				PxCapsuleGeometry capsuleGeometry;
				shape->getCapsuleGeometry(capsuleGeometry);
				capsuleGeometry.radius = radiu;
				capsuleGeometry.halfHeight = halfhigh;
				shape->setGeometry(capsuleGeometry);
				//PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
				//shape->setLocalPose(relativePose);
				//PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
				//PxShape* aCapsuleShape = PxRigidActorExt::createExclusiveShape(*aCapsuleActor,
				//	PxCapsuleGeometry(radius, halfHeight), aMaterial);
				//aCapsuleShape->setLocalPose(relativePose);
				//PxRigidBodyExt::updateMassAndInertia(*aCapsuleActor, capsuleDensity);
				//shape->setLocalPose(PxTransform(PxVec3(2, 2, 2), Transform.q));
			}
			else if (Type == PxGeometryType::eSPHERE)
			{
				PxSphereGeometry sphererGeometry;
				shape->getSphereGeometry(sphererGeometry);
				sphererGeometry.radius = radiu;
				shape->setGeometry(sphererGeometry);
				shape->setLocalPose(PxTransform(PxVec3(0, 0, 0), Transform.q));
			}
		}
	}
	//添加、设置玩家形态坐标
	void PhysicsManager::UpdateActorRigid()
	{
		std::unique_ptr<unique_readguard<WfirstRWLock>> rlock(new unique_readguard<WfirstRWLock>(gSceneLock));
		if (!IsRigidValid((ptr_t)gScene))
			return;
		gScene->lockWrite();
		auto RigidActor = GetRigidActor();
		if (RigidActor.size() < 10000)
		{
			std::unique_ptr<unique_readguard<WfirstRWLock>> dynamicActorMaprlock(new unique_readguard<WfirstRWLock >(dynamicActorMapLock));
			std::vector<ptr_t>RemoveActor;
			for (auto iter : dynamicActorMap)
			{
				if (RigidActor.find(iter.first) == RigidActor.end()) {
					//if (iter.second && IsRigidValid((ptr_t)*iter.second)) {
					//	(*iter.second)->setGlobalPose(PxTransform(PxVec3(0, 0, 0), PxQuat(0, 0, 0, 1)));
					//}
					RemoveActorFromScene(*iter.second);
					RemoveActor.push_back(iter.first);
				}
			}
			for (int i = 0; i < RemoveActor.size(); i++)
			{
				auto FindResult = dynamicActorMap.find(RemoveActor[i]);
				if (FindResult != dynamicActorMap.end())
					dynamicActorMap.erase(FindResult);
			}
			dynamicActorMaprlock.reset();
		}

		for (auto& Actor : RigidActor) {

			auto rigidDynamic = FindDynamicActorRigid(Actor.first);
			if (rigidDynamic && IsRigidValid((IronMan::ptr_t)*rigidDynamic)) {
				continue;
			}
			else if (rigidDynamic && !(*rigidDynamic)) {
				*rigidDynamic = gPhysics->createRigidDynamic(Actor.second->BodyTransform);
				if (!IsRigidValid((IronMan::ptr_t)*rigidDynamic))
					continue;
				PxShape* pxShape = nullptr;
				if (Actor.second->halfHigh != 0.f) {
					PxCapsuleGeometry geometry(Actor.second->Radius, Actor.second->halfHigh);
					pxShape = gPhysics->createShape(geometry, *gMaterial);
					if (!IsRigidValid((IronMan::ptr_t)pxShape)) {
						if (pxShape) {
							pxShape->release();
						}
						(*rigidDynamic)->release();
						*rigidDynamic = nullptr;
						continue;
					}
				}
				else {
					PxSphereGeometry geometry(Actor.second->Radius);
					pxShape = gPhysics->createShape(geometry, *gMaterial);
					if (!IsRigidValid((IronMan::ptr_t)pxShape)) {
						if (pxShape) {
							pxShape->release();
						}
						(*rigidDynamic)->release();
						*rigidDynamic = nullptr;
						continue;
					}
				}

				if (rigidDynamic && IsRigidValid((__int64)*rigidDynamic) && IsRigidValid((__int64)pxShape)) {
					auto Userdata = new RigidUserData();
					if (!Userdata)
						break;
					InsertAllcateMemory(Userdata);
					Userdata->DataType = RigidFreshType::RPlayer;
					Userdata->RigidPoint = Actor.first;
					(*rigidDynamic)->userData = (void*)Userdata;
					pxShape->setSimulationFilterData(PxFilterData(1, 1, 1, 1));
					pxShape->setQueryFilterData(PxFilterData(1, 1, 1, 1));
					(*rigidDynamic)->attachShape(*pxShape);
					pxShape->release();
					gScene->addActor(*(*rigidDynamic));
					InsertDynamicActorRigid(Actor.first, rigidDynamic);
				}
			}
		}
		gScene->unlockWrite();
		rlock.reset();
		return;
	}
	//需要一直更新的
	void PhysicsManager::UpdateAlwaysUpdatedRigid(VMMDLL_SCATTER_HANDLE ScatterHandle)
	{
		static FVector LastLocation = FVector();
		if (LastLocation.IsZero())
			LastLocation = sdk::GetLocalPos();
		auto CurrentLocation = sdk::GetLocalPos();
		auto Distance = (CurrentLocation - LastLocation).Size() / 100.f;
		if (abs(Distance) >= 500.f)
		{
			std::unique_ptr<unique_writeguard<WfirstRWLock>> wlock(new unique_writeguard<WfirstRWLock>(gSceneLock));
			if (!IsRigidValid((ptr_t)gScene))
				return;
			gScene->lockWrite();
			try
			{
				PxU32 numActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);
				if (numActors) {
					std::vector<PxRigidActor*> staticActors(numActors);
					gScene->getActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC, reinterpret_cast<PxActor**>(staticActors.data()), numActors);
					PxVec3 SubVec3 = PxVec3(0, 0, 0);
					int ChangeCount = 0;
					int NotChangeCount = 0;
					PxTransform ChangeValue = PxTransform(PxVec3(0, 0, 0), PxQuat(0, 0, 0, 0));
					for (PxRigidActor* actor : staticActors) {
						if (!IsRigidValid((IronMan::ptr_t)actor))
							continue;
						auto userData = (RigidUserData*)actor->userData;
						if (Utils::IsSafeReadPtr(userData, 8))
							continue;
						if (actor->is<PxRigidStatic>())
						{
							if (ChangeCount >= 5 || NotChangeCount >= 5)
								break;
							auto GlobalPose = actor->getGlobalPose();
							auto TempTransform = GetDMA().Read<PxTransform>(userData->TransformPtr);
							if (SubVec3.x == 0.f && SubVec3.y == 0.f && SubVec3.z == 0.f)
							{
								SubVec3 = TempTransform.p - GlobalPose.p;
								if (SubVec3.x == 0.f && SubVec3.y == 0.f && SubVec3.z == 0)
									NotChangeCount++;
							}
							else
							{
								auto TempVec3 = TempTransform.p - GlobalPose.p;
								if (TempVec3 == SubVec3)
									ChangeCount++;
								else
									NotChangeCount++;
							}
						}
					}

					if (ChangeCount >= 5 && SubVec3.x != 0.f && SubVec3.y != 0.f)
					{
						for (PxRigidActor* actor : staticActors) {
							if (!IsRigidValid((IronMan::ptr_t)actor))
								continue;
							auto userData = (RigidUserData*)actor->userData;
							if (Utils::IsSafeReadPtr(userData, 8))
								continue;
							if (actor->is<PxRigidStatic>())
							{
								auto GlobalPose = actor->getGlobalPose();
								GlobalPose.p += SubVec3;
								actor->setGlobalPose(GlobalPose);
							}
							else if (actor->is<PxRigidDynamic>())
							{
								auto GlobalPose = actor->getGlobalPose();
								GlobalPose.p += SubVec3;
								userData->DynamicBufferedBody2World = GlobalPose;
								actor->setGlobalPose(GlobalPose);

							}
						}
					}
				}
			}
			catch (...)
			{

			}
			gScene->unlockWrite();
			wlock.reset();
		}
		LastLocation = CurrentLocation;


		static auto SlowFreshTime = GetTickCount64();
		static auto StaticFreshTime = GetTickCount64();

		bool FreshSlow = false;
		if (GetTickCount64() - SlowFreshTime >= 100)
		{
			SlowFreshTime = GetTickCount64();
			FreshSlow = true;
		}

		UpdateActorRigid();
		FVector LocalPos = sdk::GetLocalPos();
		std::unique_ptr<unique_writeguard<WfirstRWLock>> wlock(new unique_writeguard<WfirstRWLock>(gSceneLock));
		if (!IsRigidValid((ptr_t)gScene))
			return;
		gScene->lockWrite();
		try
		{
			PxU32 numActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
			if (numActors) {
				std::vector<PxRigidActor*> dynamicActors(numActors);
				gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, reinterpret_cast<PxActor**>(dynamicActors.data()), numActors);
				for (PxRigidActor* actor : dynamicActors) {
					if (!IsRigidValid((IronMan::ptr_t)actor))
						continue;
					auto userData = (RigidUserData*)actor->userData;
					if (Utils::IsSafeReadPtr(userData, 8))
						continue;
					switch (userData->DataType)
					{
					case RigidFreshType::RPlayer:
					{
						auto PointToActor = userData->RigidPoint;
						auto UpdateData = FindRigidActor(PointToActor);
						if (UpdateData && UpdateData->Actor != PointToActor)
							break;
						setActorGlobalPoseInternal(actor->is<PxRigidDynamic>(), UpdateData->BodyTransform, UpdateData->Radius, UpdateData->halfHigh);
						break;
					}
					default:
						break;
					}
				}
			}
		}
		catch (...)
		{

		}
		gScene->unlockWrite();
		wlock.reset();
		static ptr_t DebugPtr = 0;
		std::unique_ptr<unique_readguard<WfirstRWLock>> rlock(new unique_readguard<WfirstRWLock>(gSceneLock));
		gScene->lockRead();
		try
		{
			PxU32 numActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
			if (numActors) {
				std::vector<PxRigidActor*> dynamicActors(numActors);
				gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, reinterpret_cast<PxActor**>(dynamicActors.data()), numActors);
				for (PxRigidActor* actor : dynamicActors) {
					if (!IsRigidValid((IronMan::ptr_t)actor))
						continue;
					auto userData = (RigidUserData*)actor->userData;
					if (Utils::IsSafeReadPtr(userData, 8))
						continue;
					switch (userData->DataType)
					{
					case RigidFreshType::RAlways:
					{
						if (DebugPtr == (ptr_t)actor)
							Sleep(0);
						if (!IsAddrValid(userData->TransformPtr) || !IsAddrValid(userData->DynamicBufferedBody2WorldPtr))
							break;
						bool updateRigid = false;
						int RigidReadTime = userData->ReadCount;
						if (userData->CalculateTransform == PxTransform(PxVec3(0, 0, 0), PxQuat(0, 0, 0, 0)) && userData->DynamicBufferedBody2World == PxTransform(PxVec3(0, 0, 0), PxQuat(0, 0, 0, 0)))
						{
							GetDMA().queueScatterReadEx(ScatterHandle, userData->TransformPtr, &userData->CalculateTransform, sizeof(PxTransform));
							updateRigid = true;
						}
						else
						{
							FVector2D screen;
							PxTransform RigidTransform = CalulateDynamicGlobalPose2(userData->CalculateTransform, userData->DynamicBufferedBody2World);
							auto delta = (PxVec3ToFVector(RigidTransform.p) - LocalPos);
							float dis = delta.Size() / 100.0f;
							if (dis > MaxDynamicRigidDistance)
								break;
							if (dis <= 5.f)
							{
								updateRigid = true;
								userData->ReadCount = 0;
								userData->ForceUpdate = 0;
							}
							else if (!sdk::WorldToScreen(PxVec3ToFVector(RigidTransform.p), screen))
							{
								if (RigidReadTime >= 60)
								{
									userData->ReadCount = 0;
									userData->ForceUpdate = 1;
									updateRigid = true;
								}
								else
								{
									RigidReadTime++;
									userData->ReadCount = RigidReadTime;
								}
								if (userData->ForceUpdate)
								{
									updateRigid = true;
								}
							}
							else
							{
								updateRigid = true;
								userData->ReadCount = 0;
								userData->ForceUpdate = 0;
							}
						}
						if (updateRigid)
						{
							bool AlreadyReadTransform = false;
							auto NbShapes = actor->getNbShapes();
							std::vector<PxShape*>Shapes(NbShapes);
							actor->getShapes(Shapes.data(), Shapes.size());
							for (auto& Shape : Shapes)
							{
								if (!IsRigidValid((ptr_t)Shape))
									continue;
								ShapeUserData* ShapeUserdata = (ShapeUserData*)Shape->userData;
								if (Utils::IsSafeReadPtr(ShapeUserdata, 8))
									continue;
								ptr_t ShapeTransformPtr = ShapeUserdata->ShapeTransformPtr;
								ptr_t SimulationFilterDataPtr = ShapeUserdata->SimulationFilterDataPtr;
								PxTransform ShapeTransform = ShapeUserdata->ShapeTransform;
								PxFilterData SimulationFilterData = ShapeUserdata->SimulationFilterData;
								TArray<ptr_t> WriteTArray = ShapeUserdata->WriteTArray;
								int ReadTime = ShapeUserdata->ReadCount;
								if (!IsAddrValid(ShapeTransformPtr) || !IsAddrValid(SimulationFilterDataPtr))
									continue;
								if (userData->ForceUpdate && userData->ForceUpdate < 8)
								{
									if (userData->ForceUpdate < 5)
									{
										GetDMA().queueScatterReadEx(ScatterHandle, SimulationFilterDataPtr + (userData->ForceUpdate - 1) * 4, (void*)((ptr_t)&ShapeUserdata->SimulationFilterData + (userData->ForceUpdate - 1) * 4), 4);
										GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x28 + (userData->ForceUpdate - 1) * 2, (void*)((ptr_t)&ShapeUserdata->WriteTArray + (userData->ForceUpdate - 1) * 2), sizeof(WORD));
									}
									GetDMA().queueScatterReadEx(ScatterHandle, userData->DynamicBufferedBody2WorldPtr + (userData->ForceUpdate - 1) * 4, (void*)((ptr_t)&userData->DynamicBufferedBody2World + (userData->ForceUpdate - 1) * 4), 4);
									userData->ForceUpdate++;
									AlreadyReadTransform = true;
									continue;
								}
								if (ShapeTransform == PxTransform(PxVec3(0, 0, 0), PxQuat(0, 0, 0, 0)) || SimulationFilterData == PxFilterData(0, 0, 0, 0) || WriteTArray->Data == nullptr || WriteTArray->Num() == 0)
								{
									GetDMA().queueScatterReadEx(ScatterHandle, ShapeTransformPtr, (void*)&ShapeUserdata->ShapeTransform, sizeof(PxTransform));
									GetDMA().queueScatterReadEx(ScatterHandle, SimulationFilterDataPtr, (void*)&ShapeUserdata->SimulationFilterData, sizeof(PxFilterData));
									GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x28, (void*)&ShapeUserdata->WriteTArray, sizeof(TArray<ptr_t>));
								}
								else
								{
									if (ReadTime >= 60)
									{
										if (SimulationFilterData.word3 == 2)
										{
											GetDMA().queueScatterReadEx(ScatterHandle, userData->DynamicBufferedBody2WorldPtr, (void*)&userData->DynamicBufferedBody2World, sizeof(PxTransform));
											AlreadyReadTransform = true;
										}
										GetDMA().queueScatterReadEx(ScatterHandle, SimulationFilterDataPtr, (void*)&ShapeUserdata->SimulationFilterData, sizeof(PxFilterData));
										ShapeUserdata->ReadCount = 0;
									}
									else if (ReadTime == 40)
									{
										GetDMA().queueScatterReadEx(ScatterHandle, ShapeUserdata->ShapePtr + 0x98, (void*)&ShapeUserdata->ShapeType, sizeof(BYTE));
										ReadTime++;
										ShapeUserdata->ReadCount = ReadTime;
										AlreadyReadTransform = false;
									}
									else if (ReadTime == 30)
									{
										GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x28, (void*)&ShapeUserdata->WriteTArray->Data, sizeof(ptr_t));
										GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x30, (void*)&ShapeUserdata->WriteTArray->Count, sizeof(WORD));
										//GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x28, (void*)&ShapeUserdata->WriteTArray, sizeof(TArray<ptr_t>));
										ReadTime++;
										ShapeUserdata->ReadCount = ReadTime;
										AlreadyReadTransform = false;
									}
									else
									{
										ReadTime++;
										ShapeUserdata->ReadCount = ReadTime;
										AlreadyReadTransform = false;
									}
								}
								if (ShapeUserdata->SimulationFilterData.word3 == 50331658 || ShapeUserdata->SimulationFilterData.word3 == 50331662)
									GetDMA().queueScatterReadEx(ScatterHandle, ShapeTransformPtr, (void*)&ShapeUserdata->ShapeTransform.q, sizeof(PxQuat));
							}
							if (!AlreadyReadTransform)
							{
								GetDMA().queueScatterReadEx(ScatterHandle, userData->DynamicBufferedBody2WorldPtr, (void*)&userData->DynamicBufferedBody2World, sizeof(PxTransform));
							}
						}
						break;
					}
					case RigidFreshType::RSlow:
					case RigidFreshType::RNone:
					{
						if (!FreshSlow)
							break;
						if (!IsAddrValid(userData->TransformPtr) || !IsAddrValid(userData->DynamicBufferedBody2WorldPtr))
							break;
						bool updateRigid = false;
						int RigidReadTime = userData->ReadCount;
						if (userData->CalculateTransform == PxTransform(PxVec3(0, 0, 0), PxQuat(0, 0, 0, 0)) && userData->DynamicBufferedBody2World == PxTransform(PxVec3(0, 0, 0), PxQuat(0, 0, 0, 0)))
						{
							GetDMA().queueScatterReadEx(ScatterHandle, userData->TransformPtr, &userData->CalculateTransform, sizeof(PxTransform));
							updateRigid = true;
						}
						else
						{
							FVector2D screen;
							PxTransform RigidTransform = CalulateDynamicGlobalPose2(userData->CalculateTransform, userData->DynamicBufferedBody2World);
							auto delta = (PxVec3ToFVector(RigidTransform.p) - LocalPos);
							float dis = delta.Size() / 100.0f;
							if (dis > MaxDynamicRigidDistance)
								break;
							if (dis <= 5.f)
							{
								updateRigid = true;
								userData->ReadCount = 0;
								userData->ForceUpdate = 0;
							}
							else if (!sdk::WorldToScreen(PxVec3ToFVector(RigidTransform.p), screen))
							{
								if (RigidReadTime >= 60)
								{
									userData->ReadCount = 0;
									userData->ForceUpdate = 1;
									updateRigid = true;
								}
								else
								{
									RigidReadTime++;
									userData->ReadCount = RigidReadTime;
								}
								if (userData->ForceUpdate)
								{
									updateRigid = true;
								}
							}
							else
							{
								updateRigid = true;
								userData->ReadCount = 0;
								userData->ForceUpdate = 0;
							}
						}
						if (updateRigid)
						{
							bool AlreadyReadTransform = false;
							auto NbShapes = actor->getNbShapes();
							std::vector<PxShape*>Shapes(NbShapes);
							actor->getShapes(Shapes.data(), Shapes.size());
							for (auto& Shape : Shapes)
							{
								if (!IsRigidValid((ptr_t)Shape))
									continue;
								ShapeUserData* ShapeUserdata = (ShapeUserData*)Shape->userData;
								if (Utils::IsSafeReadPtr(ShapeUserdata, 1))
									continue;
								ptr_t ShapeTransformPtr = ShapeUserdata->ShapeTransformPtr;
								ptr_t SimulationFilterDataPtr = ShapeUserdata->SimulationFilterDataPtr;
								PxTransform ShapeTransform = ShapeUserdata->ShapeTransform;
								PxFilterData SimulationFilterData = ShapeUserdata->SimulationFilterData;
								TArray<ptr_t> WriteTArray = ShapeUserdata->WriteTArray;
								int ReadTime = ShapeUserdata->ReadCount;
								if (!IsAddrValid(ShapeTransformPtr) || !IsAddrValid(SimulationFilterDataPtr))
									continue;
								if (userData->ForceUpdate && userData->ForceUpdate < 8)
								{
									if (userData->ForceUpdate < 5)
									{
										GetDMA().queueScatterReadEx(ScatterHandle, SimulationFilterDataPtr + (userData->ForceUpdate - 1) * 4, (void*)((ptr_t)&ShapeUserdata->SimulationFilterData + (userData->ForceUpdate - 1) * 4), 4);
										GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x28 + (userData->ForceUpdate - 1) * 2, (void*)((ptr_t)&ShapeUserdata->WriteTArray + (userData->ForceUpdate - 1) * 2), sizeof(WORD));
									}
									GetDMA().queueScatterReadEx(ScatterHandle, userData->DynamicBufferedBody2WorldPtr + (userData->ForceUpdate - 1) * 4, (void*)((ptr_t)&userData->DynamicBufferedBody2World + (userData->ForceUpdate - 1) * 4), 4);
									userData->ForceUpdate++;
									AlreadyReadTransform = true;
									continue;
								}
								if (ShapeTransform == PxTransform(PxVec3(0, 0, 0), PxQuat(0, 0, 0, 0)) || SimulationFilterData == PxFilterData(0, 0, 0, 0) || WriteTArray->Data == nullptr || WriteTArray->Num() == 0)
								{
									GetDMA().queueScatterReadEx(ScatterHandle, ShapeTransformPtr, (void*)&ShapeUserdata->ShapeTransform, sizeof(PxTransform));
									GetDMA().queueScatterReadEx(ScatterHandle, SimulationFilterDataPtr, (void*)&ShapeUserdata->SimulationFilterData, sizeof(PxFilterData));
									GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x28, (void*)&ShapeUserdata->WriteTArray, sizeof(TArray<ptr_t>));
								}
								else
								{
									if (ReadTime >= 60)
									{
										if (SimulationFilterData.word3 == 2)
										{
											GetDMA().queueScatterReadEx(ScatterHandle, userData->DynamicBufferedBody2WorldPtr, (void*)&userData->DynamicBufferedBody2World, sizeof(PxTransform));
											AlreadyReadTransform = true;
										}
										GetDMA().queueScatterReadEx(ScatterHandle, SimulationFilterDataPtr, (void*)&ShapeUserdata->SimulationFilterData, sizeof(PxFilterData));
										ShapeUserdata->ReadCount = 0;
									}
									else if (ReadTime == 50)
									{
										GetDMA().queueScatterReadEx(ScatterHandle, ShapeUserdata->ShapePtr + 0x98, (void*)&ShapeUserdata->ShapeType, sizeof(BYTE));
										GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x30, (void*)&ShapeUserdata->WriteTArray->Count, sizeof(WORD));
										ReadTime++;
										ShapeUserdata->ReadCount = ReadTime;
										AlreadyReadTransform = false;
									}
									else if (ReadTime == 40)
									{
										GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x28 + 6, (void*)((ptr_t)&ShapeUserdata->WriteTArray + 6), sizeof(WORD));
										//GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x28, (void*)&ShapeUserdata->WriteTArray, sizeof(TArray<ptr_t>));
										ReadTime++;
										ShapeUserdata->ReadCount = ReadTime;
										AlreadyReadTransform = false;
									}
									else if (ReadTime == 30)
									{
										GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x28 + 4, (void*)((ptr_t)&ShapeUserdata->WriteTArray + 4), sizeof(WORD));
										//GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x28, (void*)&ShapeUserdata->WriteTArray, sizeof(TArray<ptr_t>));
										ReadTime++;
										ShapeUserdata->ReadCount = ReadTime;
										AlreadyReadTransform = false;
									}
									else if (ReadTime == 20)
									{
										GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x28 + 2, (void*)((ptr_t)&ShapeUserdata->WriteTArray + 2), sizeof(WORD));
										//GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x28, (void*)&ShapeUserdata->WriteTArray, sizeof(TArray<ptr_t>));
										ReadTime++;
										ShapeUserdata->ReadCount = ReadTime;
										AlreadyReadTransform = false;
									}
									else if (ReadTime == 10)
									{
										GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x28 + 0, (void*)((ptr_t)&ShapeUserdata->WriteTArray + 0), sizeof(WORD));
										//GetDMA().queueScatterReadEx(ScatterHandle, userData->RigidPoint + 0x28, (void*)&ShapeUserdata->WriteTArray, sizeof(TArray<ptr_t>));
										ReadTime++;
										ShapeUserdata->ReadCount = ReadTime;
										AlreadyReadTransform = false;
									}
									else
									{
										ReadTime++;
										ShapeUserdata->ReadCount = ReadTime;
										AlreadyReadTransform = false;
									}
								}
							}
							if (!AlreadyReadTransform)
							{
								GetDMA().queueScatterReadEx(ScatterHandle, userData->DynamicBufferedBody2WorldPtr, (void*)&userData->DynamicBufferedBody2World, sizeof(PxTransform));
							}
						}
						break;
					}
					default:
						break;
					}
				}
			}
		}
		catch (...)
		{
			CONSOLE_INFO("Always Update Cause Exception！");
		}
		gScene->unlockRead();
		rlock.reset();

		int totleSize = GetDMA().executeScatterRead2(ScatterHandle);
		//CONSOLE_INFO("Update Read : %d", totleSize);
		wlock.reset(new unique_writeguard<WfirstRWLock>(gSceneLock));
		if (!IsRigidValid((ptr_t)gScene))
			return;
		gScene->lockWrite();
		try
		{
			auto numActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
			if (numActors) {
				std::vector<PxRigidActor*> dynamicActors(numActors);
				gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, reinterpret_cast<PxActor**>(dynamicActors.data()), numActors);
				for (PxRigidActor* actor : dynamicActors) {
					if (!actor || !IsRigidValid((IronMan::ptr_t)actor))
						continue;
					auto userData = (RigidUserData*)actor->userData;
					if (Utils::IsSafeReadPtr((void*)userData, 8))
						break;
					switch (userData->DataType)
					{
					case RigidFreshType::RSlow:
					case RigidFreshType::RNone:
					case RigidFreshType::RAlways:
					{
						if (userData->DataType == RigidFreshType::RSlow || userData->DataType == RigidFreshType::RNone)
							if (!FreshSlow)
								break;

						if (!IsAddrValid(userData->TransformPtr) || !IsAddrValid(userData->DynamicBufferedBody2WorldPtr))
							break;
						FVector2D Screen;
						PxTransform RigidTransform = CalulateDynamicGlobalPose2(userData->CalculateTransform, userData->DynamicBufferedBody2World);
						auto delta = (PxVec3ToFVector(RigidTransform.p) - LocalPos);
						float dis = delta.Size() / 100.0f;
						if (dis > MaxDynamicRigidDistance)
						{
							AddtoErase(userData->RigidPoint);
							continue;
						}
						if (DebugPtr == (ptr_t)actor)
							Sleep(0);
						bool NeedForceUpdate = false;
						if (userData->ForceUpdate == 8)
						{

							userData->ForceUpdate = 0;
							NeedForceUpdate = true;
						}
						if (NeedForceUpdate || dis <= 5.f || sdk::WorldToScreen(PxVec3ToFVector(RigidTransform.p), Screen))
							setGlobalPoseInternal(actor, RigidTransform);
						break;
					}
					default:
						break;
					}
				}
			}
		}
		catch (...)
		{
			CONSOLE_INFO("Always Update Cause Exception2!");
		}
		gScene->unlockWrite();
		wlock.reset();
		try
		{
			auto RayArray = GetRayCastPtr();
			if (RayArray.size() && RayArray.size() < 1000)
			{
				std::vector<std::pair<ptr_t, std::vector<bool>>> Result;
				Result.resize(RayArray.size());
				for (int i = 0; i < RayArray.size(); i++)
				{
					Result[i].first = RayArray[i].first;
					Result[i].second.resize(24);
					for (auto j = 0; j < 24; j++)
						Result[i].second[j] = false;
				}
				auto AimTarget = sdk::GetAimTarget();
				for (int i = 0; i < RayArray.size(); i++)
				{
					if (RayArray[i].second && !Utils::IsSafeReadPtr(RayArray[i].second->data(), 8))
					{
						if (RayArray[i].second->size() && RayArray[i].second->size() < 1000)
						{
							auto LocalPos = sdk::GetLocalPos();
							//CONSOLE_INFO("LocalPos:%f|%f|%f", LocalPos.X, LocalPos.Y, LocalPos.Z);
							//CONSOLE_INFO("FiringPo:%f|%f|%f", Firing.X, Firing.Y, Firing.Z);
							auto LocalIsDead = sdk::IsLocalPlayerAlive();
							for (int j = 0; j < RayArray[i].second->size(); j++)
							{
								if (RayArray[i].first != AimTarget)
								{
									if (j == 1 || j == 3 || j == 4 || j == 12 || j == 13 || j == 20 || j == 21 || j == 22)
										continue;
								}
								if (j == 23 && LocalIsDead)
									LocalPos = sdk::GetLocalHeadPos();
								//else if (LocalIsDead && sdk::GetIsAiming_CP() && sdk::GetIsScoping_CP() != 0.f)
								//{
								//	LocalPos = sdk::GetFiringLocation();
								//	//LocalPos.Z += sdk::GetBaseEyeHeight();
								//}
								if (!Utils::IsSafeReadPtr((LPVOID)((ptr_t)RayArray[i].second->data() + j * sizeof(FVector)), 0xC) && !(*RayArray[i].second)[j].IsZero())
									Result[i].second[j] = WeaponCanHit(LocalPos, (*RayArray[i].second)[j], 200000.f, false);
							}
						}
					}
				}
				SetActorVisibleAll(Result);
			}
		}
		catch (...)
		{
			CONSOLE_INFO("Loop Weapon Can Hit Exception!");
		}
	}
	//同步线程
	void PhysicsManager::syncScenes()
	{
		static VMMDLL_SCATTER_HANDLE ScatterHandle = GetDMA().createScatterHandle();
		using framerate = std::chrono::duration<int, std::ratio<1, 60>>;
		auto tp = std::chrono::system_clock::now() + framerate{ 1 };
		do
		{
			try
			{
				int count = 0;
				auto start = std::chrono::high_resolution_clock::now();
				auto end = start + std::chrono::seconds(1);
				while (std::chrono::high_resolution_clock::now() < end) {
					if (!sdk::InGame() || !IsSceneCreate() || !Vars.Menu.PhysxTrace)
					{
						OtherThreadIsEnd = true;
						Sleep(2000);
						continue;
					}
					OtherThreadIsEnd = false;
					UpdateAlwaysUpdatedRigid(ScatterHandle);
					std::this_thread::sleep_until(tp);
					tp += framerate{ 1 };
				}
			}
			catch (...)
			{
				CONSOLE_INFO("syncScenes Exception!");
			}
		} while (true);
		return;
	}

	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//绘制场景对象的形态
	void PhysicsManager::DrawRigidBox()
	{
		if (!IsSceneCreate() || !Vars.Aimbot.TriangleMesh)
			return;
		static bool Ignore = false;
		PxActorTypeFlags Flag;
		if (Vars.Aimbot.StaticMesh)
		{
			Flag = PxActorTypeFlag::eRIGID_STATIC;
			if (Vars.Aimbot.DynamicMesh)
				Flag |= PxActorTypeFlag::eRIGID_DYNAMIC;
		}
		else if (Vars.Aimbot.DynamicMesh)
			Flag = PxActorTypeFlag::eRIGID_DYNAMIC;
		FVector LocalPos = sdk::GetLocalPos();
		unique_readguard<WfirstRWLock> rlock(gSceneLock);
		if (!IsRigidValid((ptr_t)gScene))
			return;
		gScene->lockRead();
		try
		{
			PxU32 numActors = gScene->getNbActors(Flag);
			if (numActors) {
				std::vector<PxRigidActor*> dynamicActors(numActors);
				gScene->getActors(Flag, reinterpret_cast<PxActor**>(dynamicActors.data()), numActors);
				for (PxRigidActor* actor : dynamicActors) {
					if (!actor || !IsRigidValid((IronMan::ptr_t)actor))
						continue;
					PxTransform SceneTransform = actor->getGlobalPose();
					auto delta = (PxVec3ToFVector(SceneTransform.p) - LocalPos);
					float dis = delta.Size() / 100.0f;
					if (dis > 20.f)
						continue;
					PxU32 shapeCount = actor->getNbShapes();
					std::vector<PxShape*> shapes(shapeCount);
					actor->getShapes(shapes.data(), shapeCount);
					for (PxShape* shape : shapes) {
						if (shape && IsRigidValid((IronMan::ptr_t)shape))
						{
							PxTransform ShapeTransform = shape->getLocalPose();
							PxGeometryHolder geometryHolder = shape->getGeometry();
							switch (geometryHolder.getType()) {
							case PxGeometryType::eTRIANGLEMESH:
							{
								auto triangleMesh = geometryHolder.triangleMesh();
								auto scale = triangleMesh.scale;
								FQuat totalRotation = PxQuatToFQuat(SceneTransform.q) * PxQuatToFQuat(ShapeTransform.q);
								FVector center = PxVec3ToFVector(SceneTransform.p) + PxQuatToFQuat(SceneTransform.q).RotateVector(PxVec3ToFVector(ShapeTransform.p));

								// 转换所有顶点到世界坐标
								auto VerticesNum = triangleMesh.triangleMesh->getNbVertices();
								auto Vertices = triangleMesh.triangleMesh->getVertices();
								auto Color = 0xFFFFFFFF;
								//if (Flag & PxActorTypeFlag::eRIGID_STATIC)
									//if (Flag & PxActorTypeFlag::eRIGID_DYNAMIC)
								{
									auto SF = shape->getSimulationFilterData();
									//RigidUserData* userdata = (RigidUserData*)actor->userData;
									//if (userdata)
									//{
									//	ShapeUserData* shapeuserdata = (ShapeUserData*)shape->userData;
									//	auto RigidType = userdata->DataType;
									//	if (RigidType != RigidFreshType::RNotAdd)
									//	{
									//		if (Ignore && RigidType != RigidFreshType::RNone)
									//			break;
									//		FVector2D OutScreen;
									//		if (sdk::WorldToScreen(PxVec3ToFVector(SceneTransform.p), OutScreen))
									//		{
									//			Color = RigidType != RigidFreshType::RNone ? 0x8000FF00 : 0xFFFFFFFF;
									//			//if (VerticesNum > 5000 && VerticesNum < 10000)
									//			//	Color = 0xFF00FFFF;
									//			//else if (VerticesNum >= 10000)
									//			//	Color = 0xFF0000FF;
									//			Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, OutScreen, Color, 0xEE000000, 0, "Vertices:%d\n[%d]", VerticesNum,
									//				SF.word3);
									//		}
									//	}
									//}
								}

								if (dis > 20.f)
									break;
								if (!Vars.Aimbot.TriangleMesh)
									break;
								std::vector<FVector> transformedVertices(VerticesNum);

								for (size_t v = 0; v < VerticesNum; ++v) {
									PxVec3 vertex = Vertices[v];
									vertex = scale.rotation.rotate(vertex).multiply(scale.scale);
									transformedVertices[v] = center + totalRotation.RotateVector(PxVec3ToFVector(vertex));
								}
								// 绘制每个三角形
								if (triangleMesh.triangleMesh->getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES)
								{
									auto Indices16 = reinterpret_cast<const PxU16*>(triangleMesh.triangleMesh->getTriangles());
									for (size_t t = 0; t < triangleMesh.triangleMesh->getNbTriangles() * 3; t += 3) {
										PxU16 vIndex0 = Indices16[t];
										PxU16 vIndex1 = Indices16[t + 1];
										PxU16 vIndex2 = Indices16[t + 2];

										FVector2D screenPoint0, screenPoint1, screenPoint2;
										bool visible0 = sdk::WorldToScreen(transformedVertices[vIndex0], screenPoint0);
										bool visible1 = sdk::WorldToScreen(transformedVertices[vIndex1], screenPoint1);
										bool visible2 = sdk::WorldToScreen(transformedVertices[vIndex2], screenPoint2);

										if (visible0 && visible1 && visible2) {
											Drawing::DrawLine(screenPoint0, screenPoint1, Color, 0.5f);
											Drawing::DrawLine(screenPoint1, screenPoint2, Color, 0.5f);
											Drawing::DrawLine(screenPoint2, screenPoint0, Color, 0.5f);
										}
									}
								}
								else {
									auto Indices32 = reinterpret_cast<const PxU32*>(triangleMesh.triangleMesh->getTriangles());
									for (size_t t = 0; t < triangleMesh.triangleMesh->getNbTriangles() * 3; t += 3) {
										PxU32 vIndex0 = Indices32[t];
										PxU32 vIndex1 = Indices32[t + 1];
										PxU32 vIndex2 = Indices32[t + 2];

										FVector2D screenPoint0, screenPoint1, screenPoint2;
										bool visible0 = sdk::WorldToScreen(transformedVertices[vIndex0], screenPoint0);
										bool visible1 = sdk::WorldToScreen(transformedVertices[vIndex1], screenPoint1);
										bool visible2 = sdk::WorldToScreen(transformedVertices[vIndex2], screenPoint2);

										if (visible0 && visible1 && visible2) {
											Drawing::DrawLine(screenPoint0, screenPoint1, Color, 0.5f);
											Drawing::DrawLine(screenPoint1, screenPoint2, Color, 0.5f);
											Drawing::DrawLine(screenPoint2, screenPoint0, Color, 0.5f);
										}
									}
								}

								break;
							}
							case PxGeometryType::eCAPSULE:
							{
								PxCapsuleGeometry Capsule = geometryHolder.capsule();
								const FQuat& Scenerotation = PxQuatToFQuat(SceneTransform.q);

								// Get capsule parameters
								FVector shapeTranslation = PxVec3ToFVector(ShapeTransform.p);
								FVector center = PxVec3ToFVector(SceneTransform.p) + Scenerotation.RotateVector(shapeTranslation);
								float radius = Capsule.radius;
								float halfHeight = Capsule.halfHeight;

								ImU32 ColorRender = ((round(Capsule.radius) == 40.f && (round(Capsule.halfHeight) == 48.f)) || ((round(Capsule.radius) == 40.f && round(Capsule.halfHeight) == 20.f)) || ((round(Capsule.radius) == 20.f && (round(Capsule.halfHeight) == 70.f))) && shapes.size() == 1) ? 0xFF0000FF : 0xFFFFFFFF;
								//if (ColorRender == 0xFF0000FF)
								{
									FVector2D screen1;
									if (sdk::WorldToScreen(PxVec3ToFVector(SceneTransform.p), screen1))
									{
										auto SF = shape->getSimulationFilterData();
										auto QF = shape->getQueryFilterData();
										Drawing::DrawCircleFilled(screen1, 2.5, 0xFF00FF00);
										//Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, screen1 + ImVec2(0, 4), 0xFFFFFFFF, 0xFF000000, 0, "%d|%d|%d|%d", SF.word0, SF.word1, SF.word2, SF.word3);
										//Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, screen1 + ImVec2(0, 14), 0xFFFFFFFF, 0xFF000000, 0, "%d|%d|%d|%d", QF.word0, QF.word1, QF.word2, QF.word3);
									}
								}
								// Compute capsule top and bottom centers
								FVector topCenter = center + Scenerotation.RotateVector(FVector(0, 0, halfHeight));
								FVector bottomCenter = center + Scenerotation.RotateVector(FVector(0, 0, -halfHeight));

								std::vector<ImVec2> pointsTop;
								std::vector<ImVec2> pointsBottom;
								std::vector<ImVec2> pointsCylinder;

								int segments = 24;
								float step = 6.28318530718f / segments;  // Divide circle into segments

								for (float theta = 0; theta < 6.28318530718f; theta += step) {
									// Calculate points around the top and bottom circles
									FVector topPoint = topCenter + Scenerotation.RotateVector(FVector(radius * cosf(theta), radius * sinf(theta), 0));
									FVector bottomPoint = bottomCenter + Scenerotation.RotateVector(FVector(radius * cosf(theta), radius * sinf(theta), 0));

									FVector2D screenTopPoint, screenBottomPoint;
									if (sdk::WorldToScreen(topPoint, screenTopPoint) && sdk::WorldToScreen(bottomPoint, screenBottomPoint))
									{
										pointsTop.push_back(screenTopPoint);
										pointsBottom.push_back(screenBottomPoint);

										// Calculate points for the cylinder lines
										pointsCylinder.push_back(screenTopPoint);
										pointsCylinder.push_back(screenBottomPoint);
									}
								}

								// Draw top and bottom circles
								if (pointsTop.size() > 1) {
									for (size_t i = 0; i < pointsTop.size(); ++i) {
										size_t next = (i + 1) % pointsTop.size();
										Drawing::DrawLine(pointsTop[i], pointsTop[next], ColorRender, 1.5f);
										Drawing::DrawLine(pointsBottom[i], pointsBottom[next], ColorRender, 1.5f);
									}
								}

								// Draw cylinder lines
								if (pointsCylinder.size() > 1) {
									for (size_t i = 0; i < pointsCylinder.size(); i += 2) {
										Drawing::DrawLine(pointsCylinder[i], pointsCylinder[i + 1], ColorRender, 1.5f);
									}
								}

								break;
							}
							case PxGeometryType::eHEIGHTFIELD:
							{
								break;
								// 获取高度场几何信息
								PxHeightFieldGeometry heightField = geometryHolder.heightField();
								// 获取场景和形状的变换
								FTransform t_SceneTransform = FTransform(PxQuatToFQuat(SceneTransform.q), PxVec3ToFVector(SceneTransform.p));
								FTransform t_ShapeTransform = FTransform(PxQuatToFQuat(ShapeTransform.q), PxVec3ToFVector(ShapeTransform.p));

								// 计算中心和总旋转
								FVector center = t_SceneTransform.GetTranslation() + t_SceneTransform.GetRotation().RotateVector(t_ShapeTransform.GetTranslation());
								FQuat totalRotation = t_SceneTransform.GetRotation() * t_ShapeTransform.GetRotation();

								// 获取高度场的行、列和缩放因子
								auto NumRows = heightField.heightField->getNbRows();
								auto NumCols = heightField.heightField->getNbColumns();
								auto HeightScale = heightField.heightScale;
								auto RowScale = heightField.rowScale;
								auto ColScale = heightField.columnScale;

								// 存储顶点
								std::vector<FVector> Vertices;

								// 生成顶点
								for (int32_t Row = 0; Row < NumRows; ++Row) {
									for (int32_t Col = 0; Col < NumCols; ++Col) {
										PxHeightFieldSample Sample = heightField.heightField->getSample(Row, Col);
										float Height = Sample.height * HeightScale;
										FVector Vertex(Col * ColScale, Row * RowScale, Height);
										Vertices.push_back(Vertex);
									}
								}

								// 应用总变换
								for (auto& Vertex : Vertices) {
									//Vertex = totalRotation.RotateVector(Vertex) + center;
									Vertex = Vertex + center;
								}

								//std::vector<ImVec2> ScreenPositions;
								for (const auto& Vertex : Vertices) {
									float disss = (Vertex - LocalPos).Size() / 100.0f;
									if (disss > 200.f)
										continue;
									FVector2D ScreenPosition;
									if (sdk::WorldToScreen(Vertex, ScreenPosition)) {
										//ScreenPositions.push_back(ImVec2(ScreenPosition.X, ScreenPosition.Y));
										Drawing::DrawCircleFilled(ImVec2(ScreenPosition.X, ScreenPosition.Y), 2.f, 0xFF0000FF);
									}
								}

								//// 绘制高度场网格
								//for (int32 Row = 0; Row < NumRows - 1; ++Row) {
								//	for (int32 Col = 0; Col < NumCols - 1; ++Col) {
								//		int32 BottomLeft = Col + Row * NumCols;
								//		int32 BottomRight = (Col + 1) + Row * NumCols;
								//		int32 TopLeft = Col + (Row + 1) * NumCols;
								//		int32 TopRight = (Col + 1) + (Row + 1) * NumCols;

								//		if (BottomLeft < ScreenPositions.size() && BottomRight < ScreenPositions.size() && TopLeft < ScreenPositions.size() && TopRight < ScreenPositions.size()) {
								//			// 绘制顶点和连接线
								//			//Drawing::DrawLine(ScreenPositions[BottomLeft], ScreenPositions[BottomRight], 0xFFFFFFFF, 1.5f);
								//			//Drawing::DrawLine(ScreenPositions[BottomRight], ScreenPositions[TopRight], 0xFFFFFFFF, 1.5f);
								//			//Drawing::DrawLine(ScreenPositions[TopRight], ScreenPositions[TopLeft], 0xFFFFFFFF, 1.5f);
								//			//Drawing::DrawLine(ScreenPositions[TopLeft], ScreenPositions[BottomLeft], 0xFFFFFFFF, 1.5f);

								//			Drawing::DrawLine(ScreenPositions[BottomLeft], ScreenPositions[BottomLeft] + ImVec2(1, 1), 0xFFFFFFFF, 1.5f);
								//			Drawing::DrawLine(ScreenPositions[BottomRight], ScreenPositions[BottomRight] + ImVec2(1, 1), 0xFFFFFFFF, 1.5f);
								//			Drawing::DrawLine(ScreenPositions[TopRight], ScreenPositions[TopRight] + ImVec2(1, 1), 0xFFFFFFFF, 1.5f);
								//			Drawing::DrawLine(ScreenPositions[TopLeft], ScreenPositions[TopLeft] + ImVec2(1, 1), 0xFFFFFFFF, 1.5f);
								//		}
								//	}
								//}

								break;
							}
							case PxGeometryType::eCONVEXMESH:
							{
								break;
								auto ConvexMesh = geometryHolder.convexMesh();
								auto Vertices = ConvexMesh.convexMesh->getVertices();
								auto IndexBuffer = ConvexMesh.convexMesh->getIndexBuffer();
								auto VerticeSize = ConvexMesh.convexMesh->getNbVertices();
								auto PolygonSize = ConvexMesh.convexMesh->getNbPolygons();
								FTransform t_ShapeTransform = FTransform(PxQuatToFQuat(ShapeTransform.q), PxVec3ToFVector(ShapeTransform.p));

								FVector center = PxVec3ToFVector(SceneTransform.p) + PxQuatToFQuat(SceneTransform.q).RotateVector(t_ShapeTransform.GetTranslation());
								FQuat totalRotation = PxQuatToFQuat(SceneTransform.q) * t_ShapeTransform.GetRotation();

								// 转换所有顶点到世界坐标
								std::vector<FVector> transformedVertices(VerticeSize);
								for (size_t v = 0; v < VerticeSize; ++v) {
									transformedVertices[v] = center + totalRotation.RotateVector(PxVec3ToFVector(Vertices[v]));
								}

								// 绘制每个多边形
								for (int i = 0; i < PolygonSize; i++) {
									PxHullPolygon polygon;
									auto bget = ConvexMesh.convexMesh->getPolygonData(i, polygon);
									for (size_t p = 0; p < polygon.mNbVerts; ++p) {
										PxU8 vIndex0 = IndexBuffer[polygon.mIndexBase + p];
										PxU8 vIndex1 = IndexBuffer[polygon.mIndexBase + (p + 1) % polygon.mNbVerts];

										// 检查索引是否越界
										if (vIndex0 >= transformedVertices.size() || vIndex1 >= transformedVertices.size()) {
											continue; // 跳过非法索引
										}

										FVector2D screenPoint0, screenPoint1;
										bool visible0 = sdk::WorldToScreen(transformedVertices[vIndex0], screenPoint0);
										bool visible1 = sdk::WorldToScreen(transformedVertices[vIndex1], screenPoint1);

										if (visible0 && visible1) {
											Drawing::DrawLine(screenPoint0, screenPoint1, 0xFFFFFFFF, 1.5f);
										}
									}
								}
								break;
							}
							case PxGeometryType::eBOX:
							{
								break;
								auto Box = geometryHolder.box();
								FQuat sceneRotation = PxQuatToFQuat(SceneTransform.q);
								FQuat shapeRotation = PxQuatToFQuat(ShapeTransform.q);
								FVector halfExtents = PxVec3ToFVector(Box.halfExtents);
								FVector shapeTranslation = PxVec3ToFVector(ShapeTransform.p);
								FVector center = PxVec3ToFVector(SceneTransform.p) + sceneRotation.RotateVector(shapeTranslation);
								if (actor->is<PxRigidDynamic>())
								{
									FVector2D screen1, screen2;
									if (sdk::WorldToScreen(PxVec3ToFVector(SceneTransform.p), screen1))
									{
										Drawing::DrawCircleFilled(screen1, 2.5, 0xFFFFFFFF);
									}
									if (sdk::WorldToScreen(center, screen2))
									{
										auto SF = shape->getSimulationFilterData();
										auto QF = shape->getQueryFilterData();
										Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, screen2, 0xFFFFFFFF, 0xFF000000, FONT_LEFT, "[%d|%d|%d|%d]", SF.word0, SF.word1, SF.word2, SF.word3);
										Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, screen2 + ImVec2(0, 14), 0xFF00FF00, 0xFF000000, FONT_LEFT, "[%d|%d|%d|%d]", QF.word0, QF.word1, QF.word2, QF.word3);
										//Drawing::DrawCircleFilled(screen2, 2.5, 0xFF00FF00);
									}
								}

								FQuat totalRotation = sceneRotation * shapeRotation;

								std::vector<FVector> points = {
									center + totalRotation.RotateVector(FVector(halfExtents.X, halfExtents.Y, halfExtents.Z)),
									center + totalRotation.RotateVector(FVector(halfExtents.X, halfExtents.Y, -halfExtents.Z)),
									center + totalRotation.RotateVector(FVector(halfExtents.X, -halfExtents.Y, halfExtents.Z)),
									center + totalRotation.RotateVector(FVector(halfExtents.X, -halfExtents.Y, -halfExtents.Z)),
									center + totalRotation.RotateVector(FVector(-halfExtents.X, halfExtents.Y, halfExtents.Z)),
									center + totalRotation.RotateVector(FVector(-halfExtents.X, halfExtents.Y, -halfExtents.Z)),
									center + totalRotation.RotateVector(FVector(-halfExtents.X, -halfExtents.Y, halfExtents.Z)),
									center + totalRotation.RotateVector(FVector(-halfExtents.X, -halfExtents.Y, -halfExtents.Z))
								};
								// Define the lines to be drawn between vertices
								std::vector<std::pair<int, int>> lineIndices = {
									{0, 1}, {1, 3}, {3, 2}, {2, 0}, // Top face
									{4, 5}, {5, 7}, {7, 6}, {6, 4}, // Bottom face
									{0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical edges
								};

								std::vector<ImVec2> lines;
								for (const auto& lineIndex : lineIndices) {
									FVector2D screenStart, screenEnd;
									if (sdk::WorldToScreen(points[lineIndex.first], screenStart) && sdk::WorldToScreen(points[lineIndex.second], screenEnd)) {
										lines.push_back(screenStart);
										lines.push_back(screenEnd);
									}
								}

								if (!lines.empty()) {
									for (size_t k = 0; k < lines.size(); k += 2) {
										Drawing::DrawLine(lines[k], lines[k + 1], 0xFFFFFFFF, 1);
									}
								}

								break;
							}
							case PxGeometryType::eSPHERE:
							{
								break;
								PxSphereGeometry Sphere = geometryHolder.sphere();
								FTransform t_ShapeTransform = FTransform(PxQuatToFQuat(ShapeTransform.q), PxVec3ToFVector(ShapeTransform.p));
								FTransform t_SceneTransform = FTransform(PxQuatToFQuat(SceneTransform.q), PxVec3ToFVector(SceneTransform.p));
								FVector center = t_SceneTransform.GetTranslation() + t_SceneTransform.GetRotation().RotateVector(t_ShapeTransform.GetTranslation());
								float radius = Sphere.radius;
								FVector2D screen;
								if (sdk::WorldToScreen(center, screen))
								{
									Drawing::DrawCircleFilled(screen, 3.5f, 0xFF0000FF);
								}
								break;
							}
							default:
								break;
							}
						}
					}
				}
			}
		}
		catch (...)
		{
			CONSOLE_INFO("DrawRigid Cause Exception!");
		}
		gScene->unlockRead();
	}
	//绘制单个场景对象的形态
	void PhysicsManager::DrawSingleRigid(PxRigidActor* actor)
	{
		if (!IsSceneCreate())
			return;
		if (!actor || !IsRigidValid((IronMan::ptr_t)actor))
			return;
		PxTransform SceneTransform = actor->getGlobalPose();
		auto delta = (PxVec3ToFVector(SceneTransform.p) - LocalPos);
		float dis = delta.Size() / 100.0f;
		//FVector2D screen;
		//if (!sdk::WorldToScreen(PxVec3ToFVector(SceneTransform.p), screen))
		//	continue;

		PxU32 shapeCount = actor->getNbShapes();
		std::vector<PxShape*> shapes(shapeCount);
		actor->getShapes(shapes.data(), shapeCount);
		for (PxShape* shape : shapes) {
			if (shape && IsRigidValid((IronMan::ptr_t)shape))
			{
				PxTransform ShapeTransform = shape->getLocalPose();
				PxGeometryHolder geometryHolder = shape->getGeometry();
				switch (geometryHolder.getType()) {
				case PxGeometryType::eTRIANGLEMESH:
				{
					auto triangleMesh = geometryHolder.triangleMesh();
					auto scale = triangleMesh.scale;
					FQuat totalRotation = PxQuatToFQuat(SceneTransform.q) * PxQuatToFQuat(ShapeTransform.q);
					FVector center = PxVec3ToFVector(SceneTransform.p) + PxQuatToFQuat(SceneTransform.q).RotateVector(PxVec3ToFVector(ShapeTransform.p));

					// 转换所有顶点到世界坐标
					auto VerticesNum = triangleMesh.triangleMesh->getNbVertices();
					auto Vertices = triangleMesh.triangleMesh->getVertices();
					auto Color = 0xFFFFFFFF;

					std::vector<FVector> transformedVertices(VerticesNum);

					for (size_t v = 0; v < VerticesNum; ++v) {
						PxVec3 vertex = Vertices[v];
						vertex = scale.rotation.rotate(vertex).multiply(scale.scale);
						transformedVertices[v] = center + totalRotation.RotateVector(PxVec3ToFVector(vertex));
					}
					// 绘制每个三角形
					if (triangleMesh.triangleMesh->getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES)
					{
						auto Indices16 = reinterpret_cast<const PxU16*>(triangleMesh.triangleMesh->getTriangles());
						for (size_t t = 0; t < triangleMesh.triangleMesh->getNbTriangles() * 3; t += 3) {
							PxU16 vIndex0 = Indices16[t];
							PxU16 vIndex1 = Indices16[t + 1];
							PxU16 vIndex2 = Indices16[t + 2];

							FVector2D screenPoint0, screenPoint1, screenPoint2;
							bool visible0 = sdk::WorldToScreen(transformedVertices[vIndex0], screenPoint0);
							bool visible1 = sdk::WorldToScreen(transformedVertices[vIndex1], screenPoint1);
							bool visible2 = sdk::WorldToScreen(transformedVertices[vIndex2], screenPoint2);

							if (visible0 && visible1 && visible2) {
								Drawing::DrawLine(screenPoint0, screenPoint1, Color, 0.5f);
								Drawing::DrawLine(screenPoint1, screenPoint2, Color, 0.5f);
								Drawing::DrawLine(screenPoint2, screenPoint0, Color, 0.5f);
							}
						}
					}
					else {
						auto Indices32 = reinterpret_cast<const PxU32*>(triangleMesh.triangleMesh->getTriangles());
						for (size_t t = 0; t < triangleMesh.triangleMesh->getNbTriangles() * 3; t += 3) {
							PxU32 vIndex0 = Indices32[t];
							PxU32 vIndex1 = Indices32[t + 1];
							PxU32 vIndex2 = Indices32[t + 2];

							FVector2D screenPoint0, screenPoint1, screenPoint2;
							bool visible0 = sdk::WorldToScreen(transformedVertices[vIndex0], screenPoint0);
							bool visible1 = sdk::WorldToScreen(transformedVertices[vIndex1], screenPoint1);
							bool visible2 = sdk::WorldToScreen(transformedVertices[vIndex2], screenPoint2);

							if (visible0 && visible1 && visible2) {
								Drawing::DrawLine(screenPoint0, screenPoint1, Color, 0.5f);
								Drawing::DrawLine(screenPoint1, screenPoint2, Color, 0.5f);
								Drawing::DrawLine(screenPoint2, screenPoint0, Color, 0.5f);
							}
						}
					}

					break;
				}
				case PxGeometryType::eCAPSULE:
				{
					break;
					PxCapsuleGeometry Capsule = geometryHolder.capsule();
					const FQuat& Scenerotation = PxQuatToFQuat(SceneTransform.q);

					// Get capsule parameters
					FVector shapeTranslation = PxVec3ToFVector(ShapeTransform.p);
					FVector center = PxVec3ToFVector(SceneTransform.p) + Scenerotation.RotateVector(shapeTranslation);
					float radius = Capsule.radius;
					float halfHeight = Capsule.halfHeight;

					ImU32 ColorRender = ((round(Capsule.radius) == 40.f && (round(Capsule.halfHeight) == 48.f)) || ((round(Capsule.radius) == 40.f && round(Capsule.halfHeight) == 20.f)) || ((round(Capsule.radius) == 20.f && (round(Capsule.halfHeight) == 70.f))) && shapes.size() == 1) ? 0xFF0000FF : 0xFFFFFFFF;
					//if (ColorRender == 0xFF0000FF)
					{
						FVector2D screen1;
						if (sdk::WorldToScreen(PxVec3ToFVector(SceneTransform.p), screen1))
						{
							auto SF = shape->getSimulationFilterData();
							auto QF = shape->getQueryFilterData();
							Drawing::DrawCircleFilled(screen1, 2.5, 0xFF00FF00);
							//Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, screen1 + ImVec2(0, 4), 0xFFFFFFFF, 0xFF000000, 0, "%d|%d|%d|%d", SF.word0, SF.word1, SF.word2, SF.word3);
							//Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, screen1 + ImVec2(0, 14), 0xFFFFFFFF, 0xFF000000, 0, "%d|%d|%d|%d", QF.word0, QF.word1, QF.word2, QF.word3);
						}
					}
					// Compute capsule top and bottom centers
					FVector topCenter = center + Scenerotation.RotateVector(FVector(0, 0, halfHeight));
					FVector bottomCenter = center + Scenerotation.RotateVector(FVector(0, 0, -halfHeight));

					std::vector<ImVec2> pointsTop;
					std::vector<ImVec2> pointsBottom;
					std::vector<ImVec2> pointsCylinder;

					int segments = 24;
					float step = 6.28318530718f / segments;  // Divide circle into segments

					for (float theta = 0; theta < 6.28318530718f; theta += step) {
						// Calculate points around the top and bottom circles
						FVector topPoint = topCenter + Scenerotation.RotateVector(FVector(radius * cosf(theta), radius * sinf(theta), 0));
						FVector bottomPoint = bottomCenter + Scenerotation.RotateVector(FVector(radius * cosf(theta), radius * sinf(theta), 0));

						FVector2D screenTopPoint, screenBottomPoint;
						if (sdk::WorldToScreen(topPoint, screenTopPoint) && sdk::WorldToScreen(bottomPoint, screenBottomPoint))
						{
							pointsTop.push_back(screenTopPoint);
							pointsBottom.push_back(screenBottomPoint);

							// Calculate points for the cylinder lines
							pointsCylinder.push_back(screenTopPoint);
							pointsCylinder.push_back(screenBottomPoint);
						}
					}

					// Draw top and bottom circles
					if (pointsTop.size() > 1) {
						for (size_t i = 0; i < pointsTop.size(); ++i) {
							size_t next = (i + 1) % pointsTop.size();
							Drawing::DrawLine(pointsTop[i], pointsTop[next], ColorRender, 1.5f);
							Drawing::DrawLine(pointsBottom[i], pointsBottom[next], ColorRender, 1.5f);
						}
					}

					// Draw cylinder lines
					if (pointsCylinder.size() > 1) {
						for (size_t i = 0; i < pointsCylinder.size(); i += 2) {
							Drawing::DrawLine(pointsCylinder[i], pointsCylinder[i + 1], ColorRender, 1.5f);
						}
					}

					break;
				}
				case PxGeometryType::eHEIGHTFIELD:
				{
					break;
					// 获取高度场几何信息
					PxHeightFieldGeometry heightField = geometryHolder.heightField();

					// 获取场景和形状的变换
					FTransform t_SceneTransform = FTransform(PxQuatToFQuat(SceneTransform.q), PxVec3ToFVector(SceneTransform.p));
					FTransform t_ShapeTransform = FTransform(PxQuatToFQuat(ShapeTransform.q), PxVec3ToFVector(ShapeTransform.p));

					// 计算中心和总旋转
					FVector center = t_SceneTransform.GetTranslation() + t_SceneTransform.GetRotation().RotateVector(t_ShapeTransform.GetTranslation());
					FQuat totalRotation = t_SceneTransform.GetRotation() * t_ShapeTransform.GetRotation();

					// 获取高度场的行、列和缩放因子
					auto NumRows = heightField.heightField->getNbRows();
					auto NumCols = heightField.heightField->getNbColumns();
					auto HeightScale = heightField.heightScale;
					auto RowScale = heightField.rowScale;
					auto ColScale = heightField.columnScale;

					// 存储顶点
					std::vector<FVector> Vertices;

					// 生成顶点
					for (int32_t Row = 0; Row < NumRows; ++Row) {
						for (int32_t Col = 0; Col < NumCols; ++Col) {
							PxHeightFieldSample Sample = heightField.heightField->getSample(Row, Col);
							float Height = Sample.height * HeightScale;
							FVector Vertex(Col * ColScale, Row * RowScale, Height);
							Vertices.push_back(Vertex);
						}
					}

					// 应用总变换
					for (auto& Vertex : Vertices) {
						//Vertex = Vertex;
						Vertex = Vertex + center;
						//Vertex = totalRotation.RotateVector(Vertex) + center;
					}

					std::vector<ImVec2> ScreenPositions;
					for (const auto& Vertex : Vertices) {
						FVector2D ScreenPosition;
						if (sdk::WorldToScreen(Vertex, ScreenPosition)) {
							ScreenPositions.push_back(ImVec2(ScreenPosition.X, ScreenPosition.Y));
						}
					}

					// 绘制高度场网格
					for (int32_t Row = 0; Row < NumRows - 1; ++Row) {
						for (int32_t Col = 0; Col < NumCols - 1; ++Col) {
							int32_t BottomLeft = Col + Row * NumCols;
							int32_t BottomRight = (Col + 1) + Row * NumCols;
							int32_t TopLeft = Col + (Row + 1) * NumCols;
							int32_t TopRight = (Col + 1) + (Row + 1) * NumCols;

							if (BottomLeft < ScreenPositions.size() && BottomRight < ScreenPositions.size() && TopLeft < ScreenPositions.size() && TopRight < ScreenPositions.size()) {
								// 绘制顶点和连接线
								Drawing::DrawLine(ScreenPositions[BottomLeft], ScreenPositions[BottomRight], 0xFFFFFFFF, 1.5f);
								Drawing::DrawLine(ScreenPositions[BottomRight], ScreenPositions[TopRight], 0xFFFFFFFF, 1.5f);
								Drawing::DrawLine(ScreenPositions[TopRight], ScreenPositions[TopLeft], 0xFFFFFFFF, 1.5f);
								Drawing::DrawLine(ScreenPositions[TopLeft], ScreenPositions[BottomLeft], 0xFFFFFFFF, 1.5f);
							}
						}
					}

					break;
				}
				case PxGeometryType::eCONVEXMESH:
				{
					break;
					auto ConvexMesh = geometryHolder.convexMesh();
					auto Vertices = ConvexMesh.convexMesh->getVertices();
					auto IndexBuffer = ConvexMesh.convexMesh->getIndexBuffer();
					auto VerticeSize = ConvexMesh.convexMesh->getNbVertices();
					auto PolygonSize = ConvexMesh.convexMesh->getNbPolygons();
					FTransform t_ShapeTransform = FTransform(PxQuatToFQuat(ShapeTransform.q), PxVec3ToFVector(ShapeTransform.p));

					FVector center = PxVec3ToFVector(SceneTransform.p) + PxQuatToFQuat(SceneTransform.q).RotateVector(t_ShapeTransform.GetTranslation());
					FQuat totalRotation = PxQuatToFQuat(SceneTransform.q) * t_ShapeTransform.GetRotation();

					// 转换所有顶点到世界坐标
					std::vector<FVector> transformedVertices(VerticeSize);
					for (size_t v = 0; v < VerticeSize; ++v) {
						transformedVertices[v] = center + totalRotation.RotateVector(PxVec3ToFVector(Vertices[v]));
					}



					// 绘制每个多边形
					for (int i = 0; i < PolygonSize; i++) {
						PxHullPolygon polygon;
						auto bget = ConvexMesh.convexMesh->getPolygonData(i, polygon);
						for (size_t p = 0; p < polygon.mNbVerts; ++p) {
							PxU8 vIndex0 = IndexBuffer[polygon.mIndexBase + p];
							PxU8 vIndex1 = IndexBuffer[polygon.mIndexBase + (p + 1) % polygon.mNbVerts];

							// 检查索引是否越界
							if (vIndex0 >= transformedVertices.size() || vIndex1 >= transformedVertices.size()) {
								continue; // 跳过非法索引
							}

							FVector2D screenPoint0, screenPoint1;
							bool visible0 = sdk::WorldToScreen(transformedVertices[vIndex0], screenPoint0);
							bool visible1 = sdk::WorldToScreen(transformedVertices[vIndex1], screenPoint1);

							if (visible0 && visible1) {
								Drawing::DrawLine(screenPoint0, screenPoint1, 0xFFFFFFFF, 1.5f);
							}
						}
					}
					break;
				}
				case PxGeometryType::eBOX:
				{
					break;
					auto Box = geometryHolder.box();
					FQuat sceneRotation = PxQuatToFQuat(SceneTransform.q);
					FQuat shapeRotation = PxQuatToFQuat(ShapeTransform.q);
					FVector halfExtents = PxVec3ToFVector(Box.halfExtents);
					FVector shapeTranslation = PxVec3ToFVector(ShapeTransform.p);
					FVector center = PxVec3ToFVector(SceneTransform.p) + sceneRotation.RotateVector(shapeTranslation);
					if (actor->is<PxRigidDynamic>())
					{
						FVector2D screen1, screen2;
						if (sdk::WorldToScreen(PxVec3ToFVector(SceneTransform.p), screen1))
						{
							Drawing::DrawCircleFilled(screen1, 2.5, 0xFFFFFFFF);
						}
						if (sdk::WorldToScreen(center, screen2))
						{
							auto SF = shape->getSimulationFilterData();
							auto QF = shape->getQueryFilterData();
							Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, screen2, 0xFFFFFFFF, 0xFF000000, FONT_LEFT, "[%d|%d|%d|%d]", SF.word0, SF.word1, SF.word2, SF.word3);
							Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, screen2 + ImVec2(0, 14), 0xFF00FF00, 0xFF000000, FONT_LEFT, "[%d|%d|%d|%d]", QF.word0, QF.word1, QF.word2, QF.word3);
							//Drawing::DrawCircleFilled(screen2, 2.5, 0xFF00FF00);
						}
					}

					FQuat totalRotation = sceneRotation * shapeRotation;

					std::vector<FVector> points = {
						center + totalRotation.RotateVector(FVector(halfExtents.X, halfExtents.Y, halfExtents.Z)),
						center + totalRotation.RotateVector(FVector(halfExtents.X, halfExtents.Y, -halfExtents.Z)),
						center + totalRotation.RotateVector(FVector(halfExtents.X, -halfExtents.Y, halfExtents.Z)),
						center + totalRotation.RotateVector(FVector(halfExtents.X, -halfExtents.Y, -halfExtents.Z)),
						center + totalRotation.RotateVector(FVector(-halfExtents.X, halfExtents.Y, halfExtents.Z)),
						center + totalRotation.RotateVector(FVector(-halfExtents.X, halfExtents.Y, -halfExtents.Z)),
						center + totalRotation.RotateVector(FVector(-halfExtents.X, -halfExtents.Y, halfExtents.Z)),
						center + totalRotation.RotateVector(FVector(-halfExtents.X, -halfExtents.Y, -halfExtents.Z))
					};
					// Define the lines to be drawn between vertices
					std::vector<std::pair<int, int>> lineIndices = {
						{0, 1}, {1, 3}, {3, 2}, {2, 0}, // Top face
						{4, 5}, {5, 7}, {7, 6}, {6, 4}, // Bottom face
						{0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical edges
					};

					std::vector<ImVec2> lines;
					for (const auto& lineIndex : lineIndices) {
						FVector2D screenStart, screenEnd;
						if (sdk::WorldToScreen(points[lineIndex.first], screenStart) && sdk::WorldToScreen(points[lineIndex.second], screenEnd)) {
							lines.push_back(screenStart);
							lines.push_back(screenEnd);
						}
					}

					if (!lines.empty()) {
						for (size_t k = 0; k < lines.size(); k += 2) {
							Drawing::DrawLine(lines[k], lines[k + 1], 0xFFFFFFFF, 1);
						}
					}

					break;
				}
				case PxGeometryType::eSPHERE:
				{
					break;
					PxSphereGeometry Sphere = geometryHolder.sphere();
					FTransform t_ShapeTransform = FTransform(PxQuatToFQuat(ShapeTransform.q), PxVec3ToFVector(ShapeTransform.p));
					FTransform t_SceneTransform = FTransform(PxQuatToFQuat(SceneTransform.q), PxVec3ToFVector(SceneTransform.p));
					FVector center = t_SceneTransform.GetTranslation() + t_SceneTransform.GetRotation().RotateVector(t_ShapeTransform.GetTranslation());
					float radius = Sphere.radius;
					FVector2D screen;
					if (sdk::WorldToScreen(center, screen))
					{
						Drawing::DrawCircleFilled(screen, 3.5f, 0xFF0000FF);
					}
					break;
				}
				default:
					break;
				}
			}
		}
	}
}