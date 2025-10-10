#include "Include/WinHeaders.hpp"
#include "Core/Auth.h"
#include "Core/ItemManager.h"
#include "SDK.h"
#include "ObjectsStore.h"
#include "ObjectManager.h"
#include "../Config.h"
#include <thread>

namespace IronMan::Core::SDK
{
	static ptr_t offsetOfCanvasWidget = 0;

	HANDLE ObjectManager::_hThread = nullptr;
	bool ObjectManager::_bWork = false;
	WfirstRWLock ObjectManager::_rwLock;

	std::unordered_map <ptr_t, std::shared_ptr<AActor>>  ObjectManager::cachedObjects;
	std::unordered_map <ptr_t, std::shared_ptr<AActor>>  cachedObjectsCopy;

	bool ClearOnce = false;
	ptr_t DecryptAActor = 0;
	//MyHud
	ptr_t pWidgetStateMap = 0;
	//小地图
	void* pCanvasWidget = nullptr;
	int32_t EncryptCanvasWidgetID = 0;
	float pMinimapSizeOffset = 0;
	int32_t pSelectMinimapSizeIndex = 0;
	//Fog
	TArray<void*> DormantCharacterClientList;
	std::vector<ptr_t>FogPlayer;
	//本人
	ptr_t DecryptRootComponent = 0;
	ptr_t DecryptInventoryFacede = 0;
	ptr_t m_LocalMesh = 0;
	ptr_t m_LocalWeaponPorcess = 0;
	ptr_t pEquipment = 0;
	ptr_t CurrentWeapon = 0;
	ptr_t CurrentWeapon1 = 0;
	ptr_t CurrentWeapon2 = 0;
	std::vector<void*>Equipment_ItemsObj;
	ptr_t pMesh3P = 0;
	int32_t pAttachedStaticComponentIndex;
	TArray<UAttachableItem> pAttachedItem = 0;
	ItemInfoArrayData pAttachArrayData;
	ptr_t pWeaponTrajectoryData = 0;
	std::vector<void*>pAttachedItemSaveArray;
	std::vector<void*> pAttachedItemTableRowSaveArray;
	std::vector<FName>FNameArray;
	std::vector<FText>ItemFTextNameArray;
	std::vector<FText>ItemFTextCategoryArray;
	std::vector<FTextstrdata>ItemNameDataArray;
	std::vector<FTextstrdata>ItemCategoryDataArray;
	std::vector<std::wstring>ItemWNameArray;
	std::vector<std::wstring>ItemWCategoryArray;
	std::vector<std::string>ItemNameArray;
	std::vector<std::string>ItemCategoryArray;

	std::vector<void*>SocketsList;
	std::vector<FName>SocketNameList;
	std::vector<void*>SkeletonSocketsList;
	std::vector<FName>SkeletonSocketNameList;

	bool initCmpClass = false;

	OMFClassBaseChain EPlayerUClass;
	OMFClassBaseChain EVehicleUClass;
	OMFClassBaseChain EBoatUClass;
	OMFClassBaseChain EItemGroupUClass;
	OMFClassBaseChain EItemUClass;
	OMFClassBaseChain EDeathBoxUClass;
	OMFClassBaseChain EAirdropUClass;
	OMFClassBaseChain EProjectileUClass;
	OMFClassBaseChain EDroppedItemUClass;

	ptr_t offset_AntiCheatCharacterSyncManager = 0;
	ptr_t	t_PlayerState1 = 0;
	ptr_t	t_PlayerState2 = 0;
	ptr_t	t_VehicleRiderComponent = 0;
	ptr_t	t_LastVehiclePawn = 0;

	bool ObjectManager::InitializeScatterHandle()
	{
		GetDMA().clearHandleVector();
		if (sdk::GetHandle1())
			GetDMA().closeScatterHandle(sdk::GetHandle1());
		if (sdk::GetHandle2())
			GetDMA().closeScatterHandle(sdk::GetHandle2());
		if (sdk::GetUpdateData())
			GetDMA().closeScatterHandle(sdk::GetUpdateData());
		if (sdk::GetSelfUpdateHandle())
			GetDMA().closeScatterHandle(sdk::GetSelfUpdateHandle());
		if (sdk::GetViewPortHandle())
			GetDMA().closeScatterHandle(sdk::GetViewPortHandle());
		if (sdk::GetUpdatePlayer())
			GetDMA().closeScatterHandle(sdk::GetUpdatePlayer());
		DWORD old;
		VirtualProtect(_Scatter_buffer, 0x1000, PAGE_READWRITE, &old);
		*(void**)((ptr_t)_Scatter_buffer + 0x00) = GetDMA().createScatterHandle();
		*(void**)((ptr_t)_Scatter_buffer + 0x10) = GetDMA().createScatterHandle();
		*(void**)((ptr_t)_Scatter_buffer + 0x20) = GetDMA().createScatterHandle();
		*(void**)((ptr_t)_Scatter_buffer + 0x30) = GetDMA().createScatterHandle();
		*(void**)((ptr_t)_Scatter_buffer + 0x40) = GetDMA().createScatterHandle();
		*(void**)((ptr_t)_Scatter_buffer + 0x50) = GetDMA().createScatterHandle();
		VirtualProtect(_Scatter_buffer, 0x1000, PAGE_READONLY, &old);
		ZeroMemory(&EPlayerUClass, sizeof(EPlayerUClass));
		ZeroMemory(&EVehicleUClass, sizeof(EVehicleUClass));
		ZeroMemory(&EBoatUClass, sizeof(EBoatUClass));
		ZeroMemory(&EItemGroupUClass, sizeof(EItemGroupUClass));
		ZeroMemory(&EItemUClass, sizeof(EItemUClass));
		ZeroMemory(&EDeathBoxUClass, sizeof(EDeathBoxUClass));
		ZeroMemory(&EAirdropUClass, sizeof(EAirdropUClass));
		ZeroMemory(&EProjectileUClass, sizeof(EProjectileUClass));
		ZeroMemory(&EDroppedItemUClass, sizeof(EDroppedItemUClass));
		initCmpClass = false;
		if (!initCmpClass)
		{
			EPlayerUClass = OMFClassBaseChain((ptr_t)ATslCharacter::StaticClass()->GetPtr(), GetDMA().Read<int32_t>((ptr_t)ATslCharacter::StaticClass()->GetPtr() + g_PatternData.offset_UClassBaseChainArray + 8));
			EVehicleUClass = OMFClassBaseChain((ptr_t)ATslWheeledVehicle::StaticClass()->GetPtr(), GetDMA().Read<int32_t>((ptr_t)ATslWheeledVehicle::StaticClass()->GetPtr() + g_PatternData.offset_UClassBaseChainArray + 8));
			EBoatUClass = OMFClassBaseChain((ptr_t)ATslFloatingVehicle::StaticClass()->GetPtr(), GetDMA().Read<int32_t>((ptr_t)ATslFloatingVehicle::StaticClass()->GetPtr() + g_PatternData.offset_UClassBaseChainArray + 8));
			EItemGroupUClass = OMFClassBaseChain((ptr_t)ADroppedItemGroup::StaticClass()->GetPtr(), GetDMA().Read<int32_t>((ptr_t)ADroppedItemGroup::StaticClass()->GetPtr() + g_PatternData.offset_UClassBaseChainArray + 8));
			EItemUClass = OMFClassBaseChain((ptr_t)ADroppedItem::StaticClass()->GetPtr(), GetDMA().Read<int32_t>((ptr_t)ADroppedItem::StaticClass()->GetPtr() + g_PatternData.offset_UClassBaseChainArray + 8));
			EDeathBoxUClass = OMFClassBaseChain((ptr_t)AFloorSnapItemPackage::StaticClass()->GetPtr(), GetDMA().Read<int32_t>((ptr_t)AFloorSnapItemPackage::StaticClass()->GetPtr() + g_PatternData.offset_UClassBaseChainArray + 8));
			EAirdropUClass = OMFClassBaseChain((ptr_t)ACarePackageItem::StaticClass()->GetPtr(), GetDMA().Read<int32_t>((ptr_t)ACarePackageItem::StaticClass()->GetPtr() + g_PatternData.offset_UClassBaseChainArray + 8));
			EProjectileUClass = OMFClassBaseChain((ptr_t)ATslProjectile::StaticClass()->GetPtr(), GetDMA().Read<int32_t>((ptr_t)ATslProjectile::StaticClass()->GetPtr() + g_PatternData.offset_UClassBaseChainArray + 8));
			EDroppedItemUClass = OMFClassBaseChain((ptr_t)UDroppedItemInteractionComponent::StaticClass()->GetPtr(), GetDMA().Read<int32_t>((ptr_t)UDroppedItemInteractionComponent::StaticClass()->GetPtr() + g_PatternData.offset_UClassBaseChainArray + 8));
			if (EPlayerUClass.ClassBaseChainArray == 0 && EPlayerUClass.NumClassBasesInChainMinusOne == 0)
			{
				CONSOLE_INFO("EPlayerUClass Failed!%p", ATslCharacter::StaticClass()->GetPtr());
				return false;
			}
			if (EVehicleUClass.ClassBaseChainArray == 0 && EVehicleUClass.NumClassBasesInChainMinusOne == 0)
			{
				CONSOLE_INFO("EVehicleUClass Failed!%p", ATslWheeledVehicle::StaticClass()->GetPtr());
				return false;
			}
			if (EBoatUClass.ClassBaseChainArray == 0 && EBoatUClass.NumClassBasesInChainMinusOne == 0)
			{
				CONSOLE_INFO("EBoatUClass Failed!%p", ATslFloatingVehicle::StaticClass()->GetPtr());
				return false;
			}
			if (EItemGroupUClass.ClassBaseChainArray == 0 && EItemGroupUClass.NumClassBasesInChainMinusOne == 0)
			{
				CONSOLE_INFO("EItemGroupUClass Failed!%p", ADroppedItemGroup::StaticClass()->GetPtr());
				return false;
			}
			if (EItemUClass.ClassBaseChainArray == 0 && EItemUClass.NumClassBasesInChainMinusOne == 0)
			{
				CONSOLE_INFO("EItemUClass Failed!%p", ADroppedItem::StaticClass()->GetPtr());
				return false;
			}
			if (EDeathBoxUClass.ClassBaseChainArray == 0 && EDeathBoxUClass.NumClassBasesInChainMinusOne == 0)
			{
				CONSOLE_INFO("EDeathBoxUClass Failed!%p", AFloorSnapItemPackage::StaticClass()->GetPtr());
				return false;
			}
			if (EAirdropUClass.ClassBaseChainArray == 0 && EAirdropUClass.NumClassBasesInChainMinusOne == 0)
			{
				CONSOLE_INFO("EAirdropUClass Failed!%p", ACarePackageItem::StaticClass()->GetPtr());
				return false;
			}
			if (EProjectileUClass.ClassBaseChainArray == 0 && EProjectileUClass.NumClassBasesInChainMinusOne == 0)
			{
				CONSOLE_INFO("EProjectileUClass Failed!%p", ATslProjectile::StaticClass()->GetPtr());
				return false;
			}
			if (EDroppedItemUClass.ClassBaseChainArray == 0 && EDroppedItemUClass.NumClassBasesInChainMinusOne == 0)
			{
				CONSOLE_INFO("EDroppedItemUClass Failed!%p", UDroppedItemInteractionComponent::StaticClass()->GetPtr());
				return false;
			}
			initCmpClass = true;
		}
		return initCmpClass;
	}

	bool ObjectManager::Initialize()
	{
		_bWork = true;
		CreateThread(NULL, NULL, UpdateThread, NULL, NULL, NULL);
		sdk::OnUpdate += []() {UpdateWorldThread(0); };
		return true;
	}

	AActor* ObjectManager::PrepareCreateObjectPointerFromObject(AActor& gameObj, UObjectType Type)
	{
		if (!gameObj->IsValid())
			return nullptr;
		if (Type == EProjectile && gameObj->SetIsA(EProjectile))
		{
			return new ATslProjectile(gameObj);
		}
		else if (Type == EPlayer && gameObj->SetIsA(EPlayer))
		{
			return new ATslCharacter(gameObj);
		}
		else if (Type == EVehicle && gameObj->SetIsA(EVehicle))
		{
			return new ATslWheeledVehicle(gameObj);
		}
		else if (Type == EBoat && gameObj->SetIsA(EBoat))
		{
			return new ATslFloatingVehicle(gameObj);
		}
		else if (Type == EItemGroup && gameObj->SetIsA(EItemGroup))
		{
			return new ADroppedItemGroup(gameObj);
		}
		else if (Type == EItem && gameObj->SetIsA(EItem))
		{
			return new ADroppedItem(gameObj);
		}
		else if (Type == EAirdrop && gameObj->SetIsA(EAirdrop))
		{
			return new ACarePackageItem(gameObj);
		}
		else if (Type == EDeathBox && gameObj->SetIsA(EDeathBox))
		{
			return new AFloorSnapItemPackage(gameObj);
		}
		gameObj->SetObjectType(UObjectType::EOther);
		return new AActor(gameObj);
	}

	AActor* ObjectManager::CreateObjectPointerFromObject(AActor& gameObj)
	{
		if (!gameObj->IsValid())
			return nullptr;
		if (gameObj->IsA(EProjectile))
		{
			return new ATslProjectile(gameObj);
		}
		else if (gameObj->IsA(EPlayer))
		{
			return new ATslCharacter(gameObj);
		}
		else if (gameObj->IsA(EVehicle))
		{
			return new ATslWheeledVehicle(gameObj);
		}
		else if (gameObj->IsA(EBoat))
		{
			return new ATslFloatingVehicle(gameObj);
		}
		else if (gameObj->IsA(EItemGroup))
		{
			return new ADroppedItemGroup(gameObj);
		}
		else if (gameObj->IsA(EItem))
		{
			return new ADroppedItem(gameObj);
		}
		else if (gameObj->IsA(EAirdrop))
		{
			return new ACarePackageItem(gameObj);
		}
		else if (gameObj->IsA(EDeathBox))
		{
			return new AFloorSnapItemPackage(gameObj);
		}
		gameObj->SetObjectType(UObjectType::EOther);
		return new AActor(gameObj);
	}

