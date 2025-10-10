#pragma once
#pragma warning(disable:4172)
#include "CoreUObject/Object.h"
#include "CoreUObject/Class.h"
#include "CoreUObject/Parameters.h"
#include "UnrealNames.h"
#include "ObjectsStore.h"
#include "UEncrypt.h"
#include "../Math/Vector.hpp"
#include "../Math/Matrix.hpp"
#include "../Math/Math.h"
namespace IronMan::Core::SDK
{
	struct FSkinMapData
	{
	public:
		FName SkinItemID; /* Ofs: 0x0 Size: 0x8 NameProperty TslGame.ReplicatedSkinParam.SkinItemID */
		std::string SkinItemName; /* Ofs: 0x8 Size: 0x4 IntProperty TslGame.ReplicatedSkinParam.SkinOwnerPlayerId */
		FName ItemSkinID;
		inline FSkinMapData(FName a, std::string b, FName c)
		{
			SkinItemID = a;
			SkinItemName = b;
			ItemSkinID = c;
		}
	};

	using namespace Math;

	extern float DeltaTime;

	enum Bone : int
	{
		forehead,
		Head,
		neck_01,
		spine_03,
		spine_02,
		spine_01,
		upperarm_l,
		upperarm_r,
		lowerarm_l,
		lowerarm_r,
		hand_l,
		hand_r,
		thumb_01_l,
		thumb_01_r,
		thigh_l,
		thigh_r,
		calf_l,
		calf_r,
		foot_l,
		foot_r,
		ik_foot_root,
		ik_foot_l,
		ik_foot_r
	};

	struct TargetInfo
	{
		float Fov;
		std::shared_ptr<AActor> Target;
		std::shared_ptr<AActor> LastTarget;
	};

	class sdk
	{
		friend class ObjectManager;
	public:
		static bool Initialize();
		static void updateHandle();

		static void UpdateViewPortVEH();
		static DWORD UpdateViewPort(LPVOID Param);
		static void UpdateSelfPlayerVEH();
		static DWORD UpdateSelfPlayer(LPVOID Param);
		static void UpdateRadarToServerVEH();
		static DWORD UpdateRadarToServer(LPVOID Param);
		//static void UpdateObjectThreadVEH();
		//static DWORD UpdateObjectThread(LPVOID Param);
		static DWORD UpdateDMADataThread(LPVOID Param);
		static DWORD UpdateKeys(LPVOID Param);
		inline static auto& GetHandle1() { return *(void**)((ptr_t)_Scatter_buffer + 0); };
		inline static auto& GetHandle2() { return *(void**)((ptr_t)_Scatter_buffer + 0x10); };
		inline static auto& GetUpdateData() { return *(void**)((ptr_t)_Scatter_buffer + 0x20); };
		inline static auto& GetSelfUpdateHandle() { return *(void**)((ptr_t)_Scatter_buffer + 0x30); };
		inline static auto& GetViewPortHandle() { return *(void**)((ptr_t)_Scatter_buffer + 0x40); };
		inline static auto& GetUpdatePlayer() { return *(void**)((ptr_t)_Scatter_buffer + 0x50); };

		inline static auto& GetWorld()
		{
			unique_readguard<WfirstRWLock> rlock(mWorldLock);
			return mWorld;
		}
		inline static auto& GetWeapon()
		{
			unique_readguard<WfirstRWLock> rlock(mWeaponLock);
			return mWeapon;
		}
		inline static auto& GetWeaponNum1()
		{
			unique_readguard<WfirstRWLock> rlock(mWeaponNum1Lock);
			return mWeaponNum1;
		}
		inline static auto& GetWeaponNum2()
		{
			unique_readguard<WfirstRWLock> rlock(mWeaponNum2Lock);
			return mWeaponNum1;
		}
		inline static auto& GetMesh3P()
		{
			unique_readguard<WfirstRWLock> rlock(mMesh3PLock);
			return mMesh3P;
		}
		inline static auto& GetMesh()
		{
			unique_readguard<WfirstRWLock> rlock(mMeshLock);
			return mMesh;
		}
		inline static auto& GetControl()
		{
			unique_readguard<WfirstRWLock> rlock(mControlLock);
			return mControl;
		}
		inline static auto& GetMyHUD()
		{
			unique_readguard<WfirstRWLock> rlock(m_MyHUDLock);
			return m_MyHUD;
		}
		inline static auto& GetPlayer()
		{
			unique_readguard<WfirstRWLock> rlock(m_playerLock);
			return m_player;
		}
		inline static ptr_t& GetWheeledVehiclePlayer()
		{
			unique_readguard<WfirstRWLock> rlock(m_WheeledVehicleplayerLock);
			return m_WheeledVehicleplayer;
		}
		inline static ptr_t& GetFloatingVehiclePlayer()
		{
			unique_readguard<WfirstRWLock> rlock(m_FloatingVehicleplayerLock);
			return m_FloatingVehicleplayer;
		}
		inline static ptr_t& GetMortarPlayer()
		{
			unique_readguard<WfirstRWLock> rlock(m_MortarplayerLock);
			return m_Mortarplayer;
		}

		inline static ptr_t& GetAimTarget()
		{
			unique_readguard<WfirstRWLock> rlock(m_targetLock);
			return m_target;
		}

		inline static std::vector<FWeaponAttachmentData>& GetWeponAttachmentData()
		{
			unique_readguard<WfirstRWLock> rlock(m_WeaponAttachmentDataLock);
			return m_WeaponAttachmentData;
		}
		inline static std::vector<Inventory_Equipment>& GetInventory_Equipment()
		{
			unique_readguard<WfirstRWLock> rlock(m_InventoryEquipmentLock);
			return m_InventoryEquipment;
		}

		inline static FTransform& GetFiringTransform()
		{
			unique_readguard<WfirstRWLock> rlock(m_FiringTransformLock);
			return m_FiringTransform;
		}

		inline static FTransform& GetScopingTransform()
		{
			unique_readguard<WfirstRWLock> rlock(m_ScopingTransformLock);
			return m_ScopingTransform;
		}

		inline static FVector& GetLerpSafetyZonePosition()
		{
			unique_readguard<WfirstRWLock> rlock(m_LerpSafetyZonePositionLock);
			return m_LerpSafetyZonePosition;
		}

		inline static float& GetLerpSafetyZoneRadius()
		{
			unique_readguard<WfirstRWLock> rlock(m_LerpSafetyZoneRadiusLock);
			return m_LerpSafetyZoneRadius;
		}

		inline static FVector& GetLerpBlueZonePosition()
		{
			unique_readguard<WfirstRWLock> rlock(m_LerpBlueZonePositionLock);
			return m_LerpBlueZonePosition;
		}

		inline static float& GetLerpBlueZoneRadius()
		{
			unique_readguard<WfirstRWLock> rlock(m_LerpBlueZoneRadiusLock);
			return m_LerpBlueZoneRadius;
		}

		inline static FVector& GetRedZonePosition()
		{
			unique_readguard<WfirstRWLock> rlock(m_RedZonePositionLock);
			return m_RedZonePosition;
		}

