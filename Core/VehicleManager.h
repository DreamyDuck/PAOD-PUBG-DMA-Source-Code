#pragma once
#include "Core/DirectX/DirectX11.h"
#include "ThirdParty/cpplinq.hpp"
#include "Helper/Misc/JsonHelper.h"
#include <unordered_map>
#include <map>
namespace IronMan::Core
{

	class VehicleSprites
	{
	public:
		VehicleSprites() :frames(), meta()
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

	struct VehicleDatabase
	{
	public:
		struct vehicle
		{
			JsonReader& Deserialize(JsonReader& ar)
			{
				ar.StartObject();

				ar.Member(("names"));
				size_t namesSize;
				ar.StartArray(&namesSize);
				names_.resize(namesSize);
				for (int i = 0; i < namesSize; i++)
					ar& names_[i];
				ar.EndArray();

				ar.Member(("cnsimple"));
				ar& cnsimple_;

				ar.Member(("cntradit"));
				ar& cntradit_;

				ar.Member(("english"));
				ar& english_;

				ar.Member(("japanese"));
				ar& japanese_;

				ar.Member(("korean"));
				ar& korean_;

				ar.Member("icons");
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

				return ar.EndObject();
			}

			std::vector<std::string> names_;
			std::string cnsimple_;
			std::string cntradit_;
			std::string english_;
			std::string japanese_;
			std::string korean_;
			std::vector<hash_t> icons_;
		};

		//std::string version;
		std::vector<vehicle> vehicles;

		JsonReader& Deserialize(JsonReader& ar)
		{
			ar.StartObject();
			//ar.Member(("version")) & version;

			ar.Member(("vehicles"));
			size_t vehiclesSize;
			ar.StartArray(&vehiclesSize);
			vehicles.resize(vehiclesSize);

			for (int i = 0; i < vehiclesSize; i++)
				vehicles[i].Deserialize(ar);
			ar.EndArray();
			return ar.EndObject();
		}
	};
	struct Vehicle
	{
		struct IconInfo
		{
			ImVec2 iconSize;
			ImVec2 uv0;
			ImVec2 uv1;
		};

		std::string name_cnsimple;
		std::string name_cntradit;
		std::string name_english;
		std::string name_japanese;
		std::string name_korean;
		std::vector<IconInfo> icon;
	};

	class VehicleManager
	{
	public:
		static VehicleManager* GetManager();
		bool Load();
		inline static Vehicle* GetVehicle(hash_t name) {
			const auto manager = GetManager();
			auto vehicle = manager->mVehicles.find(name);
			if (vehicle != manager->mVehicles.end())
				return vehicle->second;
			return nullptr;
		}
		inline static auto GetTexture() { return GetManager()->mTexture; }
	private:
		VehicleManager();
		~VehicleManager();
	private:
		static VehicleManager* mInstance;
		bool mLoaded;
		VehicleSprites* mSprites;
		std::vector<ImTextureID> mTexture;
		VehicleDatabase* mVehicleDatabase;
		std::unordered_map<hash_t, Vehicle*> mVehicles;
	};

}