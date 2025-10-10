#pragma once
#include "Core/Math/Vector.hpp"
#include "Core/Math/Math.h"
#define UMETA(...)
#define IS_BIT_SET(var,pos) ((var) & (1<<(pos)))
namespace IronMan::Core::SDK
{
	using namespace Math;

	typedef uint16_t FBoneIndexType;

	struct CClassBaseChain
	{
	public:
		ptr_t ClassBaseChainArray;
		int32_t NumClassBasesInChainMinusOne;
		inline CClassBaseChain()
		{
			ClassBaseChainArray = 0;
			NumClassBasesInChainMinusOne = 0;
		}
		inline CClassBaseChain(ptr_t a, int32_t b)
		{
			ClassBaseChainArray = a;
			NumClassBasesInChainMinusOne = b;
		}
	};

	struct FTextstrdata
	{
		union
		{
			FTextData<0x20> Data20;
			FTextData<0x28> Data28;
		};
	};

	struct ItemInfoArrayData
	{
		std::vector<void*> _itemArray;
		std::vector<int> _itemFreshArray;
		std::vector<void*> _itemTableRowArray;
		std::vector<FName>_ItemsIDArray;
		std::vector<std::string>_ItemsIDNameArray;
	};

	struct ItemInfoData
	{
		void* _itemTableRow;
		FName _ItemID;
		FText _ItemFTextName;
		FText _ItemFTextCategory;
		FTextstrdata _ItemFTextDataName;
		FTextstrdata _ItemFTextDataCategory;
		std::string _ItemIDName;
		std::wstring _ItemWName;
		std::wstring _ItemWCategory;
		std::string _ItemName;
		std::string _ItemCategory;
		ULONGLONG _tickTime;
		inline ItemInfoData()
		{
			_itemTableRow = nullptr;
			_ItemID = FName();
			_ItemFTextName = FText();
			_ItemFTextCategory = FText();
			_ItemFTextDataName = FTextstrdata();
			_ItemFTextDataCategory = FTextstrdata();
			_ItemIDName = "";
			_ItemWName = L"";
			_ItemWCategory = L"";
			_ItemName = "";
			_ItemCategory = "";
			_tickTime = 0;
		}
	};

	struct TBitArray
	{
	public:
		uint32_t AllocatorInstanceData[6];
		int32_t NumBits;
		int32_t MaxBits;
		inline TBitArray() :
			NumBits(0), MaxBits(0)
		{
			AllocatorInstanceData[0] = 0;
			AllocatorInstanceData[1] = 0;
			AllocatorInstanceData[2] = 0;
			AllocatorInstanceData[3] = 0;
			AllocatorInstanceData[4] = 0;
			AllocatorInstanceData[5] = 0;
		}
	};

	struct TSet_Components
	{
		uintptr_t diga;
		int32_t count;
		int32_t pad;
		TBitArray OwnedComponentsBitArray;
		inline TSet_Components() :
			diga(0), count(0), pad(0), OwnedComponentsBitArray(TBitArray()) {}
	};

	struct digaStruct
	{
		void* obj;
		ptr_t unknown;
		inline digaStruct() :
			obj(nullptr), unknown(0) {}
	};

	struct ItemGroupStruct
	{
		void* obj;
		void* TempUItem;
		void* UItem;
		void* ItemTableRow;
		bool  bReadUItem;
		int	  ReGetUItemTime;
		bool  bReadItemTable;
		int	  ReGetTableTime;
		FName TempID;
		FName ItemID;
		std::string ItemIDName;
		ptr_t LogReReadTime;
		FVector RelativeLocation;
		inline ItemGroupStruct() :
			obj(nullptr), TempUItem(nullptr), UItem(nullptr), ItemTableRow(nullptr),
			TempID(FName()), ItemID(FName()), ItemIDName(""), RelativeLocation(FVector()), LogReReadTime(0),
			ReGetUItemTime(0), ReGetTableTime(0), bReadUItem(), bReadItemTable() {}
	};

	struct UBlockInputUser
	{
		void* obj;
		std::string WorldName;
		bool bWorldMap;
		float WorldMapZoomFactor;
		FVector2D WorldMapPosition;
		int32_t WorldLocationX;
		int32_t WorldLocationY;
		inline UBlockInputUser()
			: obj(nullptr), WorldName(""), bWorldMap(false), WorldMapZoomFactor(0.f), WorldMapPosition(FVector2D()), WorldLocationX(0), WorldLocationY(0) {
		}
		inline UBlockInputUser(void* _obj, std::string _Name, std::string _WorldName, bool _bWorldMap, float _WorldMapZoomFactor, FVector2D _WorldMapPosition, int _WorldLocationX, int _WorldLocationY)
			: obj(_obj), WorldName(_WorldName), bWorldMap(_bWorldMap), WorldMapZoomFactor(_WorldMapZoomFactor), WorldMapPosition(_WorldMapPosition), WorldLocationX(_WorldLocationX), WorldLocationY(_WorldLocationY) {
		}
	};

	enum class ESight
	{
		Iron,
		RedDot,
		Holo,
		X2,
		X3,
		X4,
		X6,
		X8,
		X15
	};

	enum class SmartItemID : uint8_t
	{
		None = 0,
		NoWeapon = 1,
		AllArmed = 2,
		Armed = 3,
		Equipped = 4,
		SuitAttach = 5,
		UnSuit = 6,
		Full = 7
	};

	// Enum TslGame.EEquipSlotID
	enum class EEquipSlotID : uint8_t
	{
		Head = 0,
		Hat = 1,
		Eyes = 2,
		Mask = 3,
		Ghillie = 4,
		Torso = 5,
		TorsoArmor = 6,
		Outer = 7,
		Backpack = 8,
		Hands = 9,
		Legs = 10,
		Feet = 11,
		Belt = 12,
		Special01 = 13,
		Special02 = 14,
		WeaponPrimary = 15,
		WeaponSecondary = 16,
		WeaponMelee = 17,
		WeaponThrowable = 18,
		WeaponTacticalGe = 19,
		Bluechip = 20,
		MaxOrNone = 21,
		EEquipSlotID_MAX = 22


	};

	enum { INDEX_NONE = -1 };
	/** The space for the transform */
	enum ERelativeTransformSpace
	{
		/** World space transform. */
		RTS_World,
		/** Actor space transform. */
		RTS_Actor,
		/** Component space transform. */
		RTS_Component,
		/** Parent bone space transform */
		RTS_ParentBoneSpace,
	};

	// Enum UMG.ESlateVisibility
	enum class ESlateVisibility : uint8_t
	{
		Visible = 0,
		Collapsed = 1,
		Hidden = 2,
		HitTestInvisible = 3,
		SelfHitTestInvisible = 4,
		ESlateVisibility_MAX = 5
	};

	// Enum Engine.ETraceTypeQuery
	enum class ETraceTypeQuery : uint8_t
	{
		TraceTypeQuery1 = 0,
		TraceTypeQuery2 = 1,
		TraceTypeQuery3 = 2,
		TraceTypeQuery4 = 3,
		TraceTypeQuery5 = 4,
		TraceTypeQuery6 = 5,
		TraceTypeQuery7 = 6,
		TraceTypeQuery8 = 7,
		TraceTypeQuery9 = 8,
		TraceTypeQuery10 = 9,
		TraceTypeQuery11 = 10,
		TraceTypeQuery12 = 11,
		TraceTypeQuery13 = 12,
		TraceTypeQuery14 = 13,
		TraceTypeQuery15 = 14,
		TraceTypeQuery16 = 15,
		TraceTypeQuery17 = 16,
		TraceTypeQuery18 = 17,
		TraceTypeQuery19 = 18,
		TraceTypeQuery20 = 19,
		TraceTypeQuery21 = 20,
		TraceTypeQuery22 = 21,
		TraceTypeQuery23 = 22,
		TraceTypeQuery24 = 23,
		TraceTypeQuery25 = 24,
		TraceTypeQuery26 = 25,
		TraceTypeQuery27 = 26,
		TraceTypeQuery28 = 27,
		TraceTypeQuery29 = 28,
		TraceTypeQuery30 = 29,
		TraceTypeQuery31 = 30,
		TraceTypeQuery32 = 31,
		TraceTypeQuery_MAX = 32,
		ETraceTypeQuery_MAX = 33
	};

	enum class EDrawDebugTrace : uint8_t
	{
		EDrawDebugTrace__None = 0,
		EDrawDebugTrace__ForOneFrame = 1,
		EDrawDebugTrace__ForDuration = 2,
		EDrawDebugTrace__Persistent = 3,
		EDrawDebugTrace__EDrawDebugTrace_MAX = 4
	};

	enum ECollisionChannel
	{

		ECC_WorldStatic,
		ECC_WorldDynamic,
		ECC_Pawn,
		ECC_Visibility,
		ECC_Camera,
		ECC_PhysicsBody,
		ECC_Vehicle,
		ECC_Destructible,

		/** Reserved for gizmo collision */
		ECC_EngineTraceChannel1,

		ECC_EngineTraceChannel2,
		ECC_EngineTraceChannel3,
		ECC_EngineTraceChannel4,
		ECC_EngineTraceChannel5,
		ECC_EngineTraceChannel6,

		ECC_GameTraceChannel1,//Weapon
		ECC_GameTraceChannel2,//Projectile
		ECC_GameTraceChannel3,//WorldCollision
		ECC_GameTraceChannel4,//Grenade
		ECC_GameTraceChannel5,//InteractiveObject
		ECC_GameTraceChannel6,//MeleeWeapon
		ECC_GameTraceChannel7,//AudioObstruction
		ECC_GameTraceChannel8,//CrosshairImpact
		ECC_GameTraceChannel9,//CharacterMesh
		ECC_GameTraceChannel10,//EffectPropagation
		ECC_GameTraceChannel11,//WallVerification
		ECC_GameTraceChannel12,//OverlapObject
		ECC_GameTraceChannel13,
		ECC_GameTraceChannel14,
		ECC_GameTraceChannel15,
		ECC_GameTraceChannel16,
		ECC_GameTraceChannel17,
		ECC_GameTraceChannel18,

		/** Add new serializeable channels above here (i.e. entries that exist in FCollisionResponseContainer) */
		/** Add only nonserialized/transient flags below */

		// NOTE!!!! THESE ARE BEING DEPRECATED BUT STILL THERE FOR BLUEPRINT. PLEASE DO NOT USE THEM IN CODE

		ECC_OverlapAll_Deprecated,
		ECC_MAX,
	};
	enum class EQueryMobilityType
	{
		Any,
		Static,	//Any shape that is considered static by physx (static mobility)
		Dynamic	//Any shape that is considered dynamic by physx (movable/stationary mobility)
	};

	// Enum Engine.ERichCurveExtrapolation
	enum ERichCurveExtrapolation : uint8_t
	{
		RCCE_Cycle = 0,
		RCCE_CycleWithOffset = 1,
		RCCE_Oscillate = 2,
		RCCE_Linear = 3,
		RCCE_Constant = 4,
		RCCE_None = 5,
		RCCE_MAX = 6
	};

	// Enum Engine.ERichCurveTangentWeightMode
	enum ERichCurveTangentWeightMode : uint8_t
	{
		RCTWM_WeightedNone = 0,
		RCTWM_WeightedArrive = 1,
		RCTWM_WeightedLeave = 2,
		RCTWM_WeightedBoth = 3,
		RCTWM_MAX = 4
	};


	// Enum Engine.ERichCurveTangentMode
	enum ERichCurveTangentMode : uint8_t
	{
		RCTM_Auto = 0,
		RCTM_User = 1,
		RCTM_Break = 2,
		RCTM_None = 3,
		RCTM_MAX = 4
	};


