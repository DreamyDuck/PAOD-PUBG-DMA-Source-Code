#include "Core/DirectX/DirectX11.h"
#include "Core/DirectX/DirectX11.h"
#include "ThirdParty/cpplinq.hpp"
#include "Helper/Misc/JsonHelper.h"
#include <unordered_map>
#include <map>

namespace IronMan::Core
{
	class SkinWeaponSprites
	{
	public:
		SkinWeaponSprites() :frames(), meta()
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

	struct SkinWeaponDatabase
	{
	public:
		struct SkinWeapon
		{
			JsonReader& Deserialize(JsonReader& ar)
			{
				ar.StartObject();

				ar.Member(("names"));
				ar& names_;

				ar.Member(("namespace"));
				ar& namespace_;

				ar.Member(("key"));
				ar& key_;

				return ar.EndObject();
			}

			std::string names_;
			std::string namespace_;
			std::string key_;
		};

		//std::string version;
		std::vector<SkinWeapon> skinweapon;

		JsonReader& Deserialize(JsonReader& ar)
		{
			ar.StartObject();
			//ar.Member(("version")) & version;

			ar.Member(("weaponskin"));
			size_t skinweaponsSize;
			ar.StartArray(&skinweaponsSize);
			skinweapon.resize(skinweaponsSize);

			for (int i = 0; i < skinweaponsSize; i++)
				skinweapon[i].Deserialize(ar);
			ar.EndArray();
			return ar.EndObject();
		}
	};
	struct SkinWeapon
	{
		std::string names_;
		std::string namespace_;
		std::string key_;
		std::string formatName_;
	};

	class SkinWeaponManager
	{
	public:
		static SkinWeaponManager* GetManager();
		bool Load();
		inline static SkinWeapon* GetHandWeapon(hash_t name) {
			const auto manager = GetManager();
			auto handweapon = manager->mSkinWeapons.find(name);
			if (handweapon != manager->mSkinWeapons.end())
				return handweapon->second;
			return nullptr;
		}
	private:
		SkinWeaponManager();
		~SkinWeaponManager();
	private:
		static SkinWeaponManager* mInstance;
		bool mLoaded;
		SkinWeaponSprites* mSprites;
		SkinWeaponDatabase* mSkinWeaponDatabase;
		std::unordered_map<hash_t, SkinWeapon*> mSkinWeapons;
	};
}