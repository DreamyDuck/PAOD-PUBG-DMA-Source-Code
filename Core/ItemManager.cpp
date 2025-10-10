#include "Include/WinHeaders.hpp"
#include "ItemManager.h"
#include "FilePak.h"
#include "SDK/SDK.h"

namespace IronMan::Core
{
	using namespace SDK;

	extern std::string itemConfigPath;
	extern std::string itemDefConfigPath;

	std::unordered_map<hash_t, NameBind>SaveNameBind;

	ItemCategory GetItemCategory(std::string categoryString)
	{
		auto hash_string = hash_(categoryString.c_str());
		switch (hash_string)
		{
		case "weapon"_hash:
			return ItemCategory::Weapon;
		case "attach"_hash:
			return ItemCategory::Attach;
		case "ammo"_hash:
			return ItemCategory::Ammo;
		case "useable"_hash:
			return ItemCategory::Usebale;
		case "equipment"_hash:
			return ItemCategory::Equipment;
		case "jacket"_hash:
			return ItemCategory::Jacket;
		case "special"_hash:
			return ItemCategory::Special;
		case "tactical"_hash:
			return ItemCategory::Tactical;
		case "misc"_hash:
			return ItemCategory::Misc;
		default:
			return ItemCategory::Unknown;
		}
	}

	LootCategory GetLootCategory(std::string categoryString)
	{
		auto hash_string = hash_(categoryString.c_str());
		switch (hash_string)
		{
		case "Main Weapon"_hash:
		case "M_ItemLegacy_0794"_hash:
		case "K_LPU_00"_hash:
			return LootCategory::MainWeapon;
		case "K_WDUM_00"_hash:
			return LootCategory::Tactical;
		case "Attachment"_hash:
		case "K_LPU_01"_hash:
		case "M_ItemLegacy_0039"_hash:
			return LootCategory::Attachment;
		case "Throwable"_hash:
		case "M_ItemLegacy_0049"_hash:
		case "K_LPU_02"_hash:
		case "K_IGU_KEYSC"_hash:
			return LootCategory::Throwable;
		case "Fuel"_hash:
		case "K_LPU_04"_hash:
			return LootCategory::Fuel;
		case "Armored Vest"_hash:
		case "K_LPU_05"_hash:
			return LootCategory::ArmoredVest;
		case "Backpack"_hash:
		case "K_LPU_06"_hash:
			return LootCategory::Backpack;
		case "Handgun"_hash:
		case "K_LPU_07"_hash:
		case "M_ItemLegacy_0796"_hash:
			return LootCategory::Handgun;
		case "Melee Weapon"_hash:
		case "K_LPU_08"_hash:
			return LootCategory::MeleeWeapon;
		case "Heal"_hash:
		case "K_LPU_10"_hash:
			return LootCategory::Heal;
		case "Boost"_hash:
		case "K_LPU_11"_hash:
			return LootCategory::Boost;
		case "Main"_hash:
			return LootCategory::Main;
		case "Ammunition"_hash:
		case "M_ItemLegacy_0040"_hash:
			return LootCategory::Ammuntion;
		case "Headgear"_hash:
			return LootCategory::Headgear;
		case "Jacket"_hash:
		case "M_ItemLegacy_0005"_hash:
		case "M_IteamLegacy_1226"_hash:
			return LootCategory::Jacket;
		case "Sub_Vehicle"_hash:
		case "K_LPU_12"_hash:
		case "K_IGU_077"_hash:
			return LootCategory::Special;
		default:
			return LootCategory::Unknown;
		}
	}