		inline static float& GetRedZoneRadius()
		{
			unique_readguard<WfirstRWLock> rlock(m_RedZoneRadiusLock);
			return m_RedZoneRadius;
		}

		inline static FVector& GetBlackZonePosition()
		{
			unique_readguard<WfirstRWLock> rlock(m_BlackZonePositionLock);
			return m_BlackZonePosition;
		}

		inline static float& GetBlackZoneRadius()
		{
			unique_readguard<WfirstRWLock> rlock(m_BlackZoneRadiusLock);
			return m_BlackZoneRadius;
		}

		inline static FVector& GetSafetyZonePosition()
		{
			unique_readguard<WfirstRWLock> rlock(m_SafetyZonePositionLock);
			return m_SafetyZonePosition;
		}

		inline static float& GetSafetyZoneRadius()
		{
			unique_readguard<WfirstRWLock> rlock(m_SafetyZoneRadiusLock);
			return m_SafetyZoneRadius;
		}

		inline static int& GetNumAliveTeams()
		{
			unique_readguard<WfirstRWLock> rlock(m_NumAliveTeamsLock);
			return m_NumAliveTeams;
		}

		inline static FVector2D& GetWorldLocation()
		{
			unique_readguard<WfirstRWLock> rlock(m_WorldLocationLock);
			return m_WorldLocation;
		}

		inline static TUObjectArray& GetObjectArray()
		{
			unique_readguard<WfirstRWLock> rlock(m_ObjectArrayLock);
			return m_ObjectArray;
		}

		inline static FVector& GetFiringLocation()
		{
			unique_readguard<WfirstRWLock> rlock(m_FiringLocationLock);
			return m_FiringLocation.GetTranslation();
		}

		inline static FTransform& GetFiringLocationTransform()
		{
			unique_readguard<WfirstRWLock> rlock(m_FiringLocationLock);
			return m_FiringLocation;
		}

		inline static POINT& GetCursorPosition()
		{
			unique_readguard<WfirstRWLock> rlock(m_CursorPositionLock);
			return m_CursorPosition;
		}

		inline static auto& GetWeaponName()
		{
			unique_readguard<WfirstRWLock> rlock(m_WeaponNameLock);
			return m_WeaponName;
		}

		inline static FRichCurve_3& GetFloatCurves()
		{
			unique_readguard<WfirstRWLock> rlock(m_FloatCurvesLock);
			return m_FloatCurves;
		}

		inline static std::vector<std::vector<FRichCurveKey>>& GetCurvesKeysArray()
		{
			unique_readguard<WfirstRWLock> rlock(m_CurvesKeysArrayLock);
			return m_CurvesKeysArray;
		}

		inline static float& GetFov()
		{
			unique_readguard<WfirstRWLock> rlock(m_fovLock);
			return m_fov;
		}

		inline static float& GetMaxFov()
		{
			unique_readguard<WfirstRWLock> rlock(m_MaxfovLock);
			return m_Maxfov;
		}

		inline static FVector& GetLocalPos()
		{
			unique_readguard<WfirstRWLock> rlock(m_localPosLock);
			return m_localPos;
		}
		inline static FRotator& GetLocalRot()
		{
			unique_readguard<WfirstRWLock> rlock(m_localRotLock);
			return m_localRot;
		}

		inline static int& GetWeaponIndex()
		{
			unique_readguard<WfirstRWLock> rlock(m_WeaponIndexLock);
			return m_WeaponIndex;
		}

		inline static bool& GetbIsCanted()
		{
			unique_readguard<WfirstRWLock> rlock(m_bIsCantedLock);
			return m_bIsCanted;
		}

		inline static float& GetCustomTimeDilation()
		{
			unique_readguard<WfirstRWLock> rlock(m_CustomTimeDilationLock);
			return m_CustomTimeDilation;
		}

		inline static float& GetTrajectoryGravityZ()
		{
			unique_readguard<WfirstRWLock> rlock(m_TrajectoryGravityZLock);
			return m_TrajectoryGravityZ;
		}

		inline static int& GetCurrentZeroLevel()
		{
			unique_readguard<WfirstRWLock> rlock(m_CurrentZeroLevelLock);
			return m_CurrentZeroLevel;
		}

		inline static bool& GetbSpawnBulletFromBarrel()
		{
			unique_readguard<WfirstRWLock> rlock(m_bSpawnBulletFromBarrelLock);
			return m_bSpawnBulletFromBarrel;
		}

		inline static int& GetSpectatedCount()
		{
			unique_readguard<WfirstRWLock> rlock(m_SpectatedCountLock);
			return m_SpectatedCount;
		}

		inline static int& GetbThirdPerson()
		{
			unique_readguard<WfirstRWLock> rlock(m_bThirdPersonLock);
			return m_bThirdPerson;
		}

		inline static int& GetFireState()
		{
			unique_readguard<WfirstRWLock> rlock(m_FireStateLock);
			return m_FireState;
		}

		inline static int& GetSelectMinimapSizeIndex()
		{
			unique_readguard<WfirstRWLock> rlock(m_SelectMinimapSizeIndexLock);
			return m_SelectMinimapSizeIndex;
		}

		inline static ptr_t& GetLocalSelectMinimapSizeIndex()
		{
			unique_readguard<WfirstRWLock> rlock(m_LocalSelectMinimapSizeIndexLock);
			return m_LocalSelectMinimapSizeIndex;
		}

		inline static float& GetBaseEyeHeight()
		{
			unique_readguard<WfirstRWLock> rlock(m_BaseEyeHeightLock);
			return m_BaseEyeHeight;
		}


		inline static float& GetLastRenderTimeOnScreen()
		{
			unique_readguard<WfirstRWLock> rlock(m_LastRenderTimeOnScreenLock);
			return m_LastRenderTimeOnScreen;
		}

		inline static int& GetIsReloading_CP()
		{
			unique_readguard<WfirstRWLock> rlock(m_IsReloading_CPLock);
			return m_IsReloading_CP;
		}


		inline static bool& GetIsAiming_CP()
		{
			unique_readguard<WfirstRWLock> rlock(m_IsAiming_CPLock);
			return m_IsAiming_CP;
		}

		inline static float& GetScopingAlpha_CP()
		{
			unique_readguard<WfirstRWLock> rlock(m_IsScopingAlpha_CPLock);
			return m_IsScopingAlpha_CP;
		}

		inline static bool& GetIsScoping_CP()
		{
			unique_readguard<WfirstRWLock> rlock(m_bIsScoping_CPLock);
			return m_bIsScoping_CP;
		}

		inline static FRotator& GetControlRotation_CP()
		{
			unique_readguard<WfirstRWLock> rlock(m_ControlRotation_CPLock);
			return m_ControlRotation_CP;
		}

		inline static FRotator& GetControlRotation()
		{
			unique_readguard<WfirstRWLock> rlock(m_ControlRotationLock);
			return m_ControlRotation;
		}

		inline static bool& GetIsDMR()
		{
			unique_readguard<WfirstRWLock> rlock(m_IsDMRLock);
			return m_IsDMR;
		}

