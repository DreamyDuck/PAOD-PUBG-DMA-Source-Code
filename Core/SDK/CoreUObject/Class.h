#pragma once
#include "ObjectMacros.h"
#include "Object.h"
#include "Struct.h"
#include "Parameters.h"
#include "Core/ItemManager.h"
#define ObjectNameHash

namespace IronMan::Core::SDK
{
#pragma warning(disable:4307) 

	class UFont : public UObject
	{
	public:
		UFont(const UObject& object) :UObject(object) {}
		UFont(void* object) :UObject(object) {}
		UFont(uint64_t object) :UObject(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		MAKE_GENERAL_PROPERTY(int, LegacyFontSize, "IntProperty Engine.Font.LegacyFontSize");
#else
		MAKE_GENERAL_PROPERTY(int, LegacyFontSize, "IntProperty Engine.Font.*25cef308a1", g_PatternData.offset_UFont_LegacyFontSize);//13C
#endif
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};


	class UCanvas : public UObject
	{
	public:
		UCanvas(const UObject& object) :UObject(object) {}
		UCanvas(void* object) :UObject(object) {}
		UCanvas(uint64_t object) :UObject(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//FF 90 ? ? 00 00 66 0F 6E ? ? 66 0F 6E ? ? 0F 5B + 6
		//89 51 ? 89 51 ? 33 D2 F3 0F ? ? ? ? 00 00 44 + 0
		MAKE_GENERAL_PROPERTY(int, SizeX, "IntProperty Engine.Canvas.SizeX");
		//FF 90 ? ? 00 00 66 0F 6E ? ? 66 0F 6E ? ? 0F 5B + B
		//44 89 ? ? 44 89 ? ? 4C 89 ? ? ? 00 00 FF 90 + 0
		MAKE_GENERAL_PROPERTY(int, SizeY, "IntProperty Engine.Canvas.SizeY");
#else
		MAKE_GENERAL_PROPERTY(int, SizeX, "IntProperty Engine.Canvas.SizeX", g_PatternData.offset_UCanvas_SizeX);
		MAKE_GENERAL_PROPERTY(int, SizeY, "IntProperty Engine.Canvas.SizeY", g_PatternData.offset_UCanvas_SizeY);
#endif

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};


	class UField :public UObject
	{
	public:
		UField(const UObject& object) :UObject(object), Next(INVALID_POINTER) {}
		UField(void* object);

	public:
		UField GetNext();
	private:
		/*UField*/void* Next;
	};

	class UStruct :public UField
	{
	public:
		UStruct(const UObject& object) :UField(object),
			SuperStruct(INVALID_POINTER),
			Children(INVALID_POINTER),
			PropertiesSize(-1) {}
		UStruct(void* object);
	public:
		UStruct GetSuperStruct();
		UField GetChildren();
		int32_t GetPropertiesSize();
	private:
		/*UStruct*/void* SuperStruct;
		/*UField*/void* Children;
		int32_t PropertiesSize;
	};

	class UScriptStruct : public UStruct
	{
	public:
		UScriptStruct(void* object) :UStruct(object) {}

	};

	class UClass : public UStruct
	{
		struct FClassBaseChain
		{
		private:
			ptr_t/*FClassBaseChain***/ ClassBaseChainArray;
			int32_t NumClassBasesInChainMinusOne;
			friend class UClass;
		};

		struct FClassBaseChainMap
		{
		public:
			ptr_t ClassBaseChainArray;
			int32_t NumClassBasesInChainMinusOne;
			ptr_t BaseChainArray;
			inline FClassBaseChainMap()
				: ClassBaseChainArray(0), NumClassBasesInChainMinusOne(0), BaseChainArray(0) {
			}
			inline FClassBaseChainMap(ptr_t _A, int32_t _B, ptr_t _C)
				: ClassBaseChainArray(_A), NumClassBasesInChainMinusOne(_B), BaseChainArray(_C) {
			}
		};
	public:
		UClass(const UObject& object) :UStruct(object) {}
		UClass(void* object) :UStruct(object) {}

	public:

		FORCEINLINE const FClassBaseChain GetClassBaseChain() const
		{
			return GetDMA().ReadCache<FClassBaseChain>((ptr_t)GetPtr() + g_PatternData.offset_UClassBaseChainArray);
		}

		FORCEINLINE bool IsAUsingClassArray(const UClass& Parent) const
		{
			FClassBaseChain thisClassBaseChain = GetClassBaseChain();
			FClassBaseChain parentClassBaseChain = Parent.GetClassBaseChain();
			int32_t NumParentClassBasesInChainMinusOne = parentClassBaseChain.NumClassBasesInChainMinusOne;
			return NumParentClassBasesInChainMinusOne <= thisClassBaseChain.NumClassBasesInChainMinusOne &&
				GetDMA().ReadCache<ptr_t>(thisClassBaseChain.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*))
				== (ptr_t)Parent.GetPtr() + g_PatternData.offset_UClassBaseChainArray;
		}


		template<typename T>
		bool UClassIsA()
		{
			if (!IsValid())
				return false;

			auto cmp = T::StaticClass();
			if (!cmp.IsValid())
			{
				return false;
			}

			for (auto super = *this; super.IsValid(); super = super.GetSuperStruct().Cast<UClass>())
			{
				if (super.GetPtr() == cmp.GetPtr())
				{
					return true;
				}
			}

			return false;
		}
		auto operator->() { return this; }
		static UClass StaticClass();
	};

	class UProperty : public UField
	{
	public:
		UProperty(const UObject& object) :UField(object),
			ArrayDim(-1),
			ElementSize(-1),
			PropertyFlags(-1),
			PropertySize(-1),
			Offset(-1) {}
		UProperty(void* object);
	public:
		int32_t		GetArrayDim();
		int32_t		GetElementSize();
		uint64_t	GetPropertyFlags();
		int32_t		GetPropertySize();
		int32_t		GetOffset();
		int32_t		GetOffset_Debug(ptr_t Offset);
	private:
		int32_t		ArrayDim;		//0x0038 
		int32_t		ElementSize;	//0x003C 
		uint64_t	PropertyFlags;	//0x0040 
		int32_t		PropertySize;	//0x0048 
		int32_t		Offset;			//0x0058 
	};

	class UFunction : public UStruct
	{
	public:
		UFunction(const UObject& object) :UStruct(object) {}
		UFunction(void* object) :UStruct(object) {}
		UFunction(uint64_t object) :UFunction(reinterpret_cast<void*>(object)) {}

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};


	// Class Engine.CurveBase
	// 0x0008 (0x0038 - 0x0030)
	class UCurveBase : public UObject
	{
	public:
		UCurveBase(const UObject& object) :UObject(object) {}
		UCurveBase(void* object) :UObject(object) {}
		UCurveBase(uint64_t object) :UObject(reinterpret_cast<void*>(object)) {}
	public:

	public:
		static UClass StaticClass();
		auto operator->() { return this; }
	};

	// Class Engine.CurveVector
	// 0x0150 (0x0188 - 0x0038)
	class UCurveVector : public UCurveBase
	{
	public:
		UCurveVector(const UObject& object) :UCurveBase(object) {}
		UCurveVector(void* object) :UCurveBase(object) {}
		UCurveVector(uint64_t object) :UCurveBase(reinterpret_cast<void*>(object)) {}

#ifndef ObjectNameHash
		//F3 0F 5C ? 0F 54 ? 48 83 C1 ? 0F 28 ? 0F 57 ? E8 + 7
		//48 85 ? 0F 84 ? ? ? ? 48 83 C1 ? 0F 57 ? 0F 28 ? E8 + 9
		//48 8B ? ? ? 00 00 48 83 C1 ? F3 0F 10 ? ? ? ? ? 0F 57 + 7

		//0F 28 ? E8 ? ? ? ? 0F 57 ? 0F 2F ? 77 + 3,1,5]进入Call 读取 + 3一字节
		MAKE_STRUCT_PROPERTY(FRichCurve_3, FloatCurves, "StructProperty Engine.CurveVector.FloatCurves");
#else
		MAKE_STRUCT_PROPERTY(FRichCurve_3, FloatCurves, "StructProperty Engine.*c951b7bd49.*345eae5fa9", g_PatternData.offset_UCurveVector_FloatCurves);
#endif

		FVector GetVectorValue(float InTime, FRichCurve_3 floatCurves, std::vector<std::vector<FRichCurveKey>>floatCurvesKeys);
	public:
		static UClass StaticClass();
		auto operator->() { return this; }
	};

	class UGameInstance :public UObject
	{
	public:
		UGameInstance(const UObject& object) :UObject(object) {}
		UGameInstance(void* object) :UObject(object) {}
		UGameInstance(uint64_t object) :UGameInstance(reinterpret_cast<void*>(object)) {}
	public:
		TArray<UEEncryptedObjectProperty<class ULocalPlayer, DecryptFunc::General>> GetLocalPlayers();
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};


	class ULevel :public UObject
	{
	public:
		ULevel(const UObject& object) :UObject(object),
			AActors(object.GetPtr(), g_PatternData.offsetAActors) {}
		ULevel(void* object)
			:UObject(object),
			AActors(object, g_PatternData.offsetAActors)
		{}
		ULevel(uint64_t object) :ULevel(reinterpret_cast<void*>(object)) {}
	public:
		UEncryptedObjectProperty<TArray<void*>, DecryptFunc::General> AActors;
#ifndef ObjectNameHash
		//48 8B ? ? ? 00 00 44 38 ? ? ? ? ? F3 0F 10 ? ? ? 00 00 F3 0F 59 ? ? ? 00 00 F3 + 0
		//48 8B ? ? ? 00 00 48 85 ? 74 ? 48 39 ? ? ? 00 00 74 ? F3 0F 10 + 0
		MAKE_GENERAL_PROPERTY(ptr_t, WorldSettings, "ObjectProperty Engine.Level.WorldSettings");
#else
		MAKE_GENERAL_PROPERTY(ptr_t, WorldSettings, "ObjectProperty Engine.Level.WorldSettings", g_PatternData.offset_ULvel_WorldSettings);
#endif
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class FPhysScene;
	class PxScene;

	class UWorld :public UObject
	{
	public:
		UWorld(void* object)
			:UObject(object),
			OwningGameInstance(object, g_PatternData.offset_OwningGameInstance),
			PersistentLevel(object, g_PatternData.offsetPersistentLevel),
			GameState(object, g_PatternData.offset_GameState)
		{
		}
		UWorld(const UObject& object) :UObject(object),
			OwningGameInstance(object.GetPtr(), g_PatternData.offset_OwningGameInstance),
			PersistentLevel(object.GetPtr(), g_PatternData.offsetPersistentLevel),
			GameState(object.GetPtr(), g_PatternData.offset_GameState) {}
		UWorld(uint64_t object) :UWorld(reinterpret_cast<void*>(object)) {}
	public:
		UEncryptedObjectProperty<UGameInstance, DecryptFunc::General> OwningGameInstance;
		UEncryptedObjectProperty<ULevel, DecryptFunc::General> PersistentLevel;
		UEncryptedObjectProperty<UObject, DecryptFunc::General> GameState;

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};



	class USkeletalMeshSocket :public UObject
	{
	public:
		USkeletalMeshSocket(const UObject& object) :UObject(object) {}
		USkeletalMeshSocket(void* object = INVALID_POINTER) :UObject(object) {}
		USkeletalMeshSocket(uint64_t object) :UObject(reinterpret_cast<void*>(object)) {}

	public:
#ifndef ObjectNameHash
		//bForceAlwaysAnimated - 0x34
		MAKE_STRUCT_PROPERTY(struct FName, SocketName, "NameProperty Engine.SkeletalMeshSocket.SocketName");
		//bForceAlwaysAnimated - 0x2C
		MAKE_STRUCT_PROPERTY(struct FName, BoneName, "NameProperty Engine.SkeletalMeshSocket.BoneName");
		//bForceAlwaysAnimated - 0x24
		MAKE_STRUCT_PROPERTY(struct FVector, RelativeLocation, "StructProperty Engine.SkeletalMeshSocket.RelativeLocation");
		//bForceAlwaysAnimated - 0x18
		MAKE_STRUCT_PROPERTY(struct FRotator, RelativeRotation, "StructProperty Engine.SkeletalMeshSocket.RelativeRotation");
		//bForceAlwaysAnimated - 0xC
		MAKE_STRUCT_PROPERTY(struct FVector, RelativeScale, "StructProperty Engine.SkeletalMeshSocket.RelativeScale");
		//66 89 ? ? 48 8D ? ? 80 ? ? 00 74 ? 48 8D + 8
		//66 89 ? ? 48 8D ? ? 80 ? ? 00 0F 84 ? ? ? ? 48 8D + 8
		//66 89 ? ? 48 8D ? ? 41 80 ? ? 00 48 8D ? ? 75 + 8
		//8b ? 83 ? FF 0F 84 ? ? ? ? 41 ? ? ? 00 74 ? ? 8b + 0xB
		MAKE_GENERAL_PROPERTY(char, bForceAlwaysAnimated, "BoolProperty Engine.SkeletalMeshSocket.bForceAlwaysAnimated");
#else
		MAKE_STRUCT_PROPERTY(struct FName, SocketName, "NameProperty Engine.SkeletalMeshSocket.SocketName", g_PatternData.offset_SkeletalMeshSocket_SocketName);
		MAKE_STRUCT_PROPERTY(struct FName, BoneName, "NameProperty Engine.SkeletalMeshSocket.BoneName", g_PatternData.offset_SkeletalMeshSocket_BoneName);
		MAKE_STRUCT_PROPERTY(struct FVector, RelativeLocation, "StructProperty Engine.SkeletalMeshSocket.RelativeLocation", g_PatternData.offset_SkeletalMeshSocket_RelativeLocation);
		MAKE_STRUCT_PROPERTY(struct FRotator, RelativeRotation, "StructProperty Engine.SkeletalMeshSocket.RelativeRotation", g_PatternData.offset_SkeletalMeshSocket_RelativeRotation);
		MAKE_STRUCT_PROPERTY(struct FVector, RelativeScale, "StructProperty Engine.SkeletalMeshSocket.RelativeScale", g_PatternData.offset_SkeletalMeshSocket_RelativeScale);
		MAKE_GENERAL_PROPERTY(char, bForceAlwaysAnimated, "BoolProperty Engine.SkeletalMeshSocket.bForceAlwaysAnimated", g_PatternData.offset_SkeletalMeshSocket_bForceAlwaysAnimated);
#endif
		/** returns FTransform of Socket local transform */
		FTransform GetSocketLocalTransform();
	public:
		auto operator->() { return this; }
		static UClass StaticClass();
	};

	class USkeleton :public UObject
	{
	public:
		USkeleton(const UObject& object) :UObject(object) {}
		USkeleton(void* object = INVALID_POINTER) :UObject(object) {}
		USkeleton(uint64_t object) :UObject(reinterpret_cast<void*>(object)) {}

	public:
#ifndef ObjectNameHash
		//8b ? ? ? 00 00 48 8b ? e8 ? ? ? ? 48 85 ? 74 ? ? 8b ? ? 8b ? ? 8b ? e8 第一个Call进去然后再一个call
		//85 ? 7E ? ? 8B ? ? ? 00 00 ? 8B ? ? 8B ? ? 8B ? ? 85 ? 74 ? ? 39 ? ? 74 ? ? FF ? ? FF + 4
		//85 ? 7E ? ? 8B ? ? ? 00 00 ? 8B ? ? 8B ? 66 90 ? 8B ? ? 85 + 4
		//TArray.Num->C7 ? FF FF FF FF ? 89 ? 24 ? ? 3B ? 74 ? 48 63 ? ? ? 00 00 + 0x10
		//TArray.Num->C7 ? FF FF FF FF ? 3B ? 74 ? 48 63 ? ? ? 00 00 + 0xB
		MAKE_GENERAL_PROPERTY(TArray<class USkeletalMeshSocket*>, Sockets, "ArrayProperty Engine.Skeleton.Sockets");
#else
		MAKE_GENERAL_PROPERTY(TArray<class USkeletalMeshSocket*>, Sockets, "ArrayProperty Engine.Skeleton.Sockets", g_PatternData.offset_USkeleton_Sockets);
#endif
		class USkeletalMeshSocket FindSocketAndIndex(FName InSocketName, int32_t& OutIndex);
		class USkeletalMeshSocket FindSocket(FName InSocketName);
	public:
		auto operator->() { return this; }
		static UClass StaticClass();
	};

	class USkeletalMesh :public UObject
	{
	public:
		USkeletalMesh(const UObject& object) :UObject(object) {}
		USkeletalMesh(void* object = INVALID_POINTER) :UObject(object) {}
		USkeletalMesh(uint64_t object) :UObject(reinterpret_cast<void*>(object)) {}

	public:
#ifndef ObjectNameHash
		//0F 84 ? ? ? ? ? 8B ? ? ? ? ? ? ? 00 00 0F B7 ? ? 8B ? ? ? 00 00 ? 8B + 6
		//74 ? ? 8B ? ? ? 85 ? 74 ? 41 0F B7 ? ? ? 00 00 66 + 2
		//48 83 ? ? 00 ? 8B ? 48 8B ? 48 8B ? 74 ? ? 85 + 0
		//0f 8f ? ? ? ? 48 8B ? ? 48 85 ? 74 ? C7 ? FF FF FF FF + 6
		MAKE_OBJECT_PROPERTY(USkeleton, Skeleton, "ObjectProperty Engine.SkeletalMesh.Skeleton");
		//48 83 22 00 48 63 ? ? ? 00 00 48 8B ? ? ? 00 00 89 ? 08 85 ? 0F 85 ? ? ? ? ? 7A + 0xB
		////48 83 22 00 48 63 ? ? ? 00 00 48 8B ? ? ? 00 00 89 ? 08 85 ? 75 ? ? 7A
		//48 ? ? 00 00 00 00 48 63 ? ? ? 00 00 48 8B ? ? ? 00 00 89 ? 08 85 ? 75 ? ? 7A + 0xE
		MAKE_GENERAL_PROPERTY(TArray<USkeletalMeshSocket*>, Sockets, "ArrayProperty Engine.SkeletalMesh.Sockets");


		MAKE_EXTRA_STRUCT_PROPERTY(FReferenceSkeleton, RefSkeleton, "ArrayProperty Engine.SkeletalMesh.MorphTargets", 0x18);
#else
		MAKE_OBJECT_PROPERTY(USkeleton, Skeleton, "ObjectProperty Engine.SkeletalMesh.Skeleton", g_PatternData.offset_USkeletalMesh_Skeleton);
		MAKE_GENERAL_PROPERTY(TArray<USkeletalMeshSocket*>, Sockets, "ArrayProperty Engine.SkeletalMesh.Sockets", g_PatternData.offset_USkeletalMesh_Sockets);
		MAKE_STRUCT_PROPERTY(FReferenceSkeleton, RefSkeleton, "ArrayProperty Engine.SkeletalMesh.MorphTargets", g_PatternData.offset_USkeletalMesh_RefSkeleton);
#endif
		/**
		 *	Find a socket object in this SkeletalMesh by name.
		 *	Entering NAME_None will return NULL. If there are multiple sockets with the same name, will return the first one.
		 */
		USkeletalMeshSocket FindSocket(FName InSocketName);

		/**
		 *	Find a socket object in this SkeletalMesh by name.
		 *	Entering NAME_None will return NULL. If there are multiple sockets with the same name, will return the first one.
		 *  Also returns the index for the socket allowing for future fast access via GetSocketByIndex()
		 */
		USkeletalMeshSocket FindSocketAndIndex(FName InSocketName, int32_t& OutIndex);

	public:
		auto operator->() { return this; }
		static UClass StaticClass();
	};

	class AActor :public UObject
	{
	public:
		AActor(const UObject& object) :UObject(object), RootComponent(object.GetPtr(), g_PatternData.offset_Actor_RootComponent), Owner(object.GetPtr(), g_PatternData.offset_Actor_Owner), _RefSkeleton(),
			_bInit(false), _EncryptRootComponent(0), _RootComponent(nullptr), _VehicleCommonComponent(nullptr), _VehicleCommonComponentHealth(0.f),
			_VehicleCommonComponentHealthMax(0.f), _VehicleName(""), _VehicleRiderComponent(nullptr), _LastVehiclePawn(nullptr), _LastVehiclePawnEncryptIndex(0),
			_SeatIndex(0), _LinearVelocity(FVector()), _EncryptCharacterMovement(0), _CharacterMovement(nullptr), _Velocity(FVector()), _CurrentFloor(FFindFloorResult()),
			_CurrentFloor_HitResult_Component(nullptr), _CurrentFloor_HitResult_Component_BodyInstance_bOverrideWalkableSlopeOnInstance(0), _CurrentFloor_HitResult_Component_BodyInstance_WalkableSlopeOverride(FWalkableSlopeOverride()),
			_CurrentFloor_HitResult_Component_BodyInstance_BodySetup(TWeakObjectPtr<class UBodySetup>()), _bMaintainHorizontalGroundVelocity(UCharacterMovementComponent_BIT4()),
			_WalkableFloorZ(0.f), _WeaponProcess(nullptr), _EncryptItem(0), _Item(nullptr), _ItemTableRow(nullptr), _Durability(0.f), _DurabilityMax(0.f), _ItemID(FName()),
			_ItemName(""), _Items(TArray<UItem*>()), _ItemsNum(0), _CacheLocation(FVector()), _Location(FVector()), _delta(FVector()), _dist(0),
			_TimeTillExplosion(0), _ExplosionDelay(0), _ExistExplorTime(0), _LogTimeTillExplosion(0), _bProjectleValid(0), _ProjectileConfig(FProjectileData()),
			_ProjectileName(""), _OwnedComponents(TSet_Components()), _EncryptObjectNameIndex(0), _EncryptActorID(0), _ActorName(""),
			_transforActor(0), _ValidItem(true), _NextReadPosTime(0), _SkeletalMesh(nullptr), _VehicleMesh(nullptr), _VehicleCommonComponentHealthBack(0.f),
			_LastUpdateTime(0), _LastUpdateTime_CacheLocation(0), _StanceMode(0), _EncryptClass(0), _Class(0), _ClassBaseChain(CClassBaseChain()), _EPlayerValue(0),
			_EVehicleValue(0), _EBoatValue(0), _EItemGroupValue(0), _EItemValue(0), _EDeathBoxValue(0), _EAirdropValue(0), _EProjectileValue(0),
			_EDroppedItemValue(0), _InitCheck(false), _LastClass(0), _InitCmp(0), _LastUpdateTime_Check(0), _LastUpdateTime_Update(0)