	UActorComponent* ObjectManager::CreateComponentObjectPointerFromObject(UActorComponent& gameObj)
	{
		if (!gameObj->IsValid())
			return nullptr;

		if (gameObj->IsA(EDroppedItem))
			return new UDroppedItemInteractionComponent(gameObj);

		gameObj->SetObjectType(UObjectType::EOther);
		return new UActorComponent(gameObj);
	}

	DWORD ObjectManager::UpdateWorldThread(LPVOID lpParam)
	{
		do
		{
			if (Vars.Menu.PauseEveryThing)
			{
				Sleep(1000);
				continue;
			}
			try
			{
				if (!Auth::Check())
				{
					CONSOLE_INFO2(u8"驗證使用者資訊失敗");
					break;
				}
				GetDMA().FreshFREQ();
				Sleep(600);
				GameWorld();
			}
			catch (...)
			{
				CONSOLE_INFO2(u8"UpdateWorld 出現異常");
			}
		} while (true);
		return 0;
	}

	DWORD ObjectManager::UpdateThread(LPVOID lpParam)
	{
		using framerate = std::chrono::duration<int, std::ratio<1, 10>>;
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
				if (!Auth::Check())
				{
					return 0;
				}
				GameUpdate();
				CacheObjects();
				std::this_thread::sleep_until(tp);
				tp += framerate{ 1 };
			}
			catch (...)
			{
				CONSOLE_INFO2("UpdateThread 出現異常!");
			}
		} while (true);
		return 0;
	}

	void ObjectManager::CacheObjects()
	{
		if (Vars.Menu.bNeedReFresh)
		{
			CONSOLE_INFO2(u8"刷新全部数据!");
			auto cachedObjectsNew = ObjectManager::GetObjects();
			if (!cachedObjectsNew.empty())
			{
				cachedObjectsNew.clear();
				cachedObjectsCopy.clear();
				ObjectManager::SetObjects(std::unordered_map<ptr_t, std::shared_ptr<AActor>>());
			}
			Sleep(1000);
			Vars.Menu.bNeedReFresh = false;
			return;
		}
		auto ScatterHandle = sdk::GetHandle2();
		if (sdk::InGame())
		{
			auto AActors = UWorld(sdk::GetWorld())->PersistentLevel->AActors.Get();
			// Copy a cast to prevent it from being altered
			int actorsNum = AActors->Num();
			if (actorsNum > 20000 || actorsNum <= 0)
				return;

			std::vector<ptr_t> actorsCopy(actorsNum);
			if (!GetDMA().Read((ptr_t)AActors->GetData(), (ptr_t)actorsCopy.data(), actorsNum * 8))
			{
				for (int i = 0; i < actorsNum; i++)
				{
					GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)AActors->GetData() + i * 8, &actorsCopy[i], 8);
				}
				GetDMA().executeScatterRead(ScatterHandle);
			}
			auto CmpEmpty = VirtualAlloc(nullptr, actorsCopy.size() * 8, MEM_COMMIT, PAGE_READWRITE);
			if (!CmpEmpty)
				return;
			if (memcmp(actorsCopy.data(), CmpEmpty, actorsCopy.size() * 8) == 0)
			{
				actorsCopy.clear();
				VirtualFree(CmpEmpty, 0, MEM_RELEASE); // 正确使用MEM_RELEASE
				return;
			}
			VirtualFree(CmpEmpty, 0, MEM_RELEASE); // 正确使用MEM_RELEASE

			cachedObjectsCopy = ObjectManager::GetObjects();
			if (cachedObjectsCopy.size() > 100000 || cachedObjectsCopy.size() < 0)
			{
				cachedObjectsCopy.clear();
				ObjectManager::SetObjects(std::unordered_map<ptr_t, std::shared_ptr<AActor>>());
				return;
			}
			// Remove cached object
			for (auto cacheIter = cachedObjectsCopy.begin(); cacheIter != cachedObjectsCopy.end(); )
			{
				auto memoryAddress = reinterpret_cast<ptr_t>((*cacheIter->second)->GetPtr());
				auto actorIter = actorsCopy.begin();
				if (actorsCopy.empty())
					cacheIter = cachedObjectsCopy.erase(cacheIter);
				while (actorIter != actorsCopy.end())
				{
					// Remove existing actors
					if (memoryAddress == *actorIter)
					{
						cacheIter++;
						actorsCopy.erase(actorIter);
						break;
					}
					// Not found!
					if (++actorIter == actorsCopy.end())
					{
						//CONSOLE_INFO("Release A Object:%p,%d", (*cacheIter->second)->GetPtr(), (*cacheIter->second)->GetObjectType());
						(*cacheIter->second)->Release();
						cacheIter = cachedObjectsCopy.erase(cacheIter);
					}
				}
			}

			std::unique_ptr <std::vector<std::unique_ptr<std::string>>> tempObjectsName(new std::vector<std::unique_ptr<std::string>>());
			std::unique_ptr < std::vector<FName>> tempObjectsFName(new std::vector<FName>());
			std::unique_ptr < std::vector<ptr_t>> tempObjectsClass(new std::vector<ptr_t>());
			std::unique_ptr < std::vector<OMFClassBaseChain>> tempClassChain(new std::vector<OMFClassBaseChain>);
			std::unique_ptr < std::unordered_map <UObjectType, std::vector<MapClassBaseChain>>>TypeArray(new std::unordered_map <UObjectType, std::vector<MapClassBaseChain>>());
			if (actorsCopy.empty())
				goto noUpdate;
			auto LoopSize = actorsCopy.size();
			tempObjectsName->resize(LoopSize);
			tempObjectsFName->resize(LoopSize);
			tempObjectsClass->resize(LoopSize);
			tempClassChain->resize(LoopSize);
			// Add cached object
			for (int i = 0; i < LoopSize; i++)
			{
				if (!IsAddrValid(actorsCopy[i]))
					continue;
				GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)actorsCopy[i] + g_PatternData.offset_UObjectClass, &(*tempObjectsClass)[i], 8);
				GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)actorsCopy[i] + g_PatternData.offset_UObjectNameComparisonIndex, &(*tempObjectsFName)[i], 4);
			}
			GetDMA().executeScatterRead(ScatterHandle);
			for (int i = 0; i < LoopSize; i++)
			{
				if (!IsAddrValid(actorsCopy[i]))
					continue;
				(*tempObjectsClass)[i] = UEEncryptedObjectProperty<ptr_t, DecryptFunc::UObjectClass>::STATIC_Get((*tempObjectsClass)[i]);
				if (!IsAddrValid((*tempObjectsClass)[i]))
					continue;
				(*tempObjectsFName)[i].ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get((*tempObjectsFName)[i].ComparisonIndex);
				(*tempObjectsFName)[i].Number = 0;
				if ((*tempObjectsFName)[i].ComparisonIndex)
				{
					(*tempObjectsName)[i].reset(new std::string((*tempObjectsFName)[i].GetName2()));
					if (!Utils::StringIsValid(*(*tempObjectsName)[i]))
						(*tempObjectsName)[i]->clear();
				}
				GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)(*tempObjectsClass)[i] + g_PatternData.offset_UClassBaseChainArray, &(*tempClassChain)[i], sizeof(OMFClassBaseChain));
			}
			GetDMA().executeScatterRead(ScatterHandle);
			TypeArray->emplace(EPlayer, std::vector<MapClassBaseChain>());
			(*TypeArray)[EPlayer].resize(LoopSize);
			TypeArray->emplace(EVehicle, std::vector<MapClassBaseChain>());
			(*TypeArray)[EVehicle].resize(LoopSize);
			TypeArray->emplace(EBoat, std::vector<MapClassBaseChain>());
			(*TypeArray)[EBoat].resize(LoopSize);
			TypeArray->emplace(EItemGroup, std::vector<MapClassBaseChain>());
			(*TypeArray)[EItemGroup].resize(LoopSize);
			TypeArray->emplace(EItem, std::vector<MapClassBaseChain>());
			(*TypeArray)[EItem].resize(LoopSize);
			TypeArray->emplace(EDeathBox, std::vector<MapClassBaseChain>());
			(*TypeArray)[EDeathBox].resize(LoopSize);
			TypeArray->emplace(EAirdrop, std::vector<MapClassBaseChain>());
			(*TypeArray)[EAirdrop].resize(LoopSize);
			TypeArray->emplace(EProjectile, std::vector<MapClassBaseChain>());
			(*TypeArray)[EProjectile].resize(LoopSize);
			for (int i = 0; i < LoopSize; i++)
			{
				if (!IsAddrValid(actorsCopy[i]))
					continue;
				(*TypeArray)[EPlayer][i] = (MapClassBaseChain(actorsCopy[i], (*tempClassChain)[i], 0));
				int32_t NumParentClassBasesInChainMinusOne = EPlayerUClass.NumClassBasesInChainMinusOne;
				GetDMA().queueScatterReadEx(ScatterHandle, (*TypeArray)[EPlayer][i].Chian.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
					&(*TypeArray)[EPlayer][i].Value, 8);

				(*TypeArray)[EVehicle][i] = (MapClassBaseChain(actorsCopy[i], (*tempClassChain)[i], 0));
				NumParentClassBasesInChainMinusOne = EVehicleUClass.NumClassBasesInChainMinusOne;
				GetDMA().queueScatterReadEx(ScatterHandle, (*TypeArray)[EVehicle][i].Chian.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
					&(*TypeArray)[EVehicle][i].Value, 8);

				(*TypeArray)[EBoat][i] = (MapClassBaseChain(actorsCopy[i], (*tempClassChain)[i], 0));
				NumParentClassBasesInChainMinusOne = EBoatUClass.NumClassBasesInChainMinusOne;
				GetDMA().queueScatterReadEx(ScatterHandle, (*TypeArray)[EBoat][i].Chian.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
					&(*TypeArray)[EBoat][i].Value, 8);

				(*TypeArray)[EItemGroup][i] = (MapClassBaseChain(actorsCopy[i], (*tempClassChain)[i], 0));
				NumParentClassBasesInChainMinusOne = EItemGroupUClass.NumClassBasesInChainMinusOne;
				GetDMA().queueScatterReadEx(ScatterHandle, (*TypeArray)[EItemGroup][i].Chian.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
					&(*TypeArray)[EItemGroup][i].Value, 8);

				(*TypeArray)[EItem][i] = (MapClassBaseChain(actorsCopy[i], (*tempClassChain)[i], 0));
				NumParentClassBasesInChainMinusOne = EItemUClass.NumClassBasesInChainMinusOne;
				GetDMA().queueScatterReadEx(ScatterHandle, (*TypeArray)[EItem][i].Chian.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
					&(*TypeArray)[EItem][i].Value, 8);

				(*TypeArray)[EDeathBox][i] = (MapClassBaseChain(actorsCopy[i], (*tempClassChain)[i], 0));
				NumParentClassBasesInChainMinusOne = EDeathBoxUClass.NumClassBasesInChainMinusOne;
				GetDMA().queueScatterReadEx(ScatterHandle, (*TypeArray)[EDeathBox][i].Chian.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
					&(*TypeArray)[EDeathBox][i].Value, 8);

				(*TypeArray)[EAirdrop][i] = (MapClassBaseChain(actorsCopy[i], (*tempClassChain)[i], 0));
				NumParentClassBasesInChainMinusOne = EAirdropUClass.NumClassBasesInChainMinusOne;
				GetDMA().queueScatterReadEx(ScatterHandle, (*TypeArray)[EAirdrop][i].Chian.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
					&(*TypeArray)[EAirdrop][i].Value, 8);

				(*TypeArray)[EProjectile][i] = (MapClassBaseChain(actorsCopy[i], (*tempClassChain)[i], 0));
				NumParentClassBasesInChainMinusOne = EProjectileUClass.NumClassBasesInChainMinusOne;
				GetDMA().queueScatterReadEx(ScatterHandle, (*TypeArray)[EProjectile][i].Chian.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
					&(*TypeArray)[EProjectile][i].Value, 8);
			}
			GetDMA().executeScatterRead(ScatterHandle);

			for (int i = 0; i < LoopSize; i++)
			{
				UObjectType Type = EOther;
				AActor actor = actorsCopy[i];
				if (!IsAddrValid(actorsCopy[i]))
					continue;

				OMFClassBaseChain PlayerBaseChain = EPlayerUClass;
				OMFClassBaseChain VehicleBaseChain = EVehicleUClass;
				OMFClassBaseChain BoatBaseChain = EBoatUClass;
				OMFClassBaseChain ItemGroupBaseChain = EItemGroupUClass;
				OMFClassBaseChain ItemBaseChain = EItemUClass;
				OMFClassBaseChain DeathBoxChain = EDeathBoxUClass;
				OMFClassBaseChain AirdropChain = EAirdropUClass;
				OMFClassBaseChain ProjectileChain = EProjectileUClass;

				int32_t NumParentClassBasesInChainMinusOne = PlayerBaseChain.NumClassBasesInChainMinusOne;
				if (NumParentClassBasesInChainMinusOne <= (*TypeArray)[EPlayer][i].Chian.NumClassBasesInChainMinusOne &&
					(*TypeArray)[EPlayer][i].Value == PlayerBaseChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
				{
					Type = EPlayer;
					goto bCreate;
				}
				NumParentClassBasesInChainMinusOne = VehicleBaseChain.NumClassBasesInChainMinusOne;
				if (NumParentClassBasesInChainMinusOne <= (*TypeArray)[EVehicle][i].Chian.NumClassBasesInChainMinusOne &&
					(*TypeArray)[EVehicle][i].Value == VehicleBaseChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
				{
					Type = EVehicle;
					goto bCreate;
				}
				NumParentClassBasesInChainMinusOne = BoatBaseChain.NumClassBasesInChainMinusOne;
				if (NumParentClassBasesInChainMinusOne <= (*TypeArray)[EBoat][i].Chian.NumClassBasesInChainMinusOne &&
					(*TypeArray)[EBoat][i].Value == BoatBaseChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
				{
					Type = EBoat;
					goto bCreate;

				}
				NumParentClassBasesInChainMinusOne = ItemGroupBaseChain.NumClassBasesInChainMinusOne;
				if (NumParentClassBasesInChainMinusOne <= (*TypeArray)[EItemGroup][i].Chian.NumClassBasesInChainMinusOne &&
					(*TypeArray)[EItemGroup][i].Value == ItemGroupBaseChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
				{
					Type = EItemGroup;
					goto bCreate;
				}
				NumParentClassBasesInChainMinusOne = ItemBaseChain.NumClassBasesInChainMinusOne;
				if (NumParentClassBasesInChainMinusOne <= (*TypeArray)[EItem][i].Chian.NumClassBasesInChainMinusOne &&
					(*TypeArray)[EItem][i].Value == ItemBaseChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
				{
					Type = EItem;
					goto bCreate;
				}
				NumParentClassBasesInChainMinusOne = DeathBoxChain.NumClassBasesInChainMinusOne;
				if (NumParentClassBasesInChainMinusOne <= (*TypeArray)[EDeathBox][i].Chian.NumClassBasesInChainMinusOne &&
					(*TypeArray)[EDeathBox][i].Value == DeathBoxChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
				{
					Type = EDeathBox;
					goto bCreate;
				}
				NumParentClassBasesInChainMinusOne = AirdropChain.NumClassBasesInChainMinusOne;
				if (NumParentClassBasesInChainMinusOne <= (*TypeArray)[EAirdrop][i].Chian.NumClassBasesInChainMinusOne &&
					(*TypeArray)[EAirdrop][i].Value == AirdropChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
				{
					Type = EAirdrop;
					goto bCreate;
				}
				NumParentClassBasesInChainMinusOne = ProjectileChain.NumClassBasesInChainMinusOne;
				if (NumParentClassBasesInChainMinusOne <= (*TypeArray)[EProjectile][i].Chian.NumClassBasesInChainMinusOne &&
					(*TypeArray)[EProjectile][i].Value == ProjectileChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
				{
					Type = EProjectile;
					goto bCreate;
				}
				if (tempObjectsName && (*tempObjectsName)[i])
				{
					switch (hash_(*(*tempObjectsName)[i]))
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
						Type = EPlayer;
						break;
					}
					default:
						break;
					}
				}

			bCreate:
				auto managedObject = PrepareCreateObjectPointerFromObject(actor, Type);
				if (!managedObject)
					continue;
				cachedObjectsCopy.emplace((ptr_t)managedObject->GetPtr(), std::shared_ptr<AActor>(managedObject));
			}
		noUpdate:
			std::vector<ptr_t>EraseMap;
			for (auto iter : cachedObjectsCopy)
			{
				if (IsAddrValid(iter.first))
				{
					auto ObjectType = iter.second->GetObjectType();
					switch (ObjectType)
					{
					case IronMan::Core::SDK::EPlayer:
					{
						auto player = dynamic_cast<ATslCharacter*>(iter.second.get());
						if (!player || !player->IsValid())
							EraseMap.push_back(iter.first);
						break;
					}
					case IronMan::Core::SDK::EVehicle:
					{
						auto vehicle = dynamic_cast<ATslWheeledVehicle*>(iter.second.get());
						if (!vehicle || !vehicle->IsValid())
							EraseMap.push_back(iter.first);
						break;
					}
					case IronMan::Core::SDK::EBoat:
					{
						auto boat = dynamic_cast<ATslFloatingVehicle*>(iter.second.get());
						if (!boat || !boat->IsValid())
							EraseMap.push_back(iter.first);
						break;
					}
					case IronMan::Core::SDK::EItemGroup:
					{
						auto itemGroup = dynamic_cast<ADroppedItemGroup*>(iter.second.get());
						if (!itemGroup || !itemGroup->IsValid())
							EraseMap.push_back(iter.first);
						break;
					}
					case IronMan::Core::SDK::EItem:
					{
						auto item = dynamic_cast<ADroppedItem*>(iter.second.get());
						if (!item || !item->IsValid())
							EraseMap.push_back(iter.first);
						break;
					}
					case IronMan::Core::SDK::EDeathBox:
					{
						auto deadbox = dynamic_cast<AFloorSnapItemPackage*>(iter.second.get());
						if (!deadbox || !deadbox->IsValid())
							EraseMap.push_back(iter.first);
						break;
					}
					case IronMan::Core::SDK::EAirdrop:
					{
						auto airdrop = dynamic_cast<ACarePackageItem*>(iter.second.get());
						if (!airdrop || !airdrop->IsValid())
							EraseMap.push_back(iter.first);
						break;
					}
					case IronMan::Core::SDK::EProjectile:
					{
						auto proj = dynamic_cast<ATslProjectile*>(iter.second.get());
						if (!proj || !proj->IsValid())
							EraseMap.push_back(iter.first);
						break;
					}
					case IronMan::Core::SDK::EDroppedItem:
					case IronMan::Core::SDK::EOther:
					case IronMan::Core::SDK::EUnknown:
					{
						auto Aactor = dynamic_cast<AActor*>(iter.second.get());
						if (!Aactor || !Aactor->IsValid())
							EraseMap.push_back(iter.first);
						break;
					}
					default:
						break;
					}
				}
				else
				{
					EraseMap.push_back(iter.first);
				}
			}
			if (EraseMap.size())
			{
				for (int i = 0; i < EraseMap.size(); i++)
				{
					CONSOLE_INFO("Erase:%p", EraseMap[i]);
					auto FindResult = cachedObjectsCopy.find(EraseMap[i]);
					if (FindResult != cachedObjectsCopy.end())
						cachedObjectsCopy.erase(FindResult);
				}
			}
			SetObjects(cachedObjectsCopy);
			if (cachedObjectsCopy.size())
				cachedObjectsCopy.clear();
		}
		else
		{
			auto cachedObjectsNew = ObjectManager::GetObjects();
			if (!cachedObjectsNew.empty())
			{
				cachedObjectsNew.clear();
				cachedObjectsCopy.clear();
				ObjectManager::SetObjects(std::unordered_map<ptr_t, std::shared_ptr<AActor>>());
			}
		}
		return;
	}

	void ObjectManager::GameUpdate()
	{
		auto ScatterHandle = sdk::GetHandle2();
		if (!ScatterHandle)
			return;
		static FName t_FiringAttachPoint = FName();
		static FName t_ScopingAttachPoint = FName();
		static void* m_FiringAttachPoint_Socket = nullptr;
		static void* m_ScopingAttachPoint_Socket = nullptr;
		static ptr_t t_LocalEquippedWeapons = 0;
		static BYTE t_WeaponIndex = 0;

		bool t_bIsCanted = false;
		float t_CustomTimeDilation = 0.f;
		float t_TrajectoryGravityZ = 0.f;
		int t_CurrentZeroLevel = 0;

		std::unique_ptr<std::vector<Inventory_Equipment>> t_InventoryEquipment(new std::vector<Inventory_Equipment>());
		t_InventoryEquipment->resize(3);
		TArray<FRichCurveKey> t_FloatCurvesKey1 = TArray<FRichCurveKey>();
		TArray<FRichCurveKey> t_FloatCurvesKey2 = TArray<FRichCurveKey>();
		TArray<FRichCurveKey> t_FloatCurvesKey3 = TArray<FRichCurveKey>();
		std::vector<std::vector<FRichCurveKey>> t_CurvesKeysArray;
		t_CurvesKeysArray.resize(3);
		std::vector<FEuqimentReplicatedSkinItem>  t_HumanBodyRepliedSkin;
		t_HumanBodyRepliedSkin.resize(50);
		ptr_t t_LocalAnimScriptInstance = 0;

		{
			auto& MyHudObj = sdk::GetMyHUD();
			if (IsAddrValid(MyHudObj))
			{
				GetDMA().queueScatterReadEx(ScatterHandle, MyHudObj + g_PatternData.offset_AHUD_WidgetStateMap, &pWidgetStateMap, 8);
				if (pWidgetStateMap > 0x1000)
				{
					if (!offsetOfCanvasWidget)
					{
						std::vector<void*>WidgetStateMapList;
						WidgetStateMapList.resize(2000);
						std::vector<FName>WidgetStateMapFNameList;
						WidgetStateMapFNameList.resize(2000);
						for (int i = 0; i < WidgetStateMapList.size(); i++)
						{
							GetDMA().queueScatterReadEx(ScatterHandle, pWidgetStateMap + i * 8, &WidgetStateMapList[i], 8);
						}
						GetDMA().executeScatterRead(ScatterHandle);

						for (int i = 0; i < WidgetStateMapList.size(); i++)
						{
							if (IsAddrValid(WidgetStateMapList[i]))
								GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)WidgetStateMapList[i] + g_PatternData.offset_UObjectNameComparisonIndex, &WidgetStateMapFNameList[i], 4);
							else
								WidgetStateMapFNameList[i] = FName(0);
						}
						GetDMA().executeScatterRead(ScatterHandle);

						for (int i = 0; i < WidgetStateMapFNameList.size(); i++)
						{
							WidgetStateMapFNameList[i].ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(WidgetStateMapFNameList[i].ComparisonIndex);
						}

						for (int i = 0; i < WidgetStateMapList.size(); i++)
						{
							std::unique_ptr<UObject> Widget(new UObject(WidgetStateMapList[i]));
							if (IsAddrValid(WidgetStateMapList[i]))
							{
								std::unique_ptr<std::string> WidgetStateMapName = std::make_unique<std::string>(WidgetStateMapFNameList[i].GetName2());
								if (hash_(WidgetStateMapName->c_str()) == "MinimapOriginalType_C"_hash || hash_(WidgetStateMapName->c_str()) == "BP_FBRMinimapOriginalType_C"_hash)
								{
									UMinimapCanvasWidget CanvasWidget = Widget->SafeCast<UMinimapCanvasWidget>();
									if (!CanvasWidget || !CanvasWidget->IsValid())
										continue;
									auto t_SelectMinimapSizeIndex = CanvasWidget->SelectMinimapSizeIndex;
									if (t_SelectMinimapSizeIndex > 1 || t_SelectMinimapSizeIndex < 0)
										continue;
									auto MinimapSizeOffset = CanvasWidget->MinimapSizeOffset;
									if (MinimapSizeOffset <= 0.f || MinimapSizeOffset >= 20.f)
										continue;
									sdk::SetSelectMinimapSizeIndex(t_SelectMinimapSizeIndex);
									sdk::SetLocalSelectMinimapSizeIndex((ptr_t)Widget->GetPtr() + g_PatternData.offset_UMinimapCanvasWidget_SelectMinimapSizeIndex);
									offsetOfCanvasWidget = i;
									pCanvasWidget = Widget->GetPtr();
									CONSOLE_INFO("MiniMapRader Offset Change to %d", offsetOfCanvasWidget);
									break;
								}
							}
						}
					}
					else
					{
						GetDMA().queueScatterReadEx(ScatterHandle, pWidgetStateMap + offsetOfCanvasWidget * 8, &pCanvasWidget, 8);
					}
					std::unique_ptr <UObject> Widget(new UObject(pCanvasWidget));
					if (IsAddrValid(Widget->GetPtr()))
					{
						GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)Widget->GetPtr() + g_PatternData.offset_UObjectNameComparisonIndex, &EncryptCanvasWidgetID, 4);
						if (EncryptCanvasWidgetID != 0)
						{
							FName CanvasWidgetFName;
							CanvasWidgetFName.ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(EncryptCanvasWidgetID);
							std::string CanvasWidgetName = CanvasWidgetFName.GetName2();
							if (CanvasWidgetName.find("MinimapOriginalType_C") != std::string::npos)
							{
								UMinimapCanvasWidget CanvasWidget = Widget->Cast<UMinimapCanvasWidget>();
								sdk::SetLocalSelectMinimapSizeIndex((ptr_t)Widget->GetPtr() + g_PatternData.offset_UMinimapCanvasWidget_SelectMinimapSizeIndex);
								GetDMA().queueScatterReadEx(ScatterHandle, CanvasWidget->SGetMinimapSizeOffset(), &pMinimapSizeOffset, 4);
								GetDMA().queueScatterReadEx(ScatterHandle, CanvasWidget->SGetSelectMinimapSizeIndex(), &pSelectMinimapSizeIndex, 4);
								GetDMA().executeScatterRead(ScatterHandle);
								//CONSOLE_INFO("%f", pMinimapSizeOffset);
								if (pSelectMinimapSizeIndex > 1 || pSelectMinimapSizeIndex < 0 || pMinimapSizeOffset <= 0.f || pMinimapSizeOffset >= 20.f)
								{
									CONSOLE_INFO("MiniMapRader Invalid %d", offsetOfCanvasWidget);
									offsetOfCanvasWidget = 0;
								}
							}
							else
							{
								pSelectMinimapSizeIndex = 0;
								pMinimapSizeOffset = 0;
								offsetOfCanvasWidget = 0;
								sdk::SetLocalSelectMinimapSizeIndex(0);
							}
						}
						else
						{
							pSelectMinimapSizeIndex = 0;
							pMinimapSizeOffset = 0;
							offsetOfCanvasWidget = 0;
							sdk::SetLocalSelectMinimapSizeIndex(0);
						}
					}
					else
					{
						EncryptCanvasWidgetID = 0;
						offsetOfCanvasWidget = 0;
						sdk::SetLocalSelectMinimapSizeIndex(0);
					}
				}
			}
			else
			{
				sdk::SetLocalSelectMinimapSizeIndex(0);
				pSelectMinimapSizeIndex = 0;
				pMinimapSizeOffset = 0;
				EncryptCanvasWidgetID = 0;
				pCanvasWidget = 0;
				pWidgetStateMap = 0;
			}
			auto& player = sdk::GetPlayer();
			if (IsAddrValid(player))
			{
				GetDMA().queueScatterReadEx(ScatterHandle, player + g_PatternData.offset_Actor_RootComponent, &DecryptRootComponent, 8);
				GetDMA().queueScatterReadEx(ScatterHandle, player + g_PatternData.offset_ATslCharacter_InventoryFacade, &DecryptInventoryFacede, 8);
				GetDMA().queueScatterReadEx(ScatterHandle, player + g_PatternData.offset_ACharacter_Mesh, &m_LocalMesh, 8);
				GetDMA().queueScatterReadEx(ScatterHandle, player + g_PatternData.offset_ATslCharacterBase_WeaponProcessor, &m_LocalWeaponPorcess, 8);
				if (DecryptRootComponent)
					sdk::SetLocalRoot(UEEncryptedObjectProperty<ptr_t, DecryptFunc::General>::STATIC_Get(DecryptRootComponent));
				else
					sdk::SetLocalRoot(0);

				std::unique_ptr <USkeletalMeshComponent> mesh(new USkeletalMeshComponent(m_LocalMesh));
				if (IsAddrValid(m_LocalMesh))
				{
					sdk::SetMesh(mesh->GetPtr());
					GetDMA().queueScatterReadEx(ScatterHandle, mesh->SGetAnimScriptInstance(), &t_LocalAnimScriptInstance, 8);
				}
				else
				{
					sdk::SetMesh(nullptr);
				}

				std::unique_ptr <UWeaponProcessorComponent> WeaponProcessor(new UWeaponProcessorComponent(m_LocalWeaponPorcess));
				if (WeaponProcessor->IsValid() && IsAddrValid(m_LocalWeaponPorcess))
				{
					GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)WeaponProcessor->GetPtr() + g_PatternData.offset_UWeaponProcessorComponent_WeaponIndex + 1, &t_WeaponIndex, 1);
					GetDMA().queueScatterReadEx(ScatterHandle, WeaponProcessor->SGetEquippedWeapons(), &t_LocalEquippedWeapons, 8);
					if (IsAddrValid(t_LocalEquippedWeapons))
					{
						GetDMA().queueScatterReadEx(ScatterHandle, t_LocalEquippedWeapons + t_WeaponIndex * 8, &CurrentWeapon, 8);
						GetDMA().queueScatterReadEx(ScatterHandle, t_LocalEquippedWeapons, &CurrentWeapon1, 8);
						GetDMA().queueScatterReadEx(ScatterHandle, t_LocalEquippedWeapons + 8, &CurrentWeapon2, 8);
						if (IsAddrValid(CurrentWeapon))
						{
							sdk::SetWeapon((void*)CurrentWeapon);
							std::unique_ptr <ATslWeapon_Trajectory> Weapon(new ATslWeapon_Trajectory(CurrentWeapon));
							if (IsAddrValid(CurrentWeapon))
							{

								GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)Weapon->GetPtr() + g_PatternData.offset_UObjectNameComparisonIndex, &sdk::m_LocalWeaponEncryptIndex, 4);
								GetDMA().queueScatterReadEx(ScatterHandle, Weapon->SGetbIsCanted(), &t_bIsCanted, 1);
								GetDMA().queueScatterReadEx(ScatterHandle, Weapon->SGetCurrentZeroLevel(), &t_CurrentZeroLevel, 4);
								GetDMA().queueScatterReadEx(ScatterHandle, Weapon->SGetCustomTimeDilation(), &t_CustomTimeDilation, 4);
								GetDMA().queueScatterReadEx(ScatterHandle, Weapon->SGetTrajectoryGravityZ(), &t_TrajectoryGravityZ, 4);
								GetDMA().queueScatterReadEx(ScatterHandle, Weapon->SGetAttachedItem(), &pAttachedItem, sizeof(pAttachedItem));
								GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)Weapon->GetPtr() + g_PatternData.offset_ATslWeapon_Mesh3P, &pMesh3P, 8);
								GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)Weapon->GetPtr() + g_PatternData.offset_ATslWeapon_Trajectory_WeaponTrajectoryData, &pWeaponTrajectoryData, 8);
								GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)Weapon->SGetFiringAttachPoint(), &t_FiringAttachPoint, 4);
								GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)Weapon->SGetScopingAttachPoint(), &t_ScopingAttachPoint, 4);
								if (IsAddrValid(pWeaponTrajectoryData))
								{
									GetDMA().queueScatterReadEx(ScatterHandle, pWeaponTrajectoryData + g_PatternData.offset_WeaponTrajectoryData_TrajectoryConfig, &sdk::m_WeaponTrajectoryConfig, sizeof(FWeaponTrajectoryConfig));
									UCurveVector ballisticCurve(sdk::m_WeaponTrajectoryConfig.BallisticCurve);
									if (ballisticCurve->IsValid() && IsAddrValid(sdk::m_WeaponTrajectoryConfig.BallisticCurve))
									{
										GetDMA().queueScatterReadEx(ScatterHandle, ballisticCurve->SGetFloatCurves(), &sdk::m_FloatCurves, sizeof(FRichCurve_3));
										t_FloatCurvesKey1 = sdk::m_FloatCurves.FloatCurves[0].Keys;
										t_FloatCurvesKey2 = sdk::m_FloatCurves.FloatCurves[1].Keys;
										t_FloatCurvesKey3 = sdk::m_FloatCurves.FloatCurves[2].Keys;
										//1
										{
											if (t_FloatCurvesKey1.Num() > 0 && t_FloatCurvesKey1.Num() < 100 && t_FloatCurvesKey1.Data)
											{
												if (t_CurvesKeysArray[0].size() != t_FloatCurvesKey1.Num())
													t_CurvesKeysArray[0].resize(t_FloatCurvesKey1.Num());
												GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)t_FloatCurvesKey1->GetData(), t_CurvesKeysArray[0].data(), sizeof(FRichCurveKey) * t_FloatCurvesKey1.Num());
											}
											else if (!t_FloatCurvesKey1.Data)
											{
												if (t_CurvesKeysArray[0].size())
													t_CurvesKeysArray[0].clear();
											}
										}
										//2
										{
											if (t_FloatCurvesKey2.Num() > 0 && t_FloatCurvesKey2.Num() < 100 && t_FloatCurvesKey2.Data)
											{
												if (t_CurvesKeysArray[1].size() != t_FloatCurvesKey2.Num())
													t_CurvesKeysArray[1].resize(t_FloatCurvesKey2.Num());
												GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)t_FloatCurvesKey2->GetData(), t_CurvesKeysArray[1].data(), sizeof(FRichCurveKey) * t_FloatCurvesKey2.Num());
											}
											else if (!t_FloatCurvesKey2.Data)
											{
												if (t_CurvesKeysArray[1].size())
													t_CurvesKeysArray[1].clear();
											}
										}
										//3
										{
											if (t_FloatCurvesKey3.Num() > 0 && t_FloatCurvesKey3.Num() < 100 && t_FloatCurvesKey3.Data)
											{
												if (t_CurvesKeysArray[2].size() != t_FloatCurvesKey3.Num())
													t_CurvesKeysArray[2].resize(t_FloatCurvesKey3.Num());
												GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)t_FloatCurvesKey3->GetData(), t_CurvesKeysArray[2].data(), sizeof(FRichCurveKey) * t_FloatCurvesKey3.Num());
											}
											else if (!t_FloatCurvesKey3.Data)
											{
												if (t_CurvesKeysArray[2].size())
													t_CurvesKeysArray[2].clear();
											}
										}
									}
								}
								else
								{
									ZeroMemory(&sdk::m_WeaponTrajectoryConfig, sizeof(sdk::m_WeaponTrajectoryConfig));
								}

								if (IsAddrValid(pAttachedItem->GetData()) && pAttachedItem.Num() > 0 && pAttachedItem.Num() < 100)
								{
									if (pAttachedItemSaveArray.size() != pAttachedItem.Num())
										pAttachedItemSaveArray.resize(pAttachedItem.Num());
									GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)pAttachedItem->GetData(), pAttachedItemSaveArray.data(), pAttachedItem.Num() * 8);
									if (pAttachedItemSaveArray.size())
									{
										if (pAttachedItemTableRowSaveArray.size() != pAttachedItemSaveArray.size())
											pAttachedItemTableRowSaveArray.resize(pAttachedItemSaveArray.size());
										for (uint32_t i = 0; i < pAttachedItemSaveArray.size(); i++)
										{
											auto AttachableItem = UAttachableItem(pAttachedItemSaveArray[i]);
											if (!IsAddrValid(pAttachedItemSaveArray[i]))
											{
												pAttachedItemTableRowSaveArray[i] = 0;
												continue;
											}
											GetDMA().queueScatterReadEx(ScatterHandle, AttachableItem->SGetItemTableRowPtr(), &pAttachedItemTableRowSaveArray[i], 8);
										}
										if (pAttachedItemTableRowSaveArray.size())
										{
											if (FNameArray.size() != pAttachedItemTableRowSaveArray.size())
												FNameArray.resize(pAttachedItemTableRowSaveArray.size());
											if (ItemFTextNameArray.size() != pAttachedItemTableRowSaveArray.size())
												ItemFTextNameArray.resize(pAttachedItemTableRowSaveArray.size());
											if (ItemFTextCategoryArray.size() != pAttachedItemTableRowSaveArray.size())
												ItemFTextCategoryArray.resize(pAttachedItemTableRowSaveArray.size());
											if (ItemNameDataArray.size() != pAttachedItemTableRowSaveArray.size())
												ItemNameDataArray.resize(pAttachedItemTableRowSaveArray.size());
											if (ItemCategoryDataArray.size() != pAttachedItemTableRowSaveArray.size())
												ItemCategoryDataArray.resize(pAttachedItemTableRowSaveArray.size());
											if (ItemWNameArray.size() != pAttachedItemTableRowSaveArray.size())
												ItemWNameArray.resize(pAttachedItemTableRowSaveArray.size());
											if (ItemWCategoryArray.size() != pAttachedItemTableRowSaveArray.size())
												ItemWCategoryArray.resize(pAttachedItemTableRowSaveArray.size());
											if (ItemNameArray.size() != pAttachedItemTableRowSaveArray.size())
												ItemNameArray.resize(pAttachedItemTableRowSaveArray.size());
											if (ItemCategoryArray.size() != pAttachedItemTableRowSaveArray.size())
												ItemCategoryArray.resize(pAttachedItemTableRowSaveArray.size());

											for (uint32_t i = 0; i < pAttachedItemTableRowSaveArray.size(); i++)
											{
												auto ItemTableRow = ptr_t(pAttachedItemTableRowSaveArray[i]);
												if (!IsAddrValid(ItemTableRow))
												{
													ZeroMemory(&ItemFTextNameArray[i], sizeof(ItemFTextNameArray[i]));
													ZeroMemory(&ItemFTextCategoryArray[i], sizeof(ItemFTextCategoryArray[i]));
													ZeroMemory(&ItemNameDataArray[i], sizeof(ItemNameDataArray[i]));
													ZeroMemory(&ItemCategoryDataArray[i], sizeof(ItemCategoryDataArray[i]));
													ItemWNameArray[i] = L"";
													ItemWCategoryArray[i] = L"";
													ItemNameArray[i] = "";
													ItemCategoryArray[i] = "";
													continue;
												}
												GetDMA().queueScatterReadEx(ScatterHandle, ItemTableRow + g_PatternData.offset_ItemTableRowBase_ItemID, &FNameArray[i], 4);
												GetDMA().queueScatterReadEx(ScatterHandle, ItemTableRow + g_PatternData.offset_ItemTableRowBase_ItemName, &ItemFTextNameArray[i], sizeof(FText));
												GetDMA().queueScatterReadEx(ScatterHandle, ItemTableRow + g_PatternData.offset_ItemTableRowBase_ItemCategory, &ItemFTextCategoryArray[i], sizeof(FText));
											}
											for (uint32_t i = 0; i < pAttachedItemTableRowSaveArray.size(); i++)
											{
												if (!IsAddrValid(ItemFTextNameArray[i].Data20) || !IsAddrValid(ItemFTextCategoryArray[i].Data20))
													continue;
												GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)ItemFTextNameArray[i].Data20, &ItemNameDataArray[i], sizeof(FTextstrdata));
												GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)ItemFTextCategoryArray[i].Data20, &ItemCategoryDataArray[i], sizeof(FTextstrdata));
											}
											for (uint32_t i = 0; i < pAttachedItemTableRowSaveArray.size(); i++)
											{
												if ((!IsAddrValid(ItemNameDataArray[i].Data20.Name) && !IsAddrValid(ItemNameDataArray[i].Data28.Name))
													|| (!IsAddrValid(ItemCategoryDataArray[i].Data20.Name)) && !IsAddrValid(ItemCategoryDataArray[i].Data28.Name))
													continue;

												wchar_t* name1 = ItemNameDataArray[i].Data20.Name;
												auto length1 = ItemNameDataArray[i].Data20.Length;
												auto unk1 = ItemNameDataArray[i].Data20.unknown;
												if (length1 <= 0 || length1 > 255 || unk1 <= 0 || unk1 >= 255)
												{
													name1 = ItemNameDataArray[i].Data28.Name;
													length1 = ItemNameDataArray[i].Data28.Length;
												}
												if (name1 && length1 <= 255)
												{
													if (ItemWNameArray[i].size() != length1 * 2 + 1)
														ItemWNameArray[i].resize(length1 * 2 + 1);
													GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)name1, ItemWNameArray[i].data(), length1 * 2);
													if (*(int*)ItemWNameArray[i].data() != 0)
													{
														std::string ItemName = Utils::UnicodeToUTF8(ItemWNameArray[i].data());
														if (ItemName != "")
														{
															ItemNameArray[i] = ItemName;
														}
													}
												}

												wchar_t* name2 = ItemCategoryDataArray[i].Data20.Name;
												auto length2 = ItemCategoryDataArray[i].Data20.Length;
												auto unk2 = ItemCategoryDataArray[i].Data20.unknown;
												if (length2 <= 0 || length2 > 255 || unk2 <= 0 || unk2 >= 255)
												{
													name2 = ItemCategoryDataArray[i].Data28.Name;
													length2 = ItemCategoryDataArray[i].Data28.Length;
												}
												if (name2 && length2 <= 255)
												{
													if (ItemWCategoryArray[i].size() != length2 * 2 + 1)
														ItemWCategoryArray[i].resize(length2 * 2 + 1);
													GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)name2, ItemWCategoryArray[i].data(), length2 * 2);
													if (*(int*)ItemWCategoryArray[i].data() != 0)
													{
														std::string ItemCategory = Utils::UnicodeToUTF8(ItemWCategoryArray[i].data());
														if (ItemCategory != "")
														{
															ItemCategoryArray[i] = ItemCategory;
														}
													}
												}
											}
										}
									}
									else
									{
										if (pAttachedItemTableRowSaveArray.size())
											pAttachedItemTableRowSaveArray.clear();
										if (FNameArray.size())
											FNameArray.clear();
										if (ItemFTextNameArray.size())
											ItemFTextNameArray.clear();
										if (ItemFTextCategoryArray.size())
											ItemFTextCategoryArray.clear();
										if (ItemNameDataArray.size())
											ItemNameDataArray.clear();
										if (ItemCategoryDataArray.size())
											ItemCategoryDataArray.clear();
										if (ItemWNameArray.size())
											ItemWNameArray.clear();
										if (ItemWCategoryArray.size())
											ItemWCategoryArray.clear();
										if (ItemNameArray.size())
											ItemNameArray.clear();
										if (ItemCategoryArray.size())
											ItemCategoryArray.clear();
									}
								}
								else
								{
									if (pAttachedItemSaveArray.size())
										pAttachedItemSaveArray.clear();
									if (pAttachedItemTableRowSaveArray.size())
										pAttachedItemTableRowSaveArray.clear();
									if (FNameArray.size())
										FNameArray.clear();
									if (ItemFTextNameArray.size())
										ItemFTextNameArray.clear();
									if (ItemFTextCategoryArray.size())
										ItemFTextCategoryArray.clear();
									if (ItemNameDataArray.size())
										ItemNameDataArray.clear();
									if (ItemCategoryDataArray.size())
										ItemCategoryDataArray.clear();
									if (ItemWNameArray.size())
										ItemWNameArray.clear();
									if (ItemWCategoryArray.size())
										ItemWCategoryArray.clear();
									if (ItemNameArray.size())
										ItemNameArray.clear();
									if (ItemCategoryArray.size())
										ItemCategoryArray.clear();
								}

								if (sdk::m_LocalWeaponEncryptIndex != 0)
								{
									auto DecryptWeaponIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(sdk::m_LocalWeaponEncryptIndex);
									FName WeaponName;
									WeaponName.ComparisonIndex = DecryptWeaponIndex;
									sdk::SetWeaponName(WeaponName.GetName2());
									sdk::SetIsDMR(sdk::IsDMR());
									sdk::SetIsSR(sdk::IsSR());
									sdk::SetIsShotGun(sdk::IsShotGun());
									sdk::SetIsGrenade(sdk::IsGrenade());

									if (sdk::m_IsDMR)
										sdk::SetWeaponType(2);
									else if (sdk::m_IsSR)
										sdk::SetWeaponType(1);
									else
										sdk::SetWeaponType(0);
								}

								//设置武器Mesh3P
								std::unique_ptr <UWeaponMeshComponent> Mesh3P(new UWeaponMeshComponent(UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(pMesh3P)));
								if (Mesh3P->IsValid())
								{
									sdk::SetWeaponMesh3P(Mesh3P->GetPtr());
									if (t_FiringAttachPoint != NAME_None && t_ScopingAttachPoint != NAME_None)
									{
										auto SkinMesh3P = Mesh3P->Cast<USkinnedMeshComponent>();
										auto componentSpaceTransformsArrayOffset = g_PatternData.offset_USkinnedMeshComponent_MasterPoseComponent + 0x8;
										sdk::SetMesh3P_ComponentSpaceTransformsArrayPtr((ptr_t)SkinMesh3P->GetPtr() + componentSpaceTransformsArrayOffset);
										GetDMA().queueScatterReadEx(ScatterHandle, SkinMesh3P->SGetSkeletalMesh(), &sdk::m_SkinMesh3P_SkeletalMesh, 8);
										if (sdk::m_SkinMesh3P_SkeletalMesh)
										{
											if (t_FiringAttachPoint == NAME_None)
											{
												m_FiringAttachPoint_Socket = nullptr;
											}
											if (t_ScopingAttachPoint == NAME_None)
											{
												m_ScopingAttachPoint_Socket = nullptr;
											}
											USkeletalMesh SkeletalMesh = sdk::m_SkinMesh3P_SkeletalMesh;
											GetDMA().queueScatterReadEx(ScatterHandle, SkeletalMesh->SGetSockets(), &sdk::m_SkeletalMesh_Sockets, sizeof(TArray<void*>));
											GetDMA().queueScatterReadEx(ScatterHandle, SkeletalMesh->SGetSkeleton(), &sdk::m_SkeletalMesh_Skeleton, sizeof(void*));
											GetDMA().queueScatterReadEx(ScatterHandle, SkeletalMesh->SGetRefSkeleton(), &sdk::m_SkeletalMesh_RefSkeleton, sizeof(FReferenceSkeleton));

											if (sdk::m_SkeletalMesh_Sockets->GetData() && sdk::m_SkeletalMesh_Sockets.Num() > 0 && sdk::m_SkeletalMesh_Sockets.Num() < 100)
											{
												if (SocketsList.size() != sdk::m_SkeletalMesh_Sockets.Num())
													SocketsList.resize(sdk::m_SkeletalMesh_Sockets.Num());
												for (uint32_t i = 0; i < sdk::m_SkeletalMesh_Sockets.Num(); i++)
												{
													GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)sdk::m_SkeletalMesh_Sockets->GetData() + i * 8, &SocketsList[i], 8);
												}
												if (SocketsList.size())
												{
													if (SocketNameList.size() != SocketsList.size())
														SocketNameList.resize(SocketsList.size());
													for (uint32_t i = 0; i < SocketsList.size(); i++)
													{
														USkeletalMeshSocket Socket = SocketsList[i];
														if (Socket && Socket->IsValid())
														{
															GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)Socket->SGetSocketName(), &SocketNameList[i], 4);
														}
													}
													//自瞄
													int32_t FiringOutDummyIdx = INDEX_NONE;
													int32_t ScopingOutDummyIdx = INDEX_NONE;
													for (uint32_t i = 0; i < SocketNameList.size(); i++)
													{
														FName SocketName = SocketNameList[i];
														if (SocketName == t_FiringAttachPoint)
														{
															FiringOutDummyIdx = i;
															m_FiringAttachPoint_Socket = SocketsList[i];
														}
														if (SocketName == t_ScopingAttachPoint)
														{
															ScopingOutDummyIdx = i;
															m_ScopingAttachPoint_Socket = SocketsList[i];
														}
													}
													if (m_FiringAttachPoint_Socket == nullptr || m_ScopingAttachPoint_Socket == nullptr)
													{
														goto recontinueGetData;
													}
												}
											}
											else
											{
											recontinueGetData:
												USkeleton Skeleton = sdk::m_SkeletalMesh_Skeleton;
												if (Skeleton->IsValid())
												{
													GetDMA().queueScatterReadEx(ScatterHandle, Skeleton->SGetSockets(), &sdk::m_Skeleton_Sockets, sizeof(TArray<void*>));
													if (sdk::m_Skeleton_Sockets->GetData() && sdk::m_Skeleton_Sockets.Num() > 0 && sdk::m_Skeleton_Sockets.Num() < 100)
													{
														if (SkeletonSocketsList.size() != sdk::m_Skeleton_Sockets.Num())
															SkeletonSocketsList.resize(sdk::m_Skeleton_Sockets.Num());
														for (uint32_t i = 0; i < sdk::m_Skeleton_Sockets.Num(); i++)
														{
															GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)sdk::m_Skeleton_Sockets->GetData() + i * 8, &SkeletonSocketsList[i], 8);
														}
														if (SkeletonSocketsList.size())
														{
															if (SkeletonSocketNameList.size() != SkeletonSocketsList.size())
																SkeletonSocketNameList.resize(SkeletonSocketsList.size());
															for (uint32_t i = 0; i < SkeletonSocketsList.size(); i++)
															{
																USkeletalMeshSocket Socket = SkeletonSocketsList[i];
																if (Socket && Socket->IsValid())
																{
																	GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)Socket->SGetSocketName(), &SkeletonSocketNameList[i], 4);
																}
															}
															int32_t FiringOutDummyIdx = INDEX_NONE;
															int32_t ScopingOutDummyIdx = INDEX_NONE;
															void* FiringAttachPoint_Socket = nullptr;
															void* ScopingAttachPoint_Socket = nullptr;
															for (uint32_t i = 0; i < SkeletonSocketNameList.size(); i++)
															{
																FName SocketName = SkeletonSocketNameList[i];
																if (!FiringAttachPoint_Socket)
																{
																	if (SocketName == t_FiringAttachPoint)
																	{
																		FiringOutDummyIdx = (int32_t)i + (int32_t)SocketNameList.size();
																		FiringAttachPoint_Socket = SkeletonSocketsList[i];
																	}
																}
																if (!ScopingAttachPoint_Socket)
																{
																	if (SocketName == t_ScopingAttachPoint)
																	{
																		ScopingOutDummyIdx = (int32_t)i + (int32_t)SocketNameList.size();
																		ScopingAttachPoint_Socket = SkeletonSocketsList[i];
																	}
																}
																if (FiringAttachPoint_Socket && ScopingAttachPoint_Socket)
																{
																	m_FiringAttachPoint_Socket = FiringAttachPoint_Socket;
																	m_ScopingAttachPoint_Socket = ScopingAttachPoint_Socket;
																	break;
																}
															}
															if (!FiringAttachPoint_Socket || !ScopingAttachPoint_Socket)
															{
																m_FiringAttachPoint_Socket = nullptr;
																m_ScopingAttachPoint_Socket = nullptr;
															}
														}
													}
												}
											}
										}
										else
										{
											m_FiringAttachPoint_Socket = nullptr;
											m_ScopingAttachPoint_Socket = nullptr;
											sdk::SetMesh3P_ComponentSpaceTransformsArrayPtr(0);
											sdk::m_SkinMesh3P_SkeletalMesh = 0;
											sdk::m_SkeletalMesh_Sockets = 0;
											sdk::m_SkeletalMesh_Skeleton = 0;
											ZeroMemory(&sdk::m_SkeletalMesh_RefSkeleton, sizeof(sdk::m_SkeletalMesh_RefSkeleton));
										}
									}

									GetDMA().queueScatterReadEx(ScatterHandle, Mesh3P->SGetAttachedStaticComponentMap(), &sdk::m_AttachedStaticComponentMap, sizeof(TMap));
									auto pMap = reinterpret_cast<ptr_t>(sdk::m_AttachedStaticComponentMap.UnknownData);
									if (*(uint32_t*)(pMap + 8) != *(uint32_t*)(pMap + 0x34))
									{
										auto component = Weapon->GetAttachedComponent(sdk::m_AttachedStaticComponentMap, 0);
										if (!component)
											sdk::m_AttachedStaticComponentMapValid = false;
										else
										{
											GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)component->GetPtr() + g_PatternData.offset_UObjectIndex, &pAttachedStaticComponentIndex, 4);
											if (pAttachedStaticComponentIndex != 0)
											{
												if (!ObjectsStore().GetItemByIdRealTime(UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectIndex>::STATIC_Get(pAttachedStaticComponentIndex)).IsPendingKill())
												{
													sdk::m_AttachedStaticComponentMapValid = true;
												}
												else
													sdk::m_AttachedStaticComponentMapValid = false;
											}
										}
									}
								}
								else
								{
									sdk::SetWeaponMesh3P(nullptr);
									m_FiringAttachPoint_Socket = nullptr;
									m_ScopingAttachPoint_Socket = nullptr;
									sdk::SetMesh3P_ComponentSpaceTransformsArrayPtr(0);
									sdk::m_SkinMesh3P_SkeletalMesh = 0;
									sdk::m_SkeletalMesh_Sockets = 0;
									sdk::m_SkeletalMesh_Skeleton = 0;
									ZeroMemory(&sdk::m_SkeletalMesh_RefSkeleton, sizeof(sdk::m_SkeletalMesh_RefSkeleton));
								}
							}
							else
							{
								sdk::m_LocalWeaponEncryptIndex = 0;
								ZeroMemory(&pAttachedItem, sizeof(pAttachedItem));
								pMesh3P = 0;
								pWeaponTrajectoryData = 0;
								ZeroMemory(&t_FiringAttachPoint, sizeof(FName));
								ZeroMemory(&t_ScopingAttachPoint, sizeof(FName));
								ZeroMemory(&sdk::m_WeaponTrajectoryConfig, sizeof(sdk::m_WeaponTrajectoryConfig));

								if (pAttachedItemSaveArray.size())
									pAttachedItemSaveArray.clear();
								if (pAttachedItemTableRowSaveArray.size())
									pAttachedItemTableRowSaveArray.clear();
								if (FNameArray.size())
									FNameArray.clear();
								if (ItemFTextNameArray.size())
									ItemFTextNameArray.clear();
								if (ItemFTextCategoryArray.size())
									ItemFTextCategoryArray.clear();
								if (ItemNameDataArray.size())
									ItemNameDataArray.clear();
								if (ItemCategoryDataArray.size())
									ItemCategoryDataArray.clear();
								if (ItemWNameArray.size())
									ItemWNameArray.clear();
								if (ItemWCategoryArray.size())
									ItemWCategoryArray.clear();
								if (ItemNameArray.size())
									ItemNameArray.clear();
								if (ItemCategoryArray.size())
									ItemCategoryArray.clear();
								sdk::SetWeaponMesh3P(nullptr);
								m_FiringAttachPoint_Socket = nullptr;
								m_ScopingAttachPoint_Socket = nullptr;
								sdk::SetMesh3P_ComponentSpaceTransformsArrayPtr(0);
								sdk::m_SkinMesh3P_SkeletalMesh = 0;
								sdk::m_SkeletalMesh_Sockets = 0;
								sdk::m_SkeletalMesh_Skeleton = 0;
								ZeroMemory(&sdk::m_SkeletalMesh_RefSkeleton, sizeof(sdk::m_SkeletalMesh_RefSkeleton));
							}
						}
						else
						{
							sdk::SetWeapon(nullptr);
							sdk::m_LocalWeaponEncryptIndex = 0;
							ZeroMemory(&pAttachedItem, sizeof(pAttachedItem));
							pMesh3P = 0;
							pWeaponTrajectoryData = 0;
							ZeroMemory(&t_FiringAttachPoint, sizeof(FName));
							ZeroMemory(&t_ScopingAttachPoint, sizeof(FName));
							ZeroMemory(&sdk::m_WeaponTrajectoryConfig, sizeof(sdk::m_WeaponTrajectoryConfig));

							if (pAttachedItemSaveArray.size())
								pAttachedItemSaveArray.clear();
							if (pAttachedItemTableRowSaveArray.size())
								pAttachedItemTableRowSaveArray.clear();
							if (FNameArray.size())
								FNameArray.clear();
							if (ItemFTextNameArray.size())
								ItemFTextNameArray.clear();
							if (ItemFTextCategoryArray.size())
								ItemFTextCategoryArray.clear();
							if (ItemNameDataArray.size())
								ItemNameDataArray.clear();
							if (ItemCategoryDataArray.size())
								ItemCategoryDataArray.clear();
							if (ItemWNameArray.size())
								ItemWNameArray.clear();
							if (ItemWCategoryArray.size())
								ItemWCategoryArray.clear();
							if (ItemNameArray.size())
								ItemNameArray.clear();
							if (ItemCategoryArray.size())
								ItemCategoryArray.clear();
							sdk::SetWeaponMesh3P(nullptr);
							m_FiringAttachPoint_Socket = nullptr;
							m_ScopingAttachPoint_Socket = nullptr;
							sdk::SetMesh3P_ComponentSpaceTransformsArrayPtr(0);
							sdk::m_SkinMesh3P_SkeletalMesh = 0;
							sdk::m_SkeletalMesh_Sockets = 0;
							sdk::m_SkeletalMesh_Skeleton = 0;
							ZeroMemory(&sdk::m_SkeletalMesh_RefSkeleton, sizeof(sdk::m_SkeletalMesh_RefSkeleton));
						}
					}
					else
					{
						CurrentWeapon = 0;
						CurrentWeapon1 = 0;
						CurrentWeapon2 = 0;
					}
				}
				else
				{
					sdk::m_LocalWeaponEncryptIndex = 0;
					ZeroMemory(&pAttachedItem, sizeof(pAttachedItem));
					pMesh3P = 0;
					pWeaponTrajectoryData = 0;
					ZeroMemory(&t_FiringAttachPoint, sizeof(FName));
					ZeroMemory(&t_ScopingAttachPoint, sizeof(FName));
					ZeroMemory(&sdk::m_WeaponTrajectoryConfig, sizeof(sdk::m_WeaponTrajectoryConfig));

					if (pAttachedItemSaveArray.size())
						pAttachedItemSaveArray.clear();
					if (pAttachedItemTableRowSaveArray.size())
						pAttachedItemTableRowSaveArray.clear();
					if (FNameArray.size())
						FNameArray.clear();
					if (ItemFTextNameArray.size())
						ItemFTextNameArray.clear();
					if (ItemFTextCategoryArray.size())
						ItemFTextCategoryArray.clear();
					if (ItemNameDataArray.size())
						ItemNameDataArray.clear();
					if (ItemCategoryDataArray.size())
						ItemCategoryDataArray.clear();
					if (ItemWNameArray.size())
						ItemWNameArray.clear();
					if (ItemWCategoryArray.size())
						ItemWCategoryArray.clear();
					if (ItemNameArray.size())
						ItemNameArray.clear();
					if (ItemCategoryArray.size())
						ItemCategoryArray.clear();
					sdk::SetWeaponMesh3P(nullptr);
					m_FiringAttachPoint_Socket = nullptr;
					m_ScopingAttachPoint_Socket = nullptr;
					sdk::SetMesh3P_ComponentSpaceTransformsArrayPtr(0);
					sdk::m_SkinMesh3P_SkeletalMesh = 0;
					sdk::m_SkeletalMesh_Sockets = 0;
					sdk::m_SkeletalMesh_Skeleton = 0;
					ZeroMemory(&sdk::m_SkeletalMesh_RefSkeleton, sizeof(sdk::m_SkeletalMesh_RefSkeleton));
					CurrentWeapon = 0;
					CurrentWeapon1 = 0;
					CurrentWeapon2 = 0;
					t_WeaponIndex = 0;
					t_LocalEquippedWeapons = 0;
				}

				std::unique_ptr<AInventoryFacade> InventoryFacede(new AInventoryFacade(UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(DecryptInventoryFacede)));
				if (InventoryFacede->IsValid())
				{
					GetDMA().queueScatterReadEx(ScatterHandle, InventoryFacede->SGetEquipment(), &pEquipment, 8);
					AEquipment Equipment = pEquipment;
					if (Equipment->IsValid())
					{
						GetDMA().queueScatterReadEx(ScatterHandle, Equipment->SGetItems(), &sdk::m_LocalEquipment_Item, sizeof(TArray<void*>));
						if (sdk::m_LocalEquipment_Item->GetData() && sdk::m_LocalEquipment_Item.Num() > 0 && sdk::m_LocalEquipment_Item.Num() < 100)
						{
							Equipment_ItemsObj.resize(sdk::m_LocalEquipment_Item.Num());
							GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)sdk::m_LocalEquipment_Item.GetData(), Equipment_ItemsObj.data(), sdk::m_LocalEquipment_Item.Num() * 8);
							if (sdk::m_LocalEquipment_Item->GetData() && sdk::m_LocalEquipment_Item.Num() > 0 && sdk::m_LocalEquipment_Item.Num() < 100)
							{
								if (pAttachArrayData._itemTableRowArray.size() != sdk::m_LocalEquipment_Item.Num())
									pAttachArrayData._itemTableRowArray.resize(sdk::m_LocalEquipment_Item.Num());
								if (pAttachArrayData._ItemsIDArray.size() != sdk::m_LocalEquipment_Item.Num())
									pAttachArrayData._ItemsIDArray.resize(sdk::m_LocalEquipment_Item.Num());

								if (pAttachArrayData._ItemsIDNameArray.size() != sdk::m_LocalEquipment_Item.Num())
									pAttachArrayData._ItemsIDNameArray.resize(sdk::m_LocalEquipment_Item.Num());

								for (uint32_t i = 0; i < sdk::m_LocalEquipment_Item.Num(); i++)
								{
									auto item = UEquipableItem(Equipment_ItemsObj[i]);
									if (item.IsValid() && IsAddrValid(item->GetPtr()))
									{
										t_HumanBodyRepliedSkin[i].Addr = (ptr_t)item->GetPtr();
										t_HumanBodyRepliedSkin[i].EquimentName = pAttachArrayData._ItemsIDNameArray[i];
										GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)item->SGetReplicatedSkinItem(), &t_HumanBodyRepliedSkin[i].Skin, sizeof(FReplicatedSkinItem));
										GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)item->GetPtr() + g_PatternData.offset_UItem_ItemTableRowPtr, &pAttachArrayData._itemTableRowArray[i], sizeof(void*));
										if (IsAddrValid(pAttachArrayData._itemTableRowArray[i]))
										{
											GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)pAttachArrayData._itemTableRowArray[i] + g_PatternData.offset_ItemTableRowBase_ItemID, &pAttachArrayData._ItemsIDArray[i], sizeof(int32_t));
											{
												pAttachArrayData._ItemsIDNameArray[i] = pAttachArrayData._ItemsIDArray[i].GetName2();
												if (!Utils::StringIsValid(pAttachArrayData._ItemsIDNameArray[i]))
													pAttachArrayData._ItemsIDNameArray[i].clear();
											}
										}

										if (pAttachArrayData._ItemsIDNameArray[i].empty() || pAttachArrayData._ItemsIDNameArray[i] == "")
											continue;

										auto ItemAllInfo = ItemManager::FindItemAllInfo(hash_(pAttachArrayData._ItemsIDNameArray[i]));

										if (ItemAllInfo.ItemName.empty() || ItemAllInfo.ItemCategory.empty())
											continue;

										auto ItemNameString = ItemAllInfo.ItemName;
										auto ItemCategoryString = ItemAllInfo.ItemCategory;

										if (!IsAddrValid(item->GetPtr()) || !IsAddrValid(pAttachArrayData._itemTableRowArray[i])
											|| ItemNameString.size() <= 4 || ItemCategoryString.size() <= 2)
											continue;

										std::string itemName = ItemNameString;

										std::string itemCategoryName = ItemCategoryString;

										if (itemName.empty() || itemCategoryName.empty())
											continue;

										hash_t itemHashName = hash_(itemName);
										LootCategory itemCategory = GetLootCategory(itemCategoryName);
										if (itemCategory != LootCategory::Unknown)
										{
											const auto& loots = ItemManager::GetGroupManager()->GetLoots()[itemCategory];
											if (loots.empty())
												continue;
											auto lootInfo = loots.find(itemHashName);
											if (lootInfo == loots.end())
												continue;
											auto loot = lootInfo->second;
											if (loot == nullptr)
												continue;
											//auto EquipSlotID = item->GetEquipSlotID();
											if (i == (uint32_t)EEquipSlotID::Head)
											{
												(*t_InventoryEquipment)[0].itemName = itemName;
												(*t_InventoryEquipment)[0].LootId = loot->id;
												GetDMA().queueScatterReadEx(ScatterHandle, item->SGetDurability(), &(*t_InventoryEquipment)[0].Durability, 4);
												GetDMA().queueScatterReadEx(ScatterHandle, item->SGetDurabilityMax(), &(*t_InventoryEquipment)[0].DurabilityMax, 4);
											}
											else if (i == (uint32_t)EEquipSlotID::TorsoArmor)
											{
												(*t_InventoryEquipment)[1].itemName = itemName;
												(*t_InventoryEquipment)[1].LootId = loot->id;
												GetDMA().queueScatterReadEx(ScatterHandle, item->SGetDurability(), &(*t_InventoryEquipment)[1].Durability, 4);
												GetDMA().queueScatterReadEx(ScatterHandle, item->SGetDurabilityMax(), &(*t_InventoryEquipment)[1].DurabilityMax, 4);
											}
											else if (i == (uint32_t)EEquipSlotID::Backpack)
											{
												(*t_InventoryEquipment)[2].itemName = itemName;
												(*t_InventoryEquipment)[2].LootId = loot->id;
												GetDMA().queueScatterReadEx(ScatterHandle, item->SGetDurability(), &(*t_InventoryEquipment)[2].Durability, 4);
												GetDMA().queueScatterReadEx(ScatterHandle, item->SGetDurabilityMax(), &(*t_InventoryEquipment)[2].DurabilityMax, 4);
											}
										}
									}
									else
									{
										if (i == (uint32_t)EEquipSlotID::Head || i == (uint32_t)EEquipSlotID::TorsoArmor || i == (uint32_t)EEquipSlotID::Backpack)
										{
											switch (i)
											{
											case (int)EEquipSlotID::Head:
												(*t_InventoryEquipment)[0] = Inventory_Equipment();
												break;
											case (int)EEquipSlotID::TorsoArmor:
												(*t_InventoryEquipment)[1] = Inventory_Equipment();
												break;
											case (int)EEquipSlotID::Backpack:
												(*t_InventoryEquipment)[2] = Inventory_Equipment();
												break;
											default:
												break;
											}
											continue;
										}
									}

								}
							}
						}
					}

				}
			}
			else
			{
				CurrentWeapon = 0;
				CurrentWeapon1 = 0;
				CurrentWeapon2 = 0;
				DecryptRootComponent = 0;
				DecryptInventoryFacede = 0;
				m_LocalMesh = 0;
				m_LocalWeaponPorcess = 0;
				t_WeaponIndex = 0;
				t_LocalEquippedWeapons = 0;
			}
		}

		if (m_FiringAttachPoint_Socket && m_ScopingAttachPoint_Socket)
		{
			std::unique_ptr<USkeletalMeshSocket> FiringSocket(new USkeletalMeshSocket(m_FiringAttachPoint_Socket));
			std::unique_ptr<USkeletalMeshSocket> ScopingSocket(new USkeletalMeshSocket(m_ScopingAttachPoint_Socket));

			GetDMA().queueScatterReadEx(ScatterHandle, FiringSocket->SGetBoneName(), &sdk::m_FiringSocket_BoneName, 8);
			GetDMA().queueScatterReadEx(ScatterHandle, ScopingSocket->SGetBoneName(), &sdk::m_ScopingSocket_BoneName, 8);
			if (sdk::m_FiringSocket_BoneName.ComparisonIndex != 0)
			{
				int32_t FiringBoneIndex = sdk::m_SkeletalMesh_RefSkeleton.FindBoneIndex(sdk::m_FiringSocket_BoneName);
				if (FiringBoneIndex != INDEX_NONE)
				{
					sdk::SetFiringSocket_BoneIndex(FiringBoneIndex);
				}
			}
			if (sdk::m_ScopingSocket_BoneName.ComparisonIndex != 0)
			{
				int32_t ScopingBoneIndex = sdk::m_SkeletalMesh_RefSkeleton.FindBoneIndex(sdk::m_ScopingSocket_BoneName);
				if (ScopingBoneIndex != INDEX_NONE)
				{
					sdk::SetScopingSocket_BoneIndex(ScopingBoneIndex);
				}
			}
		}
		else
		{
			m_FiringAttachPoint_Socket = nullptr;
			m_ScopingAttachPoint_Socket = nullptr;
		}



		float t_BallisticDragScale = 0.f;
		float t_BallisticDropScale = 0.f;

		auto TslSettings = UTslSettings::GetTslSettings();
		if (TslSettings->IsValid())
		{
			GetDMA().queueScatterReadEx(ScatterHandle, TslSettings->SGetBallisticDragScale(), &t_BallisticDragScale, 4);
			GetDMA().queueScatterReadEx(ScatterHandle, TslSettings->SGetBallisticDropScale(), &t_BallisticDropScale, 4);
		}

		GetDMA().executeScatterRead(ScatterHandle);

		sdk::SetBallisticDragScale(t_BallisticDragScale);
		sdk::SetBallisticDropScale(t_BallisticDropScale);
		sdk::SetFiringAttachPoint_Socket(m_FiringAttachPoint_Socket);
		sdk::SetScopingAttachPoint_Socket(m_ScopingAttachPoint_Socket);
		//设置武器各位置
		{
			if (IsAddrValid(CurrentWeapon1))
				sdk::SetWeaponNum1((void*)CurrentWeapon1);
			else
				sdk::SetWeaponNum1(nullptr);
			if (IsAddrValid(CurrentWeapon2))
				sdk::SetWeaponNum2((void*)CurrentWeapon2);
			else
				sdk::SetWeaponNum2(nullptr);
		}

		//处理武器配件
		{
			std::vector<FWeaponAttachmentData> RetArray;
			if (FNameArray.size() && ItemNameArray.size() && ItemCategoryArray.size())
			{
				for (uint32_t i = 0; i < pAttachedItemTableRowSaveArray.size(); i++)
				{
					auto ItemTableRow = ptr_t(pAttachedItemTableRowSaveArray[i]);
					if (ItemTableRow <= 0x100000) continue;
					std::string itemName = ItemNameArray[i];
					std::string itemCategoryName = ItemCategoryArray[i];
					RetArray.push_back(FWeaponAttachmentData(FNameArray[i].ComparisonIndex, itemName, itemCategoryName));
				}
			}
			if (RetArray.size())
				sdk::SetWeaponAttachmentData(RetArray);
			else
				sdk::SetWeaponAttachmentData(std::vector<FWeaponAttachmentData>());
		}

		sdk::SetInventoryEquipment(*t_InventoryEquipment);
		sdk::SetCurvesKeysArray(t_CurvesKeysArray);
		sdk::SetWeaponIndex(t_WeaponIndex);
		sdk::SetbIsCanted(t_bIsCanted);
		sdk::SetCustomTimeDilation(t_CustomTimeDilation);
		sdk::SetTrajectoryGravityZ(t_TrajectoryGravityZ);
		sdk::SetCurrentZeroLevel(t_CurrentZeroLevel);
		sdk::SetHumanBodyRepliedSkin(t_HumanBodyRepliedSkin);
		sdk::SetLocalAnimScriptInstance(t_LocalAnimScriptInstance);

		return;
	}

	void ObjectManager::GameWorld()
	{
		std::unique_ptr<ptr_t> ModuleValue(new ptr_t(0));
		std::unique_ptr<ptr_t> MousePosPtr(new ptr_t(0));
		std::unique_ptr<ptr_t> encryptWorld(new ptr_t(0));
		std::unique_ptr<ptr_t> Language1(new ptr_t(0));
		std::unique_ptr<ptr_t> EncryptOwningGameInstance(new ptr_t(0));
		std::unique_ptr<ptr_t> EncryptPersistentLevel(new ptr_t(0));
		std::unique_ptr<ptr_t> EncryptGameState(new ptr_t(0));
		std::unique_ptr <int32_t> WorldEncryptIndex(new int32_t(0));
		std::unique_ptr <FVector2D> t_WorldLocation(new FVector2D());
		std::unique_ptr <TUObjectArray> t_ObjectArray(new TUObjectArray());
		std::unique_ptr <ptr_t> EncryptAActor(new ptr_t(0));
		std::unique_ptr < TArray<void*>> LocalPlayerArray(new TArray<void*>());
		std::unique_ptr < FVector> t_LerpSafetyZonePosition(new FVector());
		std::unique_ptr <float> t_LerpSafetyZoneRadius(new float(0.f));
		std::unique_ptr < FVector> t_LerpBlueZonePosition(new FVector());
		std::unique_ptr <float> t_LerpBlueZoneRadius(new float(0.f));
		std::unique_ptr < FVector> t_RedZonePosition(new FVector());
		std::unique_ptr <float> t_RedZoneRadius(new float(0.f));
		std::unique_ptr < FVector> t_BlackZonePosition(new FVector());
		std::unique_ptr <float> t_BlackZoneRadius(new float(0.f));
		std::unique_ptr < FVector> t_SafetyZonePosition(new FVector());
		std::unique_ptr <float> t_SafetyZoneRadius(new float(0.f));
		std::unique_ptr <int> NumAliveTeams(new int(0));
		std::unique_ptr<ptr_t> DecryptacknowledgedPawn(new ptr_t(0));
		std::unique_ptr<ptr_t> DecryptPCPawn(new ptr_t(0));
		std::unique_ptr<ptr_t> pPlayerCameraManager(new ptr_t(0));
		std::unique_ptr<ptr_t> pMyHud(new ptr_t(0));
		std::unique_ptr<ptr_t> pAntiCheatCharacterSyncManager(new ptr_t(0));
		std::unique_ptr<ptr_t> pPlayerInput(new ptr_t(0));
		std::unique_ptr < ptr_t> pInputAxisProperties(new ptr_t(0));
		std::unique_ptr <float> t_DamageDelta(new float(0.f));
		std::unique_ptr < int32_t> acknowledgedPawnIndex(new int32_t(0));
		std::unique_ptr < ptr_t> EncryptacknowledgedPawnClass(new ptr_t(0));
		char LanguageName[32] = {};

		auto ScatterHandle = sdk::GetHandle1();
		if (!ScatterHandle)
			return;
		do
		{
			GetDMA().queueScatterReadEx(ScatterHandle, g_PatternData.ModuleHandle, &(*ModuleValue), 8);
			GetDMA().queueScatterReadEx(ScatterHandle, g_PatternData.ptr_GetMousePostion, &(*MousePosPtr), 8);
			GetDMA().queueScatterReadEx(ScatterHandle, g_PatternData.UWorld, &(*encryptWorld), 8);
			GetDMA().queueScatterReadEx(ScatterHandle, g_PatternData.offset_Language, &(*Language1), 8);
			GetDMA().executeScatterRead(ScatterHandle);
			if (*ModuleValue == 0)
			{
				CONSOLE_INFO("ModuleValue %p|%p|%p|%p", g_PatternData.ModuleHandle, *ModuleValue, *encryptWorld, ScatterHandle);
				sdk::SetInGame(false);
				return;
			}
			if (!*encryptWorld)
				return;
			auto tempWorld = std::make_unique<UWorld>(UEEncryptedObjectProperty<ptr_t, DecryptFunc::General>::STATIC_Get(*encryptWorld));
			if (tempWorld->IsValid())
				sdk::SetWorld(tempWorld->GetPtr());

			GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)tempWorld->GetPtr() + g_PatternData.offset_WorldLocation, &(*t_WorldLocation), sizeof(FVector2D));
			GetDMA().queueScatterReadEx(ScatterHandle, *MousePosPtr, &(*MousePosPtr), sizeof(ptr_t));
			GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)tempWorld->GetPtr() + g_PatternData.offset_UObjectNameComparisonIndex, &(*WorldEncryptIndex), 4);
			GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)tempWorld->GetPtr() + g_PatternData.offset_OwningGameInstance, &(*EncryptOwningGameInstance), 8);
			GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)tempWorld->GetPtr() + g_PatternData.offsetPersistentLevel, &(*EncryptPersistentLevel), 8);
			GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)tempWorld->GetPtr() + g_PatternData.offset_GameState, &(*EncryptGameState), 8);
			GetDMA().queueScatterReadEx(ScatterHandle, g_PatternData.UObjectArray, &(*t_ObjectArray), sizeof(TUObjectArray));
			GetDMA().queueScatterReadEx(ScatterHandle, *Language1 + 0x30, LanguageName, sizeof(LanguageName));
			GetDMA().executeScatterRead(ScatterHandle);
			*MousePosPtr = GetDMA().Read<ptr_t>(*MousePosPtr + 0xE0);
			sdk::SetMousePosPtr(*MousePosPtr);
			std::unique_ptr <std::string> WorldLanguageName(new std::string(LanguageName));
			switch (hash_(*WorldLanguageName))
			{
			case "zh_CN"_hash:
				sdk::SetLanguageType(0);
				break;
			case "zh_TW"_hash:
				sdk::SetLanguageType(1);
				break;
			case "en"_hash:
				sdk::SetLanguageType(2);
				break;
			case "ja"_hash:
				sdk::SetLanguageType(3);
				break;
			case "ko"_hash:
				sdk::SetLanguageType(4);
				break;
			default:
				sdk::SetLanguageType(3);
				break;
			}

			sdk::SetWorldLocation(*t_WorldLocation);
			sdk::SetObjectArray(*t_ObjectArray);

			std::unique_ptr < FName> WorldFName(new FName(UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(*WorldEncryptIndex)));
			std::unique_ptr<std::string> WorldMapName = std::make_unique<std::string>(WorldFName->GetName2());
			sdk::SetWorldName(*WorldMapName);
			if (WorldMapName->find("Lobby") != std::string::npos)
			{
				sdk::m_NumAliveTeams = 0;
				sdk::SetPlayerState(0);
				sdk::SetDamageDelta(0.f);
				CONSOLE_INFO("Not In Game");
				break;
			}

			auto OwningGameInstance = std::make_unique<UGameInstance>(UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(*EncryptOwningGameInstance));
			if (!OwningGameInstance->IsValid())
				return;

			auto pLevel = std::make_unique<ULevel>(UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(*EncryptPersistentLevel));
			if (!pLevel->IsValid())
				return;
			ATslGameState TslGameState = UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(*EncryptGameState);
			if (IsAddrValid(TslGameState->GetPtr()))
			{
				if (sdk::InGame())
				{
					GetDMA().queueScatterReadEx(ScatterHandle, TslGameState->SGetNumAliveTeams(), &(*NumAliveTeams), 4);
					GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)TslGameState->GetPtr() + g_PatternData.offset_ATslGameState_LerpSafetyZonePosition, &(*t_LerpSafetyZonePosition), sizeof(FVector));
					GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)TslGameState->GetPtr() + g_PatternData.offset_ATslGameState_LerpSafetyZoneRadius, &(*t_LerpSafetyZoneRadius), sizeof(float));
					GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)TslGameState->GetPtr() + g_PatternData.offset_ATslGameState_LerpBlueZonePosition, &(*t_LerpBlueZonePosition), sizeof(FVector));
					GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)TslGameState->GetPtr() + g_PatternData.offset_ATslGameState_LerpBlueZoneRadius, &(*t_LerpBlueZoneRadius), sizeof(float));
					GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)TslGameState->GetPtr() + g_PatternData.offset_ATslGameState_RedZonePosition, &(*t_RedZonePosition), sizeof(FVector));
					GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)TslGameState->GetPtr() + g_PatternData.offset_ATslGameState_RedZoneRadius, &(*t_RedZoneRadius), sizeof(float));
					GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)TslGameState->GetPtr() + g_PatternData.offset_ATslGameState_BlackZonePosition, &(*t_BlackZonePosition), sizeof(FVector));
					GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)TslGameState->GetPtr() + g_PatternData.offset_ATslGameState_BlackZoneRadius, &(*t_BlackZoneRadius), sizeof(float));
					GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)TslGameState->GetPtr() + g_PatternData.offset_ATslGameState_SafetyZonePosition, &(*t_SafetyZonePosition), sizeof(FVector));
					GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)TslGameState->GetPtr() + g_PatternData.offset_ATslGameState_SafetyZoneRadius, &(*t_SafetyZoneRadius), sizeof(float));
				}
			}


			GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)pLevel->GetPtr() + g_PatternData.offsetAActors, &(*EncryptAActor), 8);
			GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)OwningGameInstance->GetPtr() + g_PatternData.offset_LocalPlayers, &(*LocalPlayerArray), sizeof(TArray<void*>));
			GetDMA().executeScatterRead(ScatterHandle);

			sdk::SetLerpSafetyZonePosition(*t_LerpSafetyZonePosition);
			sdk::SetLerpBlueZonePosition(*t_LerpBlueZonePosition);
			sdk::SetRedZonePosition(*t_RedZonePosition);
			sdk::SetBlackZonePosition(*t_BlackZonePosition);
			sdk::SetSafetyZonePosition(*t_SafetyZonePosition);
			sdk::SetLerpSafetyZoneRadius(*t_LerpSafetyZoneRadius);
			sdk::SetLerpBlueZoneRadius(*t_LerpBlueZoneRadius);
			sdk::SetRedZoneRadius(*t_RedZoneRadius);
			sdk::SetBlackZoneRadius(*t_BlackZoneRadius);
			sdk::SetSafetyZoneRadius(*t_SafetyZoneRadius);

			if (*NumAliveTeams >= 0 && *NumAliveTeams <= 200)
				sdk::SetNumAliveTeams(*NumAliveTeams);
			else
				sdk::SetNumAliveTeams(0);

			if (LocalPlayerArray->Empty())
				return;

			DecryptAActor = UEEncryptedObjectProperty<ptr_t, DecryptFunc::General>::STATIC_Get(*EncryptAActor);
			if (!DecryptAActor)
				return;

			auto localPlayer = std::make_unique<ULocalPlayer>(UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(GetDMA().Read<ptr_t>((ptr_t)LocalPlayerArray->Data)));
			if (!localPlayer->IsValid())
				return;

			auto PC = std::make_unique<APlayerController>(UEEncryptedObjectProperty<APlayerController, DecryptFunc::General>::STATIC_Get(GetDMA().Read<ptr_t>((ptr_t)localPlayer->GetPtr() + g_PatternData.offset_PlayerController)));
			if (!PC)
				return;

			sdk::SetControl(PC->GetPtr());


			GetDMA().queueScatterReadEx(ScatterHandle, PC->SGetPlayerCameraManager(), &(*pPlayerCameraManager), 8);
			GetDMA().queueScatterReadEx(ScatterHandle, PC->SGetMyHUD(), &(*pMyHud), 8);
			GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)PC->GetPtr() + g_PatternData.offset_APlayerController_AcknowledgedPawn, &(*DecryptacknowledgedPawn), 8);
			GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)PC->GetPtr() + g_PatternData.offset_ATslPlayerController_PlayerInput, &(*pPlayerInput), 8);
			GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)PC->GetPtr() + g_PatternData.offset_AController_Pawn, &(*DecryptPCPawn), 8);
			if (offset_AntiCheatCharacterSyncManager != 0)
				GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)PC->GetPtr() + offset_AntiCheatCharacterSyncManager, &(*pAntiCheatCharacterSyncManager), 8);

			GetDMA().executeScatterRead(ScatterHandle);

			if (IsAddrValid(*pMyHud))
				sdk::SetMyHUD((void*)*pMyHud);

			auto playerCameraManager = std::make_unique<APlayerCameraManager>(*pPlayerCameraManager);
			if (!playerCameraManager || !playerCameraManager->IsValid())
				return;

			sdk::SetViewTarget_POV_FOV(playerCameraManager->GetViewTarget_POV_FOV());
			sdk::SetViewTarget_POV_Location(playerCameraManager->GetViewTarget_POV_Location());
			sdk::SetViewTarget_POV_Rotation(playerCameraManager->GetViewTarget_POV_Rotation());


			if (IsAddrValid(*pPlayerInput))
			{
				GetDMA().queueScatterReadEx(ScatterHandle, *pPlayerInput + g_PatternData.InputAxisProperties, &(*pInputAxisProperties), 8);
			}
			//std::unique_ptr <FVector2D> MouseSensitivity(new FVector2D());
			//if (IsAddrValid(*pInputAxisProperties))
			//{
			//	GetDMA().queueScatterReadEx(ScatterHandle, *pInputAxisProperties + 0x240, &((*MouseSensitivity).X), 4);
			//	GetDMA().queueScatterReadEx(ScatterHandle, *pInputAxisProperties + 0x240 + 0x28, &((*MouseSensitivity).Y), 4);
			//}
			std::unique_ptr <float> t_BaseEyeHeight(new float(0.f));
			auto LocalPawn = std::make_unique<APawn>(UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(*DecryptPCPawn));
			if (LocalPawn->IsValid())
				GetDMA().queueScatterReadEx(ScatterHandle, LocalPawn->SGetBaseEyeHeight(), &(*t_BaseEyeHeight), 4);

			GetDMA().executeScatterRead(ScatterHandle);

			sdk::SetBaseEyeHeight(*t_BaseEyeHeight);
			//sdk::SetMouseSensitivity(*MouseSensitivity);

			//if (offset_AntiCheatCharacterSyncManager == 0)
			//{
			//	for (int i = 0xC00; i < 0x1200; i += 8)
			//	{
			//		UObject obj = *Game.memory().Read<void*>((ptr_t)PC->GetPtr() + i);
			//		if (obj->IsValid() && obj->IsA<UObject>())
			//		{
			//			if (obj->GetName(false, false) == ("ACCharacterSyncManager"))
			//			{
			//				offset_AntiCheatCharacterSyncManager = i;
			//				break;
			//			}
			//		}
			//	}
			//}
			//else
			//{
			//	//迷雾玩家
			//	if (pAntiCheatCharacterSyncManager > 0x100000)
			//	{
			//		GetDMA().queueScatterReadEx(ScatterHandle, pAntiCheatCharacterSyncManager + 0x50, &DormantCharacterClientList, sizeof(TArray<void*>));
			//		if (DormantCharacterClientList->GetData() && DormantCharacterClientList.Num() > 0 && DormantCharacterClientList.Num() < 100)
			//		{
			//			FogPlayer.resize(DormantCharacterClientList.Num());
			//			GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)DormantCharacterClientList.Data, FogPlayer.data(), 8 * DormantCharacterClientList.Num());
			//		}
			//	}
			//}


			auto acknowledgedPawn = std::make_unique<APawn>(UEEncryptedObjectProperty<void*, DecryptFunc::General>::STATIC_Get(*DecryptacknowledgedPawn));
			if (!acknowledgedPawn->IsValid())
			{
				sdk::SetPlayerState(0);
				sdk::SetDamageDelta(0.f);
				sdk::SetPlayer(nullptr);
				sdk::SetWheeledVehiclePlayer(nullptr);
				sdk::SetFloatingVehiclePlayer(nullptr);
			}
			else
			{
				auto& PLAYER = sdk::GetPlayer();
				if (IsAddrValid(PLAYER))
				{
					GetDMA().queueScatterReadEx(ScatterHandle, PLAYER + g_PatternData.offset_ATslCharacter_VehicleRiderComponent, &t_VehicleRiderComponent, 8);
					if (IsAddrValid(t_VehicleRiderComponent))
					{
						GetDMA().queueScatterReadEx(ScatterHandle, t_VehicleRiderComponent + g_PatternData.offset_UVehicleRiderComponent_LastVehiclePawn, &t_LastVehiclePawn, 8);
					}
					GetDMA().queueScatterReadEx(ScatterHandle, PLAYER + g_PatternData.offset_APawn_PlayerState, &t_PlayerState1, 8);
					if (t_PlayerState1)
					{
						auto PlayerState = std::make_unique<ATslPlayerState>(UEEncryptedObjectProperty<ATslPlayerState, DecryptFunc::General>::STATIC_Get(t_PlayerState1));
						if (!PlayerState || !PlayerState->IsValid())
						{
							APawn LastVehiclePawn = t_LastVehiclePawn;
							if (LastVehiclePawn->IsValid() && IsAddrValid(t_LastVehiclePawn))
							{
								GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)LastVehiclePawn->GetPtr() + g_PatternData.offset_APawn_PlayerState, &t_PlayerState2, 8);
								if (t_PlayerState2)
								{
									PlayerState.reset(new ATslPlayerState(UEEncryptedObjectProperty<ATslPlayerState, DecryptFunc::General>::STATIC_Get(t_PlayerState2)));
									if (PlayerState && PlayerState->IsValid())
									{
										sdk::SetPlayerState((ptr_t)PlayerState->GetPtr());
										GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)PlayerState->GetPtr() + g_PatternData.offset_ATslPlayerState_DamageDealtOnEnemy, &(*t_DamageDelta), 4);
									}
								}
							}
						}
						else
						{
							sdk::SetPlayerState((ptr_t)PlayerState->GetPtr());
							GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)PlayerState->GetPtr() + g_PatternData.offset_ATslPlayerState_DamageDealtOnEnemy, &(*t_DamageDelta), 4);
						}
					}
				}
				else
				{
					sdk::SetPlayerState((ptr_t)0);
				}

				GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)acknowledgedPawn->GetPtr() + g_PatternData.offset_UObjectNameComparisonIndex, &(*acknowledgedPawnIndex), 4);
				GetDMA().queueScatterReadEx(ScatterHandle, (ptr_t)acknowledgedPawn->GetPtr() + g_PatternData.offset_UObjectClass, &(*EncryptacknowledgedPawnClass), 8);
				GetDMA().executeScatterRead(ScatterHandle);

				sdk::SetDamageDelta(*t_DamageDelta);

				////更新迷雾玩家
				//{
				//	fogObjectsCopy = fogObjects;
				//	for (auto cacheIter = fogObjectsCopy.begin(); cacheIter != fogObjectsCopy.end(); )
				//	{
				//		auto memoryAddress = reinterpret_cast<ptr_t>((*cacheIter)->GetPtr());
				//		auto actorIter = FogPlayer.begin();
				//		if (FogPlayer.empty())
				//			cacheIter = fogObjectsCopy.erase(cacheIter);
				//		while (actorIter != FogPlayer.end())
				//		{
				//			// Remove existing actors
				//			if (memoryAddress == *actorIter)
				//			{
				//				cacheIter++;
				//				FogPlayer.erase(actorIter);
				//				break;
				//			}
				//			// Not found!
				//			if (++actorIter == FogPlayer.end())
				//			{
				//				(*cacheIter)->Release();
				//				cacheIter = fogObjectsCopy.erase(cacheIter);
				//			}
				//		}
				//	}
				//	for (AActor actor : FogPlayer)
				//	{
				//		if (!actor->IsValid())
				//			continue;
				//		fogObjectsCopy.push_back(std::shared_ptr<AActor>(new AActor(actor)));
				//	}
				//	SetFogObjects(&fogObjectsCopy);
				//}

				std::unique_ptr < FName> acknowledgedIndex(new FName(UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(*acknowledgedPawnIndex)));
				std::unique_ptr<std::string> Name = std::make_unique<std::string>(acknowledgedIndex->GetName2());
				if (Name->find(("Default")) != std::string::npos)
					break;

				if (!*EncryptacknowledgedPawnClass)
					break;
				std::unique_ptr<ptr_t> tempObjectsClass(new ptr_t(UEEncryptedObjectProperty<ptr_t, DecryptFunc::UObjectClass>::STATIC_Get(*EncryptacknowledgedPawnClass)));
				if (!*tempObjectsClass)
					break;
				std::unique_ptr<OMFClassBaseChain> acknowledgedChain(new OMFClassBaseChain(GetDMA().Read<OMFClassBaseChain>((ptr_t)*tempObjectsClass + g_PatternData.offset_UClassBaseChainArray)));
				std::unique_ptr<std::unordered_map < UObjectType, MapClassBaseChain>>TypeArray(new std::unordered_map < UObjectType, MapClassBaseChain>());
				TypeArray->emplace(EPlayer, MapClassBaseChain((ptr_t)acknowledgedPawn->GetPtr(), *acknowledgedChain, 0));
				TypeArray->emplace(EVehicle, MapClassBaseChain((ptr_t)acknowledgedPawn->GetPtr(), *acknowledgedChain, 0));
				TypeArray->emplace(EBoat, MapClassBaseChain((ptr_t)acknowledgedPawn->GetPtr(), *acknowledgedChain, 0));

				int NumParentClassBasesInChainMinusOne = EPlayerUClass.NumClassBasesInChainMinusOne;
				GetDMA().queueScatterReadEx(ScatterHandle, (*TypeArray)[EPlayer].Chian.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
					&(*TypeArray)[EPlayer].Value, 8);

				NumParentClassBasesInChainMinusOne = EVehicleUClass.NumClassBasesInChainMinusOne;
				GetDMA().queueScatterReadEx(ScatterHandle, (*TypeArray)[EVehicle].Chian.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
					&(*TypeArray)[EVehicle].Value, 8);

				NumParentClassBasesInChainMinusOne = EBoatUClass.NumClassBasesInChainMinusOne;
				GetDMA().queueScatterReadEx(ScatterHandle, (*TypeArray)[EBoat].Chian.ClassBaseChainArray + NumParentClassBasesInChainMinusOne * sizeof(void*),
					&(*TypeArray)[EBoat].Value, 8);

				GetDMA().executeScatterRead(ScatterHandle);


				auto PlayerBaseChain = EPlayerUClass;
				NumParentClassBasesInChainMinusOne = PlayerBaseChain.NumClassBasesInChainMinusOne;
				if ((NumParentClassBasesInChainMinusOne <= (*TypeArray)[EPlayer].Chian.NumClassBasesInChainMinusOne &&
					(*TypeArray)[EPlayer].Value == PlayerBaseChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray))
				{

					sdk::SetPlayer(acknowledgedPawn->GetPtr());
					sdk::SetWheeledVehiclePlayer(nullptr);
					sdk::SetFloatingVehiclePlayer(nullptr);
					sdk::SetMortarPlayer(nullptr);

				}
				else
				{
					auto VehicleChain = EVehicleUClass;
					NumParentClassBasesInChainMinusOne = VehicleChain.NumClassBasesInChainMinusOne;
					if (NumParentClassBasesInChainMinusOne <= (*TypeArray)[EVehicle].Chian.NumClassBasesInChainMinusOne &&
						(*TypeArray)[EVehicle].Value == VehicleChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
					{
						sdk::SetWheeledVehiclePlayer(acknowledgedPawn->GetPtr());
						sdk::SetFloatingVehiclePlayer(nullptr);
						sdk::SetMortarPlayer(nullptr);
					}
					else if (*Name == "MortarPawn_C")
					{
						sdk::SetMortarPlayer(acknowledgedPawn->GetPtr());
						sdk::SetWheeledVehiclePlayer(nullptr);
						sdk::SetFloatingVehiclePlayer(nullptr);
					}
					else
					{
						auto BoatChain = EBoatUClass;
						NumParentClassBasesInChainMinusOne = BoatChain.NumClassBasesInChainMinusOne;
						if (NumParentClassBasesInChainMinusOne <= (*TypeArray)[EBoat].Chian.NumClassBasesInChainMinusOne &&
							(*TypeArray)[EBoat].Value == BoatChain.ClassBaseChainArray + g_PatternData.offset_UClassBaseChainArray)
						{
							sdk::SetFloatingVehiclePlayer(acknowledgedPawn->GetPtr());
							sdk::SetWheeledVehiclePlayer(nullptr);
							sdk::SetMortarPlayer(nullptr);
						}
					}
				}
			}
			sdk::SetInGame(true);
			ClearOnce = true;
			return;
		} while (false);
		sdk::SetWorld(nullptr);
		sdk::SetControl(nullptr);
		sdk::SetPlayer(nullptr);
		sdk::SetMesh(nullptr);
		sdk::SetWheeledVehiclePlayer(nullptr);
		sdk::SetFloatingVehiclePlayer(nullptr);
		sdk::SetWeapon(nullptr);
		sdk::SetWeaponNum1(nullptr);
		sdk::SetWeaponNum2(nullptr);
		sdk::SetWeaponMesh3P(nullptr);
		sdk::SetMyHUD(nullptr);
		sdk::SetInGame(false);

		if (ClearOnce)
		{
			ClearOnce = false;
			DecryptAActor = 0;
			//MyHud
			pWidgetStateMap = 0;
			//小地图
			pCanvasWidget = nullptr;
			EncryptCanvasWidgetID = 0;
			pMinimapSizeOffset = 0;
			pSelectMinimapSizeIndex = 0;
			//Fog
			ZeroMemory(&DormantCharacterClientList, sizeof(DormantCharacterClientList));
			FogPlayer.clear();
			//本人
			sdk::SetPlayerState(0);
			t_PlayerState1 = 0;
			t_PlayerState2 = 0;
			t_VehicleRiderComponent = 0;
			t_LastVehiclePawn = 0;
			DecryptRootComponent = 0;
			DecryptInventoryFacede = 0;
			m_LocalMesh = 0;
			pEquipment = 0;
			CurrentWeapon = 0;
			CurrentWeapon1 = 0;
			CurrentWeapon2 = 0;
			Equipment_ItemsObj.clear();
			pMesh3P = 0;
			ZeroMemory(&pAttachedItem, sizeof(pAttachedItem));
			pWeaponTrajectoryData = 0;
			pAttachedItemSaveArray.clear();
			pAttachedItemTableRowSaveArray.clear();
			FNameArray.clear();
			ItemWNameArray.clear();
			ItemWCategoryArray.clear();
			ItemNameArray.clear();
			ItemCategoryArray.clear();
			SocketsList.clear();
			SocketNameList.clear();
			SkeletonSocketsList.clear();
			SkeletonSocketNameList.clear();
		}

		return;
	}
}