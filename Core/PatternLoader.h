#pragma once
#include <windows.h>
typedef uintptr_t ptr_t;
namespace IronMan::Core
{
	struct PatternData
	{

		ptr_t	ModuleHandle = 0;
		ptr_t	ptr_GetMousePostion = 0;
		ptr_t	SteamClient64 = 0;
		ptr_t	DecryptAddress = 0;
		ptr_t	retaddrofLineTrace = 0;
		ptr_t	InputAxisProperties = 0;
		ptr_t	GameTimePtr = 0;
		ptr_t	offset_PhysScene = 0;
		ptr_t	offset_PxScene = 0;

		ptr_t	offset_bIsCanted = 0;
		ptr_t	offset_bSpawnBulletFromBarrel = 0;
		ptr_t   ComponentLocationOffset = 0;
		
		ptr_t	offset_UFunction_Func = 0;
		ptr_t   offset_ProcessEvent = 0;
		ptr_t	fnProcessEvent = 0;
		ptr_t	offset_ComponentSpaceTransforms = 0;
		ptr_t	offset_ComponentToWorld = 0;
		ptr_t	fnGetNames = 0;
		ptr_t	GNames = 0;
		ptr_t	UObjectArray = 0;
		ptr_t	UObjectArrayNum = 0;
		ptr_t   ElementsPerChunk = 0;

		ptr_t	UWrold_Ptr = 0;
		ptr_t	UWorld = 0;
		ptr_t	offset_OwningGameInstance = 0;
		ptr_t	offset_LocalPlayers = 0;
		ptr_t	offset_ViewportClient = 0;
		ptr_t	offset_PlayerController = 0;
		ptr_t	offsetAActors = 0;
		ptr_t	offsetPersistentLevel = 0;

		ptr_t	offset_UWorld = 0;
		ptr_t	vtindex_GetWorld = 38;//44 0F 28 C1 0F 84 ? ? ? ? 48 8B 01    
		
		ptr_t	decryptUWorld = 0;
		ptr_t	decryptOwningGameInstance = 0;
		ptr_t	decryptLocalPlayers = 0;// and ViewportClient decrypt
		ptr_t	decryptViewportClient = 0;
		ptr_t	decryptPlayerController = 0;
		ptr_t	decryptPersistentLevel = 0;

		ptr_t	encryAActors = 0;
		ptr_t	decryptAActors = 0;
		ptr_t   decryptUObject = 0;
		ptr_t   decryptUObjectIndex = 0;
		ptr_t   decryptUObjectArray = 0;

		ptr_t UObjectSort[5] = { 0 };
		ptr_t offset_UObjectFlags = 0;
		ptr_t offset_UObjectIndex = 0;
		ptr_t offset_UObjectNameNumber = 0;
		ptr_t offset_UObjectNameComparisonIndex = 0;
		ptr_t offset_UObjectOuter = 0;
		ptr_t offset_UObjectClass = 0;
		ptr_t UObjectSize = 0;

		ptr_t offset_UStruct_SuperStruct = 0;
		ptr_t offset_UStruct_Children = 0;
		ptr_t offset_UStruct_PropertiesSize = 0;

		ptr_t fnGetOffset = 0;

		ptr_t offset_UClassBaseChainArray;	//FClassBaseChain   IsA
		ptr_t offset_UClassNumClassBasesInChainMinusOne;


		ptr_t offset_UFunction_FunctionFlags = 0;

		ptr_t	SteamResizeBuffer = 0;
		ptr_t	GameDrawing = 0;
		ptr_t	GameDrawing2 = 0;

		ptr_t	UnrealEngineRender = 0;
		ptr_t	offset_JmpR14 = 0;
		ptr_t	offset_JmpR15 = 0;
		ptr_t	EmptyMemory = 0;
		ptr_t	fnFindTextInLocalizationTable = 0;
		ptr_t	fnGetComponentsByClass = 0;
		ptr_t	offset_OwnedComponents = 0;
		ptr_t	offset_OwnedActorComponent = 0;
		//ptr_t	offset_WeaponIndex = 0;