	// Enum Engine.ERichCurveInterpMode
	enum ERichCurveInterpMode : uint8_t
	{
		RCIM_Linear = 0,
		RCIM_Constant = 1,
		RCIM_Cubic = 2,
		RCIM_None = 3,
		RCIM_MAX = 4
	};

	// Enum TslGame.EWeaponAttachmentSlotID
	enum EWeaponAttachmentSlotID : uint8_t
	{
		EWeaponAttachmentSlotID__None = 0,
		EWeaponAttachmentSlotID__Muzzle = 1,
		EWeaponAttachmentSlotID__LowerRail = 2,
		EWeaponAttachmentSlotID__UpperRail = 3,
		EWeaponAttachmentSlotID__Magazine = 4,
		EWeaponAttachmentSlotID__Stock = 5,
		EWeaponAttachmentSlotID__Angled = 6,
		EWeaponAttachmentSlotID__EWeaponAttachmentSlotID_MAX = 7
	};

	enum EPhysicsSceneType
	{
		/** The synchronous scene, which must finish before Unreal simulation code is run. */
		PST_Sync,
		/** The cloth scene, which may run while Unreal simulation code runs. */
		PST_Cloth,
		/** The asynchronous scene, which may run while Unreal simulation code runs. */
		PST_Async,
		PST_MAX,
	};

	// Number of bits used currently from FMaskFilter.
	enum { NumExtraFilterBits = 6 };

	// Bit counts for Word3 of filter data.
	// (ExtraFilter (top NumExtraFilterBits) + MyChannel (next NumCollisionChannelBits) as ECollisionChannel + Flags (remaining NumFilterDataFlagBits)
	// [NumExtraFilterBits] [NumCollisionChannelBits] [NumFilterDataFlagBits] = 32 bits
	enum { NumCollisionChannelBits = 5 };
	enum { NumFilterDataFlagBits = 32 - NumExtraFilterBits - NumCollisionChannelBits };

	enum ECollisionResponse
	{
		ECR_Ignore UMETA(DisplayName = "Ignore"),
		ECR_Overlap UMETA(DisplayName = "Overlap"),
		ECR_Block UMETA(DisplayName = "Block"),
		ECR_MAX,
	};
	/**
	 * Set of flags stored in the PhysX FilterData
	 *
	 * When this flag is saved in CreateShapeFilterData or CreateQueryFilterData, we only use 23 bits
	 * If you plan to use more than 23 bits, you'll also need to change the format of ShapeFilterData,QueryFilterData
	 * Make sure you also change preFilter/SimFilterShader where it's used
	 */
	enum EPhysXFilterDataFlags
	{

		EPDF_SimpleCollision = 0x0001,
		EPDF_ComplexCollision = 0x0002,
		EPDF_CCD = 0x0004,
		EPDF_ContactNotify = 0x0008,
		EPDF_StaticShape = 0x0010,
		EPDF_ModifyContacts = 0x0020,
		EPDF_KinematicKinematicPairs = 0x0040,
	};


	/** enum for empty constructor tag*/
	enum PxEMPTY
	{
		PxEmpty
	};

	/** enum for zero constructor tag for vectors and matrices */
	enum PxZERO
	{
		PxZero
	};

	/** enum for identity constructor flag for quaternions, transforms, and matrices */
	enum PxIDENTITY
	{
		PxIdentity
	};



	enum EObjectTypeQuery
	{
		ObjectTypeQuery1 UMETA(Hidden),
		ObjectTypeQuery2 UMETA(Hidden),
		ObjectTypeQuery3 UMETA(Hidden),
		ObjectTypeQuery4 UMETA(Hidden),
		ObjectTypeQuery5 UMETA(Hidden),
		ObjectTypeQuery6 UMETA(Hidden),
		ObjectTypeQuery7 UMETA(Hidden),
		ObjectTypeQuery8 UMETA(Hidden),
		ObjectTypeQuery9 UMETA(Hidden),
		ObjectTypeQuery10 UMETA(Hidden),
		ObjectTypeQuery11 UMETA(Hidden),
		ObjectTypeQuery12 UMETA(Hidden),
		ObjectTypeQuery13 UMETA(Hidden),
		ObjectTypeQuery14 UMETA(Hidden),
		ObjectTypeQuery15 UMETA(Hidden),
		ObjectTypeQuery16 UMETA(Hidden),
		ObjectTypeQuery17 UMETA(Hidden),
		ObjectTypeQuery18 UMETA(Hidden),
		ObjectTypeQuery19 UMETA(Hidden),
		ObjectTypeQuery20 UMETA(Hidden),
		ObjectTypeQuery21 UMETA(Hidden),
		ObjectTypeQuery22 UMETA(Hidden),
		ObjectTypeQuery23 UMETA(Hidden),
		ObjectTypeQuery24 UMETA(Hidden),
		ObjectTypeQuery25 UMETA(Hidden),
		ObjectTypeQuery26 UMETA(Hidden),
		ObjectTypeQuery27 UMETA(Hidden),
		ObjectTypeQuery28 UMETA(Hidden),
		ObjectTypeQuery29 UMETA(Hidden),
		ObjectTypeQuery30 UMETA(Hidden),
		ObjectTypeQuery31 UMETA(Hidden),
		ObjectTypeQuery32 UMETA(Hidden),

		ObjectTypeQuery_MAX	UMETA(Hidden)
	};

	enum class BodyInstanceSceneState : uint8_t
	{
		NotAdded,
		AwaitingAdd,
		Added,
		AwaitingRemove,
		Removed
	};
	enum class ESleepFamily : uint8_t
	{
		/** Engine defaults. */
		Normal,
		/** A family of values with a lower sleep threshold; good for slower pendulum-like physics. */
		Sensitive,
		/** Specify your own sleep threshold multiplier */
		Custom,
	};



	enum EWalkableSlopeBehavior
	{
		/** Don't affect the walkable slope. Walkable slope angle will be ignored. */
		WalkableSlope_Default		UMETA(DisplayName = "Unchanged"),

		/**
		 * Increase walkable slope.
		 * Makes it easier to walk up a surface, by allowing traversal over higher-than-usual angles.
		 * @see FWalkableSlopeOverride::WalkableSlopeAngle
		 */
		WalkableSlope_Increase		UMETA(DisplayName = "Increase Walkable Slope"),

		/**
		 * Decrease walkable slope.
		 * Makes it harder to walk up a surface, by restricting traversal to lower-than-usual angles.
		 * @see FWalkableSlopeOverride::WalkableSlopeAngle
		 */
		WalkableSlope_Decrease		UMETA(DisplayName = "Decrease Walkable Slope"),

		/**
		 * Make surface unwalkable.
		 * Note: WalkableSlopeAngle will be ignored.
		 */
		WalkableSlope_Unwalkable	UMETA(DisplayName = "Unwalkable"),

		WalkableSlope_Max		UMETA(Hidden),
	};


	// Enum Engine.EDOFMode
	enum class EDOFMode : uint8_t
	{
		EDOFMode__Default = 0,
		EDOFMode__SixDOF = 1,
		EDOFMode__YZPlane = 2,
		EDOFMode__XZPlane = 3,
		EDOFMode__XYPlane = 4,
		EDOFMode__CustomPlane = 5,
		EDOFMode__None = 6,
		EDOFMode__EDOFMode_MAX = 7
	};


	// Enum Engine.ECollisionEnabled
	enum class ECollisionEnabled : uint8_t
	{
		ECollisionEnabled__NoCollision = 0,
		ECollisionEnabled__QueryOnly = 1,
		ECollisionEnabled__PhysicsOnly = 2,
		ECollisionEnabled__QueryAndPhysics = 3,
		ECollisionEnabled__ECollisionEnabled_MAX = 4
	};


	struct TStatId {};
	struct FCollisionQueryParams
	{
		/** Tag used to provide extra information or filtering for debugging of the trace (e.g. Collision Analyzer) */
		FName TraceTag;

		/** Tag used to indicate an owner for this trace */
		FName OwnerTag;

		/** Whether we should perform the trace in the asynchronous scene.  Default is false. */
		bool bTraceAsyncScene;

		/** Whether we should trace against complex collision */
		bool bTraceComplex;

		/** Whether we want to find out initial overlap or not. If true, it will return if this was initial overlap. */
		bool bFindInitialOverlaps;

		/** Whether we want to return the triangle face index for complex static mesh traces */
		bool bReturnFaceIndex;

		/** Only fill in the PhysMaterial field of  */
		bool bReturnPhysicalMaterial;

		/** Whether to ignore blocking results. */
		bool bIgnoreBlocks;

		/** Whether to ignore touch/overlap results. */
		bool bIgnoreTouches;

		/** Filters query by mobility types (static vs stationary/movable)*/
		EQueryMobilityType MobilityType;

		/** TArray typedef of components to ignore. */
		typedef TArray<uint32_t> IgnoreComponentsArrayType;

		/** TArray typedef of actors to ignore. */
		typedef TArray<uint32_t> IgnoreActorsArrayType;

		/** Extra filtering done on the query. See declaration for filtering logic */
		uint8_t IgnoreMask;

		/** StatId used for profiling individual expensive scene queries */
		TStatId StatId;
	private:

		/** Tracks whether the IgnoreComponents list is verified unique. */
		mutable bool bComponentListUnique;

		/** Set of components to ignore during the trace */
		mutable IgnoreComponentsArrayType IgnoreComponents;

		/** Set of actors to ignore during the trace */
		IgnoreActorsArrayType IgnoreActors;
		char pad_[0xE0];
	};
	//constexpr auto size = sizeof(FCollisionQueryParams);

	/** Structure that defines response container for the query. Advanced option. */
	struct FCollisionResponseParams
	{
	};


	// Enum TslGame.EProjectileExplosionStartType
	enum class EProjectileExplosionStartType : uint8_t
	{
		EProjectileExplosionStartType__NotStart = 0,
		EProjectileExplosionStartType__Impact = 1,
		EProjectileExplosionStartType__Delay = 2,
		EProjectileExplosionStartType__ImpactOrDelay = 3,
		EProjectileExplosionStartType__EProjectileExplosionStartType_MAX = 4
	};

	template<typename ElementType, DecryptFunc Func, hash_t hash = 0>
	struct UEncryptedObjectProperty
	{
		typedef void* (__fastcall* DECRYPT_FUNC)(uint64_t);
	public:
		explicit UEncryptedObjectProperty(void* object, ptr_t offset)
			:object(object),
			Offset(offset),
			data(INVALID_POINTER),
			endata(0)
		{
		}
		~UEncryptedObjectProperty()
		{

		}
	public:
		void* GetPtr() const
		{
			DECRYPT_FUNC func = reinterpret_cast<DECRYPT_FUNC>(g_DecryptFuncs[Func]->GetFunction());

			if (func && object && Offset != -1)
			{
				auto encryptedData(GetDMA().Read<uint64_t>((ptr_t)object + Offset));
				if (data == INVALID_POINTER || endata != encryptedData)
				{
					if (encryptedData)
					{
						const_cast<void*>(data) = func(encryptedData);
						static_cast<uint64_t>(endata) = encryptedData;
					}

				}
			}
			if (data == INVALID_POINTER)
				return nullptr;
			return data;
		}

		ElementType Get() const
		{
			return static_cast<ElementType>(GetPtr());
		}
		ElementType operator()(void) const { return Get(); }
		ElementType operator->() const
		{
			return Get();
		}

		bool operator==(void* p) const noexcept { return GetPtr() == p; }
		bool operator!=(void* p) const noexcept { return GetPtr() != p; }
		explicit operator bool() const noexcept { return GetPtr() != nullptr; }
		auto operator[](size_t idx) const { return Get()[idx]; }

	private:
		void* object;
		int64_t Offset;
		void* data;
		uint64_t endata;
	};