	std::string GetItemDescriptionsNamespace(const ItemCategory& itemCategory)
	{
		switch (itemCategory)
		{
		case ItemCategory::Weapon:
			return ("ST_Item_Weapon_ItemDescriptions|NS_WEAPONDESC");
		case ItemCategory::Attach:
			return ("ST_Attach_ItemDescriptions");
		case ItemCategory::Ammo:
			return ("ST_Item_Ammo_ItemDescriptions");
		case ItemCategory::Equipment:
			return ("ST_Item_Equipment_ItemDescriptions");
		case ItemCategory::Usebale:
			return ("ST_Item_Usable_ItemDescriptions");
		case ItemCategory::Jacket:
			return ("[14293F83437A460E239A7DAD94008A48]|NS_ItemLegacy");
		case ItemCategory::Special:
			return ("ST_Item_Usable_ItemDescriptions|NS_VEHICLE");
		case ItemCategory::Misc:
			return ("NS_Item_Usable_ItemDescriptions");
		case ItemCategory::Tactical:
			return ("NS_WEAPONDESC");
		default:
			break;
		}
		return "";
	}

	std::string GetItemNamespace(const ItemCategory& itemCategory, const std::string itemName)
	{
		switch (itemCategory)
		{
		case ItemCategory::Weapon:
			return ("ST_Item_Weapon_ItemNames|NS_WEAPONNAME");
		case ItemCategory::Attach:
			return ("ST_Item_Attach_ItemNames");
		case ItemCategory::Ammo:
			return ("ST_Item_Ammo_ItemName");
		case ItemCategory::Equipment:
			return ("ST_Item_Equipment_ItemNames");
		case ItemCategory::Usebale:
			return ("ST_Item_Usable_ItemNames");
		case ItemCategory::Jacket:
			return ("[14293F83437A460E239A7DAD94008A48]|NS_ItemLegacy");
		case ItemCategory::Special:
		{
			if (itemName == "Item_BulletproofShield_C")
				return ("NS_Item_Usable_ItemNames|NS_VEHICLE");
			else
				return ("ST_Item_Usable_ItemNames|NS_VEHICLE");
		}
		case ItemCategory::Misc:
			return ("NS_Item_Usable_ItemNames");
		case ItemCategory::Tactical:
			return ("NS_WEAPONNAME");
		default:
			break;
		}
		return "";
	}