		ptr_t	ExtFindTextInLocalizationTableRcx = 0;
		ptr_t	ExtFindTextInLocalizationTableCall = 0;

		ptr_t	fnLineTraceSingle = 0;
		ptr_t	fnLineTraceSingleByChannel = 0;
		ptr_t	pDefaultQueryParam = 0;
		ptr_t	pDefaultResponseParam = 0;
		ptr_t	fnFCollisionQueryParamsConstructible = 0;
		ptr_t	fnFCollisionQueryParamsDestructor = 0;

		ptr_t	fnGetTslSettingsInstance = 0;
		ptr_t	offset_TslSettings = 0;

		ptr_t	offset_ATslCharacterUnk1 = 0;
		ptr_t	offset_ATslCharacterUnk2 = 0;

		ptr_t	offset_WorldPhysicsScene = 0;
		ptr_t   fnGetPhysXScene = 0;
		ptr_t   fnraycastIndex = 0;
		ptr_t   fnSetHitResultFromShapeAndFaceIndex = 0;


		ptr_t	offset_GetOffsets = 0;
		ptr_t	offset_Is_Scope = 0;
		ptr_t	offset_WorldLocation = 0;
		ptr_t	offset_JudgeMap = 0;
		ptr_t	offset_MapBaseAddr = 0;

		ptr_t	offset_FireState = 0;
		ptr_t	offset_bThirdPerson = 0;

		ptr_t	offset_BackPackState = 0;
		ptr_t	offset_Language = 0;
		ptr_t	offset_ScopeCamera_Breath = 0;

		ptr_t	offset_GameState = 0;
		ptr_t	addrof_Detection = 0;
		ptr_t	OffsetVersion = 0;

		ptr_t	CharacterState = 0;
		ptr_t	offset_UFont_LegacyFontSize = 0;
		ptr_t	offset_UCanvas_SizeX = 0;
		ptr_t	offset_UCanvas_SizeY = 0;
		ptr_t	offset_UCurveVector_FloatCurves = 0;
		ptr_t	offset_ULvel_WorldSettings = 0;
		ptr_t	offset_SkeletalMeshSocket_SocketName = 0;
		ptr_t	offset_SkeletalMeshSocket_BoneName = 0;
		ptr_t	offset_SkeletalMeshSocket_RelativeLocation = 0;
		ptr_t	offset_SkeletalMeshSocket_RelativeRotation = 0;
		ptr_t	offset_SkeletalMeshSocket_RelativeScale = 0;
		ptr_t	offset_SkeletalMeshSocket_bForceAlwaysAnimated = 0;
		ptr_t	offset_USkeleton_Sockets = 0;
		ptr_t	offset_USkeletalMesh_Skeleton = 0;
		ptr_t	offset_USkeletalMesh_Sockets = 0;
		ptr_t	offset_USkeletalMesh_RefSkeleton = 0;
		ptr_t	offset_Actor_RootComponent = 0;
		ptr_t	offset_Actor_Owner = 0;
		ptr_t	offset_Actor_Instigator = 0;
		ptr_t	offset_Actor_CustomTimeDilation = 0;
		ptr_t	offset_Actor_LinearVelocity = 0;
		ptr_t	offset_ATslGameState_NumAliveTeams = 0;
		ptr_t	offset_ATslGameState_LerpSafetyZonePosition = 0;
		ptr_t	offset_ATslGameState_LerpSafetyZoneRadius = 0;
		ptr_t	offset_ATslGameState_LerpBlueZonePosition = 0;
		ptr_t	offset_ATslGameState_LerpBlueZoneRadius = 0;
		ptr_t	offset_ATslGameState_RedZonePosition = 0;
		ptr_t	offset_ATslGameState_RedZoneRadius = 0;
		ptr_t	offset_ATslGameState_BlackZonePosition = 0;
		ptr_t	offset_ATslGameState_BlackZoneRadius = 0;
		ptr_t	offset_ATslGameState_SafetyZonePosition = 0;
		ptr_t	offset_ATslGameState_SafetyZoneRadius = 0;