	template<typename ElementType, hash_t hash = 0>
	struct StructProperty
	{
		template<typename T>
		using cleanup_t = std::remove_cv_t<std::remove_pointer_t<T>>;

		template<typename T, typename S>
		static constexpr bool is_string_ptr = (std::is_pointer_v<T> && std::is_same_v<cleanup_t<T>, S>);

		template<typename T>
		static constexpr bool is_number = (std::is_integral_v<T> || std::is_enum_v<T>);

		template<typename T>
		static constexpr bool is_void_ptr = (std::is_pointer_v<T> && std::is_void_v<cleanup_t<T>>);

		using RAW_T = std::decay_t<ElementType>;

	public:
		static_assert(hash != 0, "hash is empty!");
		explicit StructProperty()
		{
		}
	public:
		ElementType Get(UObject* object, ptr_t offset) const
		{
			if constexpr (std::is_pointer_v<RAW_T> && !is_void_ptr<RAW_T>)
			{
				return object->IsValid() && offset >= 0
					? reinterpret_cast<ElementType>((ptr_t)object->GetPtr() + offset)
					: (ElementType)(nullptr);
				//set(dataPtr, sizeof(cleanup_t<RAW_T>), reinterpret_cast<uint64_t>(arg));
			}
			else
			{
				return object->IsValid() && offset >= 0
					? GetDMA().Read<ElementType>((ptr_t)object->GetPtr() + offset)
					: ElementType();
			}
		}

		ptr_t ScatterGet(UObject* object, ptr_t offset) const
		{
			if (object->IsValid() && offset >= 0)
				return (ptr_t)object->GetPtr() + offset;
			return 0;
		}

		SIZE_T SGetSize(UObject* object)
		{
			return sizeof(ElementType);
		}
	};

	template<typename ElementType, hash_t hash = 0>
	struct ObjectProperty
	{
	public:
		static_assert(hash != 0, "hash is empty!");
		explicit ObjectProperty()
		{
		}
	public:
		ElementType Get(UObject* object, ptr_t offset) const
		{
			return object && object->IsValid() && offset >= 0
				? (ElementType)(GetDMA().Read<void*>((ptr_t)object->GetPtr() + offset))
				: (ElementType)(nullptr);
		}

		ptr_t ScatterGet(UObject* object, ptr_t offset) const
		{
			if (object->IsValid() && offset >= 0)
				return (ptr_t)object->GetPtr() + offset;
			return 0;
		}

		SIZE_T SGetSize(UObject* object)
		{
			return sizeof(ElementType);
		}
	};

	template<typename ElementType, hash_t hash = 0>
	struct GeneralProperty
	{
	public:
		static_assert(hash != 0, "hash is empty!");
		explicit GeneralProperty()
		{
		}
	public:
		ElementType Get(UObject* object, ptr_t offset) const
		{
			return object->IsValid() && offset >= 0
				? GetDMA().Read<ElementType>((ptr_t)object->GetPtr() + offset)
				: (ElementType)0;
		}

		ptr_t ScatterGet(UObject* object, ptr_t offset) const
		{
			if (object->IsValid() && offset >= 0)
				return (ptr_t)object->GetPtr() + offset;
			return 0;
		}

		SIZE_T SGetSize(UObject* object)
		{
			return sizeof(ElementType);
		}

		ElementType* GetPtr(UObject* object, ptr_t offset) const
		{
			return object->IsValid() && offset >= 0
				? reinterpret_cast<ElementType*>((ptr_t)object->GetPtr() + offset)
				: (ElementType*)0;
		}

		void Set(UObject* object, ElementType value, ptr_t offset) const
		{
			if (object->IsValid() && offset >= 0)
				GetDMA().write<ElementType>((ptr_t)object->GetPtr() + offset, value);
		}
	};

	//template<typename Key, typename Value>
	struct TMap
	{
		char UnknownData[0x50];
	};

	template<class TEnum_TMap>
	class TEnumAsByte_TMap
	{
	public:
		typedef TEnum_TMap EnumType;
		TEnumAsByte_TMap() = default;
		TEnumAsByte_TMap(const TEnumAsByte_TMap&) = default;
		TEnumAsByte_TMap& operator=(const TEnumAsByte_TMap&) = default;
		FORCEINLINE TEnumAsByte_TMap(TEnum_TMap InValue) : Value(static_cast<uint8_t>(InValue)) {}
		explicit FORCEINLINE TEnumAsByte_TMap(int32_t InValue) : Value(static_cast<uint8_t>(InValue)) {}
		explicit FORCEINLINE TEnumAsByte_TMap(uint8_t InValue) : Value(InValue) {}
		bool operator==(TEnum_TMap InValue) const { return static_cast<TEnum_TMap>(Value) == InValue; }
		bool operator==(TEnumAsByte_TMap InValue) const { return Value == InValue.Value; }
		operator TEnum_TMap() const { return (TEnum_TMap)Value; }
		TEnum_TMap GetValue() const { return (TEnum_TMap)Value; }
	private:
		uint8_t Value;
	};

	template<class T>
	struct TSetElement {
		T Value;//0
		int HashNextId;//9
		int HashIndex;//A
	};

	//template<class T>
	//class TSet : public TArray<TSetElement<T>> {};

	template<class KeyType, class ValueType>
	struct TPair {
		KeyType Key;
		ValueType Value;
	};

	template<class KeyType, class ValueType>
	class TMap2 : public TArray<TSetElement<TPair<KeyType, ValueType>>> {
	public:
		bool GetValue(const KeyType& Key, ValueType& Value) const {
			for (const auto& Elem : this->GetVector()) {
				if (Elem.Value.Key == Key) {
					Value = Elem.Value.Value;
					return true;
				}
			}
			return false;
		}
	};

	constexpr auto sssa = sizeof(TBitArray);

	struct FSkinParam // Size: 0x10
	{
	public:
		TEnumAsByte<enum ESkinCategory> SkinCategory; /* Ofs: 0x0 Size: 0x1 EnumProperty TslGame.SkinParam.SkinCategory */
		uint8_t UnknownData1[0x7];
		FName SkinName; /* Ofs: 0x8 Size: 0x8 NameProperty TslGame.SkinParam.SkinName */
	};


	struct FReplicatedSkinParam // Size: 0x20
	{
	public:
		FName SkinItemID; /* Ofs: 0x0 Size: 0x8 NameProperty TslGame.ReplicatedSkinParam.SkinItemID */
		FSkinParam SkinParam; /* Ofs: 0x8 Size: 0x10 StructProperty TslGame.ReplicatedSkinParam.SkinParam */
		int32_t SkinOwnerPlayerId; /* Ofs: 0x18 Size: 0x4 IntProperty TslGame.ReplicatedSkinParam.SkinOwnerPlayerId */
	};

	struct FReplicatedSkinItem // Size: 0xC
	{
	public:
		FName SkinItemID; /* Ofs: 0x0 Size: 0x8 NameProperty TslGame.ReplicatedSkinParam.SkinItemID */
		int32_t SkinOwnerPlayerId; /* Ofs: 0x8 Size: 0x4 IntProperty TslGame.ReplicatedSkinParam.SkinOwnerPlayerId */
	};

	struct FEuqimentReplicatedSkinItem // Size: 24
	{
	public:
		ptr_t Addr;
		std::string EquimentName;
		FReplicatedSkinItem Skin;
	};

	struct FTslPlayerStatistics
	{
		int                                                NumKills;                                                 // 0x0000(0x0004) (ZeroConstructor, IsPlainOldData)
		int                                                NumPlayerKills;                                           // 0x0004(0x0004) (ZeroConstructor, IsPlainOldData)
		int												   NumAssists;												 // 0x0008(0x0004) (ZeroConstructor, IsPlainOldData)
	};

	// ScriptStruct TslGame.ItemTableRowBase
	//struct ItemTableRowBase
	//{
	//	unsigned char						UnknownData01[8];				//0x0
	//	bool								bStackable;						//0x8
	//	bool								bUsable;						//0x8
	//	bool								bIsConsumable;					//0x8
	//	bool								bDisableSpawnInGame;			//0x8
	//	int									StackCount;						//0xC
	//	int									DefaultStackCount;				//0x10
	//	unsigned char						UnknownData02[4];				//0x14
	//	FText								ItemName;						//0x18
	//	FText								ItemCategory;					//0x30
	//	FText								ItemDetailedName;				//0x48
	//	FText								ItemDescription;				//0x60
	//	float								ItemMaxInteractableDistance;	//0x78
	//	int									StackCountMax;					//0x7C		
	//	float								SpacePerItem;					//0x80
	//	unsigned char						UnknownData03[4];				//0x84
	//	unsigned char						DroppedMesh[8];					//0x88
	//	unsigned char						UnknownData04[0x14];			//0xA4
	//	float								DroppedMeshScale;				//0xA8
	//	bool								bDropAsProjectile;				//0xAC
	//	unsigned char						UnknownData05[0x3];				//0xAF
	//	unsigned char						ProjectileConfig[0x20];			//0xB0
	//	unsigned char						Icon[0x20];						//0xD0
	//	unsigned char						ToolTipIcon[0x20];				//0xF0
	//	float								IconSizeRatio;					//0x110
	//	float								ToolTipIconSizeRatio;			//0x114
	//	unsigned char						DropSoundAk[0x20];				//0x118
	//	ptr_t								DropAnimation;					//0x138
	//	unsigned char						PickUpSoundAk[0x20];			//0x140
	//	unsigned char						PickUpAnimationOverride[8];		//0x160
	//	unsigned char						InventoryPutSoundAk[0x20];		//0x168
	//	int									SortPriority;					//0x188
	//	unsigned char						UnderlyingType[4];				//0x18C
	//	int									SubCategory;					//0x190
	//	unsigned char						UnknownData06[4];				//0x194
	//	bool								bIsToy;							//0x198
	//	bool								bIsLobbyItem;					//0x199
	//	unsigned char						UnknownData07[3];				//0x19B
	//	int									LobbyItemID;					//0x19C
	//	unsigned char						LobbyItemDesc[0x60];			//0x1A0
	//	bool								bIsLobbyWeapon;					//0x200
	//	int									LobbyWeaponID;					//0x204
	//	unsigned char						LobbyWeaponInstance[0x20];		//0x208
	//	bool								bVested;						//0x228
	//	bool								bDropVolatility;				//0x229
	//	bool								bCanDropItem;					//0x22A
	//	unsigned char						UnknownData08[4];				//0x22F
	//	FString								Note;							//0x230
	//	int									AIFullValue;					//0x240
	//	int									ItemSubType;					//0x244
	//	ptr_t								ItemID;							//0x248
	//	unsigned char						ItemClass[8];					//0x250
	//	unsigned char						CastableImplementClass[8];		//0x258
	//	unsigned char						DestructionBehavior[8];			//0x260
	//};