		inline static bool& GetIsShotGun()
		{
			unique_readguard<WfirstRWLock> rlock(m_IsShotGunLock);
			return m_IsShotGun;
		}

		inline static bool& GetIsSR()
		{
			unique_readguard<WfirstRWLock> rlock(m_IsSRLock);
			return m_IsSR;
		}

		inline static bool& GetIsGrenade()
		{
			unique_readguard<WfirstRWLock> rlock(m_IsGrenadeLock);
			return m_IsGrenade;
		}

		inline static int& GetWeaponType()
		{
			unique_readguard<WfirstRWLock> rlock(WeaponTypeLock);
			return m_WeaponType;
		}

		inline static FVector& GetComponentLocation()
		{
			unique_readguard<WfirstRWLock> rlock(m_ComponentLocationLock);
			return m_ComponentLocation;
		}


		inline static float& GetBallisticDragScale()
		{
			unique_readguard<WfirstRWLock> rlock(m_BallisticDragScaleLock);
			return m_BallisticDragScale;
		}

		inline static float& GetBallisticDropScale()
		{
			unique_readguard<WfirstRWLock> rlock(m_BallisticDropScaleLock);
			return m_BallisticDropScale;
		}

		inline static bool& InGame()
		{
			unique_readguard<WfirstRWLock> rlock(m_inGameLock);
			return m_inGame;
		}

		inline static FReplicatedSkinParam& GetWeaponSkinParam()
		{
			unique_readguard<WfirstRWLock> rlock(m_WeaponReplicatedSkinLock);
			return m_WeaponReplicatedSkin;
		}

		inline static std::vector<FEuqimentReplicatedSkinItem>& GetHumanEuqiment()
		{
			unique_readguard<WfirstRWLock> rlock(m_HumanBodyRepliedSkinLock);
			return m_HumanBodyRepliedSkin;
		}

		inline static bool IsLocalPlayerAlive()
		{
			unique_readguard<WfirstRWLock> rlock(m_PlayerStateLock);
			return m_PlayerState != 0;
		};

		static __forceinline std::unordered_map < hash_t, std::vector<std::string>> GetVehicleBoneMapName()
		{
			unique_readguard<WfirstRWLock> rlock(_VehicleBoneMapLock);
			return VehicleBoneNameHash;
		}

		static __forceinline void SetVehicleBoneMapName(std::unordered_map < hash_t, std::vector<std::string>> objects)
		{
			unique_writeguard<WfirstRWLock> wlock(_VehicleBoneMapLock);
			VehicleBoneNameHash = objects;
			objects.clear();
		}

		inline static FVector2D& GetMouseSensitivity()
		{
			unique_readguard<WfirstRWLock> rlock(m_MouseSensitivityLock);
			return m_MouseSensitivity;
		}

		inline static FVector& GetLocalHeadPos()
		{
			unique_readguard<WfirstRWLock> rlock(m_LocalHeadPosLock);
			return m_LocalHeadPos;
		}

		static __forceinline void SetLocalHeadPos(FVector Object)
		{
			if (m_LocalHeadPos != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_LocalHeadPosLock);
				m_LocalHeadPos = Object;
			}
		}

		inline static float& GetDamageDelta()
		{
			unique_readguard<WfirstRWLock> rlock(m_DamageDeltaLock);
			return m_DamageDelta;
		}

		inline static bool& GetbMapOpen()
		{
			unique_readguard<WfirstRWLock> rlock(m_bMapOpenLock);
			return m_bMapOpenVar;
		}

		inline static BYTE& GetSkydiveState()
		{
			unique_readguard<WfirstRWLock> rlock(m_SkydiveStateLock);
			return m_SkydiveState;
		}

		inline static ptr_t& GetMousePosPtr()
		{
			unique_readguard<WfirstRWLock> rlock(m_MousePosPtrLock);
			return m_MousePosPtr;
		}

		inline static ptr_t& GetViewTarget_POV_FOV()
		{
			unique_readguard<WfirstRWLock> rlock(m_ViewTarget_POV_FOVLock);
			return m_ViewTarget_POV_FOV;
		}

		inline static ptr_t& GetViewTarget_POV_Location()
		{
			unique_readguard<WfirstRWLock> rlock(m_ViewTarget_POV_LocationLock);
			return m_ViewTarget_POV_Location;
		}

		inline static ptr_t& GetViewTarget_POV_Rotation()
		{
			unique_readguard<WfirstRWLock> rlock(m_ViewTarget_POV_RotationLock);
			return m_ViewTarget_POV_Rotation;
		}

		inline static ptr_t& GetLocalRoot()
		{
			unique_readguard<WfirstRWLock> rlock(m_LocalRootLock);
			return m_LocalRoot;
		}

		inline static ptr_t& GetLocalAnimScriptInstance()
		{
			unique_readguard<WfirstRWLock> rlock(m_LocalAnimScriptInstanceLock);
			return m_LocalAnimScriptInstance;
		}

		inline static std::string& GetWorldName()
		{
			unique_readguard<WfirstRWLock> rlock(m_WorldNameLock);
			return m_WorldName;
		}

		inline static ptr_t& GetMesh3P_ComponentSpaceTransformsArrayPtr()
		{
			unique_readguard<WfirstRWLock> rlock(m_Mesh3P_ComponentSpaceTransformsArrayPtrLock);
			return m_Mesh3P_ComponentSpaceTransformsArrayPtr;
		}

		inline static void*& GetFiringAttachPoint_Socket()
		{
			unique_readguard<WfirstRWLock> rlock(m_FiringAttachPoint_SocketLock);
			return m_FiringAttachPoint_SocketVars;
		}

		inline static void*& GetScopingAttachPoint_Socket()
		{
			unique_readguard<WfirstRWLock> rlock(m_ScopingAttachPoint_SocketLock);
			return m_ScopingAttachPoint_SocketVars;
		}

		inline static void*& GetLockTarget()
		{
			unique_readguard<WfirstRWLock> rlock(LockTargetLock);
			return LockTarget;
		}

		static __forceinline void SetLockTarget(void* Object)
		{
			if (LockTarget != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(LockTargetLock);
				LockTarget = Object;
			}
		}

		inline static auto& GetLanguageType()
		{
			unique_readguard<WfirstRWLock> rlock(m_LanguageTypeLock);
			return m_LanguageType;
		}

		inline static auto& GetFiringSocket_BoneIndex()
		{
			unique_readguard<WfirstRWLock> rlock(m_FiringSocket_BoneIndexLock);
			return m_FiringSocket_BoneIndex;
		}

		inline static auto& GetScopingSocket_BoneIndex()
		{
			unique_readguard<WfirstRWLock> rlock(m_ScopingSocket_BoneIndexLock);
			return m_ScopingSocket_BoneIndex;
		}

		inline static auto& GetLastTeamNum()
		{
			unique_readguard<WfirstRWLock> rlock(m_LastTeamNumLock);
			return m_LastTeamNum;
		}
		
		inline static TMap& GetAttachedStaticComponentMap() { return m_AttachedStaticComponentMap; }
		inline static bool& GetAttachedStaticComponentMapValid() { return m_AttachedStaticComponentMapValid; }
		inline static FWeaponTrajectoryConfig& GetWeaponTrajectoryConfig() { return m_WeaponTrajectoryConfig; }


