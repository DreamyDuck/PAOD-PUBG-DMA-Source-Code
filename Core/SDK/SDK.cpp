#include "Include/WinHeaders.hpp"
#include "SDK.h"
#include "ObjectsStore.h"
#include "Core/Visuals.h"
#include "Core/Config.h"
#include "Core/Core.hpp"
#include "ObjectManager.h"
#include "Core/DirectX/DirectX11.h"
#include "../ShareRadar/json.h"
#include "../VehicleManager.h"
#include <random>
#include <ratio>
#include <chrono>
#define ShowEOther

using json = nlohmann::json;

bool bMMapRadarIsOpen;
extern std::string RadarIP;
bool CheckPlayerIsInWhiteListMap(hash_t Name);
bool CheckPlayerIsInBlackListMap(hash_t Name);
namespace IronMan::Core::SDK
{
	//解密血量
	typedef float* (NTAPI* fnDecryptHealth)(float* Encrypt, uint32_t LoopTime, uint32_t XorKeys);
	fnDecryptHealth CallDecryptHealth = NULL;
	void* DecryptHealthPoint = nullptr;

	//玩家所在载具实时刷新
	std::unordered_map<ptr_t, int>VActorMap;
	WfirstRWLock _VArwLock;

	static bool ActorIsInVechile(ptr_t Actor)
	{
		unique_readguard<WfirstRWLock> rlock(_VArwLock);
		return VActorMap.find(Actor) != VActorMap.end();
	}

	static void SetVehicleActor(std::vector<ptr_t> objects)
	{
		unique_writeguard<WfirstRWLock> wlock(_VArwLock);
		if (VActorMap.size())
			VActorMap.clear();
		for (int i = 0; i < objects.size(); i++)
		{
			VActorMap.emplace(objects[i], 1);
		}
	}

	delegate<void()> sdk::OnUpdate;
	delegate<void()> sdk::OnPresent;
	delegate<void()> sdk::OnEnd;

	extern OMFClassBaseChain EPlayerUClass;
	extern OMFClassBaseChain EVehicleUClass;
	extern OMFClassBaseChain EBoatUClass;
	extern OMFClassBaseChain EItemGroupUClass;
	extern OMFClassBaseChain EItemUClass;
	extern OMFClassBaseChain EDeathBoxUClass;
	extern OMFClassBaseChain EAirdropUClass;
	extern OMFClassBaseChain EProjectileUClass;
	extern OMFClassBaseChain EDroppedItemUClass;

	WfirstRWLock sdk::mWorldLock;
	ptr_t sdk::mWorld;

	WfirstRWLock sdk::mWeaponLock;
	ptr_t sdk::mWeapon;

	WfirstRWLock sdk::mWeaponNum1Lock;
	ptr_t sdk::mWeaponNum1;

	WfirstRWLock sdk::mWeaponNum2Lock;
	ptr_t sdk::mWeaponNum2;

	WfirstRWLock sdk::mMesh3PLock;
	ptr_t sdk::mMesh3P;

	WfirstRWLock sdk::mMeshLock;
	ptr_t sdk::mMesh;

	WfirstRWLock sdk::mControlLock;
	ptr_t sdk::mControl;

	WfirstRWLock sdk::m_MyHUDLock;
	ptr_t sdk::m_MyHUD;

	WfirstRWLock sdk::m_playerLock;
	ptr_t sdk::m_player;

	WfirstRWLock sdk::m_WheeledVehicleplayerLock;
	ptr_t sdk::m_WheeledVehicleplayer;

	WfirstRWLock sdk::m_FloatingVehicleplayerLock;
	ptr_t sdk::m_FloatingVehicleplayer;

	WfirstRWLock sdk::m_MortarplayerLock;
	ptr_t sdk::m_Mortarplayer;

	WfirstRWLock sdk::m_targetLock;
	ptr_t sdk::m_target;

	WfirstRWLock sdk::m_WeaponAttachmentDataLock;
	std::vector<FWeaponAttachmentData> sdk::m_WeaponAttachmentData;

	WfirstRWLock sdk::m_InventoryEquipmentLock;
	std::vector<Inventory_Equipment> sdk::m_InventoryEquipment;

	WfirstRWLock sdk::m_FiringTransformLock;
	FTransform sdk::m_FiringTransform;

	WfirstRWLock sdk::m_ScopingTransformLock;
	FTransform sdk::m_ScopingTransform;

	WfirstRWLock sdk::m_LerpSafetyZonePositionLock;
	FVector		sdk::m_LerpSafetyZonePosition;

	WfirstRWLock sdk::m_LerpSafetyZoneRadiusLock;
	float	    sdk::m_LerpSafetyZoneRadius;

	WfirstRWLock sdk::m_LerpBlueZonePositionLock;
	FVector		sdk::m_LerpBlueZonePosition;

	WfirstRWLock sdk::m_LerpBlueZoneRadiusLock;
	float	    sdk::m_LerpBlueZoneRadius;

	WfirstRWLock sdk::m_RedZonePositionLock;
	FVector		sdk::m_RedZonePosition;

	WfirstRWLock sdk::m_RedZoneRadiusLock;
	float	    sdk::m_RedZoneRadius;

	WfirstRWLock sdk::m_BlackZonePositionLock;
	FVector		sdk::m_BlackZonePosition;

	WfirstRWLock sdk::m_BlackZoneRadiusLock;
	float	    sdk::m_BlackZoneRadius;

	WfirstRWLock sdk::m_SafetyZonePositionLock;
	FVector		sdk::m_SafetyZonePosition;

	WfirstRWLock sdk::m_SafetyZoneRadiusLock;
	float	    sdk::m_SafetyZoneRadius;

	WfirstRWLock sdk::m_NumAliveTeamsLock;
	int sdk::m_NumAliveTeams;

	WfirstRWLock	sdk::m_WorldLocationLock;
	FVector2D		sdk::m_WorldLocation;

	WfirstRWLock	sdk::m_ObjectArrayLock;
	TUObjectArray sdk::m_ObjectArray;

	WfirstRWLock	sdk::m_FiringLocationLock;
	FTransform		sdk::m_FiringLocation;

	WfirstRWLock	sdk::m_CursorPositionLock;
	POINT	    sdk::m_CursorPosition;

	WfirstRWLock	sdk::m_WeaponNameLock;
	std::string sdk::m_WeaponName;

	WfirstRWLock	sdk::m_FloatCurvesLock;
	FRichCurve_3	sdk::m_FloatCurves;

	WfirstRWLock	sdk::m_CurvesKeysArrayLock;
	std::vector<std::vector<FRichCurveKey>>	sdk::m_CurvesKeysArray;

	WfirstRWLock	sdk::m_fovLock;
	float		sdk::m_fov = 0.0f;

	WfirstRWLock	sdk::m_MaxfovLock;
	float		sdk::m_Maxfov = 0.0f;

	WfirstRWLock	sdk::m_localPosLock;
	FVector		sdk::m_localPos;

	WfirstRWLock	sdk::m_localRotLock;
	FRotator	sdk::m_localRot;

	WfirstRWLock	sdk::m_WeaponIndexLock;
	int			sdk::m_WeaponIndex = 0;

	WfirstRWLock	sdk::m_bIsCantedLock;
	bool		sdk::m_bIsCanted = 0;

	WfirstRWLock	sdk::m_CustomTimeDilationLock;
	float		sdk::m_CustomTimeDilation = 0;

	WfirstRWLock	sdk::m_TrajectoryGravityZLock;
	float		sdk::m_TrajectoryGravityZ = 0;

	WfirstRWLock	sdk::m_CurrentZeroLevelLock;
	int			sdk::m_CurrentZeroLevel = 0;

	WfirstRWLock	sdk::m_bSpawnBulletFromBarrelLock;
	bool		sdk::m_bSpawnBulletFromBarrel = 0;

	WfirstRWLock	sdk::m_SpectatedCountLock;
	int			sdk::m_SpectatedCount = 0;

	WfirstRWLock	sdk::m_bThirdPersonLock;
	int			sdk::m_bThirdPerson = 0;

	WfirstRWLock	sdk::m_FireStateLock;
	int			sdk::m_FireState = 0;

	WfirstRWLock	sdk::m_SelectMinimapSizeIndexLock;
	int			sdk::m_SelectMinimapSizeIndex = 0;

	WfirstRWLock	sdk::m_LocalSelectMinimapSizeIndexLock;
	ptr_t sdk::m_LocalSelectMinimapSizeIndex;

	WfirstRWLock	sdk::m_BaseEyeHeightLock;
	float		sdk::m_BaseEyeHeight = 0.0f;

	WfirstRWLock	sdk::m_LastRenderTimeOnScreenLock;
	float		sdk::m_LastRenderTimeOnScreen = 0.0f;

	WfirstRWLock	sdk::m_IsReloading_CPLock;
	int			sdk::m_IsReloading_CP = false;

	WfirstRWLock	sdk::m_IsAiming_CPLock;
	bool		sdk::m_IsAiming_CP = false;

	WfirstRWLock	sdk::m_IsScopingAlpha_CPLock;
	float		sdk::m_IsScopingAlpha_CP = false;

	WfirstRWLock	sdk::m_bIsScoping_CPLock;
	bool		sdk::m_bIsScoping_CP = false;

	WfirstRWLock	sdk::m_ControlRotation_CPLock;
	FRotator	sdk::m_ControlRotation_CP = FRotator();

	WfirstRWLock	sdk::m_ControlRotationLock;
	FRotator	sdk::m_ControlRotation = FRotator();

	WfirstRWLock	sdk::m_IsDMRLock;
	bool		sdk::m_IsDMR = false;
	WfirstRWLock	sdk::m_IsShotGunLock;
	bool		sdk::m_IsShotGun = false;
	WfirstRWLock	sdk::m_IsSRLock;
	bool		sdk::m_IsSR = false;
	WfirstRWLock	sdk::m_IsGrenadeLock;
	bool		sdk::m_IsGrenade = false;

	WfirstRWLock sdk::WeaponTypeLock;
	int			sdk::m_WeaponType = 0;

	WfirstRWLock sdk::m_ComponentLocationLock;
	FVector		sdk::m_ComponentLocation;

	WfirstRWLock sdk::m_BallisticDragScaleLock;
	float		sdk::m_BallisticDragScale;

	WfirstRWLock sdk::m_BallisticDropScaleLock;
	float		sdk::m_BallisticDropScale;

	WfirstRWLock sdk::m_inGameLock;
	bool		sdk::m_inGame = false;

	WfirstRWLock sdk::m_WeaponReplicatedSkinLock;
	FReplicatedSkinParam  sdk::m_WeaponReplicatedSkin;

	WfirstRWLock sdk::m_HumanBodyRepliedSkinLock;
	std::vector<FEuqimentReplicatedSkinItem>  sdk::m_HumanBodyRepliedSkin;

	WfirstRWLock sdk::_VehicleBoneMapLock;
	std::unordered_map < hash_t, std::vector<std::string>>sdk::VehicleBoneNameHash;

	WfirstRWLock sdk::m_PlayerStateLock;
	ptr_t		sdk::m_PlayerState;

	WfirstRWLock sdk::m_MouseSensitivityLock;
	FVector2D	sdk::m_MouseSensitivity;

	WfirstRWLock sdk::m_LocalHeadPosLock;
	FVector		sdk::m_LocalHeadPos;

	WfirstRWLock sdk::m_DamageDeltaLock;
	float		sdk::m_DamageDelta;

	WfirstRWLock sdk::m_bMapOpenLock;
	bool sdk::m_bMapOpenVar;

	WfirstRWLock sdk::m_SkydiveStateLock;
	BYTE sdk::m_SkydiveState;

	WfirstRWLock sdk::m_MousePosPtrLock;
	ptr_t sdk::m_MousePosPtr;

	WfirstRWLock sdk::m_ViewTarget_POV_FOVLock;
	ptr_t sdk::m_ViewTarget_POV_FOV;
	WfirstRWLock sdk::m_ViewTarget_POV_LocationLock;
	ptr_t sdk::m_ViewTarget_POV_Location;
	WfirstRWLock sdk::m_ViewTarget_POV_RotationLock;
	ptr_t sdk::m_ViewTarget_POV_Rotation;

	WfirstRWLock sdk::m_LocalRootLock;
	ptr_t sdk::m_LocalRoot;

	WfirstRWLock sdk::m_LocalAnimScriptInstanceLock;
	ptr_t sdk::m_LocalAnimScriptInstance;

	WfirstRWLock sdk::m_WorldNameLock;
	std::string sdk::m_WorldName;

	WfirstRWLock sdk::m_Mesh3P_ComponentSpaceTransformsArrayPtrLock;
	ptr_t sdk::m_Mesh3P_ComponentSpaceTransformsArrayPtr;


	WfirstRWLock sdk::m_FiringAttachPoint_SocketLock;
	void* sdk::m_FiringAttachPoint_SocketVars;

	WfirstRWLock sdk::m_ScopingAttachPoint_SocketLock;
	void* sdk::m_ScopingAttachPoint_SocketVars;

	WfirstRWLock sdk::LockTargetLock;
	void* sdk::LockTarget = nullptr;

	WfirstRWLock sdk::m_LanguageTypeLock;
	int sdk::m_LanguageType = 0;

	WfirstRWLock sdk::m_FiringSocket_BoneIndexLock;
	int32_t sdk::m_FiringSocket_BoneIndex;
	WfirstRWLock sdk::m_ScopingSocket_BoneIndexLock;
	int32_t sdk::m_ScopingSocket_BoneIndex;

	WfirstRWLock sdk::m_LastTeamNumLock;
	int32_t sdk::m_LastTeamNum;
	//不需要加锁的
	FWeaponTrajectoryConfig sdk::m_WeaponTrajectoryConfig;
	TMap	   sdk::m_AttachedStaticComponentMap;
	bool	   sdk::m_AttachedStaticComponentMapValid;

	std::unordered_map<UObjectType, std::vector<std::shared_ptr<AActor>>> sdk::m_usefulObjects;
	std::unordered_map<UObjectType, std::vector<std::shared_ptr<AActor>>> sdk::m_cacheObjectsActor;
	std::unordered_map<UObjectType, std::vector<std::shared_ptr<AActor>>> sdk::m_cacheObjectsAimbot;
	//std::unordered_map<UObjectType, std::vector<std::shared_ptr<AActor>>> sdk::m_cacheObjectsUpdate;
	std::unordered_map<UObjectType, std::vector<std::shared_ptr<AActor>>> sdk::m_cacheObjectsRadar;

	std::unordered_map<UObjectType, TargetInfo> sdk::m_targets;




	void* sdk::m_MapGridWidget;
	void* sdk::m_MapGridWidgetSlot;

	int32_t sdk::m_LocalWeaponEncryptIndex;
	TArray<void*> sdk::m_LocalEquipment_Item;
	UBlockInputUser sdk::m_HudBlockInputArray;
	void* sdk::m_SkinMesh3P_SkeletalMesh;
	TArray<FTransform> sdk::m_Mesh3P_ComponentSpaceTransformsArray;

	int32_t sdk::m_SkinMesh3P_CurrentReadComponentTransforms;


	TArray<void*> sdk::m_SkeletalMesh_Sockets;
	void* sdk::m_SkeletalMesh_Skeleton;
	FReferenceSkeleton sdk::m_SkeletalMesh_RefSkeleton;

	TArray<void*> sdk::m_Skeleton_Sockets;


	FName sdk::m_FiringSocket_BoneName;
	FName sdk::m_ScopingSocket_BoneName;






	namespace sdkMath
	{
		D3DMATRIX Matrix(FRotator rot, FRotator origin = FRotator(0, 0, 0))
		{
			float radPitch = (rot.Pitch * float(M_PI) / 180.f);
			float radYaw = (rot.Yaw * float(M_PI) / 180.f);
			float radRoll = (rot.Roll * float(M_PI) / 180.f);

			float SP = sinf(radPitch);
			float CP = cosf(radPitch);
			float SY = sinf(radYaw);
			float CY = cosf(radYaw);
			float SR = sinf(radRoll);
			float CR = cosf(radRoll);

			D3DMATRIX matrix;
			matrix.m[0][0] = CP * CY;
			matrix.m[0][1] = CP * SY;
			matrix.m[0][2] = SP;
			matrix.m[0][3] = 0.f;

			matrix.m[1][0] = SR * SP * CY - CR * SY;
			matrix.m[1][1] = SR * SP * SY + CR * CY;
			matrix.m[1][2] = -SR * CP;
			matrix.m[1][3] = 0.f;

			matrix.m[2][0] = -(CR * SP * CY + SR * SY);
			matrix.m[2][1] = CY * SR - CR * SP * SY;
			matrix.m[2][2] = CR * CP;
			matrix.m[2][3] = 0.f;

			matrix.m[3][0] = origin.Pitch;
			matrix.m[3][1] = origin.Yaw;
			matrix.m[3][2] = origin.Roll;
			matrix.m[3][3] = 1.f;

			return matrix;
		}
	}
	namespace TargetSelector
	{
		const float SelectorFOV = 5.0f;
		const float AimbotFOV = 15.f;
		float bestPlayerFov = 0.0f;
		float playerDistMin = 0.0f;
		float bestGroggyPlayerFov = 0.0f;
		void* aimTarget = nullptr;
		void* aimTargetByDist = nullptr;
		void* aimTargetByGroggy = nullptr;
		bool aimLock = false;

		bool CacheRadarTarget(std::shared_ptr<AActor> actor, UObjectType type, json& data, int& Num, int& NumOther, int XWorld, int YWorld, bool Team)
		{
			if (!actor || !(*actor)->IsValid())
				return false;
			switch (type)
			{
			case IronMan::Core::SDK::EPlayer:
			{
				auto player = dynamic_cast<ATslCharacter*>(actor.get());
				if (!player || !player->IsValid())
					break;
				if (!player->_isInFog)
					return false;
				auto Location = player->_Location;
				FVector2D MapPos = sdk::GetMapPos(Location, XWorld, YWorld, false);
				ULONG TeamID = player->_LastTeamNum;
				if (TeamID >= 100000)
					TeamID -= 100000;
				auto State = (player->_Health <= 0.0f) && (player->_GroggyHealth > 0.0f);
				bool duiyou = !!player->_Team || Team;

				char buff[32] = {};
				sprintf(buff, "%.1f", player->_KDA);
				std::string PlayerName = Utils::StringIsValid(player->_PlayerName) ? player->_PlayerName : "";
				if (player->_Health > 0.0f || player->_GroggyHealth > 0.0f && PlayerName != "")
				{
					data["xx"].push_back(int(MapPos.X));
					data["yy"].push_back(int(MapPos.Y));
					data["rr"].push_back(int(player->_AimOffsets.Yaw));
					data["name"].push_back(PlayerName.c_str());
					data["team"].push_back(int(TeamID));
					data["health"].push_back(int(player->_Health));
					data["friend"].push_back(int(duiyou));
					Num++;
					return true;
				}
				break;
			}
			case IronMan::Core::SDK::EVehicle:
			{
				auto vehicle = dynamic_cast<ATslWheeledVehicle*>(actor.get());
				if (!vehicle || !vehicle->IsValid())
					break;
				auto Location = vehicle->_Location;
				FVector2D MapPos = sdk::GetMapPos(Location, XWorld, YWorld, false);
				data["otherxx"].push_back(int(MapPos.X));
				data["otheryy"].push_back(int(MapPos.Y));
				data["othertype"].push_back(int(0));
				NumOther++;
				return true;
			}
			case IronMan::Core::SDK::EBoat:
			{
				auto boat = dynamic_cast<ATslFloatingVehicle*>(actor.get());
				if (!boat || !boat->IsValid())
					break;
				auto Location = boat->_Location;
				FVector2D MapPos = sdk::GetMapPos(Location, XWorld, YWorld, false);
				data["otherxx"].push_back(int(MapPos.X));
				data["otheryy"].push_back(int(MapPos.Y));
				data["othertype"].push_back(int(1));
				NumOther++;
				return true;
			}
			case IronMan::Core::SDK::EAirdrop:
			{
				auto airdrop = dynamic_cast<ACarePackageItem*>(actor.get());
				if (!airdrop || !airdrop->IsValid())
					break;
				auto Location = airdrop->_Location;
				FVector2D MapPos = sdk::GetMapPos(Location, XWorld, YWorld, false);
				std::string displayName = u8"空投";
				data["otherxx"].push_back(int(MapPos.X));
				data["otheryy"].push_back(int(MapPos.Y));
				data["othertype"].push_back(int(2));
				NumOther++;
				return true;
			}
			default:
				break;
			}
			return false;
		}

		void CacheAllData(std::shared_ptr<AActor> actor, UObjectType type, VMMDLL_SCATTER_HANDLE inHandle, ULONGLONG UpdateTime)
		{
			if (!actor || !(*actor)->IsValid())
				return;
			switch (type)
			{
			case IronMan::Core::SDK::EPlayer:
			{
				auto player = dynamic_cast<ATslCharacter*>(actor.get());
				if (!player || !player->IsValid())
					return;
				ptr_t Target = (ptr_t)player->GetPtr();
				//只读一次的
				{
					//Index
					if (!Utils::StringIsValid(player->_ActorName))
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_UObjectNameComparisonIndex, &player->_EncryptActorID, 4);
					//只需要读一次的
					if (player->_EncryptActorID != 0 && !Utils::StringIsValid(player->_ActorName))
					{
						FName NamePrivate;
						NamePrivate.ComparisonIndex = UEEncryptedObjectProperty
							<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(player->_EncryptActorID);

						player->_ActorName = NamePrivate.GetName2();
						if (!Utils::StringIsValid(player->_ActorName))
							player->_ActorName.clear();
						if (player->_ActorName.find("Female") != std::string::npos)
							player->_Female = true;
						else
							player->_Female = false;
					}
				}

				if (player->_HealthMax == 0.f)
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_ATslCharacterBase_HealthMax, &player->_HealthMax, 4);
				if (player->_GroggyHealthMax == 0.f)
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_ATslCharacter_GroggyHealthMax, &player->_GroggyHealthMax, 4);
				if (player->_LastTeamNum == 0)
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_ATslCharacter_LastTeamNum, &player->_LastTeamNum, 4);


