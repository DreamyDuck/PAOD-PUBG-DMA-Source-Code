#include "Include/WinHeaders.hpp"
#include "Class.h"
#include "../ObjectsStore.h"
#include "Core/PatternLoader.h"
#include "../SDK.h"
namespace IronMan::Core::SDK
{

	UClass UFont::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.Font"_hash>();
		return c;
	}

	UClass UCanvas::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.Canvas"_hash>();
		return c;
	}
	/*
	 * UField
	 */
	UField::UField(void* object)
		:UObject(object)
		, Next(INVALID_POINTER)
	{

	}
	UField UField::GetNext()
	{
		if (Next == INVALID_POINTER && IsValid())
		{
			Next = GetDMA().Read<void*>((ptr_t)GetPtr() + g_PatternData.UObjectSize + 0x8);
		}
		return UField(Next);
	}

	/*
	 * UStruct
	 */
	UStruct::UStruct(void* object)
		: UField(object),
		SuperStruct(INVALID_POINTER),
		Children(INVALID_POINTER),
		PropertiesSize(-1)
	{
	}
	UStruct UStruct::GetSuperStruct()
	{
		if (SuperStruct == INVALID_POINTER && IsValid())
		{
			SuperStruct = GetDMA().Read<void*>((ptr_t)GetPtr() + g_PatternData.offset_UStruct_SuperStruct);
		}
		return UStruct(SuperStruct);
	}
	UField  UStruct::GetChildren()
	{
		if (Children == INVALID_POINTER && IsValid())
		{
			Children = GetDMA().Read<void*>((ptr_t)GetPtr() + g_PatternData.offset_UStruct_Children);
		}
		return UField(Children);
	}
	int32_t UStruct::GetPropertiesSize()
	{
		if (PropertiesSize < 0 && IsValid())
		{
			PropertiesSize = GetDMA().Read<int32_t>((ptr_t)GetPtr() + g_PatternData.offset_UStruct_PropertiesSize);
		}
		return PropertiesSize;
	}

	/*
	 * UProperty
	 */
	UProperty::UProperty(void* object)
		:UField(object),
		ArrayDim(-1),
		ElementSize(-1),
		PropertyFlags(-1),
		PropertySize(-1),
		Offset(-1)
	{
	}
	int32_t  UProperty::GetArrayDim()
	{
		if (ArrayDim < 0 && IsValid())
		{
			ArrayDim = GetDMA().Read<int32_t>((ptr_t)GetPtr() + g_PatternData.UObjectSize + 0x10);
		}
		return ArrayDim;
	}
	int32_t  UProperty::GetElementSize()
	{
		if (ElementSize < 0 && IsValid())
		{
			ElementSize = GetDMA().Read<int32_t>((ptr_t)GetPtr() + g_PatternData.UObjectSize + 0x14);
		}
		return ElementSize;
	}
	uint64_t UProperty::GetPropertyFlags()
	{
		if (PropertyFlags < 0 && IsValid())
		{
			PropertyFlags = GetDMA().Read<int32_t>((ptr_t)GetPtr() + g_PatternData.UObjectSize + 0x18);
		}
		return PropertyFlags;
	}
	int32_t  UProperty::GetPropertySize()
	{
		if (PropertySize < 0 && IsValid())
		{
			PropertySize = GetDMA().Read<int32_t>((ptr_t)GetPtr() + g_PatternData.UObjectSize + 0x20);
		}
		return PropertySize;
	}
	int32_t  UProperty::GetOffset()
	{
		if (Offset < 0 && IsValid())
		{
			return (int32_t)Offset;
			//Offset = Game.memory().Read<int32_t>((ptr_t)GetPtr() + g_PatternData.UObjectSize + g_PatternData.offset_GetOffsets).result(0);//0x30
		}
		return Offset;
	}

	int32_t  UProperty::GetOffset_Debug(ptr_t SetOffset)
	{
		return 0;
		//return Game.memory().Read<int32_t>((ptr_t)GetPtr() + g_PatternData.UObjectSize + SetOffset).result(0);//0x30
	}

	FVector AActor::GetActorLocation() const
	{
		return RootComponent ? RootComponent->GetLocation() : FVector::ZeroVector;
	}

	FTransform AActor::ActorToWorld() const
	{
		return (RootComponent ? RootComponent->GetComponentTransform() : FTransform::Identity);
	}

	/*
	 * AActor
	 */
	UClass AActor::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.Actor"_hash>();
		return c;
	}

	bool ATslCharacter::GetUnkBool()
	{
		if (!IsValid())
			return false;
		bool v1 = GetDMA().Read<char>((ptr_t)GetPtr() + g_PatternData.offset_ATslCharacterUnk1);
		auto v2 = GetDMA().Read<ptr_t>((ptr_t)GetPtr() + g_PatternData.offset_ATslCharacterUnk2);
		if (!v1 && v2)
		{
			return (GetDMA().Read<uint32_t>(v2) >> 6) & 1;
		}
		else if (v2)
		{
			return (GetDMA().Read<uint32_t>(v2) >> 15) & 1 ^ 1;
		}
		return true;
	}


	ATslWeapon ATslCharacter::GetWeaponByIndex(int NumOfWeapon)
	{
		if (!this->IsValid())
			return nullptr;
		auto WeaponProcessor = this->WeaponProcessor;
		if (WeaponProcessor->IsValid())
		{
			ATslWeapon RetWeapon = WeaponProcessor->GetEquippedWeapons()[NumOfWeapon];
			if (RetWeapon->IsValid())
				return RetWeapon;
		}

		return nullptr;
	}

	FRotator ATslCharacter::GetControlRotation_CP()
	{
		if (this->IsValid())
		{
			auto mesh = this->Mesh;
			if (mesh->IsValid())
			{
				auto tslAnim = mesh->AnimScriptInstance->SafeCast<UTslAnimInstance>();
				if (tslAnim->IsValid())
				{
					return tslAnim->ControlRotation_CP;
				}
			}
		}
		return FRotator();
	}

	FRotator ATslCharacter::GetRecoilADSRotation_CP()
	{
		if (this->IsValid())
		{
			auto mesh = this->Mesh;
			if (mesh->IsValid())
			{
				auto tslAnim = mesh->AnimScriptInstance->SafeCast<UTslAnimInstance>();
				if (tslAnim->IsValid())
				{
					return tslAnim->RecoilADSRotation_CP;
				}
			}
		}
		return FRotator();
	}

	float ATslCharacter::GetScopingAlpha_CP()
	{
		if (this->IsValid())
		{
			auto mesh = this->Mesh;
			if (mesh->IsValid())
			{
				auto tslAnim = mesh->AnimScriptInstance->SafeCast<UTslAnimInstance>();
				if (tslAnim->IsValid())
				{
					return tslAnim->ScopingAlpha_CP;
				}
			}
		}
		return 0.f;
	}

	bool ATslCharacter::GetIsScoping_CP()
	{
		if (this->IsValid())
		{
			auto mesh = this->Mesh;
			if (mesh->IsValid())
			{
				auto tslAnim = mesh->AnimScriptInstance->SafeCast<UTslAnimInstance>();
				if (tslAnim->IsValid())
				{
					return tslAnim->bIsScoping_CP;
				}
			}
		}
		return false;
	}

	bool ATslCharacter::GetIsAiming_CP()
	{
		if (this->IsValid())
		{
			auto mesh = this->Mesh;
			if (mesh->IsValid())
			{
				auto tslAnim = mesh->AnimScriptInstance->SafeCast<UTslAnimInstance>();
				if (tslAnim->IsValid())
				{
					return tslAnim->bIsAiming_CP;
				}
			}
		}
		return false;
	}

	bool ATslCharacter::GetIsReloading_CP()
	{
		if (this->IsValid())
		{
			auto mesh = this->Mesh;
			if (mesh->IsValid())
			{
				auto tslAnim = mesh->AnimScriptInstance->SafeCast<UTslAnimInstance>();
				if (tslAnim->IsValid())
				{
					return tslAnim->bIsReloading_CP;
				}
			}
		}
		return false;
	}

	FRotator ATslCharacter::GetScopingCameraBreath()
	{

		if (this->IsValid())
		{
			ptr_t ScopingCamera = (ptr_t)this->ScopingCamera;
			if (ScopingCamera)
			{
				FRotator Breath = GetDMA().Read<FRotator>((ptr_t)ScopingCamera + g_PatternData.offset_ScopeCamera_Breath);
				return Breath;
			}
		}
		return FRotator();
	}

	float ATslCharacter::GetLeanLeftAlpha_CP()
	{
		if (this->IsValid())
		{
			auto mesh = this->Mesh;
			if (mesh->IsValid())
			{
				auto tslAnim = mesh->AnimScriptInstance->SafeCast<UTslAnimInstance>();
				if (tslAnim->IsValid())
				{
					return tslAnim->LeanLeftAlpha_CP;
				}
			}
		}
		return 0.f;
	}

	float ATslCharacter::GetLeanRightAlpha_CP()
	{
		if (this->IsValid())
		{
			auto mesh = this->Mesh;
			if (mesh->IsValid())
			{
				auto tslAnim = mesh->AnimScriptInstance->SafeCast<UTslAnimInstance>();
				if (tslAnim->IsValid())
				{
					return tslAnim->LeanRightAlpha_CP;
				}
			}
		}
		return 0.f;
	}


	/*
	 * ATslCharacter
	 */
	UClass ATslCharacter::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslCharacter"_hash>();
		return c;
	}

	/*
	 * ATslCharacterBase
	 */
	UClass ATslCharacterBase::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslCharacterBase"_hash>();
		return c;
	}

	/*
	 * ADummyAICharacter
	 */
	UClass ADummyAICharacter::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.DummyAICharacter"_hash>();
		return c;
	}

	/*
	 * ATslNpcCharacter
	 */
	UClass ATslNpcCharacter::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslNpcCharacter"_hash>();
		return c;
	}

	/*
	 * AZDF2AICharacter
	 */
	UClass AZDF2AICharacter::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.ZDF2AICharacter"_hash>();
		return c;
	}

	/*
	 * AZDF2AICharacter
	 */
	UClass ATslSLBAICharacter::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslSLBAICharacter"_hash>();
		return c;
	}

	/*
	 * UWolrd
	 */
	UClass UWorld::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.World"_hash>();
		return c;
	}

	/*
	 * ULevel
	 */
	UClass ULevel::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.Level"_hash>();
		return c;
	}

	/*
	 * ULocalPlayer
	 */
	UClass ULocalPlayer::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.LocalPlayer"_hash>();
		return c;
	}

	TArray<UEEncryptedObjectProperty<class ULocalPlayer, DecryptFunc::General>> UGameInstance::GetLocalPlayers()
	{
		if (IsValid())
		{
			return GetDMA().Read<TArray<UEEncryptedObjectProperty<class ULocalPlayer, DecryptFunc::General>>>((ptr_t)GetPtr() + g_PatternData.offset_LocalPlayers);
		}
		return TArray<UEEncryptedObjectProperty<class ULocalPlayer, DecryptFunc::General>>();
	}

	/*
	* AWorldSettings
	*/
	UClass AWorldSettings::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.WorldSettings"_hash>();
		return c;
	}

	UClass AGameMode::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.GameMode"_hash>();
		return c;
	}

	/*
	 * ATslGameMode
	 */
	UClass ATslGameMode::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslGameMode"_hash>();
		return c;
	}

	/*
	 * AGameStateBase
	 */
	UClass AGameState::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.GameState"_hash>();
		return c;
	}

	/*
	 * ATslGameState
	 */
	UClass ATslGameState::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslGameState"_hash>();
		return c;
	}

	/*
	 * UGameInstance
	 */
	UClass UGameInstance::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.GameInstance"_hash>();
		return c;
	}

	/*
	 * UGameViewportClient
	 */
	UClass UGameViewportClient::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.GameViewportClient"_hash>();
		return c;
	}

	void APlayerController::GetPlayerViewPoint(FVector& out_Location, FRotator& out_Rotation)
	{
		if (PlayerCameraManager->IsValid() && PlayerCameraManager->GetCameraCache_Timestamp() > 0.f) // Whether camera was updated at least once)
		{
			PlayerCameraManager->GetCameraViewPoint(out_Location, out_Rotation);
		}
		else
		{

		}
	}

	/*
	 * APlayerController
	 */
	UClass APlayerController::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.PlayerController"_hash>();
		return c;
	}

	/*
	* APawn
	*/
	UClass APawn::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.Pawn"_hash>();
		return c;
	}

	/*
	* AInfo
	*/
	UClass AInfo::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.Info"_hash>();
		return c;
	}

	/*
	 * AController
	 */
	UClass AController::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.Controller"_hash>();
		return c;
	}


	/*
	* UFunction
	*/
	UClass UFunction::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class CoreUObject.Function"_hash>();
		return c;
	}

	FVector USceneComponent::GetLocation()
	{
		if (!this->IsValid())
			return FVector();

		return GetDMA().Read<FVector>((ptr_t)this->GetPtr() + g_PatternData.ComponentLocationOffset);
	}

	FTransform USceneComponent::GetSocketTransform(FName SocketName, ERelativeTransformSpace TransformSpace)
	{
		switch (TransformSpace)
		{
		case RTS_Actor:
		{
			return GetComponentTransform().GetRelativeTransform(this->Owner->GetTransform());
			break;
		}
		case RTS_Component:
		case RTS_ParentBoneSpace:
		{
			return FTransform::Identity;
		}
		default:
		{
			return GetComponentTransform();
		}
		}
	}

	FVector USceneComponent::GetSocketLocation(FName SocketName)
	{
		if (this && this->IsValid())
			return this->GetComponentTransform().GetTranslation();
		else
			return FVector();
		//return GetSocketTransform(SocketName, RTS_World).GetTranslation();
	}

	FRotator USceneComponent::GetSocketRotation(FName SocketName)
	{
		if (this && this->IsValid())
			return this->GetComponentTransform().GetRotation().Rotator();
		else
			return FRotator();
		//return GetSocketTransform(SocketName, RTS_World).GetRotation().Rotator();
	}

	FQuat USceneComponent::GetSocketQuaternion(FName SocketName)
	{
		if (this && this->IsValid())
			return this->GetComponentTransform().GetRotation();
		else
			return FQuat();
		//return GetSocketTransform(SocketName, RTS_World).GetRotation();
	}

	UClass UTslVehicleCommonComponent::StaticClass()
	{
#ifndef ObjectNameHash
		auto c = ObjectsStore().FindClass<"Class TslGame.TslVehicleCommonComponent"_hash>();
#else
		auto c = ObjectsStore().FindClass<"Class TslGame.*dc96c7f0f3"_hash>();
#endif
		return c;
	}

	/*
	 * USceneComponent
	 */
	UClass USceneComponent::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.SceneComponent"_hash>();
		return c;
	}

	/*
	 * UActorComponent
	 */
	UClass UActorComponent::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.ActorComponent"_hash>();
		return c;
	}

	/*
	 * APlayerCameraManager
	 */
	UClass APlayerCameraManager::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.PlayerCameraManager"_hash>();
		return c;
	}

	UClass AHUD::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.HUD"_hash>();
		return c;
	}

	ptr_t APlayerCameraManager::GetViewTarget_POV_FOV()
	{
		if (!IsValid()) return 0;
		return GetViewTarget_POV() + GetPOV_FOVOffset();
	}

	ptr_t APlayerCameraManager::GetViewTarget_POV_Location()
	{
		if (!IsValid()) return 0;
		return GetViewTarget_POV() + GetPOV_LocationOffset();
	}

	ptr_t APlayerCameraManager::GetViewTarget_POV_Rotation()
	{
		if (!IsValid()) return 0;
		return GetViewTarget_POV() + GetPOV_RotationOffset();
	}

	float APlayerCameraManager::GetCameraCache_Timestamp()
	{
		static auto TimestampOffset = -1;
		if (TimestampOffset == -1)
#ifndef ObjectNameHash
			//48 8B ? ? 48 8B ? ? ? 00 00 ? 8B ? 48 81 C2 ? ? 00 00 E8 + 0xE
			//48 8B ? ? ? 00 00 ? 8B ? 48 81 C2 ? ? 00 00 E8 ? ? ? ? 48 83 ? ? ? ? ? 00 ? 8B + 0
			TimestampOffset = GETOFFSET("FloatProperty Engine.CameraCacheEntry.Timestamp");
#else
			TimestampOffset = g_PatternData.offset_CameraCacheEntry_Timestamp;
#endif
		return GetDMA().Read<float>((ptr_t)CameraCache + TimestampOffset);
	}

	float APlayerCameraManager::GetCameraCache_POV_FOV()
	{
		if (!IsValid()) return 0.0f;
		return GetDMA().Read<float>(GetCameraCache_POV() + GetPOV_FOVOffset());
	}

	FVector APlayerCameraManager::GetCameraCache_POV_Location()
	{
		if (!IsValid()) return FVector();
		return GetDMA().Read<FVector>(GetCameraCache_POV() + GetPOV_LocationOffset());
	}

	FRotator APlayerCameraManager::GetCameraCache_POV_Rotation()
	{
		if (!IsValid()) return FRotator();
		return GetDMA().Read<FRotator>(GetCameraCache_POV() + GetPOV_RotationOffset());
	}

	void APlayerCameraManager::GetCameraViewPoint(FVector& OutCamLoc, FRotator& OutCamRot)
	{
		OutCamLoc = this->GetCameraCache_POV_Location(); //CameraCache.POV.Location;
		OutCamRot = this->GetCameraCache_POV_Rotation(); //CameraCache.POV.Rotation;
	}

	ptr_t APlayerCameraManager::GetViewTarget_POV()
	{
		static auto POVOffset = -1;
		if (POVOffset == -1)
#ifndef ObjectNameHash
			POVOffset = GETOFFSET("StructProperty Engine.TViewTarget.POV");
#else
			POVOffset = g_PatternData.offset_TViewTarget_POV;
#endif

		return (ptr_t)ViewTarget + POVOffset;
	}

	ptr_t APlayerCameraManager::GetCameraCache_POV()
	{
		static auto POVOffset = -1;
		if (POVOffset == -1)
#ifndef ObjectNameHash
			//FF 90 30 01 00 00 F3 0F 10 ? ? ? 00 00 48 8D ? ? ? 00 00 0f 2e ? ? ? 00 00 + 0xE] - FF 90 30 01 00 00 F3 0F 10 ? ? ? 00 00 48 8D ? ? ? 00 00 0f 2e ? ? ? 00 00 + 0x6]
			//TslGame.exe+421402 - movss xmm0,[rbx+00001020]
			//TslGame.exe + 42140A - lea rdi, [rbx + 00001030]
			// 0x1030 - 0x1020
			POVOffset = GETOFFSET("StructProperty Engine.CameraCacheEntry.POV");
#else
			POVOffset = g_PatternData.offset_CameraCacheEntry_POV;
#endif

		return (ptr_t)CameraCache + POVOffset;
	}
	uint32_t APlayerCameraManager::GetPOV_FOVOffset()
	{
		static auto FOVOffset = -1;
		if (FOVOffset == -1)
#ifndef ObjectNameHash
			//89 81 ? ? ? ? 48 8B 01 F3 0F 11 44 24 + 0
			//83 78 08 00 74 03 48 8B 18 F3 0F 10 87 + 9
			//F3 0F 10 81 ? ? ? ? 0F ? ? ? ? ? ? 77 08 F3 0F 10 81 + 0x11
			FOVOffset = GETOFFSET("FloatProperty Engine.MinimalViewInfo.FOV");
#else
			FOVOffset = g_PatternData.offset_MinimalViewInfo_FOV;
#endif

		return FOVOffset;
	}
	uint32_t APlayerCameraManager::GetPOV_LocationOffset()
	{
		static auto LocationOffset = -1;
		if (LocationOffset == -1)
#ifndef ObjectNameHash
			//F3 0F 10 ? ? ? 00 00 ? 8D ? ? ? 00 00 F3 0F 10 ? ? ? 00 00 ? 8D ? 24 ? ? 8B ? E8 + 8
			//48 8D ? ? ? 00 00 48 8D ? ? E8 ? ? ? ? ? 8B ? ? 8D ? ? ? 00 00 ? 8D ? ? E8 ? ? ? ? 90 83 + 0x13
			//0F 84 ? ? ? ? 8B ? ? ? 00 00 ? 8B ? F2 0F 10 ? ? ? 00 00 89 ? ? ? 8B + 0xF
			LocationOffset = GETOFFSET("StructProperty Engine.MinimalViewInfo.Location");
#else
			LocationOffset = g_PatternData.offset_MinimalViewInfo_Location;
#endif

		return LocationOffset;
	}
	uint32_t APlayerCameraManager::GetPOV_RotationOffset()
	{
		static auto RotationOffset = -1;
		if (RotationOffset == -1)
#ifndef ObjectNameHash
			//F3 0F 10 ? ? ? 00 00 ? 8D ? ? ? 00 00 F3 0F 10 ? ? ? 00 00 ? 8D ? 24 ? ? 8B ? E8 + 0xF
			//48 8D ? ? ? 00 00 48 8D ? ? E8 ? ? ? ? ? 8B ? ? 8D ? ? ? 00 00 ? 8D ? ? E8 ? ? ? ? 90 83 + 0
			//F2 0F 10 ? ? ? 00 00 ? 8B ? F2 0F 11 ? 24 ? F3 0F 10 ? 24 ? 0F 28 + 0
			RotationOffset = GETOFFSET("StructProperty Engine.MinimalViewInfo.Rotation");
#else
			RotationOffset = g_PatternData.offset_MinimalViewInfo_Rotation;
#endif

		return RotationOffset;
	}

	/*
	 * ACharacter
	 */
	UClass ACharacter::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.Character"_hash>();
		return c;
	}

	FWalkableSlopeOverride UPrimitiveComponent::GetWalkableSlopeOverride()
	{
		//bOverrideWalkableSlopeOnInstance
		//F7 ? ? 00 00 00 01 ? 8B ? 0F 85 ? ? ? ? 33 ? 39 ? ? ? 00 00 0F
		//8A 08 88 0B 8B 48 04 89 4B 04 8B 48 08 89 4B 08 8B 40 0C
		auto pBodyInstance = this->BodyInstance;
		if (pBodyInstance.bOverrideWalkableSlopeOnInstance || !pBodyInstance.BodySetup.IsValid())
		{
			return pBodyInstance.WalkableSlopeOverride;
		}
		else
		{
			return pBodyInstance.BodySetup->WalkableSlopeOverride;
		};
	}


	UClass UPrimitiveComponent::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.PrimitiveComponent"_hash>();
		return c;
	}

	/*
	 * UMeshComponent
	 */
	UClass UMeshComponent::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.MeshComponent"_hash>();
		return c;
	}

	TArray<FTransform> USkinnedMeshComponent::GetComponentSpaceTransformsArray(int32_t idx)
	{
		static ptr_t componentSpaceTransformsArrayOffset = 0;
		if (componentSpaceTransformsArrayOffset == 0)
		{
			componentSpaceTransformsArrayOffset = g_PatternData.offset_USkinnedMeshComponent_MasterPoseComponent + 0x8;
		}

		return GetDMA().Read<TArray<FTransform>>((ptr_t)GetPtr() + componentSpaceTransformsArrayOffset + idx * sizeof(TArray<FTransform>));
	}

	FTransform USkinnedMeshComponent::GetBoneTransform(int32_t BoneIdx, const FTransform& LocalToWorld)
	{
		// Handle case of use a MasterPoseComponent - get bone matrix from there.
		USkinnedMeshComponent MasterPoseComponentInst = MasterPoseComponent.Get();
		if (MasterPoseComponentInst)
		{
			if (!MasterPoseComponentInst->IsRegistered())
			{
				// We aren't going to get anything valid from the master pose if it
				// isn't valid so for now return identity
				return FTransform::Identity;
			}
			if (BoneIdx < (int32_t)MasterBoneMap.Num())
			{
				int32_t ParentBoneIndex = MasterBoneMap[BoneIdx];

				// If ParentBoneIndex is valid, grab matrix from MasterPoseComponent.
				if (ParentBoneIndex != INDEX_NONE &&
					ParentBoneIndex < MasterPoseComponentInst->GetNumComponentSpaceTransforms())
				{
					return MasterPoseComponentInst->GetComponentSpaceTransforms()[ParentBoneIndex] * LocalToWorld;
				}
				else
				{
					// UE_LOG(LogSkinnedMeshComp, Warning, TEXT("GetBoneTransform : ParentBoneIndex(%d) out of range of MasterPoseComponent->SpaceBases for %s"), BoneIdx, *this->GetFName().ToString());
					return FTransform::Identity;
				}
			}
			else
			{
				// UE_LOG(LogSkinnedMeshComp, Warning, TEXT("GetBoneTransform : BoneIndex(%d) out of range of MasterBoneMap for %s"), BoneIdx, *this->GetFName().ToString());
				return FTransform::Identity;
			}
		}
		else
		{
			const int32_t NumTransforms = GetNumComponentSpaceTransforms();
			if (NumTransforms > 0 && BoneIdx < NumTransforms)
			{
				return GetComponentSpaceTransforms()[BoneIdx] * LocalToWorld;
			}
			else
			{
				//UE_LOG(LogSkinnedMeshComp, Warning, TEXT("GetBoneTransform : BoneIndex(%d) out of range of SpaceBases for %s (%s)"), BoneIdx, *GetPathName(), SkeletalMesh ? *SkeletalMesh->GetFullName() : TEXT("NULL"));
				return FTransform::Identity;
			}
		}
	}

	FTransform USkinnedMeshComponent::GetBoneTransform(int32_t BoneIdx)
	{
		if (!IsRegistered())
		{
			// if not registered, we don't have SpaceBases yet. 
			// also GetComponentTransform() isn't set yet (They're set from relativelocation, relativerotation, relativescale)
			return FTransform::Identity;
		}

		return GetBoneTransform(BoneIdx, GetComponentTransform());
	}

	FTransform USkinnedMeshComponent::GetSocketTransform2(int32_t BoneIndex, FTransform GunFT, FTransform SocketLocalTransform)
	{
		FTransform OutSocketTransform = GunFT;
		if (BoneIndex != INDEX_NONE)
		{
			FTransform BoneTransform = GetBoneTransform(BoneIndex);
			OutSocketTransform = SocketLocalTransform * BoneTransform;
			return OutSocketTransform;
		}
		return OutSocketTransform;
	}


	FTransform USkinnedMeshComponent::GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace)
	{
		FTransform OutSocketTransform = this->GetComponentToWorld();

		if (InSocketName != NAME_None)
		{
			USkeletalMeshSocket Socket = GetSocketByName(InSocketName);
			// apply the socket transform first if we find a matching socket
			if (Socket)
			{
				FTransform SocketLocalTransform = Socket->GetSocketLocalTransform();

				if (TransformSpace == RTS_ParentBoneSpace)
				{
					//we are done just return now
					return SocketLocalTransform;
				}

				int32_t BoneIndex = GetBoneIndex(Socket->BoneName);
				if (BoneIndex != INDEX_NONE)
				{
					FTransform BoneTransform = GetBoneTransform(BoneIndex);
					OutSocketTransform = SocketLocalTransform * BoneTransform;
				}
			}
			else
			{
				int32_t BoneIndex = GetBoneIndex(InSocketName);
				if (BoneIndex != INDEX_NONE)
				{
					OutSocketTransform = GetBoneTransform(BoneIndex);

					if (TransformSpace == RTS_ParentBoneSpace)
					{
						FName ParentBone = GetParentBone(InSocketName);
						int32_t ParentIndex = GetBoneIndex(ParentBone);
						if (ParentIndex != INDEX_NONE)
						{
							return OutSocketTransform.GetRelativeTransform(GetBoneTransform(ParentIndex));
						}
						return OutSocketTransform.GetRelativeTransform(GetComponentTransform());
					}
				}
			}

		}
		return OutSocketTransform;
	}

	int32_t USkinnedMeshComponent::GetBoneIndex(FName BoneName)
	{
		int32_t BoneIndex = INDEX_NONE;
		if (BoneName != NAME_None && SkeletalMesh)
		{
			BoneIndex = SkeletalMesh->RefSkeleton.FindBoneIndex(BoneName);
		}

		return BoneIndex;
	}

	USkeletalMeshSocket USkinnedMeshComponent::GetSocketByName(FName InSocketName)
	{
		USkeletalMeshSocket Socket = nullptr;

		if (SkeletalMesh)
		{
			Socket = SkeletalMesh->FindSocket(InSocketName);
		}
		else
		{
			/*UE_LOG(LogSkinnedMeshComp, Warning, TEXT("GetSocketByName(%s): No SkeletalMesh for Component(%s) Actor(%s)"),
				*InSocketName.ToString(), *GetName(), *GetNameSafe(GetOuter()));*/
		}
		return Socket;
	}

	FName USkinnedMeshComponent::GetParentBone(FName BoneName)
	{
		FName Result = NAME_None;

		int32_t BoneIndex = GetBoneIndex(BoneName);
		if ((BoneIndex != INDEX_NONE) && (BoneIndex > 0)) // This checks that this bone is not the root (ie no parent), and that BoneIndex != INDEX_NONE (ie bone name was found)
		{
			Result = SkeletalMesh->RefSkeleton.GetBoneName(SkeletalMesh->RefSkeleton.GetParentIndex(BoneIndex));
		}
		return Result;
	}

	FName USkinnedMeshComponent::GetBoneNameByIndex(int32_t BoneIndex)
	{
		auto Result = SkeletalMesh->RefSkeleton.GetBoneName(BoneIndex);
		return Result;
	}

	/*
	 * USkinnedMeshComponent
	 */
	UClass USkinnedMeshComponent::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.SkinnedMeshComponent"_hash>();
		return c;
	}

	/*
	 * USkeletalMeshComponent
	 */
	UClass USkeletalMeshComponent::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.SkeletalMeshComponent"_hash>();
		return c;
	}

	/*
	 * APlayerState
	 */
	UClass APlayerState::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.PlayerState"_hash>();
		return c;
	}

	/*
	 * ATslPlayerState
	 */
	UClass ATslPlayerState::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslPlayerState"_hash>();
		return c;
	}

	/*
	 * AWheeledVehicle
	 */
	UClass AWheeledVehicle::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class PhysXVehicles.WheeledVehicle"_hash>();
		return c;
	}

	/*
	 * ATslWheeledVehicle
	 */
	UClass ATslWheeledVehicle::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslWheeledVehicle"_hash>();
		return c;
	}

	/*
	* ATslWheeledVehicle
	*/
	UClass ATslFlyingVehilce::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslFlyingVehicle"_hash>();
		return c;
	}
	/*
	 * ADroppedItemGroup
	 */
	UClass ADroppedItemGroup::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.DroppedItemGroup"_hash>();
		return c;
	}

	/*
	 * USceneInteractionComponent
	 */
	UClass USceneInteractionComponent::StaticClass()
	{
#ifndef ObjectNameHash
		auto c = ObjectsStore().FindClass<"Class TslGame.SceneInteractionComponent"_hash>();
#else
		//.AllowInteractBy
		auto c = ObjectsStore().FindClass<"Class TslGame.*1e516b0170"_hash>();
#endif
		return c;
	}

	/*
	 * UDroppedItemInteractionComponent
	 */
	UClass UDroppedItemInteractionComponent::StaticClass()
	{
#ifndef ObjectNameHash
		auto c = ObjectsStore().FindClass<"Class TslGame.DroppedItemInteractionComponent"_hash>();
#else
		auto c = ObjectsStore().FindClass<"Class TslGame.*ae84a20c65"_hash>();
#endif
		return c;
	}

	/*
	 * UItem
	 */
	UClass UItem::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.Item"_hash>();
		return c;
	}

	uint32_t UItem::GetItemNameOffset()
	{
		static auto ItemNameOffset = -1;
		if (ItemNameOffset == -1)
#ifndef ObjectNameHash
			ItemNameOffset = GETOFFSET("TextProperty TslGame.ItemTableRowBase.ItemName");
#else
			//ToolTipIconSizeRatio
			ItemNameOffset = g_PatternData.offset_ItemTableRowBase_ItemName;
#endif

		return ItemNameOffset;
	}

	FText UItem::GetItemName()
	{
		if (!IsValid())
			return FText();
		auto ItemTableRow = GetDMA().Read<ptr_t>((ptr_t)this->GetPtr() + g_PatternData.offset_UItem_ItemTableRowPtr);
		if (!ItemTableRow)
			return FText();
		return GetDMA().Read<FText>(ItemTableRow + GetItemNameOffset());
	}

	uint32_t UItem::GetItemCategoryOffset()
	{
		static auto ItemCategoryOffset = -1;
		if (ItemCategoryOffset == -1)
#ifndef ObjectNameHash
			ItemCategoryOffset = GETOFFSET("TextProperty TslGame.ItemTableRowBase.ItemCategory");
#else
			ItemCategoryOffset = g_PatternData.offset_ItemTableRowBase_ItemCategory;
#endif

		return ItemCategoryOffset;
	}

	FText UItem::GetItemCategory()
	{
		if (!IsValid())
			return FText();
		auto ItemTableRow = GetDMA().Read<ptr_t>((ptr_t)this->GetPtr() + g_PatternData.offset_UItem_ItemTableRowPtr);
		if (!ItemTableRow)
			return FText();
		return GetDMA().Read<FText>(ItemTableRow + GetItemCategoryOffset());
	}

	uint32_t UItem::GetItemDetailedNameOffset()
	{
		static auto ItemDetailedNameOffset = -1;
		if (ItemDetailedNameOffset == -1)
#ifndef ObjectNameHash
			ItemDetailedNameOffset = GETOFFSET("TextProperty TslGame.ItemTableRowBase.ItemDetailedName");
#else
			ItemDetailedNameOffset = g_PatternData.offset_ItemTableRowBase_ItemDetailedName;
#endif

		return ItemDetailedNameOffset;
	}

	FText UItem::GetItemDetailedName()
	{
		if (!IsValid())
			return FText();
		auto ItemTableRow = GetDMA().Read<ptr_t>((ptr_t)this->GetPtr() + g_PatternData.offset_UItem_ItemTableRowPtr);
		if (!ItemTableRow)
			return FText();
		return GetDMA().Read<FText>(ItemTableRow + GetItemDetailedNameOffset());
	}

	uint32_t UItem::GetItemDescriptionOffset()
	{
		static auto ItemDescriptionOffset = -1;
		if (ItemDescriptionOffset == -1)
#ifndef ObjectNameHash
			ItemDescriptionOffset = GETOFFSET("TextProperty TslGame.ItemTableRowBase.ItemDescription");
#else
			ItemDescriptionOffset = g_PatternData.offset_ItemTableRowBase_ItemDescription;
#endif

		return ItemDescriptionOffset;
	}

	FText UItem::GetItemDescription()
	{
		if (!IsValid())
			return FText();
		auto ItemTableRow = GetDMA().Read<ptr_t>((ptr_t)this->GetPtr() + g_PatternData.offset_UItem_ItemTableRowPtr);
		if (!ItemTableRow)
			return FText();
		return GetDMA().Read<FText>(ItemTableRow + GetItemDescriptionOffset());
	}

	uint32_t UItem::GetItemIDOffset()
	{
		static auto ItemIDOffset = -1;
		if (ItemIDOffset == -1)
#ifndef ObjectNameHash
			ItemIDOffset = GETOFFSET("NameProperty TslGame.ItemTableRowBase.ItemID");
#else
			ItemIDOffset = g_PatternData.offset_ItemTableRowBase_ItemID;
#endif

		return ItemIDOffset;
	}

	FName UItem::GetItemID()
	{
		if (!IsValid())
			return FName();
		auto ItemTableRow = GetDMA().Read<ptr_t>((ptr_t)this->GetPtr() + g_PatternData.offset_UItem_ItemTableRowPtr);
		if (!ItemTableRow)
			return FName();
		return GetDMA().Read<FName>(ItemTableRow + GetItemIDOffset());
	}

	/*
	 * ASubActor
	 */
	UClass ASubActor::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.SubActor"_hash>();
		return c;
	}

	/*
	 * AItemSlotContainer
	 */
	UClass AItemSlotContainer::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.ItemSlotContainer"_hash>();
		return c;
	}

	/*
	 * AInventory
	 */
	UClass AInventory::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.Inventory"_hash>();
		return c;
	}

	/*
	 * EquipableItem
	 */
	UClass UEquipableItem::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.*7ed540bffc"_hash>();
		return c;
	}

	/*
	 * AEquipment
	 */
	UClass AEquipment::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.Equipment"_hash>();
		return c;
	}

	/*
	 * AItemExplorer
	 */
	UClass AItemExplorer::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.ItemExplorer"_hash>();
		return c;
	}

	/*
	 * AItemPackage
	 */
	UClass AItemPackage::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.ItemPackage"_hash>();
		return c;
	}

	/*
	 * ACarePackageItem
	 */
	UClass ACarePackageItem::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.CarePackageItem"_hash>();
		return c;
	}

	/*
	 * AFloorSnapItemPackage
	 */
	UClass AFloorSnapItemPackage::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.FloorSnapItemPackage"_hash>();
		return c;
	}

	/*
	 * AFloorSnapItemPackage
	 */
	UClass ATslFloatingVehicle::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslFloatingVehicle"_hash>();
		return c;
	}

	UAsyncStaticMeshComponent ATslWeapon::GetAttachedComponent(int a2)
	{
		unsigned char SlotID = a2 ? (a2 == 1 ? 6 : 0) : 3;

		if (this->Mesh3P)
		{
			auto mapData = this->Mesh3P->AttachedStaticComponentMap;
			auto pMap = reinterpret_cast<ptr_t>(mapData.UnknownData);

			if (*(uint32_t*)(pMap + 8) != *(uint32_t*)(pMap + 0x34))
			{
				ptr_t v4 = *(ptr_t*)(pMap + 0x40);
				ptr_t v5 = 0;
				if (!v4)
					v5 = pMap + 0x38;

				int32_t v6 = -1;
				if (v4)
					v6 = GetDMA().ReadCache<int32_t>(v4 + 4 * ((*(int32_t*)(pMap + 0x48) - 1i64) & SlotID));
				else if (v5)
					v6 = GetDMA().ReadCache<int32_t>(v5 + 4 * ((*(int32_t*)(pMap + 0x48) - 1i64) & SlotID));

				ptr_t v8 = 0;
				if (v6 != -1)
				{
					ptr_t array = *(ptr_t*)pMap;
					while (array)
					{
						v8 = array + 0x18ull * v6;
						if (GetDMA().ReadCache<unsigned char>(v8) == SlotID)
							break;

						v6 = GetDMA().ReadCache<int32_t>(v8 + 0x10);
						if (v6 == -1)
							return nullptr;
					}
					if (v8)
						return GetDMA().ReadCache<ptr_t>(v8 + 8);
				}
			}
		}

		return nullptr;
	}

	UAsyncStaticMeshComponent ATslWeapon::GetAttachedComponent(TMap AttachedStaticComponentMap, int a2)
	{
		unsigned char SlotID = a2 ? (a2 == 1 ? 6 : 0) : 3;

		if (this->Mesh3P)
		{
			auto mapData = AttachedStaticComponentMap;
			auto pMap = reinterpret_cast<ptr_t>(mapData.UnknownData);
			if (*(uint32_t*)(pMap + 8) != *(uint32_t*)(pMap + 0x34))
			{
				ptr_t v4 = *(ptr_t*)(pMap + 0x40);
				ptr_t v5 = 0;
				if (!v4)
					v5 = pMap + 0x38;

				int32_t v6 = -1;
				if (v4)
					v6 = GetDMA().ReadCache<int32_t>(v4 + 4 * ((*(int32_t*)(pMap + 0x48) - 1i64) & SlotID));
				else if (v5)
					v6 = GetDMA().ReadCache<int32_t>(v5 + 4 * ((*(int32_t*)(pMap + 0x48) - 1i64) & SlotID));

				ptr_t v8 = 0;
				if (v6 != -1)
				{
					int looptime = 0;
					ptr_t array = *(ptr_t*)pMap;
					while (array)
					{
						looptime++;
						if (looptime >= 10)
							return nullptr;
						v8 = array + 0x18ull * v6;
						if (GetDMA().ReadCache<unsigned char>(v8) == SlotID)
							break;

						v6 = GetDMA().ReadCache<int32_t>(v8 + 0x10);
						if (v6 == -1)
							return nullptr;
					}
					if (v8)
						return GetDMA().ReadCache<ptr_t>(v8 + 8);
				}
			}
		}

		return nullptr;
	}

	UClass ATslWeapon::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslWeapon"_hash>();
		return c;
	}


	/*
	 * ATslWeapon_Gun
	 */
	float ATslWeapon_Gun::GetZeroingDistance(unsigned char bCanted)
	{
		unsigned char SlotID = bCanted ? (bCanted == 1 ? 6 : 0) : 3;

		return 0.0f;
	}

	UClass ATslWeapon_Gun::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslWeapon_Gun"_hash>();
		return c;
	}

	FWeaponTrajectoryConfig ATslWeapon_Trajectory::GetWeaponTrajectoryConfig()
	{
		if (!this->IsValid())
			return FWeaponTrajectoryConfig();
		auto WeaponTrajectoryData = GetDMA().Read<ptr_t>((ptr_t)this->GetPtr() + g_PatternData.offset_ATslWeapon_Trajectory_WeaponTrajectoryData);
		if (WeaponTrajectoryData)
		{
			static auto TrajectoryConfigOffset = -1;
			if (TrajectoryConfigOffset == -1)
				TrajectoryConfigOffset = g_PatternData.offset_WeaponTrajectoryData_TrajectoryConfig;
			return GetDMA().Read<FWeaponTrajectoryConfig>(WeaponTrajectoryData + TrajectoryConfigOffset);
		}
		return FWeaponTrajectoryConfig();
	}

	/*
	 * ATslWeapon_Trajectory
	 */
	UClass ATslWeapon_Trajectory::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslWeapon_Trajectory"_hash>();
		return c;
	}

	/*
	 * ATslWeapon_Throwable
	 */
	UClass ATslWeapon_Throwable::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslWeapon_Throwable"_hash>();
		return c;
	}

	/*
	 * ATslWeapon_Gun_Projectile
	 */
	UClass ATslWeapon_Gun_Projectile::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslWeapon_Gun_Projectile"_hash>();
		return c;
	}

	/*
	 * UWeaponProcessorComponent
	 */
	int UWeaponProcessorComponent::GetWeaponIndex()
	{
		static ptr_t offset_WeaponIndex = 0;
		if (offset_WeaponIndex == 0)
		{
			//RightWeaponIndex
			//01 75 ? 0F BE 81 ? ? 00 00 C3 0F BE 81 ? ? 00 00 + 3
			//0F BE ?? ?? ?? ?? ?? 48 8B CF + 0
			//0F BE ?? ?? ?? ?? ?? 48 8B CF E8 ?? ?? ?? ?? 41 ?? ?? 89 ?? ?? ?? ?? ?? + 0
			//0F BE ?? ?? ?? ?? ?? 45 33 C0 ?? ?? ?? ?? + 0
#ifndef ObjectNameHash
			offset_WeaponIndex = GETOFFSET("StructProperty TslGame.WeaponProcessorComponent.WeaponArmInfo");
#else
			offset_WeaponIndex = g_PatternData.offset_UWeaponProcessorComponent_WeaponIndex;
#endif
			//LeftWeaponIndex
			//01 75 ? 0F BE 81 ? ? 00 00 C3 0F BE 81 ? ? 00 00 + 11

		}
		//CONSOLE_INFO("offset_WeaponIndex:%p", offset_WeaponIndex + 1);
		return GetDMA().Read<byte>((ptr_t)GetPtr() + offset_WeaponIndex + 1);

	}

	ATslWeapon UWeaponProcessorComponent::GetCurrentWeapon()
	{
		if (!this->IsValid())
			return nullptr;

		auto WeaponProcessor = this;
		int CurrentWeaponIndex = WeaponProcessor->GetWeaponIndex();

		if (CurrentWeaponIndex >= 0 && CurrentWeaponIndex <= 4)
		{
			return WeaponProcessor->GetEquippedWeapons()[CurrentWeaponIndex];
		}
		return nullptr;
	}
	UClass UWeaponProcessorComponent::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.WeaponProcessorComponent"_hash>();
		return c;
	}

	/*
	 * ATslWeapon_Melee
	 */
	UClass ATslWeapon_Melee::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslWeapon_Melee"_hash>();
		return c;
	}


	/*
	 * ATslProjectile
	 */
	UClass ATslProjectile::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslProjectile"_hash>();
		return c;
	}

	/*
	 * UShapeComponent
	 */
	UClass UShapeComponent::StaticClass()
	{
#ifndef ObjectNameHash
		auto c = ObjectsStore().FindClass<"Class Engine.ShapeComponent"_hash>();
#else
		//.ShapeColor
		auto c = ObjectsStore().FindClass<"Class Engine.*9b360611c3"_hash>();
#endif
		return c;
	}

	/*
	* USphereComponent
	*/
	UClass USphereComponent::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.SphereComponent"_hash>();
		return c;
	}
	/*
	 * USphereComponent
	 */
	UClass ATeam::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.Team"_hash>();
		return c;
	}

	/*
	 * ADroppedItem
	 */
	UClass ADroppedItem::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.DroppedItem"_hash>();
		return c;
	}

	/*
	 * ADefaultPawn
	 */
	UClass ADefaultPawn::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.DefaultPawn"_hash>();
		return c;
	}

	/*
	 * UVehicleRiderComponent
	 */
	UClass UVehicleRiderComponent::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.VehicleRiderComponent"_hash>();
		return c;
	}

	/*
	 * UAnimInstance
	 */
	UClass UAnimInstance::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.AnimInstance"_hash>();
		return c;
	}


	/*
	 * UTslAnimInstance
	 */
	UClass UTslAnimInstance::StaticClass()
	{
#ifndef ObjectNameHash
		auto c = ObjectsStore().FindClass<"Class TslGame.TslAnimInstance"_hash>();
#else
		auto c = ObjectsStore().FindClass<"Class TslGame.*4d150103af"_hash>();
#endif
		return c;
	}


	/*
	 * UTslSettings
	 */
	UClass UTslSettings::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslSettings"_hash>();
		return c;
	}

	UTslSettings UTslSettings::GetTslSettings()
	{
		static auto pTslSettingsInstance = 0ull;
		if (!pTslSettingsInstance && g_PatternData.fnGetTslSettingsInstance)
			pTslSettingsInstance = PatternLoader::GetDataPattern(g_PatternData.fnGetTslSettingsInstance + 7, 3, 8);

		if (pTslSettingsInstance)
		{
			auto instance = GetDMA().Read<ptr_t>(pTslSettingsInstance);
			if (instance)
			{
				return GetDMA().Read<ptr_t>(instance + g_PatternData.offset_TslSettings);
			}
		}

		return nullptr;
		/*static auto fn = ObjectsStore().FindObject<UFunction, "Function TslGame.TslSettings.GetTslSettings"_hash>();

		UTslSettings_GetTslSettings_Params params;

		auto flags = fn->FunctionFlags;
		fn->FunctionFlags |= 0x400;

		UObject::ProcessEvent(fn, &params);

		fn->FunctionFlags = flags;

		return params.ReturnValue;*/
	}

	/*
	 * ATslPlayerController
	 */
	UClass ATslPlayerController::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslPlayerController"_hash>();
		return c;
	}

	bool UKismetSystemLibrary::STATIC_LineTraceMulti(UObject** WorldContextObject, FVector* Start, FVector* End, TEnumAsByte<ETraceTypeQuery>* TraceChannel, bool* bTraceComplex, TArray<class AActor*>* ActorsToIgnore, TEnumAsByte<EDrawDebugTrace>* DrawDebugType, bool* bIgnoreSelf, FLinearColor* TraceColor, FLinearColor* TraceHitColor, float* DrawTime, TArray<struct FHitResult>* OutHits)
	{
		static auto fn = ObjectsStore().FindObject<UFunction, "Function Engine.KismetSystemLibrary.LineTraceMulti"_hash>();

		UKismetSystemLibrary_LineTraceMulti_Params params;
		params.WorldContextObject = WorldContextObject;
		params.Start = Start;
		params.End = End;
		params.TraceChannel = TraceChannel;
		params.bTraceComplex = bTraceComplex;
		params.ActorsToIgnore = ActorsToIgnore;
		params.DrawDebugType = DrawDebugType;
		params.bIgnoreSelf = bIgnoreSelf;
		params.TraceColor = TraceColor;
		params.TraceHitColor = TraceHitColor;
		params.DrawTime = DrawTime;

		//auto flags = fn->FunctionFlags;
		//fn->FunctionFlags |= 0x400;

		UObject::ProcessEvent(fn, &params);

		//fn->FunctionFlags = flags;

		if (OutHits != nullptr)
			*OutHits = params.OutHits;

		return params.ReturnValue;
	}

	/*
	 * UKismetSystemLibrary
	 */
	UClass UKismetSystemLibrary::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.KismetSystemLibrary"_hash>();
		return c;
	}

	FVector ATslWeapon_Trajectory::GetAdjustedAim()
	{
		APawn instigator = this->Instigator;

		ATslPlayerController PC = instigator->IsValid() ? instigator->Controller->SafeCast<ATslPlayerController>() : nullptr;
		FVector FinalAim = FVector();

		if (PC->IsValid())
		{
			FVector CamLoc;
			FRotator CamRot;
			PC->GetPlayerViewPoint(CamLoc, CamRot);

			FinalAim = CamRot.Vector();
		}
		else if (instigator->IsValid())
		{
			auto ow = instigator->Owner->SafeCast<ATslCharacter>();
			if (ow->IsValid())
			{
				auto AC = ow->Controller->SafeCast<TslAIController>();
				FinalAim = AC->ControlRotation.Vector();
			}
			else
			{
				// instigator->GetBaseAimRotation().Vector(); ??? maby
			}
		}
		return FinalAim;
	}

	FVector ATslWeapon_Trajectory::GetMuzzleLocation()
	{
		return Mesh3P->GetSocketLocation(MuzzleAttachPoint);
	}

	FVector ATslWeapon_Trajectory::GetFiringLocation()
	{
		return Mesh3P->GetSocketLocation(FiringAttachPoint);
	}

	FVector ATslWeapon_Trajectory::GetCameraDamageStartLocation(const FVector& AimDir)
	{
		FVector OutStartTrace = GetMuzzleLocation();
		auto player = this->Owner->SafeCast<ATslCharacter>();
		if (player)
		{
			APlayerController control = player->Controller->SafeCast<ATslPlayerController>();
			//if (!control)
			//{
			   // //VehicleRiderComponent
			//}
			if (control)
			{
				FRotator DummyRot;
				control->GetPlayerViewPoint(OutStartTrace, DummyRot);
				if (player->GetUnkBool())
				{
					APawn instigator = this->Instigator;
					if (instigator)
					{
						// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
						OutStartTrace = OutStartTrace + AimDir * (FVector::DotProduct((instigator->GetActorLocation() - OutStartTrace), AimDir));
					}
				}
			}
		}
		return OutStartTrace;
	}

	FVector UCurveVector::GetVectorValue(float InTime, FRichCurve_3 floatCurves, std::vector<std::vector<FRichCurveKey>>floatCurvesKeys)
	{
		FVector Result;
		if (floatCurvesKeys.size() != 3)
			return Result;
		Result.X = floatCurves.FloatCurves[0].EvalModifier(InTime, floatCurvesKeys[0]);
		Result.Y = floatCurves.FloatCurves[1].EvalModifier(InTime, floatCurvesKeys[1]);
		Result.Z = floatCurves.FloatCurves[2].EvalModifier(InTime, floatCurvesKeys[2]);
		return Result;
	}

	/*
	 * UCurveVector
	 */
	UClass UCurveVector::StaticClass()
	{
#ifndef ObjectNameHash
		auto c = ObjectsStore().FindClass<"Class Engine.CurveVector"_hash>();
#else
		auto c = ObjectsStore().FindClass<"Class Engine.*2b6179eb88"_hash>();
#endif
		return c;
	}

	/*
	 * UCurveBase
	 */
	UClass UCurveBase::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.CurveBase"_hash>();
		return c;
	}

	/*
	 * TslAIController
	 */
	UClass TslAIController::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.TslAIController"_hash>();
		return c;
	}


	/*
	 * UWeaponMeshComponent
	 */
	ptr_t UWeaponMeshComponent::GetReplicatedSkinParamOffset()
	{
		return g_PatternData.offset_UWeaponMeshComponent_ReplicatedSkinParam;
	}

	UClass UWeaponMeshComponent::StaticClass()
	{
#ifndef ObjectNameHash
		auto c = ObjectsStore().FindClass<"Class TslGame.WeaponMeshComponent"_hash>();
#else
		//OnAttachmentStaticMeshLoaded
		auto c = ObjectsStore().FindClass<"Class TslGame.*63a58eb6c9"_hash>();
#endif
		return c;
	}

	FTransform USkeletalMeshSocket::GetSocketLocalTransform()
	{
		return FTransform(RelativeRotation, RelativeLocation, RelativeScale);
	}

	/*
	 * USkeletalMeshSocket
	 */
	UClass USkeletalMeshSocket::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.SkeletalMeshSocket"_hash>();
		return c;
	}

	USkeletalMeshSocket USkeletalMesh::FindSocket(FName InSocketName)
	{
		int32_t DummyIdx;
		return FindSocketAndIndex(InSocketName, DummyIdx);
	}

	USkeletalMeshSocket USkeletalMesh::FindSocketAndIndex(FName InSocketName, int32_t& OutIndex)
	{
		OutIndex = INDEX_NONE;
		if (InSocketName == NAME_None)
		{
			return nullptr;
		}

		for (uint32_t i = 0; i < Sockets.Num(); i++)
		{
			USkeletalMeshSocket Socket = Sockets[i];
			if (Socket && Socket->SocketName == InSocketName)
			{
				OutIndex = i;
				return Socket;
			}
		}

		// If the socket isn't on the mesh, try to find it on the skeleton
		if (Skeleton)
		{
			USkeletalMeshSocket SkeletonSocket = Skeleton->FindSocketAndIndex(InSocketName, OutIndex);
			if (SkeletonSocket)
			{
				OutIndex += Sockets.Num();
			}
			return SkeletonSocket;
		}

		return nullptr;
	}

	/*
	 * USkeletalMesh
	 */
	UClass USkeletalMesh::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.SkeletalMesh"_hash>();
		return c;
	}

	USkeletalMeshSocket USkeleton::FindSocketAndIndex(FName InSocketName, int32_t& OutIndex)
	{
		OutIndex = INDEX_NONE;
		if (InSocketName == NAME_None)
		{
			return nullptr;
		}

		for (uint32_t i = 0; i < Sockets.Num(); ++i)
		{
			USkeletalMeshSocket Socket = Sockets[i];
			if (Socket && Socket->SocketName == InSocketName)
			{
				OutIndex = i;
				return Socket;
			}
		}

		return nullptr;
	}

	USkeletalMeshSocket USkeleton::FindSocket(FName InSocketName)
	{
		int32_t DummyIndex;
		return FindSocketAndIndex(InSocketName, DummyIndex);
	}

	/*
	 * USkeleton
	 */
	UClass USkeleton::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.Skeleton"_hash>();
		return c;
	}

	UStaticMeshSocket UStaticMeshComponent::GetSocketByName(FName InSocketName)
	{
		UStaticMeshSocket Socket(nullptr);

		if (StaticMesh)
		{
			Socket = StaticMesh->FindSocket(InSocketName);
		}

		return Socket;
	}

	FTransform UStaticMeshComponent::GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace)
	{
		if (InSocketName != NAME_None)
		{
			UStaticMeshSocket Socket = GetSocketByName(InSocketName);
			if (Socket)
			{
				FTransform SocketWorldTransform;
				if (Socket->GetSocketTransform(SocketWorldTransform, this))
				{
					switch (TransformSpace)
					{
					case RTS_World:
					{
						return SocketWorldTransform;
					}
					case RTS_Actor:
					{
						if (AActor Actor = this->Owner.Get())
						{
							return SocketWorldTransform.GetRelativeTransform(Actor->GetTransform());
						}
						break;
					}
					case RTS_Component:
					{
						return SocketWorldTransform.GetRelativeTransform(GetComponentTransform());
					}
					}
				}
			}
		}

		return UMeshComponent::GetSocketTransform(InSocketName, TransformSpace);
	}

	/*
	 * UStaticMeshComponent
	 */
	UClass UStaticMeshComponent::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.StaticMeshComponent"_hash>();
		return c;
	}

	/*
	 * UStaticMeshComponent
	 */
	UClass USplineMeshComponent::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.SplineMeshComponent"_hash>();
		return c;
	}

	/*
	 * UAsyncStaticMeshComponent
	 */
	UClass UAsyncStaticMeshComponent::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.AsyncStaticMeshComponent"_hash>();
		return c;
	}

	/*
	 * UAttachableItem
	 */
	UClass UAttachableItem::StaticClass()
	{
#ifndef ObjectNameHash
		auto c = ObjectsStore().FindClass<"Class TslGame.AttachableItem"_hash>();
#else
		//IntProperty TslGame.TslSettings.ObserverNetSpeed下一行
		//IntProperty TslGame.GameEventFluentdAdapter.Port下一行
		auto c = ObjectsStore().FindClass<"Class TslGame.*fd5bd8e039"_hash>();
#endif
		return c;
	}

	bool UStaticMeshSocket::GetSocketTransform(FTransform& OutTransform, UStaticMeshComponent* const MeshComp)
	{
		OutTransform = FTransform(RelativeRotation, RelativeLocation, RelativeScale) * MeshComp->GetComponentTransform();
		return true;
	}

	/*
	 * UStaticMeshSocket
	 */
	UClass UStaticMeshSocket::StaticClass()
	{
#ifndef ObjectNameHash
		auto c = ObjectsStore().FindClass<"Class Engine.StaticMeshSocket"_hash>();
#else
		auto c = ObjectsStore().FindClass<"Class Engine.*d4f521befd"_hash>();
#endif
		return c;
	}

	UStaticMeshSocket UStaticMesh::FindSocket(FName InSocketName)
	{
		if (InSocketName == NAME_None)
		{
			return nullptr;
		}

		for (uint32_t i = 0; i < Sockets.Num(); i++)
		{
			UStaticMeshSocket Socket = Sockets[i];
			if (Socket && Socket->SocketName == InSocketName)
			{
				return Socket;
			}
		}
		return nullptr;
	}

	/*
	 * UStaticMesh
	 */
	UClass UStaticMesh::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.StaticMesh"_hash>();
		return c;
	}

	bool UCharacterMovementComponent::IsWalkable(const FHitResult& Hit, float _WalkableFloorZ, UPrimitiveComponent HitResult_Component,
		FWalkableSlopeOverride WalkableSlopeOverride)
	{
		if (!Hit.IsValidBlockingHit())
		{
			// No hit, or starting in penetration
			return false;
		}

		// Never walk up vertical surfaces.
		if (Hit.ImpactNormal.Z < KINDA_SMALL_NUMBER)
		{
			return false;
		}

		float TestWalkableZ = _WalkableFloorZ;

		// See if this component overrides the walkable floor z.
		UPrimitiveComponent HitComponent = HitResult_Component;
		if (HitComponent->IsValid())
		{
			//const FWalkableSlopeOverride& SlopeOverride = HitComponent->GetWalkableSlopeOverride();
			FWalkableSlopeOverride& SlopeOverride = WalkableSlopeOverride;
			TestWalkableZ = SlopeOverride.ModifyWalkableFloorZ(TestWalkableZ);
		}

		// Can't walk on this surface if it is too steep.
		if (Hit.ImpactNormal.Z < TestWalkableZ)
		{
			return false;
		}

		return true;
	}

	FVector UCharacterMovementComponent::ComputeGroundMovementDelta(const FVector& Delta, const FHitResult& RampHit,
		const bool bHitFromLineTrace, UCharacterMovementComponent_BIT4 bit4, float WalkZ, UPrimitiveComponent HitResult_Component,
		FWalkableSlopeOverride WalkableSlopeOverride)
	{
		FVector FloorNormal = RampHit.ImpactNormal;
		FVector ContactNormal = RampHit.Normal;
		if (FloorNormal.Z < (1.f - KINDA_SMALL_NUMBER) && FloorNormal.Z > KINDA_SMALL_NUMBER && ContactNormal.Z > KINDA_SMALL_NUMBER && !bHitFromLineTrace && IsWalkable(RampHit, WalkZ, HitResult_Component, WalkableSlopeOverride))
		{
			// Compute a vector that moves parallel to the surface, by projecting the horizontal movement direction onto the ramp.
			float FloorDotDelta = (FloorNormal | Delta);
			FVector RampMovement(Delta.X, Delta.Y, -FloorDotDelta / FloorNormal.Z);
			if (bit4.bMaintainHorizontalGroundVelocity)
			{
				return RampMovement;
			}
			else
			{
				return RampMovement.GetSafeNormal() * Delta.Size();
			}
		}

		return Delta;
	}

	/*
	 * UCharacterMovementComponent
	 */
	UClass UWheeledVehicleMovementComponent::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class PhysXVehicles.WheeledVehicleMovementComponent"_hash>();
		return c;
	}



	/*
	 * UCharacterMovementComponent
	 */
	UClass UCharacterMovementComponent::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.CharacterMovementComponent"_hash>();
		return c;
	}

	/*
	 * UBodySetup
	 */
	UClass UBodySetup::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class Engine.BodySetup"_hash>();
		return c;
	}

	/*
	* UVisual
	*/
	UClass UVisual::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class UMG.Visual"_hash>();
		return c;
	}

	UClass UPanelSlot::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class UMG.PanelSlot"_hash>();
		return c;
	}

	UClass UCanvasPanelSlot::StaticClass()
	{
#ifndef ObjectNameHash
		auto c = ObjectsStore().FindClass<"Class UMG.CanvasPanelSlot"_hash>();
#else
		auto c = ObjectsStore().FindClass<"Class UMG.*CanvasPanelSlot"_hash>();
#endif
		return c;
	}

	UClass UWidget::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class UMG.Widget"_hash>();
		return c;
	}

	UClass UUserWidget::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class UMG.UserWidget"_hash>();
		return c;
	}

	UClass UTslUserWidget::StaticClass()
	{
#ifndef ObjectNameHash
		auto c = ObjectsStore().FindClass<"Class TslGame.TslUserWidget"_hash>();
#else
		//.ClearUserFocus.Player
		auto c = ObjectsStore().FindClass<"Class TslGame.*TslUserWidget"_hash>();
#endif
		return c;
	}

	UClass UUmgBaseWidget::StaticClass()
	{
#ifndef ObjectNameHash
		auto c = ObjectsStore().FindClass<"Class TslGame.UmgBaseWidget"_hash>();
#else
		//.BindFakeCursor
		auto c = ObjectsStore().FindClass<"Class TslGame.UmgBaseWidget"_hash>();
#endif
		return c;
	}

	UClass UMapGridWidget::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.MapGridWidget"_hash>();
		return c;
	}

	UClass UBlockInputUserWidget::StaticClass()
	{
#ifndef ObjectNameHash
		auto c = ObjectsStore().FindClass<"Class TslGame.BlockInputUserWidget"_hash>();
#else
		//.bCancelDragDrop
		auto c = ObjectsStore().FindClass<"Class TslGame.*BlockInputUserWidget"_hash>();
#endif
		return c;
	}


	UClass UTslNewWorldMapWidget::StaticClass()
	{
#ifndef ObjectNameHash
		auto c = ObjectsStore().FindClass<"Class TslGame.TslNewWorldMapWidget"_hash>();
#else
		auto c = ObjectsStore().FindClass<"Class TslGame.*TslNewWorldMapWidget"_hash>();
#endif
		return c;
	}

	float UTslNewWorldMapWidget::GetZoomFactor() {
		auto pMapGridWidget = this->MapGrid;
		if (!pMapGridWidget->IsValid())
			return 0.f;
		UMapGridWidget MapGridWidget = pMapGridWidget->GetPtr();
		if (!MapGridWidget->IsValid())
			return 0.f;

		UCanvasPanelSlot Slot = MapGridWidget->Cast<UWidget>()->Slot->Cast<UCanvasPanelSlot>();
		if (!Slot.IsValid())
			return 0.f;
		const FVector2D CurrentSize = { Slot.LayoutData.Offsets.Right, Slot.LayoutData.Offsets.Bottom };
		return CurrentSize.X / 1080.f;
	}

	FVector2D UTslNewWorldMapWidget::GetPosition() {
		auto pMapGridWidget = this->MapGrid;
		if (!pMapGridWidget->IsValid())
			return FVector2D(0.f, 0.f);
		UMapGridWidget MapGridWidget = pMapGridWidget->GetPtr();
		if (!MapGridWidget->IsValid())
			return FVector2D(0.f, 0.f);

		UCanvasPanelSlot Slot = MapGridWidget->Cast<UWidget>()->Slot->Cast<UCanvasPanelSlot>();
		if (!Slot.IsValid())
			return FVector2D(0.f, 0.f);

		const FVector2D CurrentPos = {
			Slot.LayoutData.Offsets.Right * (Slot.LayoutData.Alignment.X - 0.5f) - Slot.LayoutData.Offsets.Left,
			Slot.LayoutData.Offsets.Bottom * (Slot.LayoutData.Alignment.Y - 0.5f) - Slot.LayoutData.Offsets.Top
		};

		const float ZoomFactor = GetZoomFactor();
		return { CurrentPos.X / 1080.f / ZoomFactor * 2.0f, CurrentPos.Y / 1080.f / ZoomFactor * 2.0f };
	}

	bool UTslNewWorldMapWidget::IsVisible() {
		if (IsNearlyZero(GetZoomFactor()))
			return false;
		return Visibility == ESlateVisibility::SelfHitTestInvisible;
	}

	UClass UMinimapCanvasWidget::StaticClass()
	{
#ifndef ObjectNameHash
		auto c = ObjectsStore().FindClass<"Class TslGame.MinimapCanvasWidget"_hash>();
#else
		auto c = ObjectsStore().FindClass<"Class TslGame.MinimapCanvasWidget"_hash>();
#endif
		return c;
	}

	/*
	 * AInventoryFacade
	 */

	UClass AInventoryFacade::StaticClass()
	{
		auto c = ObjectsStore().FindClass<"Class TslGame.InventoryFacade"_hash>();
		return c;
	}
}