		inline static std::shared_ptr<AActor> GetTarget(UObjectType type = UObjectType::EPlayer) { return m_targets[type].Target; }
		inline static void* GetTargetPtr(UObjectType type = UObjectType::EPlayer) { return m_targets[type].Target ? m_targets[type].Target->GetPtr() : nullptr; }
		inline static void* GetLastTargetPtr(UObjectType type = UObjectType::EPlayer) { return m_targets[type].LastTarget ? m_targets[type].LastTarget->GetPtr() : nullptr; }

		template<UObjectType type>
		static std::vector<std::shared_ptr<AActor>>& GetActors() { return m_usefulObjects[type]; }

		static bool WorldToScreen2(const FVector& worldPos, FVector2D& screenPos);
		static bool WorldToScreen(const FVector& worldPos, FVector2D& screenPos);
		static bool WorldToScreen(const FVector& worldPos, const FRotator& rotation, FVector2D& screenPos);
		static FVector2D WorldToScreen(const FVector& worldPos);
		static FVector2D WorldToScreen(const FVector& worldPos, const FRotator& rotation);
		static bool GetBoneTable1(ATslCharacter& player, VMMDLL_SCATTER_HANDLE scatterHandle);
		static bool GetBoneTable2(ATslCharacter& player, VMMDLL_SCATTER_HANDLE scatterHandle, bool Visible);
		static bool GetBoneTable3(ATslCharacter& player, VMMDLL_SCATTER_HANDLE scatterHandle, bool Visible);
		static bool GetVehicldeBoneTable1(ATslCharacter& player, VMMDLL_SCATTER_HANDLE scatterHandle);
		static bool GetVehicldeBoneTable2(ATslCharacter& player, VMMDLL_SCATTER_HANDLE scatterHandle, std::vector<int> NeedVechile);
		static bool GetVehicldeBoneTable3(ATslCharacter& player, VMMDLL_SCATTER_HANDLE scatterHandle, std::vector<int> NeedVechile);
		static FVector GetBonePosition(ATslCharacter& player, int boneIndex);
		static bool GetBoneScreenPosition(ATslCharacter& player, FVector2D& result, int boneIndex);
		static void GetComponentRotation(FQuat BoxVec1, FQuat BoxVec2, FQuat& Out);

		static bool PlayerVisibleCheck(ATslCharacter& player);
		static int GetActorBoneBySex(ATslCharacter& player, int bone);
		static FVector2D GetMapPos(FVector ActorLocation, int X, int Y, bool Zone = false);

		static __forceinline void SetAimTarget(void* target)
		{
			unique_writeguard<WfirstRWLock> wlock(m_targetLock);
			m_target = (ptr_t)target;
		}
		static void ClearAimTarget();
	private:
		static __forceinline void SetWorld(void* world)
		{
			if ((ptr_t)world != mWorld)
			{
				unique_writeguard<WfirstRWLock> wlock(mWorldLock);
				mWorld = (ptr_t)world;
			}
		}
		static __forceinline void SetWeapon(void* weapon)
		{
			if ((ptr_t)weapon != mWeapon)
			{
				unique_writeguard<WfirstRWLock> wlock(mWeaponLock);
				mWeapon = (ptr_t)weapon;
			}
		}
		static __forceinline void SetWeaponNum1(void* weapon)
		{
			if ((ptr_t)weapon != mWeaponNum1)
			{
				unique_writeguard<WfirstRWLock> wlock(mWeaponNum1Lock);
				mWeaponNum1 = (ptr_t)weapon;
			}

		}
		static __forceinline void SetWeaponNum2(void* weapon)
		{
			if ((ptr_t)weapon != mWeaponNum2)
			{
				unique_writeguard<WfirstRWLock> wlock(mWeaponNum2Lock);
				mWeaponNum2 = (ptr_t)weapon;
			}

		}
		static __forceinline void SetWeaponMesh3P(void* Mesh3P)
		{
			if ((ptr_t)Mesh3P != mMesh3P)
			{
				unique_writeguard<WfirstRWLock> wlock(mMesh3PLock);
				mMesh3P = (ptr_t)Mesh3P;
			}

		}
		static __forceinline void SetMesh(void* Mesh)
		{
			if ((ptr_t)Mesh != mMesh)
			{
				unique_writeguard<WfirstRWLock> wlock(mMeshLock);
				mMesh = (ptr_t)Mesh;
			}
		}
		static __forceinline void SetControl(void* control)
		{
			if ((ptr_t)control != mControl)
			{
				unique_writeguard<WfirstRWLock> wlock(mControlLock);
				mControl = (ptr_t)control;
			}
		}
		static __forceinline void SetMyHUD(void* HUD)
		{
			if ((ptr_t)HUD != m_MyHUD)
			{
				unique_writeguard<WfirstRWLock> wlock(m_MyHUDLock);
				m_MyHUD = (ptr_t)HUD;
			}
		}
		static __forceinline void SetPlayer(void* player)
		{
			if ((ptr_t)player != m_player)
			{
				unique_writeguard<WfirstRWLock> wlock(m_playerLock);
				m_player = (ptr_t)player;
			}
		}
		static __forceinline void SetWheeledVehiclePlayer(void* player)
		{
			if ((ptr_t)player != m_WheeledVehicleplayer)
			{
				unique_writeguard<WfirstRWLock> wlock(m_WheeledVehicleplayerLock);
				m_WheeledVehicleplayer = (ptr_t)player;
			}
		}
		static __forceinline void SetFloatingVehiclePlayer(void* player)
		{
			if ((ptr_t)player != m_FloatingVehicleplayer)
			{
				unique_writeguard<WfirstRWLock> wlock(m_FloatingVehicleplayerLock);
				m_FloatingVehicleplayer = (ptr_t)player;
			}
		}
		static __forceinline void SetMortarPlayer(void* player)
		{
			if ((ptr_t)m_Mortarplayer != m_Mortarplayer)
			{
				unique_writeguard<WfirstRWLock> wlock(m_MortarplayerLock);
				m_Mortarplayer = (ptr_t)player;
			}
		}
		static __forceinline void SetWeaponAttachmentData(std::vector<FWeaponAttachmentData> Object)
		{
			if (Object.size() != m_WeaponAttachmentData.size() || memcmp(Object.data(), m_WeaponAttachmentData.data(), Object.size() * sizeof(FWeaponAttachmentData)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_WeaponAttachmentDataLock);
				m_WeaponAttachmentData = Object;
			}
		}
		static __forceinline void SetInventoryEquipment(std::vector<Inventory_Equipment> Object)
		{
			if (Object.size() != m_InventoryEquipment.size() || memcmp(Object.data(), m_InventoryEquipment.data(), Object.size() * sizeof(Inventory_Equipment)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_InventoryEquipmentLock);
				m_InventoryEquipment = Object;
			}
		}
		static __forceinline void SetFiringTransform(FTransform Object)
		{
			if (memcmp(&Object, &m_FiringTransform, sizeof(FTransform)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_FiringTransformLock);
				m_FiringTransform = Object;
			}
		}
		static __forceinline void SetScopingTransform(FTransform Object)
		{
			if (memcmp(&Object, &m_FiringTransform, sizeof(FTransform)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_ScopingTransformLock);
				m_ScopingTransform = Object;
			}
		}