				if (!IsAddrValid(player->_RootComponent))
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_Actor_RootComponent, &player->_EncryptRootComponent, 8);
				if (!IsAddrValid(player->_VehicleRiderComponent))
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_ATslCharacter_VehicleRiderComponent, &player->_VehicleRiderComponent, 8);
				if (!IsAddrValid(player->_WeaponProcess))
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_ATslCharacterBase_WeaponProcessor, &player->_WeaponProcess, 8);
				if (!IsAddrValid(player->_Mesh))
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_ACharacter_Mesh, &player->_Mesh, 8);
				if (!IsAddrValid(player->_CharacterMovement))
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_ACharacter_CharacterMovement, &player->_EncryptCharacterMovement, 8);
				if (IsAddrValid(player->_Mesh))
				{
					if (!IsAddrValid(player->_AnimScriptInstance))
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_Mesh + g_PatternData.offset_USkeletalMeshComponent_AnimScriptInstance, &player->_AnimScriptInstance, 8);
				}

				//解密
				if (player->_EncryptTeam)
					player->_Team = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(player->_EncryptTeam);
				if (player->_EncryptRootComponent)
					player->_RootComponent = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(player->_EncryptRootComponent);
				if (player->_EncryptCharacterMovement)
					player->_CharacterMovement = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(player->_EncryptCharacterMovement);
				if (player->_EncryptPlayerState)
				{
					auto PlayerState = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(player->_EncryptPlayerState);
					if (PlayerState)
						player->_PlayerState = PlayerState;
					player->_EncryptPlayerState = 0;
				}

				std::unique_ptr<ATslPlayerState> playerState(new ATslPlayerState(player->_PlayerState));
				bool DelayUpdate = false;

				if (UpdateTime - player->_LastUpdateTime_Update >= 200)
				{
					player->_LastUpdateTime_Update = UpdateTime;
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_Actor_RootComponent, &player->_EncryptRootComponent, 8);
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_ATslCharacter_VehicleRiderComponent, &player->_VehicleRiderComponent, 8);
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_ATslCharacterBase_WeaponProcessor, &player->_WeaponProcess, 8);
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_ACharacter_Mesh, &player->_Mesh, 8);
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_ACharacter_CharacterMovement, &player->_EncryptCharacterMovement, 8);

				}
				//1秒读60次
				if (UpdateTime - player->_LastUpdateTime_CacheLocation >= 16)
				{
					DelayUpdate = true;
					player->_LastUpdateTime_CacheLocation = UpdateTime;

					if (IsAddrValid(player->_VehicleRiderComponent))
					{
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_VehicleRiderComponent + g_PatternData.offset_UVehicleRiderComponent_LastVehiclePawn, &player->_LastVehiclePawn, 8);
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_VehicleRiderComponent + g_PatternData.offset_UVehicleRiderComponent_SeatIndex, &player->_SeatIndex, 4);
						if (player->_LastVehiclePawn)
						{
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_LastVehiclePawn + g_PatternData.offset_UObjectNameComparisonIndex, &player->_LastVehiclePawnEncryptIndex, 4);
						}
					}

					if (IsAddrValid(player->_CharacterMovement))
					{
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_CharacterMovement + 0x500, &player->_StanceMode, 4);
					}
					if (IsAddrValid(player->_RootComponent))
					{
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_RootComponent + g_PatternData.ComponentLocationOffset,
							&player->_CacheLocation, sizeof(FVector));
					}

					if (IsAddrValid(player->_Mesh))
					{
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_Mesh + g_PatternData.offset_UPrimitiveComponent_bRenderCustomDepth - 0xC, &player->_bCheckAsyncSceneOnMove, sizeof(UCHAR));
						player->_isInFog = player->_bCheckAsyncSceneOnMove & 0x03;
						if (IsAddrValid(player->_AnimScriptInstance))
						{
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_AnimScriptInstance + g_PatternData.offset_UTslAnimInstance_bIsAiming_CP, &player->_IsAiming_CP, 1);
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_AnimScriptInstance + g_PatternData.offset_UTslAnimInstance_ScopingAlpha_CP, &player->_ScopingAlpha_CP, 4);
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_AnimScriptInstance + g_PatternData.offset_UTslAnimInstance_bIsReloading_CP, &player->_IsReloading_CP, 1);
						}
					}
					FVector2D _2NAxis;
					if (!sdk::WorldToScreen(player->_CacheLocation, _2NAxis))
					{
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_HealthFlag1, &player->_HealthFlag1, 1);
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_HealthFlag2, &player->_HealthFlag2, 4);
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_HealthFlag3, &player->_HealthFlag3, 1);
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_HealthIndex1, &player->_HealthIndex1, 1);
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_HealthXorKeys, &player->_HealthXorKeys, 4);
						if (player->_HealthFlag1 == 3 || (player->_HealthXorKeys != 0 && player->_HealthFlag2 == 0))
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_HealthIndex2, &player->_Health, 4);
						else
						{
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + player->_HealthIndex1 + g_PatternData.offset_ATslCharacterBase_Health, &player->_EncryptHealth, 4);
							if (player->_HealthFlag3 != 0)
								CallDecryptHealth(&player->_EncryptHealth, 4, player->_HealthXorKeys);
							player->_Health = player->_EncryptHealth;
						}

						if (player->_Health <= 0)
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_ATslCharacter_GroggyHealth, &player->_GroggyHealth, 4);
					}

					if (IsAddrValid(player->_WeaponProcess))
					{
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_WeaponProcess + g_PatternData.offset_UWeaponProcessorComponent_WeaponIndex + 1, &player->_WeaponIndex, 1);
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_WeaponProcess + g_PatternData.offset_UWeaponProcessorComponent_EquippedWeapons, &player->_EquippedWeapon, sizeof(player->_EquippedWeapon));
						if ((ptr_t)player->_EquippedWeapon > 0x100000 && player->_WeaponIndex >= 0 && player->_WeaponIndex <= 4)
						{
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_EquippedWeapon + player->_WeaponIndex * 8, &player->_CurrentWeapon, 8);
							if (IsAddrValid(player->_CurrentWeapon))
							{
								GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_CurrentWeapon + g_PatternData.offset_UObjectNameComparisonIndex, &player->_EncryptWeaponID, 4);
							}
							else
								player->_EncryptWeaponID = 0;
						}
						else
						{
							player->_CurrentWeapon = nullptr;
						}
					}
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_ATslCharacter_AimOffsets, &player->_AimOffsets, sizeof(FRotator));
				}

				//200毫秒读一次的
				if (UpdateTime - player->_LastUpdateTime >= 200)
				{
					player->_LastUpdateTime = UpdateTime;
					if (!IsAddrValid(player->_Team))
						GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_ATslCharacter_Team, &player->_EncryptTeam, 8);
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_APawn_PlayerState, &player->_EncryptPlayerState, 8);
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_ATslCharacter_SpectatedCount, &player->_SpectatedCount, 4);
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.offset_ATslCharacter_LastAimOffsets, &player->_LastAimOffsets, sizeof(FRotator));
					GetDMA().queueScatterReadEx(inHandle, Target + g_PatternData.CharacterState, &player->_CharacterState, 4);
					if (player->_CharacterState == 4)
						player->_isDisconnected = true;
					else
						player->_isDisconnected = false;
					if (!playerState || !playerState->IsValid())
					{
						if (IsAddrValid(player->_LastVehiclePawn))
						{
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_LastVehiclePawn + g_PatternData.offset_APawn_PlayerState, &player->_EncryptPlayerState2, 8);
							if (player->_EncryptPlayerState2)
							{
								auto PlayerState = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(player->_EncryptPlayerState2);
								if (PlayerState)
								{
									player->_PlayerState = PlayerState;
									GetDMA().queueScatterReadEx(inHandle, (ptr_t)PlayerState + g_PatternData.offset_ATslPlayerState_PartnerLevel, &player->_PartnerLevel, sizeof(uint8_t));
								}
								player->_EncryptPlayerState2 = 0;
							}
						}
					}
					else
					{
						if (!Utils::StringIsValid(player->_PlayerName))
							player->_PlayerName = playerState->PlayerName.ToString();
						else
						{
							if (!player->_InitPartner)
							{
								if (player->_PlayerName.find("mikey-T-D") != std::string::npos || player->_PlayerName.find("luoluolo") != std::string::npos
									|| player->_PlayerName.find("LSP_PangHuiGG") != std::string::npos || player->_PlayerName.find("Misaya--Li") != std::string::npos
									|| player->_PlayerName.find("Chase-V") != std::string::npos || player->_PlayerName.find("Huazi-_-1003") != std::string::npos
									|| player->_PlayerName.find("aokangna") != std::string::npos || player->_PlayerName.find("Xingchen-1") != std::string::npos
									|| player->_PlayerName.find("wushen_gg") != std::string::npos)
								{
									player->_PartnerLevel = 5;
									player->_isParthers = true;
								}
								player->_InitPartner = true;
							}
						}
						if (player->_SurvivalLevel <= 0 || player->_SurvivalLevel > 500)
						{
							auto PubgIdData = playerState->PubgIdData;
							int SuvivalLevel = PubgIdData.SurvivalLevel;
							int SurvivalTier = PubgIdData.SurvivalTier;
							if (SuvivalLevel != 0 && SuvivalLevel != player->_SurvivalLevel)
								player->_SurvivalLevel = SuvivalLevel;
							if (SurvivalTier != 0 && SurvivalTier != player->_SurvivalTier)
								player->_SurvivalTier = SurvivalTier;
							player->_ClanTag = PubgIdData.ClanTag.ToString();
							//player->_EmblemId = PubgIdData.EmblemId.ToString();
							if (player->_PartnerLevel != 5)
								GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_PlayerState + g_PatternData.offset_ATslPlayerState_PartnerLevel, &player->_PartnerLevel, sizeof(uint8_t));
						}
					}

					if (playerState->IsValid())
					{
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_PlayerState + g_PatternData.offset_ATslPlayerState_DamageDealtOnEnemy, &player->_DamageDealtOnEnemy, sizeof(float));
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_PlayerState + g_PatternData.offset_ATslPlayerState_PlayerStatistics, &player->_PlayerStatistics, sizeof(FTslPlayerStatistics));
						int killNum = player->_PlayerStatistics.NumKills;
						int assistsNum = player->_PlayerStatistics.NumAssists;
						if (player->_KillNum < 100)
							player->_KillNum = killNum > player->_KillNum ? killNum : player->_KillNum;
						else
							player->_KillNum = killNum;
						if (player->_AssistsNum < 100)
							player->_AssistsNum = assistsNum > player->_AssistsNum ? assistsNum : player->_AssistsNum;
						else
							player->_AssistsNum = assistsNum;
					}
					//bool VRCValid = IsAddrValid(player->_VehicleRiderComponent);
					//if (VRCValid)
					//	if ((!(VRCValid && player->_SeatIndex >= 0) && !playerState->IsValid())
					//		|| (VRCValid && player->_SeatIndex > 0) && !playerState->IsValid())
					//		player->_isDisconnected = true;
					//	else
					//		player->_isDisconnected = false;
					//else
					//	player->_isDisconnected = false;
				}
				std::shared_ptr<AActor>target;
				if (!player->_isInFog)
					goto PlayerExit;
				bool LocalPlayer = false;
				if (player->_bInit && sdk::IsLocalPlayerAlive())
				{
					auto& LPlayer = sdk::GetPlayer();
					if (LPlayer && IsAddrValid(LPlayer))
					{
						if (player->GetPtr() == (void*)LPlayer)
						{
							player->_bInit = true;
							LocalPlayer = true;
						}
					}
				}

				if (player->_bInit)
				{
					if (player->_ShowName == "")
						player->_ShowName = (Utils::StringIsValid2(player->_ClanTag) ? "[" + player->_ClanTag + "]" : "") + (Utils::StringIsValid(player->_PlayerName) ? player->_PlayerName : "");
					if (player->_RankName == "")
						player->_RankName = Utils::StringIsValid(player->_PlayerName) ? player->_PlayerName : "";
					if (player->_TeamName == "")
						player->_TeamName = (Utils::StringIsValid2(player->_ClanTag) ? "[" + player->_ClanTag + "]" : "");
					FVector2D _2NAxis;
					if (!sdk::WorldToScreen(player->_CacheLocation, _2NAxis) || LocalPlayer)
					{
						player->_Location = player->_CacheLocation;
						if (IsAddrValid(player->_Mesh) && player->_dist <= Vars.espHuman.DistanceSlider)
						{
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_Mesh + g_PatternData.offset_UPrimitiveComponent_LastRenderTimeOnScreen, &player->_LastRenderTimeOnScreen, 4);
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_Mesh + g_PatternData.offset_UPrimitiveComponent_LastRenderTime, &player->_LastRenderTime, 4);
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_Mesh + g_PatternData.offset_UPrimitiveComponent_LastSubmitTime, &player->_LastSubmitTime, 4);
							if (DelayUpdate)
							{
								sdk::GetBoneTable1(*player, inHandle);
								sdk::GetBoneTable2(*player, inHandle, true);
								sdk::GetBoneTable3(*player, inHandle, true);
								if (LocalPlayer)
								{
									sdk::SetLocalHeadPos(sdk::GetBonePosition(*player, Bone::forehead));
									goto PlayerExit;
								}
							}
						}
						else
						{
							player->_LastRenderTimeOnScreen = 0;
							player->_LastRenderTime = 0;
							player->_LastSubmitTime = 0;
						}
						goto PlayerExit;
					}
				}

				//血量
				GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_HealthFlag1, &player->_HealthFlag1, 1);
				GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_HealthFlag2, &player->_HealthFlag2, 4);
				GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_HealthFlag3, &player->_HealthFlag3, 1);
				GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_HealthIndex1, &player->_HealthIndex1, 1);
				GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_HealthXorKeys, &player->_HealthXorKeys, 4);
				if (player->_HealthFlag1 == 3 || (player->_HealthXorKeys != 0 && player->_HealthFlag2 == 0))
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_HealthIndex2, &player->_Health, 4);
				else
				{
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + player->_HealthIndex1 + g_PatternData.offset_ATslCharacterBase_Health, &player->_EncryptHealth, 4);
					if (player->_HealthFlag3 != 0)
						CallDecryptHealth(&player->_EncryptHealth, 4, player->_HealthXorKeys);
					player->_Health = player->_EncryptHealth;
				}
				if (player->_Health <= 0)
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->GetPtr() + g_PatternData.offset_ATslCharacter_GroggyHealth, &player->_GroggyHealth, 4);

				//更新位置
				if (IsAddrValid(player->_RootComponent))
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_RootComponent + g_PatternData.ComponentLocationOffset, &player->_Location, sizeof(FVector));
				else
					player->_Location = FVector();

				//更新可视和骨骼
				if (IsAddrValid(player->_Mesh) && player->_dist <= Vars.espHuman.DistanceSlider)
				{
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_Mesh + g_PatternData.offset_UPrimitiveComponent_LastRenderTimeOnScreen, &player->_LastRenderTimeOnScreen, 4);
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_Mesh + g_PatternData.offset_UPrimitiveComponent_LastRenderTime, &player->_LastRenderTime, 4);
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_Mesh + g_PatternData.offset_UPrimitiveComponent_LastSubmitTime, &player->_LastSubmitTime, 4);
					if (player->_bInit && !sdk::PlayerVisibleCheck(*player) && !DelayUpdate)
					{
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_Mesh + g_PatternData.offset_ComponentToWorld, &player->_ComponentToWorld, sizeof(FTransform));
						sdk::GetBoneTable3(*player, inHandle, true);
					}
					else
					{
						sdk::GetBoneTable1(*player, inHandle);
						sdk::GetBoneTable2(*player, inHandle, true);
						if (sdk::GetBoneTable3(*player, inHandle, true))
						{
							if (!player->_bInit)
							{
								bool BoneZero = false;
								int BoneSame = 0;
								for (int i = 0; i < player->_Player_BoneTable.size(); i++)
								{
									if (player->_Player_BoneTable[i].IsZero())
									{
										BoneZero = true;
										break;
									}
								}
								FVector LogPos;
								for (int i = 0; i < player->_Player_BoneTable.size(); i++)
								{
									if (i == 0)
									{
										LogPos = player->_Player_BoneTable[i];
										continue;
									}
									if (LogPos == player->_Player_BoneTable[i])
									{
										BoneSame++;
										break;
									}
									else
									{
										LogPos = player->_Player_BoneTable[i];
									}
								}
								if (!BoneZero && BoneSame < 5 && !player->_Player_BoneTable.empty())
									player->_bInit = true;
							}

						}
						else
							player->_bInit = false;
					}
				}
				else
				{
					player->_LastRenderTimeOnScreen = 0;
					player->_LastRenderTime = 0;
					player->_LastSubmitTime = 0;

				}
				//如果在载具上更新载具位置
				if (IsAddrValid(player->_LastVehiclePawn))
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_LastVehiclePawn + g_PatternData.offset_Actor_LinearVelocity, &player->_LinearVelocity, sizeof(FVector));
				else
					player->_LinearVelocity = FVector();

				//更新自身是否开镜、换弹状态
				if (IsAddrValid(player->_AnimScriptInstance))
				{
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_AnimScriptInstance + g_PatternData.offset_UTslAnimInstance_bIsAiming_CP, &player->_IsAiming_CP, 1);
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_AnimScriptInstance + g_PatternData.offset_UTslAnimInstance_ScopingAlpha_CP, &player->_ScopingAlpha_CP, 4);
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_AnimScriptInstance + g_PatternData.offset_UTslAnimInstance_bIsReloading_CP, &player->_IsReloading_CP, 1);
				}

				//仅目标为自瞄目标才更新
				bool MouseNearly = false;
				target = sdk::GetTarget();
				if (target->IsValid())
				{
					if (player->GetPtr() == target->GetPtr())
					{
						MouseNearly = true;
					}
				}
				if (player->GetPtr() == sdk::GetLockTarget() || MouseNearly)
				{
					if (IsAddrValid(player->_CharacterMovement))
					{
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_CharacterMovement + g_PatternData.offset_UCharacterMovementComponent_Velocity, &player->_Velocity, sizeof(FVector));
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_CharacterMovement + g_PatternData.offset_UCharacterMovementComponent_CurrentFloor, &player->_CurrentFloor, sizeof(FFindFloorResult));
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_CharacterMovement + g_PatternData.offset_UCharacterMovementComponent_WalkableFloorZ, &player->_WalkableFloorZ, sizeof(float));
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_CharacterMovement + g_PatternData.offset_UCharacterMovementComponent_bit4, &player->_bMaintainHorizontalGroundVelocity, sizeof(UCharacterMovementComponent_BIT4));
						FHitResult TargetHitResult = player->_CurrentFloor.HitResult;
						if (TargetHitResult.Component.ObjectIndex != 0 && TargetHitResult.Component.ObjectSerialNumber != 0)
						{
							auto GlobalObjects = &sdk::GetObjectArray();
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)GlobalObjects->Objects.Get() + TargetHitResult.Component.ObjectIndex * sizeof(FUObjectItem), &player->_CurrentFloor_HitResult_ComponentObject, sizeof(FUObjectItem));
							void* HitComponent = player->_CurrentFloor_HitResult_ComponentObject.Object;
							if (IsAddrValid(HitComponent))
							{
								player->_CurrentFloor_HitResult_Component = HitComponent;
								//0F 28 F0 0F 2F 73 2C
								GetDMA().queueScatterReadEx(inHandle, (ptr_t)HitComponent + g_PatternData.offset_UPrimitiveComponent_BodyInstance + 0x74, &player->_CurrentFloor_HitResult_Component_BodyInstance_bOverrideWalkableSlopeOnInstance, 4);
								GetDMA().queueScatterReadEx(inHandle, (ptr_t)HitComponent + g_PatternData.offset_UPrimitiveComponent_BodyInstance + 0x150, &player->_CurrentFloor_HitResult_Component_BodyInstance_BodySetup, sizeof(TWeakObjectPtr<class UBodySetup>));
								if ((player->_CurrentFloor_HitResult_Component_BodyInstance_bOverrideWalkableSlopeOnInstance & 0x2000000) != 0 || !player->_CurrentFloor_HitResult_Component_BodyInstance_BodySetup->IsValid())
								{
									GetDMA().queueScatterReadEx(inHandle, (ptr_t)HitComponent + g_PatternData.offset_UPrimitiveComponent_BodyInstance + 0xD0, &player->_CurrentFloor_HitResult_Component_BodyInstance_WalkableSlopeOverride, sizeof(FWalkableSlopeOverride));
								}
								else
								{
									GetDMA().queueScatterReadEx(inHandle, (ptr_t)GlobalObjects->Objects.Get() + player->_CurrentFloor_HitResult_Component_BodyInstance_BodySetup.ObjectIndex * sizeof(FUObjectItem), &player->_CurrentFloor_HitResult_Component_BodyInstance_BodySetupObject, sizeof(FUObjectItem));
									void* BodySetup = player->_CurrentFloor_HitResult_Component_BodyInstance_BodySetupObject.Object;
									if (IsAddrValid(BodySetup))
									{
										GetDMA().queueScatterReadEx(inHandle, (ptr_t)BodySetup + g_PatternData.offset_UBodySetup_WalkableSlopeOverride, &player->_CurrentFloor_HitResult_Component_BodyInstance_WalkableSlopeOverride, sizeof(FWalkableSlopeOverride));
									}
									else
									{
										GetDMA().queueScatterReadEx(inHandle, (ptr_t)HitComponent + g_PatternData.offset_UPrimitiveComponent_BodyInstance + 0xD0, &player->_CurrentFloor_HitResult_Component_BodyInstance_WalkableSlopeOverride, sizeof(FWalkableSlopeOverride));
									}
								}
							}
							else
							{
								player->_CurrentFloor_HitResult_Component = nullptr;
								player->_CurrentFloor_HitResult_Component_BodyInstance_bOverrideWalkableSlopeOnInstance = 0;
								player->_CurrentFloor_HitResult_Component_BodyInstance_WalkableSlopeOverride = FWalkableSlopeOverride();
								player->_CurrentFloor_HitResult_Component_BodyInstance_BodySetup = TWeakObjectPtr<class UBodySetup>();
							}
						}
						else
						{
							player->_CurrentFloor_HitResult_Component = nullptr;
							player->_CurrentFloor_HitResult_Component_BodyInstance_bOverrideWalkableSlopeOnInstance = 0;
							player->_CurrentFloor_HitResult_Component_BodyInstance_WalkableSlopeOverride = FWalkableSlopeOverride();
							player->_CurrentFloor_HitResult_Component_BodyInstance_BodySetup = TWeakObjectPtr<class UBodySetup>();
						}
					}
					else
					{
						ZeroMemory(&player->_Velocity, sizeof(FVector));
						ZeroMemory(&player->_CurrentFloor, sizeof(FFindFloorResult));
						player->_WalkableFloorZ = 0.f;
						player->_CurrentFloor_HitResult_Component = nullptr;
						player->_CurrentFloor_HitResult_Component_BodyInstance_bOverrideWalkableSlopeOnInstance = 0;
						player->_CurrentFloor_HitResult_Component_BodyInstance_WalkableSlopeOverride = FWalkableSlopeOverride();
						player->_CurrentFloor_HitResult_Component_BodyInstance_BodySetup = TWeakObjectPtr<class UBodySetup>();
					}
					if (player->_LastVehiclePawn)
					{
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_LastVehiclePawn + g_PatternData.offset_AWheeledVehicle_Mesh, &player->_VehicleMesh, 8);
						if (player->_VehicleMesh && player->_LastVehiclePawnEncryptIndex != 0)
						{
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_VehicleMesh + g_PatternData.offset_USkinnedMeshComponent_SkeletalMesh, &player->_SkeletalMesh, 8);
							if (player->_SkeletalMesh)
							{
								GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_SkeletalMesh + g_PatternData.offset_USkeletalMesh_RefSkeleton, &player->_RefSkeleton, sizeof(FReferenceSkeleton));
								if (IsAddrValid(player->_RefSkeleton.FinalRefBoneInfo.Data) && player->_RefSkeleton.FinalRefBoneInfo.Count > 0 && player->_RefSkeleton.FinalRefBoneInfo.Count < 1000)
								{
									FName vehicleFName;
									vehicleFName.ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(player->_LastVehiclePawnEncryptIndex);
									std::unique_ptr<std::string> vehicleName = std::make_unique<std::string>(vehicleFName.GetName2());
									auto VehicleBoneNameHash = sdk::GetVehicleBoneMapName();
									auto BoneResult = VehicleBoneNameHash.find(hash_(*vehicleName));
									if (BoneResult == VehicleBoneNameHash.end())
									{
										auto NumBone = player->_RefSkeleton.FinalRefBoneInfo.Count;
										if (NumBone > 500)
											goto PlayerExit;
										if (player->_VehicleBoneInfo.size() != NumBone)
											player->_VehicleBoneInfo.resize(NumBone);
										if (player->_VehicleBoneName.size() != NumBone)
											player->_VehicleBoneName.resize(NumBone);
										if (player->_Vehicle_BoneTransFormTable.size() != NumBone)
											player->_Vehicle_BoneTransFormTable.resize(NumBone);
										if (player->_Vehicle_BoneTable.size() != NumBone)
											player->_Vehicle_BoneTable.resize(NumBone);
										if (player->_VehicleBoneInfo[0].Name.ComparisonIndex == 0 && player->_VehicleBoneInfo[0].Name.Number == 0)
										{
											GetDMA().queueScatterReadEx(inHandle, (ptr_t)player->_RefSkeleton.FinalRefBoneInfo.Data, player->_VehicleBoneInfo.data(), player->_RefSkeleton.FinalRefBoneInfo.Count * sizeof(FMeshBoneInfo));
										}
										else
										{
											if (player->_VehicleBoneName[0] == "")
											{
												for (uint32_t i = 0; i < NumBone; i++)
												{
													player->_VehicleBoneName[i] = player->_VehicleBoneInfo[i].Name.GetName2();
													if (player->_VehicleBoneName[i] == "None")
													{
														auto BackupName = player->_VehicleBoneInfo[i].Name.ComparisonIndex;
														player->_VehicleBoneInfo[i].Name.ComparisonIndex = player->_VehicleBoneInfo[i].Name.Number;
														player->_VehicleBoneInfo[i].Name.Number = BackupName;
														player->_VehicleBoneName[i] = player->_VehicleBoneInfo[i].Name.GetName2();
													}
												}
											}
											else if (player->_VehicleBoneName.size() > 0)
											{
												VehicleBoneNameHash.emplace(hash_(*vehicleName), player->_VehicleBoneName);
												sdk::SetVehicleBoneMapName(VehicleBoneNameHash);
											}
										}
									}
									else
									{
										auto NumBone = BoneResult->second.size();
										if (NumBone > 500)
											goto PlayerExit;
										if (player->_VehicleBoneInfo.size() != NumBone)
											player->_VehicleBoneInfo.resize(NumBone);
										if (player->_VehicleBoneName.size() != NumBone)
											player->_VehicleBoneName.resize(NumBone);
										if (player->_Vehicle_BoneTransFormTable.size() != NumBone)
											player->_Vehicle_BoneTransFormTable.resize(NumBone);
										if (player->_Vehicle_BoneTable.size() != NumBone)
											player->_Vehicle_BoneTable.resize(NumBone);
										std::vector<int>FindBoneArray;
										for (int i = 0; i < BoneResult->second.size(); i++)
										{
											auto BoneStr = BoneResult->second[i];
											if (BoneStr.find(("Tire_")) != std::string::npos || BoneStr.find(("WheelF_Tire")) != std::string::npos ||
												BoneStr.find(("WheelR_Tire")) != std::string::npos || BoneStr.find(("Helper_")) != std::string::npos ||
												BoneStr.find(("PhysicsWheel_")) != std::string::npos || (BoneStr.find(("_Tire")) != std::string::npos && BoneStr != ("Spare_Tire"))
												|| BoneStr.find(("PhysWheel")) != std::string::npos
												)
											{
												FindBoneArray.push_back(i);
											}
										}
										sdk::GetVehicldeBoneTable1(*player, inHandle);
										sdk::GetVehicldeBoneTable2(*player, inHandle, FindBoneArray);
										sdk::GetVehicldeBoneTable3(*player, inHandle, FindBoneArray);
									}

								}
							}
						}
					}
				}
				else
				{
					if (!player->_Velocity.IsZero())
						ZeroMemory(&player->_Velocity, sizeof(player->_Velocity));
					ZeroMemory(&player->_CurrentFloor, sizeof(player->_CurrentFloor));
					player->_WalkableFloorZ = 0.f;
					ZeroMemory(&player->_bMaintainHorizontalGroundVelocity, sizeof(player->_bMaintainHorizontalGroundVelocity));
					player->_CurrentFloor_HitResult_Component_BodyInstance_bOverrideWalkableSlopeOnInstance = 0;
					ZeroMemory(&player->_CurrentFloor_HitResult_Component_BodyInstance_WalkableSlopeOverride, sizeof(player->_CurrentFloor_HitResult_Component_BodyInstance_WalkableSlopeOverride));
					ZeroMemory(&player->_CurrentFloor_HitResult_Component_BodyInstance_BodySetup, sizeof(player->_CurrentFloor_HitResult_Component_BodyInstance_BodySetup));
				}

			PlayerExit:
				break;
			}
			case IronMan::Core::SDK::EVehicle:
			{
				auto vehicle = dynamic_cast<ATslWheeledVehicle*>(actor.get());
				if (!vehicle || !vehicle->IsValid())
					break;
				//只读一次
				if (!IsAddrValid(vehicle->_RootComponent))
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)vehicle->GetPtr() + g_PatternData.offset_Actor_RootComponent, &vehicle->_EncryptRootComponent, 8);
				if (!IsAddrValid(vehicle->_VehicleCommonComponent))
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)vehicle->GetPtr() + g_PatternData.offset_ATslWheeledVehicle_VehicleCommonComponent, &vehicle->_VehicleCommonComponent, 8);

				if (!Utils::StringIsValid(vehicle->_VehicleName))
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)vehicle->GetPtr() + g_PatternData.offset_UObjectNameComparisonIndex, &vehicle->_EncryptObjectNameIndex, 4);
				if (vehicle->_EncryptObjectNameIndex && !Utils::StringIsValid(vehicle->_VehicleName))
				{
					FName ObjectFName;
					ObjectFName.ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(vehicle->_EncryptObjectNameIndex);
					vehicle->_VehicleName = ObjectFName.GetName2();
					if (!Utils::StringIsValid(vehicle->_VehicleName))
						vehicle->_VehicleName.clear();
				}

				if (!IsAddrValid(vehicle->_RootComponent) && vehicle->_EncryptRootComponent)
					vehicle->_RootComponent = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(vehicle->_EncryptRootComponent);

				if (UpdateTime - vehicle->_LastUpdateTime >= 16)
				{
					vehicle->_LastUpdateTime = UpdateTime;
					if (IsAddrValid(vehicle->_RootComponent))
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)vehicle->_RootComponent + g_PatternData.ComponentLocationOffset, &vehicle->_CacheLocation, sizeof(FVector));
				}

				FVector2D _2NAxis;
				if (!sdk::WorldToScreen(vehicle->_CacheLocation, _2NAxis))
				{
					vehicle->_Location = vehicle->_CacheLocation;
					goto VehicleExit;
				}
				else
				{
					if (ActorIsInVechile((ptr_t)vehicle->GetPtr()))
					{
						if (IsAddrValid(vehicle->_RootComponent))
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)vehicle->_RootComponent + g_PatternData.ComponentLocationOffset, &vehicle->_Location, sizeof(FVector));
					}
					else
						vehicle->_Location = vehicle->_CacheLocation;
				}


				if (IsAddrValid(vehicle->_VehicleCommonComponent))
				{
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)vehicle->_VehicleCommonComponent + g_PatternData.offset_UTslVehicleCommonComponent_Health, &vehicle->_VehicleCommonComponentHealthBack, sizeof(float));
					if (vehicle->_VehicleCommonComponentHealthBack > 0.f && vehicle->_VehicleCommonComponentHealthBack < 10000.f)
						vehicle->_VehicleCommonComponentHealth = vehicle->_VehicleCommonComponentHealthBack;
					else
						vehicle->_VehicleCommonComponentHealth = 0.f;
					if (vehicle->_VehicleCommonComponentHealthMax <= 0.f || vehicle->_VehicleCommonComponentHealthMax > 10000.f)
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)vehicle->_VehicleCommonComponent + g_PatternData.offset_UTslVehicleCommonComponent_HealthMax, &vehicle->_VehicleCommonComponentHealthMax, sizeof(float));
				}
			VehicleExit:
				break;
			}
			case IronMan::Core::SDK::EBoat:
			{
				auto boat = dynamic_cast<ATslFloatingVehicle*>(actor.get());
				if (!boat || !boat->IsValid())
					break;
				//只读一次
				if (!IsAddrValid(boat->_RootComponent))
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)boat->GetPtr() + g_PatternData.offset_Actor_RootComponent, &boat->_EncryptRootComponent, 8);
				if (!IsAddrValid(boat->_VehicleCommonComponent))
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)boat->GetPtr() + g_PatternData.offset_ATslFloatingVehicle_VehicleCommonComponent, &boat->_VehicleCommonComponent, 8);

				if (!Utils::StringIsValid(boat->_VehicleName))
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)boat->GetPtr() + g_PatternData.offset_UObjectNameComparisonIndex, &boat->_EncryptObjectNameIndex, 4);
				if (boat->_EncryptObjectNameIndex && !Utils::StringIsValid(boat->_VehicleName))
				{
					FName ObjectFName;
					ObjectFName.ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(boat->_EncryptObjectNameIndex);
					boat->_VehicleName = ObjectFName.GetName2();
					if (!Utils::StringIsValid(boat->_VehicleName))
						boat->_VehicleName.clear();
				}

				if (!IsAddrValid(boat->_RootComponent) && boat->_EncryptRootComponent)
					boat->_RootComponent = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(boat->_EncryptRootComponent);

				if (UpdateTime - boat->_LastUpdateTime >= 16)
				{
					boat->_LastUpdateTime = UpdateTime;
					if (IsAddrValid(boat->_RootComponent))
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)boat->_RootComponent + g_PatternData.ComponentLocationOffset, &boat->_CacheLocation, sizeof(FVector));
				}
				FVector2D _2NAxis;
				if (!sdk::WorldToScreen(boat->_CacheLocation, _2NAxis))
				{
					boat->_Location = boat->_CacheLocation;
					goto BoatExit;
				}
				else
				{
					if (ActorIsInVechile((ptr_t)boat->GetPtr()))
					{
						if (IsAddrValid(boat->_RootComponent))
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)boat->_RootComponent + g_PatternData.ComponentLocationOffset, &boat->_Location, sizeof(FVector));
					}
					else
						boat->_Location = boat->_CacheLocation;
				}


				if (IsAddrValid(boat->_VehicleCommonComponent))
				{
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)boat->_VehicleCommonComponent + g_PatternData.offset_UTslVehicleCommonComponent_Health, &boat->_VehicleCommonComponentHealthBack, sizeof(float));
					if (boat->_VehicleCommonComponentHealthBack > 0.f && boat->_VehicleCommonComponentHealthBack < 10000.f)
						boat->_VehicleCommonComponentHealth = boat->_VehicleCommonComponentHealthBack;
					else
						boat->_VehicleCommonComponentHealth = 0.f;
					if (boat->_VehicleCommonComponentHealthMax <= 0.f || boat->_VehicleCommonComponentHealthMax > 10000.f)
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)boat->_VehicleCommonComponent + g_PatternData.offset_UTslVehicleCommonComponent_HealthMax, &boat->_VehicleCommonComponentHealthMax, sizeof(float));
				}
			BoatExit:
				break;
			}
			case IronMan::Core::SDK::EItemGroup:
			{
				auto itemGroup = dynamic_cast<ADroppedItemGroup*>(actor.get());
				if (!itemGroup || !itemGroup->IsValid())
					break;
				if (Vars.Menu.战斗模式)
					break;
				bool CanReadArray = false;
				if (UpdateTime - itemGroup->_LastUpdateTime > 16)
				{
					itemGroup->_LastUpdateTime = UpdateTime;
					if (itemGroup->_Location.IsZero() || (!itemGroup->_Location.IsZero() && (UpdateTime - itemGroup->_NextReadPosTime) > 200))
					{
						if (!IsAddrValid(itemGroup->_RootComponent))
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)itemGroup->GetPtr() + g_PatternData.offset_Actor_RootComponent, &itemGroup->_EncryptRootComponent, 8);
						if (itemGroup->_EncryptRootComponent)
							itemGroup->_RootComponent = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(itemGroup->_EncryptRootComponent);
						if (IsAddrValid(itemGroup->_RootComponent))
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)(itemGroup->_RootComponent) + g_PatternData.ComponentLocationOffset, &itemGroup->_Location, sizeof(FVector));
						itemGroup->_NextReadPosTime = UpdateTime;
						CanReadArray = true;
					}
					if (!IsAddrValid(itemGroup->_OwnedComponents.diga))
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)itemGroup->GetPtr() + g_PatternData.offset_OwnedComponents, &itemGroup->_OwnedComponents, sizeof(TSet_Components));
					else if (IsAddrValid(itemGroup->_OwnedComponents.diga) && itemGroup->_OwnedComponents.count > 0 && itemGroup->_OwnedComponents.count < 200)
					{
						if (itemGroup->_OwnedComponents.count < 200)
						{
							if (itemGroup->_digaArray.empty())
								itemGroup->_digaArray.resize(itemGroup->_OwnedComponents.count);
							else if (itemGroup->_digaArray.size() != itemGroup->_OwnedComponents.count)
								itemGroup->_digaArray.resize(itemGroup->_OwnedComponents.count);
						}
						else
							break;
						if (itemGroup->_OwnedComponents.count < 200)
						{
							if (itemGroup->_ItemArray.empty())
								itemGroup->_ItemArray.resize(itemGroup->_OwnedComponents.count);
							else if (itemGroup->_ItemArray.size() != itemGroup->_OwnedComponents.count)
								itemGroup->_ItemArray.resize(itemGroup->_OwnedComponents.count);
						}
						else
							break;
						if (CanReadArray)
							GetDMA().queueScatterReadEx(inHandle, itemGroup->_OwnedComponents.diga, itemGroup->_digaArray.data(), itemGroup->_OwnedComponents.count * sizeof(digaStruct));
						for (int i = 0; i < itemGroup->_digaArray.size(); i++)
						{
							if (!IsAddrValid(itemGroup->_digaArray[i].obj))
							{
								itemGroup->_ItemArray[i] = ItemGroupStruct();
								continue;
							}

							if ((itemGroup->_ItemArray[i].RelativeLocation.IsZero() || (!itemGroup->_ItemArray[i].RelativeLocation.IsZero() && (GetTickCount64() - itemGroup->_NextReadPosTime) > 200))
								&& IsAddrValid(itemGroup->_ItemArray[i].obj) && IsAddrValid(itemGroup->_ItemArray[i].TempUItem)
								&& IsAddrValid(itemGroup->_ItemArray[i].UItem) && IsAddrValid(itemGroup->_ItemArray[i].ItemTableRow))
							{
								GetDMA().queueScatterReadEx(inHandle, (ptr_t)itemGroup->_ItemArray[i].obj + g_PatternData.offset_USceneComponent_RelativeLocation, &itemGroup->_ItemArray[i].RelativeLocation, sizeof(FVector));
							}
							if (itemGroup->_ItemArray[i].obj != itemGroup->_digaArray[i].obj)
							{
								itemGroup->_ItemArray[i].obj = itemGroup->_digaArray[i].obj;
								itemGroup->_ItemArray[i].bReadUItem = false;
								itemGroup->_ItemArray[i].bReadItemTable = false;
								itemGroup->_ItemArray[i].ReGetUItemTime = 0;
								itemGroup->_ItemArray[i].ReGetTableTime = 0;
							}
							if ((!itemGroup->_ItemArray[i].bReadUItem || CanReadArray) && itemGroup->_ItemArray[i].ReGetUItemTime < 5)
							{
								GetDMA().queueScatterReadEx(inHandle, (ptr_t)itemGroup->_ItemArray[i].obj + g_PatternData.offset_UDroppedItemInteractionComponent_Item, &itemGroup->_ItemArray[i].TempUItem, 8);
								itemGroup->_ItemArray[i].bReadUItem = true;
								itemGroup->_ItemArray[i].ReGetUItemTime++;
							}
							if ((IsAddrValid(itemGroup->_ItemArray[i].TempUItem) && itemGroup->_ItemArray[i].TempUItem != itemGroup->_ItemArray[i].UItem) || itemGroup->_ItemArray[i].ItemIDName.empty())
							{
								if (itemGroup->_ItemArray[i].TempUItem != itemGroup->_ItemArray[i].UItem)
								{
									itemGroup->_ItemArray[i].ItemIDName.clear();
								}
								itemGroup->_ItemArray[i].UItem = itemGroup->_ItemArray[i].TempUItem;
								if (!IsAddrValid(itemGroup->_ItemArray[i].UItem))
									continue;

								if (!IsAddrValid(itemGroup->_ItemArray[i].ItemTableRow))
								{
									if ((!itemGroup->_ItemArray[i].bReadItemTable || CanReadArray) && itemGroup->_ItemArray[i].ReGetTableTime < 5)
									{
										GetDMA().queueScatterReadEx(inHandle, (ptr_t)itemGroup->_ItemArray[i].UItem + g_PatternData.offset_UItem_ItemTableRowPtr, &itemGroup->_ItemArray[i].ItemTableRow, 8);
										itemGroup->_ItemArray[i].bReadItemTable = true;
										itemGroup->_ItemArray[i].ReGetTableTime++;
									}
								}
								else
								{
									if (!Utils::StringIsValid(itemGroup->_ItemArray[i].ItemIDName))
									{
										GetDMA().queueScatterReadEx(inHandle, (ptr_t)itemGroup->_ItemArray[i].ItemTableRow + g_PatternData.offset_ItemTableRowBase_ItemID, &itemGroup->_ItemArray[i].TempID, 4);
										if ((itemGroup->_ItemArray[i].TempID.ComparisonIndex != 0 && itemGroup->_ItemArray[i].ItemID.ComparisonIndex != itemGroup->_ItemArray[i].TempID.ComparisonIndex) && !Utils::StringIsValid(itemGroup->_ItemArray[i].ItemIDName))
										{
											itemGroup->_ItemArray[i].ItemID = itemGroup->_ItemArray[i].TempID;
											itemGroup->_ItemArray[i].ItemIDName = itemGroup->_ItemArray[i].ItemID.GetName2();
											if (!Utils::StringIsValid(itemGroup->_ItemArray[i].ItemIDName))
											{
												itemGroup->_ItemArray[i].ItemIDName.clear();
												itemGroup->_ItemArray[i].ItemID.ComparisonIndex = 0;
												itemGroup->_ItemArray[i].ItemID.Number = 0;
											}
										}
									}
								}
							}
							else if (Utils::StringIsValid(itemGroup->_ItemArray[i].ItemIDName) && hash_(itemGroup->_ItemArray[i].ItemIDName) == "Item_JerryCan_C"_hash)
							{
								if (!itemGroup->_ItemArray[i].LogReReadTime)
									itemGroup->_ItemArray[i].LogReReadTime = GetTickCount64();
								if (GetTickCount64() - itemGroup->_ItemArray[i].LogReReadTime > 200)
								{
									GetDMA().queueScatterReadEx(inHandle, (ptr_t)itemGroup->_ItemArray[i].obj + g_PatternData.offset_UDroppedItemInteractionComponent_Item, &itemGroup->_ItemArray[i].TempUItem, 8);
									itemGroup->_ItemArray[i].LogReReadTime = GetTickCount64();
								}
							}
						}
					}
				}
				break;
			}
			case IronMan::Core::SDK::EItem:
			{
				auto item = dynamic_cast<ADroppedItem*>(actor.get());
				if (!item || !item->IsValid())
					break;
				//只读一遍
				if (!IsAddrValid(item->_RootComponent))
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)item->GetPtr() + g_PatternData.offset_Actor_RootComponent, &item->_EncryptRootComponent, 8);
				if (!IsAddrValid(item->_Item))
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)item->GetPtr() + g_PatternData.offset_ADroppedItem_Item, &item->_EncryptItem, 8);
				if (IsAddrValid(item->_Item))
				{
					if (!IsAddrValid(item->_ItemTableRow))
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)item->_Item + g_PatternData.offset_UItem_ItemTableRowPtr, &item->_ItemTableRow, sizeof(void*));
					else if (item->_ItemID.ComparisonIndex == 0 && item->_ItemID.CompositeComparisonValue == 0)
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)item->_ItemTableRow + g_PatternData.offset_ItemTableRowBase_ItemID, &item->_ItemID, 4);
				}

				if (item->_EncryptRootComponent)
					item->_RootComponent = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(item->_EncryptRootComponent);
				if (item->_EncryptItem != 0 && !item->_Item)
					item->_Item = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(item->_EncryptItem);

				if (UpdateTime - item->_LastUpdateTime >= 16)
				{
					item->_LastUpdateTime = UpdateTime;
					if (IsAddrValid(item->_RootComponent))
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)item->_RootComponent + g_PatternData.ComponentLocationOffset, &item->_Location, sizeof(FVector));
				}

				if (IsAddrValid(item->_Item) && IsAddrValid(item->_ItemTableRow))
				{
					if (!Utils::StringIsValid(item->_ItemName))
					{
						if (item->_ItemID.ComparisonIndex != 0 && item->_ItemID.CompositeComparisonValue != 0)
						{
							item->_ItemName = item->_ItemID.GetName2();
							if (!Utils::StringIsValid(item->_ItemName))
								item->_ItemName.clear();
						}
					}
				}
				break;
			}
			case IronMan::Core::SDK::EDeathBox:
			{
				auto deadbox = dynamic_cast<AFloorSnapItemPackage*>(actor.get());
				if (!deadbox || !deadbox->IsValid())
					break;
				if (deadbox->_ArrayData._itemArray.empty())
					deadbox->_ArrayData._itemArray.resize(40);
				if (deadbox->_ArrayData._itemArray.size() > 100)
					break;
				if (!IsAddrValid(deadbox->_RootComponent))
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)deadbox->GetPtr() + g_PatternData.offset_Actor_RootComponent, &deadbox->_EncryptRootComponent, 8);

				if (deadbox->_EncryptRootComponent)
					deadbox->_RootComponent = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(deadbox->_EncryptRootComponent);

				if (UpdateTime - deadbox->_LastUpdateTime >= 16)
				{
					deadbox->_LastUpdateTime = UpdateTime;
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)deadbox->GetPtr() + g_PatternData.offset_AItemPackage_Items, &deadbox->_Items, sizeof(deadbox->_Items));

					if (IsAddrValid(deadbox->_RootComponent))
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)deadbox->_RootComponent + g_PatternData.ComponentLocationOffset, &deadbox->_Location, sizeof(FVector));
				}

				FVector2D screen;
				if (!sdk::WorldToScreen(deadbox->_Location, screen))
					goto DeadBoxExit;

				if (UpdateTime - deadbox->_LastUpdateTime_CacheLocation >= 50)
				{
					deadbox->_LastUpdateTime_CacheLocation = UpdateTime;
					deadbox->_ItemsNum = deadbox->_Items.Num();
					if (deadbox->_ItemsNum < 40 && deadbox->_ItemsNum > 0)
					{
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)deadbox->_Items->GetData(), deadbox->_ArrayData._itemArray.data(), deadbox->_ItemsNum * sizeof(ptr_t));
						if (deadbox->_ItemsNum < 100)
						{
							if (deadbox->_ArrayData._itemTableRowArray.size() != deadbox->_ItemsNum)
							{
								deadbox->_ArrayData._itemTableRowArray.clear();
								deadbox->_ArrayData._ItemsIDArray.clear();
								deadbox->_ArrayData._ItemsIDNameArray.clear();
								deadbox->_ArrayData._itemFreshArray.clear();
								deadbox->_ArrayData._itemTableRowArray.resize(deadbox->_ItemsNum);
								deadbox->_ArrayData._ItemsIDArray.resize(deadbox->_ItemsNum);
								deadbox->_ArrayData._ItemsIDNameArray.resize(deadbox->_ItemsNum);
								deadbox->_ArrayData._itemFreshArray.resize(deadbox->_ItemsNum);
							}
						}
						else
							goto DeadBoxExit;
						for (uint32_t i = 0; i < deadbox->_ItemsNum; i++)
						{
							auto item = deadbox->_ArrayData._itemArray[i];
							if (!item || !deadbox->_Items.IsValidIndex(i))
							{
								deadbox->_ArrayData._itemFreshArray[i] = 0;
								continue;
							}
							deadbox->_ArrayData._itemFreshArray[i] = 1;
							if (!IsAddrValid(deadbox->_ArrayData._itemTableRowArray[i]))
							{
								GetDMA().queueScatterReadEx(inHandle, (ptr_t)item + g_PatternData.offset_UItem_ItemTableRowPtr, &deadbox->_ArrayData._itemTableRowArray[i], sizeof(void*));
							}
							else if (deadbox->_ArrayData._ItemsIDArray[i].ComparisonIndex == 0 && deadbox->_ArrayData._ItemsIDArray[i].Number == 0)
							{
								GetDMA().queueScatterReadEx(inHandle, (ptr_t)deadbox->_ArrayData._itemTableRowArray[i] + g_PatternData.offset_ItemTableRowBase_ItemID, &deadbox->_ArrayData._ItemsIDArray[i], 4);
							}
							else if (!Utils::StringIsValid(deadbox->_ArrayData._ItemsIDNameArray[i]))
							{
								deadbox->_ArrayData._ItemsIDNameArray[i] = deadbox->_ArrayData._ItemsIDArray[i].GetName2();
								if (!Utils::StringIsValid(deadbox->_ArrayData._ItemsIDNameArray[i]))
								{
									deadbox->_ArrayData._ItemsIDNameArray[i].clear();
								}
							}
						}
					}
					else
					{
						deadbox->_ItemsNum = 0;
						ZeroMemory(deadbox->_ArrayData._itemArray.data(), 40 * 8);
					}
				}
			DeadBoxExit:
				break;
			}
			case IronMan::Core::SDK::EAirdrop:
			{
				auto airdrop = dynamic_cast<ACarePackageItem*>(actor.get());
				if (!airdrop || !airdrop->IsValid())
					break;
				if (airdrop->_ArrayData._itemArray.empty())
					airdrop->_ArrayData._itemArray.resize(40);
				if (airdrop->_ArrayData._itemArray.size() > 100)
					break;
				if (!IsAddrValid(airdrop->_RootComponent))
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)airdrop->GetPtr() + g_PatternData.offset_Actor_RootComponent, &airdrop->_EncryptRootComponent, 8);

				if (airdrop->_EncryptRootComponent)
					airdrop->_RootComponent = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(airdrop->_EncryptRootComponent);

				if (UpdateTime - airdrop->_LastUpdateTime >= 16)
				{
					airdrop->_LastUpdateTime = UpdateTime;
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)airdrop->GetPtr() + g_PatternData.offset_AItemPackage_Items, &airdrop->_Items, sizeof(airdrop->_Items));

					if (IsAddrValid(airdrop->_RootComponent))
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)airdrop->_RootComponent + g_PatternData.ComponentLocationOffset, &airdrop->_Location, sizeof(FVector));
				}

				FVector2D screen;
				if (!sdk::WorldToScreen(airdrop->_Location, screen))
					goto AirDropExit;

				if (UpdateTime - airdrop->_LastUpdateTime_CacheLocation >= 50)
				{
					airdrop->_LastUpdateTime_CacheLocation = UpdateTime;
					airdrop->_ItemsNum = airdrop->_Items.Num();
					if (airdrop->_ItemsNum < 40 && airdrop->_ItemsNum > 0)
					{
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)airdrop->_Items->GetData(), airdrop->_ArrayData._itemArray.data(), airdrop->_ItemsNum * sizeof(ptr_t));
						if (airdrop->_ItemsNum < 100)
						{
							if (airdrop->_ArrayData._itemTableRowArray.size() != airdrop->_ItemsNum)
							{
								airdrop->_ArrayData._itemTableRowArray.clear();
								airdrop->_ArrayData._ItemsIDArray.clear();
								airdrop->_ArrayData._ItemsIDNameArray.clear();
								airdrop->_ArrayData._itemFreshArray.clear();
								airdrop->_ArrayData._itemTableRowArray.resize(airdrop->_ItemsNum);
								airdrop->_ArrayData._ItemsIDArray.resize(airdrop->_ItemsNum);
								airdrop->_ArrayData._ItemsIDNameArray.resize(airdrop->_ItemsNum);
								airdrop->_ArrayData._itemFreshArray.resize(airdrop->_ItemsNum);
							}
						}
						else
							goto AirDropExit;
						for (uint32_t i = 0; i < airdrop->_ItemsNum; i++)
						{
							auto item = airdrop->_ArrayData._itemArray[i];
							if (!item || !airdrop->_Items.IsValidIndex(i))
							{
								airdrop->_ArrayData._itemFreshArray[i] = 0;
								continue;
							}
							airdrop->_ArrayData._itemFreshArray[i] = 1;
							if (!IsAddrValid(airdrop->_ArrayData._itemTableRowArray[i]))
							{
								GetDMA().queueScatterReadEx(inHandle, (ptr_t)item + g_PatternData.offset_UItem_ItemTableRowPtr, &airdrop->_ArrayData._itemTableRowArray[i], sizeof(void*));
							}
							else if (airdrop->_ArrayData._ItemsIDArray[i].ComparisonIndex == 0 && airdrop->_ArrayData._ItemsIDArray[i].Number == 0)
							{
								GetDMA().queueScatterReadEx(inHandle, (ptr_t)airdrop->_ArrayData._itemTableRowArray[i] + g_PatternData.offset_ItemTableRowBase_ItemID, &airdrop->_ArrayData._ItemsIDArray[i], 4);
							}
							else if (!Utils::StringIsValid(airdrop->_ArrayData._ItemsIDNameArray[i]))
							{
								airdrop->_ArrayData._ItemsIDNameArray[i] = airdrop->_ArrayData._ItemsIDArray[i].GetName2();
								if (!Utils::StringIsValid(airdrop->_ArrayData._ItemsIDNameArray[i]))
								{
									airdrop->_ArrayData._ItemsIDNameArray[i].clear();
								}
							}
						}
					}
					else
					{
						airdrop->_ItemsNum = 0;
						ZeroMemory(airdrop->_ArrayData._itemArray.data(), 40 * 8);
					}
				}
			AirDropExit:
				break;
			}
			case IronMan::Core::SDK::EProjectile:
			{
				auto proj = dynamic_cast<ATslProjectile*>(actor.get());
				if (!proj || !proj->IsValid())
					break;
				//只读一次
				if (!proj->_EncryptRootComponent)
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)proj->GetPtr() + g_PatternData.offset_Actor_RootComponent, &proj->_EncryptRootComponent, 8);

				if (!IsAddrValid(proj->_EncryptObjectNameIndex))
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)proj->GetPtr() + g_PatternData.offset_UObjectNameComparisonIndex, &proj->_EncryptObjectNameIndex, 4);

				if (proj->_EncryptObjectNameIndex && !Utils::StringIsValid(proj->_ProjectileName))
				{
					FName ObjectFName;
					ObjectFName.ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(proj->_EncryptObjectNameIndex);
					proj->_ProjectileName = ObjectFName.GetName2();
					if (!Utils::StringIsValid(proj->_ProjectileName))
						proj->_ProjectileName.clear();
				}

				if (proj->_EncryptRootComponent)
					proj->_RootComponent = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(proj->_EncryptRootComponent);
				if (IsAddrValid(proj->_RootComponent))
				{
					if (!proj->_ExplosionDelay)
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)proj->GetPtr() + g_PatternData.offset_ATslProjectile_ExplosionDelay, &proj->_ExplosionDelay, sizeof(float));
					if (!proj->_ProjectileConfig.ExplosionRadius)
						GetDMA().queueScatterReadEx(inHandle, (ptr_t)proj->GetPtr() + g_PatternData.offset_ATslProjectile_ProjectileConfig, &proj->_ProjectileConfig, 0x10);
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)proj->GetPtr() + g_PatternData.offset_ATslProjectile_TimeTillExplosion, &proj->_TimeTillExplosion, sizeof(float));
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)proj->_RootComponent + g_PatternData.ComponentLocationOffset, &proj->_Location, sizeof(FVector));
				}
				break;
			}
			case IronMan::Core::SDK::EDroppedItem:
				break;
			case IronMan::Core::SDK::EOther:
			{
				auto Aactor = dynamic_cast<AActor*>(actor.get());
				if (!Aactor || !Aactor->IsValid())
					return;

				//if (!Aactor->_RootComponent)
				//	GetDMA().queueScatterReadEx(inHandle, (ptr_t)Aactor->GetPtr() + g_PatternData.offset_Actor_RootComponent, &Aactor->_EncryptRootComponent, 8);
				//if (Aactor->_EncryptRootComponent)
				//	Aactor->_RootComponent = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(Aactor->_EncryptRootComponent);
				//USceneComponent RootComponent = Aactor->_RootComponent;
				//if (RootComponent->IsValid())
				//	GetDMA().queueScatterReadEx(inHandle, (ptr_t)RootComponent->GetPtr() + g_PatternData.ComponentLocationOffset, &Aactor->_Location, sizeof(FVector));
				//

				if (UpdateTime - Aactor->_LastUpdateTime_Check > 1000)
				{
					Aactor->_LastUpdateTime_Check = UpdateTime;
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)Aactor->GetPtr() + g_PatternData.offset_UObjectClass, &Aactor->_EncryptClass, 8);
					if(Aactor->_EncryptClass)
						Aactor->_Class = UEEncryptedObjectProperty<ptr_t, DecryptFunc::UObjectClass>::STATIC_Get(Aactor->_EncryptClass);
					if (IsAddrValid(Aactor->_Class))
					{
						if (!Aactor->_InitCheck)
						{
							GetDMA().queueScatterReadEx(inHandle, (ptr_t)Aactor->_Class + g_PatternData.offset_UClassBaseChainArray, &Aactor->_ClassBaseChain, sizeof(CClassBaseChain));
							if (IsAddrValid(Aactor->_ClassBaseChain.ClassBaseChainArray) && Aactor->_ClassBaseChain.NumClassBasesInChainMinusOne > 0)
							{
								int32_t NumParentClassBasesInChainMinusOne = EPlayerUClass.NumClassBasesInChainMinusOne;
								GetDMA().queueScatterReadEx(inHandle, Aactor->_ClassBaseChain.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
									&Aactor->_EPlayerValue, 8);

								NumParentClassBasesInChainMinusOne = EVehicleUClass.NumClassBasesInChainMinusOne;
								GetDMA().queueScatterReadEx(inHandle, Aactor->_ClassBaseChain.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
									&Aactor->_EVehicleValue, 8);

								NumParentClassBasesInChainMinusOne = EBoatUClass.NumClassBasesInChainMinusOne;
								GetDMA().queueScatterReadEx(inHandle, Aactor->_ClassBaseChain.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
									&Aactor->_EBoatValue, 8);

								NumParentClassBasesInChainMinusOne = EItemGroupUClass.NumClassBasesInChainMinusOne;
								GetDMA().queueScatterReadEx(inHandle, Aactor->_ClassBaseChain.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
									&Aactor->_EItemGroupValue, 8);

								NumParentClassBasesInChainMinusOne = EItemUClass.NumClassBasesInChainMinusOne;
								GetDMA().queueScatterReadEx(inHandle, Aactor->_ClassBaseChain.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
									&Aactor->_EItemValue, 8);

								NumParentClassBasesInChainMinusOne = EDeathBoxUClass.NumClassBasesInChainMinusOne;
								GetDMA().queueScatterReadEx(inHandle, Aactor->_ClassBaseChain.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
									&Aactor->_EDeathBoxValue, 8);

								NumParentClassBasesInChainMinusOne = EAirdropUClass.NumClassBasesInChainMinusOne;
								GetDMA().queueScatterReadEx(inHandle, Aactor->_ClassBaseChain.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
									&Aactor->_EAirdropValue, 8);

								NumParentClassBasesInChainMinusOne = EProjectileUClass.NumClassBasesInChainMinusOne;
								GetDMA().queueScatterReadEx(inHandle, Aactor->_ClassBaseChain.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
									&Aactor->_EProjectileValue, 8);

								NumParentClassBasesInChainMinusOne = EDroppedItemUClass.NumClassBasesInChainMinusOne;
								GetDMA().queueScatterReadEx(inHandle, Aactor->_ClassBaseChain.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
									&Aactor->_EDroppedItemValue, 8);

								Aactor->_InitCheck = true;
							}
						}
						else
						{
							if (Aactor->_LastClass != Aactor->_Class)
							{
								if (Aactor->_LastClass == 0)
								{
									Aactor->_LastClass = Aactor->_Class;
								}
								else
								{
									Aactor->_LastClass = Aactor->_Class;
									Aactor->_InitCheck = false;
									Aactor->_InitCmp = false;
								}
							}
							if (!Aactor->_InitCmp)
							{
								OMFClassBaseChain PlayerBaseChain = EPlayerUClass;
								OMFClassBaseChain VehicleBaseChain = EVehicleUClass;
								OMFClassBaseChain BoatBaseChain = EBoatUClass;
								OMFClassBaseChain ItemGroupBaseChain = EItemGroupUClass;
								OMFClassBaseChain ItemBaseChain = EItemUClass;
								OMFClassBaseChain DeathBoxChain = EDeathBoxUClass;
								OMFClassBaseChain AirdropChain = EAirdropUClass;
								OMFClassBaseChain ProjectileChain = EProjectileUClass;
								OMFClassBaseChain DroppedItemChain = EDroppedItemUClass;
								int32_t NumParentClassBasesInChainMinusOne = PlayerBaseChain.NumClassBasesInChainMinusOne;
								if (NumParentClassBasesInChainMinusOne <= Aactor->_ClassBaseChain.NumClassBasesInChainMinusOne &&
									Aactor->_EPlayerValue == PlayerBaseChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
								{
									std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
									ObjectManager::AddObject(*actor, EPlayer);
									CONSOLE_INFO("%p|%s Set EPlayer", Aactor->GetPtr(), Aactor->_ActorName.c_str());
									break;
								}
								NumParentClassBasesInChainMinusOne = VehicleBaseChain.NumClassBasesInChainMinusOne;
								if (NumParentClassBasesInChainMinusOne <= Aactor->_ClassBaseChain.NumClassBasesInChainMinusOne &&
									Aactor->_EVehicleValue == VehicleBaseChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
								{
									std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
									ObjectManager::AddObject(*actor, EVehicle);
									CONSOLE_INFO("%p|%s Set EVehicle", Aactor->GetPtr(), Aactor->_ActorName.c_str());
									break;
								}
								NumParentClassBasesInChainMinusOne = BoatBaseChain.NumClassBasesInChainMinusOne;
								if (NumParentClassBasesInChainMinusOne <= Aactor->_ClassBaseChain.NumClassBasesInChainMinusOne &&
									Aactor->_EBoatValue == BoatBaseChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
								{
									std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
									ObjectManager::AddObject(*actor, EBoat);
									CONSOLE_INFO("%p|%s Set EBoat", Aactor->GetPtr(), Aactor->_ActorName.c_str());
									break;
								}
								NumParentClassBasesInChainMinusOne = ItemGroupBaseChain.NumClassBasesInChainMinusOne;
								if (NumParentClassBasesInChainMinusOne <= Aactor->_ClassBaseChain.NumClassBasesInChainMinusOne &&
									Aactor->_EItemGroupValue == ItemGroupBaseChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
								{
									std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
									ObjectManager::AddObject(*actor, EItemGroup);
									CONSOLE_INFO("%p|%s Set EItemGroup", Aactor->GetPtr(), Aactor->_ActorName.c_str());
									break;
								}
								NumParentClassBasesInChainMinusOne = ItemBaseChain.NumClassBasesInChainMinusOne;
								if (NumParentClassBasesInChainMinusOne <= Aactor->_ClassBaseChain.NumClassBasesInChainMinusOne &&
									Aactor->_EItemValue == ItemBaseChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
								{
									std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
									ObjectManager::AddObject(*actor, EItem);
									CONSOLE_INFO("%p|%s Set EItem", Aactor->GetPtr(), Aactor->_ActorName.c_str());
									break;
								}
								NumParentClassBasesInChainMinusOne = DeathBoxChain.NumClassBasesInChainMinusOne;
								if (NumParentClassBasesInChainMinusOne <= Aactor->_ClassBaseChain.NumClassBasesInChainMinusOne &&
									Aactor->_EDeathBoxValue == DeathBoxChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
								{
									std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
									ObjectManager::AddObject(*actor, EDeathBox);
									CONSOLE_INFO("%p|%s Set EDeathBox", Aactor->GetPtr(), Aactor->_ActorName.c_str());
									break;
								}
								NumParentClassBasesInChainMinusOne = AirdropChain.NumClassBasesInChainMinusOne;
								if (NumParentClassBasesInChainMinusOne <= Aactor->_ClassBaseChain.NumClassBasesInChainMinusOne &&
									Aactor->_EAirdropValue == AirdropChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
								{
									std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
									ObjectManager::AddObject(*actor, EAirdrop);
									CONSOLE_INFO("%p|%s Set EAirdrop", Aactor->GetPtr(), Aactor->_ActorName.c_str());
									break;
								}
								NumParentClassBasesInChainMinusOne = ProjectileChain.NumClassBasesInChainMinusOne;
								if (NumParentClassBasesInChainMinusOne <= Aactor->_ClassBaseChain.NumClassBasesInChainMinusOne &&
									Aactor->_EProjectileValue == ProjectileChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
								{
									std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
									ObjectManager::AddObject(*actor, EProjectile);
									CONSOLE_INFO("%p|%s Set EProjectile", Aactor->GetPtr(), Aactor->_ActorName.c_str());
									break;
								}
								NumParentClassBasesInChainMinusOne = DroppedItemChain.NumClassBasesInChainMinusOne;
								if (NumParentClassBasesInChainMinusOne <= Aactor->_ClassBaseChain.NumClassBasesInChainMinusOne &&
									Aactor->_EDroppedItemValue == DroppedItemChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
								{
									std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
									ObjectManager::AddObject(*actor, EDroppedItem);
									CONSOLE_INFO("%p|%s Set EDroppedItem", Aactor->GetPtr(), Aactor->_ActorName.c_str());
									break;
								}
								Aactor->_InitCmp = true;
							}
						}
					}
				}

				if (Aactor->_ActorName.size() < 3)
					GetDMA().queueScatterReadEx(inHandle, (ptr_t)Aactor->GetPtr() + g_PatternData.offset_UObjectNameComparisonIndex, &Aactor->_EncryptActorID, 4);

				if (Aactor->_EncryptActorID != 0 && !Utils::StringIsValid(Aactor->_ActorName))
				{
					FName NamePrivate;
					NamePrivate.ComparisonIndex = UEEncryptedObjectProperty
						<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(Aactor->_EncryptActorID);
					auto ActorName = NamePrivate.GetName2();
					if (!Utils::StringIsValid(Aactor->_ActorName))
						Aactor->_ActorName = ActorName;

					if (!Utils::StringIsValid(Aactor->_ActorName))
						Aactor->_ActorName.clear();
					else
						Aactor->_VehicleName = Aactor->_ActorName;
					//CONSOLE_INFO("%s", Aactor->_ActorName.c_str());
					switch (hash_(Aactor->_ActorName))
					{
					case "PlayerMale_A"_hash:
					case "PlayerMale_A_C"_hash:
					case "PlayerFemale_A"_hash:
					case "PlayerFemale_A_C"_hash:
					case "AIPawn_Base_C"_hash:
					case "AIPawn_Base_Female_C"_hash:
					case "AIPawn_Base_Male_C"_hash:
					case "AIPawn_Base_Pillar_C"_hash:
					case "AIPawn_Base_Female_Pillar_C"_hash:
					case "AIPawn_Base_Male_Pillar_C"_hash:
					case "UltAIPawn_Base_C"_hash:
					case "UltAIPawn_Base_Female_C"_hash:
					case "UltAIPawn_Base_Male_C"_hash:
					case "ZDF2_NPC_Runner_C"_hash:
					case "ZDF2_NPC_Burning_C"_hash:
					case "ZDF2_NPC_Tanker_C"_hash:
					case "ZDF2_NPC_Female_C"_hash:
					case "ZombieNpcNewPawn_Tanker_C"_hash:
					{
						if (!Aactor->_transforActor)
						{
							Aactor->_transforActor = true;
							std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
							ObjectManager::AddObject(*actor, EPlayer);
							CONSOLE_INFO("%p|%s Set EPlayer", Aactor->GetPtr(), Aactor->_ActorName.c_str());
						}
						break;
					}
					case "Uaz_A_01_C"_hash:
					case "Uaz_Armored_C"_hash:
					case "Uaz_B_01_C"_hash:
					case "Uaz_B_01_esports_C"_hash:
					case "Uaz_C_01_C"_hash:
					case "BP_Motorbike_04_C"_hash:
					case "BP_Motorbike_04_Desert_C"_hash:
					case "BP_Motorbike_04_SideCar_C"_hash:
					case "BP_Van_A_01_C"_hash:
					case "BP_Van_A_02_C"_hash:
					case "BP_Van_A_03_C"_hash:
					case "BP_PickupTruck_A_01_C"_hash:
					case "BP_PickupTruck_A_02_C"_hash:
					case "BP_PickupTruck_A_03_C"_hash:
					case "BP_PickupTruck_A_04_C"_hash:
					case "BP_PickupTruck_A_05_C"_hash:
					case "BP_PickupTruck_A_esports_C"_hash:
					case "BP_PickupTruck_B_01_C"_hash:
					case "BP_PickupTruck_B_02_C"_hash:
					case "BP_PickupTruck_B_03_C"_hash:
					case "BP_PickupTruck_B_04_C"_hash:
					case "BP_PickupTruck_B_05_C"_hash:
					case "Buggy_A_01_C"_hash:
					case "Buggy_A_02_C"_hash:
					case "Buggy_A_03_C"_hash:
					case "Buggy_A_04_C"_hash:
					case "Buggy_A_05_C"_hash:
					case "Buggy_A_06_C"_hash:
					case "ABP_PickupTruck_C"_hash:
					case "ABP_Motorbike_04_Sidecar"_hash:
					case "ABP_Motorbike_04_Sidecar_C"_hash:
					case "BP_Motorbike_04_SideCar_Desert_C"_hash:
					case "BP_Motorbike_Solitario_C"_hash:
					case "BP_Motorglider_C"_hash:
					case "BP_Motorglider_Green_C"_hash:
					case "BP_Mirado_A_01_C"_hash:
					case "BP_Mirado_A_02_C"_hash:
					case "BP_Mirado_A_03_C"_hash:
					case "BP_Mirado_A_04_C"_hash:
					case "BP_Mirado_Open_01_C"_hash:
					case "BP_Mirado_Open_02_C"_hash:
					case "BP_Mirado_Open_03_C"_hash:
					case "BP_Mirado_Open_04_C"_hash:
					case "BP_ATV_C"_hash:
					case "BP_BRDM_C"_hash:
					case "BP_Bicycle_C"_hash:
					case "BP_Blanc_C"_hash:
					case "BP_CoupeRB_C"_hash:
					case "BP_Dirtbike_C"_hash:
					case "BP_Food_Truck_C"_hash:
					case "BP_KillTruck_C"_hash:
					case "BP_LootTruck_C"_hash:
					case "BP_M_Rony_A_01_C"_hash:
					case "BP_M_Rony_A_02_C"_hash:
					case "BP_M_Rony_A_03_C"_hash:
					case "BP_McLarenGT_Lx_Yellow_C"_hash:
					case "BP_McLarenGT_St_white_C"_hash:
					case "BP_McLarenGT_St_black_C"_hash:
					case "BP_Mirado_A_03_Esports_C"_hash:
					case "BP_Mirado_Open_05_C"_hash:
					case "BP_Niva_01_C"_hash:
					case "BP_Niva_02_C"_hash:
					case "BP_Niva_03_C"_hash:
					case "BP_Niva_04_C"_hash:
					case "BP_Niva_05_C"_hash:
					case "BP_Niva_06_C"_hash:
					case "BP_Niva_07_C"_hash:
					case "BP_Niva_Esports_C"_hash:
					case "BP_Pillar_Car_C"_hash:
					case "BP_PonyCoupe_C"_hash:
					case "BP_Porter_C"_hash:
					case "BP_Scooter_01_A_C"_hash:
					case "BP_Scooter_02_A_C"_hash:
					case "BP_Scooter_03_A_C"_hash:
					case "BP_Scooter_04_A_C"_hash:
					case "BP_Snowbike_01_C"_hash:
					case "BP_Snowbike_02_C"_hash:
					case "BP_Snowmobile_01_C"_hash:
					case "BP_Snowmobile_02_C"_hash:
					case "BP_Snowmobile_03_C"_hash:
					case "BP_Special_FbrBike_C"_hash:
					case "BP_TukTukTuk_A_01_C"_hash:
					case "BP_TukTukTuk_A_02_C"_hash:
					case "BP_TukTukTuk_A_03_C"_hash:
					case "Dacia_A_01_v2_C"_hash:
					case "Dacia_A_01_v2_snow_C"_hash:
					case "Dacia_A_02_v2_C"_hash:
					case "Dacia_A_03_v2_C"_hash:
					case "Dacia_A_03_v2_Esports_C"_hash:
					case "Dacia_A_04_v2_C"_hash:
					case "BP_PicoBus_C"_hash:
					{
						if (!Aactor->_transforActor)
						{
							Aactor->_transforActor = true;
							std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
							ObjectManager::AddObject(*actor, EVehicle);
							CONSOLE_INFO("%s Set EVehicle", Aactor->_ActorName.c_str());
						}
						break;
					}
					case "Boat_PG117_C"_hash:
					case "PG117_A_01_C"_hash:
					case "AquaRail_A_01_C"_hash:
					case "AquaRail_A_02_C"_hash:
					case "AquaRail_A_03_C"_hash:
					case "AquaRail_C"_hash:
					case "AirBoat_V2_C"_hash:
					{
						//if (!Aactor->_transforActor)
						{
							Aactor->_transforActor = true;
							std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
							ObjectManager::AddObject(*actor, EBoat);
							CONSOLE_INFO("%s Set EBoat", Aactor->_ActorName.c_str());
						}
						break;
					}
					case "DroppedItemGroup"_hash:
					{
						//if (!Aactor->_transforActor)
						{
							Aactor->_transforActor = true;
							std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
							ObjectManager::AddObject(*actor, EItemGroup);
							CONSOLE_INFO("%s Set EItemGroup", Aactor->_ActorName.c_str());
						}
						break;
					}
					case "DroppedItem"_hash:
					{
						//if (!Aactor->_transforActor)
						{
							Aactor->_transforActor = true;
							std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
							ObjectManager::AddObject(*actor, EItem);
							CONSOLE_INFO("%s Set EItem", Aactor->_ActorName.c_str());
						}
						break;
					}
					case "DroppedItemInteractionComponent"_hash:
					{
						//if (!Aactor->_transforActor)
						{
							Aactor->_transforActor = true;
							std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
							ObjectManager::AddObject(*actor, EDroppedItem);
							CONSOLE_INFO("%s Set EDroppedItem", Aactor->_ActorName.c_str());
						}
						break;
					}
					case "Carapackage_RedBox_C"_hash:
					{
						//if (!Aactor->_transforActor)
						{
							Aactor->_transforActor = true;
							std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
							ObjectManager::AddObject(*actor, EAirdrop);
							CONSOLE_INFO("%s Set EAirdrop", Aactor->_ActorName.c_str());
						}
						break;
					}
					case "DeathDropItemPackage_C"_hash:
					{
						//if (!Aactor->_transforActor)
						{
							Aactor->_transforActor = true;
							std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
							ObjectManager::AddObject(*actor, EDeathBox);
							CONSOLE_INFO("%s Set EDeathBox", Aactor->_ActorName.c_str());
						}
						break;
					}
					case "ProjFlashBang_C"_hash:
					case "ProjGrenade_C"_hash:
					case "ProjSmokeBomb_v2_C"_hash:
					case "ProjMolotov_C"_hash:
					{
						//if (!Aactor->_transforActor)
						{
							Aactor->_transforActor = true;
							std::unique_ptr <AActor> actor(new AActor(Aactor->GetPtr()));
							ObjectManager::AddObject(*actor, EProjectile);
							CONSOLE_INFO("%p|%s Set EProjectile", Aactor->GetPtr(), Aactor->_ActorName.c_str());
						}
						break;
					}
					default:
						break;
					}
				}
				break;

			}
			case IronMan::Core::SDK::EUnknown:
			{
				auto Aactor = dynamic_cast<AActor*>(actor.get());
				if (!Aactor || !Aactor->IsValid())
					return;
				Aactor->_EncryptActorID = 0;
				Aactor->_ActorName = "";
				break;
			}
			default:
				break;
			}
		}

		void CacheActorInVehicle(std::shared_ptr<AActor> actor, std::vector<ptr_t>& tempVAMap)
		{
			if (!actor || !(*actor)->IsValid())
				return;
			auto player = dynamic_cast<ATslCharacter*>(actor.get());
			if (!player || !player->IsValid())
				return;
			if (player->_LastVehiclePawn)
				tempVAMap.push_back((ptr_t)player->_LastVehiclePawn);
		}

		void BeginFrame()
		{
			bestPlayerFov = FLT_MAX;
			playerDistMin = FLT_MAX;
			bestGroggyPlayerFov = FLT_MAX;
			aimTarget = nullptr;
			aimTargetByDist = nullptr;
			aimTargetByGroggy = nullptr;
			for (int i = 0; i <= EOther; i++)
				sdk::m_targets[(UObjectType)i] = TargetInfo{ SelectorFOV ,nullptr ,nullptr };
		}

		void EvaluateTarget(std::shared_ptr<AActor> actor, UObjectType type)
		{
			if (!actor || !(*actor)->IsValid())
				return;
			if ((void*)sdk::GetPlayer() && actor->GetPtr() == (void*)sdk::GetPlayer())
				return;
			FVector2D _2NAxis;
			switch (type)
			{
			case IronMan::Core::SDK::EPlayer:
			{
				auto player = dynamic_cast<ATslCharacter*>(actor.get());
				if (!player || !player->IsValid() || !player->_bInit)
					return;
				if (Vars.espHuman.Collaborator && player->_PartnerLevel > 0)
					player->_isParthers = true;
				if (!player->_isInFog)
					return;
				if (Vars.Misc.BlackListShow)
				{
					if (player->_LastTeamNum <= 100)
					{
						if (CheckPlayerIsInBlackListMap(hash_(player->_ShowName)))
							player->_bBlackList = true;
						else
						{
							if (CheckPlayerIsInBlackListMap(hash_(player->_TeamName)))
								player->_bBlackList = true;
							else
								player->_bBlackList = false;
						}
					}
					else
					{
						player->_bBlackList = false;
					}
				}
				else
					player->_bBlackList = false;

				if (player->_CacheLocation.IsZero() || !sdk::WorldToScreen(player->_CacheLocation, _2NAxis))
					return;
				auto pos = player->_Location;
				auto pos1 = sdk::GetBonePosition(*player, Bone::forehead);
				auto pos2 = sdk::GetBonePosition(*player, Bone::spine_01);
				auto LocalPos = sdk::GetLocalPos();
				auto delta = (pos - LocalPos);
				auto delta1 = (pos1 - LocalPos);
				auto delta2 = (pos2 - LocalPos);
				float dis = delta.Size() / 100.0f;
				FRotator ideal = Clamp(Vec2Rot(delta));
				FRotator ideal1 = Clamp(Vec2Rot(delta1));
				FRotator ideal2 = Clamp(Vec2Rot(delta2));
				auto LocalRot = sdk::GetLocalRot();
				auto angDelta = Clamp(ideal - LocalRot);
				auto angDelta1 = Clamp(ideal1 - LocalRot);
				auto angDelta2 = Clamp(ideal2 - LocalRot);
				float angFOV = angDelta.Size();
				float angFOV1 = angDelta1.Size();
				float angFOV2 = angDelta2.Size();
				if (angFOV < sdk::m_targets[type].Fov)
				{
					sdk::m_targets[type].Fov = angFOV;
					sdk::m_targets[type].Target = actor;
				}
				player->_delta = delta;
				player->_dist = dis;
				try
				{
					auto WeaponType = sdk::GetWeaponType();
					bool IsAimkeyDown = false;
					if (WeaponType == 0)
					{
						IsAimkeyDown = (OverlayEngine::IsKeyDown(Vars.Aimbot.AR_HotKey1) || OverlayEngine::IsKeyDown(Vars.Aimbot.AR_HotKey2));
					}
					else if (WeaponType == 1)
					{
						IsAimkeyDown = (OverlayEngine::IsKeyDown(Vars.Aimbot.SR_HotKey1) || OverlayEngine::IsKeyDown(Vars.Aimbot.SR_HotKey2));
					}
					else
					{
						IsAimkeyDown = (OverlayEngine::IsKeyDown(Vars.Aimbot.DMR_HotKey1) || OverlayEngine::IsKeyDown(Vars.Aimbot.DMR_HotKey2));
					}

					bool GrenadeLock = false;
					if (Vars.Aimbot.GrenadePredict)
						GrenadeLock = true;
					else
						GrenadeLock = sdk::GetIsGrenade() ? false : true;

					if (GrenadeLock && Vars.Aimbot.Aimbot && IsAimkeyDown)
					{
						if (CheckPlayerIsInWhiteListMap(hash_(player->_ShowName)))
							return;
						float health = player->_Health;
						float groggyHealth = player->_GroggyHealth;
						bool isDeath = health <= 0.0f && groggyHealth <= 0.0f;
						bool _isGroggy = (health <= 0.0f);
						bool duiyou = !!player->_Team;
						if (player && (!Vars.Aimbot.Team ? true : !duiyou) && !isDeath)
						{
							bool isKnock = WeaponType == 0 ? Vars.Aimbot.AR_Knock : WeaponType == 1 ? Vars.Aimbot.SR_Knock : Vars.Aimbot.DMR_Knock;
							if (!isKnock && !OverlayEngine::IsKeyDown(Vars.Aimbot.InKnockDownHotKey) && _isGroggy)
								return;
							float fovDis = angFOV * dis;
							float FovRange = (!Vars.Aimbot.AimMode ? (WeaponType == 0 ? Vars.Aimbot.AR_FovRange1 : WeaponType == 1 ? Vars.Aimbot.SR_FovRange1 : Vars.Aimbot.DMR_FovRange1) :
								(WeaponType == 0 ? Vars.Aimbot.AR_FovRange2 : WeaponType == 1 ? Vars.Aimbot.SR_FovRange2 : Vars.Aimbot.DMR_FovRange2));
							float Percent = sdk::GetFov() / 80.f;
							float ScopeangFov = angFOV / Percent;
							float ScopeangFov1 = angFOV1 / Percent;
							float ScopeangFov2 = angFOV2 / Percent;
							if (_isGroggy)
							{
								fovDis *= 1.5f;
							}
							static ptr_t lastFireTime = GetTickCount64();
							if (OverlayEngine::IsKeyDown(VK_LBUTTON))
								lastFireTime = GetTickCount64();

							bool bIsMortar = IsAddrValid(sdk::GetMortarPlayer());
							if (bIsMortar ? std::min(std::max(1.0f, 15.f), 30.0f) : ScopeangFov < std::min(std::max(1.0f, (dis <= 5.f) ? 8.f : FovRange), 30.0f)
								|| ScopeangFov1 < std::min(std::max(1.0f, (dis <= 5.f) ? 8.f : FovRange), 30.0f)
								|| ScopeangFov2 < std::min(std::max(1.0f, (dis <= 5.f) ? 8.f : FovRange), 30.0f))
							{
								if (fovDis < bestPlayerFov)
								{
									bestPlayerFov = fovDis;
									aimTarget = player->GetPtr();
								}

								if (_isGroggy && fovDis < bestGroggyPlayerFov)
								{
									bestGroggyPlayerFov = fovDis;
									aimTargetByGroggy = player->GetPtr();
								}
							}
							else if (sdk::GetAimTarget() == (ptr_t)player->GetPtr() && !player->_LastVehiclePawn)
							{
								if (Vars.Aimbot.AimbotDrag)
								{
									bool bIsGrenade = false;
									if (Vars.Aimbot.GrenadePredict && sdk::GetIsGrenade())
										bIsGrenade = true;
									if (!bIsGrenade)
									{
										if (GetTickCount64() - lastFireTime >= 700 || WeaponType == 0)
										{
											aimLock = false;
											sdk::SetAimTarget(nullptr);
										}
									}
								}
							}
						}
					}
				}
				catch (...)
				{

				}
				break;
			}
			case IronMan::Core::SDK::EVehicle:
			{
				auto vehicle = dynamic_cast<ATslWheeledVehicle*>(actor.get());
				if (!vehicle || !vehicle->IsValid())
					return;
				auto pos = vehicle->_Location;
				auto delta = (pos - sdk::GetLocalPos());
				float dis = delta.Size() / 100.0f;

				FRotator ideal = Clamp(Vec2Rot(delta));
				auto angDelta = Clamp(ideal - sdk::GetLocalRot());
				float angFOV = angDelta.Size();
				if (angFOV < sdk::m_targets[type].Fov)
				{
					sdk::m_targets[type].Fov = angFOV;
					sdk::m_targets[type].Target = actor;
				}
				vehicle->_delta = delta;
				vehicle->_dist = dis;
				break;
			}
			case IronMan::Core::SDK::EBoat:
			{
				auto boat = dynamic_cast<ATslFloatingVehicle*>(actor.get());
				if (!boat || !boat->IsValid())
					return;
				auto pos = boat->_Location;
				auto delta = (pos - sdk::GetLocalPos());
				float dis = delta.Size() / 100.0f;

				FRotator ideal = Clamp(Vec2Rot(delta));
				auto angDelta = Clamp(ideal - sdk::GetLocalRot());
				float angFOV = angDelta.Size();
				if (angFOV < sdk::m_targets[type].Fov)
				{
					sdk::m_targets[type].Fov = angFOV;
					sdk::m_targets[type].Target = actor;
				}
				boat->_delta = delta;
				boat->_dist = dis;
				break;
			}
			case IronMan::Core::SDK::EItem:
			{
				auto item = dynamic_cast<ADroppedItem*>(actor.get());
				if (!item || !item->IsValid())
					return;
				auto pos = item->_Location;
				auto delta = (pos - sdk::GetLocalPos());
				float dis = delta.Size() / 100.0f;

				FRotator ideal = Clamp(Vec2Rot(delta));
				auto angDelta = Clamp(ideal - sdk::GetLocalRot());
				float angFOV = angDelta.Size();
				if (angFOV < sdk::m_targets[type].Fov)
				{
					sdk::m_targets[type].Fov = angFOV;
					sdk::m_targets[type].Target = actor;
				}
				item->_delta = delta;
				item->_dist = dis;
				break;
			}
			case IronMan::Core::SDK::EDeathBox:
			{
				auto deadbox = dynamic_cast<AFloorSnapItemPackage*>(actor.get());
				if (!deadbox || !deadbox->IsValid())
					return;
				auto pos = deadbox->_Location;
				auto delta = (pos - sdk::GetLocalPos());
				float dis = delta.Size() / 100.0f;

				FRotator ideal = Clamp(Vec2Rot(delta));
				auto angDelta = Clamp(ideal - sdk::GetLocalRot());
				float angFOV = angDelta.Size();
				if (angFOV < sdk::m_targets[type].Fov)
				{
					sdk::m_targets[type].Fov = angFOV;
					sdk::m_targets[type].Target = actor;
				}
				deadbox->_delta = delta;
				deadbox->_dist = dis;
				break;
			}
			case IronMan::Core::SDK::EAirdrop:
			{
				auto airdrop = dynamic_cast<ACarePackageItem*>(actor.get());
				if (!airdrop || !airdrop->IsValid())
					return;
				auto pos = airdrop->_Location;
				auto delta = (pos - sdk::GetLocalPos());
				float dis = delta.Size() / 100.0f;

				FRotator ideal = Clamp(Vec2Rot(delta));
				auto angDelta = Clamp(ideal - sdk::GetLocalRot());
				float angFOV = angDelta.Size();
				if (angFOV < sdk::m_targets[type].Fov)
				{
					sdk::m_targets[type].Fov = angFOV;
					sdk::m_targets[type].Target = actor;
				}
				airdrop->_delta = delta;
				airdrop->_dist = dis;
				break;
			}
			case IronMan::Core::SDK::EProjectile:
			{
				auto proj = dynamic_cast<ATslProjectile*>(actor.get());
				if (!proj || !proj->IsValid())
					return;
				auto pos = proj->_Location;
				auto delta = (pos - sdk::GetLocalPos());
				float dis = delta.Size() / 100.0f;

				FRotator ideal = Clamp(Vec2Rot(delta));
				auto angDelta = Clamp(ideal - sdk::GetLocalRot());
				float angFOV = angDelta.Size();
				if (angFOV < sdk::m_targets[type].Fov)
				{
					sdk::m_targets[type].Fov = angFOV;
					sdk::m_targets[type].Target = actor;
				}
				proj->_delta = delta;
				proj->_dist = dis;
				break;
			}
			case IronMan::Core::SDK::EOther:
			{
				auto Aactor = dynamic_cast<AActor*>(actor.get());
				if (!Aactor || !Aactor->IsValid())
					return;
				auto pos = Aactor->_Location;
				auto delta = (pos - sdk::GetLocalPos());
				float dis = delta.Size() / 100.0f;

				FRotator ideal = Clamp(Vec2Rot(delta));
				auto angDelta = Clamp(ideal - sdk::GetLocalRot());
				float angFOV = angDelta.Size();
				if (angFOV < sdk::m_targets[type].Fov)
				{
					sdk::m_targets[type].Fov = angFOV;
					sdk::m_targets[type].Target = actor;
				}
				Aactor->_delta = delta;
				Aactor->_dist = dis;
				break;
			}
			default:
				break;
			}
			return;
		}

		void EndFrame()
		{
			bool IsAimkeyDown = false;
			auto WeaponType = sdk::GetWeaponType();
			if (WeaponType == 0)
			{
				IsAimkeyDown = (OverlayEngine::IsKeyDown(Vars.Aimbot.AR_HotKey1) || OverlayEngine::IsKeyDown(Vars.Aimbot.AR_HotKey2));
			}
			else if (WeaponType == 1)
			{
				IsAimkeyDown = (OverlayEngine::IsKeyDown(Vars.Aimbot.SR_HotKey1) || OverlayEngine::IsKeyDown(Vars.Aimbot.SR_HotKey2));
			}
			else
			{
				IsAimkeyDown = (OverlayEngine::IsKeyDown(Vars.Aimbot.DMR_HotKey1) || OverlayEngine::IsKeyDown(Vars.Aimbot.DMR_HotKey2));
			}

			bool GrenadeLock = false;
			if (Vars.Aimbot.GrenadePredict)
				GrenadeLock = true;
			else
				GrenadeLock = sdk::GetIsGrenade() ? false : true;

			if (GrenadeLock && !OverlayEngine::IsKeyDown(Vars.Aimbot.PauseLockKey) && IsAimkeyDown)
			{
				std::unique_ptr<ATslCharacter*> target(new ATslCharacter * (nullptr));
				ptr_t targetPtr = sdk::GetAimTarget();
				if (IsAddrValid(targetPtr))
				{
					for (auto actor : sdk::m_cacheObjectsActor[EPlayer])
					{
						if (!actor || !actor->IsValid())
							continue;
						auto AimPlayer = *actor.get()->CCast<ATslCharacter>();
						if (!AimPlayer || !AimPlayer->IsValid() || !AimPlayer->_bInit)
							continue;
						if (AimPlayer->GetPtr() == (void*)targetPtr)
						{
							*target = dynamic_cast<ATslCharacter*>(actor.get());
							break;
						}
					}
				}
				if ((void*)sdk::GetPlayer() && (*target)->GetPtr() == (void*)sdk::GetPlayer())
					return;
				bool isKnock = WeaponType == 0 ? Vars.Aimbot.AR_Knock : WeaponType == 1 ? Vars.Aimbot.SR_Knock : Vars.Aimbot.DMR_Knock;
				if (!*target || !(*target)->IsValid()
					|| ((*target)->_Health <= 0.0f && (*target)->_GroggyHealth <= 0.0f))//已经死亡,切换下一个目标
				{
					if (!(*target)->IsValid())
					{
						sdk::SetAimTarget(nullptr);
						sdk::SetLockTarget(nullptr);
						aimLock = false;
					}
					if (!aimLock)//没有锁定,就切换目标
					{
						if (bestPlayerFov < FLT_MAX && aimTarget)
						{
							sdk::SetAimTarget(aimTarget);
							sdk::SetLockTarget(aimTarget);
						}
						else if (bestGroggyPlayerFov < FLT_MAX && aimTargetByGroggy)
						{
							sdk::SetAimTarget(aimTargetByGroggy);
							sdk::SetLockTarget(aimTargetByGroggy);
						}
					}
					else
					{
						sdk::SetAimTarget(nullptr);
						sdk::SetLockTarget(nullptr);
					}
				}
				else if (!(*target)->IsValid() || ((*target)->_Health <= 0.f && (*target)->_GroggyHealth > 0.0f) ? ((!isKnock && !OverlayEngine::IsKeyDown(Vars.Aimbot.InKnockDownHotKey)) ? true : false) : false)
				{
					aimLock = false;
					sdk::SetAimTarget(nullptr);
					sdk::SetLockTarget(nullptr);
				}
				else
				{
					aimLock = true;
				}
			}
			else
			{
				aimLock = false;
				sdk::SetAimTarget(nullptr);
				sdk::SetLockTarget(nullptr);
			}

			for (int i = 0; i <= EOther; i++)
			{
				sdk::m_targets[(UObjectType)i].LastTarget = sdk::m_targets[(UObjectType)i].Target;
				if (sdk::m_targets[(UObjectType)i].Fov >= SelectorFOV)
				{
					sdk::m_targets[(UObjectType)i].Target = nullptr;
				}
			}
		}
	}

	uint8_t Visibility = 0;
	FAnchorData LayoutData;
	FTransform FiringTransform;
	FTransform ScopingTransform;
	//大地图
	TArray<void*> pBlockInputWidgetList;
	ptr_t pMapGrid = 0;
	//TWeakObjectPtr<class UMapGridWidget>pMapGrid;
	//FUObjectItem pMapGridObj;
	void* pMapPoint = nullptr;

	std::vector<ptr_t> g_BlockInputWidgetListData;
	std::vector<int32_t> g_BlockInputWidgetListDataEncryptID;
	bool InitArray = false;

	ptr_t g_SkydiveComponent = 0;

	void sdk::UpdateViewPortVEH()
	{
		auto ExecHandle = sdk::GetViewPortHandle();
		if (!ExecHandle)
			return;
		int count = 0;
		int scatterCount = 0;
		auto start = std::chrono::high_resolution_clock::now();
		auto end = start + std::chrono::seconds(1);
		using framerate = std::chrono::duration<int, std::ratio<1, 500>>;
		auto tp = std::chrono::system_clock::now() + framerate{ 1 };
		while (std::chrono::high_resolution_clock::now() < end) {

			float TempFov = 0.f;
			FVector TemplocalPos = FVector();
			FRotator TemplocalRot = FRotator();
			POINT t_CursorPosition = POINT();
			//F3 0F 2D C9 D1 F9 D1 F8
			auto MousePtr = sdk::GetMousePosPtr();
			if (IsAddrValid(MousePtr))
				GetDMA().queueScatterReadEx(ExecHandle, MousePtr + 0x1E8, &t_CursorPosition, sizeof(POINT));
			auto FOVPtr = sdk::GetViewTarget_POV_FOV();
			auto LocationPtr = sdk::GetViewTarget_POV_Location();
			auto RotationPtr = sdk::GetViewTarget_POV_Rotation();
			if (IsAddrValid(FOVPtr) && IsAddrValid(LocationPtr) && IsAddrValid(RotationPtr))
			{
				GetDMA().queueScatterReadEx(ExecHandle, FOVPtr, &TempFov, sizeof(float));
				GetDMA().queueScatterReadEx(ExecHandle, LocationPtr, &TemplocalPos, sizeof(FVector));
				GetDMA().queueScatterReadEx(ExecHandle, RotationPtr, &TemplocalRot, sizeof(FVector));
			}
			scatterCount = GetDMA().executeScatterRead(ExecHandle);

			if (TempFov != 0.f)
			{
				sdk::SetFov(TempFov);
				sdk::SetMaxFov(TempFov);
			}
			if (!TemplocalPos.IsZero() && TemplocalPos.IsVaild())
				sdk::SetLocalPos(TemplocalPos);
			if (!TemplocalRot.IsZero() && TemplocalRot.IsVaild())
				sdk::SetLocalRot(TemplocalRot);
			sdk::SetCursorPosition(t_CursorPosition);
			std::this_thread::sleep_until(tp);
			tp += framerate{ 1 };
			count++;
		}
		CONSOLE_INFO("View:%d|%f", count, (float)count / 1000.f);

		return;
	}

	DWORD sdk::UpdateViewPort(LPVOID Param)
	{
		using framerate = std::chrono::duration<int, std::ratio<1, 144>>;
		auto tp = std::chrono::system_clock::now() + framerate{ 1 };
		do
		{
			try
			{
				if (Vars.Menu.PauseEveryThing)
				{
					Sleep(1000);
					continue;
				}
				UpdateViewPortVEH();
				if (!sdk::InGame())
				{
					std::this_thread::sleep_until(tp);
					tp += framerate{ 1 };
				}
			}
			catch (...)
			{
				CONSOLE_INFO2("UpdateViewPort Exception!!!");
			}
		} while (true);
		return 0;
	}

	//更新自身
	void sdk::UpdateSelfPlayerVEH()
	{
		if (!InitArray)
		{
			g_BlockInputWidgetListData.resize(10);
			g_BlockInputWidgetListDataEncryptID.resize(10);
			InitArray = true;
		}
		auto ExecHandle = sdk::GetSelfUpdateHandle();
		if (!ExecHandle)
			return;
		std::unique_ptr<int> t_ThirdPersonValue(new int(0));
		std::unique_ptr<int> t_FireState(new int(0));
		std::unique_ptr<int> t_SpectatedCount(new int(0));
		std::unique_ptr<FVector> t_ComponentLocation(new FVector(0, 0, 0));
		std::unique_ptr<float> t_LastRenderTimeOnScreen(new float(0.f));
		std::unique_ptr<int> t_IsReloading_CP(new int(0));
		std::unique_ptr<int> t_IsAiming_CP(new int(0));
		std::unique_ptr<float> t_IsScopingAlpha_CP(new float(0.f));
		std::unique_ptr<int> t_bIsScoping_CP(new int(0.f));
		std::unique_ptr <FRotator> t_ControlRotation_CP(new FRotator(0.f, 0.f, 0.f));
		std::unique_ptr<int> t_bSpawnBulletFromBarrel(new int(0.f));
		std::unique_ptr<FReplicatedSkinParam> t_WeaponReplicatedSkin(new FReplicatedSkinParam());
		std::unique_ptr <FRotator> t_ControlRotation(new FRotator(0.f, 0.f, 0.f));
		std::unique_ptr<int> t_SelectMinimapSizeIndex(new int(0));
		std::unique_ptr<int> t_LastTeamNum(new int(0));

		//需要赋值的局部变量
		static FTransform t_FiringLocation = FTransform();
		static FTransform t_FiringSocketLocalTransform = FTransform();
		static FTransform t_ScopingSocketLocalTransform = FTransform();

		bool MapOpen = false;
		BYTE t_SkydiveState = 0;

		auto bAliveLocal = sdk::IsLocalPlayerAlive();
		auto& player = sdk::GetPlayer();
		if (IsAddrValid(player) && bAliveLocal)
		{
			GetDMA().queueScatterReadEx(ExecHandle, player + g_PatternData.offset_ATslCharacter_LastTeamNum, t_LastTeamNum.get(), 4);
			GetDMA().queueScatterReadEx(ExecHandle, player + g_PatternData.offset_bThirdPerson, t_ThirdPersonValue.get(), 4);
			GetDMA().queueScatterReadEx(ExecHandle, player + g_PatternData.offset_FireState, t_FireState.get(), 4);
			GetDMA().queueScatterReadEx(ExecHandle, player + g_PatternData.offset_ATslCharacter_SpectatedCount, t_SpectatedCount.get(), 4);
			auto LocalRoot = sdk::GetLocalRoot();
			if (IsAddrValid(LocalRoot))
				GetDMA().queueScatterReadEx(ExecHandle, LocalRoot + g_PatternData.ComponentLocationOffset, t_ComponentLocation.get(), sizeof(FVector));

			auto& Mesh = sdk::GetMesh();
			if (IsAddrValid(Mesh))
			{
				GetDMA().queueScatterReadEx(ExecHandle, Mesh + g_PatternData.offset_UPrimitiveComponent_LastRenderTimeOnScreen, t_LastRenderTimeOnScreen.get(), 4);
				auto tslAnim = sdk::GetLocalAnimScriptInstance();
				if (IsAddrValid(tslAnim))
				{
					GetDMA().queueScatterReadEx(ExecHandle, tslAnim + g_PatternData.offset_UTslAnimInstance_bIsReloading_CP, t_IsReloading_CP.get(), sizeof(int));
					GetDMA().queueScatterReadEx(ExecHandle, tslAnim + g_PatternData.offset_UTslAnimInstance_bIsAiming_CP, t_IsAiming_CP.get(), sizeof(bool));
					GetDMA().queueScatterReadEx(ExecHandle, tslAnim + g_PatternData.offset_UTslAnimInstance_ScopingAlpha_CP, t_IsScopingAlpha_CP.get(), sizeof(float));
					GetDMA().queueScatterReadEx(ExecHandle, tslAnim + g_PatternData.offset_UTslAnimInstance_bIsScoping_CP, t_bIsScoping_CP.get(), sizeof(bool));
					GetDMA().queueScatterReadEx(ExecHandle, tslAnim + g_PatternData.offset_UTslAnimInstance_ControlRotation_CP, t_ControlRotation_CP.get(), sizeof(FRotator));
				}
			}
			auto& Weapon = sdk::GetWeapon();
			if (IsAddrValid(Weapon))
			{
				GetDMA().queueScatterReadEx(ExecHandle, Weapon + g_PatternData.offset_bSpawnBulletFromBarrel, t_bSpawnBulletFromBarrel.get(), sizeof(bool));
			}
			else
			{
				*t_bSpawnBulletFromBarrel = 0;
			}

			auto& WeaponMesh3P = sdk::GetMesh3P();
			if (IsAddrValid(WeaponMesh3P))
			{
				GetDMA().queueScatterReadEx(ExecHandle, WeaponMesh3P + g_PatternData.offset_USkinnedMeshComponent_CurrentReadComponentTransforms, &sdk::m_SkinMesh3P_CurrentReadComponentTransforms, sizeof(int32_t));
				GetDMA().queueScatterReadEx(ExecHandle, WeaponMesh3P + g_PatternData.offset_ComponentToWorld, &t_FiringLocation, sizeof(FTransform));
				GetDMA().queueScatterReadEx(ExecHandle, WeaponMesh3P + g_PatternData.offset_UWeaponMeshComponent_ReplicatedSkinParam, t_WeaponReplicatedSkin.get(), sizeof(FReplicatedSkinParam));
				auto FiringAttachPoint_Socket = sdk::GetFiringAttachPoint_Socket();
				auto ScopingAttachPoint_Socket = sdk::GetScopingAttachPoint_Socket();
				if (IsAddrValid(FiringAttachPoint_Socket) && IsAddrValid(ScopingAttachPoint_Socket))
				{
					GetDMA().queueScatterReadEx(ExecHandle, (ptr_t)FiringAttachPoint_Socket + g_PatternData.offset_SkeletalMeshSocket_RelativeRotation, &t_FiringSocketLocalTransform.Rotation, sizeof(FQuat));
					GetDMA().queueScatterReadEx(ExecHandle, (ptr_t)FiringAttachPoint_Socket + g_PatternData.offset_SkeletalMeshSocket_RelativeLocation, &t_FiringSocketLocalTransform.Translation, sizeof(FVector));
					GetDMA().queueScatterReadEx(ExecHandle, (ptr_t)FiringAttachPoint_Socket + g_PatternData.offset_SkeletalMeshSocket_RelativeScale, &t_FiringSocketLocalTransform.Scale3D, sizeof(FVector));

					GetDMA().queueScatterReadEx(ExecHandle, (ptr_t)ScopingAttachPoint_Socket + g_PatternData.offset_SkeletalMeshSocket_RelativeRotation, &t_ScopingSocketLocalTransform.Rotation, sizeof(FQuat));
					GetDMA().queueScatterReadEx(ExecHandle, (ptr_t)ScopingAttachPoint_Socket + g_PatternData.offset_SkeletalMeshSocket_RelativeLocation, &t_ScopingSocketLocalTransform.Translation, sizeof(FVector));
					GetDMA().queueScatterReadEx(ExecHandle, (ptr_t)ScopingAttachPoint_Socket + g_PatternData.offset_SkeletalMeshSocket_RelativeScale, &t_ScopingSocketLocalTransform.Scale3D, sizeof(FVector));
				}
				else
				{
					ZeroMemory(&t_FiringSocketLocalTransform, sizeof(FTransform));
					ZeroMemory(&t_ScopingSocketLocalTransform, sizeof(FTransform));
				}
			}
			else
			{
				sdk::m_SkinMesh3P_CurrentReadComponentTransforms = 0;
				ZeroMemory(&t_FiringLocation, sizeof(t_FiringLocation));
				ZeroMemory(&t_FiringSocketLocalTransform, sizeof(FTransform));
				ZeroMemory(&t_ScopingSocketLocalTransform, sizeof(FTransform));
			}
		}
		auto& PlayerControl = sdk::GetControl();
		if (IsAddrValid(PlayerControl))
		{
			GetDMA().queueScatterReadEx(ExecHandle, PlayerControl + g_PatternData.offset_AController_ControlRotation, t_ControlRotation.get(), sizeof(FRotator));
			auto& MyHud = sdk::GetMyHUD();
			if (IsAddrValid(MyHud))
			{
				auto PtrOfLocalSelectMinimapSizeIndex = sdk::GetLocalSelectMinimapSizeIndex();
				if (IsAddrValid(PtrOfLocalSelectMinimapSizeIndex))
					GetDMA().queueScatterReadEx(ExecHandle, PtrOfLocalSelectMinimapSizeIndex, t_SelectMinimapSizeIndex.get(), 4);

				GetDMA().queueScatterReadEx(ExecHandle, MyHud + g_PatternData.offset_AHUD_BlockInputWidgetList, &pBlockInputWidgetList, sizeof(pBlockInputWidgetList));
				if (pBlockInputWidgetList->GetData() && pBlockInputWidgetList->Num() > 0 && pBlockInputWidgetList->Num() < 10)
				{
					GetDMA().queueScatterReadEx(ExecHandle, (ptr_t)pBlockInputWidgetList->GetData(), g_BlockInputWidgetListData.data(), 8 * pBlockInputWidgetList->Num());
					for (uint32_t i = 0; i < pBlockInputWidgetList.Num(); i++)
					{
						if (IsAddrValid(g_BlockInputWidgetListData[i]))
							GetDMA().queueScatterReadEx(ExecHandle, g_BlockInputWidgetListData[i] + g_PatternData.offset_UObjectNameComparisonIndex, &g_BlockInputWidgetListDataEncryptID[i], 4);
						else
							g_BlockInputWidgetListDataEncryptID[i] = 0;
					}
					//大地图
					bool bFindMap = false;
					for (int i = 0; i < (int)pBlockInputWidgetList->Num(); i++)
					{
						if (g_BlockInputWidgetListDataEncryptID[i] == 0 || !IsAddrValid(g_BlockInputWidgetListData[i]))
							continue;
						std::unique_ptr<UBlockInputUserWidget> Obj(new UBlockInputUserWidget(g_BlockInputWidgetListData[i]));
						if (Obj->IsValid())
						{
							FName MapName;
							MapName.ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(g_BlockInputWidgetListDataEncryptID[i]);
							auto MName = MapName.GetName2();
							if (Utils::StringIsValid(MName))
							{
								switch (hash_(MName))
								{
								case "NewWorldMapWidget_BP_C"_hash:
								case "BP_FBRWorldMapWidget_C"_hash:
								{
									pMapPoint = Obj->GetPtr();
									/*	static int OffsetMapGrid = 0x8;
										static bool bFindOffset = false;
										if (!bFindOffset)
										{
											for (size_t i = 0; i < 0x1000; i++)
											{
												UObject Test = (UObject)GetDMA().Read<void*>((ptr_t)Obj->GetPtr() + OffsetMapGrid);
												if (Test.GetName() == "MapGrid")
												{
													bFindOffset = true;
													break;
												}
												OffsetMapGrid += 8;
											}
										}*/
									GetDMA().queueScatterReadEx(ExecHandle, (ptr_t)Obj->GetPtr() + g_PatternData.offset_UTslNewWorldMapWidget_MapGrid, &pMapGrid, sizeof(pMapGrid));
									bFindMap = true;
									break;
								}
								case "UI_TrainingWorldMapWidget_C"_hash:
								{
									pMapPoint = Obj->GetPtr();
									GetDMA().queueScatterReadEx(ExecHandle, (ptr_t)Obj->GetPtr() + g_PatternData.offset_TslTrainingWorldMapWidget_TrainingMapGrid, &pMapGrid, sizeof(pMapGrid));
									bFindMap = true;
									break;
								}
								default:
									break;
								}
							}
						}
					}

					if (!bFindMap)
					{
						pMapPoint = nullptr;
						pMapGrid = 0;
					}

					//大地图
					if (pMapPoint && IsAddrValid(pMapGrid))
					{
						auto GlobalObjects = sdk::GetObjectArray();
						if (IsAddrValid(GlobalObjects.Objects.Get()))
						{
							//GetDMA().queueScatterReadEx(ExecHandle, (ptr_t)GlobalObjects.Objects.Get() + pMapGrid.ObjectIndex * sizeof(FUObjectItem), &pMapGridObj, sizeof(FUObjectItem));
							//if (IsAddrValid(pMapGridObj.Object))
							{
								std::unique_ptr<UMapGridWidget> MapGridWidget(new UMapGridWidget(pMapGrid));
								CONSOLE_INFO("%s", MapGridWidget->GetName().c_str());
								if (MapGridWidget->IsValid())
								{
									sdk::m_MapGridWidget = MapGridWidget->GetPtr();
									GetDMA().queueScatterReadEx(ExecHandle, (ptr_t)MapGridWidget->GetPtr() + g_PatternData.offset_UWidget_Slot, &sdk::m_MapGridWidgetSlot, 8);
								}
								else
								{
									sdk::m_MapGridWidget = nullptr;
									sdk::m_MapGridWidgetSlot = nullptr;
									ZeroMemory(&pMapGrid, sizeof(pMapGrid));
								}
							}
							/*else
							{
								sdk::m_MapGridWidget = nullptr;
								sdk::m_MapGridWidgetSlot = nullptr;
								ZeroMemory(&pMapGridObj, sizeof(pMapGridObj));
							}*/
						}
						else
						{
							sdk::m_MapGridWidget = nullptr;
							sdk::m_MapGridWidgetSlot = nullptr;
							ZeroMemory(&pMapGrid, sizeof(pMapGrid));
						}
					}
					else
					{
						sdk::m_MapGridWidget = nullptr;
						sdk::m_MapGridWidgetSlot = nullptr;
						ZeroMemory(&pMapGrid, sizeof(pMapGrid));
					}

					MapOpen = sdk::m_MapGridWidget && sdk::m_MapGridWidgetSlot;

					if (MapOpen && IsAddrValid(sdk::m_MapGridWidget) && IsAddrValid(sdk::m_MapGridWidgetSlot))
					{
						GetDMA().queueScatterReadEx(ExecHandle, (ptr_t)sdk::m_MapGridWidget + g_PatternData.offset_UWidget_Visibility, &Visibility, sizeof(uint8_t));
						GetDMA().queueScatterReadEx(ExecHandle, (ptr_t)sdk::m_MapGridWidgetSlot + g_PatternData.offset_UCanvasPanelSlot_LayoutData, &LayoutData, sizeof(FAnchorData));
					}
					else
					{
						sdk::m_HudBlockInputArray = UBlockInputUser();
						Visibility = 0;
						ZeroMemory(&LayoutData, sizeof(FAnchorData));
					}

					if (MapOpen && IsAddrValid(sdk::m_MapGridWidget) && IsAddrValid(sdk::m_MapGridWidgetSlot))
					{
						if ((ESlateVisibility)Visibility == ESlateVisibility::SelfHitTestInvisible)
						{
							FVector2D CurrentSize = { LayoutData.Offsets.Right, LayoutData.Offsets.Bottom };
							FVector2D CurrentPos = {
								LayoutData.Offsets.Right * (LayoutData.Alignment.X - 0.5f) - LayoutData.Offsets.Left,
								LayoutData.Offsets.Bottom * (LayoutData.Alignment.Y - 0.5f) - LayoutData.Offsets.Top
							};
							if (CurrentSize.X != 0.f)
							{
								float WorldMapZoomFactor = CurrentSize.X / 1080.f;
								FVector2D WorldMapPosition = { CurrentPos.X / 1080.f / WorldMapZoomFactor * 2.0f, CurrentPos.Y / 1080.f / WorldMapZoomFactor * 2.0f };
								auto WorldLocationX = sdk::GetWorldLocation();
								m_HudBlockInputArray.obj = sdk::m_MapGridWidget;
								m_HudBlockInputArray.WorldName = sdk::GetWorldName();
								m_HudBlockInputArray.bWorldMap = true;
								m_HudBlockInputArray.WorldMapZoomFactor = WorldMapZoomFactor;
								m_HudBlockInputArray.WorldMapPosition = WorldMapPosition;
								m_HudBlockInputArray.WorldLocationX = *(int32_t*)((ptr_t)&WorldLocationX);
								m_HudBlockInputArray.WorldLocationY = *(int32_t*)((ptr_t)&WorldLocationX + 4);
							}
						}
					}
				}
				else
				{
					goto ZeroHud;
				}
			}
			else
			{
				goto ZeroHud;
			}
		}
		else
		{
		ZeroHud:
			pMapPoint = nullptr;
			ZeroMemory(g_BlockInputWidgetListData.data(), g_BlockInputWidgetListData.size() * 8);
			ZeroMemory(g_BlockInputWidgetListDataEncryptID.data(), g_BlockInputWidgetListDataEncryptID.size() * 4);
			ZeroMemory(&pBlockInputWidgetList, sizeof(TArray<void*>));
			ZeroMemory(&pMapGrid, sizeof(pMapGrid));
			sdk::m_MapGridWidget = nullptr;
			sdk::m_MapGridWidgetSlot = nullptr;
			sdk::m_HudBlockInputArray = UBlockInputUser();
			Visibility = 0;
			ZeroMemory(&LayoutData, sizeof(FAnchorData));
		}

		auto FiringAttachPoint_Socket = sdk::GetFiringAttachPoint_Socket();
		auto ScopingAttachPoint_Socket = sdk::GetScopingAttachPoint_Socket();
		if (IsAddrValid(FiringAttachPoint_Socket) && IsAddrValid(ScopingAttachPoint_Socket) && bAliveLocal)
		{
			auto Mesh3P_CSTAP = sdk::GetMesh3P_ComponentSpaceTransformsArrayPtr();
			if (IsAddrValid(Mesh3P_CSTAP))
			{
				GetDMA().queueScatterReadEx(ExecHandle, Mesh3P_CSTAP + sdk::m_SkinMesh3P_CurrentReadComponentTransforms * sizeof(TArray<FTransform>), &sdk::m_Mesh3P_ComponentSpaceTransformsArray, sizeof(TArray<FTransform>));
				if (IsAddrValid(sdk::m_Mesh3P_ComponentSpaceTransformsArray.Data) && sdk::m_Mesh3P_ComponentSpaceTransformsArray->Num() > 0 && sdk::m_Mesh3P_ComponentSpaceTransformsArray->Num() < 1000)
				{
					int32_t NumTransforms = sdk::m_Mesh3P_ComponentSpaceTransformsArray->Num();
					auto ScopingSocket_BoneIndex = sdk::GetScopingSocket_BoneIndex();
					auto FiringSocket_BoneIndex = sdk::GetFiringSocket_BoneIndex();
					if (NumTransforms > 0 && NumTransforms < 10000 && FiringSocket_BoneIndex < NumTransforms && ScopingSocket_BoneIndex < NumTransforms)
					{
						GetDMA().queueScatterReadEx(ExecHandle, (ptr_t)sdk::m_Mesh3P_ComponentSpaceTransformsArray->GetData() + FiringSocket_BoneIndex * sizeof(FTransform), &FiringTransform, sizeof(FTransform));
						GetDMA().queueScatterReadEx(ExecHandle, (ptr_t)sdk::m_Mesh3P_ComponentSpaceTransformsArray->GetData() + ScopingSocket_BoneIndex * sizeof(FTransform), &ScopingTransform, sizeof(FTransform));
						sdk::SetFiringTransform(FiringTransform * t_FiringLocation * t_FiringSocketLocalTransform);
						sdk::SetScopingTransform(ScopingTransform * t_FiringLocation * t_ScopingSocketLocalTransform);
					}
				}
				else
					goto ZeroAll;
			}
			else
				goto ZeroAll;
		}
		else
		{
		ZeroAll:
			ZeroMemory(&t_FiringLocation, sizeof(t_FiringLocation));
			ZeroMemory(&t_FiringSocketLocalTransform, sizeof(t_FiringSocketLocalTransform));
			ZeroMemory(&t_ScopingSocketLocalTransform, sizeof(t_ScopingSocketLocalTransform));
			ZeroMemory(&FiringTransform, sizeof(FiringTransform));
			ZeroMemory(&ScopingTransform, sizeof(ScopingTransform));
			sdk::SetFiringTransform(FTransform());
			sdk::SetScopingTransform(FTransform());
		}
		GetDMA().executeScatterRead(ExecHandle);
		sdk::SetLastTeamNum(*t_LastTeamNum);
		sdk::SetbThirdPerson(*t_ThirdPersonValue);
		sdk::SetFireState(*t_FireState);
		sdk::SetSpectatedCount(*t_SpectatedCount);
		sdk::SetComponentLocation(*t_ComponentLocation);
		sdk::SetLastRenderTimeOnScreen(*t_LastRenderTimeOnScreen);
		sdk::SetIsReloading_CP(*t_IsReloading_CP);
		sdk::SetIsAiming_CP(*t_IsAiming_CP);
		sdk::SetIsScopingAlpha_CP(*t_IsScopingAlpha_CP);
		sdk::SetbIsScoping_CP(*t_bIsScoping_CP);
		sdk::SetControlRotation_CP(*t_ControlRotation_CP);
		sdk::SetbSpawnBulletFromBarrel(*t_bSpawnBulletFromBarrel);
		sdk::SetFiringLocation(t_FiringLocation);
		sdk::SetWeaponReplicatedSkin(*t_WeaponReplicatedSkin);
		sdk::SetControlRotation(*t_ControlRotation);
		sdk::SetSelectMinimapSizeIndex(*t_SelectMinimapSizeIndex);

		sdk::SetMapOpen(MapOpen);
		sdk::SetSkydiveState(t_SkydiveState);
	}

	DWORD sdk::UpdateSelfPlayer(LPVOID Param)
	{
		using framerate = std::chrono::duration<int, std::ratio<1, 300>>;
		auto tp = std::chrono::system_clock::now() + framerate{ 1 };
		do
		{
			int count = 0;
			auto start = std::chrono::high_resolution_clock::now();
			auto end = start + std::chrono::seconds(1);
			while (std::chrono::high_resolution_clock::now() < end) {
				try
				{
					if (Vars.Menu.PauseEveryThing)
					{
						Sleep(1000);
						break;
					}
					if (sdk::InGame())
					{
						UpdateSelfPlayerVEH();
						count++;
						std::this_thread::sleep_until(tp);
						tp += framerate{ 1 };
					}
					else
					{
						Visibility = 0;
						ZeroMemory(&LayoutData, sizeof(LayoutData));
						ZeroMemory(&FiringTransform, sizeof(FiringTransform));
						ZeroMemory(&ScopingTransform, sizeof(ScopingTransform));
						std::this_thread::sleep_until(tp);
						tp += framerate{ 1 };
					}
				}
				catch (...)
				{
					continue;
				}
			}
			//if (count > 0)
			//{
			//	CONSOLE_INFO("Self:%d", count);
			//}
		} while (true);
		return 1;
	}

	////更新全部数据
	//void sdk::UpdateObjectThreadVEH()
	//{
	//	static ptr_t BackupWorld = 0;
	//	static std::string BackupWorldName = "";
	//	using framerate = std::chrono::duration<int, std::ratio<1, 100>>;
	//	auto tp = std::chrono::system_clock::now() + framerate{ 1 };
	//	int count = 0;
	//	auto start = std::chrono::high_resolution_clock::now();
	//	auto end = start + std::chrono::seconds(1);
	//	while (std::chrono::high_resolution_clock::now() < end) {
	//		if (!sdk::InGame())
	//			break;
	//		auto AllObject = ObjectManager::GetObjects();
	//		for (auto& object : AllObject)
	//		{
	//			if (!object.second|| !*object.second || !object.second->IsValid() || !IsAddrValid(object.second->GetPtr()))
	//				continue;
	//			auto type = object.second->GetObjectType();
	//			if (object.second->IsValid() && type <= EOther)
	//			{
	//				if (m_player && object.second->GetPtr() == (void*)m_player)
	//					continue;
	//				m_cacheObjectsUpdate[type].push_back(object.second);
	//			}
	//		}

	//		TargetSelector::BeginFrame();
	//		for (auto& actor : m_cacheObjectsUpdate)
	//		{
	//			for (auto& object : actor.second)
	//			{
	//				if (!object || !*object || !object->IsValid() || !IsAddrValid(object->GetPtr()))
	//					continue;
	//				TargetSelector::EvaluateTarget(object, actor.first);
	//			}
	//		}
	//		TargetSelector::EndFrame();

	//		if (!m_cacheObjectsUpdate.empty())
	//		{
	//			for (auto& objects : m_cacheObjectsUpdate)
	//				objects.second.clear();
	//			m_cacheObjectsUpdate.clear();
	//		}

	//		std::this_thread::sleep_until(tp);
	//		tp += framerate{ 1 };
	//	}
	//}

	//DWORD sdk::UpdateObjectThread(LPVOID Param)
	//{
	//	do
	//	{
	//		try
	//		{
	//			if (Vars.Menu.PauseEveryThing)
	//			{
	//				Sleep(1000);
	//				continue;
	//			}
	//			if (sdk::InGame())
	//			{
	//				UpdateObjectThreadVEH();
	//			}
	//			else
	//			{
	//				Sleep(100);
	//			}
	//		}
	//		catch (...)
	//		{
	//			continue;
	//		}
	//	} while (true);
	//	return 1;
	//}

	DWORD sdk::UpdateDMADataThread(LPVOID Param)
	{
		using framerate = std::chrono::duration<int, std::ratio<1, 500>>;
		auto tp = std::chrono::system_clock::now() + framerate{ 1 };
		int ReadCount = 0;
		do
		{
			if (!sdk::InGame())
			{
				Sleep(2000);
				continue;
			}
			VMMDLL_SCATTER_HANDLE ScatterHandle = sdk::GetUpdatePlayer();
			int count = 0;
			auto start = std::chrono::high_resolution_clock::now();
			auto end = start + std::chrono::seconds(1);
			auto AllObject = ObjectManager::GetObjects();
			for (auto& object : AllObject)
			{
				if (!object.second || !*object.second || !object.second->IsValid() || !IsAddrValid(object.second->GetPtr()))
					continue;
				auto type = object.second->GetObjectType();
				if (object.second->IsValid() && type <= EOther)
				{
					m_cacheObjectsActor[type].push_back(object.second);
				}
			}

			if (AllObject.size())
				AllObject.clear();

			while (std::chrono::high_resolution_clock::now() < end) {
				if (!sdk::InGame())
					break;
				auto UpdateTime = GetTickCount64();
				std::vector<ptr_t>TempVAMap;
				for (auto& actor : sdk::m_cacheObjectsActor[EPlayer])
				{
					if (!actor || !*actor || !actor->IsValid() || !IsAddrValid(actor->GetPtr()))
						continue;
					TargetSelector::CacheActorInVehicle(actor, TempVAMap);
				}
				SetVehicleActor(TempVAMap);
				TempVAMap.clear();
				TargetSelector::BeginFrame();
				for (auto& actor : m_cacheObjectsActor)
				{
					for (auto& object : actor.second)
					{
						if (!object || !*object || !object->IsValid() || !IsAddrValid(object->GetPtr()))
							continue;
						TargetSelector::CacheAllData(object, actor.first, ScatterHandle, UpdateTime);
						TargetSelector::EvaluateTarget(object, actor.first);
					}
				}
				TargetSelector::EndFrame();
				ReadCount = GetDMA().executeScatterRead(ScatterHandle) / 1024;
				count++;
				std::this_thread::sleep_until(tp);
				tp += framerate{ 1 };
			}
			//if (sdk::InGame())
			//{
			//	for (auto& actor : m_cacheObjectsActor[EOther])
			//	{
			//		if (!actor || !*actor || !actor->IsValid() || !IsAddrValid(actor->GetPtr()))
			//			continue;
			//		TargetSelector::CacheAllData(actor, EUnknown, ScatterHandle, 0);
			//	}
			//}
			if (!m_cacheObjectsActor.empty())
			{
				//for (auto& objects : m_cacheObjectsActor)
				//	objects.second.clear();
				m_cacheObjectsActor.clear();
			}
			//if (count > 0)
			//{
			//	CONSOLE_INFO("All:%d|%f", count, (float)count / 1000.f);
			//}
		} while (true);
		return 0;
	}

	DWORD sdk::UpdateKeys(LPVOID Param)
	{
		using framerate = std::chrono::duration<int, std::ratio<1, 1000>>;
		auto tp = std::chrono::system_clock::now() + framerate{ 1 };
		do
		{
			try
			{
				OverlayEngine::UpdateKeyState();
				GetDMA().UpdateKeys();
				std::this_thread::sleep_until(tp);
				tp += framerate{ 1 };
			}
			catch (...)
			{
			}
		} while (true);
		return 1;
	}

	void GetHttp(const char* ip, int port, const char* utl) {
		auto hSession = InternetOpenA("Mozilla/4.0 (compatible; MSIE 9.0; Windows NT 6.1)", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if (hSession != NULL)
		{
			auto Connect = InternetConnectA(hSession, ip, port, NULL, NULL, 3, 0, 0);
			if (Connect != NULL) {
				auto hHttp = HttpOpenRequestA(Connect, "GET", utl, "HTTP/1.1", NULL, NULL, INTERNET_FLAG_RELOAD | INTERNET_COOKIE_THIRD_PARTY | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS, 0);
				if (hHttp != NULL) {
					HttpSendRequestA(hHttp, "Accept: */*", 11, NULL, 0);
					InternetCloseHandle(hHttp);
					InternetCloseHandle(hSession);
				}
			}
		}
	}

	void SendData(json data) {
		try
		{
			std::string datadump = data.dump();
			/*httplib::Client cli(RadarIP);
			auto Res = cli.Post("/syn", datadump, "application/json");
			if (Res->status == 200)*/
				return;
			//CONSOLE_INFO("%s|%s", RadarIP.c_str(), data.dump().c_str());
			//std::string Web = "/syn?id=123&json=" + data.dump();
			//static bool LogFile = false;
			//static int LogCount = 0;
			//if (OverlayEngine::WasKeyPressed(VK_INSERT))
			//{
			//	LogFile = true;
			//	LogCount = 0;
			//}
			//if (LogFile)
			//{
			//	if (LogCount < 2)
			//	{
			//		tfm::format(o, "%s\n", data.dump());
			//		LogCount++;
			//	}
			//	else
			//	{
			//		LogFile = false;
			//		o.close();
			//	}
			//}

			//GetHttp(ip.c_str(), port, Web.c_str());
		}
		catch (...)
		{

		}

	}

	//更新雷达线程
	void sdk::UpdateRadarToServerVEH()
	{
		static bool LastGameState = false;
		static bool PrintOnce = false;
		using framerate = std::chrono::duration<int, std::ratio<1, 60>>;
		auto tp = std::chrono::system_clock::now() + framerate{ 1 };
		auto start = std::chrono::high_resolution_clock::now();
		auto end = start + std::chrono::seconds(1);
		auto AllObject = ObjectManager::GetObjects();
		std::unique_ptr<json> data = std::make_unique<json>();
		for (auto& object : AllObject)
		{
			if (!object.second || !object.second->IsValid())
				continue;
			auto type = object.second->GetObjectType();
			if (object.second->IsValid())
			{
				m_cacheObjectsRadar[type].push_back(object.second);
			}
		}
		if (AllObject.size())
			AllObject.clear();
		while (std::chrono::high_resolution_clock::now() < end) {
			if (!sdk::InGame())
			{
				Sleep(2000);
				LastGameState = false;
#ifdef ENABLE_CONSOLE
				PrintOnce = false;
#endif
			}
			auto FloatWorldLocation = sdk::GetWorldLocation();
			int32_t X = *(int32_t*)((ptr_t)&FloatWorldLocation);
			int32_t Y = *(int32_t*)((ptr_t)&FloatWorldLocation + 4);

			FVector LerpSafetyZonePosition = sdk::GetLerpSafetyZonePosition();
			FVector LerpBlueZonePosition = sdk::GetLerpBlueZonePosition();
			FVector RedZonePosition = sdk::GetRedZonePosition();
			FVector BlackZonePosition = sdk::GetBlackZonePosition();

			LerpSafetyZonePosition.Z = sdk::GetLerpSafetyZoneRadius();
			LerpBlueZonePosition.Z = sdk::GetLerpBlueZoneRadius();
			RedZonePosition.Z = sdk::GetRedZoneRadius();
			BlackZonePosition.Z = sdk::GetBlackZoneRadius();

			FVector2D SafeZoneRadarPos = sdk::GetMapPos(LerpSafetyZonePosition, X, Y, true);
			FVector LerpSafetyZonePosition1 = FVector(LerpSafetyZonePosition.X + LerpSafetyZonePosition.Z, LerpSafetyZonePosition.Y, 0);
			FVector2D SafeZoneRadarPos1 = sdk::GetMapPos(LerpSafetyZonePosition1, X, Y, true);
			float SafeZoneRadius = abs(SafeZoneRadarPos1.X - SafeZoneRadarPos.X);

			FVector2D GasWarningRadarPos = sdk::GetMapPos(LerpBlueZonePosition, X, Y, true);
			FVector PoisonGasWarningPosition1 = FVector(LerpBlueZonePosition.X + LerpBlueZonePosition.Z, LerpBlueZonePosition.Y, 0);
			FVector2D GasWarningRadarPos1 = sdk::GetMapPos(PoisonGasWarningPosition1, X, Y, true);
			float GasWarningRadius = abs(GasWarningRadarPos1.X - GasWarningRadarPos.X);

			FVector2D RedZoneRadarPos = sdk::GetMapPos(RedZonePosition, X, Y, true);
			FVector RedZonePosition1 = FVector(RedZonePosition.X + RedZonePosition.Z, RedZonePosition.Y, 0);
			FVector2D RedZoneRadarPos1 = sdk::GetMapPos(RedZonePosition1, X, Y, true);
			float RedZoneRadius = abs(RedZoneRadarPos1.X - RedZoneRadarPos.X);

			FVector2D BlackZoneRadarPos = sdk::GetMapPos(BlackZonePosition, X, Y, true);
			FVector BlackZonePosition1 = FVector(BlackZonePosition.X + BlackZonePosition.Z, BlackZonePosition.Y, 0);
			FVector2D BlackZoneRadarPos1 = sdk::GetMapPos(BlackZonePosition1, X, Y, true);
			float BlackZoneRadius = abs(BlackZoneRadarPos1.X - BlackZoneRadarPos.X);


			(*data)["ingame"] = int(sdk::InGame());
			(*data)["map_name"] = sdk::GetWorldName();

			(*data)["safex"] = int(GasWarningRadarPos.X);
			(*data)["safey"] = int(GasWarningRadarPos.Y);
			(*data)["safer"] = int(GasWarningRadius);

			(*data)["poisonx"] = int(SafeZoneRadarPos.X);
			(*data)["poisony"] = int(SafeZoneRadarPos.Y);
			(*data)["poisonr"] = int(SafeZoneRadius);

			(*data)["redzonex"] = int(RedZoneRadarPos.X);
			(*data)["redzoney"] = int(RedZoneRadarPos.Y);
			(*data)["redzoner"] = int(RedZoneRadius);

			(*data)["blackzonex"] = int(RedZoneRadarPos.X);
			(*data)["blackzoney"] = int(RedZoneRadarPos.Y);
			(*data)["blackzoner"] = int(RedZoneRadius);

			(*data)["xx"] = json::array();
			(*data)["yy"] = json::array();
			(*data)["rr"] = json::array();
			(*data)["name"] = json::array();
			(*data)["team"] = json::array();
			(*data)["health"] = json::array();
			(*data)["friend"] = json::array();

			(*data)["otherxx"] = json::array();
			(*data)["otheryy"] = json::array();
			(*data)["othertype"] = json::array();

			bool LocalPlayerIsAdd = false;
			int LoopNum = 0;
			int LoopNumOther = 0;
			auto& LocalPlayer = sdk::GetPlayer();
			if (IsAddrValid(LocalPlayer))
			{
				for (auto& actor : m_cacheObjectsRadar)
				{
					if (actor.first == EPlayer)
					{
						for (auto& object : actor.second)
						{
							if (!object || !object->IsValid() || !IsAddrValid(object->GetPtr()))
								continue;
							auto player = dynamic_cast<ATslCharacter*>(object.get());
							if (!player || !player->IsValid())
								continue;
							if (player->GetPtr() == (void*)LocalPlayer && sdk::IsLocalPlayerAlive())
							{
								//auto ReadGuard = object->getReadLock();
								LocalPlayerIsAdd = TargetSelector::CacheRadarTarget(object, actor.first, *data, LoopNum, LoopNumOther, X, Y, true);
								//ReadGuard.reset();
								break;
							}
						}
					}
					if (LocalPlayerIsAdd)
						break;
				}
			}

			if (IsAddrValid(LocalPlayer) && !LastGameState && !LocalPlayerIsAdd && sdk::InGame())
				goto reloop;
			else if (!IsAddrValid(LocalPlayer) || (!LastGameState && LocalPlayerIsAdd))
				LastGameState = true;
			if (LastGameState)
			{
				for (auto& actor : m_cacheObjectsRadar)
				{
					for (auto& object : actor.second)
					{
						if (!object || !object->IsValid() || !IsAddrValid(object->GetPtr()))
							continue;
						if (actor.first == EPlayer)
						{
							auto player = dynamic_cast<ATslCharacter*>(object.get());
							if (!player || !player->IsValid())
								continue;
							if (player->GetPtr() == (void*)LocalPlayer)
								continue;
						}
						//auto ReadGuard = object->getReadLock();
						TargetSelector::CacheRadarTarget(object, actor.first, *data, LoopNum, LoopNumOther, X, Y, false);
						//ReadGuard.reset();
					}
				}
			}
			(*data)["num"] = int(LoopNum);
			(*data)["numother"] = int(LoopNumOther);
#ifdef ENABLE_CONSOLE
			if (LastGameState && !PrintOnce)
			{
				//CONSOLE_INFO("%s", data->dump().c_str());
				PrintOnce = true;
			}
#endif
			SendData(*data);
		reloop:
			data->clear();
			std::this_thread::sleep_until(tp);
			tp += framerate{ 1 };
		}
		if (!m_cacheObjectsRadar.empty())
		{
			//for (auto& objects : m_cacheObjectsRadar)
			//	objects.second.clear();
			m_cacheObjectsRadar.clear();
		}
	}

	DWORD sdk::UpdateRadarToServer(LPVOID Param)
	{
		do
		{
			try
			{
				if (Vars.Menu.PauseEveryThing)
				{
					Sleep(1000);
					continue;
				}
				if (!sdk::InGame())
					Sleep(2000);
				if (Vars.Misc.ShareRadar && Vars.Misc.ShareRadarState && RadarIP != "")
				{
					UpdateRadarToServerVEH();
				}
				else
				{
					Sleep(1000);
				}
			}
			catch (...)
			{
				continue;
			}
		} while (true);
		return 1;
	}

	bool sdk::Initialize()
	{
		bool result = UEDecryptBuilder::Initialize();
		if (result)
		{
			CONSOLE_INFO2(u8"已附加到游戏,等待游戏世界数据初始化完成!");
			do
			{
				auto encryptWorld = GetDMA().Read<uint64_t>(g_PatternData.UWorld);
				if (encryptWorld)
				{
					const auto tempWorld = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(encryptWorld);
					if (tempWorld)
					{
						break;
					}
				}
			} while (true);
			if (!DecryptHealthPoint)
			{
				DecryptHealthPoint = VirtualAlloc(nullptr, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				if (DecryptHealthPoint)
				{
					GetDMA().ReadFull(g_PatternData.offset_HealthDecryptFunction, (ptr_t)DecryptHealthPoint, 0x1000);
					CallDecryptHealth = (fnDecryptHealth)DecryptHealthPoint;
					Zydis mCp;
					auto fnHealth = (ptr_t)DecryptHealthPoint;
					while (true)
					{
						unsigned char data[MAX_DISASM_BUFFER];
						if (!Utils::IsSafeReadPtr((LPVOID)fnHealth, MAX_DISASM_BUFFER))
						{
							memcpy(data, (BYTE*)fnHealth, MAX_DISASM_BUFFER);
						}
						else
							break;
						if (!mCp.Disassemble(fnHealth, reinterpret_cast<const unsigned char*>(data)))
						{
							fnHealth++;
							continue;
						}
						if (mCp.OpCount() == 2 && mCp.GetId() == ZYDIS_MNEMONIC_MOV && mCp[0].type == ZYDIS_OPERAND_TYPE_REGISTER
							&& mCp[1].mem.base == ZYDIS_REGISTER_RIP && mCp[1].mem.disp.has_displacement)
						{
							memset((BYTE*)fnHealth, 0x90, mCp.Size());
						}
						else if (mCp.IsCall())
						{
							memset((BYTE*)fnHealth, 0x90, mCp.Size());
						}
						else if (mCp.IsRet())
						{
							ZeroMemory((BYTE*)(fnHealth + 1), ((ptr_t)DecryptHealthPoint + 0x1000) - (fnHealth + 1));
							break;
						}
						fnHealth += mCp.Size();
					}
					DWORD old;
					VirtualProtect(DecryptHealthPoint, 0x1000, PAGE_EXECUTE_READ, &old);
				}
			}
			result = NamesStore::Initialize() && ObjectsStore::Initialize();
			if (result)
			{
				mWorld = 0;
				mWeapon = 0;
				mWeaponNum1 = 0;
				mWeaponNum2 = 0;
				mMesh3P = 0;
				mMesh = 0;
				mControl = 0;
				m_MyHUD = 0;
				m_player = 0;
				m_WheeledVehicleplayer = 0;
				m_FloatingVehicleplayer = 0;
				m_Mortarplayer = 0;
				m_target = 0;
				m_InventoryEquipment.resize(3);
				m_CurvesKeysArray.resize(3);
				m_HumanBodyRepliedSkin.resize(50);
				m_WeaponName = "";
				ZeroMemory(m_InventoryEquipment.data(), sizeof(Inventory_Equipment) * 3);
				ZeroMemory(m_HumanBodyRepliedSkin.data(), sizeof(FEuqimentReplicatedSkinItem) * 50);
				if (!Vars.Menu.firstInit)
				{
					ObjectManager::Initialize();
				}
				int Count = 0;
				while (!ObjectManager::InitializeScatterHandle())
				{
					result = NamesStore::Initialize() && ObjectsStore::Initialize();
					Sleep(10);
					Count++;
					if (Count >= 3)
					{
						CONSOLE_INFO2(u8"初始化DMA分散数据失败,请重新开启辅助尝试！");
						Sleep(3000);
						//TerminateProcess(GetCurrentProcess(), 1);
					}
				}
				CONSOLE_INFO2(u8"初始化DMA分散数据");
			}
			else
				return false;
		}
		else
			return false;


		if (result && !Vars.Menu.firstInit)
		{
			OverlayEngine::OnUpdate += []()
			{
				if (sdk::InGame())
				{
					static ptr_t TickRun = 0;
					if (TickRun == 0)
						TickRun = GetTickCount64();
					if (GetTickCount64() - TickRun >= 200)
					{
						TickRun = GetTickCount64();
						if (m_usefulObjects.size())
						{
							//for (auto& objects : m_usefulObjects)
							//	objects.second.clear();
							m_usefulObjects.clear();
						}

						auto AllObject = ObjectManager::GetObjects();
						for (auto& object : AllObject)
						{
							if (!object.second || !*object.second || !object.second->IsValid() || !IsAddrValid(object.second->GetPtr()))
								continue;
							auto type = object.second->GetObjectType();
							if (object.second->IsValid() && type < EOther)
							{
								if (m_player && object.second->GetPtr() == (void*)m_player)
									continue;
								m_usefulObjects[type].push_back(object.second);
							}
						}
						if (AllObject.size())
							AllObject.clear();
					}
				}
			};

			OverlayEngine::OnPostPresent += []()
			{
				//for (auto& objects : m_usefulObjects)
				//	objects.second.clear();
				//m_usefulObjects.clear();
			};
		}
		return result;
	}

	void sdk::updateHandle()
	{
		CreateThread(NULL, NULL, UpdateDMADataThread, NULL, NULL, NULL);
		CreateThread(NULL, NULL, UpdateViewPort, NULL, NULL, NULL);
		CreateThread(NULL, NULL, UpdateSelfPlayer, NULL, NULL, NULL);
		CreateThread(NULL, NULL, UpdateKeys, NULL, NULL, NULL);
		CreateThread(NULL, NULL, UpdateRadarToServer, NULL, NULL, NULL);
		//CreateThread(NULL, NULL, UpdateObjectThread, NULL, NULL, NULL);
		const static int a = 0;
		do
		{
			try
			{
				sdk::OnUpdate();
			}
			catch (...)
			{
				continue;
			}
		} while (a == 0);
	}

	bool sdk::WorldToScreen2(const FVector& worldPos, FVector2D& screenPos)
	{
		FRotator Rotation = sdk::GetLocalRot(); // FRotator
		D3DMATRIX tempMatrix = sdkMath::Matrix(Rotation); // Matrix
		FVector vAxisX, vAxisY, vAxisZ;

		vAxisX = FVector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
		vAxisY = FVector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
		vAxisZ = FVector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

		FVector vDelta = worldPos - sdk::GetLocalPos();
		FVector vTransformed = FVector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

		if (vTransformed.Z < 1.f)
			vTransformed.Z = 1.f;

		float FovAngle = sdk::GetFov();

		FVector2D ScreenSize = Drawing::GetViewPort();
		FVector2D ScreenCenter = ScreenSize / 2.0f;

		screenPos.X = ScreenCenter.X + vTransformed.X * (ScreenCenter.X / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.Z;
		screenPos.Y = ScreenCenter.Y - vTransformed.Y * (ScreenCenter.X / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.Z;

		return screenPos.X > 0.f && screenPos.Y > 0.f && screenPos.X < ScreenSize.X && screenPos.Y < ScreenSize.Y;
	}

	bool sdk::WorldToScreen(const FVector& worldPos, FVector2D& screenPos)
	{
		FRotator Rotation = sdk::GetLocalRot(); // FRotator
		D3DMATRIX tempMatrix = sdkMath::Matrix(Rotation); // Matrix
		FVector vAxisX, vAxisY, vAxisZ;

		vAxisX = FVector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
		vAxisY = FVector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
		vAxisZ = FVector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

		FVector vDelta = worldPos - sdk::GetLocalPos();
		FVector vTransformed = FVector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

		if (vTransformed.Z < 1.f)
			return false;

		float FovAngle = sdk::GetFov();

		FVector2D ScreenSize = Drawing::GetViewPort();
		FVector2D ScreenCenter = ScreenSize / 2.0f;

		screenPos.X = ScreenCenter.X + vTransformed.X * (ScreenCenter.X / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.Z;
		screenPos.Y = ScreenCenter.Y - vTransformed.Y * (ScreenCenter.X / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.Z;

		return screenPos.X > 0.f && screenPos.Y > 0.f && screenPos.X < ScreenSize.X && screenPos.Y < ScreenSize.Y;
	}

	bool sdk::WorldToScreen(const FVector& worldPos, const FRotator& rotation, FVector2D& screenPos)
	{
		D3DMATRIX tempMatrix = sdkMath::Matrix(rotation); // Matrix

		FVector vAxisX, vAxisY, vAxisZ;

		vAxisX = FVector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
		vAxisY = FVector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
		vAxisZ = FVector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

		FVector vDelta = worldPos - sdk::GetLocalPos();
		FVector vTransformed = FVector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

		if (vTransformed.Z < 1.f)
		{
			screenPos.X = 0;
			screenPos.Y = 0;
			return false;
		}
		if (vTransformed.Z / 100.f > 1000.f)
		{
			screenPos.X = 0;
			screenPos.Y = 0;
			return false;
		}

		float FovAngle = sdk::GetFov();

		FVector2D ScreenSize = Drawing::GetViewPort();
		FVector2D ScreenCenter = ScreenSize / 2.0f;

		screenPos.X = ScreenCenter.X + vTransformed.X * (ScreenCenter.X / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.Z;
		screenPos.Y = ScreenCenter.Y - vTransformed.Y * (ScreenCenter.X / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.Z;

		return screenPos.X > 0.f && screenPos.Y > 0.f && screenPos.X < ScreenSize.X && screenPos.Y < ScreenSize.Y;
	}

	FVector2D sdk::WorldToScreen(const FVector& worldPos)
	{
		FVector2D screen;
		WorldToScreen(worldPos, screen);
		return screen;
	}

	FVector2D sdk::WorldToScreen(const FVector& worldPos, const FRotator& rotation)
	{
		FVector2D screen;
		WorldToScreen(worldPos, rotation, screen);
		return screen;
	}

	FVector2D sdk::GetMapPos(FVector ActorLocation, int X, int Y, bool Zone)
	{
		static std::map<hash_t, float> WorldMapSize = {
			{hash_("Tiger_Main"), 404886.f},//泰戈
			{hash_("Desert_Main"), 404886.f},//米拉玛
			{hash_("Kiki_Main"), 408000.f},//帝斯顿 Heaven_Main
			{hash_("Range_Main"), 101175.0f},//训练场
			{hash_("Summerland_Main"), 101175.f},//卡拉金 Summerland_Main
			{hash_("Baltic_Main"), 406567.f},//艾伦格
			{hash_("Savage_Main"), 202387.5f},//萨诺
			{hash_("DihorOtok_Main"), 405056.f},//维寒迪 雪地 DihorOtok_Main
			{hash_("Chimera_Main"), 153003.f},//帕拉莫 Chimera_Main
			{hash_("Neon_Main"), 404776.f},//荣都 Neon_Main
			{hash_("Heaven_Main"), 101175.f}//褐湾 Heaven_Main
		};
		float MapSize = WorldMapSize[hash_(sdk::m_WorldName)];

		//static float MainSize = 406372.f;
		//if (OverlayEngine::IsKeyDown(VK_UP))
		//{
		//	MainSize += 1.f;
		//	CONSOLE_INFO("%f", MainSize);

		//}
		//if (OverlayEngine::IsKeyDown(VK_DOWN))
		//{
		//	MainSize -= 1.f;
		//	CONSOLE_INFO("%f", MainSize);
		//}
		//MapSize = MainSize;

		FVector OriginLocation = FVector(X, Y, 0);
		float MapSizeFactored = MapSize;
		FVector WorldLocation;
		if (Zone)
			WorldLocation = ActorLocation;
		else
			WorldLocation = ActorLocation + OriginLocation;
		FVector WorldCenterLocation = { MapSize , MapSize , 0.0f };
		FVector RadarPos = WorldLocation - WorldCenterLocation;
		static FVector2D MapScreen = { 8130.0f,8130.0f };
		FVector2D RadarScreenCenterPos = FVector2D(MapScreen.X / 2, MapScreen.Y / 2);
		FVector2D RadarScreenPos = RadarScreenCenterPos + FVector2D{
		RadarPos.X / MapSizeFactored * RadarScreenCenterPos.X,
		RadarPos.Y / MapSizeFactored * RadarScreenCenterPos.Y
		};
		return FVector2D(RadarScreenPos.X, 8192.f - RadarScreenPos.Y);
	}

	int sdk::GetActorBoneBySex(ATslCharacter& player, int bone)
	{
		//bool isFemale = player->_Female;
		bool isFemale = true;
		switch (bone)
		{
		case Bone::forehead:
			return 15;
		case Bone::Head:
			return 6;
		case Bone::neck_01:
			return 5;
		case Bone::spine_03:
			return 4;
		case Bone::spine_02:
			return 3;
		case Bone::spine_01:
			return 2;
		case Bone::upperarm_l:
			return 88;
		case Bone::upperarm_r:
			return 115;
		case Bone::lowerarm_l:
			return 89;
		case Bone::lowerarm_r:
			return 116;
		case Bone::hand_l:
			return 90;
		case Bone::hand_r:
			return 117;
		case Bone::thumb_01_l:
			return 91;
		case Bone::thumb_01_r:
			return 118;
		case Bone::thigh_l:
			return isFemale ? 172 : 168;
		case Bone::thigh_r:
			return isFemale ? 178 : 174;
		case Bone::calf_l:
			return isFemale ? 173 : 169;
		case Bone::calf_r:
			return 179;
		case Bone::foot_l:
			return isFemale ? 174 : 170;
		case Bone::foot_r:
			return isFemale ? 180 : 176;
		case Bone::ik_foot_root:
			return isFemale ? 204 : 200;
		case Bone::ik_foot_l:
			return isFemale ? 205 : 201;
		case Bone::ik_foot_r:
			return isFemale ? 206 : 202;
		default:
			break;
		}
		return Bone::forehead;
	}

	bool sdk::GetBoneTable1(ATslCharacter& player, VMMDLL_SCATTER_HANDLE scatterHandle)
	{
		std::unique_ptr<USkeletalMeshComponent> mesh(new USkeletalMeshComponent(player->_Mesh));
		std::unique_ptr <USceneComponent> Root(new USceneComponent(player->_RootComponent));
		if (!mesh->IsValid())
			goto ExitTable1;
		if (!Root->IsValid())
			goto ExitTable1;
		GetDMA().queueScatterReadEx(scatterHandle, (ptr_t)mesh->GetPtr() + g_PatternData.offset_ComponentSpaceTransforms, &player->_ComponentSpaceTransformsArray, sizeof(TArray<FTransform>));
		GetDMA().queueScatterReadEx(scatterHandle, (ptr_t)mesh->GetPtr() + g_PatternData.offset_ComponentToWorld, &player->_ComponentToWorld, sizeof(FTransform));
	ExitTable1:
		return true;
	}

	bool sdk::GetBoneTable2(ATslCharacter& player, VMMDLL_SCATTER_HANDLE scatterHandle, bool Visible)
	{
		std::unique_ptr<USkeletalMeshComponent> mesh(new USkeletalMeshComponent(player->_Mesh));
		std::unique_ptr <USceneComponent> Root(new USceneComponent(player->_RootComponent));
		TArray<FTransform> ComponentSpaceTransformsArray = player->_ComponentSpaceTransformsArray;
		if (!mesh->IsValid())
			goto ExitTable2;
		if (!Root->IsValid())
			goto ExitTable2;
		if (!ComponentSpaceTransformsArray.Data)
			goto ExitTable2;



		static std::vector<int> boneVec =
		{
			 Bone::forehead, Bone::Head, Bone::neck_01 ,
			 Bone::spine_03, Bone::spine_02, Bone::spine_01 ,
			 Bone::upperarm_l, Bone::upperarm_r, Bone::lowerarm_l,
			 Bone::lowerarm_r, Bone::hand_l, Bone::hand_r,
			 Bone::thumb_01_l, Bone::thumb_01_r, Bone::thigh_l,
			 Bone::thigh_r,Bone::calf_l,Bone::calf_r,
			 Bone::foot_l,Bone::foot_r,Bone::ik_foot_root,
			 Bone::ik_foot_l,Bone::ik_foot_r
		};

		static std::vector<int> NotAimboneVec =
		{
			Bone::hand_l, Bone::lowerarm_l, Bone::upperarm_l,
			Bone::hand_r, Bone::lowerarm_r, Bone::upperarm_r,
			Bone::foot_l, Bone::calf_l, Bone::thigh_l,
			Bone::foot_r, Bone::calf_r, Bone::thigh_r,
			Bone::spine_01, Bone::neck_01, Bone::forehead
		};

		if (player->_BoneTransFormTable.size() != boneVec.size() && player->_BoneTransFormTable.size() < 100)
			player->_BoneTransFormTable.resize(boneVec.size());
		for (int i = 0; i < boneVec.size(); i++)
		{
			bool bSkip = true;
			if (player->GetPtr() != sdk::GetLockTarget())
			{
				if (Visible)
				{
					for (auto NotAim : NotAimboneVec)
					{
						if (NotAim == i)
						{
							bSkip = false;
							break;
						}
					}
				}
				else
				{
					if (i == Bone::forehead)
					{
						bSkip = false;
					}
				}
			}
			else
				bSkip = false;
			if (bSkip)
				continue;
			GetDMA().queueScatterReadEx(scatterHandle, (ptr_t)ComponentSpaceTransformsArray.Data + GetActorBoneBySex(player, boneVec[i]) * sizeof(FTransform),
				(void*)((ptr_t)player->_BoneTransFormTable.data() + boneVec[i] * sizeof(FTransform)), sizeof(FTransform));
		}
	ExitTable2:
		return true;
	}

	bool sdk::GetBoneTable3(ATslCharacter& player, VMMDLL_SCATTER_HANDLE scatterHandle, bool Visible)
	{
		std::unique_ptr<USkeletalMeshComponent> mesh(new USkeletalMeshComponent(player->_Mesh));
		std::unique_ptr <USceneComponent> Root(new USceneComponent(player->_RootComponent));
		TArray<FTransform> ComponentSpaceTransformsArray = player->_ComponentSpaceTransformsArray;
		if (!mesh->IsValid())
			goto ExitTable3;
		if (!Root->IsValid())
			goto ExitTable3;
		if (!ComponentSpaceTransformsArray.Data)
			goto ExitTable3;

		if (player->_Player_BoneTable.size() != 23)
		{
			player->_Player_BoneTable.resize(23);
		}

		static std::vector<int> boneVec =
		{
			 Bone::forehead, Bone::Head, Bone::neck_01 ,
			 Bone::spine_03, Bone::spine_02, Bone::spine_01 ,
			 Bone::upperarm_l, Bone::upperarm_r, Bone::lowerarm_l,
			 Bone::lowerarm_r, Bone::hand_l, Bone::hand_r,
			 Bone::thumb_01_l, Bone::thumb_01_r, Bone::thigh_l,
			 Bone::thigh_r,Bone::calf_l,Bone::calf_r,Bone::foot_l,
			 Bone::foot_r,Bone::ik_foot_root,Bone::ik_foot_l,Bone::ik_foot_r
		};

		if (boneVec.size() != player->_BoneTransFormTable.size())
			return false;

		for (int i = 0; i < boneVec.size(); i++)
		{
			if (!Visible)
			{
				if (boneVec[i] != Bone::forehead)
					continue;
			}
			FTransform ComponentSpaceTransformsArray = player->_BoneTransFormTable[i];
			FVector RetPos = (ComponentSpaceTransformsArray.ToMatrixWithScale() * player->_ComponentToWorld.ToMatrixWithScale()).GetOrigin();
			player->_Player_BoneTable[i] = RetPos;
		}
	ExitTable3:
		return true;
	}

	FVector sdk::GetBonePosition(ATslCharacter& player, int boneIndex)
	{
		try
		{
			if (player->_Player_BoneTable.empty() || player->_Player_BoneTable.size() != 23)
			{
				return FVector(0, 0, 0);
			}
			return player->_Player_BoneTable[boneIndex];
		}
		catch (...)
		{

		}
		return FVector(0, 0, 0);
	}

	bool sdk::GetVehicldeBoneTable1(ATslCharacter& player, VMMDLL_SCATTER_HANDLE scatterHandle)
	{
		std::unique_ptr<USkeletalMeshComponent> mesh(new USkeletalMeshComponent(player->_VehicleMesh));
		if (!mesh->IsValid())
			return false;
		GetDMA().queueScatterReadEx(scatterHandle, (ptr_t)mesh->GetPtr() + g_PatternData.offset_ComponentSpaceTransforms, &player->_Vehicle_ComponentSpaceTransformsArray, sizeof(TArray<FTransform>));
		GetDMA().queueScatterReadEx(scatterHandle, (ptr_t)mesh->GetPtr() + g_PatternData.offset_ComponentToWorld, &player->_Vehicle_ComponentToWorld, sizeof(FTransform));
		return true;
	}

	bool sdk::GetVehicldeBoneTable2(ATslCharacter& player, VMMDLL_SCATTER_HANDLE scatterHandle, std::vector<int> NeedVechile)
	{
		std::unique_ptr<USkeletalMeshComponent> mesh(new USkeletalMeshComponent(player->_VehicleMesh));
		if (!mesh->IsValid())
			return false;
		TArray<FTransform> ComponentSpaceTransformsArray = player->_Vehicle_ComponentSpaceTransformsArray;
		if (!ComponentSpaceTransformsArray.Data)
			return false;

		for (int i = 0; i < NeedVechile.size(); i++)
		{
			GetDMA().queueScatterReadEx(scatterHandle, (ptr_t)ComponentSpaceTransformsArray.Data + NeedVechile[i] * sizeof(FTransform),
				(void*)((ptr_t)player->_Vehicle_BoneTransFormTable.data() + NeedVechile[i] * sizeof(FTransform)), sizeof(FTransform));
		}
		return true;
	}

	bool sdk::GetVehicldeBoneTable3(ATslCharacter& player, VMMDLL_SCATTER_HANDLE scatterHandle, std::vector<int> NeedVechile)
	{
		std::unique_ptr<USkeletalMeshComponent> mesh(new USkeletalMeshComponent(player->_VehicleMesh));
		if (!mesh->IsValid())
			return false;
		TArray<FTransform> ComponentSpaceTransformsArray = player->_Vehicle_ComponentSpaceTransformsArray;
		if (!ComponentSpaceTransformsArray.Data)
			return false;
		for (int i = 0; i < NeedVechile.size(); i++)
		{
			FTransform ComponentSpaceTransformsArray = player->_Vehicle_BoneTransFormTable[NeedVechile[i]];
			FVector RetPos = (ComponentSpaceTransformsArray.ToMatrixWithScale() * player->_Vehicle_ComponentToWorld.ToMatrixWithScale()).GetOrigin();
			player->_Vehicle_BoneTable[NeedVechile[i]] = RetPos;
		}
		return true;
	}

	bool sdk::GetBoneScreenPosition(ATslCharacter& player, FVector2D& result, int boneIndex)
	{
		return WorldToScreen(GetBonePosition(player, boneIndex), result);
	}

	void sdk::GetComponentRotation(FQuat BoxVec1, FQuat BoxVec2, FQuat& out1)
	{
		//__m128 v2; // xmm1
		//__int64 v3; // r10
		//__m128* v4; // r9
		//__m128 v5; // xmm1
		//int v6; // eax
		//FVector v7; // rax
		//__int64 result; // rax
		//FVector v9; // [rsp+20h] [rbp-28h] BYREF
		//__m128 v10; // [rsp+30h] [rbp-18h] BYREF

		//v3 = (__int64)&out1;
		//v4 = a1;
		//v5.m128_f32[0] = BoxVec1.X;
		//v5.m128_f32[1] = BoxVec1.Y;
		//v5.m128_f32[2] = BoxVec1.Z;
		//v5.m128_f32[3] = BoxVec1.W;

		//v2.m128_f32[0] = BoxVec2.X;
		//v2.m128_f32[1] = BoxVec2.Y;
		//v2.m128_f32[2] = BoxVec2.Z;
		//v2.m128_f32[3] = BoxVec2.W;
		//v6 = _mm_movemask_ps(_mm_cmpneq_ps(v2, v5));
		//v10 = v5;
		//if (v6)
		//{
		//	a1[41] = v5;
		//	v7 = FQuat(v5.m128_f32[0], v5.m128_f32[1], v5.m128_f32[2], v5.m128_f32[3]).Rotate(&v10, v9);
		//	v4[42].m128_u64[0] = *(QWORD*)v7;
		//	v4[42].m128_i32[2] = *(DWORD*)(v7 + 8);
		//}
		//result = v4[42].m128_u32[2];
		//*(QWORD*)v3 = v4[42].m128_u64[0];
		//*(DWORD*)(v3 + 8) = result;
	}

	bool sdk::PlayerVisibleCheck(ATslCharacter& player)
	{
		if (!player)
			return false;
		return (player->_LastRenderTimeOnScreen != 0.f && player->_LastRenderTime != 0.f &&
			player->_LastSubmitTime != 0.f) && abs(sdk::GetLastRenderTimeOnScreen() - player->_LastRenderTimeOnScreen) <= 0.1f &&
			player->_LastRenderTimeOnScreen + 0.1f >= player->_LastSubmitTime;
	}

	bool sdk::IsDMR()
	{
		return Aimbot::isDMR();
	}

	bool sdk::IsShotGun()
	{
		return Aimbot::isShotGun();
	}

	bool sdk::IsSR()
	{
		return Aimbot::isSR();
	}

	bool sdk::IsGrenade()
	{
		return Aimbot::isGrenade();
	}

	void sdk::ClearAimTarget()
	{
		TargetSelector::aimLock = false;
		sdk::SetAimTarget(nullptr);
	}

	void sdk::AimToTarget()
	{
		Aimbot::AimToTarget();
	}
}