	ItemManager* ItemManager::mInstance = nullptr;
	ItemManager* ItemManager::GetManager()
	{
		return mInstance == nullptr
			? mInstance = new ItemManager()
			: mInstance;
	}
	bool ItemManager::Initialize()
	{
		mSprites = new ItemSprites();
		mSprites2x = new ItemSprites();
		mItemDatabase = new ItemDatabase();

		bool result = true;
		const void* itemSprite = nullptr, * itemJson = nullptr,
			* itemSprite2x = nullptr, * itemJson2x = nullptr,
			* itemData = nullptr;

		size_t spriteSize, jsonSize,
			sprite2xSize, json2xSize,
			itemDataSize;

		if (AllItems.size())
			AllItems.clear();
		/// loadResource
		{
			spriteSize = Package::GetPackage()->FindFile(HASH("pubg-icon.png"), (void**)&itemSprite);
			jsonSize = Package::GetPackage()->FindFile(HASH("pubg-icon.paper2dsprites"), (void**)&itemJson);
			if (!itemSprite || !itemJson) result = false;

			sprite2xSize = Package::GetPackage()->FindFile(HASH("pubg-icon@2x.png"), (void**)&itemSprite2x);
			json2xSize = Package::GetPackage()->FindFile(HASH("pubg-icon@2x.paper2dsprites"), (void**)&itemJson2x);
			if (!itemSprite2x || !itemJson2x) result = false;

			itemDataSize = Package::GetPackage()->FindFile(HASH("itemdata.json"), (void**)&itemData);
			if (!itemData) result = false;
		}

		/// Create textures and json deserialization
		if (result)
		{

			mTexture.push_back(OverlayEngine::CreateTexture((const char*)itemSprite, spriteSize));
			mTexture.push_back(OverlayEngine::CreateTexture((const char*)itemSprite2x, sprite2xSize));

			JsonReader frameReader((const char*)itemJson, jsonSize);
			if (!mSprites->Deserialize(frameReader))
				result = false;

			JsonReader frame2xReader((const char*)itemJson2x, json2xSize);
			if (!mSprites2x->Deserialize(frame2xReader))
				result = false;

			JsonReader itemDataReader((const char*)itemData, itemDataSize);
			if (!mItemDatabase->Deserialize(itemDataReader))
				result = false;
		}

		if (!result)
		{
			return false;
		}
		size_t i = 0;
		for (const auto& item : mItemDatabase->items)
			for (const auto& itemData : item.second)
				i += item.second.size();


		/// 
		if (result)
		{
		//reSet:
			for (const auto& item : mItemDatabase->items)
			{
				auto itemCategory = GetItemCategory(item.first);
				if (AllItems.find(itemCategory) == AllItems.end())
				{
					AllItems.emplace(itemCategory, std::unordered_map <LootCategory, std::vector<Item*>>());
				}
				auto& items = AllItems[itemCategory];
				for (const auto& itemData : item.second)
				{
					auto lootCategory = GetLootCategory(itemData.category);

					if (items.find(lootCategory) == items.end())
						items.emplace(lootCategory, std::vector<Item*>());

					Item* item = new Item();
					item->id = itemData.id;
					if (itemData.gname == "Item_BulletproofShield_C")
					{
						item->lootHashName = hash_("K_UXU_0111");
						item->lootCategory = lootCategory;
						SaveNameBind.emplace(hash_(itemData.gname), NameBind("K_UXU_0111", itemData.category));
					}
					else
					{
						item->lootHashName = hash_(itemData.name);
						item->lootCategory = lootCategory;
						SaveNameBind.emplace(hash_(itemData.gname), NameBind(itemData.name, itemData.category));
					}

					if (!itemData.name.empty())
					{
						item->name_cnsimple = itemData.cnsimple;
						item->name_cntradit = itemData.cntradit;
						item->name_english = itemData.english;
						item->name_japanese = itemData.japanese;
						item->name_korean = itemData.korean;
						//sdk::LocalizationPrepare(GetItemNamespace(itemCategory, itemData.gname), itemData.name, item->name.data());
					}

					//CONSOLE_INFO("%d|%s|%s", item->id, itemData.name.c_str(), item->name.c_str());

					if (!mSprites)
						continue;
					auto spriteSize = mSprites->meta.size;
					auto frame = mSprites->frames[itemData.icon];
					if (frame)
					{
						const ImVec2 size(frame.frame.w, frame.frame.h);
						const ImVec2 minRegion(frame.frame.x, frame.frame.y);
						const ImVec2 maxRegion(minRegion + size);


						item->icon.push_back({ size ,
							ImVec2(minRegion.x / spriteSize.w, minRegion.y / spriteSize.h) ,
							ImVec2(maxRegion.x / spriteSize.w, maxRegion.y / spriteSize.h) });

					}
					if (!mSprites2x)
						continue;
					spriteSize = mSprites2x->meta.size;
					auto frame2x = mSprites2x->frames[itemData.icon];
					if (frame2x)
					{
						const ImVec2 size(frame2x.frame.w, frame2x.frame.h);
						const ImVec2 minRegion(frame2x.frame.x, frame2x.frame.y);
						const ImVec2 maxRegion(minRegion + size);

						item->icon.push_back({ size ,
							ImVec2(minRegion.x / spriteSize.w, minRegion.y / spriteSize.h) ,
							ImVec2(maxRegion.x / spriteSize.w, maxRegion.y / spriteSize.h) });
					}
					items[lootCategory].push_back(item);
				}
			}
		}
		//if (!sdk::Localization())
		//{
		//	CONSOLE_INFO("Item Data Reset");
		//	goto reSet;
		//}
		/// release
		{
			if (itemSprite) { memset((void*)itemSprite, 0, spriteSize); free((void*)itemSprite); }
			if (itemJson) { memset((void*)itemJson, 0, jsonSize); free((void*)itemJson); }
			if (itemSprite2x) { memset((void*)itemSprite2x, 0, sprite2xSize); free((void*)itemSprite2x); }
			if (itemJson2x) { memset((void*)itemJson2x, 0, json2xSize); free((void*)itemJson2x); }
			if (itemData) { memset((void*)itemData, 0, itemDataSize); free((void*)itemData); }

			if (mSprites)
				delete mSprites;
			if (mSprites2x)
				delete mSprites2x;
			if (mItemDatabase)
				delete mItemDatabase;
		}
		LoadConfig();
		//SaveConfig();
		return mLoaded = result;
	}