		static __forceinline void SetLerpSafetyZonePosition(FVector Object)
		{
			if (memcmp(&Object, &m_LerpSafetyZonePosition, sizeof(FVector)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_LerpSafetyZonePositionLock);
				m_LerpSafetyZonePosition = Object;
			}
		}
		static __forceinline void SetLerpSafetyZoneRadius(float Object)
		{
			if (Object != m_LerpSafetyZoneRadius)
			{
				unique_writeguard<WfirstRWLock> wlock(m_LerpSafetyZoneRadiusLock);
				m_LerpSafetyZoneRadius = Object;
			}
		}
		static __forceinline void SetLerpBlueZonePosition(FVector Object)
		{
			if (memcmp(&Object, &m_LerpBlueZonePosition, sizeof(FVector)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_LerpBlueZonePositionLock);
				m_LerpBlueZonePosition = Object;
			}
		}
		static __forceinline void SetLerpBlueZoneRadius(float Object)
		{
			if (Object != m_LerpBlueZoneRadius)
			{
				unique_writeguard<WfirstRWLock> wlock(m_LerpBlueZoneRadiusLock);
				m_LerpBlueZoneRadius = Object;
			}
		}
		static __forceinline void SetRedZonePosition(FVector Object)
		{
			if (memcmp(&Object, &m_RedZonePosition, sizeof(FVector)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_RedZonePositionLock);
				m_RedZonePosition = Object;
			}
		}
		static __forceinline void SetRedZoneRadius(float Object)
		{
			if (Object != m_RedZoneRadius)
			{
				unique_writeguard<WfirstRWLock> wlock(m_RedZoneRadiusLock);
				m_RedZoneRadius = Object;
			}
		}
		static __forceinline void SetBlackZonePosition(FVector Object)
		{
			if (memcmp(&Object, &m_BlackZonePosition, sizeof(FVector)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_BlackZonePositionLock);
				m_BlackZonePosition = Object;
			}
		}
		static __forceinline void SetBlackZoneRadius(float Object)
		{
			if (Object != m_BlackZoneRadius)
			{
				unique_writeguard<WfirstRWLock> wlock(m_BlackZoneRadiusLock);
				m_BlackZoneRadius = Object;
			}
		}
		static __forceinline void SetSafetyZonePosition(FVector Object)
		{
			if (memcmp(&Object, &m_SafetyZonePosition, sizeof(FVector)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_SafetyZonePositionLock);
				m_SafetyZonePosition = Object;
			}
		}
		static __forceinline void SetSafetyZoneRadius(float Object)
		{
			if (Object != m_SafetyZoneRadius)
			{
				unique_writeguard<WfirstRWLock> wlock(m_SafetyZoneRadiusLock);
				m_SafetyZoneRadius = Object;
			}
		}
		static __forceinline void SetNumAliveTeams(int Object)
		{
			if (Object != m_NumAliveTeams)
			{
				unique_writeguard<WfirstRWLock> wlock(m_NumAliveTeamsLock);
				m_NumAliveTeams = Object;
			}
		}
		static __forceinline void SetWorldLocation(FVector2D Object)
		{
			if (memcmp(&Object, &m_WorldLocation, sizeof(FVector2D)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_WorldLocationLock);
				m_WorldLocation = Object;
			}
		}
		static __forceinline void SetObjectArray(TUObjectArray Object)
		{
			if (memcmp(&Object, &m_ObjectArray, sizeof(TUObjectArray)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_ObjectArrayLock);
				m_ObjectArray = Object;
			}
		}

		static __forceinline void SetFiringLocation(FTransform Object)
		{
			if (memcmp(&Object, &m_FiringLocation, sizeof(FTransform)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_FiringLocationLock);
				m_FiringLocation = Object;
			}
		}
		static __forceinline void SetCursorPosition(POINT Object)
		{
			if (memcmp(&Object, &m_CursorPosition, sizeof(POINT)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_CursorPositionLock);
				m_CursorPosition = Object;
			}
		}
		static __forceinline void SetWeaponName(std::string Object)
		{
			if (Object != m_WeaponName)
			{
				unique_writeguard<WfirstRWLock> wlock(m_WeaponNameLock);
				m_WeaponName = Object;
			}
		}
		static __forceinline void SetFloatCurves(FRichCurve_3 Object)
		{
			if (memcmp(&Object, &m_FloatCurves, sizeof(FRichCurve_3)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_FloatCurvesLock);
				m_FloatCurves = Object;
			}
		}
		static __forceinline void SetCurvesKeysArray(std::vector<std::vector<FRichCurveKey>> Object)
		{
			unique_writeguard<WfirstRWLock> wlock(m_CurvesKeysArrayLock);
			m_CurvesKeysArray = Object;
		}
		static __forceinline void SetFov(float fov)
		{
			unique_writeguard<WfirstRWLock> wlock(m_fovLock);
			m_fov = fov;
		}

		static __forceinline void SetMaxFov(float fov)
		{
			unique_writeguard<WfirstRWLock> wlock(m_MaxfovLock);
			if (fov < 1000.f && fov > 0.f)
				m_Maxfov = std::max(m_Maxfov, fov);
		}

		static __forceinline void SetLocalPos(FVector Pos)
		{
			if (memcmp(&Pos, &m_localPos, sizeof(FVector)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_localPosLock);
				m_localPos = Pos;
			}
		}