		ptr_t	offset_AController_Pawn = 0;
		ptr_t	offset_AController_StateName = 0;
		ptr_t	offset_AController_ControlRotation = 0;
		ptr_t	offset_APawn_PlayerState = 0;
		ptr_t	offset_APawn_Controller = 0;
		ptr_t	offset_APawn_BaseEyeHeight = 0;
		ptr_t	offset_APlayerCameraManager_PCOwner = 0;
		ptr_t	offset_APlayerCameraManager_ViewTarget = 0;
		ptr_t	offset_APlayerCameraManager_CameraCache = 0;
		ptr_t	offset_MinimalViewInfo_FOV = 0;
		ptr_t	offset_MinimalViewInfo_Location = 0;
		ptr_t	offset_MinimalViewInfo_Rotation = 0;
		ptr_t	offset_TViewTarget_POV = 0;
		ptr_t	offset_CameraCacheEntry_Timestamp = 0;
		ptr_t	offset_CameraCacheEntry_POV = 0;
		ptr_t	offset_AHUD_BlockInputWidgetList = 0;
		ptr_t	offset_AHUD_WidgetStateMap = 0;
		ptr_t	offset_AHUD_DebugCanvas = 0;
		ptr_t	offset_AHUD_Canvas = 0;
		ptr_t	offset_UActorComponent_bRegistered = 0;
		ptr_t	offset_UTslVehicleCommonComponent_FuelMax = 0;
		ptr_t	offset_UTslVehicleCommonComponent_Fuel = 0;
		ptr_t	offset_UTslVehicleCommonComponent_HealthMax = 0;
		ptr_t	offset_UTslVehicleCommonComponent_Health = 0;
		ptr_t	offset_USceneComponent_ComponentVelocity = 0;
		ptr_t	offset_USceneComponent_RelativeLocation = 0;
		ptr_t	offset_USceneComponent_RelativeRotation = 0;
		ptr_t	offset_USceneComponent_AttachParent = 0;
		ptr_t	offset_UPrimitiveComponent_LastRenderTimeOnScreen = 0;
		ptr_t	offset_UPrimitiveComponent_LastRenderTime = 0;
		ptr_t	offset_UPrimitiveComponent_LastSubmitTime = 0;
		ptr_t	offset_UPrimitiveComponent_bRenderCustomDepth = 0;
		ptr_t	offset_UPrimitiveComponent_BodyInstance = 0;
		ptr_t	offset_USkinnedMeshComponent_SkeletalMesh = 0;
		ptr_t	offset_USkinnedMeshComponent_MasterPoseComponent = 0;
		ptr_t	offset_USkinnedMeshComponent_MasterBoneMap = 0;
		ptr_t	offset_USkinnedMeshComponent_CurrentEditableComponentTransforms = 0;
		ptr_t	offset_USkinnedMeshComponent_CurrentReadComponentTransforms = 0;
		ptr_t	offset_UTslAnimInstance_ControlRotation_CP = 0;
		ptr_t	offset_UTslAnimInstance_RecoilADSRotation_CP = 0;
		ptr_t	offset_UTslAnimInstance_ScopingAlpha_CP = 0;
		ptr_t	offset_UTslAnimInstance_LeanRightAlpha_CP = 0;
		ptr_t	offset_UTslAnimInstance_LeanLeftAlpha_CP = 0;
		ptr_t	offset_UTslAnimInstance_bIsScoping_CP = 0;
		ptr_t	offset_UTslAnimInstance_bIsTPPAiming = 0;
		ptr_t	offset_UTslAnimInstance_bIsAiming_CP = 0;
		ptr_t	offset_UTslAnimInstance_bIsReloading_CP = 0;
		ptr_t	offset_USkeletalMeshComponent_AnimScriptInstance = 0;
		ptr_t	offset_UStaticMeshSocket_SocketName = 0;
		ptr_t	offset_UStaticMeshSocket_RelativeLocation = 0;
		ptr_t	offset_UStaticMeshSocket_RelativeRotation = 0;
		ptr_t	offset_UStaticMeshSocket_RelativeScale = 0;
		ptr_t	offset_UStaticMesh_Sockets = 0;
		ptr_t	offset_UStaticMeshComponent_StaticMesh = 0;
		ptr_t	offset_UWeaponMeshComponent_AttachedStaticComponentMap = 0;
		ptr_t	offset_UWeaponMeshComponent_ReplicatedSkinParam = 0;
		ptr_t	offset_UCharacterMovementComponent_Velocity = 0;
		ptr_t	offset_UCharacterMovementComponent_JumpZVelocity = 0;
		ptr_t	offset_UCharacterMovementComponent_WalkableFloorZ = 0;
		ptr_t	offset_UCharacterMovementComponent_LastUpdateLocation = 0;
		ptr_t	offset_UCharacterMovementComponent_LastUpdateRotation = 0;
		ptr_t	offset_UCharacterMovementComponent_LastUpdateVelocity = 0;
		ptr_t	offset_UCharacterMovementComponent_CurrentFloor = 0;
		ptr_t	offset_UCharacterMovementComponent_bit4 = 0;
		ptr_t	offset_ACharacter_CharacterMovement = 0;
		ptr_t	offset_ACharacter_Mesh = 0;
		ptr_t	offset_UVehicleRiderComponent_SeatIndex = 0;
		ptr_t	offset_UVehicleRiderComponent_LastVehiclePawn = 0;
		ptr_t	offset_UWeaponProcessorComponent_EquippedWeapons = 0;
		ptr_t	offset_UWeaponProcessorComponent_WeaponIndex = 0;
		ptr_t	offset_ATslCharacterBase_Health = 0;
		ptr_t	offset_HealthFlag1 = 0;
		ptr_t	offset_HealthFlag2 = 0;
		ptr_t	offset_HealthFlag3 = 0;
		ptr_t	offset_HealthIndex1 = 0;
		ptr_t	offset_HealthIndex2 = 0;
		ptr_t	offset_HealthXorKeys = 0;
		ptr_t	offset_HealthDecryptFunction = 0;
		ptr_t	offset_ATslCharacterBase_HealthMax = 0;
		ptr_t	offset_ATslCharacterBase_WeaponProcessor = 0;
		ptr_t	offset_ATslCharacter_Team = 0;
		ptr_t	offset_ATslCharacter_InventoryFacade = 0;
		ptr_t	offset_ATslCharacter_VehicleRiderComponent = 0;
		ptr_t	offset_ATslCharacter_GroggyHealth = 0;
		ptr_t	offset_ATslCharacter_GroggyHealthMax = 0;
		ptr_t	offset_ATslCharacter_AimOffsets = 0;
		ptr_t	offset_ATslCharacter_LastAimOffsets = 0;
		ptr_t	offset_ATslCharacter_ScopingCamera = 0;
		ptr_t	offset_ATslCharacter_SpectatedCount = 0;
		ptr_t	offset_ATslCharacter_LastTeamNum = 0;
		ptr_t	offset_ATslCharacter_CharacterName = 0;
		ptr_t	offset_APlayerController_AcknowledgedPawn = 0;
		ptr_t	offset_APlayerController_PlayerCameraManager = 0;
		ptr_t	offset_APlayerController_MyHUD = 0;
		ptr_t	offset_APlayerController_MapSig = 0;
		ptr_t	offset_APlayerController_RotationInput = 0;
		ptr_t	offset_APlayerController_InputYawScale = 0;
		ptr_t	offset_APlayerController_InputPitchScale = 0;
		ptr_t	offset_APlayerController_InputRollScale = 0;
		ptr_t	offset_ATslPlayerController_MaxValidPing = 0;
		ptr_t	offset_ATslPlayerController_PlayerInput = 0;
		ptr_t	offset_APlayerState_PlayerName = 0;
		ptr_t	offset_APlayerState_Ping = 0;
		ptr_t	offset_ATslPlayerState_PlayerStatistics = 0;
		ptr_t	offset_ATslPlayerState_PubgIdData = 0;
		ptr_t	offset_ATslPlayerState_DamageDealtOnEnemy = 0;
		ptr_t	offset_ATslPlayerState_PartnerLevel = 0;
		ptr_t	offset_AWheeledVehicle_Mesh = 0;
		ptr_t	offset_AWheeledVehicle_VehicleMovement = 0;
		ptr_t	offset_ATslFloatingVehicle_Mesh = 0;
		ptr_t	offset_ATslFloatingVehicle_VehicleCommonComponent = 0;
		ptr_t	offset_ATslWheeledVehicle_VehicleCommonComponent = 0;
		ptr_t	offset_ADroppedItem_Item = 0;
		ptr_t	offset_UItem_StackCount = 0;
		ptr_t	offset_UItem_ItemTableRowPtr = 0;
		ptr_t	offset_ItemTableRowBase_ItemName = 0;
		ptr_t	offset_ItemTableRowBase_ItemCategory = 0;
		ptr_t	offset_ItemTableRowBase_ItemDetailedName = 0;
		ptr_t	offset_ItemTableRowBase_ItemDescription = 0;
		ptr_t	offset_ItemTableRowBase_ItemID = 0;
		ptr_t	offset_UDroppedItemInteractionComponent_Item = 0;
		ptr_t	offset_AInventory_Items = 0;
		ptr_t	offset_UEquipableItem_EquipSlotID = 0;
		ptr_t	offset_UEquipableItem_DurabilityMax = 0;
		ptr_t	offset_UEquipableItem_Durability = 0;
		ptr_t	offset_UEquipableItem_ReplicatedSkinItem = 0;
		ptr_t	offset_AEquipment_Items = 0;
		ptr_t	offset_AItemPackage_Items = 0;
		ptr_t	offset_ATslWeapon_Mesh3P = 0;
		ptr_t	offset_ATslWeapon_MuzzleAttachPoint = 0;
		ptr_t	offset_ATslWeapon_FiringAttachPoint = 0;
		ptr_t	offset_ATslWeapon_AttachedItem = 0;
		ptr_t	offset_ATslWeapon_Gun_ScopingAttachPoint = 0;
		ptr_t	offset_ATslWeapon_Gun_ScopingFocusPoint = 0;
		ptr_t	offset_ATslWeapon_Gun_CurrentAmmoData = 0;
		ptr_t	offset_ATslWeapon_Gun_CurrentZeroLevel = 0;
		ptr_t	offset_ATslWeapon_Trajectory_TrajectoryGravityZ = 0;
		ptr_t	offset_ATslWeapon_Trajectory_WeaponTrajectoryData = 0;
		ptr_t	offset_WeaponTrajectoryData_TrajectoryConfig = 0;
		ptr_t	offset_ATslProjectile_ProjectileConfig = 0;
		ptr_t	offset_ATslProjectile_CollisionComp = 0;
		ptr_t	offset_ATslProjectile_ExplosionDelay = 0;
		ptr_t	offset_ATslProjectile_TimeTillExplosion = 0;
		ptr_t	offset_UTslSettings_BallisticDragScale = 0;
		ptr_t	offset_UTslSettings_BallisticDropScale = 0;
		ptr_t	offset_UBodySetup_WalkableSlopeOverride = 0;
		ptr_t	offset_UWidget_Visibility = 0;
		ptr_t	offset_UWidget_Slot = 0;
		ptr_t	offset_UCanvasPanelSlot_LayoutData = 0;
		ptr_t	offset_UTslNewWorldMapWidget_MapGrid = 0;
		ptr_t	offset_TslTrainingWorldMapWidget_TrainingMapGrid = 0;
		ptr_t	offset_UMinimapCanvasWidget_SelectMinimapSizeIndex = 0;
		ptr_t	offset_UMinimapCanvasWidget_MinimapSizeOffset = 0;
		ptr_t	offset_UTslGameUserSettings_SelectMiniMapIndex = 0;
		ptr_t	offset_AInventoryFacade_Inventory = 0;
		ptr_t	offset_AInventoryFacade_Equipment = 0;

	};

	extern PatternData g_PatternData;

	namespace PatternLoader
	{
		ptr_t GetDataPattern(ptr_t address, int32_t dataOperandOffset, int32_t dataInstructionSize);
		void FitOffset(std::string vars, ptr_t* pOut, std::vector<std::string> netvars);
		void SetupOffset(std::vector<std::string> netvars);
		void FixOffset();
		void ReFixOffset();
	}


}