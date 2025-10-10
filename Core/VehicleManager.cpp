#include "Include/WinHeaders.hpp"
#include "VehicleManager.h"
#include "FilePak.h"
#include "SDK/SDK.h"



namespace IronMan::Core
{
	using namespace SDK;

	VehicleManager* VehicleManager::mInstance = nullptr;
	VehicleManager* VehicleManager::GetManager()
	{
		return mInstance == nullptr
			? mInstance = new VehicleManager()
			: mInstance;
	}
	bool VehicleManager::Load()
	{
		bool result = true;
		mSprites = new VehicleSprites();
		mVehicleDatabase = new VehicleDatabase();
		const void* vehcileSprite = nullptr, * vehcileJson = nullptr,
			* vehcileData = nullptr;

		size_t spriteSize, jsonSize,
			vehcileDataSize;
		if (mVehicles.size())
			mVehicles.clear();
		/// loadResource
		{
			spriteSize = Package::GetPackage()->FindFile(HASH("vehicles.png"), (void**)&vehcileSprite);
			jsonSize = Package::GetPackage()->FindFile(HASH("vehicles.paper2dsprites"), (void**)&vehcileJson);
			if (!vehcileSprite || !vehcileJson) result = false;

			vehcileDataSize = Package::GetPackage()->FindFile(HASH("vehicle.json"), (void**)&vehcileData);
			if (!vehcileData) result = false;

		}

		/// Create textures and json deserialization
		if (result)
		{

			mTexture.push_back(OverlayEngine::CreateTexture((const char*)vehcileSprite, spriteSize));

			JsonReader frameReader((const char*)vehcileJson, jsonSize);
			if (!mSprites->Deserialize(frameReader))
				result = false;

			JsonReader vehicleDataReader((const char*)vehcileData, vehcileDataSize);
			if (!mVehicleDatabase->Deserialize(vehicleDataReader))
				result = false;

		}

		/// 
		if (result)
		{
		//reSet:
			for (const auto& vehcile : mVehicleDatabase->vehicles)
			{
				for (int i = 0; i < vehcile.names_.size(); i++)
				{
					Vehicle* v = new Vehicle;
					hash_t icon;

					if (i == 0 || vehcile.icons_.size() == 1)
						icon = vehcile.icons_.front();
					else if (i < vehcile.icons_.size())
						icon = vehcile.icons_[i];
					else
						icon = vehcile.icons_.back();

					v->name_cnsimple = vehcile.cnsimple_;
					v->name_cntradit = vehcile.cntradit_;
					v->name_english = vehcile.english_;
					v->name_japanese = vehcile.japanese_;
					v->name_korean = vehcile.korean_;
					//sdk::LocalizationPrepare(vehcile.namespace_, vehcile.key_, v->name.data());

					if (!mSprites)
						continue;
					auto spriteSize = mSprites->meta.size;
					auto frame = mSprites->frames[icon];
					if (frame)
					{
						const ImVec2 size(frame.frame.w, frame.frame.h);
						const ImVec2 minRegion(frame.frame.x, frame.frame.y);
						const ImVec2 maxRegion(minRegion + size);

						v->icon.push_back({ size ,
							ImVec2(minRegion.x / spriteSize.w, minRegion.y / spriteSize.h) ,
							ImVec2(maxRegion.x / spriteSize.w, maxRegion.y / spriteSize.h) });
					}

					mVehicles.emplace(hash_(vehcile.names_[i].c_str()), v);
				}
			}
		}
		//if (!sdk::Localization())
		//{
		//	CONSOLE_INFO("Vehicle Data Reset");
		//	goto reSet;
		//}
		/// release
		{
			if (vehcileSprite) { memset((void*)vehcileSprite, 0, spriteSize);	free((void*)vehcileSprite); }
			if (vehcileJson) { memset((void*)vehcileJson, 0, jsonSize);		free((void*)vehcileJson); }
			if (vehcileData) { memset((void*)vehcileData, 0, vehcileDataSize);		free((void*)vehcileData); }

			if (mSprites)
				delete mSprites;
			if (mVehicleDatabase)
				delete mVehicleDatabase;
		}
		return mLoaded = result;
	}

	VehicleManager::VehicleManager()
		:mLoaded(false),
		mTexture(),
		mSprites(),
		mVehicleDatabase()
	{

	}

	VehicleManager::~VehicleManager()
	{
		for (const auto& vehicle : mVehicles)
		{
			if (vehicle.second != nullptr)
				delete vehicle.second;
		}

	}

}