	bool ItemManager::LoadConfig(bool default1)
	{
		GetGroupManager()->DeleteAll();
		void* json;
		size_t jsonSize = Package::GetPackage()->FindFile(HASH("ItemDefaultConfig.ini"), &json);
		JsonReader reader((const char*)json, jsonSize);
		auto gm = GetGroupManager();
		if (!gm->Deserialize(reader))
			return false;

		int addIndex = 0;
		for (auto& group : gm->GetAllGroup())
		{
			for (const auto& id : group.tids)
			{

				auto item = ItemManager::GetItemById(id);
				if (item)
				{
					item->group = &group;
					item->groupId = group.gid;
					item->addTime = ++addIndex;
					gm->GetLoots()[item->lootCategory].emplace(item->lootHashName, item);
				}
			}
		}
		return true;
	}

	bool ItemManager::SaveConfig()
	{
		//auto gm = GetGroupManager();
		//for (auto& group : gm->GetAllGroup())
		//{
		//	group.tids.clear();
		//}


		//cpplinq::from(gm->GetLoots())
		//	>> cpplinq::select_many([](auto& o) {return cpplinq::from(o.second) >> cpplinq::ref(); })
		//	>> cpplinq::select([](auto& o) {return o.get().second; })
		//	>> cpplinq::where([](auto&& o) {return o != nullptr; })
		//	>> cpplinq::orderby([](auto&& o) {return o->addTime; })
		//	>> cpplinq::for_each([&](auto& item) {
		//	if (item->groupId > 0)
		//		gm->GetGroupByGid(item->groupId).tids.push_back(item->id);
		//		});

		//JsonWriter writer;
		//if (!gm->Serialize(writer))
		//	return false;

		//std::ofstream ofs(itemConfigPath);
		//ofs.write(writer.GetString(), writer.GetStringLength());
		//ofs.close();
		return true;
	}

	std::string ItemManager::GetCategoryName(ItemCategory category, bool english)
	{
		switch (category)
		{
		case ItemCategory::Weapon:
			return english ? ("Weapon") : (u8"武器");
		case ItemCategory::Attach:
			return english ? ("Attachment") : (u8"配件");
		case ItemCategory::Ammo:
			return english ? ("Ammunition") : (u8"彈藥");
		case ItemCategory::Equipment:
			return english ? ("Equipment") : (u8"裝備");
		case ItemCategory::Usebale:
			return english ? ("Useable") : ("使用");
		case ItemCategory::Jacket:
			return english ? ("Armed") : ("夾克");
		default:
			return ("Unknown");
		}
	}

	Item* ItemManager::GetItemById(int tid)
	{
		return cpplinq::from(GetAllItems())
			>> cpplinq::select_many([](auto&& o) {return cpplinq::from(o.second) >> cpplinq::ref(); })
			>> cpplinq::select_many([](auto&& o) {return cpplinq::from(o.get().second); })
			>> cpplinq::first_or_default([tid](auto&& o) {
			return o && o->id == tid;
				});
	}


	ItemManager::ItemManager()
		:mLoaded(false),
		AllItems(),
		mTexture(),
		LogoTexture(),
		mSprites(),
		mSprites2x(),
		mItemDatabase()
	{
		mGroupManager = new ItemGroups();
	}

	ItemManager::~ItemManager()
	{
		for (const auto& items : AllItems)
		{
			for (auto item : items.second)
			{
				for (auto loot : item.second)
				{
					delete loot;
				}
			}
		}

		if (mGroupManager)
			delete mGroupManager;

	}

	std::string ItemGroups::GenGroupName()
	{
		auto groupNum = 0;
		auto result = std::find_if(mGroups.rbegin(), mGroups.rend(), [](const auto& group) {return group.name.find("Group ") != group.name.npos; });
		if (result != mGroups.rend())
		{
			groupNum = atoi(result->name.substr(result->name.rfind(' ') + 1).c_str());
			++groupNum;
		}
		return Utils::FormatString("Group %d", groupNum);
	}
}