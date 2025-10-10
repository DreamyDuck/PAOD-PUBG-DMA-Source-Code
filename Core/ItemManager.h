#pragma once
#include "Core/DirectX/DirectX11.h"
#include "Core/DirectX/Drawing.h"
#include "Helper/Misc/JsonHelper.h"
#include "ThirdParty/cpplinq.hpp"
#include <unordered_map>
#include <map>
namespace IronMan::Core
{
	struct NameBind
	{
		std::string ItemName;
		std::string ItemCategory;
		inline NameBind(std::string s, std::string b)
		{
			ItemName = s;
			ItemCategory = b;
		}
	};

	enum class ItemCategory
	{
		Weapon,
		Attach,
		Ammo,
		Equipment,
		Usebale,
		Jacket,
		Special,
		Tactical,
		Misc,
		Unknown,
		Max
	};
	enum class LootCategory
	{
		Main,
		MainWeapon,
		Handgun,
		MeleeWeapon,
		Throwable,
		Attachment,
		Ammuntion,
		Headgear,
		ArmoredVest,
		Backpack,
		Heal,
		Boost,
		Fuel,
		Jacket,
		Special,
		Tactical,
		Misc,
		Unknown
	};

	ItemCategory GetItemCategory(std::string categoryString);
	LootCategory GetLootCategory(std::string categoryString);
	std::string GetItemDescriptionsNamespace(const ItemCategory& itemCategory);
	std::string GetItemNamespace(const ItemCategory& itemCategory, const std::string itemName);


	class ItemSprites
	{
	public:
		ItemSprites() :frames(), meta()
		{
		}


		struct Pivot
		{
			Pivot() :x(), y() {}
			Pivot(float x, float y) :x(x), y(y) {}

			JsonReader& Deserialize(JsonReader& ar)
			{
				ar.StartObject();
				ar.Member("x")& x;
				ar.Member("y")& y;
				return ar.EndObject();
			}
			float x;
			float y;
		};
		struct Size
		{
			Size() :w(), h() {}
			Size(float w, float h) :w(w), h(h) {}
			JsonReader& Deserialize(JsonReader& ar)
			{
				ar.StartObject();
				ar.Member("w")& w;
				ar.Member("h")& h;
				return ar.EndObject();
			}
			float w;
			float h;
		};
		struct Rect
		{
			Rect() :x(), y(), w(), h() {}
			Rect(float x, float y, float w, float h) :x(x), y(y), w(w), h(h) {}
			JsonReader& Deserialize(JsonReader& ar)
			{
				ar.StartObject();
				ar.Member("x")& x;
				ar.Member("y")& y;
				ar.Member("w")& w;
				ar.Member("h")& h;
				return ar.EndObject();
			}
			float x;
			float y;
			float w;
			float h;
		};
		class Sprite
		{
		public:
			Sprite()
				:frame(), rotated(), trimmed(), spriteSourceSize(), sourceSize(), pivot(), initialized(false)
			{

			}
			Sprite(std::string name, Rect frame, bool rotated, bool trimmed, Rect spriteSourceSize, Size sourceSize, Pivot pivot)
				:frame(frame), rotated(rotated), trimmed(trimmed), spriteSourceSize(spriteSourceSize), sourceSize(sourceSize), pivot(pivot), initialized(false)
			{
			}
			JsonReader& Deserialize(JsonReader& ar)
			{
				ar.StartObject();


				if (ar.Member(("frame")))
					frame.Deserialize(ar);

				ar.Member(("rotated"))& rotated;
				ar.Member(("trimmed"))& trimmed;

				if (ar.HasMember(("spriteSourceSize")))
					spriteSourceSize.Deserialize(ar);

				if (ar.HasMember(("sourceSize")))
					sourceSize.Deserialize(ar);

				if (ar.HasMember(("pivot")))
					pivot.Deserialize(ar);

				if (ar)	initialized = true;


				return ar.EndObject();
			}
			operator bool() const { return initialized; }