	// ScriptStruct TslGame.ProjectileData
	// 0x00E8
	struct FProjectileData
	{
		float                                              ProjectileLife;                                           // 0x0000(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              FakeProjectileLife;                                       // 0x0004(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		int                                                ExplosionDamage;                                          // 0x0008(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              ExplosionRadius;                                          // 0x000C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              IgniteFuelInRadius;                                       // 0x0010(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		unsigned char                                      UnknownData00[0x4];                                       // 0x0014(0x0004) MISSED OFFSET
		void* DamageCurve;                                              // 0x0018(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		void* DistanceCurveCurveVertical;                               // 0x0020(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              MinImpactVelocityForSound;                                // 0x0028(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              TimeBetweenImpactSound;                                   // 0x002C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		void* DamageType;                                               // 0x0030(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		int                                                VerticalSurfaceImpactLimit;                               // 0x0038(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              AbsPitchLimitForVerticalSurface;                          // 0x003C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              WaterHit_VelocityScalar;                                  // 0x0040(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              WaterHit_GravityScalar;                                   // 0x0044(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		unsigned char                                      DestroyAtExplosion : 1;                                   // 0x0048(0x0001) (Edit, DisableEditOnInstance)
		unsigned char                                      bIgnoreVerticalSurfaces : 1;                              // 0x0048(0x0001) (Edit, DisableEditOnInstance)
		unsigned char                                      bExplodeOnDamage : 1;                                     // 0x0048(0x0001) (Edit, DisableEditOnInstance)
		unsigned char                                      UnknownData01[0x7];                                       // 0x0049(0x0007) MISSED OFFSET
		TArray<uint8_t>									   ExplodeFromDamageTypes;                                   // 0x0050(0x0010) (Edit, ZeroConstructor, DisableEditOnInstance)
		TArray<uint8_t>									   DestroyFromDamageTypes;                                   // 0x0060(0x0010) (Edit, ZeroConstructor, DisableEditOnInstance)
		bool                                               bDamageCheckThroughWalls;                                 // 0x0070(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		bool                                               bAttachToHitActor;                                        // 0x0071(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		unsigned char                                      UnknownData02[0x6];                                       // 0x0072(0x0006) MISSED OFFSET
		TArray<TEnumAsByte<uint8_t>>					   AttachToPhysicalMaterials;                                // 0x0078(0x0010) (Edit, ZeroConstructor, DisableEditOnInstance)
		TArray<void*>									   IgnoreAttachActorClasses;                                 // 0x0088(0x0010) (Edit, ZeroConstructor, DisableEditOnInstance)
		bool                                               ApplyHorizontalVerticalDamageSeperately;                  // 0x0098(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		bool                                               bShouldCheckMovementBase;                                 // 0x0099(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		unsigned char                                      UnknownData03[0x2];                                       // 0x009A(0x0002) MISSED OFFSET
		float                                              AttachedRepDistance;                                      // 0x009C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              AttachedRepDistanceConsole;                               // 0x00A0(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		unsigned char                                      UnknownData04[0x4];                                       // 0x00A4(0x0004) MISSED OFFSET
		void* DamageCauserIcon;                                         // 0x00A8(0x0008) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              DamageIconRatio;                                          // 0x00B0(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		unsigned char                                      UnknownData05[0x4];                                       // 0x00B4(0x0004) MISSED OFFSET
		struct FText                                       DamageCauserItemName;                                     // 0x00B8(0x0018) (Edit, BlueprintVisible, BlueprintReadOnly, DisableEditOnInstance)
		uint8_t											   DamageCauserClass;                                        // 0x00D0(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		bool                                               bExplodeOnBreachable;                                     // 0x00D1(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		bool                                               bCanDestroyBuildings;                                     // 0x00D2(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		bool                                               bDamageOnImpact;                                          // 0x00D3(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              ImpactDamage;                                             // 0x00D4(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		void* ImpactDamageCurve;                                        // 0x00D8(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		void* ImpactDamageType;                                         // 0x00E0(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	};

	// ScriptStruct TslGame.ThrowableAnim
	// 0x0018
	struct FThrowableAnim
	{
		class UAnimMontage* PinOff;                                                   // 0x0000(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		class UAnimMontage* Cooking;                                                  // 0x0008(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		class UAnimMontage* Release;                                                  // 0x0010(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	};

	// ScriptStruct TslGame.ThrowableWeaponData
	// 0x00B0
	struct FThrowableWeaponData
	{
		class UClass* ProjectileClass;											 // 0x0000(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		bool                                               bShowTrajectory;                                          // 0x0008(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		unsigned char                                      UnknownData00[0x7];                                       // 0x0009(0x0007) MISSED OFFSET
		struct FName                                       CookingSocketName;                                        // 0x0010(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		class UClass* CookingEffectClass;										 // 0x0018(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		class UClass* Ring;													 // 0x0020(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		struct FName                                       RingAttachSocket;                                         // 0x0028(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		EProjectileExplosionStartType                      ExplosionStartType;                                       // 0x0030(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		unsigned char                                      UnknownData01[0x3];                                       // 0x0031(0x0003) MISSED OFFSET
		float                                              ExplosionDelay;                                           // 0x0034(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		bool                                               bUseCustomCookingExplosionDelay;                          // 0x0038(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		unsigned char                                      UnknownData02[0x3];                                       // 0x0039(0x0003) MISSED OFFSET
		float                                              CookingExplosionDelay;                                    // 0x003C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              FireDelay;                                                // 0x0040(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              ActivationTimeLimit;                                      // 0x0044(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		struct FThrowableAnim                              StandAnim;                                                // 0x0048(0x0018) (Edit, DisableEditOnInstance, IsPlainOldData)
		struct FThrowableAnim                              CrouchAnim;                                               // 0x0060(0x0018) (Edit, DisableEditOnInstance, IsPlainOldData)
		struct FThrowableAnim                              ProneAnim;                                                // 0x0078(0x0018) (Edit, DisableEditOnInstance, IsPlainOldData)
		class UCurveFloat* SpeedCurve;												 // 0x0090(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		class UCurveFloat* PitchCurve;												 // 0x0098(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              ThrowTime;                                                // 0x00A0(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              ThrowCooldownDuration;                                    // 0x00A4(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		bool                                               bCanBeCooked;                                             // 0x00A8(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		bool                                               bShowCookingTimer;                                        // 0x00A9(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		unsigned char                                      UnknownData03[0x6];                                       // 0x00A9(0x0006) MISSED OFFSET
	};

	struct FWeaponTrajectoryConfig_Probability_Damage
	{
		float Damage;
		float Probability;
	};

	// ScriptStruct TslGame.WeaponTrajectoryConfig
	// 0x0040 to 0x0048
	struct FWeaponTrajectoryConfig
	{
		//初始速度
		float	InitialSpeed;//0x00
		float	_d8bdb8ed57;//0x04
		//命中伤害
		float	HitDamage;//0x08
		int		unknown_data;//0x0C
		TArray<void*>_8e4702d555;//0x10
		//车辆损坏标量
		float	VehicleDamageScalar;//0x20
		//降低夹具损坏
		float	LowerClampDamage;//0x24
		//弹道曲线
		//0F 28 F0 F3 41 0F 5C F7 - 5 call进入
		//0F 29 ? 24 ? 0F 28 ? 48 8B ? ? ? 00 00 48 85 ? 0F 84 + 8
		//0F 29 ? 24 ? ? ? ? 0F 28 ? 48 8B ? ? ? 00 00 48 85 ? 0F 84 + 0xB
		class UCurveVector* BallisticCurve;//0x28
		//范围修改器
		float	RangeModifier;//0x30
		//参考距离
		float	ReferenceDistance;//0x34
		//最大行程距离
		float	TravelDistanceMax;//0x38
		//使用最大伤害距离
		bool	bUseMaxDamageDistance;//0x3C
		//模拟子步骤时间
		float	SimulationSubstepTime;//0x40
		//阻力系数
		float	VDragCoefficient;//0x44
		float	BDS;//0x48
		//使用高级弹道
		bool	bUseAdvancedBallistics;//0x4C
		bool	bCanProduceCrackSound;//0x4D
		//是难以穿透的
		bool	IsPenetrable;//0x4E
		class UClass* DamageType;//0x50
		//是否启用了轨迹复制
		bool bIsTrajectoryReplicationEnabled;//0x58
		//武器传播z
		float WeaponSpread;//0x5C
		void* _b8acfa1f9d;//0x60
	};

	// ScriptStruct TslGame.RecoilDebugInfo
	// 0x0028
	struct FRecoilDebugInfo
	{
		float                                              RecoilValue;                                              // 0x0000(0x0004) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
		float                                              RecoilVertical;                                           // 0x0004(0x0004) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
		float                                              RecoilHorizontal;                                         // 0x0008(0x0004) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
		float                                              RecoilTargetPitch;                                        // 0x000C(0x0004) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
		float                                              RecoilTargetYaw;                                          // 0x0010(0x0004) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
		float                                              RecoilRecoveryTarget;                                     // 0x0014(0x0004) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
		float                                              RecoilVerticalDelta;                                      // 0x0018(0x0004) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
		float                                              RecoilHorizontalDelta;                                    // 0x001C(0x0004) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
		float                                              RecoilCharacterPitch;                                     // 0x0020(0x0004) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
		float                                              RecoilWeaponPitch;                                        // 0x0024(0x0004) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
	};

	// ScriptStruct TslGame.WeaponRecoilConfig
	// 0x0080
	struct FWeaponRecoilConfig
	{
		float                                              VerticalRecoilMin;                                        // 0x0000(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              VerticalRecoilMax;                                        // 0x0004(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              VerticalRecoilRecoveryMin;                                // 0x0008(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              VerticalRecoilVariation;                                  // 0x000C(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              VerticalRecoveryModifier;                                 // 0x0010(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              VerticalRecoveryClamp;                                    // 0x0014(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              VerticalRecoveryMax;                                      // 0x0018(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              LeftMax;                                                  // 0x001C(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              RightMax;                                                 // 0x0020(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              HorizontalTendency;                                       // 0x0024(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		class UCurveVector* RecoilCurve;																			 // 0x0028(0x0008) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		int                                                BulletsPerSwitch;                                         // 0x0030(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              TimePerSwitch;                                            // 0x0034(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		bool                                               bSwitchOnTime;                                            // 0x0038(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		unsigned char                                      UnknownData00[0x3];                                       // 0x0039(0x0003) MISSED OFFSET
		float                                              RecoilSpeed_Vertical;                                     // 0x003C(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              RecoilSpeed_Horizontal;                                   // 0x0040(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              RecoverySpeed_Vertical;                                   // 0x0044(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              RecoilValue_Climb;                                        // 0x0048(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              RecoilValue_Fall;                                         // 0x004C(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              RecoilModifier_Stand;                                     // 0x0050(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              RecoilModifier_Crouch;                                    // 0x0054(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              RecoilModifier_Prone;                                     // 0x0058(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		float                                              RecoilHorizontalMinScalar;                                // 0x005C(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
		struct FVector2D                                   RecoilStartTimeInMap;                                     // 0x0060(0x0008) (Edit, BlueprintVisible, BlueprintReadOnly, DisableEditOnInstance, IsPlainOldData)
		struct FVector2D                                   RecoilStartTimeOutMap;                                    // 0x0068(0x0008) (Edit, BlueprintVisible, BlueprintReadOnly, DisableEditOnInstance, IsPlainOldData)
		struct FVector2D                                   RecoilBlendInInMap;                                       // 0x0070(0x0008) (Edit, BlueprintVisible, BlueprintReadOnly, DisableEditOnInstance, IsPlainOldData)
		struct FVector2D                                   RecoilBlendInOutMap;                                      // 0x0078(0x0008) (Edit, BlueprintVisible, BlueprintReadOnly, DisableEditOnInstance, IsPlainOldData)
	};

	// ScriptStruct Engine.Vector_NetQuantize
	// 0x0000 (0x000C - 0x000C)
	struct FVector_NetQuantize : public FVector
	{

	};

	// ScriptStruct Engine.Vector_NetQuantizeNormal
	// 0x0000 (0x000C - 0x000C)
	struct FVector_NetQuantizeNormal : public FVector
	{

	};