		{
			_ArrayData._itemArray.resize(0);
			_digaArray.resize(0);
		}
		AActor(void* object = INVALID_POINTER) :UObject(object), RootComponent(object, g_PatternData.offset_Actor_RootComponent), Owner(object, g_PatternData.offset_Actor_Owner), _RefSkeleton(),
			_bInit(false), _EncryptRootComponent(0), _RootComponent(nullptr), _VehicleCommonComponent(nullptr), _VehicleCommonComponentHealth(0.f),
			_VehicleCommonComponentHealthMax(0.f), _VehicleName(""), _VehicleRiderComponent(nullptr), _LastVehiclePawn(nullptr), _LastVehiclePawnEncryptIndex(0),
			_SeatIndex(0), _LinearVelocity(FVector()), _EncryptCharacterMovement(0), _CharacterMovement(nullptr), _Velocity(FVector()), _CurrentFloor(FFindFloorResult()),
			_CurrentFloor_HitResult_Component(nullptr), _CurrentFloor_HitResult_Component_BodyInstance_bOverrideWalkableSlopeOnInstance(0), _CurrentFloor_HitResult_Component_BodyInstance_WalkableSlopeOverride(FWalkableSlopeOverride()),
			_CurrentFloor_HitResult_Component_BodyInstance_BodySetup(TWeakObjectPtr<class UBodySetup>()), _bMaintainHorizontalGroundVelocity(UCharacterMovementComponent_BIT4()),
			_WalkableFloorZ(0.f), _WeaponProcess(nullptr), _EncryptItem(0), _Item(nullptr), _ItemTableRow(nullptr), _Durability(0.f), _DurabilityMax(0.f), _ItemID(FName()),
			_ItemName(""), _Items(TArray<UItem*>()), _ItemsNum(0), _CacheLocation(FVector()), _Location(FVector()), _delta(FVector()), _dist(0),
			_TimeTillExplosion(0), _ExplosionDelay(0), _ExistExplorTime(0), _LogTimeTillExplosion(0), _bProjectleValid(0), _ProjectileConfig(FProjectileData()),
			_ProjectileName(""), _OwnedComponents(TSet_Components()), _EncryptObjectNameIndex(0), _EncryptActorID(0), _ActorName(""),
			_transforActor(0), _ValidItem(true), _NextReadPosTime(0), _SkeletalMesh(nullptr), _VehicleMesh(nullptr), _VehicleCommonComponentHealthBack(0.f),
			_LastUpdateTime(0), _LastUpdateTime_CacheLocation(0), _StanceMode(0), _EncryptClass(0),_Class(0), _ClassBaseChain(CClassBaseChain()), _EPlayerValue(0),
			_EVehicleValue(0), _EBoatValue(0), _EItemGroupValue(0), _EItemValue(0), _EDeathBoxValue(0), _EAirdropValue(0), _EProjectileValue(0),
			_EDroppedItemValue(0),_InitCheck(false), _LastClass(0), _InitCmp(0), _LastUpdateTime_Check(0), _LastUpdateTime_Update(0)
		{
			_ArrayData._itemArray.resize(0);
			_digaArray.resize(0);
		}
		AActor(uint64_t object) :AActor(reinterpret_cast<void*>(object)) {}

	public:
		//F2 0F 11 ? ? 89 43 ? 48 83 ? ? ? ? ? 00 ? 8B ? ? ? 00 00 75 + 0x10
		//F3 0F 59 ? ? 45 0F 57 ? 48 83 ? ? ? ? ? 00 ? 8B ? ? ? 00 00 75 + 0x11
		//88 46 2D 48 83 ? ? ? ? ? 00 ? 8B ? ? ? 00 00 + 0xB

		UEncryptedObjectProperty<class USceneComponent, DecryptFunc::General> RootComponent;
		//8A 81 ? ? 00 00 48 8B ? ? 8B ? ? 88 05 + 9
		//41 8D ? 01 FF 90 ? ? 00 00 48 83 ? ? ? ? ? 00 ? 8B ? ? 75 + 0x12
		//F2 0F 11 ? ? 89 43 ? 48 83 ? ? ? ? ? 00 ? 8B ? ? ? 00 00 0F 85 ? ? ? ? ? 8B ? ? 8B ? FF 15 + 0x10
		//F2 0F 11 ? ? 89 43 ? 48 83 ? ? ? ? ? 00 ? 8B ? ? 0F 85 ? ? ? ? ? 8B ? ? 8B ? FF 15 + 0x10
		UEncryptedObjectProperty<AActor, DecryptFunc::General> Owner;

		//48 8b ? ? ? 00 00 48 89 ? ? c7 45 ? 08 + 0
		//8B ? ? ? 00 00 48 89 ? 24 ? E8 ? ? ? ? 48 89 ? 24 ? C6 + 0xB] ->第一个call里面
		//C7 ? ? 08 00 00 00 48 89 ? ? 48 8B ? ? ? 00 00 48 89 ? ? 44 + 0xB
		MAKE_OBJECT_PROPERTY(/*APawn*/void*, Instigator, "ObjectProperty Engine.Actor.Instigator", g_PatternData.offset_Actor_Instigator);
		//74 ? F3 0F 10 ? ? ? 00 00 EB ? F3 0F 10 ? ? ? ? ? ? 8B ? ? ? 8B ? F3 + 2
		//74 ? F3 0F 10 ? ? EB ? F3 0F 10 ? ? ? ? ? ? 8B ? ? ? 8B ? F3 + 2
		//74 ? F3 0F 10 ? ? ? 00 00 EB ? F3 0F 10 ? ? ? ? ? 48 8B ? ? ? F3 + 2
		//48 8D ? ? ? 00 00 48 8D ? ? E8 ? ? ? ? ? 8B ? ? ? 00 00 48 8B ? 44 + 0x10
		MAKE_GENERAL_PROPERTY(float, CustomTimeDilation, "FloatProperty Engine.Actor.CustomTimeDilation", g_PatternData.offset_Actor_CustomTimeDilation);

#ifndef ObjectNameHash
		//84 C0 74 07 C6 ? ? ? 00 00 00 80 ? ? ? 00 00 02 + 0xB
		//84 C0 74 07 C6 ? ? ? 00 00 00 80 ? ? 02 0F + 0xB
		//E8 ? ? ? ? 0F 10 ? ? 8B ? ? ? 00 00 0F 10 ? ? 80 + 5
		//E8 ? ? ? ? 0F 10 ? ? 8B ? ? ? 00 00 0F 10 ? ? ? 00 00 80 + 5
		//E8 ? ? ? ? 0F 10 ? ? ? 00 00 8B ? ? ? 00 00 0F 10 ? ? ? 00 00 80 + 5
		MAKE_STRUCT_PROPERTY(struct FVector, LinearVelocity, "StructProperty Engine.Actor.ReplicatedMovement");
#else
		MAKE_STRUCT_PROPERTY(struct FVector, LinearVelocity, "StructProperty Engine.Actor.ReplicatedMovement", g_PatternData.offset_Actor_LinearVelocity);
#endif


		FORCEINLINE FVector GetActorLocation() const;

		FTransform GetTransform() const
		{
			return ActorToWorld();
		}



		/** Get the local-to-world transform of the RootComponent. Identical to GetTransform(). */
		FORCEINLINE FTransform ActorToWorld() const;

	public:
		//WfirstRWLock _AcotrLock; // 用于保护成员变量的读写锁

		bool _bInit;

		ULONGLONG _LastUpdateTime;
		ULONGLONG _LastUpdateTime_CacheLocation;
		ULONGLONG _LastUpdateTime_Check;
		ULONGLONG _LastUpdateTime_Update;

		ptr_t _EncryptClass;
		ptr_t _Class;
		ptr_t _LastClass;
		CClassBaseChain _ClassBaseChain;

		ptr_t _EPlayerValue;
		ptr_t _EVehicleValue;
		ptr_t _EBoatValue;
		ptr_t _EItemGroupValue;
		ptr_t _EItemValue;
		ptr_t _EDeathBoxValue;
		ptr_t _EAirdropValue;
		ptr_t _EProjectileValue;
		ptr_t _EDroppedItemValue;
		bool _InitCheck;
		bool _InitCmp;

		bool _transforActor;

		int32_t _EncryptActorID;
		std::string _ActorName;
		ptr_t _EncryptRootComponent;
		void* _RootComponent;
		void* _VehicleCommonComponent;
		float _VehicleCommonComponentHealthBack;
		float _VehicleCommonComponentHealth;
		float _VehicleCommonComponentHealthMax;
		int32_t _EncryptObjectNameIndex;
		std::string _VehicleName;

		void* _VehicleRiderComponent;
		void* _LastVehiclePawn;
		void* _VehicleMesh;
		void* _SkeletalMesh;
		FReferenceSkeleton _RefSkeleton;
		std::vector<FMeshBoneInfo> _VehicleBoneInfo;
		std::vector<std::string> _VehicleBoneName;
		TArray<FTransform> _Vehicle_ComponentSpaceTransformsArray;
		FTransform _Vehicle_ComponentToWorld;
		std::vector<FTransform> _Vehicle_BoneTransFormTable;
		std::vector<FVector> _Vehicle_BoneTable;
		std::vector<FVector> _Player_BoneTable;

		int32_t _LastVehiclePawnEncryptIndex;
		int	  _SeatIndex;
		FVector _LinearVelocity;

		ptr_t _EncryptCharacterMovement;
		void* _CharacterMovement;
		FVector _Velocity;
		FFindFloorResult _CurrentFloor;
		FUObjectItem _CurrentFloor_HitResult_ComponentObject;
		void* _CurrentFloor_HitResult_Component;
		uint32_t _CurrentFloor_HitResult_Component_BodyInstance_bOverrideWalkableSlopeOnInstance;
		FWalkableSlopeOverride _CurrentFloor_HitResult_Component_BodyInstance_WalkableSlopeOverride;
		TWeakObjectPtr<class UBodySetup> _CurrentFloor_HitResult_Component_BodyInstance_BodySetup;
		FUObjectItem _CurrentFloor_HitResult_Component_BodyInstance_BodySetupObject;
		UCharacterMovementComponent_BIT4 _bMaintainHorizontalGroundVelocity;
		int _StanceMode;
		float _WalkableFloorZ;

		void* _WeaponProcess;

		ptr_t _EncryptItem;
		void* _Item;
		void* _ItemTableRow;
		float _Durability;
		float _DurabilityMax;
		FName _ItemID;
		std::string _ItemName;

		TArray<class UItem*> _Items;
		uint32_t _ItemsNum;
		ItemInfoArrayData _ArrayData;

		FVector _CacheLocation;
		FVector _Location;
		FVector _delta;
		float _dist;

		float _TimeTillExplosion;
		float _ExplosionDelay;
		DWORD _ExistExplorTime;
		float _LogTimeTillExplosion;
		bool _bProjectleValid;
		FProjectileData _ProjectileConfig;
		std::string _ProjectileName;
		//DroppedItem
		TSet_Components _OwnedComponents;
		std::vector<digaStruct>_digaArray;
		std::vector<ItemGroupStruct>_ItemArray;
		bool _ValidItem;
		ptr_t _NextReadPosTime;
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class AInfo : public AActor
	{
	public:
		AInfo(const UObject& object) :AActor(object) {}
		AInfo(void* object) :AActor(object) {}
		AInfo(uint64_t object) :AInfo(reinterpret_cast<void*>(object)) {}
	public:
		static UClass StaticClass();
	public:
		auto operator->() { return this; }
	};


	class AWorldSettings :public AInfo
	{
	public:
		AWorldSettings(const UObject& object) :AInfo(object) {}
		AWorldSettings(void* object) :AInfo(object) {}
		AWorldSettings(uint64_t object) :AWorldSettings(reinterpret_cast<void*>(object)) {}
	public:

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};