			Rect frame;
			bool rotated;
			bool trimmed;
			Rect spriteSourceSize;
			Size sourceSize;
			Pivot pivot;
		private:
			bool initialized;
		};
		class Meta
		{
		public:
			Meta()
				:scale()
			{
			}
			Meta(Size size, std::string scale)
				:size(size), scale(scale)
			{
			}

			JsonReader& Deserialize(JsonReader& ar)
			{
				ar.Member(("meta"));
				ar.StartObject();
				//ar.Member(("version")) & version;
				//ar.Member(("target")) & target;
				//ar.Member(("image")) & image;
				//ar.Member(("format")) & format;

				ar.Member(("size"));
				size.Deserialize(ar);

				ar.Member(("scale"))& scale;
				//ar.Member(("smartupdate")) & smartupdate;
				return ar.EndObject();
			}
			//std::string version;
			//std::string target;
			//std::string image;
			//std::string format;
			Size size;
			std::string scale;
			//std::string smartupdate;
		};

		JsonReader& Deserialize(JsonReader& ar)
		{
			frames.clear();

			ar.StartObject();
			ar.Member(("frames"));
			size_t frameCount = 0;
			ar.StartObject(&frameCount);
			for (int i = 0; i < frameCount; i++)
			{
				std::string objectName;
				ar.Member(i, objectName);
				if (!objectName.empty())
					frames[hash_(objectName.c_str())].Deserialize(ar);
			}
			ar.EndObject();
			meta.Deserialize(ar);
			return ar.EndObject();
		}
		std::unordered_map<hash_t, Sprite> frames;
		Meta meta;
	};
	class ItemData
	{
	public:
		ItemData() :id(), name(), gname(), cnsimple(), cntradit(), english(), japanese(), korean(), category(), icon() {

		}
		int id;
		std::string name;
		std::string category;
		std::string gname;
		std::string cnsimple;
		std::string cntradit;
		std::string english;
		std::string japanese;
		std::string korean;

		hash_t icon;

		JsonReader& Deserialize(JsonReader& ar)
		{
			ar.StartObject();
			if (ar.HasMember(("id")))
				ar& id;
			if (ar.HasMember(("name")))
				ar& name;
			if (ar.HasMember(("gname")))
				ar& gname;
			if (ar.HasMember(("cnsimple")))
				ar& cnsimple;
			if (ar.HasMember(("cntradit")))
				ar& cntradit;
			if (ar.HasMember(("english")))
				ar& english;
			if (ar.HasMember(("japanese")))
				ar& japanese;
			if (ar.HasMember(("korean")))
				ar& korean;
			if (ar.HasMember(("category")))
				ar& category;
			if (ar.HasMember(("icon")))
			{
				std::string iconstr;
				ar& iconstr;
				icon = hash_(iconstr.c_str());
			}

			return ar.EndObject();
		}
	};
	class ItemDatabase
	{
	public:
		ItemDatabase() :version(), itemCategory(), items()
		{
		}

		~ItemDatabase()
		{
			version.clear();
			itemCategory.clear();
			items.clear();
		}

		std::string version;
		std::vector<std::string> itemCategory;
		std::unordered_map<std::string, std::vector<ItemData>> items;

		JsonReader& Deserialize(JsonReader& ar)
		{
			itemCategory.clear();
			items.clear();
			ar.StartObject();
			ar.Member(("version"))& version;

			ar.Member(("itemCategory"));
			size_t categorySize;
			ar.StartArray(&categorySize);
			itemCategory.resize(categorySize);

			for (int i = 0; i < categorySize; i++)
				ar& itemCategory[i];
			ar.EndArray();

			ar.Member(("items"));
			ar.StartObject();
			for (const auto& category : itemCategory)
			{
				size_t itemSize;
				if (ar.HasMember(category.c_str()))
				{
					ar.StartArray(&itemSize);
					items[category.c_str()].resize(itemSize);
					for (int i = 0; i < itemSize; i++)
						items[category.c_str()][i].Deserialize(ar);
					ar.EndArray();
				}
			}
			ar.EndObject();
			return ar.EndObject();
		}
	};
	class Item
	{
	public:
		Item()
			: id(), name_cnsimple(), detailedName(), description(), icon(),
			addTime(0), groupId(0), lastGroupId(0), group(), name_cntradit(),
			name_english(), name_japanese(), name_korean(),lootCategory(),lootHashName()
		{
		}
		struct IconInfo
		{
			ImVec2 iconSize;
			ImVec2 uv0;
			ImVec2 uv1;
		};