	// ScriptStruct Engine.HitResult
	// 0x0088
	struct FHitResult
	{
	public:
		//指示此命中结果是否是阻止s冲突的。 如果为false，则没有命中或者是重叠/触摸。
		unsigned char                                      bBlockingHit : 1;                                         // 0x0000(0x0001)
		// 迹线是否以穿透开始，即具有初始阻挡重叠。 
		// 在穿透的情况下，如果PenetrationDepth> 0.f，则它将表示沿法线向量的距离，这将导致扫掠形状与被击中的对象之间的最小接触。 
		// 在这种情况下，ImpactNormal将是与该位置的移动相反的正常（即，Normal可能不等于ImpactNormal）。 ImpactPoint与Location相同，因为没有单一的影响点可以报告。s
		unsigned char                                      bStartPenetrating : 1;                                    // 0x0000(0x0001)
		unsigned char                                      UnknownData00[0x3];                                       // 0x0001(0x0003) MISSED OFFSET
		// 如果存在命中，则沿着跟踪方向的影响“时间”（范围从0.0到1.0），表示TraceStart和TraceEnd之间的时间。
		// 对于扫掠运动（但不是查询），可以从实际撞击时间稍微拉回，以防止相邻几何体的精度问题。
		float                                              Time;                                                     // 0x0004(0x0004) (ZeroConstructor, IsPlainOldData)
		//从TraceStart到世界空间中的位置的距离。 如果存在初始重叠（在另一个碰撞对象内部开始跟踪），则该值为0。
		float                                              Distance;                                                 // 0x0008(0x0004) (ZeroConstructor, IsPlainOldData)
		//世界空间中的位置，如果有撞击，移动的形状将最终抵抗受影响的物体。 等于线路测试的影响点。
		//示例：对于球体轨迹测试，这是当球体接触另一个对象时球体中心所在的点。
		//对于扫掠运动（但不是查询），这可能不等于形状的最终位置，因为轻微拉回撞击以防止精确问题与另一表面重叠。
		struct FVector_NetQuantize                         Location;                                                 // 0x000C(0x000C)
		//在跟踪形状（框，球体，射线等）与受影响对象的实际接触的世界空间中的位置。
		//示例：对于球体轨迹测试，这是球体表面接触另一个对象的点。
		//@note：在初始重叠（bStartPenetrating = true）的情况下，ImpactPoint将与Location相同，因为没有有意义的单个影响点可以报告。
		struct FVector_NetQuantize                         ImpactPoint;                                              // 0x0018(0x000C)
		/**
		* 正常情况下，在世界空间中，对于被扫描的物体。 等于线路测试的ImpactNormal。
		* 这是针对胶囊和球体计算的，否则它将与ImpactNormal相同。
		* 示例：对于球体轨迹测试，这是一个标准化矢量，指向撞击点处球体的中心。
		*/
		struct FVector_NetQuantizeNormal                   Normal;                                                   // 0x0024(0x000C)
		/**
		 *正常的世界空间中的命中，对于被扫描击中的对象，如果有的话。
		 *例如，如果一个盒子击中一个平面，这是一个从平面指向的标准化向量。
		 *在与表面的角或边缘碰撞的情况下，通常选择“最相反”的法线（与查询方向相反）。
		*/
		struct FVector_NetQuantizeNormal                   ImpactNormal;                                             // 0x0030(0x000C)
		/**
		*开始跟踪的位置。
		*例如，如果一个球体横扫世界，这就是球体中心的起始位置。
		*/
		struct FVector                         TraceStart;                                               // 0x003C(0x000C)
		/**
		*跟踪的结束位置; 这不是影响发生的地方（如果有的话），而是尝试扫描的最远点。
		*例如，如果一个球体横扫世界，如果没有阻挡命中，这将是球体的中心。
		**/
		struct FVector                         TraceEnd;                                                 // 0x0048(0x000C)
		/**
		*如果此测试以渗透开始（bStartPenetrating为true）并且可以计算出穿透矢量，
		*此值是沿正常的距离，将导致移出渗透。
		*如果无法计算距离，则此距离将为零。
		*/
		float                                              PenetrationDepth;                                         // 0x0054(0x0004) (ZeroConstructor, IsPlainOldData)
		// **关于被击中的项目的额外数据（特定于原始特征）。*/
		int                                                Item;                                                     // 0x0058(0x0004) (ZeroConstructor, IsPlainOldData)
		/**
		*被击中的物理材料。
		* @note必须在扫描的PrimitiveComponent或查询参数中设置bReturnPhysicalMaterial才能返回此值。
		*/
		TWeakObjectPtr<class UObject>					   PhysMaterial;                                             // 0x005C(0x0008) (ZeroConstructor, IsPlainOldData)
		// **演员被追踪所击中。*/
		TWeakObjectPtr<class AActor>                       Actor;                                                    // 0x0064(0x0008) (ZeroConstructor, IsPlainOldData)
		/** PrimitiveComponent被跟踪命中。*/
		TWeakObjectPtr<class UPrimitiveComponent>          Component;                                                // 0x006C(0x0008) (ExportObject, ZeroConstructor, InstancedReference, IsPlainOldData)
		unsigned char                                      UnknownData01[0x4];                                       // 0x0074(0x0004) MISSED OFFSET
		/** 我们命中的骨骼名称（用于骨架网格物体）。*/
		struct FName                                       BoneName;                                                 // 0x0078(0x0008) (ZeroConstructor, IsPlainOldData)
		/** 我们击中的面部指数（对于具有三角形网格的复杂命中）。*/
		int                                                FaceIndex;                                                // 0x0080(0x0004) (ZeroConstructor, IsPlainOldData)
		unsigned char                                      UnknownData02[0x4];                                       // 0x0084(0x0004) MISSED OFFSET
	public:
		FORCEINLINE bool IsValidBlockingHit() const
		{
			return bBlockingHit && !bStartPenetrating;
		}

	};

	struct FCollisionResponseContainer
	{
		union
		{
			struct
			{
				//Reserved Engine Trace Channels
				uint8_t WorldStatic;			// 0
				uint8_t WorldDynamic;			// 1
				uint8_t Pawn;					// 2
				uint8_t Visibility;			// 3
				uint8_t Camera;				// 4
				uint8_t PhysicsBody;			// 5
				uint8_t Vehicle;				// 6
				uint8_t Destructible;			// 7

				// Unspecified Engine Trace Channels
				uint8_t EngineTraceChannel1;   // 8
				uint8_t EngineTraceChannel2;   // 9
				uint8_t EngineTraceChannel3;   // 10
				uint8_t EngineTraceChannel4;   // 11
				uint8_t EngineTraceChannel5;   // 12
				uint8_t EngineTraceChannel6;   // 13

				// Unspecified Game Trace Channels
				uint8_t GameTraceChannel1;     // 14
				uint8_t GameTraceChannel2;     // 15
				uint8_t GameTraceChannel3;     // 16
				uint8_t GameTraceChannel4;     // 17
				uint8_t GameTraceChannel5;     // 18
				uint8_t GameTraceChannel6;     // 19
				uint8_t GameTraceChannel7;     // 20
				uint8_t GameTraceChannel8;     // 21
				uint8_t GameTraceChannel9;     // 22
				uint8_t GameTraceChannel10;    // 23
				uint8_t GameTraceChannel11;    // 24 
				uint8_t GameTraceChannel12;    // 25
				uint8_t GameTraceChannel13;    // 26
				uint8_t GameTraceChannel14;    // 27
				uint8_t GameTraceChannel15;    // 28
				uint8_t GameTraceChannel16;    // 29 
				uint8_t GameTraceChannel17;    // 30
				uint8_t GameTraceChannel18;    // 31
			};
			uint8_t EnumArray[32];
		};
	};


	struct FResponseChannel
	{

		FName Channel;

		TEnumAsByte<enum ECollisionResponse> Response;

		FResponseChannel()
			: Response(ECR_Block) {}

		FResponseChannel(FName InChannel, ECollisionResponse InResponse)
			: Channel(InChannel)
			, Response(InResponse) {}

		bool operator==(const FResponseChannel& Other) const
		{
			return Channel == Other.Channel && Response == Other.Response;
		}
	};

	struct FCollisionResponse
	{
		/** Types of objects that this physics objects will collide with. */
		// we have to still load them until resave
		FCollisionResponseContainer ResponseToChannels;

		/** Custom Channels for Responses */
		TArray<FResponseChannel> ResponseArray;

		friend struct FBodyInstance;
	};

	struct FWalkableSlopeOverride
	{


		/**
		 * Behavior of this surface (whether we affect the walkable slope).
		 * @see GetWalkableSlopeBehavior(), SetWalkableSlopeBehavior()
		 */
		TEnumAsByte<EWalkableSlopeBehavior> WalkableSlopeBehavior;

		/**
		 * Override walkable slope angle (in degrees), applying the rules of the Walkable Slope Behavior.
		 * @see GetWalkableSlopeAngle(), SetWalkableSlopeAngle()
		 */
		float WalkableSlopeAngle;

	private:

		// Cached angle for which we computed a cosine.
		mutable float CachedSlopeAngle;
		// Cached cosine of angle.
		mutable float CachedSlopeCos;

	public:

		FWalkableSlopeOverride()
			: WalkableSlopeBehavior(WalkableSlope_Default)
			, WalkableSlopeAngle(0.f)
			, CachedSlopeAngle(0.f)
			, CachedSlopeCos(1.f)
		{ }

		FWalkableSlopeOverride(EWalkableSlopeBehavior NewSlopeBehavior, float NewSlopeAngle)
			: WalkableSlopeBehavior(NewSlopeBehavior)
			, WalkableSlopeAngle(NewSlopeAngle)
			, CachedSlopeAngle(0.f)
			, CachedSlopeCos(1.f)
		{
		}

		// Gets the slope override behavior.
		FORCEINLINE EWalkableSlopeBehavior GetWalkableSlopeBehavior() const
		{
			return WalkableSlopeBehavior;
		}

		// Gets the slope angle used for the override behavior.
		FORCEINLINE float GetWalkableSlopeAngle() const
		{
			return WalkableSlopeAngle;
		}

		// Set the slope override behavior.
		FORCEINLINE void SetWalkableSlopeBehavior(EWalkableSlopeBehavior NewSlopeBehavior)
		{
			WalkableSlopeBehavior = NewSlopeBehavior;
		}

		// Set the slope angle used for the override behavior.
		FORCEINLINE void SetWalkableSlopeAngle(float NewSlopeAngle)
		{
			WalkableSlopeAngle = Math::Clamp(NewSlopeAngle, 0.f, 90.f);
		}

		// Given a walkable floor normal Z value, either relax or restrict the value if we override such behavior.
		float ModifyWalkableFloorZ(float InWalkableFloorZ) const
		{
			switch (WalkableSlopeBehavior)
			{
			case WalkableSlope_Default:
			{
				return InWalkableFloorZ;
			}

			case WalkableSlope_Increase:
			{
				//CheckCachedData();
				return physx::PxMin(InWalkableFloorZ, CachedSlopeCos);
			}

			case WalkableSlope_Decrease:
			{
				//CheckCachedData();
				return physx::PxMax(InWalkableFloorZ, CachedSlopeCos);
			}

			case WalkableSlope_Unwalkable:
			{
				// Z component of a normal will always be less than this, so this will be unwalkable.
				return 2.0f;
			}

			default:
			{
				return InWalkableFloorZ;
			}
			}
		}
	private:
		void CheckCachedData() const
		{
			/*if (CachedSlopeAngle != WalkableSlopeAngle)
			{
				const float AngleRads = FMath::DegreesToRadians(WalkableSlopeAngle);
				CachedSlopeCos = FMath::Clamp(FMath::Cos(AngleRads), 0.f, 1.f);
				CachedSlopeAngle = WalkableSlopeAngle;
			}*/
		}

	};
	class UBodySetup;

