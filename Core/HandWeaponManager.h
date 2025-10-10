#include "Core/DirectX/DirectX11.h"
#include "ThirdParty/cpplinq.hpp"
#include "Helper/Misc/JsonHelper.h"
#include <unordered_map>
#include <map>

namespace IronMan::Core
{
	class HandWeaponSprites
	{
	public:
		HandWeaponSprites() :frames(), meta()
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
				:size(), scale()
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

	struct HandWeaponDatabase
	{
	public:
		struct HandWeapon
		{
			JsonReader& Deserialize(JsonReader& ar)
			{
				ar.StartObject();

				ar.Member(("names"));
				ar& names_;

				std::string NumberOfAttach;
				ar.Member(("numattach"));
				ar& NumberOfAttach;
				NumAttach_ = atoi(NumberOfAttach.c_str());

				ar.Member(("icons"));
				size_t iconsSize;
				ar.StartArray(&iconsSize);
				icons_.resize(iconsSize);
				for (int i = 0; i < iconsSize; i++)
				{
					std::string icon;
					ar& icon;
					icons_[i] = hash_(icon.c_str());
				}
				ar.EndArray();

				ar.Member("attach");
				size_t attachSize;
				ar.StartArray(&attachSize);
				LootId_.resize(attachSize);
				for (int i = 0; i < attachSize; i++)
				{
					std::string attachment;
					ar& attachment;
					LootId_[i] = atoi(attachment.c_str());
				}

				ar.EndArray();

				return ar.EndObject();
			}

			std::string names_;
			int NumAttach_;
			std::vector<hash_t> icons_;
			std::vector<int> LootId_;
		};

		//std::string version;
		std::vector<HandWeapon> handweapon;

		JsonReader& Deserialize(JsonReader& ar)
		{
			ar.StartObject();
			//ar.Member(("version")) & version;

			ar.Member(("handweapon"));
			size_t handweaponsSize;
			ar.StartArray(&handweaponsSize);
			handweapon.resize(handweaponsSize);

			for (int i = 0; i < handweaponsSize; i++)
				handweapon[i].Deserialize(ar);
			ar.EndArray();
			return ar.EndObject();
		}
	};
	struct HandWeapon
	{
		struct IconInfo
		{
			ImVec2 iconSize = { 0,0 };
			ImVec2 uv0 = { 0,0 };
			ImVec2 uv1 = { 0,0 };
		};
		int AttachNum;
		std::vector<IconInfo> icon;
		std::unordered_map<int, int> AttachAble;
	};

	class HandWeaponManager
	{
	public:
		static HandWeaponManager* GetManager();
		bool Load();
		inline static HandWeapon* GetHandWeapon(hash_t name) {
			const auto manager = GetManager();
			auto handweapon = manager->mHandWeapons.find(name);
			if (handweapon != manager->mHandWeapons.end())
				return handweapon->second;
			return nullptr;
		}
		inline static auto GetTexture() { return GetManager()->mTexture; }
	private:
		HandWeaponManager();
		~HandWeaponManager();
	private:
		static HandWeaponManager* mInstance;
		bool mLoaded;
		HandWeaponSprites* mSprites;
		std::vector<ImTextureID> mTexture;
		HandWeaponDatabase* mHandWeaponDatabase;
		std::unordered_map<hash_t, HandWeapon*> mHandWeapons;
	};
}