		inline const struct Group* Item::GetGroup();

		int id;
		std::string name_cnsimple;
		std::string name_cntradit;
		std::string name_english;
		std::string name_japanese;
		std::string name_korean;
		std::string detailedName;
		std::string description;
		LootCategory lootCategory;
		hash_t lootHashName;
		/*std::string lootName;
		std::string lootDetailedName;
		std::string lootDescription;*/

		hash_t  groupId;
		hash_t  lastGroupId;
		double  addTime;
		struct Group* group;
		std::vector<IconInfo> icon;
	};


	struct Group
	{
	public:
		Group() :gid(), name(), color(), opened(), enable() {}
		Group(std::string _name, ImColor _color)
			:name(_name), color(_color), opened(true), enable(true), gid(hash_(_name))
		{
			CONSOLE_INFO("New Group: gid:%lld", gid);
		}

		JsonReader& Deserialize(JsonReader& ar, int index)
		{
			std::string sgid;
			ar.Member(index, sgid);
			gid = strtoull(sgid.c_str(), nullptr, 10);

			ar.StartObject();
			{
				//ar.Member(HASH("name")) & name;
				ar.Member((hash_t)0x2F98ED37466E3996)& name;
				ImU32 col;
				//ar.Member(HASH("color")) & col;
				ar.Member((hash_t)0x428BD27C1A08BA08)& col;
				//ar.Member(HASH("opened")) & opened;
				ar.Member((hash_t)0x7A183335508841B0)& opened;
				//ar.Member(HASH("enable")) & enable;
				ar.Member((hash_t)0x57BF8BBF79DF205E)& enable;

				color = ImColor(col);

				//ar.Member(HASH("tids"));
				ar.Member((hash_t)0xE151FA5C38C22C57);
				size_t tidsCount = 0;
				ar.StartArray(&tidsCount);
				tids.resize(tidsCount);
				for (int i = 0; i < tidsCount; i++)
					ar& tids[i];
				ar.EndArray();

			}
			ar.EndObject();
			return ar;
		}
		JsonWriter& Serialize(JsonWriter& ar)
		{
			ar.Member(gid);
			ar.StartObject();
			{
				//ar.Member(HASH("name")) & name;
				ar.Member((hash_t)0x2F98ED37466E3996)& name;
				ImU32 col = color;
				//ar.Member(HASH("color")) & col;
				ar.Member((hash_t)0x428BD27C1A08BA08)& col;
				//ar.Member(HASH("opened"))& opened;
				ar.Member((hash_t)0x7A183335508841B0)& opened;
				//ar.Member(HASH("enable"))& enable;
				ar.Member((hash_t)0x57BF8BBF79DF205E)& enable;
				//ar.Member(HASH("tids"));
				ar.Member((hash_t)0xE151FA5C38C22C57);
				ar.StartArray();
				for (int i = 0; i < tids.size(); i++)
					ar& tids[i];
				ar.EndArray();
			}
			ar.EndObject();
			return ar;
		}

	public:
		hash_t gid;
		std::string name;
		ImColor color;
		bool enable;
		bool opened;
		std::vector<int> tids;
	};