	// ScriptStruct Engine.BodyInstance
	// 0x0240
	struct FBodyInstance
	{
		unsigned char                                      UnknownData00[0x14];                                      // 0x0000(0x0014)	
		struct FCollisionResponseContainer                 ResponseToChannels;                                       // 0x0014(0x0020) (Deprecated, NoDestructor, NativeAccessSpecifierPublic) 	
		unsigned char                                      UnknownData01[0x4];                                       // 0x0034(0x0004)	
		struct FName                                       CollisionProfileName;                                     // 0x0038(0x0008) (Edit, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate) 	
		struct FCollisionResponse                          CollisionResponses;                                       // 0x0040(0x0030) (Edit, NativeAccessSpecifierPrivate) 	
		unsigned char                                      UnknownData02[0x4];                                       // 0x0070(0x0004)	
		unsigned char                                      bUseCCD : 1;                                              // 0x0074(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bNotifyRigidBodyCollision : 1;                            // 0x0074(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bSimulatePhysics : 1;                                     // 0x0074(0x0001) (Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bUseComplexForSimulation : 1;                             // 0x0074(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bOverrideMass : 1;                                        // 0x0074(0x0001) (Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bEnableGravity : 1;                                       // 0x0074(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bAutoWeld : 1;                                            // 0x0074(0x0001) (Edit, BlueprintVisible, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bStartAwake : 1;                                          // 0x0074(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bGenerateWakeEvents : 1;                                  // 0x0075(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bUpdateMassWhenScaleChanges : 1;                          // 0x0075(0x0001) (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bLockTranslation : 1;                                     // 0x0075(0x0001) (Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bLockRotation : 1;                                        // 0x0075(0x0001) (Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bLockXTranslation : 1;                                    // 0x0075(0x0001) (Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bLockYTranslation : 1;                                    // 0x0075(0x0001) (Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bLockZTranslation : 1;                                    // 0x0075(0x0001) (Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bLockXRotation : 1;                                       // 0x0075(0x0001) (Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bLockYRotation : 1;                                       // 0x0076(0x0001) (Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bLockZRotation : 1;                                       // 0x0076(0x0001) (Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bOverrideMaxAngularVelocity : 1;                          // 0x0076(0x0001) (Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      bUseAsyncScene : 1;                                       // 0x0076(0x0001) (Edit, BlueprintVisible, NoDestructor, AdvancedDisplay, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected) 	
		unsigned char                                      bOverrideMaxDepenetrationVelocity : 1;                    // 0x0077(0x0001) (Edit, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected) 	
		unsigned char                                      bOverrideWalkableSlopeOnInstance : 1;                     // 0x0077(0x0001) (Edit, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected) 	
		float                                              MaxDepenetrationVelocity;                                 // 0x0078(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, AdvancedDisplay, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected) 	
		unsigned char                                      UnknownData03[0x8];                                       // 0x007C(0x0008)	
		float                                              MassInKgOverride;                                         // 0x0084(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected) 	
		float                                              LinearDamping;                                            // 0x0088(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		float                                              AngularDamping;                                           // 0x008C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		struct FVector                                     CustomDOFPlaneNormal;                                     // 0x0090(0x000C) (Edit, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		struct FVector                                     COMNudge;                                                 // 0x009C(0x000C) (Edit, BlueprintVisible, IsPlainOldData, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      UnknownData04[0x10];                                      // 0x00A8(0x0010)	
		float                                              MassScale;                                                // 0x00B8(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      UnknownData05[0x14];                                      // 0x00BC(0x0014)	
		struct FWalkableSlopeOverride                      WalkableSlopeOverride;                                    // 0x00D0(0x0010) (Edit, BlueprintVisible, BlueprintReadOnly, IsPlainOldData, NoDestructor, AdvancedDisplay, Protected, NativeAccessSpecifierProtected) 	
		void*											   PhysMaterialOverride;									 // 0x00E0(0x0008) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected) 	
		float                                              MaxAngularVelocity;                                       // 0x00E8(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		float                                              CustomSleepThresholdMultiplier;                           // 0x00EC(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		float                                              StabilizationThresholdMultiplier;                         // 0x00F0(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		float                                              PhysicsBlendWeight;                                       // 0x00F4(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		int                                                PositionSolverIterationCount;                             // 0x00F8(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      UnknownData06[0x2C];                                      // 0x00FC(0x002C)	
		uint64_t                                           RigidActorSyncId;                                         // 0x0128(0x0008) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		uint64_t                                           RigidActorSyncIdunknown;                                  // 0x0130(0x0008) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		int                                                VelocitySolverIterationCount;                             // 0x0138(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		unsigned char                                      UnknownData07[0x14];                                      // 0x013C(0x0014)	
		TWeakObjectPtr<class UBodySetup>				   BodySetup;												 // 0x0150(0x0008)
		unsigned char                                      UnknownData08[0xDC];                                      // 0x0158(0x00F8)	
		ESleepFamily                                       SleepFamily;                                              // 0x0234(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		TEnumAsByte<EDOFMode>                              DOFMode;                                                  // 0x0235(0x0001) (Edit, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic) 	
		TEnumAsByte<ECollisionEnabled>                     CollisionEnabled;                                         // 0x0236(0x0001) (Edit, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate) 	
		TEnumAsByte<ECollisionChannel>                     ObjectType;                                               // 0x0237(0x0001) (Edit, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate) 	
	public:


	};

	const auto ab = FIELD_OFFSET(FBodyInstance, ObjectType);

	struct FFindFloorResult
	{
	public:
		/**
		* True if there was a blocking hit in the floor test that was NOT in initial penetration.
		* The HitResult can give more info about other circumstances.
		*/
		unsigned char bBlockingHit : 1;

		/** True if the hit found a valid walkable floor. */

		unsigned char bWalkableFloor : 1;

		/** True if the hit found a valid walkable floor using a line trace (rather than a sweep test, which happens when the sweep test fails to yield a walkable surface). */

		unsigned char bLineTrace : 1;

		unsigned char                                      UnknownData00[0x3];                                       // 0x0001(0x0003) MISSED OFFSET

		/** The distance to the floor, computed from the swept capsule trace. */

		float FloorDist;

		/** The distance to the floor, computed from the trace. Only valid if bLineTrace is true. */

		float LineDist;

		unsigned char                                      UnknownData01[0x4];                                       // 0x000C(0x0004) MISSED OFFSET

		/** Hit result of the test that found a floor. Includes more specific data about the point of impact and surface normal at that point. */

		FHitResult HitResult;
	public:
		/** Returns true if the floor result hit a walkable surface. */
		bool IsWalkableFloor() const
		{
			return bBlockingHit && bWalkableFloor;
		}

		/** Gets the distance to floor, either LineDist or FloorDist. */
		float GetDistanceToFloor() const
		{
			// When the floor distance is set using SetFromSweep, the LineDist value will be reset.
			// However, when SetLineFromTrace is used, there's no guarantee that FloorDist is set.
			return bLineTrace ? LineDist : FloorDist;
		}
	};

	struct UCharacterMovementComponent_BIT4 {
		uint32_t bMaintainHorizontalGroundVelocity : 1;
		uint32_t bImpartBaseVelocityX : 1;
		uint32_t bImpartBaseVelocityY : 1;
		uint32_t bImpartBaseVelocityZ : 1;
		uint32_t bImpartBaseAngularVelocity : 1;
		uint32_t bJustTeleported : 1;
		uint32_t bNetworkUpdateReceived : 1;
		uint32_t bNetworkMovementModeChanged : 1;
	};


	struct FAttackId
	{
		int                                                ID;                                                       // 0x0000(0x0004) (ZeroConstructor, IsPlainOldData)
	};

	struct FScriptMulticastDelegate
	{
		char UnknownData[16];
	};

	struct UMovementComponent_UpdatedComponent_Params
	{
		void* pawn;
	};

	struct UKismetSystemLibrary_LineTraceMulti_Params
	{
		class UObject** WorldContextObject;                                       // (Parm, ZeroConstructor, IsPlainOldData)
		struct FVector* Start;                                                    // (ConstParm, Parm, IsPlainOldData)
		struct FVector* End;                                                      // (ConstParm, Parm, IsPlainOldData)
		TEnumAsByte<ETraceTypeQuery>* TraceChannel;                                             // (Parm, ZeroConstructor, IsPlainOldData)
		bool* bTraceComplex;                                            // (Parm, ZeroConstructor, IsPlainOldData)
		TArray<class AActor*>* ActorsToIgnore;                                           // (ConstParm, Parm, OutParm, ZeroConstructor, ReferenceParm)
		TEnumAsByte<EDrawDebugTrace>* DrawDebugType;                                            // (Parm, ZeroConstructor, IsPlainOldData)
		TArray<struct FHitResult>                          OutHits;                                                  // (Parm, OutParm, ZeroConstructor)
		bool* bIgnoreSelf;                                              // (Parm, ZeroConstructor, IsPlainOldData)
		struct FLinearColor* TraceColor;                                               // (Parm, IsPlainOldData)
		struct FLinearColor* TraceHitColor;                                            // (Parm, IsPlainOldData)
		float* DrawTime;                                                 // (Parm, ZeroConstructor, IsPlainOldData)
		bool                                               ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
	};

	// ScriptStruct Engine.KeyHandleMap
	// 0x0050
	struct FKeyHandleMap
	{
		unsigned char                                      UnknownData00[0x50];                                      // 0x0000(0x0050) MISSED OFFSET
	};

	// ScriptStruct Engine.IndexedCurve
	// 0x0058
	struct FIndexedCurve
	{
		unsigned char                                      UnknownData00[0x8];                                       // 0x0000(0x0008) MISSED OFFSET
		struct FKeyHandleMap                               KeyHandlesToIndices;                                      // 0x0008(0x0050) (Transient)
	};

	// ScriptStruct Engine.RichCurveKey
	// 0x001C
	struct FRichCurveKey
	{
		TEnumAsByte<ERichCurveInterpMode>                  InterpMode;                                               // 0x0000(0x0001) (ZeroConstructor, IsPlainOldData)
		TEnumAsByte<ERichCurveTangentMode>                 TangentMode;                                              // 0x0001(0x0001) (ZeroConstructor, IsPlainOldData)
		TEnumAsByte<ERichCurveTangentWeightMode>           TangentWeightMode;                                        // 0x0002(0x0001) (ZeroConstructor, IsPlainOldData)
		unsigned char                                      UnknownData00[0x1];                                       // 0x0003(0x0001) MISSED OFFSET
		float                                              Time;                                                     // 0x0004(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
		float                                              Value;                                                    // 0x0008(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
		float                                              ArriveTangent;                                            // 0x000C(0x0004) (ZeroConstructor, IsPlainOldData)
		float                                              ArriveTangentWeight;                                      // 0x0010(0x0004) (ZeroConstructor, IsPlainOldData)
		float                                              LeaveTangent;                                             // 0x0014(0x0004) (ZeroConstructor, IsPlainOldData)
		float                                              LeaveTangentWeight;                                       // 0x0018(0x0004) (ZeroConstructor, IsPlainOldData)
	};

	/** Util to find float value on bezier defined by 4 control points */
	static float BezierInterp(float P0, float P1, float P2, float P3, float Alpha)
	{
		const float P01 = Math::Lerp(P0, P1, Alpha);
		const float P12 = Math::Lerp(P1, P2, Alpha);
		const float P23 = Math::Lerp(P2, P3, Alpha);
		const float P012 = Math::Lerp(P01, P12, Alpha);
		const float P123 = Math::Lerp(P12, P23, Alpha);
		const float P0123 = Math::Lerp(P012, P123, Alpha);

		return P0123;
	}