	class AGameMode :public AInfo
	{
	public:
		AGameMode(const UObject& object) :AInfo(object) {}
		AGameMode(void* object) :AInfo(object) {}
		AGameMode(uint64_t object) :AGameMode(reinterpret_cast<void*>(object)) {}
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ATslGameMode :public AGameMode
	{
	public:
		ATslGameMode(const UObject& object) :AGameMode(object) {}
		ATslGameMode(void* object) :AGameMode(object) {}
		ATslGameMode(uint64_t object) :ATslGameMode(reinterpret_cast<void*>(object)) {}
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class AGameState :public AInfo
	{
	public:
		AGameState(const UObject& object) :AInfo(object) {}
		AGameState(void* object) :AInfo(object) {}
		AGameState(uint64_t object) :AGameState(reinterpret_cast<void*>(object)) {}
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ATslGameState :public AGameState
	{
	public:
		ATslGameState(const UObject& object) :AGameState(object) {}
		ATslGameState(void* object) :AGameState(object) {}
		ATslGameState(uint64_t object) :ATslGameState(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//C7 45 ? 03 00 00 00 8B ? ? ? 00 00 48 8D 4D ? E8 + 7
		//8B B3 ? ? 00 00 44 8B ? ? ? 00 00 44 8B ? ? ? 00 00 48 8d 8d ? ? 00 00 + 6
		MAKE_GENERAL_PROPERTY(int, NumAlivePlayers, "IntProperty TslGame.TslGameState.NumAlivePlayers");
		MAKE_GENERAL_PROPERTY(int, NumAliveTeams, "IntProperty TslGame.TslGameState.NumAliveTeams");

		MAKE_GENERAL_PROPERTY(float, PoisonGasWarningRadius, "FloatProperty TslGame.TslGameState.PoisonGasWarningRadius");
		MAKE_GENERAL_PROPERTY(FVector, PoisonGasWarningPosition, "StructProperty TslGame.TslGameState.PoisonGasWarningPosition");
		MAKE_GENERAL_PROPERTY(float, RedZoneRadius, "FloatProperty TslGame.TslGameState.RedZoneRadius");
		MAKE_GENERAL_PROPERTY(FVector, RedZonePosition, "StructProperty TslGame.TslGameState.RedZonePosition");
		MAKE_GENERAL_PROPERTY(float, BlackZoneRadius, "FloatProperty TslGame.TslGameState.BlackZoneRadius");
		MAKE_GENERAL_PROPERTY(FVector, BlackZonePosition, "StructProperty TslGame.TslGameState.BlackZonePosition");
		MAKE_GENERAL_PROPERTY(float, SafetyZoneRadius, "FloatProperty TslGame.TslGameState.SafetyZoneRadius");
		MAKE_GENERAL_PROPERTY(FVector, SafetyZonePosition, "StructProperty TslGame.TslGameState.SafetyZonePosition");
#else
		MAKE_GENERAL_PROPERTY(int, NumAliveTeams, "IntProperty TslGame.TslGameState.NumAliveTeams", g_PatternData.offset_ATslGameState_NumAliveTeams);
		MAKE_GENERAL_PROPERTY(FVector, LerpSafetyZonePosition, "StructProperty TslGame.TslGameState.LerpSafetyZonePosition", g_PatternData.offset_ATslGameState_LerpSafetyZonePosition);
		MAKE_GENERAL_PROPERTY(float, LerpSafetyZoneRadius, "FloatProperty TslGame.TslGameState.LerpSafetyZoneRadius", g_PatternData.offset_ATslGameState_LerpSafetyZoneRadius);
		MAKE_GENERAL_PROPERTY(FVector, LerpBlueZonePosition, "StructProperty TslGame.TslGameState.LerpBlueZonePosition", g_PatternData.offset_ATslGameState_LerpBlueZonePosition);
		MAKE_GENERAL_PROPERTY(float, LerpBlueZoneRadius, "FloatProperty TslGame.TslGameState.LerpBlueZoneRadius", g_PatternData.offset_ATslGameState_LerpBlueZoneRadius);
		MAKE_GENERAL_PROPERTY(FVector, RedZonePosition, "StructProperty TslGame.TslGameState.RedZonePosition", g_PatternData.offset_ATslGameState_RedZonePosition);
		MAKE_GENERAL_PROPERTY(float, RedZoneRadius, "FloatProperty TslGame.TslGameState.RedZoneRadius", g_PatternData.offset_ATslGameState_RedZoneRadius);
		MAKE_GENERAL_PROPERTY(FVector, BlackZonePosition, "StructProperty TslGame.TslGameState.BlackZonePosition", g_PatternData.offset_ATslGameState_BlackZonePosition);
		MAKE_GENERAL_PROPERTY(float, BlackZoneRadius, "FloatProperty TslGame.TslGameState.BlackZoneRadius", g_PatternData.offset_ATslGameState_BlackZoneRadius);
		MAKE_GENERAL_PROPERTY(FVector, SafetyZonePosition, "StructProperty TslGame.TslGameState.SafetyZonePosition", g_PatternData.offset_ATslGameState_SafetyZonePosition);
		MAKE_GENERAL_PROPERTY(float, SafetyZoneRadius, "FloatProperty TslGame.TslGameState.SafetyZoneRadius", g_PatternData.offset_ATslGameState_SafetyZoneRadius);
#endif
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class AController : public AActor
	{
	public:
		AController(const UObject& object) :AActor(object), Pawn(object.GetPtr(), g_PatternData.offset_AController_Pawn) {}
		AController(void* object)
			:AActor(object)
			, Pawn(object, g_PatternData.offset_AController_Pawn)
		{}
		AController(uint64_t object) :AController(reinterpret_cast<void*>(object)) {}
	public:
		//C6 84 24 ? ? 00 00 01 33 ? 48 ? ? 0F 84 ? ? ? ? ? 8b + 0x13
		//48 ? ? 24 ? ? 00 00 E8 ? ? ? ? ? 8B ? ? ? 00 00 33 ? 48 39 + 0xD
		//0F 84 ? ? ? ? E8 ? ? ? ? 48 39 ? ? ? ? ? ? 8B ? ? 8B ? ? ? 00 00 + 0x15
		UEncryptedObjectProperty<class APawn, DecryptFunc::General> Pawn;
		//Playing
		MAKE_STRUCT_PROPERTY(struct FName*, StateName, "NameProperty Engine.Controller.StateName", g_PatternData.offset_AController_StateName);
#ifndef ObjectNameHash
		//83 ? ? ? 00 00 00 0F 84 ? ? ? ? F3 0F 10 ? ? ? 00 00 0F 2E 83 ? ? 00 00 0F 85 + 0xD
		//0F 57 ? F3 0F 10 ? ? ? 00 00 F3 0F 10 ? ? ? 00 00 F3 0F 10 5C 24 + 0xB
		MAKE_STRUCT_PROPERTY(struct FRotator, ControlRotation, "StructProperty Engine.Controller.ControlRotation");
#else
		MAKE_STRUCT_PROPERTY(struct FRotator, ControlRotation, "StructProperty Engine.Controller.*79d43be664", g_PatternData.offset_AController_ControlRotation);
#endif
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class APawn : public AActor
	{
	public:
		APawn(const UObject& object) :AActor(object), PlayerState(object.GetPtr(), g_PatternData.offset_APawn_PlayerState) {}
		APawn(void* object) :AActor(object), PlayerState(object, g_PatternData.offset_APawn_PlayerState) {}
		APawn(uint64_t object) :APawn(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//E8 ? ? ? ? ? 3B ? 0F 94 85 ? ? 00 00 48 {0,1,5} GetPlayerState Call
		//8B ? 48 85 ? 74 ? ? 39 ? ? ? ? ? ? 8B ? ? ? 00 00 48 ? 44 24 ? 75 ? ? 8B + 0xE
		UEncryptedObjectProperty<class APlayerState, DecryptFunc::General, "EncryptedObjectProperty Engine.Pawn.PlayerState"_hash> PlayerState;
		//73 ? C6 81 ? ? 00 00 00 48 8B ? ? ? 00 00 48 85 + 9
		//83 ? ? ? 00 00 01 0f 85 ? ? ? ? 48 8B ? ? ? 00 00 48 85 ? 74 + 0xD
		//48 39 ? ? ? 00 00 0f 85 ? ? ? ? F7 87 ? ? 00 00 00 01 00 00 + 0
		//48 3B ? ? ? 00 00 75 ? 83 89 ? ? 00 00 10 C3 + 0
		MAKE_OBJECT_PROPERTY(class AController, Controller, "ObjectProperty Engine.Pawn.Controller");
		//48 33 C8 E8 ? ? ? ? 8B 88 ? ? 00 00 89 8B ? ? 00 00 48 83 C4 20 5B C3 + 8
		//48 8B 01 48 8B D9 0F 29 74 24 30 0F 28 F2 - call qword ptr [rax+000006E8]
		MAKE_GENERAL_PROPERTY(float, BaseEyeHeight, "FloatProperty Engine.Pawn.BaseEyeHeight");
#else
		UEncryptedObjectProperty<class APlayerState, DecryptFunc::General, "EncryptedObjectProperty Engine.Pawn.PlayerState"_hash> PlayerState;
		MAKE_OBJECT_PROPERTY(class AController, Controller, "ObjectProperty Engine.Pawn.Controller", g_PatternData.offset_APawn_Controller);
		MAKE_GENERAL_PROPERTY(float, BaseEyeHeight, "FloatProperty Engine.Pawn.BaseEyeHeight", g_PatternData.offset_APawn_BaseEyeHeight);
#endif
	public:
		static UClass StaticClass();
	public:
		auto operator->() { return this; }
	};

	class ADefaultPawn : public APawn
	{
	public:
		ADefaultPawn(const UObject& object) :APawn(object) {}
		ADefaultPawn(void* object) :APawn(object) {}
		ADefaultPawn(uint64_t object) :ADefaultPawn(reinterpret_cast<void*>(object)) {}
	public:
		static UClass StaticClass();
	public:
		auto operator->() { return this; }
	};

	class APlayerCameraManager : public AActor
	{
	public:
		APlayerCameraManager(const UObject& object) :AActor(object) {}
		APlayerCameraManager(void* object) :AActor(object) {}
		APlayerCameraManager(uint64_t object) :APlayerCameraManager(reinterpret_cast<void*>(object)) {}

	public:
#ifndef ObjectNameHash
		//48 83 3B 00 75 ? 48 8B ? ? ? 00 00 48 8D ? ? ? 00 00 48 8B ? E8 + 6
		//48 8B ? ? ? 00 00 ? 8B ? 0F 29 ? ? ? 8B ? 0F 29 ? ? 0F + 0
		//8B ? ? ? 00 00 8B ? ? ? 00 00 D1 ? ? 01 74 + 0
		MAKE_STRUCT_PROPERTY(ptr_t*, PCOwner, "ObjectProperty Engine.PlayerCameraManager.PCOwner");
#else
		MAKE_STRUCT_PROPERTY(ptr_t*, PCOwner, "ObjectProperty Engine.PlayerCameraManager.*0cacdaf2a1", g_PatternData.offset_APlayerCameraManager_PCOwner);
#endif

		ptr_t GetViewTarget_POV_FOV();
		ptr_t GetViewTarget_POV_Location();
		ptr_t GetViewTarget_POV_Rotation();

		float GetCameraCache_Timestamp();
		float GetCameraCache_POV_FOV();
		FVector GetCameraCache_POV_Location();
		FRotator GetCameraCache_POV_Rotation();

		/**
		* Master function to retrieve Camera's actual view point.
		* Consider calling PlayerController::GetPlayerViewPoint() instead.
		*
		* @param	OutCamLoc	Returned camera location
		* @param	OutCamRot	Returned camera rotation
		*/
		void GetCameraViewPoint(FVector& OutCamLoc, FRotator& OutCamRot);
	public:
#ifndef ObjectNameHash
		//48 8B 93 ? ? ? ? 0F 28 D6 ? 89 74 24 + 0
		//48 8B ? ? ? 00 00 0F 28 ? 48 89 ? 24 ? ? 8B ? ? 48 8B 01 FF 90 + 0
		MAKE_STRUCT_PROPERTY(ptr_t*, ViewTarget, "StructProperty Engine.PlayerCameraManager.ViewTarget");
		//0F 57 C0 0F 2F ? ? ? 00 00 73 ? F2 0F 10 ? ? ? 00 00 + 3
		//FF 90 30 01 00 00 F3 0F 10 ? ? ? 00 00 48 8D ? ? ? 00 00 0f 2e ? ? ? 00 00 + 6
		MAKE_STRUCT_PROPERTY(ptr_t*, CameraCache, "StructProperty Engine.PlayerCameraManager.CameraCache");
#else
		MAKE_STRUCT_PROPERTY(ptr_t*, ViewTarget, "StructProperty Engine.PlayerCameraManager.ViewTarget", g_PatternData.offset_APlayerCameraManager_ViewTarget);
		MAKE_STRUCT_PROPERTY(ptr_t*, CameraCache, "StructProperty Engine.PlayerCameraManager.*28bf51f41b", g_PatternData.offset_APlayerCameraManager_CameraCache);

		MAKE_GENERAL_PROPERTY(float, MinimalViewInfo_FOV, "FloatProperty Engine.*fef0e66e96.FOV", g_PatternData.offset_MinimalViewInfo_FOV);
		MAKE_GENERAL_PROPERTY(float, MinimalViewInfo_Location, "StructProperty Engine.*fef0e66e96.Location", g_PatternData.offset_MinimalViewInfo_Location);
		MAKE_GENERAL_PROPERTY(float, MinimalViewInfo_Rotation, "StructProperty Engine.*fef0e66e96.Rotation", g_PatternData.offset_MinimalViewInfo_Rotation);
		MAKE_GENERAL_PROPERTY(float, TViewTarget_POV, "StructProperty Engine.*29ed580470.POV", g_PatternData.offset_TViewTarget_POV);
		//这里是偏移的
		MAKE_GENERAL_PROPERTY(float, CameraCacheEntry_Timestamp, "FloatProperty Engine.*02a8d27e6c.Timestamp", g_PatternData.offset_CameraCacheEntry_Timestamp);
		MAKE_GENERAL_PROPERTY(float, CameraCacheEntry_POV, "StructProperty Engine.*02a8d27e6c.*2a95b455f1", g_PatternData.offset_CameraCacheEntry_POV);

#endif


		ptr_t GetViewTarget_POV();
		ptr_t GetCameraCache_POV();

		uint32_t GetPOV_FOVOffset();
		uint32_t GetPOV_LocationOffset();
		uint32_t GetPOV_RotationOffset();


	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class AHUD : public AActor
	{
	public:
		AHUD(const UObject& object) :AActor(object) {}
		AHUD(void* object) :AActor(object) {}
		AHUD(uint64_t object) :AHUD(reinterpret_cast<void*>(object)) {}
	public:

#ifndef ObjectNameHash
		//C7 44 24 ? 01 00 00 00 ? 8B ? ? ? 00 00 48 63 ? ? ? 00 00 ? 8D 34 + 8

		//80 ? ? ? 00 00 00 74 ? 48 8B ? 48 8B ? E8 ? ? ? ? 48 8B ? 48 8B ? E8 ? ? ? ? 48 8B ? 24 + 0x1A,1,5] 第一个lea
		//80 ? ? ? 00 00 00 74 ? 48 ? ? ? ? 00 00 48 8B ? E8 ? ? ? ? 48 8B ? 48 8B ? E8 ? ? ? ? 48 8B ? 24 + 0x1E,1,5] 第一个lea
		//80 ? ? ? 00 00 00 74 ? ? 8D ? ? ? 00 00 48 89 ? 24 ? 48 8D ? 24 ? E8 ? ? ? ? 48 8B ? 48 8B ? E8 + 0x25,1,5] 第一个lea

		MAKE_GENERAL_PROPERTY(TArray<class UBlockInputUserWidget>, BlockInputWidgetList, "ArrayProperty TslGame.TslBaseHUD.BlockInputWidgetList");
		//48 ? ? ? ? 00 00 48 8B ? ? ? 00 00 48 8B 01 FF 90 ? ? 00 00 48 8B 44 24 ? ? ? ? ? ? 00 00 ? ? ? ? ? 00 00 + 0
		//FF 90 ? ? 00 00 ? 8B ? ? ? 00 00 ? 8B ? E8 ? ? ? ? ? 8B ? ? 8B ? FF + 6
		//48 ? ? ? ? 00 00 48 ? ? ? ? 00 00 48 8B ? ? ? 00 00 ? 8B ? FF 90 ? ? 00 00 48 8b ? ? ? 89 + 7
		MAKE_OBJECT_PROPERTY(UCanvas, DebugCanvas, "ObjectProperty Engine.HUD.DebugCanvas");
		//48 ? ? ? ? 00 00 48 ? ? ? ? 00 00 48 8B ? ? ? 00 00 48 8B 01 FF 90 ? ? 00 00 48 8B 44 24 ? ? ? ? ? ? 00 00 ? ? ? ? ? 00 00 + 0
		//48 ? ? ? ? 00 00 48 ? ? ? ? 00 00 48 8B ? ? ? 00 00 ? 8B ? FF 90 ? ? 00 00 48 8b ? ? ? 89 + 0
		MAKE_OBJECT_PROPERTY(UCanvas, Canvas, "ObjectProperty Engine.HUD.Canvas");
#else
		MAKE_GENERAL_PROPERTY(TArray<class UBlockInputUserWidget>, BlockInputWidgetList, "ArrayProperty TslGame.TslBaseHUD.*a1afe7c295", g_PatternData.offset_AHUD_BlockInputWidgetList);
		MAKE_GENERAL_PROPERTY(ptr_t, WidgetStateMap, "MapProperty TslGame.TslBaseHUD.*2962d70f4d", g_PatternData.offset_AHUD_WidgetStateMap);
		MAKE_OBJECT_PROPERTY(UCanvas, DebugCanvas, "ObjectProperty Engine.HUD.*7f111f08fd", g_PatternData.offset_AHUD_DebugCanvas);
		MAKE_OBJECT_PROPERTY(UCanvas, Canvas, "ObjectProperty Engine.HUD.Canvas", g_PatternData.offset_AHUD_Canvas);
#endif
	public:
		//void DrawLine(float StartScreenX, float StartScreenY, float EndScreenX, float EndScreenY, FLinearColor LineColor, float LineThickness);
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UGameViewportClient : public UObject
	{
	public:
		UGameViewportClient(const UObject& object) :UObject(object) {}
		UGameViewportClient(void* object) :UObject(object) {}
		UGameViewportClient(uint64_t object) :UGameViewportClient(reinterpret_cast<void*>(object)) {}

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UActorComponent : public UObject
	{
	public:
		UActorComponent(const UObject& object) :UObject(object), Owner(object.GetPtr(), g_PatternData.offset_OwnedActorComponent), _RelativeLocation(0, 0, 0), _GroupItemInfo(ItemInfoData()), _Out(false) {}
		UActorComponent(void* object) :UObject(object), Owner(object, g_PatternData.offset_OwnedActorComponent), _RelativeLocation(0, 0, 0), _GroupItemInfo(ItemInfoData()), _Out(false) {}
		UActorComponent(uint64_t object) :UActorComponent(reinterpret_cast<void*>(object)) {}
	public:
		UEncryptedObjectProperty<AActor, DecryptFunc::General> Owner;
#ifndef ObjectNameHash
		//F6 81 ? ? 00 00 01 48 8B ? 74 ? ? 8D ? ? ? 00 00 E8 + 0
		//8B ? ? ? 00 00 C1 ? 0E 40 80 ? 01 48 85 ? 0F 84 + 0
		//FF 90 ? ? 00 00 F6 ? ? ? 00 00 01 74 ? 48 8B ? ? ? 00 00 41 + 6
		//F6 81 ? ? 00 00 01 48 8B ? 75 ? 0F 28 + 0
		MAKE_GENERAL_PROPERTY(char, bRegistered, "BoolProperty Engine.ActorComponent.bReplicates");
#else
		MAKE_GENERAL_PROPERTY(char, bRegistered, "BoolProperty Engine.ActorComponent.bReplicates", g_PatternData.offset_UActorComponent_bRegistered);
#endif
		/** See if this component is currently registered */
		FORCEINLINE bool IsRegistered() { return bRegistered; }
	public:
		FVector _RelativeLocation;
		ItemInfoData _GroupItemInfo;
		bool _Out;
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UTslVehicleCommonComponent : public UActorComponent
	{
	public:
		UTslVehicleCommonComponent(const UObject& object) :UActorComponent(object) {}
		UTslVehicleCommonComponent(void* object) :UActorComponent(object) {}
		UTslVehicleCommonComponent(uint64_t object) :UTslVehicleCommonComponent(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//0F 54 ? ? ? ? ? 0F 2F ? ? ? ? ? ? ? F3 0F 10 ? ? ? 00 00 F3 0F 5E ? F3 0F 59 燃料 血量

		//F3 0F 10 ? ? ? 00 00 F3 0F 11 ? 24 ? F3 0F 10 ? ? ? 00 00 F3 0F 11 ? 24 ? E8 ? ? ? ? 48 8B 4C + 0xE
		//E8 ? ? ? ? 48 8B ? 0F 28 ? 0F 28 ? 48 8B ? FF 90 ? ? 00 00 + 0,1,5] call里面第一个movss F3 0F 10
		MAKE_GENERAL_PROPERTY(float, FuelMax, "FloatProperty TslGame.TslVehicleCommonComponent.FuelMax");
		//F3 0F 10 ? ? ? 00 00 F3 0F 11 ? 24 ? F3 0F 10 ? ? ? 00 00 F3 0F 11 ? 24 ? E8 ? ? ? ? 48 8B 4C + 0
		//0F 2F ? ? ? 00 00 73 ? F6 ? ? ? 00 00 02 75 + 0
		//E8 ? ? ? ? 48 8B ? 0F 28 ? E8 ? ? ? ? 48 8B ? 0F 28 ? 0F 28 ? 48 8B ? FF 90 ? ? 00 00 + 0,1,5] call里面第一个movss F3 0F 10
		MAKE_GENERAL_PROPERTY(float, Fuel, "FloatProperty TslGame.TslVehicleCommonComponent.Fuel");
		//Health + 4
		//48 8B ? 48 8B ? FF 92 ? ? 00 00 48 8B ? 48 8B ? 0F 28 ? FF 92 ? ? 00 00 48 8B + 0xF]虚表Call

		MAKE_GENERAL_PROPERTY(float, HealthMax, "FloatProperty TslGame.TslVehicleCommonComponent.HealthMax");
		//48 8B ? 48 8B ? FF 92 ? ? 00 00 48 8B ? 48 8B ? 0F 28 ? FF 92 ? ? 00 00 48 8B + 6 ] 虚表Call

		//0F 57 ? 0F 2F ? ? ? 00 00 0F 83 ? ? ? ? 32 C0 + 3
		//0F 84 ? ? ? ? 48 8B ? E8 ? ? ? ? 48 8B ? 0F 28 ? E8 ? ? ? ? C6
		MAKE_GENERAL_PROPERTY(float, Health, "FloatProperty TslGame.TslVehicleCommonComponent.Health");
#else
		MAKE_GENERAL_PROPERTY(float, FuelMax, "FloatProperty TslGame.*0fb9c84a03.*803c82c973", g_PatternData.offset_UTslVehicleCommonComponent_FuelMax);
		MAKE_GENERAL_PROPERTY(float, Fuel, "FloatProperty TslGame.*0fb9c84a03.Fuel", g_PatternData.offset_UTslVehicleCommonComponent_Fuel);
		MAKE_GENERAL_PROPERTY(float, HealthMax, "FloatProperty TslGame.*0fb9c84a03.HealthMax", g_PatternData.offset_UTslVehicleCommonComponent_HealthMax);
		MAKE_GENERAL_PROPERTY(float, Health, "FloatProperty TslGame.*0fb9c84a03.Health", g_PatternData.offset_UTslVehicleCommonComponent_Health);

#endif

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class USceneComponent : public UActorComponent
	{
	public:
		USceneComponent(const UObject& object) :UActorComponent(object) {}
		USceneComponent(void* object) :UActorComponent(object) {}
		USceneComponent(uint64_t object) :USceneComponent(reinterpret_cast<void*>(object)) {}
#ifndef ObjectNameHash
		//F2 0F 10 ? ? ? 00 00 F2 0F 11 03 8B ? ? ? 00 00 89 43 08 EB + 0
		//75 ? F2 0F 10 ? ? ? 00 00 F2 0F 11 03 8B ? ? ? 00 00 89 43 08 + 2
		MAKE_GENERAL_PROPERTY(FVector, ComponentVelocity, "StructProperty Engine.SceneComponent.ComponentVelocity");
		//83 ? 08 89 ? ? ? 00 00 F3 0F 10 ? ? ? 00 00 + 9] - 8
		MAKE_GENERAL_PROPERTY(FVector, RelativeLocation, "StructProperty Engine.SceneComponent.RelativeLocation");
		//0F 11 57 40 ? 8B ? ? ? 00 00 F3 0F 10 ? ? ? 00 00 + 0xB
		//0F 11 57 40 ? 8B ? ? ? 00 00 ? 8D ? ? ? 00 00 F3 41 0F 10 ? ? ? 00 00 + 0x12
		//0F 11 57 40 ? 8B ? ? ? 00 00 ? 8D ? ? ? 00 00 F3 0F 10 ? ? ? 00 00 + 0x12
		MAKE_GENERAL_PROPERTY(FRotator, RelativeRotation, "StructProperty Engine.SceneComponent.RelativeRotation");
#else
		MAKE_GENERAL_PROPERTY(FVector, ComponentVelocity, "StructProperty Engine.SceneComponent.*985336bfd7", g_PatternData.offset_USceneComponent_ComponentVelocity);
		MAKE_GENERAL_PROPERTY(FVector, RelativeLocation, "StructProperty Engine.SceneComponent.RelativeLocation", g_PatternData.offset_USceneComponent_RelativeLocation);
		MAKE_GENERAL_PROPERTY(FRotator, RelativeRotation, "StructProperty Engine.SceneComponent.RelativeRotation", g_PatternData.offset_USceneComponent_RelativeRotation);
		MAKE_OBJECT_PROPERTY(USceneComponent, AttachParent, "ObjectProperty Engine.SceneComponent.AttachParent", g_PatternData.offset_USceneComponent_AttachParent);
#endif

	public:
		FVector GetLocation();

		/** Get the current component-to-world transform for this component */
		FORCEINLINE FTransform GetComponentTransform()
		{
			return GetComponentToWorld();
		}

		/**
		 * Get the current component-to-world transform for this component
		 * TODO: probably deprecate this in favor of GetComponentTransform
		 */
		struct FTransform GetComponentToWorld() const
		{
			return GetDMA().Read<FTransform>((ptr_t)GetPtr() + g_PatternData.offset_ComponentToWorld);
		}

		virtual FTransform GetSocketTransform(FName SocketName, ERelativeTransformSpace TransformSpace = RTS_World);
		virtual FVector GetSocketLocation(FName InSocketName);
		virtual FRotator GetSocketRotation(FName InSocketName);
		virtual FQuat GetSocketQuaternion(FName InSocketName);

		//FTransform K2_GetComponentToWorld();
		//FVector K2_GetComponentScale();
		//FRotator K2_GetComponentRotation();
		//FVector K2_GetComponentLocation();
		//FVector GetUpVector();
		//FVector GetRightVector();
		//FTransform GetRelativeTransform();
		//FVector GetForwardVector();
		//FVector FnGetComponentVelocity();
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class UPrimitiveComponent : public USceneComponent
	{
	public:
		UPrimitiveComponent(const UObject& object) :USceneComponent(object) {}
		UPrimitiveComponent(void* object) :USceneComponent(object) {}
		UPrimitiveComponent(uint64_t object) :UPrimitiveComponent(reinterpret_cast<void*>(object)) {}

	public:
#ifndef ObjectNameHash
		//MAKE_GENERAL_PROPERTY(int, iCustomDepthStencilValue, "ByteProperty Engine.PrimitiveComponent.CustomDepthStencilValue");
		//LastRenderTime + 4
		MAKE_GENERAL_PROPERTY(float, LastRenderTimeOnScreen, "FloatProperty Engine.PrimitiveComponent.LastRenderTimeOnScreen");
		//F3 0F ? ? ? ? 00 00 F3 0F 10 4C 24 ?? 0F 2F C8 0F 93 C0 45 33 + 0
		MAKE_GENERAL_PROPERTY(float, LastRenderTime, "FloatProperty Engine.PrimitiveComponent.LastRenderTime");
		//LastRenderTime - 4
		//F3 0F 10 ? ? ? 00 00 0F 28 ? F3 0F 5C ? 0F 2F 05 ? ? ? ? 72 + 0
		MAKE_GENERAL_PROPERTY(float, LastSubmitTime, "FloatProperty Engine.PrimitiveComponent.LastSubmitTime");
		//8B ? ? ? 00 00 8A ? ? ? 00 00 C1 ? 06 + 0
		MAKE_GENERAL_PROPERTY(unsigned char, bRenderCustomDepth, "BoolProperty Engine.PrimitiveComponent.bRenderCustomDepth");
		//45 84 ? 74 ? 48 8B ? ? ? 00 00 48 85 ? 75 ? 48 8D ? ? ? 00 00 C3 + 0x11 或 第一个lea指令
		MAKE_STRUCT_PROPERTY(struct FBodyInstance, BodyInstance, "StructProperty Engine.PrimitiveComponent.BodyInstance");//0x4F0
#else
		//MAKE_GENERAL_PROPERTY(int, iCustomDepthStencilValue, "ByteProperty Engine.PrimitiveComponent.CustomDepthStencilValue");
		MAKE_GENERAL_PROPERTY(float, LastRenderTimeOnScreen, "FloatProperty Engine.PrimitiveComponent.*73f7b71c01", g_PatternData.offset_UPrimitiveComponent_LastRenderTimeOnScreen);
		MAKE_GENERAL_PROPERTY(float, LastRenderTime, "FloatProperty Engine.PrimitiveComponent.*6c5a28aafc", g_PatternData.offset_UPrimitiveComponent_LastRenderTime);
		MAKE_GENERAL_PROPERTY(float, LastSubmitTime, "FloatProperty Engine.PrimitiveComponent.*fb334d2482", g_PatternData.offset_UPrimitiveComponent_LastSubmitTime);
		MAKE_GENERAL_PROPERTY(unsigned char, bRenderCustomDepth, "BoolProperty Engine.PrimitiveComponent.bRenderCustomDepth", g_PatternData.offset_UPrimitiveComponent_bRenderCustomDepth);
		MAKE_STRUCT_PROPERTY(struct FBodyInstance, BodyInstance, "StructProperty Engine.PrimitiveComponent.BodyInstance", g_PatternData.offset_UPrimitiveComponent_BodyInstance);
#endif
		struct FWalkableSlopeOverride GetWalkableSlopeOverride();

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UMeshComponent : public UPrimitiveComponent
	{
	public:
		UMeshComponent(const UObject& object) :UPrimitiveComponent(object) {}
		UMeshComponent(void* object) :UPrimitiveComponent(object) {}
		UMeshComponent(uint64_t object) :UMeshComponent(reinterpret_cast<void*>(object)) {}
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class USkinnedMeshComponent : public UMeshComponent
	{
	public:
		USkinnedMeshComponent(const UObject& object) :UMeshComponent(object) {}
		USkinnedMeshComponent(void* object) :UMeshComponent(object) {}
		USkinnedMeshComponent(uint64_t object) :USkinnedMeshComponent(reinterpret_cast<void*>(object)) {}

	public:
#ifndef ObjectNameHash
		//48 39 ? ? ? 00 00 74 ? F6 ? ? ? 00 00 05 + 0
		//48 8B ? ? ? 00 00 48 85 ? 0F 84 ? ? ? ? 83 CE FF + 0
		//48 83 ? ? ? 00 00 00 ? ? ? 0F 29 ? ? ? 8B ? 0F 29 ? ? 0F 28 ? 0F 84 + 0
		MAKE_OBJECT_PROPERTY(USkeletalMesh, SkeletalMesh, "ObjectProperty Engine.SkinnedMeshComponent.SkeletalMesh");
		//C7 05 ? ? ? ? ? ? ? ? 83 ? ? ? 00 00 00 0F 84 ? ? ? ? ? 63 ? ? ? 00 00 ? 85 ? 0F 88 + 0x17
		MAKE_STRUCT_PROPERTY(TWeakObjectPtr<class USkinnedMeshComponent>, MasterPoseComponent, "WeakObjectProperty Engine.SkinnedMeshComponent.MasterPoseComponent");
		MAKE_EXTRA_GENERAL_PROPERTY(int32_t, CurrentEditableComponentTransforms, "WeakObjectProperty Engine.SkinnedMeshComponent.MasterPoseComponent", 0x28);
		MAKE_EXTRA_GENERAL_PROPERTY(int32_t, CurrentReadComponentTransforms, "WeakObjectProperty Engine.SkinnedMeshComponent.MasterPoseComponent", 0x2C);
		MAKE_EXTRA_GENERAL_PROPERTY(TArray<int32_t>, MasterBoneMap, "BoolProperty Engine.SkinnedMeshComponent.bUseBoundsFromMasterPoseComponent", -0x20);
#else
		MAKE_OBJECT_PROPERTY(USkeletalMesh, SkeletalMesh, "ObjectProperty Engine.SkinnedMeshComponent.SkeletalMesh", g_PatternData.offset_USkinnedMeshComponent_SkeletalMesh);
		MAKE_STRUCT_PROPERTY(TWeakObjectPtr<class USkinnedMeshComponent>, MasterPoseComponent, "WeakObjectProperty Engine.SkinnedMeshComponent.MasterPoseComponent", g_PatternData.offset_USkinnedMeshComponent_MasterPoseComponent);
		MAKE_GENERAL_PROPERTY(TArray<int32_t>, MasterBoneMap, "BoolProperty Engine.SkinnedMeshComponent.bUseBoundsFromMasterPoseComponent", g_PatternData.offset_USkinnedMeshComponent_MasterBoneMap);
		MAKE_GENERAL_PROPERTY(int32_t, CurrentEditableComponentTransforms, "WeakObjectProperty Engine.SkinnedMeshComponent.MasterPoseComponent", g_PatternData.offset_USkinnedMeshComponent_CurrentEditableComponentTransforms);
		MAKE_GENERAL_PROPERTY(int32_t, CurrentReadComponentTransforms, "WeakObjectProperty Engine.SkinnedMeshComponent.MasterPoseComponent", g_PatternData.offset_USkinnedMeshComponent_CurrentReadComponentTransforms);
#endif



		TArray<FTransform> GetComponentSpaceTransformsArray(int32_t idx);


		/** Access ComponentSpaceTransforms for reading */
		TArray<FTransform> GetComponentSpaceTransforms()
		{
			return GetComponentSpaceTransformsArray(CurrentReadComponentTransforms);
		}

		/** Get current number of component space transorms */
		int32_t GetNumComponentSpaceTransforms()
		{
			return GetComponentSpaceTransforms().Num();
		}


		/**
		 * Get world space bone transform from bone index, also specifying the component transform to use
		 *
		 * @param BoneIndex Index of the bone
		 *
		 * @return the transform of the bone at the specified index
		 */
		FTransform GetBoneTransform(int32_t BoneIndex, const FTransform& LocalToWorld);

		/**
		 * Get Bone Transform from index
		 *
		 * @param BoneIndex Index of the bone
		 *
		 * @return the transform of the bone at the specified index
		 */
		FTransform GetBoneTransform(int32_t BoneIndex);

		virtual FTransform GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace = RTS_World) override;

		FTransform GetSocketTransform2(int32_t BoneIndex, FTransform GunFT, FTransform SocketLocalTransform);

		/**
		 * Find the index of bone by name. Looks in the current SkeletalMesh being used by this SkeletalMeshComponent.
		 *
		 * @param BoneName Name of bone to look up
		 *
		 * @return Index of the named bone in the current SkeletalMesh. Will return INDEX_NONE if bone not found.
		 *
		 * @see USkeletalMesh::GetBoneIndex.
		 */
		int32_t GetBoneIndex(FName BoneName);

		/**
		 * @return SkeletalMeshSocket of named socket on the skeletal mesh component, or NULL if not found.
		 */
		USkeletalMeshSocket GetSocketByName(FName InSocketName);

		/**
		 * Get Parent Bone of the input bone
		 *
		 * @param BoneName Name of the bone
		 *
		 * @return the name of the parent bone for the specified bone. Returns 'None' if the bone does not exist or it is the root bone
		 */

		FName GetParentBone(FName BoneName);


		FName GetBoneNameByIndex(int32_t Index);

	public:
		auto operator->() { return this; }
		static UClass StaticClass();

	};

	class UAnimInstance : public UObject
	{
	public:
		UAnimInstance(const UObject& object) :UObject(object) {}
		UAnimInstance(void* object) :UObject(object) {}
		UAnimInstance(uint64_t object) :UAnimInstance(reinterpret_cast<void*>(object)) {}
	public:
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UTslAnimInstance : public UAnimInstance
	{
	public:
		UTslAnimInstance(const UObject& object) :UAnimInstance(object) {}
		UTslAnimInstance(void* object) :UAnimInstance(object) {}
		UTslAnimInstance(uint64_t object) :UTslAnimInstance(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//F3 0F 10 ? ? ? 00 00 F3 0F 10 ? 04 E8 ? ? ? ? F3 0F ? ? F3 + 0
		//88 ? ? ? 00 00 0F 57 ? F3 0F 10 ? ? ? 00 00 F3 + 9
		//FF 90 ? ? ? ? F3 0F 59 3D ? ? ? ? F2 0F 10 + 0xE
		MAKE_GENERAL_PROPERTY(struct FRotator, ControlRotation_CP, "StructProperty TslGame.TslAnimInstance.ControlRotation_CP");
		//0F 14 ?? F2 0F 11 ?? ?? ?? 00 00 F3 0F 11 ?? ?? ?? 00 00 F3 44 + 0x3
		//48 8B 8B ? ? ? ? 0F 14 ? F2 0F 11 8B + 0xA
		//48 8B 8B ?? ?? ?? ?? ?? ?? ?? 0F 14 ?? F2 0F 11 8B + 13
		//41 0F 28 ?? 0F 14 ?? F2 0F 11 ?? ?? ?? 00 00 F3 0F 11 ?? ?? ?? 00 00 F3 0F 11 ?? ?? 8B + 7
		MAKE_GENERAL_PROPERTY(struct FRotator, RecoilADSRotation_CP, "StructProperty TslGame.TslAnimInstance.RecoilADSRotation_CP");
		//F3 0F 10 ? 24 ? F3 0F ? ? F3 0F 10 ? ? ? 00 00 F3 0F ? ? 0F 28 + 0xA
		//F3 0F 10 ?? 24 ?? F3 0F ?? ?? F3 0F 10 ?? ?? ?? 00 00 0F 28 ?? F3 0F 59 ?? 0F + 0xA
		//F3 0F 10 ?? ?? ?? 00 00 48 8B ?? ?? ?? 00 00 0F 28 ?? F3 41 + 0
		//8B ?? ?? ?? 00 00 89 ?? ?? ?? 00 00 E8 ?? ?? ?? ?? 48 8B ?? ?? ?? 00 00 88 ?? ?? ?? 00 00 + 0
		MAKE_GENERAL_PROPERTY(float, ScopingAlpha_CP, "FloatProperty TslGame.TslAnimInstance.ScopingAlpha_CP");
		//0F 57 ? F3 0F 10 ? ? ? 00 00 41 0F 28 ? E8 ? ? ? ? F3 0F 10 ? ? ? ? ? 44 0F 28 + 3
		//F3 0F 11 ? ? ? 00 00 44 0F 28 ? 0F 57 ? 40 + 0
		MAKE_GENERAL_PROPERTY(float, LeanRightAlpha_CP, "FloatProperty TslGame.TslAnimInstance.LeanRightAlpha_CP");
		//0F 57 ? F3 0F 10 ? ? ? 00 00 41 0F 28 ? E8 ? ? ? ? 44 0F 28 ? F3 44 0F 11 ? ? ? 00 00 + 3
		//44 0F 28 ? F3 44 0F 11 ? ? ? 00 00 45 84 ? 74 ? 41 0F 28 + 4
		MAKE_GENERAL_PROPERTY(float, LeanLeftAlpha_CP, "FloatProperty TslGame.TslAnimInstance.LeanLeftAlpha_CP");
		//75 ? ? 38 ? ? ? 00 00 75 ? ? 84 ? 74 + 2
		//E8 ? ? ? ? 80 ? ? ? 00 00 00 F3 0F 11 ? ? ? 00 00 74 ? F3 0F 58 ? 0F + 5
		//E8 ? ? ? ? 80 ? ? ? 00 00 00 0F 57 ? F3 0F 10 ? ? ? 00 00 F3 0F 59 ? ? ? 00 00 + 5
		MAKE_GENERAL_PROPERTY(byte, bIsScoping_CP, "BoolProperty TslGame.TslAnimInstance.bIsScoping_CP");
		//F3 0F 11 ? ? ? 00 00 ? 38 ? ? ? 00 00 0F 85 ? ? ? ? 48 8B ? ? ? 00 00 ? 85 + 8
		MAKE_GENERAL_PROPERTY(byte, bIsAiming_CP, "BoolProperty TslGame.TslAnimInstance.bIsAiming_CP");
		//02 ? 88 ? ? ? 00 00 ? 88 ? ? ? 00 00 ? 8A ? ? ? ? 74 + 8
		//0F 2F ? ? ? 00 00 0F 82 ? ? ? ? ? 38 ? ? ? 00 00 0F 85 ? ? ? ? ? 38 + 0xD
		//E8 ? ? ? ? ? 38 ? ? ? 00 00 74 ? F3 0F 10 ? ? ? 00 00 F3 0F 11 ? ? ? ? ? E9 + 5
		MAKE_GENERAL_PROPERTY(byte, bIsReloading_CP, "BoolProperty TslGame.TslAnimInstance.bIsReloading_CP");
#else
		MAKE_GENERAL_PROPERTY(struct FRotator, ControlRotation_CP, "StructProperty TslGame.*5f04206550.ControlRotation_CP", g_PatternData.offset_UTslAnimInstance_ControlRotation_CP);
		MAKE_GENERAL_PROPERTY(struct FRotator, RecoilADSRotation_CP, "StructProperty TslGame.*5f04206550.RecoilADSRotation_CP", g_PatternData.offset_UTslAnimInstance_RecoilADSRotation_CP);
		MAKE_GENERAL_PROPERTY(float, ScopingAlpha_CP, "FloatProperty TslGame.*5f04206550.ScopingAlpha_CP", g_PatternData.offset_UTslAnimInstance_ScopingAlpha_CP);
		MAKE_GENERAL_PROPERTY(float, LeanRightAlpha_CP, "FloatProperty TslGame.*5f04206550.LeanRightAlpha_CP", g_PatternData.offset_UTslAnimInstance_LeanRightAlpha_CP);
		MAKE_GENERAL_PROPERTY(float, LeanLeftAlpha_CP, "FloatProperty TslGame.*5f04206550.LeanLeftAlpha_CP", g_PatternData.offset_UTslAnimInstance_LeanLeftAlpha_CP);
		MAKE_GENERAL_PROPERTY(byte, bIsScoping_CP, "BoolProperty TslGame.*5f04206550.bIsScoping_CP", g_PatternData.offset_UTslAnimInstance_bIsScoping_CP);
		MAKE_GENERAL_PROPERTY(byte, bIsTPPAiming, "BoolProperty TslGame.*5f04206550.bIsTPPAiming", g_PatternData.offset_UTslAnimInstance_bIsTPPAiming);
		MAKE_GENERAL_PROPERTY(byte, bIsAiming_CP, "BoolProperty TslGame.*5f04206550.bIsAiming_CP", g_PatternData.offset_UTslAnimInstance_bIsAiming_CP);
		MAKE_GENERAL_PROPERTY(byte, bIsReloading_CP, "BoolProperty TslGame.*5f04206550.bIsReloading_CP", g_PatternData.offset_UTslAnimInstance_bIsReloading_CP);
#endif


	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class USkeletalMeshComponent : public USkinnedMeshComponent
	{
	public:
		USkeletalMeshComponent(const UObject& object) :USkinnedMeshComponent(object) {}
		USkeletalMeshComponent(void* object) :USkinnedMeshComponent(object) {}
		USkeletalMeshComponent(uint64_t object) :USkeletalMeshComponent(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//48 8B ? ? ? 00 00 48 85 ? 74 ? 80 78 ? 03 75 + 0
		//48 83 ? ? ? 00 00 00 75 ? 48 83 ? ? ? 00 00 00 0f 84 ? ? ? ? f6 + 0
		//00 00 0f 28 ? E8 ? ? ? ? 48 8B ? ? ? 00 00 48 85 ? 74 ? ? 8B ? 0F + 0xA
		MAKE_OBJECT_PROPERTY(UAnimInstance, AnimScriptInstance, "ObjectProperty Engine.SkeletalMeshComponent.AnimScriptInstance");
#else
		MAKE_OBJECT_PROPERTY(UAnimInstance, AnimScriptInstance, "ObjectProperty Engine.SkeletalMeshComponent.AnimScriptInstance", g_PatternData.offset_USkeletalMeshComponent_AnimScriptInstance);
#endif

		/** Access ComponentSpaceTransforms for reading */
		TArray<FTransform> GetComponentSpaceTransforms() const
		{
			return GetDMA().Read<TArray<FTransform>>((ptr_t)GetPtr() + g_PatternData.offset_ComponentSpaceTransforms);
		}


	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class UStaticMeshSocket : public UObject
	{
	public:
		UStaticMeshSocket(const UObject& object) :UObject(object) {}
		UStaticMeshSocket(void* object) :UObject(object) {}
		UStaticMeshSocket(uint64_t object) :UObject(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		MAKE_STRUCT_PROPERTY(struct FName, SocketName, "NameProperty Engine.StaticMeshSocket.SocketName");
		MAKE_STRUCT_PROPERTY(struct FVector, RelativeLocation, "StructProperty Engine.StaticMeshSocket.RelativeLocation");
		MAKE_STRUCT_PROPERTY(struct FRotator, RelativeRotation, "StructProperty Engine.StaticMeshSocket.RelativeRotation");
		MAKE_STRUCT_PROPERTY(struct FVector, RelativeScale, "StructProperty Engine.StaticMeshSocket.RelativeScale");
#else
		MAKE_STRUCT_PROPERTY(struct FName, SocketName, "NameProperty Engine.*25e8963454.SocketName", g_PatternData.offset_UStaticMeshSocket_SocketName);
		MAKE_STRUCT_PROPERTY(struct FVector, RelativeLocation, "StructProperty Engine.*25e8963454.RelativeLocation", g_PatternData.offset_UStaticMeshSocket_RelativeLocation);
		MAKE_STRUCT_PROPERTY(struct FRotator, RelativeRotation, "StructProperty Engine.*25e8963454.RelativeRotation", g_PatternData.offset_UStaticMeshSocket_RelativeRotation);
		MAKE_STRUCT_PROPERTY(struct FVector, RelativeScale, "StructProperty Engine.*25e8963454.RelativeScale", g_PatternData.offset_UStaticMeshSocket_RelativeScale);
#endif

		bool GetSocketTransform(FTransform& OutTransform, class UStaticMeshComponent* const MeshComp);
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UStaticMesh : public UObject
	{
	public:
		UStaticMesh(const UObject& object) :UObject(object) {}
		UStaticMesh(void* object) :UObject(object) {}
		UStaticMesh(uint64_t object) :UObject(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		MAKE_GENERAL_PROPERTY(TArray<UStaticMeshSocket*>, Sockets, "ArrayProperty Engine.StaticMesh.Sockets");
#else
		MAKE_GENERAL_PROPERTY(TArray<UStaticMeshSocket*>, Sockets, "ArrayProperty Engine.StaticMesh.Sockets", g_PatternData.offset_UStaticMesh_Sockets);
#endif
		UStaticMeshSocket FindSocket(FName InSocketName);
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UStaticMeshComponent : public UMeshComponent
	{
	public:
		UStaticMeshComponent(const UObject& object) :UMeshComponent(object) {}
		UStaticMeshComponent(void* object) :UMeshComponent(object) {}
		UStaticMeshComponent(uint64_t object) :UMeshComponent(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//0F 84 ? ? ? ? F3 0F 10 81 ? ? ? ? F3 0F 10 15 + 6
		MAKE_OBJECT_PROPERTY(class UStaticMesh, StaticMesh, "ObjectProperty Engine.StaticMeshComponent.StaticMesh");
#else
		MAKE_OBJECT_PROPERTY(class UStaticMesh, StaticMesh, "ObjectProperty Engine.StaticMeshComponent.StaticMesh", g_PatternData.offset_UStaticMeshComponent_StaticMesh);
#endif

		/**
		 * Returns the named socket on the static mesh component.
		 *
		 * @return UStaticMeshSocket of named socket on the static mesh component. None if not found.
		 */
		UStaticMeshSocket GetSocketByName(FName InSocketName);


		virtual FTransform GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace = RTS_World) override;

	private:

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};


	class USplineMeshComponent : public UStaticMeshComponent
	{
	public:
		USplineMeshComponent(const UObject& object) :UStaticMeshComponent(object) {}
		USplineMeshComponent(void* object) :UStaticMeshComponent(object) {}
		USplineMeshComponent(uint64_t object) :UStaticMeshComponent(reinterpret_cast<void*>(object)) {}
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class UAsyncStaticMeshComponent : public UStaticMeshComponent
	{
	public:
		UAsyncStaticMeshComponent(const UObject& object) :UStaticMeshComponent(object) {}
		UAsyncStaticMeshComponent(void* object) :UStaticMeshComponent(object) {}
		UAsyncStaticMeshComponent(uint64_t object) :UStaticMeshComponent(reinterpret_cast<void*>(object)) {}

	private:

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class UWeaponMeshComponent : public USkeletalMeshComponent
	{
	public:
		UWeaponMeshComponent(const UObject& object) :USkeletalMeshComponent(object) {}
		UWeaponMeshComponent(void* object) :USkeletalMeshComponent(object) {}
		UWeaponMeshComponent(uint64_t object) :USkeletalMeshComponent(reinterpret_cast<void*>(object)) {}
	public:

#ifndef ObjectNameHash
		//<EWeaponAttachmentSlotID, class UAsyncStaticMeshComponent>
		MAKE_STRUCT_PROPERTY(TMap, AttachedStaticComponentMap, "MapProperty TslGame.WeaponMeshComponent.AttachedStaticComponentMap");
		MAKE_STRUCT_PROPERTY(FReplicatedSkinParam, ReplicatedSkinParam, "StructProperty TslGame.WeaponMeshComponent.ReplicatedSkinParam");
#else
		//<EWeaponAttachmentSlotID, class UAsyncStaticMeshComponent>
		MAKE_STRUCT_PROPERTY(TMap, AttachedStaticComponentMap, "MapProperty TslGame.*aa9ef014cb.*1ea63f618e", g_PatternData.offset_UWeaponMeshComponent_AttachedStaticComponentMap);
		MAKE_STRUCT_PROPERTY(FReplicatedSkinParam, ReplicatedSkinParam, "StructProperty TslGame.*aa9ef014cb.*ddbee9f3a6", g_PatternData.offset_UWeaponMeshComponent_ReplicatedSkinParam);
#endif

	public:
		ptr_t GetReplicatedSkinParamOffset();

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class UWheeledVehicleMovementComponent : public UActorComponent //UPawnMovementComponent
	{

	public:
		UWheeledVehicleMovementComponent(const UObject& object) :UActorComponent(object) {}
		UWheeledVehicleMovementComponent(void* object) :UActorComponent(object) {}
		UWheeledVehicleMovementComponent(uint64_t object) :UActorComponent(reinterpret_cast<void*>(object)) {}

	public:


	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UCharacterMovementComponent : public UActorComponent //UPawnMovementComponent
	{

	public:
		UCharacterMovementComponent(const UObject& object) :UActorComponent(object) {}
		UCharacterMovementComponent(void* object) :UActorComponent(object) {}
		UCharacterMovementComponent(uint64_t object) :UActorComponent(reinterpret_cast<void*>(object)) {}

	public:
#ifndef ObjectNameHash
		//F2 0F 10 ? ? ? 00 00 48 8D ? ? ? 00 00 F2 0F 11 ? ? 8B + 0
		//8B ? 24 F2 0F 10 ? ? F2 41 0F 11 ? ? ? 00 00 8B + 8
		//0F 28 ? 24 ? F3 0F 58 ? ? ? 00 00 F3 0F 11 ? ? ? 00 00 F3 0F + 5
		MAKE_STRUCT_PROPERTY(struct FVector, Velocity, "StructProperty Engine.MovementComponent.Velocity");
		//83 ? ? ? 00 00 00 F3 0F 59 ? ? ? 00 00 41 + 7
		//F3 41 0F 10 ? ? ? 00 00 ? 8B ? 41 83 ? 10 + 0
		MAKE_GENERAL_PROPERTY(float, JumpZVelocity, "FloatProperty Engine.CharacterMovementComponent.JumpZVelocity");
		//83 ? ? 00 F3 0F 10 ? ? ? 00 00 0F 84 ? ? ? ? 48 ? ? ? 85 + 4
		MAKE_GENERAL_PROPERTY(float, WalkableFloorZ, "FloatProperty Engine.CharacterMovementComponent.WalkableFloorZ");
		//F3 0F 11 ? ? ? 00 00 F2 0F 10 ? F2 0F 11 ? ? ? 00 00 8B ? ? 89 ? ? ? 00 00 F3 + 0xC
		//F3 44 0F 11 ? ? ? 00 00 F2 0F 10 ? F2 0F 11 ? ? ? 00 00 8B ? ? 89 ? ? ? 00 00 F3 + 0xD
		//0F 29 44 24 ? 0F 2E ? ? ? 00 00 75 ? F3 + 5
		//0F 29 84 24 ? ? 00 00 0F 2E ? ? ? 00 00 75 ? F3 + 8
		MAKE_STRUCT_PROPERTY(struct FVector, LastUpdateLocation, "StructProperty Engine.CharacterMovementComponent.LastUpdateLocation");
		//8B ? 08 89 83 ? ? 00 00 F3 0F 7F ? ? ? 00 00 + 9
		MAKE_STRUCT_PROPERTY(struct FVector, LastUpdateRotation, "StructProperty Engine.CharacterMovementComponent.LastUpdateRotation");
		//F2 0F 11 ? ? ? 00 00 41 8B ? 08 89 ? ? ? 00 00 E9 ? ? ? ? ? 8b ? ? ? 00 00 + 0
		//F2 0F 11 ? ? ? 00 00 8B ? 08 89 ? ? ? 00 00 E9 ? ? ? ? ? 8b ? ? ? 00 00 + 0
		MAKE_STRUCT_PROPERTY(struct FVector, LastUpdateVelocity, "StructProperty Engine.CharacterMovementComponent.LastUpdateVelocity");
		//72 ? F3 0F 5D ? 0F 28 ? F6 83 ? ? 00 00 01 F3 + 9
		//F6 ? ? ? 00 00 01 74 ? 45 33 C0 40 8A + 0
		//FF 90 ? ? 00 00 84 C0 74 ? F6 ? ? ? 00 00 01 74 ? F3 0F ? ? ? ? 00 00 + 0xA
		MAKE_STRUCT_PROPERTY(struct FFindFloorResult, CurrentFloor, "StructProperty Engine.CharacterMovementComponent.CurrentFloor");
		//83 ? ? ? 00 00 40 ? 8B ? ? ? ? ? ? 8B ? ? ? 00 00 ? 85 + 0
		//40 84 ? ? ? 00 00 75 ? 8A ? ? ? 00 00 38 ? ? ? ? ? 75 + 0
		//0F BA ? ? ? 00 00 07 09 ? ? ? 00 00 ? 8B ? 48 8b + 0
		MAKE_STRUCT_PROPERTY(struct UCharacterMovementComponent_BIT4, bit4, "BoolProperty Engine.CharacterMovementComponent.bMaintainHorizontalGroundVelocity");
#else
		MAKE_STRUCT_PROPERTY(struct FVector, Velocity, "StructProperty Engine.MovementComponent.Velocity", g_PatternData.offset_UCharacterMovementComponent_Velocity);
		MAKE_GENERAL_PROPERTY(float, JumpZVelocity, "FloatProperty Engine.CharacterMovementComponent.JumpZVelocity", g_PatternData.offset_UCharacterMovementComponent_JumpZVelocity);
		MAKE_GENERAL_PROPERTY(float, WalkableFloorZ, "FloatProperty Engine.CharacterMovementComponent.*000d53b369", g_PatternData.offset_UCharacterMovementComponent_WalkableFloorZ);
		MAKE_STRUCT_PROPERTY(struct FVector, LastUpdateLocation, "StructProperty Engine.CharacterMovementComponent.*6013800739", g_PatternData.offset_UCharacterMovementComponent_LastUpdateLocation);
		MAKE_STRUCT_PROPERTY(struct FVector, LastUpdateRotation, "StructProperty Engine.CharacterMovementComponent.*8e229ded77", g_PatternData.offset_UCharacterMovementComponent_LastUpdateRotation);
		MAKE_STRUCT_PROPERTY(struct FVector, LastUpdateVelocity, "StructProperty Engine.CharacterMovementComponent.*d306302ed7", g_PatternData.offset_UCharacterMovementComponent_LastUpdateVelocity);
		MAKE_STRUCT_PROPERTY(struct FFindFloorResult, CurrentFloor, "StructProperty Engine.CharacterMovementComponent.CurrentFloor", g_PatternData.offset_UCharacterMovementComponent_CurrentFloor);
		MAKE_STRUCT_PROPERTY(struct UCharacterMovementComponent_BIT4, bMaintainHorizontalGroundVelocity, "BoolProperty Engine.CharacterMovementComponent.bMaintainHorizontalGroundVelocity", g_PatternData.offset_UCharacterMovementComponent_bit4);
#endif

		bool IsWalkable(const FHitResult& Hit, float _WalkableFloorZ, UPrimitiveComponent HitResult_Component, FWalkableSlopeOverride WalkableSlopeOverride);
		FVector ComputeGroundMovementDelta(const FVector& Delta, const FHitResult& RampHit,
			const bool bHitFromLineTrace, UCharacterMovementComponent_BIT4 bit4, float WalkZ, UPrimitiveComponent HitResult_Component,
			FWalkableSlopeOverride WalkableSlopeOverride);
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ULocalPlayer : public UObject
	{
	public:
		ULocalPlayer(const UObject& object) :UObject(object),
			ViewportClient(object.GetPtr(), g_PatternData.offset_ViewportClient),
			PlayerController(object.GetPtr(), g_PatternData.offset_PlayerController) {}
		ULocalPlayer(void* object)
			:UObject(object),
			ViewportClient(object, g_PatternData.offset_ViewportClient),
			PlayerController(object, g_PatternData.offset_PlayerController)
		{

		}
		ULocalPlayer(uint64_t object) :ULocalPlayer(reinterpret_cast<void*>(object)) {}

		~ULocalPlayer() {};
	public:
		UEncryptedObjectProperty<class UGameViewportClient, DecryptFunc::General> ViewportClient;
		UEncryptedObjectProperty<class APlayerController, DecryptFunc::General> PlayerController;

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ACharacter : public APawn
	{
	public:
		ACharacter(const UObject& object) :APawn(object), CharacterMovement(object.GetPtr(), g_PatternData.offset_ACharacter_CharacterMovement) {}
		ACharacter(void* object) :APawn(object), CharacterMovement(object, g_PatternData.offset_ACharacter_CharacterMovement) {}
		ACharacter(uint64_t object) :ACharacter(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//0F 57 F6 0F 28 CE E8 ? ? ? ? 48 83 3D ? ? ? ? 00 48 8B ? ? ? 00 00 48 + 0x13
		//74 ? 83 4B 10 10 48 ? ? ? ? ? ? ? 8B ? ? ? 00 00 75 + 0xD
		//F3 44 0F ? ? ? ? ? ? 48 85 ? 0F 84 ? ? ? ? 48 83 3D ? ? ? ? 00 ? 8B + 0x1A
		UEncryptedObjectProperty<class UCharacterMovementComponent, DecryptFunc::General, "EncryptedObjectProperty Engine.Character.CharacterMovement"_hash> CharacterMovement;
		//8B ? ? ? 00 00 ? 8B ? ? 85 ? 0F 84 ? ? ? ? F3 0F 10 ? ? ? ? ? 48 + 0
		//F3 0F 58 ? 24 ? 48 8B ? ? ? 00 00 48 8B ? ? B8 01 + 6
		//48 8B ? ? ? 00 00 48 85 ? 74 ? 83 64 24 ? 00 4c 8d 44 24 ? 83 64 24 ? 00 0f + 0
		MAKE_OBJECT_PROPERTY(USkeletalMeshComponent, Mesh, "ObjectProperty Engine.Character.Mesh");
#else
		UEncryptedObjectProperty<class UCharacterMovementComponent, DecryptFunc::General> CharacterMovement;
		MAKE_OBJECT_PROPERTY(USkeletalMeshComponent, Mesh, "ObjectProperty Engine.Character.Mesh", g_PatternData.offset_ACharacter_Mesh);
#endif



	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ATeam : public AActor
	{
	public:
		ATeam(const UObject& object) :AActor(object) {}
		ATeam(void* object) :AActor(object) {}
		ATeam(uint64_t object) :ATeam(reinterpret_cast<void*>(object)) {}
	public:
		//MAKE_GENERAL_PROPERTY(float, PiningTimeSeconds, "FloatProperty TslGame.Team.PiningTimeSeconds");
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UVehicleRiderComponent : public UActorComponent
	{
	public:
		UVehicleRiderComponent(const UObject& object) :UActorComponent(object) {}
		UVehicleRiderComponent(void* object) :UActorComponent(object) {}
		UVehicleRiderComponent(uint64_t object) :UVehicleRiderComponent(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//F6 ? ? ? 00 00 01 0F 85 ? ? ? ? ? 8B ? ? ? 00 00 8B ? ? ? 00 00 C1 ? 1F 34 01 + 0x14
		//33 DB ? 8B ? 39 ? ? ? 00 00 7D ? 33 C0 + 5
		//8B ? ? ? 00 00 41 FF 90 ? ? 00 00 48 8B ? 48 8B ? EB + 0
		MAKE_GENERAL_PROPERTY(int, SeatIndex, "IntProperty TslGame.VehicleRiderComponent.SeatIndex");
		//FF ? ? ? 00 00 84 ? 0f 85 ? ? ? ? 48 3B ? ? ? 00 00 74 ? E8 + 0xE
		//8D ? ? ? 00 00 ? 89 ? ? ? 00 00 ? 8B ? E8 ? ? ? ? ? 8D ? ? ? 00 00 ? 8B ? E8 + 6
		//85 ? 74 ? ? 8B ? ? ? 00 00 ? 8B ? B2 01 ? 8B ? E8 + 4
		MAKE_OBJECT_PROPERTY(APawn, LastVehiclePawn, "ObjectProperty TslGame.VehicleRiderComponent.LastVehiclePawn");
#else
		MAKE_GENERAL_PROPERTY(int, SeatIndex, "IntProperty TslGame.VehicleRiderComponent.*1db7758c4e", g_PatternData.offset_UVehicleRiderComponent_SeatIndex);
		MAKE_OBJECT_PROPERTY(APawn, LastVehiclePawn, "ObjectProperty TslGame.VehicleRiderComponent.*5f166f466c", g_PatternData.offset_UVehicleRiderComponent_LastVehiclePawn);
#endif

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ATslWeapon;
	class AInventoryFacade;

	class UWeaponProcessorComponent : public UActorComponent
	{
	public:
		UWeaponProcessorComponent(const UObject& object) :UActorComponent(object) {}
		UWeaponProcessorComponent(void* object)
			:UActorComponent(object)
		{}
		UWeaponProcessorComponent(uint64_t object) :UWeaponProcessorComponent(reinterpret_cast<void*>(object)) {}
	public:

#ifndef ObjectNameHash
		//8B ? ? ? 00 00 48 63 ? 00 02 00 00 ? 8D 3C ? ? 3B + 0
		//3B ? 00 02 00 00 0F 8D ? ? ? ? 48 8B ? ? ? 00 00 48 63 + 0xC
		//48 8B ? ? ? 00 00 48 63 ? 00 02 00 00 48 8b ? ? 8d + 0
		//0F 88 ? ? ? ? 3B ? ? ? 00 00 0F 8D ? ? ? ? 48 8B ? ? ? 00 00 48 63 ? 48 8B ? F0 48 85 + 0x12
		MAKE_GENERAL_PROPERTY(TArray<ATslWeapon*>, EquippedWeapons, "ArrayProperty TslGame.BaseWeaponProcessorComponent.EquippedWeapons");
#else
		MAKE_GENERAL_PROPERTY(TArray<ATslWeapon*>, EquippedWeapons, "ArrayProperty TslGame.*7854758044.*28cf047c4d", g_PatternData.offset_UWeaponProcessorComponent_EquippedWeapons);

		MAKE_GENERAL_PROPERTY(int, WeaponIndex_, "StructProperty TslGame.WeaponProcessorComponent.*d431f444e7", g_PatternData.offset_UWeaponProcessorComponent_WeaponIndex);
#endif
		int GetWeaponIndex();

		class ATslWeapon GetCurrentWeapon();
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ATslCharacterBase : public ACharacter
	{
	public:
		ATslCharacterBase(const UObject& object) :ACharacter(object) {}
		ATslCharacterBase(void* object = nullptr) :ACharacter(object) {}
		ATslCharacterBase(uint64_t object = 0) :ACharacter(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//0F 57 ? 0F 2F ? ? ? 00 00 73 ? 80 ? ? ? 00 00 02 + 3
		//0F 57 ? ? 8B ? 0F 2F ? ? ? 00 00 44 + 6
		//F7 42 ? 00 00 00 20 75 ? F3 0F 10 ? ? ? 00 00 EB + 9
		MAKE_GENERAL_PROPERTY(float, Health, "FloatProperty TslGame.TslCharacterBase.Health");
		//EB ? F3 0F 10 ? ? ? 00 00 48 8B ? e8 ? ? ? ? 44 + 2
		MAKE_GENERAL_PROPERTY(float, HealthMax, "FloatProperty TslGame.TslCharacterBase.HealthMax");
		//UWeaponProcessorComponent
		//48 8B ? ? ? 00 00 48 8B ? FF 90 ? ? 00 00 ? 8B ? 40 ? 01 48 85 ? 74 ? E8 + 0
		//00 00 E8 ? ? ? ? ? 8B ? ? ? 00 00 84 C0 40 0F ? ? ? 85 + 7
		//8B ? ? ? 00 00 40 8A ? 40 ? 01 ? 85 ? 74 + 0
		//FF 90 30 01 00 00 ? 8B ? ? ? 00 00 F3 0F 10 ? ? ? 00 00 ? 8B ? ? ? 00 00 E8 + 0x15
		MAKE_OBJECT_PROPERTY(UWeaponProcessorComponent, WeaponProcessor, "ObjectProperty TslGame.TslCharacterBase.WeaponProcessor");//hide
#else
		//FloatProperty TslGame.TslCharacterBase.Health下数4个
		MAKE_GENERAL_PROPERTY(float, Health, "StructProperty TslGame.TslCharacterBase.*b27f154f61", g_PatternData.offset_ATslCharacterBase_Health);
		MAKE_GENERAL_PROPERTY(float, HealthMax, "FloatProperty TslGame.TslCharacterBase.HealthMax", g_PatternData.offset_ATslCharacterBase_HealthMax);
		MAKE_OBJECT_PROPERTY(UWeaponProcessorComponent, WeaponProcessor, "ObjectProperty TslGame.TslCharacterBase.WeaponProcessor", g_PatternData.offset_ATslCharacterBase_WeaponProcessor);//hide
#endif

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ADummyAICharacter : public ATslCharacterBase
	{
	public:
		ADummyAICharacter(const UObject& object) :ATslCharacterBase(object) {}
		ADummyAICharacter(void* object = nullptr) :ATslCharacterBase(object) {}
		ADummyAICharacter(uint64_t object = 0) :ATslCharacterBase(reinterpret_cast<void*>(object)) {}
	public:

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ATslNpcCharacter : public ADummyAICharacter
	{
	public:
		ATslNpcCharacter(const UObject& object) :ADummyAICharacter(object) {}
		ATslNpcCharacter(void* object = nullptr) :ADummyAICharacter(object) {}
		ATslNpcCharacter(uint64_t object = 0) :ADummyAICharacter(reinterpret_cast<void*>(object)) {}
	public:

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class AZDF2AICharacter : public ATslNpcCharacter
	{
	public:
		AZDF2AICharacter(const UObject& object) :ATslNpcCharacter(object) {}
		AZDF2AICharacter(void* object = nullptr) :ATslNpcCharacter(object) {}
		AZDF2AICharacter(uint64_t object = 0) :ATslNpcCharacter(reinterpret_cast<void*>(object)) {}
	public:

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ATslSLBAICharacter : public AZDF2AICharacter
	{
	public:
		ATslSLBAICharacter(const UObject& object) :AZDF2AICharacter(object) {}
		ATslSLBAICharacter(void* object = nullptr) :AZDF2AICharacter(object) {}
		ATslSLBAICharacter(uint64_t object = 0) :AZDF2AICharacter(reinterpret_cast<void*>(object)) {}
	public:

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ATslCharacter :public ATslCharacterBase
	{
	public:
		ATslCharacter(const UObject& object) :ATslCharacterBase(object), Team(object.GetPtr(), g_PatternData.offset_ATslCharacter_Team), InventoryFacade(object.GetPtr(), g_PatternData.offset_ATslCharacter_InventoryFacade),
			_Female(false), _LastTeamNum(0), _SpectatedCount(0), _CurrentWeapon(nullptr), _EquippedWeapon(0),
			_WeaponIndex(0xFF), _EncryptWeaponID(0), _Mesh(nullptr), _ComponentSpaceTransformsArray(TArray<FTransform>()), _ReadType(0),
			_AnimScriptInstance(nullptr), _IsAiming_CP(0), _ScopingAlpha_CP(0.f), _IsReloading_CP(0), _HealthFlag1(0), _HealthFlag2(0), _HealthFlag3(0),
			_HealthIndex1(0), _HealthXorKeys(0), _Health(0.f), _HealthMax(0.f), _GroggyHealth(0.f), _EncryptHealth(0),
			_GroggyHealthMax(0.f), _LastRenderTimeOnScreen(0.f), _AimOffsets(FRotator()), _LastAimOffsets(FRotator()), _EncryptTeam(0), _Team(nullptr),
			_Root_RelativeLocation(FVector()), _ComponentToWorld(FTransform()), _EncryptPlayerState(0), _EncryptPlayerState2(0), _PlayerState(nullptr),
			_PlayerStatistics(FTslPlayerStatistics()), _KillNum(0), _AssistsNum(0), _SurvivalTier(0), _SurvivalLevel(0), _isParthers(0), _PartnerLevel(0),
			_PlayerName(""), _NameplateId(""), _EmblemId(""), _ClanTag(""), _isDisconnected(0), _bBlackList(0), _RankText(""), _isInFog(false), _bCheckAsyncSceneOnMove(0),
			_DamageDealtOnEnemy(0.f), _LastRenderTime(0.f), _LastSubmitTime(0.f), _CacheLocalBound1(FQuat()), _CacheLocalBound2(FQuat()), _ShowName(""), _RankName(""), _TeamName(""), _KDA(0.f), _CurrentRankPoint(0),
			_RankIcon(""), _CharacterState(0), _AvgDamage(0.f), _InitPartner(false)
		{
			if (!_BoneTransFormTable.empty())
				_BoneTransFormTable.clear();
		}

		ATslCharacter(void* object = nullptr) :ATslCharacterBase(object), Team(object, g_PatternData.offset_ATslCharacter_Team), InventoryFacade(object, g_PatternData.offset_ATslCharacter_InventoryFacade),
			_Female(false), _LastTeamNum(0), _SpectatedCount(0), _CurrentWeapon(nullptr), _EquippedWeapon(0),
			_WeaponIndex(0xFF), _EncryptWeaponID(0), _Mesh(nullptr), _ComponentSpaceTransformsArray(TArray<FTransform>()), _ReadType(0),
			_AnimScriptInstance(nullptr), _IsAiming_CP(0), _ScopingAlpha_CP(0.f), _IsReloading_CP(0), _HealthFlag1(0), _HealthFlag2(0), _HealthFlag3(0),
			_HealthIndex1(0), _HealthXorKeys(0), _Health(0.f), _HealthMax(0.f), _GroggyHealth(0.f), _EncryptHealth(0),
			_GroggyHealthMax(0.f), _LastRenderTimeOnScreen(0.f), _AimOffsets(FRotator()), _LastAimOffsets(FRotator()), _EncryptTeam(0), _Team(nullptr),
			_Root_RelativeLocation(FVector()), _ComponentToWorld(FTransform()), _EncryptPlayerState(0), _EncryptPlayerState2(0), _PlayerState(nullptr),
			_PlayerStatistics(FTslPlayerStatistics()), _KillNum(0), _AssistsNum(0), _SurvivalTier(0), _SurvivalLevel(0), _isParthers(0), _PartnerLevel(0),
			_PlayerName(""), _NameplateId(""), _EmblemId(""), _ClanTag(""), _isDisconnected(0), _bBlackList(0), _RankText(""), _isInFog(false), _bCheckAsyncSceneOnMove(0),
			_DamageDealtOnEnemy(0.f), _LastRenderTime(0.f), _LastSubmitTime(0.f), _CacheLocalBound1(FQuat()), _CacheLocalBound2(FQuat()), _ShowName(""), _RankName(""), _TeamName(""), _KDA(0.f), _CurrentRankPoint(0),
			_RankIcon(""), _CharacterState(0), _AvgDamage(0.f), _InitPartner(false)
		{
			if (!_BoneTransFormTable.empty())
				_BoneTransFormTable.clear();
		}
		ATslCharacter(uint64_t object = 0) :ATslCharacter(reinterpret_cast<void*>(object)) {}

	public:
#ifndef ObjectNameHash
		//89 44 ? ? ? 8B ? E8 ? ? ? ? 48 89 44 24 ? 48 8B ? E8 + 7,1,5 -> GetTeam Call rcx->ATslCharacter
		//0F 57 ? 89 ? ? E8 ? ? ? ? 48 8B ? ? ? 00 00 84 ? 0F 84 + 6,1,5 -> GetTeam Call rcx->ATslCharacter
		//4C 89 ? ? 4C 89 ? ? ? 8B ? ? ? 00 00 48 89 ? ? ? 39 + 8
		UEncryptedObjectProperty<class ATeam, DecryptFunc::General, "EncryptedObjectProperty TslGame.TslCharacter.Team"_hash> Team;
		//33 ? ? 8B ? ? ? 00 00 41 8B ? 44 8A ? 48 8B ? 41 ? 00 00 FF FF + 2
		//48 8B ? ? ? ? ? 33 ? ? 8B ? ? ? 00 00 44 ? ? 48 85 ? 75 ? ? 8b + 9
		//48 89 5C 24 08 48 89 74 24 20 57 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 85 ? 74 + 0x12,1,5
		UEncryptedObjectProperty<class AInventoryFacade, DecryptFunc::General, "EncryptedObjectProperty TslGame.TslCharacter.InventoryFacade"_hash> InventoryFacade;
		//F6 ? ? ? 00 00 01 0F 85 ? ? ? ? 48 8B ? ? ? 00 00 8B ? ? ? 00 00 + 0xD
		//48 8B ? ? ? 00 00 33 ? ? 8B ? ? ? ? ? 41 ? 00 00 FF FF 48 8B + 0
		//48 8B ? ? ? 00 00 E8 ? ? ? ? 48 8D 55 ? 48 8B ? 48 8B ? E8 + 0
		MAKE_OBJECT_PROPERTY(UVehicleRiderComponent, VehicleRiderComponent, "ObjectProperty TslGame.TslCharacter.VehicleRiderComponent");
		//MAKE_GENERAL_PROPERTY(FString, CharacterName, "StrProperty TslGame.TslCharacter.CharacterName");
		//0F 2F ? 76 ? F3 0F 10 ? ? ? 00 00 0F 2F ? 76 ? F3 0F 10 ? ? ? 00 00 + 0x12
		//40 ? ? 84 ? 74 ? F3 0F 10 ? ? ? 00 00 EB ? F3 0F 10 ? ? ? 00 00 40 + 7
		//73 ? 32 ? C3 0F 2F ? ? ? 00 00 72 + 5
		MAKE_GENERAL_PROPERTY(float, GroggyHealth, "FloatProperty TslGame.TslCharacter.GroggyHealth");
		//0F 2F ? 76 ? F3 0F 10 ? ? ? 00 00 0F 2F ? 76 ? F3 0F 10 ? ? ? 00 00 + 5
		//40 ? ? 74 ? F3 0F 10 ? ? ? 00 00 EB ? F3 0F 10 ? ? ? 00 00 48 8B + 5
		MAKE_GENERAL_PROPERTY(float, GroggyHealthMax, "FloatProperty TslGame.TslCharacter.GroggyHealthMax");
		//0F 84 ? ? ? ? F2 0F 10 ? ? ? 00 00 8B ? ? ? 00 00 F3 44 0F 10 ? ? ? ? ? F3 + 6
		//F2 0F 10 ? ? ? 00 00 48 8D 55 ? 8B ? ? ? 00 00 48 8B ? F2 0F 11 ? ? ? 00 00 89 + 0
		MAKE_GENERAL_PROPERTY(struct FRotator, AimOffsets, "StructProperty TslGame.TslCharacter.AimOffsets");
		//F2 0F 10 ? ? ? 00 00 48 8D 55 ? 8B ? ? ? 00 00 48 8B ? F2 0F 11 ? ? ? 00 00 89 + 0x15
		//73 ? F3 0F 58 ? F2 0F 10 ? ? ? 00 00 8B + 6
		MAKE_GENERAL_PROPERTY(struct FRotator, LastAimOffsets, "StructProperty TslGame.TslCharacter.LastAimOffsets");
		//48 8B ? ? ? 00 00 48 85 ? 0F 84 ? ? ? ? 41 ? 01 00 00 00 41 8A + 0
		//8B ? ? ? 00 00 48 8b 01 FF 90 ? ? 00 00 84 C0 0F 84 ? ? ? ? f3 41 + 0
		//8b ? ? ? 00 00 f3 0f 10 ? ? ? 00 00 f3 0f 10 ? ? ? 00 00 f3 0f 10 ? ? ? 00 00 f3 0f 59 ? f3 0f + 0
		MAKE_GENERAL_PROPERTY(void*, ScopingCamera, "ObjectProperty TslGame.TslCharacter.ScopingCamera");
		//C7 44 24 ? 42 01 00 00 48 8B 44 24 ? 48 39 ? ? ? 00 00 0F 94 C0 84 C0 0F 85 下第一个CALL
		//ff 90 ? ? 00 00 48 85 ? 74 ? 83 b8 ? ? 00 00 00 0f + 0xB
		MAKE_GENERAL_PROPERTY(int, SpectatedCount, "IntProperty TslGame.TslCharacter.SpectatedCount");
		//FF 90 ? ? 00 00 3B ? ? ? 00 00 0F 95 ? EB + 6
		//48 83 EC ?? 41 8B 80 ?? ?? ?? ?? 49 8B + 4
		MAKE_GENERAL_PROPERTY(int, LastTeamNum, "IntProperty TslGame.TslCharacter.LastTeamNum");
		//89 ? ? 48 8B 45 ? 8B 80 ? ? ? ? 89 ? ? ? ? ? ? 89 ? ? 4C 39 + D] - 0x10
		//搜StrProperty TslGame.TslCharacter. 有2个递增0x10的地址不是
		MAKE_GENERAL_PROPERTY(FString, CharacterName, "StrProperty TslGame.TslCharacter.CharacterName");

#else
		UEncryptedObjectProperty<class ATeam, DecryptFunc::General> Team;
		UEncryptedObjectProperty<class AInventoryFacade, DecryptFunc::General> InventoryFacade;
		MAKE_OBJECT_PROPERTY(UVehicleRiderComponent, VehicleRiderComponent, "ObjectProperty TslGame.TslCharacter.VehicleRiderComponent", g_PatternData.offset_ATslCharacter_VehicleRiderComponent);
		MAKE_GENERAL_PROPERTY(float, GroggyHealth, "FloatProperty TslGame.TslCharacter.GroggyHealth", g_PatternData.offset_ATslCharacter_GroggyHealth);
		MAKE_GENERAL_PROPERTY(float, GroggyHealthMax, "FloatProperty TslGame.TslCharacter.GroggyHealthMax", g_PatternData.offset_ATslCharacter_GroggyHealthMax);
		MAKE_GENERAL_PROPERTY(struct FRotator, AimOffsets, "StructProperty TslGame.TslCharacter.AimOffsets", g_PatternData.offset_ATslCharacter_AimOffsets);
		MAKE_GENERAL_PROPERTY(struct FRotator, LastAimOffsets, "StructProperty TslGame.TslCharacter.*07679c2042", g_PatternData.offset_ATslCharacter_LastAimOffsets);
		MAKE_GENERAL_PROPERTY(void*, ScopingCamera, "ObjectProperty TslGame.TslCharacter.ScopingCamera", g_PatternData.offset_ATslCharacter_ScopingCamera);
		MAKE_GENERAL_PROPERTY(int, SpectatedCount, "IntProperty TslGame.TslCharacter.*1824801210", g_PatternData.offset_ATslCharacter_SpectatedCount);
		MAKE_GENERAL_PROPERTY(int, LastTeamNum, "IntProperty TslGame.TslCharacter.*e5f7873066", g_PatternData.offset_ATslCharacter_LastTeamNum);
		MAKE_GENERAL_PROPERTY(FString, CharacterName, "StrProperty TslGame.TslCharacter.*c0c4b93015", g_PatternData.offset_ATslCharacter_CharacterName);
#endif

	public:
		bool GetUnkBool();

		class ATslWeapon GetWeaponByIndex(int NumOfWeapon);
		struct FRotator GetControlRotation_CP();//当前摇摆
		struct FRotator GetRecoilADSRotation_CP();//当前摇摆
		struct FRotator GetScopingCameraBreath();//当前摇摆
		float GetLeanLeftAlpha_CP();
		float GetLeanRightAlpha_CP();
		float GetScopingAlpha_CP();
		bool GetIsScoping_CP();
		bool GetIsAiming_CP();
		bool GetIsReloading_CP();
	public:

		bool _Female;
		int _LastTeamNum;
		int _SpectatedCount;
		int _CharacterState;

		void* _CurrentWeapon;
		void* _EquippedWeapon;
		byte	  _WeaponIndex;
		int32_t	  _EncryptWeaponID;

		void* _Mesh;
		TArray<FTransform> _ComponentSpaceTransformsArray;
		FQuat _CacheLocalBound1;
		FQuat _CacheLocalBound2;
		std::vector<FTransform> _BoneTransFormTable;
		int _ReadType = 0;

		bool _isInFog;
		UCHAR _bCheckAsyncSceneOnMove;
		void* _AnimScriptInstance;
		bool _IsAiming_CP;
		float _ScopingAlpha_CP;
		byte _IsReloading_CP;
		int _HealthFlag1;
		int _HealthFlag2;
		int _HealthFlag3;
		int _HealthIndex1;
		int _HealthXorKeys;
		float _EncryptHealth;
		float _Health;
		float _HealthMax;
		float _GroggyHealth;
		float _GroggyHealthMax;
		float _LastRenderTimeOnScreen;
		float _LastRenderTime;
		float _LastSubmitTime;
		FRotator _AimOffsets;
		FRotator _LastAimOffsets;

		ptr_t _EncryptTeam;
		void* _Team;

		FVector _Root_RelativeLocation;
		FTransform _ComponentToWorld;
		ptr_t _EncryptPlayerState;
		ptr_t _EncryptPlayerState2;
		void* _PlayerState;
		FTslPlayerStatistics _PlayerStatistics;
		float _DamageDealtOnEnemy;
		int _KillNum;
		int _AssistsNum;
		int _SurvivalTier;
		int _SurvivalLevel;
		bool _InitPartner;
		uint8_t _PartnerLevel;
		bool _isParthers;
		std::string _PlayerName;
		std::string _NameplateId;
		std::string _EmblemId;
		std::string _ClanTag;
		bool _isDisconnected;
		bool _bBlackList;
		std::string _ShowName;
		std::string _RankName;
		std::string _TeamName;
		float _KDA;
		int _CurrentRankPoint;
		float _AvgDamage;
		std::string _RankText;
		std::string _RankIcon;
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class TslAIController : public AController
	{
	public:
		TslAIController(const UObject& object) :AController(object) {}
		TslAIController(void* object)
			:AController(object)
		{}
		TslAIController(uint64_t object) :TslAIController(reinterpret_cast<void*>(object)) {}
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class APlayerController : public AController
	{
	public:
		APlayerController(const UObject& object) :AController(object), AcknowledgedPawn(object.GetPtr(), g_PatternData.offset_APlayerController_AcknowledgedPawn) {}
		APlayerController(void* object)
			:AController(object),
			AcknowledgedPawn(object, g_PatternData.offset_APlayerController_AcknowledgedPawn)
		{}
		APlayerController(uint64_t object) :APlayerController(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//0F 29 74 24 20 0F 28 F1 FF 90 ?? ?? ?? ?? 84 C0 75 ?? F3 0F 59 B3 ?? ?? ?? ?? EB + 0x12] - 420
		UEncryptedObjectProperty<APawn, DecryptFunc::General, "EncryptedObjectProperty Engine.PlayerController.AcknowledgedPawn"_hash> AcknowledgedPawn;
		//8B ? ? ? 00 00 EB ? 48 8B ? ? ? 00 00 48 85 ? 74 ? 0F 57 + 8
		//0F 84 ? ? ? ? 48 8B ? ? ? 00 00 F2 0F 10 ? ? ? 00 00 F2 0F 11 + 6
		MAKE_OBJECT_PROPERTY(APlayerCameraManager, PlayerCameraManager, "ObjectProperty Engine.PlayerController.PlayerCameraManager");
		//0F 28 ? 48 8B ? E8 ? ? ? ? 48 8B ? E8 ? ? ? ? ? 8B ? ? 38 ? ? ? 00 00 0xE,1,5 GetMyHUD Call Rcx->PlayerController
		//0F 28 ? 48 8B ? E8 ? ? ? ? 48 8B ? E8 ? ? ? ? ? 8B ? 38 ? ? ? 00 00 0xE,1,5 GetMyHUD Call Rcx->PlayerController
		//E8 ? ? ? ? 48 83 ? ? ? 00 00 00 0F 84 ? ? ? ? 48 8B 7D 10 + 5
		//48 8B ? ? ? 00 00 48 8B 01 FF 90 ? ? 00 00 48 8B 44 24 ? ? ? ? ? ? 00 00 ? ? ? ? ? 00 00 + 0
		MAKE_OBJECT_PROPERTY(AHUD, MyHUD, "ObjectProperty Engine.PlayerController.MyHUD");
		//41 8B ? ? ? 00 00 C1 E8 02 A8 01 74 ? 48 63 + 0
		MAKE_EXTRA_STRUCT_PROPERTY(int, MapSig, "ObjectProperty Engine.PlayerController.InactiveStateInputComponent", -0x8);
		//FF 90 ? ? 00 00 F2 0F 10 ? ? ? ? ? F2 0F 11 ? ? ? 00 00 8B ? ? ? ? ? ? ? ? ? 00 00 0F 28 74 24 ? ? 8D + 0xE
		MAKE_EXTRA_STRUCT_PROPERTY(FRotator*, RotationInput, "FloatProperty Engine.PlayerController.InputYawScale", -0xC);
		//FF 90 ? ? 00 00 F3 0F 59 ? ? ? 00 00 F3 0F 11 74 24 + 6
		MAKE_GENERAL_PROPERTY(float, InputYawScale, "FloatProperty Engine.PlayerController.InputYawScale");
		MAKE_GENERAL_PROPERTY(float, InputPitchScale, "FloatProperty Engine.PlayerController.InputPitchScale");
		MAKE_GENERAL_PROPERTY(float, InputRollScale, "FloatProperty Engine.PlayerController.InputRollScale");
#else
		UEncryptedObjectProperty<APawn, DecryptFunc::General> AcknowledgedPawn;
		MAKE_OBJECT_PROPERTY(APlayerCameraManager, PlayerCameraManager, "ObjectProperty Engine.PlayerController.PlayerCameraManager", g_PatternData.offset_APlayerController_PlayerCameraManager);
		MAKE_OBJECT_PROPERTY(AHUD, MyHUD, "ObjectProperty Engine.PlayerController.*9616f4b2f3", g_PatternData.offset_APlayerController_MyHUD);
		MAKE_STRUCT_PROPERTY(int, MapSig, "ObjectProperty Engine.PlayerController.*01ebf0b348", g_PatternData.offset_APlayerController_MapSig);
		MAKE_STRUCT_PROPERTY(FRotator*, RotationInput, "FloatProperty Engine.PlayerController.InputYawScale", g_PatternData.offset_APlayerController_RotationInput);
		MAKE_GENERAL_PROPERTY(float, InputYawScale, "FloatProperty Engine.PlayerController.InputYawScale", g_PatternData.offset_APlayerController_InputYawScale);
		MAKE_GENERAL_PROPERTY(float, InputPitchScale, "FloatProperty Engine.PlayerController.InputPitchScale", g_PatternData.offset_APlayerController_InputPitchScale);
		MAKE_GENERAL_PROPERTY(float, InputRollScale, "FloatProperty Engine.PlayerController.InputRollScale", g_PatternData.offset_APlayerController_InputRollScale);
#endif

	public:
		void GetPlayerViewPoint(FVector& out_Location, FRotator& out_Rotation);
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ATslPlayerController : public APlayerController
	{
	public:
		ATslPlayerController(const UObject& object) :APlayerController(object) {}
		ATslPlayerController(void* object)
			:APlayerController(object)
		{}
		ATslPlayerController(uint64_t object) :APlayerController(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//F3 0F 10 ? ? ? 00 00 F3 0F 59 ? ? ? ? ? 0F 2F ? ? ? 00 00 77 ? 48 + 0x10
		//F3 0F 10 ? ? ? 00 00 0F 5A ? 0F 5A ? F2 0F ? ? ? ? 00 00 F2 0F 59 + 0
		MAKE_GENERAL_PROPERTY(float, MaxValidPing, "FloatProperty TslGame.TslPlayerController.MaxValidPing");//调成1可中断连接
#else
		MAKE_GENERAL_PROPERTY(float, MaxValidPing, "FloatProperty TslGame.TslPlayerController.MaxValidPing", g_PatternData.offset_ATslPlayerController_MaxValidPing);//调成1可中断连接
#endif
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class APlayerState :public AActor
	{
	public:
		APlayerState(const UObject& object) :AActor(object) {}
		APlayerState(void* object)
			:AActor(object)
		{}
		APlayerState(uint64_t object) :APlayerState(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//74 ? ? 8B ? ? ? 00 00 EB ? ? 8D ? ? ? ? ? 4D 2B + 2
		//74 ? 48 63 ? ? ? 00 00 48 8B ? ? ? 00 00 89 ? 08 85 ? 75 ? 44 89 ? 0C EB + 2
		//74 ? 33 ? 48 89 ? 24 ? 48 63 ? ? ? 00 00 48 8B ? ? ? 00 00 89 ? 24 ? 85 ? 75 ? 89 ? 24 + 0x10
		MAKE_GENERAL_PROPERTY(FString, PlayerName, "StrProperty Engine.PlayerState.PlayerName");
		//0F 4D C2 88 ? ? ? 00 00 C3 0F 57 + 3
		MAKE_GENERAL_PROPERTY(unsigned char, Ping, "ByteProperty Engine.PlayerState.Ping");
#else
		MAKE_GENERAL_PROPERTY(FString, PlayerName, "StrProperty Engine.PlayerState.PlayerName", g_PatternData.offset_APlayerState_PlayerName);
		MAKE_GENERAL_PROPERTY(unsigned char, Ping, "ByteProperty Engine.PlayerState.Ping", g_PatternData.offset_APlayerState_Ping);
#endif



	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class ATslPlayerState :public APlayerState
	{
	public:
		ATslPlayerState(const UObject& object) :APlayerState(object) {}
		ATslPlayerState(void* object)
			:APlayerState(object)
		{}
		ATslPlayerState(uint64_t object) :APlayerState(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//44 8B ? ? ? 00 00 ? 8D ? ? ? 00 00 ? 8B ? ? 8B ? 44 8B + 7
		//48 8B 01 ?? 8D ?? ?? ?? 00 00 41 ?? ?? ?? 8B ?? 41 + 3
		//48 8B 01 ?? 8D ?? ?? ?? 00 00 41 ?? ?? ?? 8B ?? 40 0F + 3
		MAKE_STRUCT_PROPERTY(FTslPlayerStatistics, PlayerStatistics, "StructProperty TslGame.TslPlayerState.PlayerStatistics");
		//搜StructProperty TslGame.TslPlayerState.*
		//上一个偏移-当前偏移=0x58
		//BoolProperty TslGame.TslPlayerState.bExitToDesktop 下面第三个
		MAKE_STRUCT_PROPERTY(FWuPubgIdData, PubgIdData, "StructProperty TslGame.TslPlayerState.PubgIdData");
#else
		MAKE_STRUCT_PROPERTY(FTslPlayerStatistics, PlayerStatistics, "StructProperty TslGame.TslPlayerState.*017bf8aecf", g_PatternData.offset_ATslPlayerState_PlayerStatistics);
		MAKE_STRUCT_PROPERTY(FWuPubgIdData, PubgIdData, "StructProperty TslGame.TslPlayerState.*2d2dc0ef14", g_PatternData.offset_ATslPlayerState_PubgIdData);
		MAKE_STRUCT_PROPERTY(float, DamageDealtOnEnemy, "FloatProperty TslGame.TslPlayerState.DamageDealtOnEnemy", g_PatternData.offset_ATslPlayerState_DamageDealtOnEnemy);
		MAKE_STRUCT_PROPERTY(uint8_t, PartnerLevel, "EnumProperty TslGame.TslPlayerStateBase.PartnerLevel", g_PatternData.offset_ATslPlayerState_PartnerLevel);

#endif
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class AWheeledVehicle : public APawn
	{
	public:
		AWheeledVehicle(const UObject& object) :APawn(object) {}
		AWheeledVehicle(void* object)
			:APawn(object)
		{}
		AWheeledVehicle(uint64_t object) :AWheeledVehicle(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//48 85 ? 0F 84 ? ? ? ? ? 8B ? ? ? 00 00 ? 85 ? 0F 84 ? ? ? ? ? 8B ? ? 89 ? 24 ? ? 00 00 + 9
		//48 8B ? ? ? 00 00 48 8B 01 45 33 C9 ? 8B ? ? ? ? ? 48 8D ? ? FF 90 + 0
		//41 ? ? 48 8B ? ? ? 00 00 40 ? ? E8 ? ? ? ? 44 ? ? ? ? 00 00 + 3
		//48 8B ? ? ? 00 00 48 85 ? 0F 84 ? ? ? ? 83 ? 20 00 48 8B ? 83 ? 24 00 + 0
		MAKE_OBJECT_PROPERTY(USkeletalMeshComponent, Mesh, "ObjectProperty PhysXVehicles.WheeledVehicle.Mesh");
		//UTslWheeledVehicleMovement
		//48 8B ? ? ? 00 00 48 8B ? 0F 29 ? 24 ? 48 85 ? 0F 84 ? ? ? ? 48 8D + 0
		//8B ? ? ? 00 00 45 33 ? 0F 29 ? ? 48 8B ? 0F 29 ? ? 44 0F 29 ? ? 44 0F 29 ? ? 41 + 0
		MAKE_OBJECT_PROPERTY(UWheeledVehicleMovementComponent, VehicleMovement, "ObjectProperty PhysXVehicles.WheeledVehicle.VehicleMovement");
#else
		MAKE_OBJECT_PROPERTY(USkeletalMeshComponent, Mesh, "ObjectProperty PhysXVehicles.WheeledVehicle.Mesh", g_PatternData.offset_AWheeledVehicle_Mesh);
		MAKE_OBJECT_PROPERTY(UWheeledVehicleMovementComponent, VehicleMovement, "ObjectProperty PhysXVehicles.WheeledVehicle.VehicleMovement", g_PatternData.offset_AWheeledVehicle_VehicleMovement);
#endif


	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class ATslFloatingVehicle : public APawn
	{
	public:
		ATslFloatingVehicle(const UObject& object) :APawn(object) {}
		ATslFloatingVehicle(void* object)
			:APawn(object)
		{}
		ATslFloatingVehicle(uint64_t object) :ATslFloatingVehicle(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//FF 90 ? ? 00 00 48 83 ? ? ? 00 00 00 74 ? ? 84 ? 74 + 6
		//FF 90 ? ? 00 00 ? 8B ? ? ? 00 00 ? 8d ? 24 ? 48 8B ? ? ? 00 00 F3 + 0x12
		//44 0F 28 ? E8 ? ? ? ? 48 8B ? ? ? 00 00 ? 8D ? ? ? 8B ? ? ? 00 00 + 9
		//0f 84 ? ? ? ? 48 8b ? ? ? 00 00 ? 8D ? ? ? 8B ? ? ? 00 00 45 33 + 6
		MAKE_OBJECT_PROPERTY(USkeletalMeshComponent, Mesh, "ObjectProperty TslGame.TslFloatingVehicle.MeshComponent");
		//48 8B ? ? ? 00 00 F6 80 ? ? 00 00 02 0F 85 ? ? ? ? F3 + 0
		//48 8B ? ? ? 00 00 C6 80 ? ? 00 00 00 48 8B ? ? ? 00 00 80 B8 ? ? 00 00 00 74 + 0
		//48 83 ? ? ? 00 00 00 48 8B ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 44 0F 29 48 ? 0F + 0
		MAKE_OBJECT_PROPERTY(UTslVehicleCommonComponent, VehicleCommonComponent, "ObjectProperty TslGame.TslFloatingVehicle.VehicleCommonComponent");
#else
		MAKE_OBJECT_PROPERTY(USkeletalMeshComponent, Mesh, "ObjectProperty TslGame.TslFloatingVehicle.MeshComponent", g_PatternData.offset_ATslFloatingVehicle_Mesh);
		MAKE_OBJECT_PROPERTY(UTslVehicleCommonComponent, VehicleCommonComponent, "ObjectProperty TslGame.TslFloatingVehicle.VehicleCommonComponent", g_PatternData.offset_ATslFloatingVehicle_VehicleCommonComponent);
#endif

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class ATslWheeledVehicle :public AWheeledVehicle
	{
	public:
		ATslWheeledVehicle(const UObject& object) :AWheeledVehicle(object) {}
		ATslWheeledVehicle(void* object)
			:AWheeledVehicle(object)
		{}
		ATslWheeledVehicle(uint64_t object) :ATslWheeledVehicle(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//80 ? ? ? 00 00 00 0F 85 ? ? ? ? 80 ? ? ? 00 00 00 0F 85 ? ? ? ? 48 8B ? ? ? 00 00 48 85 ? 0F 84 ? ? ? ? 8B + 0x1A
		//74 ? 48 8B ? ? ? 00 00 F6 80 ? ? 00 00 01 74 ? 48 8B ? E8 + 2
		//48 8B ? ? ? 00 00 88 ? ? ? 00 00 48 8B ? ? ? 00 00 38 + 0
		//0F 85 ? ? ? ? 48 8B ? ? ? 00 00 ? 98 ? ? 00 00 0F 85 + 6
		MAKE_OBJECT_PROPERTY(UTslVehicleCommonComponent, VehicleCommonComponent, "ObjectProperty TslGame.TslWheeledVehicle.VehicleCommonComponent");
#else
		MAKE_OBJECT_PROPERTY(UTslVehicleCommonComponent, VehicleCommonComponent, "ObjectProperty TslGame.TslWheeledVehicle.VehicleCommonComponent", g_PatternData.offset_ATslWheeledVehicle_VehicleCommonComponent);
#endif


	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ATslFlyingVehilce :public ATslWheeledVehicle
	{
	public:
		ATslFlyingVehilce(const UObject& object) :ATslWheeledVehicle(object) {}
		ATslFlyingVehilce(void* object)
			:ATslWheeledVehicle(object)
		{}
		ATslFlyingVehilce(uint64_t object) :ATslFlyingVehilce(reinterpret_cast<void*>(object)) {}
	public:
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ADroppedItemGroup :public AActor
	{
	public:
		ADroppedItemGroup(const UObject& object) :AActor(object) {}
		ADroppedItemGroup(void* object)
			:AActor(object)
		{}
		ADroppedItemGroup(uint64_t object) :ADroppedItemGroup(reinterpret_cast<void*>(object)) {}
	public:
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class ADroppedItem : public AActor
	{
	public:
		ADroppedItem(const UObject& object) :AActor(object), Item(object.GetPtr(), g_PatternData.offset_ADroppedItem_Item) {}
		ADroppedItem(void* object)
			:AActor(object),
			Item(object, g_PatternData.offset_ADroppedItem_Item)
		{}
		ADroppedItem(uint64_t object) :ADroppedItem(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//48 8B 44 24 20 48 83 B8 ? ? ? ? 00 0F 84 ? ? ? ? ? 8B + 0x13
		UEncryptedObjectProperty<class UItem, DecryptFunc::General, "EncryptedObjectProperty TslGame.DroppedItem.Item"_hash> Item;
#else
		UEncryptedObjectProperty<class UItem, DecryptFunc::General> Item;
#endif
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class USceneInteractionComponent : public USceneComponent
	{
	public:
		USceneInteractionComponent(const UObject& object) :USceneComponent(object) {}
		USceneInteractionComponent(void* object)
			:USceneComponent(object)
		{}
		USceneInteractionComponent(uint64_t object) :USceneInteractionComponent(reinterpret_cast<void*>(object)) {}
	public:
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class UItem : public UObject
	{
	public:
		UItem(const UObject& object) :UObject(object) {}
		UItem(void* object)
			:UObject(object)
		{}
		UItem(uint64_t object) :UItem(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//74 ? 48 8B 89 ? ? ? ? 48 81 C1 ? ? ? ? 48 8D + 2
		MAKE_EXTRA_GENERAL_PROPERTY(ptr_t, ItemTableRowPtr, "ObjectProperty TslGame.Item.CastableImplement", 8);
		MAKE_GENERAL_PROPERTY(int, StackCount, "IntProperty TslGame.Item.StackCount");
#else
		MAKE_GENERAL_PROPERTY(ptr_t, ItemTableRowPtr, "ObjectProperty TslGame.Item.CastableImplement", g_PatternData.offset_UItem_ItemTableRowPtr);
		MAKE_GENERAL_PROPERTY(int, StackCount, "IntProperty TslGame.Item.StackCount", g_PatternData.offset_UItem_StackCount);
#endif
		FText GetItemName();
		FText GetItemCategory();
		FText GetItemDetailedName();
		FText GetItemDescription();
		FName GetItemID();

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	private:
		//ToolTipIconSizeRatio
		MAKE_GENERAL_PROPERTY(int, ItemTableRowBase_ItemName, "TextProperty TslGame.*74018aab85.ItemName", g_PatternData.offset_ItemTableRowBase_ItemName);
		MAKE_GENERAL_PROPERTY(int, ItemTableRowBase_ItemCategory, "TextProperty TslGame.*74018aab85.ItemCategory", g_PatternData.offset_ItemTableRowBase_ItemCategory);
		MAKE_GENERAL_PROPERTY(int, ItemTableRowBase_ItemDetailedName, "TextProperty TslGame.*74018aab85.ItemDetailedName", g_PatternData.offset_ItemTableRowBase_ItemDetailedName);
		MAKE_GENERAL_PROPERTY(int, ItemTableRowBase_ItemDescription, "TextProperty TslGame.*74018aab85.ItemDescription", g_PatternData.offset_ItemTableRowBase_ItemDescription);
		MAKE_GENERAL_PROPERTY(int, ItemTableRowBase_ItemID, "NameProperty TslGame.*74018aab85.ItemID", g_PatternData.offset_ItemTableRowBase_ItemID);

		uint32_t GetItemNameOffset();
		uint32_t GetItemCategoryOffset();
		uint32_t GetItemDetailedNameOffset();
		uint32_t GetItemDescriptionOffset();
		uint32_t GetItemIDOffset();
	};

	class UAttachableItem : public UItem
	{
	public:
		UAttachableItem(const UObject& object) :UItem(object) {}
		UAttachableItem(void* object)
			:UItem(object)
		{}
		UAttachableItem(uint64_t object) :UItem(reinterpret_cast<void*>(object)) {}

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class UDroppedItemInteractionComponent : public USceneInteractionComponent
	{
	public:
		UDroppedItemInteractionComponent(const UObject& object)
			:USceneInteractionComponent(object), _LootItem(nullptr), _bItemInitialize(false), _UItem(nullptr), _ItemCategory(LootCategory::Unknown),
			_ItemHashName(0) {}
		UDroppedItemInteractionComponent(void* object)
			:USceneInteractionComponent(object), _LootItem(nullptr), _bItemInitialize(false), _UItem(nullptr), _ItemCategory(LootCategory::Unknown),
			_ItemHashName(0)
		{}
		UDroppedItemInteractionComponent(uint64_t object)
			:UDroppedItemInteractionComponent(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//E8 ? ? ? ? ? 8B ? 33 ? ? 8B ? FF 90 ? ? 00 00 FF ? 48 ? ? 08 + 0 Call 里面
		//E8 ? ? ? ? ? 8D ? ? ? 00 00 ? 8B ? ? 8B ? FF 50 ? F3 + 0 Call里面
		//48 83 EC 20 48 8B D9 48 39 91 ? ? ? ? 74 ? 48 89 91 ? ? ? ? 48 8B 89 + 7
		MAKE_OBJECT_PROPERTY(class UItem, Item, "ObjectProperty TslGame.DroppedItemInteractionComponent.Item");
#else
		MAKE_OBJECT_PROPERTY(class UItem, Item, "ObjectProperty TslGame.*a3c40da881.Item", g_PatternData.offset_UDroppedItemInteractionComponent_Item);
#endif
		//MAKE_OBJECT_PROPERTY(UAsyncStaticMeshComponent, StaticMeshComponent, "ObjectProperty TslGame.DroppedItemInteractionComponent.StaticMeshComponent");

		void* _UItem;
		LootCategory _ItemCategory;
		hash_t _ItemHashName;
		class Item* _LootItem;
		bool _bItemInitialize;
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ASubActor : public AActor
	{
	public:
		ASubActor(const UObject& object) :AActor(object) {}
		ASubActor(void* object)
			:AActor(object)
		{}
		ASubActor(uint64_t object) :ASubActor(reinterpret_cast<void*>(object)) {}
	public:

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class AItemSlotContainer : public ASubActor
	{
	public:
		AItemSlotContainer(const UObject& object) :ASubActor(object) {}
		AItemSlotContainer(void* object)
			:ASubActor(object)
		{}
		AItemSlotContainer(uint64_t object) :ASubActor(reinterpret_cast<void*>(object)) {}
	public:

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class AInventory : public AItemSlotContainer
	{
	public:
		AInventory(const UObject& object) :AItemSlotContainer(object) {}
		AInventory(void* object)
			:AItemSlotContainer(object)
		{}
		AInventory(uint64_t object) :AInventory(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//48 8B ? ? ? 00 00 33 ? 48 63 ? ? ? 00 00 8B ? 48 C1 ? 03 0F 29 ? ? 0F 57 ? 48 8D + 0
		//45 33 ? ? 8B ? ? ? 8B ? ? ? 00 00 ? 8B ? ? ? 85 ? 0F 84 ? ? ? ? 41 8B + 7
		//45 33 ? ? 8B ? ? ? 8B ? ? ? 00 00 ? 8B ? ? ? 85 ? 0F 84 ? ? ? ? 8B + 7
		//33 ? 48 8B ? ? ? 00 00 48 8B ? ? 48 85 ? 74 ? ? 8B ? ? ? 00 00 ? 8B ? 48 39 ? ? ? 00 00 + 2
		MAKE_GENERAL_PROPERTY(TArray<class UItem*>, Items, "ArrayProperty TslGame.Inventory.Items");
#else
		MAKE_GENERAL_PROPERTY(TArray<class UItem*>, Items, "ArrayProperty TslGame.Inventory.Items", g_PatternData.offset_AInventory_Items);
#endif
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UEquipableItem : public UItem
	{
	public:
		UEquipableItem(const UObject& object) :UItem(object) {}
		UEquipableItem(void* object)
			:UItem(object)
		{}
		UEquipableItem(uint64_t object) :UEquipableItem(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash

		//8A ? ? ? 00 00 48 8D ? ? ? 8B ? E8 ? ? ? ? 83 ? ? FF 0F 95 ? 84 c0 + 0
		//8A ? ? ? 00 00 48 8D ? ? ? 8B ? E8 ? ? ? ? 83 ? ? FF 0F 95 ? 45 33 ? 84 + 0
		MAKE_GENERAL_PROPERTY(EEquipSlotID, EquipSlotID, "EnumProperty TslGame.EquipableItem.EquipSlotID");
		MAKE_GENERAL_PROPERTY(float, DurabilityMax, "FloatProperty TslGame.EquipableItem.DurabilityMax");
		MAKE_GENERAL_PROPERTY(float, Durability, "FloatProperty TslGame.EquipableItem.Durability");
#else
		//NameProperty TslGame.TslBuff.OverlapId 往下一个类
		MAKE_GENERAL_PROPERTY(EEquipSlotID, EquipSlotID, "EnumProperty TslGame.*9addf7508e.*1ef4e86644", g_PatternData.offset_UEquipableItem_EquipSlotID);
		MAKE_GENERAL_PROPERTY(float, DurabilityMax, "FloatProperty TslGame.*9addf7508e.*c10dc0656c", g_PatternData.offset_UEquipableItem_DurabilityMax);
		MAKE_GENERAL_PROPERTY(float, Durability, "FloatProperty TslGame.*9addf7508e.Durability", g_PatternData.offset_UEquipableItem_Durability);
		MAKE_STRUCT_PROPERTY(FReplicatedSkinItem, ReplicatedSkinItem, "StructProperty TslGame.*7ed540bffc.*2c2dab77f3", g_PatternData.offset_UEquipableItem_ReplicatedSkinItem);
#endif


	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class AEquipment : public AItemSlotContainer
	{
	public:
		AEquipment(const UObject& object) :AItemSlotContainer(object) {}
		AEquipment(void* object)
			:AItemSlotContainer(object)
		{}
		AEquipment(uint64_t object) :AEquipment(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//78 ? 48 ? ? ? 8B ? ? ? 00 00 48 8B 04 C8 48 83 ? ? C3 + 5
		//48 8B ? ? 8B ? ? ? 00 00 48 63 ? ? ? 00 00 ? 8D ? ? ? 8B ? ? 8D + 3
		//48 0F BE ? ? ? ? 00 00 33 ? 48 8B ? ? ? 00 00 ? 8D ? ? ? 83 ? 00 + 0xB
		//E8 ? ? ? ? ? 8B ? ? ? 00 00 ? 63 ? ? ? 00 00 ? 8D ? ? ? 3B ? 74 ? ? 8B ? ? 85 ? 75 + 5
		MAKE_GENERAL_PROPERTY(TArray<class UEquipableItem*>, Items, "ArrayProperty TslGame.Equipment.Items");

#else
		MAKE_GENERAL_PROPERTY(TArray<class UEquipableItem*>, Items, "ArrayProperty TslGame.Equipment.Items", g_PatternData.offset_AEquipment_Items);
#endif
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class AItemExplorer : public AItemSlotContainer
	{
	public:
		AItemExplorer(const UObject& object) :AItemSlotContainer(object) {}
		AItemExplorer(void* object)
			:AItemSlotContainer(object)
		{}
		AItemExplorer(uint64_t object) :AItemExplorer(reinterpret_cast<void*>(object)) {}
	public:

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class AItemPackage : public AItemExplorer
	{
	public:
		AItemPackage(const UObject& object) :AItemExplorer(object) {}
		AItemPackage(void* object)
			:AItemExplorer(object)
		{}
		AItemPackage(uint64_t object) :AItemPackage(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//F3 0F 10 B0 ? ? ? ? F3 0F 59 B0 ? ? ? ? F3 0F 59 B0 + 0x10] + 0x10
		MAKE_GENERAL_PROPERTY(TArray<class UItem*>, Items, "ArrayProperty TslGame.ItemPackage.Items");
#else
		MAKE_GENERAL_PROPERTY(TArray<class UItem*>, Items, "ArrayProperty TslGame.ItemPackage.Items", g_PatternData.offset_AItemPackage_Items);
#endif

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ACarePackageItem : public AItemPackage
	{
	public:
		ACarePackageItem(const UObject& object) :AItemPackage(object) {}
		ACarePackageItem(void* object)
			:AItemPackage(object)
		{}
		ACarePackageItem(uint64_t object) :ACarePackageItem(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//MAKE_OBJECT_PROPERTY(UAsyncStaticMeshComponent, VisualStaticMesh, "ObjectProperty TslGame.CarePackageItem.VisualStaticMesh");
#else
		//MAKE_OBJECT_PROPERTY(UAsyncStaticMeshComponent, VisualStaticMesh, "ObjectProperty TslGame.CarePackageItem.VisualStaticMesh");
#endif

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class AFloorSnapItemPackage : public AItemPackage
	{
	public:
		AFloorSnapItemPackage(const UObject& object) :AItemPackage(object) {}
		AFloorSnapItemPackage(void* object)
			:AItemPackage(object)
		{}
		AFloorSnapItemPackage(uint64_t object) :AFloorSnapItemPackage(reinterpret_cast<void*>(object)) {}
	public:

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class ATslWeapon : public AActor
	{
	public:
		ATslWeapon(const UObject& object) :AActor(object), Mesh3P(object.GetPtr(), g_PatternData.offset_ATslWeapon_Mesh3P) {}
		ATslWeapon(void* object)
			:AActor(object), Mesh3P(object, g_PatternData.offset_ATslWeapon_Mesh3P)
		{}
		ATslWeapon(uint64_t object) :ATslWeapon(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//0F 84 ? ? ? ? 48 8B ? E8 ? ? ? ? 48 8B ? 48 85 ? 0F 84 ? ? ? ? 45 33 ? 83 ? 01 + 9] GetMesh3P Call Rcx->pWeapon
		//48 33 c4 48 89 44 24 ? 48 83 3d ? ? ? ? 00 48 8b ? ? 8b ? ? ? 00 00 + 0x13
		UEncryptedObjectProperty<UWeaponMeshComponent, DecryptFunc::General, "EncryptedObjectProperty TslGame.TslWeapon.Mesh3P"_hash> Mesh3P;
		//MuzzleFlashSocket
		MAKE_STRUCT_PROPERTY(FName, MuzzleAttachPoint, "NameProperty TslGame.TslWeapon.MuzzleAttachPoint");
		//CartridgeChamber
		MAKE_STRUCT_PROPERTY(FName, FiringAttachPoint, "NameProperty TslGame.TslWeapon.FiringAttachPoint");
		//84 D2 74 ? ? 8B ? ? ? 00 00 48 63 ? ? ? 00 00 48 8D 2C ? 48 3B DD + 4
		//48 89 ? ? ? 8B ? ? ? 00 00 48 63 ? ? ? 00 00 ? 8D ? ? ? 8B + 4
		MAKE_GENERAL_PROPERTY(TArray<class UAttachableItem*>, AttachedItem, "ArrayProperty TslGame.TslWeapon.AttachedItems");
		MAKE_STRUCT_PROPERTY(FReplicatedSkinParam, ReplicatedSkinParam, "StructProperty TslGame.TslWeapon.ReplicatedSkinParam");
#else
		UEncryptedObjectProperty<UWeaponMeshComponent, DecryptFunc::General> Mesh3P;
		MAKE_STRUCT_PROPERTY(FName, MuzzleAttachPoint, "NameProperty TslGame.TslWeapon.*bd7beca37f", g_PatternData.offset_ATslWeapon_MuzzleAttachPoint);
		MAKE_STRUCT_PROPERTY(FName, FiringAttachPoint, "NameProperty TslGame.TslWeapon.*0baa17636e", g_PatternData.offset_ATslWeapon_FiringAttachPoint);
		MAKE_GENERAL_PROPERTY(TArray<class UAttachableItem*>, AttachedItem, "ArrayProperty TslGame.TslWeapon.*f08b02f5a3", g_PatternData.offset_ATslWeapon_AttachedItem);
#endif


		UAsyncStaticMeshComponent GetAttachedComponent(int a2);
		UAsyncStaticMeshComponent GetAttachedComponent(TMap AttachedStaticComponentMap, int a2);

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ATslWeapon_Gun : public ATslWeapon
	{
	public:
		ATslWeapon_Gun(const UObject& object) :ATslWeapon(object) {}
		ATslWeapon_Gun(void* object)
			:ATslWeapon(object)
		{}
		ATslWeapon_Gun(uint64_t object) :ATslWeapon_Gun(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//ScopeAimCamera
		MAKE_STRUCT_PROPERTY(FName, ScopingAttachPoint, "NameProperty TslGame.TslWeapon_Gun.ScopingAttachPoint");
		//Focus
		//MAKE_STRUCT_PROPERTY(FName, ScopingFocusPoint, "NameProperty TslGame.TslWeapon_Gun.ScopingFocusPoint");
		//84 ? 74 ? 80 ? ? ? 00 00 02 40 8a ? 74 + 4
		//85 ? 74 ? 80 ? ? ? 00 00 02 40 8a ? 74 + 4
		//84 ? 74 ? 41 80 ? ? ? 00 00 02 40 8a ? 74 + 4
		//85 ? 74 ? 41 80 ? ? ? 00 00 02 40 8a ? 74 + 4
		MAKE_GENERAL_PROPERTY(int, AmmoPerClip, "IntProperty TslGame.TslWeapon_Gun.AmmoPerClip");
		//0f b7 ? ? ? 00 00 89 ? ? 0f b7 ? ? ? 00 00 89 ? ? E9 + 0
		MAKE_GENERAL_PROPERTY(int, CurrentAmmoData, "IntProperty TslGame.TslWeapon_Gun.CurrentAmmoData");
		//备弹 0f b7 ? ? ? 00 00 89 ? ? 0f b7 ? ? ? 00 00 89 ? ? E9 + 0xA

		//41 0F ? ? 38 ? ? ? 00 00 74 ? 8A ? 48 + 4
		//01 ? ? ? ? 00 00 74 ? 8A ? 48 8B ? E8 + 1
		//48 8B ? 24 ? 8A ? ? ? 00 00 88 ? ? ? 00 00 3C + 5
		MAKE_GENERAL_PROPERTY(char, bSpawnBulletFromBarrel, "BoolProperty TslGame.TslWeapon_Gun.bSpawnBulletFromBarrel");
		//3C 03 74 ? ? ? ? ? 00 00 75 ? F3 0F 59 + 4
		//48 8B ? 8A ? ? ? 00 00 FF 90 ? ? 00 00 0F B6 ? ? ? 00 00 + 3
		//E8 ? ? ? ? 44 8A ? ? ? 00 00 48 8D ? ? ? 00 00 F3 0F + 5
		//0F 57 ? 44 8a ? ? ? 00 00 48 8b ? ff 92 + 3
		MAKE_EXTRA_GENERAL_PROPERTY(unsigned char, bIsCanted, "BoolProperty TslGame.TslWeapon_Gun.bLoackLoadedAmmo", 1);
		//75 ? 38 ? ? ? 00 00 0F 85 ? ? ? ? 85 + 2
		//MAKE_GENERAL_PROPERTY(unsigned char, bAlwaysUseIronSightZeroing, "BoolProperty TslGame.TslWeapon_Gun.bAlwaysUseIronSightZeroing");
		//44 0F B6 ? 84 ? 75 ? 8B ? ? ? 00 00 C3 + 8
		//8a ? ? ? 00 00 48 8b ? e8 ? ? ? ? 8b ? 44 38 + 9] ->GetCurrentZeroLevel Call RCX->this
		//8a ?? ?? ?? 00 00 48 8b ?? e8 ?? ?? ?? ?? 85 ?? 78 + 9] ->GetCurrentZeroLevel Call RCX->this
		MAKE_GENERAL_PROPERTY(int, CurrentZeroLevel, "IntProperty TslGame.TslWeapon_Gun.CurrentZeroLevel");
		//44 0F B6 ? 84 ? 75 ? 8B ? ? ? 00 00 C3 + 0x15
		//MAKE_GENERAL_PROPERTY(int, CurrentCantedZeroLevel, "IntProperty TslGame.TslWeapon_Gun.CurrentCantedZeroLevel");
#else
		MAKE_STRUCT_PROPERTY(FName, ScopingAttachPoint, "NameProperty TslGame.TslWeapon_Gun.ScopingAttachPoint", g_PatternData.offset_ATslWeapon_Gun_ScopingAttachPoint);
		MAKE_STRUCT_PROPERTY(FName, ScopingFocusPoint, "NameProperty TslGame.TslWeapon_Gun.*7000f340bf", g_PatternData.offset_ATslWeapon_Gun_ScopingFocusPoint);
		MAKE_GENERAL_PROPERTY(int, AmmoPerClip, "IntProperty TslGame.TslWeapon_Gun.*59a2e39591", g_PatternData.offset_ATslWeapon_Gun_CurrentAmmoData - 4);
		MAKE_GENERAL_PROPERTY(int, CurrentAmmoData, "IntProperty TslGame.TslWeapon_Gun.*f1ee47654c", g_PatternData.offset_ATslWeapon_Gun_CurrentAmmoData);
		MAKE_GENERAL_PROPERTY(unsigned char, bSpawnBulletFromBarrel, "BoolProperty TslGame.TslWeapon_Gun.bSpawnBulletFromBarrel", g_PatternData.offset_bSpawnBulletFromBarrel);//fix
		MAKE_GENERAL_PROPERTY(unsigned char, bIsCanted, "BoolProperty TslGame.TslWeapon_Gun.*40c9bf525a", g_PatternData.offset_bIsCanted);//fix
		MAKE_GENERAL_PROPERTY(int, CurrentZeroLevel, "IntProperty TslGame.TslWeapon_Gun.*7d58496f95", g_PatternData.offset_ATslWeapon_Gun_CurrentZeroLevel);
#endif
		//Function TslGame.TslWeapon_Gun.ClientNotifyAmmo
		//TslGame.TslWeapon_Gun.ServerSetHip
		float GetZeroingDistance(unsigned char bCanted);
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ATslWeapon_Trajectory : public ATslWeapon_Gun
	{
	public:
		ATslWeapon_Trajectory(const UObject& object) :ATslWeapon_Gun(object) {}
		ATslWeapon_Trajectory(void* object)
			:ATslWeapon_Gun(object)
		{}
		ATslWeapon_Trajectory(uint64_t object) :ATslWeapon_Trajectory(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//45 0F 57 ? F3 0F 59 ? ? ? 00 00 33 ? 45 + 4
		MAKE_GENERAL_PROPERTY(float, TrajectoryGravityZ, "FloatProperty TslGame.TslWeapon_Trajectory.TrajectoryGravityZ");
#else

		MAKE_GENERAL_PROPERTY(float, TrajectoryGravityZ, "FloatProperty TslGame.TslWeapon_Trajectory.TrajectoryGravityZ", g_PatternData.offset_ATslWeapon_Trajectory_TrajectoryGravityZ);

		MAKE_GENERAL_PROPERTY(int, WeaponTrajectoryData, "ObjectProperty TslGame.TslWeapon_Trajectory.WeaponTrajectoryData", g_PatternData.offset_ATslWeapon_Trajectory_WeaponTrajectoryData);
		MAKE_GENERAL_PROPERTY(int, TrajectoryConfig, "StructProperty TslGame.WeaponTrajectoryData.*02861ac621", g_PatternData.offset_WeaponTrajectoryData_TrajectoryConfig);
#endif
	public:
		FVector GetAdjustedAim();
		FVector GetMuzzleLocation();
		FVector GetFiringLocation();
		FVector GetCameraDamageStartLocation(const FVector& AimDir);
		FWeaponTrajectoryConfig GetWeaponTrajectoryConfig();
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ATslWeapon_Throwable : public ATslWeapon
	{
	public:
		ATslWeapon_Throwable(const UObject& object) :ATslWeapon(object) {}
		ATslWeapon_Throwable(void* object)
			:ATslWeapon(object)
		{}
		ATslWeapon_Throwable(uint64_t object) :ATslWeapon_Throwable(reinterpret_cast<void*>(object)) {}
	public:
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();


	};

	class ATslWeapon_Melee : public ATslWeapon
	{
	public:
	public:
		ATslWeapon_Melee(const UObject& object) :ATslWeapon(object) {}
		ATslWeapon_Melee(void* object)
			:ATslWeapon(object)
		{}
		ATslWeapon_Melee(uint64_t object) :ATslWeapon_Melee(reinterpret_cast<void*>(object)) {}
	public:

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ATslWeapon_Gun_Projectile : public ATslWeapon_Gun
	{
	public:
		ATslWeapon_Gun_Projectile(const UObject& object) :ATslWeapon_Gun(object) {}
		ATslWeapon_Gun_Projectile(void* object)
			:ATslWeapon_Gun(object)
		{}
		ATslWeapon_Gun_Projectile(uint64_t object) :ATslWeapon_Gun_Projectile(reinterpret_cast<void*>(object)) {}
	public:

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class UShapeComponent : public UPrimitiveComponent
	{
	public:
		UShapeComponent(const UObject& object) :UPrimitiveComponent(object) {}
		UShapeComponent(void* object)
			:UPrimitiveComponent(object)
		{}
		UShapeComponent(uint64_t object) :UShapeComponent(reinterpret_cast<void*>(object)) {}
	public:

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class USphereComponent : public UShapeComponent
	{
	public:
		USphereComponent(const UObject& object) :UShapeComponent(object) {}
		USphereComponent(void* object)
			:UShapeComponent(object)
		{}
		USphereComponent(uint64_t object) :USphereComponent(reinterpret_cast<void*>(object)) {}
	public:

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class ATslProjectile : public AActor
	{
	public:
		ATslProjectile(const UObject& object) :AActor(object) {}
		ATslProjectile(void* object)
			:AActor(object)
		{}
		ATslProjectile(uint64_t object) :ATslProjectile(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//MAKE_OBJECT_PROPERTY(class UProjectileMovementComponent, MovementComp, "ObjectProperty TslGame.TslProjectile.MovementComp");
		MAKE_STRUCT_PROPERTY(struct FProjectileData, ProjectileConfig, "StructProperty TslGame.TslProjectile.ProjectileConfig");
		MAKE_OBJECT_PROPERTY(class USphereComponent, CollisionComp, "ObjectProperty TslGame.TslProjectile.CollisionComp");
		MAKE_GENERAL_PROPERTY(float, ExplosionDelay, "FloatProperty TslGame.TslProjectile.ExplosionDelay");
		//89 46 08 F3 0F 10 ? ? ? 00 00 4C 8D ? 24 ? ? 00 00 + 3
		//80 ? ? ? 00 00 00 0F 85 ? ? ? ? F3 0F 10 87 + 0xD
		MAKE_GENERAL_PROPERTY(float, TimeTillExplosion, "FloatProperty TslGame.TslProjectile.TimeTillExplosion");
#else
		//MAKE_OBJECT_PROPERTY(class UProjectileMovementComponent, MovementComp, "ObjectProperty TslGame.TslProjectile.MovementComp");
		MAKE_STRUCT_PROPERTY(struct FProjectileData, ProjectileConfig, "StructProperty TslGame.TslProjectile.ProjectileConfig", g_PatternData.offset_ATslProjectile_ProjectileConfig);
		MAKE_OBJECT_PROPERTY(class USphereComponent, CollisionComp, "ObjectProperty TslGame.TslProjectile.CollisionComp", g_PatternData.offset_ATslProjectile_CollisionComp);
		MAKE_GENERAL_PROPERTY(float, ExplosionDelay, "FloatProperty TslGame.TslProjectile.ExplosionDelay", g_PatternData.offset_ATslProjectile_ExplosionDelay);
		MAKE_GENERAL_PROPERTY(float, TimeTillExplosion, "FloatProperty TslGame.TslProjectile.TimeTillExplosion", g_PatternData.offset_ATslProjectile_TimeTillExplosion);
#endif

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	private:
		std::list<FVector> Path;
	};

	class UTslSettings : public UObject
	{
	public:
		UTslSettings(const UObject& object) :UObject(object) {}
		UTslSettings(void* object)
			:UObject(object)
		{}
		UTslSettings(uint64_t object) :UTslSettings(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//F3 0F 10 ? ? ? 00 00 48 8D ? 24 ? 48 8B ? ? ? 00 00 48 8B ? F3 + 0
		MAKE_GENERAL_PROPERTY(float, BallisticDragScale, "FloatProperty TslGame.TslSettings.BallisticDragScale");//弹道阻力比例
		//E8 ? ? ? ? F3 ? 0F 10 ? ? ? 00 00 E8 ? ? ? ? ? 01 48 8B ? E8 ? ? ? ? 48 8B ? ? ? 00 00 80 + 5
		MAKE_GENERAL_PROPERTY(float, BallisticDropScale, "FloatProperty TslGame.TslSettings.BallisticDropScale");//弹道下坠比例
#else
		MAKE_GENERAL_PROPERTY(float, BallisticDragScale, "FloatProperty TslGame.TslSettings.BallisticDragScale", g_PatternData.offset_UTslSettings_BallisticDragScale);//弹道阻力比例
		MAKE_GENERAL_PROPERTY(float, BallisticDropScale, "FloatProperty TslGame.TslSettings.BallisticDropScale", g_PatternData.offset_UTslSettings_BallisticDropScale);//弹道下坠比例
#endif


		static class UTslSettings GetTslSettings();
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UKismetSystemLibrary : public UObject
	{
	public:
		UKismetSystemLibrary(const UObject& object) :UObject(object) {}
		UKismetSystemLibrary(void* object)
			:UObject(object)
		{}
		UKismetSystemLibrary(uint64_t object) :UKismetSystemLibrary(reinterpret_cast<void*>(object)) {}

		bool STATIC_LineTraceMulti(class UObject** WorldContextObject, struct FVector* Start, struct FVector* End, TEnumAsByte<ETraceTypeQuery>* TraceChannel, bool* bTraceComplex, TArray<class AActor*>* ActorsToIgnore, TEnumAsByte<EDrawDebugTrace>* DrawDebugType, bool* bIgnoreSelf, struct FLinearColor* TraceColor, struct FLinearColor* TraceHitColor, float* DrawTime, TArray<struct FHitResult>* OutHits);
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();

	};

	class UBodySetup : public UObject
	{
	public:
		UBodySetup(const UObject& object) :UObject(object) {}
		UBodySetup(void* object)
			:UObject(object)
		{}
		UBodySetup(uint64_t object) :UObject(reinterpret_cast<void*>(object)) {}

	public:
#ifndef ObjectNameHash
		MAKE_STRUCT_PROPERTY(struct FWalkableSlopeOverride, WalkableSlopeOverride, "StructProperty Engine.BodySetup.WalkableSlopeOverride");
#else
		MAKE_STRUCT_PROPERTY(struct FWalkableSlopeOverride, WalkableSlopeOverride, "StructProperty Engine.BodySetup.*49d15983b6", g_PatternData.offset_UBodySetup_WalkableSlopeOverride);
#endif
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UVisual : public UObject
	{
	public:
		UVisual(const UObject& object) : UObject(object) {}
		UVisual(void* object) :
			UObject(object) {}
		UVisual(uint64_t object) : UObject(reinterpret_cast<void*>(object)) {}
	public:
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UPanelSlot : public UVisual
	{
	public:
		UPanelSlot(const UObject& object) : UVisual(object) {}
		UPanelSlot(void* object) :UVisual(object) {}
		UPanelSlot(uint64_t object) : UPanelSlot(reinterpret_cast<void*>(object)) {}
	public:
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UWidget :public UVisual
	{
	public:
		UWidget(const UObject& object) : UVisual(object) {}
		UWidget(void* object)
			: UVisual(object) {}
		UWidget(uint64_t object) : UWidget(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//88 ? ? ? 00 00 48 83 ? ? ? 00 00 00 0F 85 ? ? ? ? 48 83 65 + 0
		//88 ? ? ? 00 00 48 83 ? ? ? 00 00 00 75 ? 48 83 65 + 0
		MAKE_GENERAL_PROPERTY(ESlateVisibility, Visibility, "EnumProperty UMG.Widget.Visibility");
		//48 8D ? ? ? 00 00 E8 ? ? ? ? 48 8B ? E8 ? ? ? ? ? 8B ? ? 8B ? ? ? 01 39 + 0xF,1,5 Call 里面 mov rbx,[rcx+30]
		MAKE_OBJECT_PROPERTY(class UPanelSlot, Slot, "ObjectProperty UMG.Widget.Slot");
#else
		MAKE_GENERAL_PROPERTY(ESlateVisibility, Visibility, "EnumProperty UMG.Widget.Visibility", g_PatternData.offset_UWidget_Visibility);
		MAKE_OBJECT_PROPERTY(class UPanelSlot, Slot, "ObjectProperty UMG.Widget.Slot", g_PatternData.offset_UWidget_Slot);
#endif

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UCanvasPanelSlot : public UPanelSlot
	{
	public:
		UCanvasPanelSlot(const UObject& object) : UPanelSlot(object) {}
		UCanvasPanelSlot(void* object) :UPanelSlot(object) {}
		UCanvasPanelSlot(uint64_t object) : UCanvasPanelSlot(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//33 DB 48 85 DB 74 ? 48 8B ? 24 ? 48 8B ? E8 ? ? ? ? 48 8B + 0xF,1,5]  第一个 movss,xmm0-> movss [rcx+38],xmm0
		//F3 0F 11 ? ? F3 0F 10 ? 24 ? F3 0F 11 ? ? 48 8B ? ? 48 85 ? 74 + 0 有2个地址 取最小的一个
		MAKE_STRUCT_PROPERTY(struct FAnchorData, LayoutData, "StructProperty UMG.CanvasPanelSlot.LayoutData");
#else
		MAKE_STRUCT_PROPERTY(struct FAnchorData, LayoutData, "StructProperty UMG.*9fab40c603.LayoutData", g_PatternData.offset_UCanvasPanelSlot_LayoutData);
#endif
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UUserWidget :public UWidget
	{
	public:
		UUserWidget(const UObject& object) : UWidget(object) {}
		UUserWidget(void* object) : UWidget(object) {}
		UUserWidget(uint64_t object) : UUserWidget(reinterpret_cast<void*>(object)) {}
	public:
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UTslUserWidget :public UUserWidget
	{
	public:
		UTslUserWidget(const UObject& object) : UUserWidget(object) {}
		UTslUserWidget(void* object) : UUserWidget(object) {}
		UTslUserWidget(uint64_t object) : UTslUserWidget(reinterpret_cast<void*>(object)) {}
	public:
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UUmgBaseWidget :public UTslUserWidget
	{
	public:
		UUmgBaseWidget(const UObject& object) : UTslUserWidget(object) {}
		UUmgBaseWidget(void* object) : UTslUserWidget(object) {}
		UUmgBaseWidget(uint64_t object) : UUmgBaseWidget(reinterpret_cast<void*>(object)) {}
	public:
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UMapGridWidget :public UUmgBaseWidget
	{
	public:
		UMapGridWidget(const UObject& object) : UUmgBaseWidget(object) {}
		UMapGridWidget(void* object) : UUmgBaseWidget(object) {}
		UMapGridWidget(uint64_t object) : UMapGridWidget(reinterpret_cast<void*>(object)) {}
	public:
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UBlockInputUserWidget :public UUmgBaseWidget
	{
	public:
		UBlockInputUserWidget(const UObject& object) : UUmgBaseWidget(object) {}
		UBlockInputUserWidget(void* object) : UUmgBaseWidget(object) {}
		UBlockInputUserWidget(uint64_t object) : UBlockInputUserWidget(reinterpret_cast<void*>(object)) {}
	public:
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UTslNewWorldMapWidget :public UBlockInputUserWidget
	{
	public:
		UTslNewWorldMapWidget(const UObject& object) : UBlockInputUserWidget(object) {}
		UTslNewWorldMapWidget(void* object) : UBlockInputUserWidget(object) {}
		UTslNewWorldMapWidget(uint64_t object) : UTslNewWorldMapWidget(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//0F 2F ? 48 8B ? 0F 28 ? 0F 96 C0 88 83 ? ? 00 00 e8 + 0x12,1,5] 进去第一个 movsxd  rsi,dword ptr [rcx+000005E0]
		MAKE_STRUCT_PROPERTY(TWeakObjectPtr<class UMapGridWidget>, MapGrid, "WeakObjectProperty TslGame.TslNewWorldMapWidget.MapGrid");
#else
		MAKE_STRUCT_PROPERTY(TWeakObjectPtr<class UMapGridWidget>, MapGrid, "WeakObjectProperty TslGame.*aed8dfd4fc.*c569acce30", g_PatternData.offset_UTslNewWorldMapWidget_MapGrid);
#endif
	public:
		bool IsVisible();
		float GetZoomFactor();
		FVector2D GetPosition();
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class UMinimapCanvasWidget :public UUmgBaseWidget
	{
	public:
		UMinimapCanvasWidget(const UObject& object) : UUmgBaseWidget(object) {}
		UMinimapCanvasWidget(void* object) : UUmgBaseWidget(object) {}
		UMinimapCanvasWidget(uint64_t object) : UMinimapCanvasWidget(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//E8 ? ? ? ? 8B 5D ? 48 63 ? ? ? 00 00 85 ? 78 ? 3B + 8
		//E8 ? ? ? ? 8B 5D ? 8B ? ? ? 00 00 03 C7 99 + 8
		//99 F7 FB ? ? ? ? 00 00 48 8B ? E8 + 3
		//E8 ? ? ? ? 8B ? 24 ? 48 63 ? ? ? 00 00 85 ? 78 + 9
		//F6 ? 48 63 ? ? ? 00 00 48 1B ? 83 ? 10 + 2
		MAKE_GENERAL_PROPERTY(int32_t, SelectMinimapSizeIndex, "IntProperty TslGame.MinimapCanvasWidget.SelectMinimapSizeIndex");
#else
		MAKE_GENERAL_PROPERTY(int32_t, SelectMinimapSizeIndex, "IntProperty TslGame.*3325fbde8f.*85af610f26", g_PatternData.offset_UMinimapCanvasWidget_SelectMinimapSizeIndex);
		MAKE_GENERAL_PROPERTY(float, MinimapSizeOffset, "FloatProperty TslGame.*3325fbde8f.*d47ea5472e", g_PatternData.offset_UMinimapCanvasWidget_MinimapSizeOffset);
#endif
	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

	class AInventoryFacade : public ASubActor
	{
	public:
		AInventoryFacade(const UObject& object) : ASubActor(object) {}
		AInventoryFacade(void* object) :ASubActor(object) {}
		AInventoryFacade(uint64_t object) :AInventoryFacade(reinterpret_cast<void*>(object)) {}
	public:
#ifndef ObjectNameHash
		//48 89 45 ? 48 8B F9 80 B9 ? ? 00 00 01 0F 84 ? ? ? ? E8 + 0x14] Call GetInventoryFacade_Inventory
		//Rcx -》ATslCharacter
		MAKE_OBJECT_PROPERTY(class AInventory, Inventory, "ObjectProperty TslGame.InventoryFacade.Inventory");
		//74 ? 48 8B ? 48 8B ? ? ? 00 00 ? 8D ? ? ? 00 00 ? 8B ? E8 + 5
		//45 33 ? 48 8D ? 24 ? 48 8B ? ? ? 00 00 E8 ? ? ? ? 90 48 8B ? 24 ? 48 63 ? 24 ? 48 + 8
		MAKE_OBJECT_PROPERTY(class AEquipment, Equipment, "ObjectProperty TslGame.InventoryFacade.Equipment");
#else
		MAKE_OBJECT_PROPERTY(class AInventory, Inventory, "ObjectProperty TslGame.InventoryFacade.Inventory", g_PatternData.offset_AInventoryFacade_Inventory);
		MAKE_OBJECT_PROPERTY(class AEquipment, Equipment, "ObjectProperty TslGame.InventoryFacade.Equipment", g_PatternData.offset_AInventoryFacade_Equipment);
#endif

	public:
		auto operator->() { return this; }
	public:
		static UClass StaticClass();
	};

#pragma warning(default:4307) 
}