	class ItemGroups
	{
	public:
		ItemGroups() :
			mCurrentEditItem(-1),
			mGroups(), bDontAskMeDelete(false), bDontAskMeDeleteAll(false)
		{
			for (int n = 0; n < IM_ARRAYSIZE(TempSavedPalette); n++)
			{
				ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f, TempSavedPalette[n].x, TempSavedPalette[n].y, TempSavedPalette[n].z);
				TempSavedPalette[n].w = 1.0f; // Alpha
			}
		}
	public:
		JsonReader& Deserialize(JsonReader& ar)
		{
			mGroups.clear();
			ar.StartObject();
			{
				//ar.Member(HASH("groups"));
				ar.Member((hash_t)0x0B55804DABA4BFDD);
				size_t gourpCount = 0;
				ar.StartObject(&gourpCount);
				mGroups.resize(gourpCount);
				for (int i = 0; i < gourpCount; i++)
					mGroups[i].Deserialize(ar, i);

				ar.EndObject();
			}
			ar.EndObject();
			return ar;
		}

		JsonWriter& Serialize(JsonWriter& ar)
		{
			ar.StartObject();
			{
				//ar.Member(HASH("groups"));
				ar.Member((hash_t)0x0B55804DABA4BFDD);
				ar.StartObject();
				for (int i = 0; i < GetGroupCount(); i++)
					mGroups[i].Serialize(ar);


				ar.EndObject();
			}
			ar.EndObject();
			return ar;
		}

	public:
		void AddGroup(std::string name, ImColor color)
		{
			if (!name.empty())
				mGroups.push_back(Group(name, color));
		}
		void DeleteGroup(int index)
		{
			for (auto cate = GetLoots().begin(); cate != GetLoots().end(); )
			{
				for (auto loot = cate->second.begin(); loot != cate->second.end();)
				{
					if (loot->second && loot->second->groupId == mGroups[index].gid)
					{
						loot->second->addTime = 0.0f;
						loot->second->groupId = 0;
						loot->second->lastGroupId = 0;
						loot->second->group = nullptr;
						loot = cate->second.erase(loot);
					}
					else
						loot++;

				}
				if (cate->second.empty())
					cate = GetLoots().erase(cate);
				else
					cate++;

			}

			if (index >= 0 && index < GetGroupCount())
				mGroups.erase(mGroups.begin() + index);

		}
		void DeleteLootByPtr(Item* item)
		{
			for (auto cate = GetLoots().begin(); cate != GetLoots().end(); )
			{
				for (auto loot = cate->second.begin(); loot != cate->second.end();)
				{
					if (loot->second && loot->second == item)
					{
						loot->second->addTime = 0.0f;
						loot->second->groupId = 0;
						loot->second->lastGroupId = 0;
						loot->second->group = nullptr;
						loot = cate->second.erase(loot);
					}
					else
						loot++;

				}
				if (cate->second.empty())
					cate = GetLoots().erase(cate);
				else
					cate++;

			}
		}

		void DeleteAll()
		{
			cpplinq::from(GetLoots())
				>> cpplinq::select_many([](auto& o) {return cpplinq::from(std::move(o.second)) >> cpplinq::ref(); })
				>> cpplinq::select([](auto& o) {return o.get().second; })
				>> cpplinq::where([](auto& o) {return o != nullptr; })
				>> cpplinq::for_each([&](auto& item)
					{
						item->group = nullptr;
						item->groupId = 0;
						item->lastGroupId = 0;
						item->addTime = 0.0;
					});
			GetLoots().clear();
			mGroups.clear();

		}
		void EditGroup(int index, std::string name, ImColor color)
		{
			if (index >= 0 && index < GetGroupCount())
			{
				auto& group = mGroups[index];
				if (name.empty())
					group.color = color;
				else
				{
					auto oldGroupId = group.gid;
					group.name = name.length() > 20 ? name.substr(0, 20) : name;
					group.color = color;
					group.gid = hash_(group.name);

					cpplinq::from(GetLoots())
						>> cpplinq::select_many([](auto& o) {return cpplinq::from(std::move(o.second)) >> cpplinq::ref(); })
						>> cpplinq::select([](auto& o) {return o.get().second; })
						>> cpplinq::where([&oldGroupId](auto& o) {return o != nullptr && o->groupId == oldGroupId; })
						>> cpplinq::for_each([&](auto& item)
							{
								item->groupId = group.gid;
								item->group = nullptr;
							});


				}
			}
		}