	static void CycleTime(float MinTime, float MaxTime, float& InTime, int& CycleCount)
	{
		float InitTime = InTime;
		float Duration = MaxTime - MinTime;

		if (InTime > MaxTime)
		{
			CycleCount = Math::FloorToInt((MaxTime - InTime) / Duration);
			InTime = InTime + Duration * CycleCount;
		}
		else if (InTime < MinTime)
		{
			CycleCount = Math::FloorToInt((InTime - MinTime) / Duration);
			InTime = InTime - Duration * CycleCount;
		}

		if (InTime == MaxTime && InitTime < MinTime)
		{
			InTime = MinTime;
		}

		if (InTime == MinTime && InitTime > MaxTime)
		{
			InTime = MaxTime;
		}

		CycleCount = Math::Abs(CycleCount);
	}

	// ScriptStruct Engine.RichCurve
	// 0x0018 (0x0070 - 0x0058)
	struct FRichCurve : public FIndexedCurve
	{
	public:
		/** Remap InTime based on pre and post infinity extrapolation values */
		void RemapTimeValue(float& InTime, float& CycleValueOffset) const
		{
			const int32_t NumKeys = Keys.Num();

			if (NumKeys < 2)
			{
				return;
			}

			if (InTime <= Keys[0].Time)
			{
				if (PreInfinityExtrap != RCCE_Linear && PreInfinityExtrap != RCCE_Constant)
				{
					float MinTime = Keys[0].Time;
					float MaxTime = Keys[NumKeys - 1].Time;

					int CycleCount = 0;
					CycleTime(MinTime, MaxTime, InTime, CycleCount);

					if (PreInfinityExtrap == RCCE_CycleWithOffset)
					{
						float DV = Keys[0].Value - Keys[NumKeys - 1].Value;
						CycleValueOffset = DV * CycleCount;
					}
					else if (PreInfinityExtrap == RCCE_Oscillate)
					{
						if (CycleCount % 2 == 1)
						{
							InTime = MinTime + (MaxTime - InTime);
						}
					}
				}
			}
			else if (InTime >= Keys[NumKeys - 1].Time)
			{
				if (PostInfinityExtrap != RCCE_Linear && PostInfinityExtrap != RCCE_Constant)
				{
					float MinTime = Keys[0].Time;
					float MaxTime = Keys[NumKeys - 1].Time;

					int CycleCount = 0;
					CycleTime(MinTime, MaxTime, InTime, CycleCount);

					if (PostInfinityExtrap == RCCE_CycleWithOffset)
					{
						float DV = Keys[NumKeys - 1].Value - Keys[0].Value;
						CycleValueOffset = DV * CycleCount;
					}
					else if (PostInfinityExtrap == RCCE_Oscillate)
					{
						if (CycleCount % 2 == 1)
						{
							InTime = MinTime + (MaxTime - InTime);
						}
					}
				}
			}
		}

		/** Evaluate this rich curve at the specified time */
		float Eval(float InTime, float InDefaultValue = 0.0f) const
		{
			// Remap time if extrapolation is present and compute offset value to use if cycling 
			float CycleValueOffset = 0;
			RemapTimeValue(InTime, CycleValueOffset);

			const int32_t NumKeys = Keys.Num();

			// If the default value hasn't been initialized, use the incoming default value
			float InterpVal = DefaultValue == MAX_flt ? InDefaultValue : DefaultValue;

			if (NumKeys == 0)
			{
				// If no keys in curve, return the Default value.
			}
			else if (NumKeys < 2 || (InTime <= Keys[0].Time))
			{
				if (PreInfinityExtrap == RCCE_Linear && NumKeys > 1)
				{
					float DT = Keys[1].Time - Keys[0].Time;

					if (Math::IsNearlyZero(DT))
					{
						InterpVal = Keys[0].Value;
					}
					else
					{
						float DV = Keys[1].Value - Keys[0].Value;
						float Slope = DV / DT;

						InterpVal = Slope * (InTime - Keys[0].Time) + Keys[0].Value;
					}
				}
				else
				{
					// Otherwise if constant or in a cycle or oscillate, always use the first key value
					InterpVal = Keys[0].Value;
				}
			}
			else if (InTime < Keys[NumKeys - 1].Time)
			{
				// perform a lower bound to get the second of the interpolation nodes
				int32_t first = 1;
				int32_t last = NumKeys - 1;
				int32_t count = last - first;

				while (count > 0)
				{
					int32_t step = count / 2;
					int32_t middle = first + step;

					if (InTime >= Keys[middle].Time)
					{
						first = middle + 1;
						count -= step + 1;
					}
					else
					{
						count = step;
					}
				}

				int32_t InterpNode = first;
				const float Diff = Keys[InterpNode].Time - Keys[InterpNode - 1].Time;

				if (Diff > 0.f && Keys[InterpNode - 1].InterpMode != RCIM_Constant)
				{
					const float Alpha = (InTime - Keys[InterpNode - 1].Time) / Diff;
					const float P0 = Keys[InterpNode - 1].Value;
					const float P3 = Keys[InterpNode].Value;

					if (Keys[InterpNode - 1].InterpMode == RCIM_Linear)
					{
						InterpVal = Math::Lerp(P0, P3, Alpha);
					}
					else
					{
						const float OneThird = 1.0f / 3.0f;
						const float P1 = P0 + (Keys[InterpNode - 1].LeaveTangent * Diff * OneThird);
						const float P2 = P3 - (Keys[InterpNode].ArriveTangent * Diff * OneThird);

						InterpVal = BezierInterp(P0, P1, P2, P3, Alpha);
					}
				}
				else
				{
					InterpVal = Keys[InterpNode - 1].Value;
				}
			}
			else
			{
				if (PostInfinityExtrap == RCCE_Linear)
				{
					float DT = Keys[NumKeys - 2].Time - Keys[NumKeys - 1].Time;

					if (Math::IsNearlyZero(DT))
					{
						InterpVal = Keys[NumKeys - 1].Value;
					}
					else
					{
						float DV = Keys[NumKeys - 2].Value - Keys[NumKeys - 1].Value;
						float Slope = DV / DT;

						InterpVal = Slope * (InTime - Keys[NumKeys - 1].Time) + Keys[NumKeys - 1].Value;
					}
				}
				else
				{
					// Otherwise if constant or in a cycle or oscillate, always use the last key value
					InterpVal = Keys[NumKeys - 1].Value;
				}
			}

			return InterpVal + CycleValueOffset;
		}

		/** Remap InTime based on pre and post infinity extrapolation values */
		void RemapTimeValueModifier(float& InTime, float& CycleValueOffset, std::vector<FRichCurveKey> NewKeys) const
		{
			int32_t NumKeys = (int32_t)NewKeys.size();

			if (NumKeys < 2)
			{
				return;
			}

			if (InTime <= NewKeys[0].Time)
			{
				if (PreInfinityExtrap != RCCE_Linear && PreInfinityExtrap != RCCE_Constant)
				{
					float MinTime = NewKeys[0].Time;
					float MaxTime = NewKeys[NumKeys - 1].Time;

					int CycleCount = 0;
					CycleTime(MinTime, MaxTime, InTime, CycleCount);

					if (PreInfinityExtrap == RCCE_CycleWithOffset)
					{
						float DV = NewKeys[0].Value - NewKeys[NumKeys - 1].Value;
						CycleValueOffset = DV * CycleCount;
					}
					else if (PreInfinityExtrap == RCCE_Oscillate)
					{
						if (CycleCount % 2 == 1)
						{
							InTime = MinTime + (MaxTime - InTime);
						}
					}
				}
			}
			else if (InTime >= NewKeys[NumKeys - 1].Time)
			{
				if (PostInfinityExtrap != RCCE_Linear && PostInfinityExtrap != RCCE_Constant)
				{
					float MinTime = NewKeys[0].Time;
					float MaxTime = NewKeys[NumKeys - 1].Time;

					int CycleCount = 0;
					CycleTime(MinTime, MaxTime, InTime, CycleCount);

					if (PostInfinityExtrap == RCCE_CycleWithOffset)
					{
						float DV = NewKeys[NumKeys - 1].Value - NewKeys[0].Value;
						CycleValueOffset = DV * CycleCount;
					}
					else if (PostInfinityExtrap == RCCE_Oscillate)
					{
						if (CycleCount % 2 == 1)
						{
							InTime = MinTime + (MaxTime - InTime);
						}
					}
				}
			}
		}

		/** Evaluate this rich curve at the specified time */
		float EvalModifier(float InTime, std::vector<FRichCurveKey> NewKeys, float InDefaultValue = 0.0f) const
		{
			// Remap time if extrapolation is present and compute offset value to use if cycling 
			float CycleValueOffset = 0;
			RemapTimeValueModifier(InTime, CycleValueOffset, NewKeys);

			int32_t NumKeys = (int32_t)NewKeys.size();

			// If the default value hasn't been initialized, use the incoming default value
			float InterpVal = DefaultValue == MAX_flt ? InDefaultValue : DefaultValue;

			if (NumKeys == 0)
			{
				// If no keys in curve, return the Default value.
			}
			else if (NumKeys < 2 || (InTime <= NewKeys[0].Time))
			{
				if (PreInfinityExtrap == RCCE_Linear && NumKeys > 1)
				{
					float DT = NewKeys[1].Time - NewKeys[0].Time;

					if (Math::IsNearlyZero(DT))
					{
						InterpVal = NewKeys[0].Value;
					}
					else
					{
						float DV = NewKeys[1].Value - NewKeys[0].Value;
						float Slope = DV / DT;

						InterpVal = Slope * (InTime - NewKeys[0].Time) + NewKeys[0].Value;
					}
				}
				else
				{
					// Otherwise if constant or in a cycle or oscillate, always use the first key value
					InterpVal = NewKeys[0].Value;
				}
			}
			else if (InTime < NewKeys[NumKeys - 1].Time)
			{
				// perform a lower bound to get the second of the interpolation nodes
				int32_t first = 1;
				int32_t last = NumKeys - 1;
				int32_t count = last - first;

				while (count > 0)
				{
					int32_t step = count / 2;
					int32_t middle = first + step;

					if (InTime >= NewKeys[middle].Time)
					{
						first = middle + 1;
						count -= step + 1;
					}
					else
					{
						count = step;
					}
				}

				int32_t InterpNode = first;
				const float Diff = NewKeys[InterpNode].Time - NewKeys[InterpNode - 1].Time;

				if (Diff > 0.f && NewKeys[InterpNode - 1].InterpMode != RCIM_Constant)
				{
					const float Alpha = (InTime - NewKeys[InterpNode - 1].Time) / Diff;
					const float P0 = NewKeys[InterpNode - 1].Value;
					const float P3 = NewKeys[InterpNode].Value;

					if (NewKeys[InterpNode - 1].InterpMode == RCIM_Linear)
					{
						InterpVal = Math::Lerp(P0, P3, Alpha);
					}
					else
					{
						const float OneThird = 1.0f / 3.0f;
						const float P1 = P0 + (NewKeys[InterpNode - 1].LeaveTangent * Diff * OneThird);
						const float P2 = P3 - (NewKeys[InterpNode].ArriveTangent * Diff * OneThird);

						InterpVal = BezierInterp(P0, P1, P2, P3, Alpha);
					}
				}
				else
				{
					InterpVal = NewKeys[InterpNode - 1].Value;
				}
			}
			else
			{
				if (PostInfinityExtrap == RCCE_Linear)
				{
					float DT = NewKeys[NumKeys - 2].Time - NewKeys[NumKeys - 1].Time;

					if (Math::IsNearlyZero(DT))
					{
						InterpVal = NewKeys[NumKeys - 1].Value;
					}
					else
					{
						float DV = NewKeys[NumKeys - 2].Value - NewKeys[NumKeys - 1].Value;
						float Slope = DV / DT;

						InterpVal = Slope * (InTime - NewKeys[NumKeys - 1].Time) + NewKeys[NumKeys - 1].Value;
					}
				}
				else
				{
					// Otherwise if constant or in a cycle or oscillate, always use the last key value
					InterpVal = NewKeys[NumKeys - 1].Value;
				}
			}

			return InterpVal + CycleValueOffset;
		}
	public:
		TEnumAsByte<ERichCurveExtrapolation>               PreInfinityExtrap;                                        // 0x0058(0x0001) (ZeroConstructor, IsPlainOldData)
		TEnumAsByte<ERichCurveExtrapolation>               PostInfinityExtrap;                                       // 0x0059(0x0001) (ZeroConstructor, IsPlainOldData)
		unsigned char                                      UnknownData00[0x2];                                       // 0x005A(0x0002) MISSED OFFSET
		float                                              DefaultValue;                                             // 0x005C(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
		TArray<struct FRichCurveKey>                       Keys;                                                     // 0x0060(0x0010) (Edit, EditFixedSize, ZeroConstructor)
	};