		static __forceinline void SetLocalRot(FRotator Rot)
		{
			if (memcmp(&Rot, &m_localRot, sizeof(FRotator)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_localRotLock);
				m_localRot = Rot;
			}
		}
		static __forceinline void SetWeaponIndex(BYTE Index)
		{
			if ((BYTE)m_WeaponIndex != Index)
			{
				unique_writeguard<WfirstRWLock> wlock(m_WeaponIndexLock);
				m_WeaponIndex = Index;
			}
		}
		static __forceinline void SetbIsCanted(bool Index)
		{
			if (m_bIsCanted != Index)
			{
				unique_writeguard<WfirstRWLock> wlock(m_bIsCantedLock);
				m_bIsCanted = Index;
			}
		}
		static __forceinline void SetCustomTimeDilation(float Object)
		{
			if (m_CustomTimeDilation != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_CustomTimeDilationLock);
				m_CustomTimeDilation = Object;
			}
		}
		static __forceinline void SetTrajectoryGravityZ(float Object)
		{
			if (m_TrajectoryGravityZ != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_TrajectoryGravityZLock);
				m_TrajectoryGravityZ = Object;
			}
		}
		static __forceinline void SetCurrentZeroLevel(int Object)
		{
			if (m_CurrentZeroLevel != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_CurrentZeroLevelLock);
				m_CurrentZeroLevel = Object;
			}
		}
		static __forceinline void SetbSpawnBulletFromBarrel(bool Object)
		{
			if (m_bSpawnBulletFromBarrel != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_bSpawnBulletFromBarrelLock);
				m_bSpawnBulletFromBarrel = Object;
			}
		}
		static __forceinline void SetSpectatedCount(int Object)
		{
			if (m_SpectatedCount != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_SpectatedCountLock);
				m_SpectatedCount = Object;
			}
		}
		static __forceinline void SetbThirdPerson(int Object)
		{
			if (m_bThirdPerson != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_bThirdPersonLock);
				m_bThirdPerson = Object;
			}
		}
		static __forceinline void SetFireState(int Object)
		{
			if (m_FireState != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_FireStateLock);
				m_FireState = Object;
			}
		}
		static __forceinline void SetSelectMinimapSizeIndex(int Object)
		{
			if (m_SelectMinimapSizeIndex != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_SelectMinimapSizeIndexLock);
				m_SelectMinimapSizeIndex = Object;
			}
		}
		static __forceinline void SetLocalSelectMinimapSizeIndex(ptr_t Object)
		{
			if (m_LocalSelectMinimapSizeIndex != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_LocalSelectMinimapSizeIndexLock);
				m_LocalSelectMinimapSizeIndex = Object;
			}
		}
		static __forceinline void SetBaseEyeHeight(float Object)
		{
			if (m_BaseEyeHeight != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_BaseEyeHeightLock);
				m_BaseEyeHeight = Object;
			}
		}
		static __forceinline void SetLastRenderTimeOnScreen(float Object)
		{
			if (m_LastRenderTimeOnScreen != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_LastRenderTimeOnScreenLock);
				m_LastRenderTimeOnScreen = Object;
			}
		}
		static __forceinline void SetIsReloading_CP(int Object)
		{
			if (m_IsReloading_CP != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_IsReloading_CPLock);
				m_IsReloading_CP = Object;
			}
		}
		static __forceinline void SetIsAiming_CP(bool Object)
		{
			if (m_IsAiming_CP != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_IsAiming_CPLock);
				m_IsAiming_CP = Object;
			}
		}
		static __forceinline void SetIsScopingAlpha_CP(float Object)
		{
			if (m_IsScopingAlpha_CP != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_IsScopingAlpha_CPLock);
				m_IsScopingAlpha_CP = Object;
			}
		}
		static __forceinline void SetbIsScoping_CP(bool Object)
		{
			if (m_bIsScoping_CP != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_bIsScoping_CPLock);
				m_bIsScoping_CP = Object;
			}
		}
		static __forceinline void SetControlRotation_CP(FRotator Object)
		{
			if (m_ControlRotation_CP != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_ControlRotation_CPLock);
				m_ControlRotation_CP = Object;
			}
		}
		static __forceinline void SetControlRotation(FRotator Object)
		{
			if (m_ControlRotation != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_ControlRotationLock);
				m_ControlRotation = Object;
			}
		}
		static __forceinline void SetIsDMR(bool Object)
		{
			if (m_IsDMR != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_IsDMRLock);
				m_IsDMR = Object;
			}
		}
		static __forceinline void SetIsShotGun(bool Object)
		{
			if (m_IsShotGun != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_IsShotGunLock);
				m_IsShotGun = Object;
			}
		}
		static __forceinline void SetIsSR(bool Object)
		{
			if (m_IsSR != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_IsSRLock);
				m_IsSR = Object;
			}
		}
		static __forceinline void SetIsGrenade(bool Object)
		{
			if (m_IsGrenade != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_IsGrenadeLock);
				m_IsGrenade = Object;
			}
		}
		static __forceinline void SetWeaponType(int Object)
		{
			if (m_WeaponType != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(WeaponTypeLock);
				m_WeaponType = Object;
			}
		}
		static __forceinline void SetComponentLocation(FVector Object)
		{
			if (m_ComponentLocation != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_ComponentLocationLock);
				m_ComponentLocation = Object;
			}
		}
		static __forceinline void SetBallisticDragScale(float Object)
		{
			if (m_BallisticDragScale != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_BallisticDragScaleLock);
				m_BallisticDragScale = Object;
			}
		}
		static __forceinline void SetBallisticDropScale(float Object)
		{
			if (m_BallisticDropScale != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_BallisticDropScaleLock);
				m_BallisticDropScale = Object;
			}
		}
		static __forceinline void SetInGame(bool Object)
		{
			if (m_inGame != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_inGameLock);
				m_inGame = Object;
			}
		}
		static __forceinline void SetWeaponReplicatedSkin(FReplicatedSkinParam Object)
		{
			if (memcmp(&m_WeaponReplicatedSkin, &Object, sizeof(FReplicatedSkinParam)))
			{
				unique_writeguard<WfirstRWLock> wlock(m_WeaponReplicatedSkinLock);
				m_WeaponReplicatedSkin = Object;
			}
		}
		static __forceinline void SetHumanBodyRepliedSkin(std::vector<FEuqimentReplicatedSkinItem> Object)
		{
			unique_writeguard<WfirstRWLock> wlock(m_HumanBodyRepliedSkinLock);
			m_HumanBodyRepliedSkin = Object;
		}
		static __forceinline void SetPlayerState(ptr_t Object)
		{
			if (m_PlayerState != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_PlayerStateLock);
				m_PlayerState = Object;
			}
		}
		static __forceinline void SetMouseSensitivity(FVector2D Object)
		{
			if (m_MouseSensitivity != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_MouseSensitivityLock);
				m_MouseSensitivity = Object;
			}
		}
		static __forceinline void SetDamageDelta(float Object)
		{
			if (m_DamageDelta != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_DamageDeltaLock);
				m_DamageDelta = Object;
			}
		}
		static __forceinline void SetMapOpen(bool Object)
		{
			if (m_bMapOpenVar != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_bMapOpenLock);
				m_bMapOpenVar = Object;
			}
		}

		static __forceinline void SetSkydiveState(BYTE Object)
		{
			if (m_SkydiveState != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_SkydiveStateLock);
				m_SkydiveState = Object;
			}
		}

		static __forceinline void SetMousePosPtr(ptr_t Object)
		{
			if (m_MousePosPtr != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_ViewTarget_POV_FOVLock);
				m_MousePosPtr = Object;
			}
		}

		static __forceinline void SetViewTarget_POV_FOV(ptr_t Object)
		{
			if (m_ViewTarget_POV_FOV != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_ViewTarget_POV_FOVLock);
				m_ViewTarget_POV_FOV = Object;
			}
		}

		static __forceinline void SetViewTarget_POV_Location(ptr_t Object)
		{
			if (m_ViewTarget_POV_Location != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_ViewTarget_POV_LocationLock);
				m_ViewTarget_POV_Location = Object;
			}
		}

		static __forceinline void SetViewTarget_POV_Rotation(ptr_t Object)
		{
			if (m_ViewTarget_POV_Rotation != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_ViewTarget_POV_RotationLock);
				m_ViewTarget_POV_Rotation = Object;
			}
		}

		static __forceinline void SetLocalRoot(ptr_t Object)
		{
			if (m_LocalRoot != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_LocalRootLock);
				m_LocalRoot = Object;
			}
		}

		static __forceinline void SetLocalAnimScriptInstance(ptr_t Object)
		{
			if (m_LocalAnimScriptInstance != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_LocalAnimScriptInstanceLock);
				m_LocalAnimScriptInstance = Object;
			}
		}

		static __forceinline void SetWorldName(std::string Object)
		{
			if (m_WorldName != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_WorldNameLock);
				m_WorldName = Object;
			}
		}

		static __forceinline void SetMesh3P_ComponentSpaceTransformsArrayPtr(ptr_t Object)
		{
			if (m_Mesh3P_ComponentSpaceTransformsArrayPtr != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_Mesh3P_ComponentSpaceTransformsArrayPtrLock);
				m_Mesh3P_ComponentSpaceTransformsArrayPtr = Object;
			}
		}

		static __forceinline void SetFiringAttachPoint_Socket(void* Object)
		{
			if (m_FiringAttachPoint_SocketVars != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_FiringAttachPoint_SocketLock);
				m_FiringAttachPoint_SocketVars = Object;
			}
		}

		static __forceinline void SetScopingAttachPoint_Socket(void* Object)
		{
			if (m_ScopingAttachPoint_SocketVars != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_ScopingAttachPoint_SocketLock);
				m_ScopingAttachPoint_SocketVars = Object;
			}
		}

		static __forceinline void SetLanguageType(int Object)
		{
			if (m_LanguageType != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_LanguageTypeLock);
				m_LanguageType = Object;
			}
		}

		static __forceinline void SetFiringSocket_BoneIndex(int32_t Object)
		{
			if (m_FiringSocket_BoneIndex != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_FiringSocket_BoneIndexLock);
				m_FiringSocket_BoneIndex = Object;
			}
		}

		static __forceinline void SetScopingSocket_BoneIndex(int32_t Object)
		{
			if (m_ScopingSocket_BoneIndex != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_ScopingSocket_BoneIndexLock);
				m_ScopingSocket_BoneIndex = Object;
			}
		}

		static __forceinline void SetLastTeamNum(int Object)
		{
			if (m_LastTeamNum != Object)
			{
				unique_writeguard<WfirstRWLock> wlock(m_LastTeamNumLock);
				m_LastTeamNum = Object;
			}
		}

		static bool IsShotGun();
		static bool IsSR();
		static bool IsGrenade();
		static bool IsDMR();
		static void AimToTarget();
	public:
		static delegate<void()> OnUpdate;
		static delegate<void()> OnPresent;
		static delegate<void()> OnEnd;

	private:

		//需要加锁的
		static WfirstRWLock mWorldLock;
		static ptr_t mWorld;
		static WfirstRWLock mWeaponLock;
		static ptr_t mWeapon;
		static WfirstRWLock mWeaponNum1Lock;
		static ptr_t mWeaponNum1;
		static WfirstRWLock mWeaponNum2Lock;
		static ptr_t mWeaponNum2;
		static WfirstRWLock mMesh3PLock;
		static ptr_t mMesh3P;
		static WfirstRWLock mMeshLock;
		static ptr_t mMesh;
		static WfirstRWLock mControlLock;
		static ptr_t mControl;
		static WfirstRWLock m_MyHUDLock;
		static ptr_t m_MyHUD;
		static WfirstRWLock m_playerLock;
		static ptr_t m_player;
		static WfirstRWLock m_WheeledVehicleplayerLock;
		static ptr_t m_WheeledVehicleplayer;
		static WfirstRWLock m_FloatingVehicleplayerLock;
		static ptr_t m_FloatingVehicleplayer;
		static WfirstRWLock m_MortarplayerLock;
		static ptr_t m_Mortarplayer;
		static WfirstRWLock m_targetLock;
		static ptr_t m_target;

		static WfirstRWLock m_WeaponAttachmentDataLock;
		static std::vector<FWeaponAttachmentData> m_WeaponAttachmentData;
		static WfirstRWLock m_InventoryEquipmentLock;
		static std::vector<Inventory_Equipment> m_InventoryEquipment;

		//自瞄的开火和镜头FTransform
		static WfirstRWLock m_FiringTransformLock;
		static FTransform m_FiringTransform;
		static WfirstRWLock m_ScopingTransformLock;
		static FTransform m_ScopingTransform;

		static WfirstRWLock m_LerpSafetyZonePositionLock;
		static FVector		m_LerpSafetyZonePosition;

		static WfirstRWLock m_LerpSafetyZoneRadiusLock;
		static float	    m_LerpSafetyZoneRadius;

		static WfirstRWLock m_LerpBlueZonePositionLock;
		static FVector		m_LerpBlueZonePosition;

		static WfirstRWLock m_LerpBlueZoneRadiusLock;
		static float	    m_LerpBlueZoneRadius;

		static WfirstRWLock m_RedZonePositionLock;
		static FVector		m_RedZonePosition;

		static WfirstRWLock m_RedZoneRadiusLock;
		static float	    m_RedZoneRadius;

		static WfirstRWLock m_BlackZonePositionLock;
		static FVector		m_BlackZonePosition;

		static WfirstRWLock m_BlackZoneRadiusLock;
		static float	    m_BlackZoneRadius;

		static WfirstRWLock m_SafetyZonePositionLock;
		static FVector		m_SafetyZonePosition;

		static WfirstRWLock m_SafetyZoneRadiusLock;
		static float	    m_SafetyZoneRadius;

		static WfirstRWLock m_NumAliveTeamsLock;
		static int			m_NumAliveTeams;

		static WfirstRWLock m_WorldLocationLock;
		static FVector2D	m_WorldLocation;

		static WfirstRWLock m_ObjectArrayLock;
		static TUObjectArray m_ObjectArray;

		static WfirstRWLock m_FiringLocationLock;
		static FTransform	m_FiringLocation;

		static WfirstRWLock m_CursorPositionLock;
		static POINT m_CursorPosition;

		static WfirstRWLock m_WeaponNameLock;
		static std::string  m_WeaponName;

		static WfirstRWLock m_FloatCurvesLock;
		static FRichCurve_3	m_FloatCurves;

		static WfirstRWLock m_CurvesKeysArrayLock;
		static std::vector<std::vector<FRichCurveKey>>	m_CurvesKeysArray;

		static WfirstRWLock m_fovLock;
		static float	m_fov;

		static WfirstRWLock m_MaxfovLock;
		static float	m_Maxfov;

		static WfirstRWLock m_localPosLock;
		static FVector	m_localPos;

		static WfirstRWLock m_localRotLock;
		static FRotator	m_localRot;

		static WfirstRWLock m_WeaponIndexLock;
		static int		m_WeaponIndex;

		static WfirstRWLock m_bIsCantedLock;
		static bool		m_bIsCanted;

		static WfirstRWLock m_CustomTimeDilationLock;
		static float	m_CustomTimeDilation;

		static WfirstRWLock m_TrajectoryGravityZLock;
		static float	m_TrajectoryGravityZ;

		static WfirstRWLock m_CurrentZeroLevelLock;
		static int		m_CurrentZeroLevel;

		static WfirstRWLock m_bSpawnBulletFromBarrelLock;
		static bool		m_bSpawnBulletFromBarrel;

		static WfirstRWLock m_SpectatedCountLock;
		static int		m_SpectatedCount;

		static WfirstRWLock m_bThirdPersonLock;
		static int		m_bThirdPerson;

		static WfirstRWLock m_FireStateLock;
		static int		m_FireState;

		static WfirstRWLock m_SelectMinimapSizeIndexLock;
		static int		m_SelectMinimapSizeIndex;

		static WfirstRWLock m_LocalSelectMinimapSizeIndexLock;
		static ptr_t m_LocalSelectMinimapSizeIndex;

		static WfirstRWLock m_BaseEyeHeightLock;
		static float	m_BaseEyeHeight;

		static WfirstRWLock m_LastRenderTimeOnScreenLock;
		static float	m_LastRenderTimeOnScreen;

		static WfirstRWLock m_IsReloading_CPLock;
		static int		m_IsReloading_CP;

		static WfirstRWLock m_IsAiming_CPLock;
		static bool		m_IsAiming_CP;

		static WfirstRWLock m_IsScopingAlpha_CPLock;
		static float	m_IsScopingAlpha_CP;

		static WfirstRWLock m_bIsScoping_CPLock;
		static bool		m_bIsScoping_CP;

		static WfirstRWLock m_ControlRotation_CPLock;
		static FRotator	m_ControlRotation_CP;

		static WfirstRWLock m_ControlRotationLock;
		static FRotator	m_ControlRotation;

		static WfirstRWLock m_IsDMRLock;
		static bool		m_IsDMR;
		static WfirstRWLock m_IsShotGunLock;
		static bool		m_IsShotGun;
		static WfirstRWLock m_IsSRLock;
		static bool		m_IsSR;
		static WfirstRWLock m_IsGrenadeLock;
		static bool		m_IsGrenade;

		static WfirstRWLock WeaponTypeLock;
		static int			m_WeaponType;

		static WfirstRWLock m_ComponentLocationLock;
		static FVector	m_ComponentLocation;

		static WfirstRWLock m_BallisticDragScaleLock;
		static float	m_BallisticDragScale;

		static WfirstRWLock m_BallisticDropScaleLock;
		static float	m_BallisticDropScale;

		static WfirstRWLock m_inGameLock;
		static bool		m_inGame;

		static WfirstRWLock m_WeaponReplicatedSkinLock;
		static FReplicatedSkinParam  m_WeaponReplicatedSkin;

		static WfirstRWLock m_HumanBodyRepliedSkinLock;
		static std::vector<FEuqimentReplicatedSkinItem>  m_HumanBodyRepliedSkin;

		static WfirstRWLock m_PlayerStateLock;
		static ptr_t	m_PlayerState;

		static WfirstRWLock _VehicleBoneMapLock;
		static std::unordered_map < hash_t, std::vector<std::string>>VehicleBoneNameHash;

		static WfirstRWLock m_MouseSensitivityLock;
		static FVector2D	m_MouseSensitivity;

		static WfirstRWLock m_LocalHeadPosLock;
		static FVector		m_LocalHeadPos;

		static WfirstRWLock m_DamageDeltaLock;
		static float		m_DamageDelta;

		static WfirstRWLock m_SkydiveStateLock;
		static BYTE m_SkydiveState;

		static WfirstRWLock m_bMapOpenLock;
		static bool m_bMapOpenVar;

		static WfirstRWLock m_MousePosPtrLock;
		static ptr_t m_MousePosPtr;
		//视角各地址
		static WfirstRWLock m_ViewTarget_POV_FOVLock;
		static ptr_t m_ViewTarget_POV_FOV;
		static WfirstRWLock m_ViewTarget_POV_LocationLock;
		static ptr_t m_ViewTarget_POV_Location;
		static WfirstRWLock m_ViewTarget_POV_RotationLock;
		static ptr_t m_ViewTarget_POV_Rotation;

		static WfirstRWLock m_LocalRootLock;
		static ptr_t m_LocalRoot;

		static WfirstRWLock m_LocalAnimScriptInstanceLock;
		static ptr_t m_LocalAnimScriptInstance;

		static WfirstRWLock m_WorldNameLock;
		static std::string m_WorldName;

		static WfirstRWLock m_Mesh3P_ComponentSpaceTransformsArrayPtrLock;
		static ptr_t m_Mesh3P_ComponentSpaceTransformsArrayPtr;

		static WfirstRWLock m_FiringAttachPoint_SocketLock;
		static void* m_FiringAttachPoint_SocketVars;

		static WfirstRWLock m_ScopingAttachPoint_SocketLock;
		static void* m_ScopingAttachPoint_SocketVars;

		static WfirstRWLock LockTargetLock;
		static void* LockTarget;

		static WfirstRWLock m_LanguageTypeLock;
		static int		 m_LanguageType;

		//不需要加锁的
		static FWeaponTrajectoryConfig	m_WeaponTrajectoryConfig;
		//TMap
		static TMap		  m_AttachedStaticComponentMap;
		static bool		  m_AttachedStaticComponentMapValid;





	public:

		//本人RootCompenont
		static void* m_MapGridWidget;
		static void* m_MapGridWidgetSlot;
		static int32_t m_LocalWeaponEncryptIndex;
		static TArray<void*> m_LocalEquipment_Item;
		static UBlockInputUser m_HudBlockInputArray;
		static void* m_SkinMesh3P_SkeletalMesh;


		static TArray<FTransform> m_Mesh3P_ComponentSpaceTransformsArray;
		static int32_t m_SkinMesh3P_CurrentReadComponentTransforms;


		static TArray<void*> m_SkeletalMesh_Sockets;
		static void* m_SkeletalMesh_Skeleton;
		static FReferenceSkeleton m_SkeletalMesh_RefSkeleton;

		static TArray<void*> m_Skeleton_Sockets;



		static FName m_FiringSocket_BoneName;
		static FName m_ScopingSocket_BoneName;

		static WfirstRWLock m_FiringSocket_BoneIndexLock;
		static int32_t m_FiringSocket_BoneIndex;
		static WfirstRWLock m_ScopingSocket_BoneIndexLock;
		static int32_t m_ScopingSocket_BoneIndex;

		static WfirstRWLock m_LastTeamNumLock;
		static int32_t m_LastTeamNum;
	public:
		static std::unordered_map<UObjectType, std::vector<std::shared_ptr<AActor>>> m_usefulObjects;
		static std::unordered_map<UObjectType, std::vector<std::shared_ptr<AActor>>> m_cacheObjectsActor;
		static std::unordered_map<UObjectType, std::vector<std::shared_ptr<AActor>>> m_cacheObjectsAimbot;
		//static std::unordered_map<UObjectType, std::vector<std::shared_ptr<AActor>>> m_cacheObjectsUpdate;
		static std::unordered_map<UObjectType, std::vector<std::shared_ptr<AActor>>> m_cacheObjectsRadar;
	public:
		static std::unordered_map <UObjectType, TargetInfo> m_targets;
	};

}