		bool GroupIsEnabled(int index)
		{
			if (index >= 0 && index < GetGroupCount())
				return mGroups[index].enable;

			return false;
		}

		void EnableGroup(int index, bool enable)
		{
			if (index >= 0 && index < GetGroupCount())
				mGroups[index].enable = enable;

		}

		void EnableGroup(int index)
		{
			if (index >= 0 && index < GetGroupCount())
				mGroups[index].enable = !mGroups[index].enable;
		}

		std::string GenGroupName();
		size_t GetGroupCount()const { return mGroups.size(); }
		std::vector<Group>& GetAllGroup() { return mGroups; }
		Group& GetGroupByIndex(int index) { return mGroups[index]; }
		Group& GetGroupByGid(hash_t gid) {

			static Group empty;

			auto result = std::find_if(mGroups.begin(), mGroups.end(), [gid](const auto& group) {return group.gid == gid; });
			return result != mGroups.end() ? *result : empty;
		}
		Group GetLastGroup() { return mGroups.back(); }
		bool HasGroupName(const std::string& name)
		{
			return std::find_if(mGroups.begin(), mGroups.end(), [&name](const auto& group) {return group.name == name; }) != mGroups.end();
		}

		std::unordered_map<LootCategory, std::unordered_map<hash_t, Item*>>& GetLoots() { return mLoots; }


	public:
		bool bDontAskMeDelete;
		bool bDontAskMeDeleteAll;
		std::string TempOldName;
		std::string TempNewName;
		ImVec4		TempNewColor;
		std::string TempOldEditName;
		std::string TempEditName;
		ImVec4 TempEditColor;
		ImVec4 TempBackupColor;
		ImVec4 TempSavedPalette[32];
	private:
		int32_t mCurrentEditItem;
		std::vector<Group> mGroups;
		std::unordered_map<LootCategory, std::unordered_map<hash_t, Item*>> mLoots;

	};

	class ItemManager
	{
	public:
		static ItemManager* GetManager();
		bool Initialize();
		static bool LoadConfig(bool default1 = false);
		static bool SaveConfig();
		static std::string GetCategoryName(ItemCategory category, bool english);
		inline bool IsLoaded()const { return mLoaded; }
		static Item* GetItemById(int tid);
		inline static auto& GetAllItems() { return GetManager()->AllItems; }
		inline static auto& GetGroupManager() { return GetManager()->mGroupManager; }
		inline static auto GetTexture() { return GetManager()->mTexture; }
		inline static auto FindItemAllInfo(hash_t Hash)
		{
			auto FindResult = GetManager()->SaveNameBind.find(Hash);
			if (FindResult == GetManager()->SaveNameBind.end())
				return NameBind("", "");
			return FindResult->second;
		}
	private:
		ItemManager();
		~ItemManager();
	private:
		static ItemManager* mInstance;
		bool mLoaded;
		ItemSprites* mSprites;
		ItemSprites* mSprites2x;
		//ItemSprites* mSprites4x;
		ItemDatabase* mItemDatabase;
		std::vector<ImTextureID> mTexture;
		ImTextureID LogoTexture;
		std::unordered_map<ItemCategory, std::unordered_map<LootCategory, std::vector<Item*>>> AllItems;
		ItemGroups* mGroupManager;
		std::unordered_map<hash_t, NameBind>SaveNameBind;

	};

	inline const struct Group* Item::GetGroup()
	{
		if (groupId == 0)
		{
			return nullptr;
		}

		if (group == nullptr)
			group = &ItemManager::GetGroupManager()->GetGroupByGid(groupId);


		return group;
	};
}