	struct FRichCurve_3
	{
		FRichCurve FloatCurves[3];
	};
	static_assert(sizeof(FRichCurve_3) == 0x150, "Size Check");


	// This contains Reference-skeleton related info
// Bone transform is saved as FTransform array
	struct FMeshBoneInfo
	{
		// Bone's name.
		FName Name;

		// 0/NULL if this is the root bone. 
		int32_t ParentIndex;

#if WITH_EDITORONLY_DATA
		// Name used for export (this should be exact as FName may mess with case) 
		FString ExportName;
#endif

		FMeshBoneInfo() : Name(NAME_None), ParentIndex(INDEX_NONE) {}

		FMeshBoneInfo(const FName& InName, const FString& InExportName, int32_t InParentIndex)
			: Name(InName)
			, ParentIndex(InParentIndex)
#if WITH_EDITORONLY_DATA
			, ExportName(InExportName)
#endif
		{}

		FMeshBoneInfo(const FMeshBoneInfo& Other)
			: Name(Other.Name)
			, ParentIndex(Other.ParentIndex)
#if WITH_EDITORONLY_DATA
			, ExportName(Other.ExportName)
#endif
		{}

		bool operator==(const FMeshBoneInfo& B) const
		{
			return(Name == B.Name);
		}

	};

	// Cached Virtual Bone data from USkeleton
	struct FVirtualBoneRefData
	{
		int32_t VBRefSkelIndex;
		int32_t SourceRefSkelIndex;
		int32_t TargetRefSkelIndex;

		FVirtualBoneRefData(int32_t InVBRefSkelIndex, int32_t InSourceRefSkelIndex, int32_t InTargetRefSkelIndex)
			: VBRefSkelIndex(InVBRefSkelIndex)
			, SourceRefSkelIndex(InSourceRefSkelIndex)
			, TargetRefSkelIndex(InTargetRefSkelIndex)
		{
		}
	};

	/** Reference Skeleton **/
	struct FReferenceSkeleton
	{
	public:
		FORCENOINLINE int32_t FindBoneIndex(const FName& BoneName)
		{
			int32_t BoneIndex = INDEX_NONE;
			if (BoneName != NAME_None)
			{
				auto finalNameToIndexMap = this->FinalNameToIndexMap;
				auto pMap = reinterpret_cast<ptr_t>(finalNameToIndexMap.UnknownData);
				if (*(uint32_t*)(pMap + 8) != *(uint32_t*)(pMap + 0x34))
				{
					ptr_t v4 = *(ptr_t*)(pMap + 0x40);
					ptr_t v5 = 0;
					if (!v4)
						v5 = pMap + 0x38;

					int32_t v6 = -1;
					if (v4)
						v6 = GetDMA().Read<int32_t>(v4 + 4 * ((*(int32_t*)(pMap + 0x48) - 1i64) & (BoneName.ComparisonIndex + BoneName.Number)));
					//v6 = *Game.memory().Read<int32_t>(v4 + 4 * ((*(int32_t*)(pMap + 0x48) - 1i64) & (BoneName.ComparisonIndex + BoneName.Number)));
					else if (v5)
						v6 = GetDMA().Read<int32_t>(v5 + 4 * ((*(int32_t*)(pMap + 0x48) - 1i64) & (BoneName.ComparisonIndex + BoneName.Number)));
					//v6 = *Game.memory().Read<int32_t>(v5 + 4 * ((*(int32_t*)(pMap + 0x48) - 1i64) & (BoneName.ComparisonIndex + BoneName.Number)));

					ptr_t v8 = 0;
					if (v6 != -1)
					{
						int LoopCount = 0;
						ptr_t array = *(ptr_t*)pMap;
						while (array)
						{
							LoopCount++;
							if (LoopCount >= 10)
								break;
							v8 = array + 0x18ull * v6;
							if (GetDMA().Read<ptr_t>(v8) == BoneName.CompositeComparisonValue)
								//if (*Game.memory().Read<ptr_t>(v8) == BoneName.CompositeComparisonValue)
								break;

							v6 = GetDMA().Read<int32_t>(v8 + 0x10);
							//v6 = *Game.memory().Read<int32_t>(v8 + 0x10);
							if (v6 == -1)
								return BoneIndex;
						}
						if (v8)
							BoneIndex = GetDMA().Read<int32_t>(v8 + 8);
						//BoneIndex = *Game.memory().Read<int32_t>(v8 + 8);
					}

				}

			}
			return BoneIndex;
		}

		int32_t GetParentIndexInternal(const int32_t BoneIndex, const TArray<FMeshBoneInfo>& BoneInfo) const
		{
			const int32_t ParentIndex = BoneInfo[BoneIndex].ParentIndex;

			// Parent must be valid. Either INDEX_NONE for Root, or before children for non root bones.
			assert(((BoneIndex == 0) && (ParentIndex == INDEX_NONE))
				|| ((BoneIndex > 0) && BoneInfo.IsValidIndex(ParentIndex) && (ParentIndex < BoneIndex)));

			return ParentIndex;
		}

		FName GetBoneName(const int32_t BoneIndex) const
		{
			return FinalRefBoneInfo[BoneIndex].Name;
		}

		int32_t GetParentIndex(const int32_t BoneIndex) const
		{
			return GetParentIndexInternal(BoneIndex, FinalRefBoneInfo);
		}
	public:
		//RAW BONES: Bones that exist in the original asset
		/** Reference bone related info to be serialized **/
		TArray<FMeshBoneInfo>	RawRefBoneInfo;
		/** Reference bone transform **/
		TArray<FTransform>		RawRefBonePose;

		//FINAL BONES: Bones for this skeleton including user added virtual bones
		/** Reference bone related info to be serialized **/
		TArray<FMeshBoneInfo>	FinalRefBoneInfo;
		/** Reference bone transform **/
		TArray<FTransform>		FinalRefBonePose;

		/** TMap to look up bone index from bone name. */
		TMap/*<FName, int32_t>*/		RawNameToIndexMap;
		TMap/*<FName, int32_t>*/		FinalNameToIndexMap;

		// cached data to allow virtual bones to be built into poses
		TArray<FBoneIndexType>  RequiredVirtualBones;
		TArray<FVirtualBoneRefData> UsedVirtualBoneData;

	};

	struct FWeaponAttachmentData
	{
		int itemID;
		std::string itemName;
		std::string itemCategory;
		//inline FWeaponAttachmentData()
		//	: itemID(0), itemName(""), itemCategory(""){
		//}
		inline FWeaponAttachmentData(int _itemID, std::string _itemName, std::string _itemCategory)
			: itemID(_itemID),
			itemName(_itemName),
			itemCategory(_itemCategory) {
		}
	};

	struct vSouleWorldMapUnknownData {
		int UnknownID;
		unsigned char Unknown_0[0xC];
		union {
			struct {
				int RightTab; // 默认 16777216 按下 0
			};
			struct {
				unsigned char cRightTab[0x4]; // 默认 0 0 0 1 按下 0 0 0 0
			};
		};
		unsigned char Unknown_1[0x4];
		union {
			struct {
				int LeftClick; // 默认 16777216 按下 16777217
			};
			struct {
				unsigned char cLeftClick[0x4]; // 默认 0 0 0 1 按下 1 0 0 1
			};
		};
		unsigned char Unknown_2[0x4];
		struct FVector2D AnimTime;
		float ZoomLevel;
		unsigned char Unknown_3[0x4];
		struct FVector2D Unknown2D_0;
		struct FVector2D Unknown2D_1;
		float GetZoomLevel() {
			return ZoomLevel;
		}
		int GetRightTabID() {
			return UnknownID;
		}
		bool IsRightClicking() {
			return (cRightTab[3] == (BYTE)0);
		}
		bool IsLeftClicking() {
			return (cLeftClick[0] == (BYTE)0x1);
		}
	};


	// ScriptStruct SlateCore.Margin
	// 0x0010
	struct FMargin
	{
		float                                              Left;                                                     // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
		float                                              Top;                                                      // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
		float                                              Right;                                                    // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
		float                                              Bottom;                                                   // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	};

	// ScriptStruct Slate.Anchors
	// 0x0010
	struct FAnchors
	{
		struct FVector2D                                   Minimum;                                                  // 0x0000(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
		struct FVector2D                                   Maximum;                                                  // 0x0008(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	};

	// ScriptStruct UMG.AnchorData
	// 0x0028
	struct FAnchorData
	{
		struct FMargin                                     Offsets;                                                  // 0x0000(0x0010) (Edit, BlueprintVisible)
		struct FAnchors                                    Anchors;                                                  // 0x0010(0x0010) (Edit, BlueprintVisible)
		struct FVector2D                                   Alignment;                                                // 0x0020(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	};

	// ScriptStruct CoreUObject.LinearColor
	// 0x0010
	struct FLinearColor
	{
		float                                              R;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float                                              G;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float                                              B;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float                                              A;                                                        // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		inline FLinearColor()
			: R(0), G(0), B(0), A(0) {
		}

		inline FLinearColor(float _R, float _G, float _B, float _A)
			: R(_R),
			G(_G),
			B(_B),
			A(_A) {
		}
	};

	// ScriptStruct TslGame.WuPubgIdData
	// 0x0090
	struct FWuPubgIdData
	{
		struct FString                                     AccountId;												 // 0x0000(0x0010) (ZeroConstructor)
		struct FString                                     Nickname;												 // 0x0010(0x0010) (ZeroConstructor)
		int												   SurvivalTier;											 // 0x0020(0x0004) MISSED OFFSET
		int                                                SurvivalLevel;                                            // 0x0024(0x0004) (ZeroConstructor, IsPlainOldData)
		int                                                ReputationLevel;                                          // 0x0028(0x0004) (ZeroConstructor, IsPlainOldData)
		struct FString                                     NameplateId;                                              // 0x0030(0x0010) (ZeroConstructor)
		struct FString                                     EmblemId;                                                 // 0x0040(0x0010) (ZeroConstructor)
		struct FString                                     MedalOne;                                                 // 0x0050(0x0010) (ZeroConstructor)
		struct FString                                     MedalTwo;                                                 // 0x0060(0x0010) (ZeroConstructor)
		struct FString                                     ClanTag;													 // 0x0070(0x0010) (ZeroConstructor)
		struct FString                                     ClanplateId;                                              // 0x0080(0x0010) (ZeroConstructor)
	};

	struct Inventory_Equipment
	{
	public:
		std::string itemName;
		int LootId;
		float Durability;
		float DurabilityMax;
		inline Inventory_Equipment()
			: itemName(""), LootId(0), Durability(0.f), DurabilityMax(0.f) {
		}
		inline Inventory_Equipment(std::string _itemName, int _LootId, float _Durability, float _DurabilityMax)
			: itemName(_itemName),
			LootId(_LootId),
			Durability(_Durability),
			DurabilityMax(_DurabilityMax) {
		}
	};
}



