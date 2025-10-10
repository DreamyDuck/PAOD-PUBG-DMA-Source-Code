#include "Include/WinHeaders.hpp"
#include "HandWeaponManager.h"
#include "FilePak.h"
#include "SDK/SDK.h"

namespace IronMan::Core
{
	using namespace SDK;
	HandWeaponManager* HandWeaponManager::mInstance = nullptr;
	HandWeaponManager* HandWeaponManager::GetManager()
	{
		return mInstance == nullptr
			? mInstance = new HandWeaponManager()
			: mInstance;
	}
	bool HandWeaponManager::Load()
	{
		bool result = true;

		mSprites = new HandWeaponSprites();
		if (!mSprites)
			return false;
		mHandWeaponDatabase = new HandWeaponDatabase();
		if (!mHandWeaponDatabase)
			return false;
		const void* handweaponSprite = nullptr, * handweaponJson = nullptr,
			* handweaponData = nullptr;
		size_t spriteSize, jsonSize,
			handweaponDataSize;
		{
			spriteSize = Package::GetPackage()->FindFile(HASH("hand.png"), (void**)&handweaponSprite);
			jsonSize = Package::GetPackage()->FindFile(HASH("hand.paper2dsprites"), (void**)&handweaponJson);
			if (!handweaponSprite || !handweaponJson) result = false;

			handweaponDataSize = Package::GetPackage()->FindFile(HASH("handweapons.json"), (void**)&handweaponData);
			if (!handweaponData) result = false;
		}
		if (result)
		{

			mTexture.push_back(OverlayEngine::CreateTexture((const char*)handweaponSprite, spriteSize));

			JsonReader frameReader((const char*)handweaponJson, jsonSize);
			if (!mSprites->Deserialize(frameReader))
				result = false;

			JsonReader handweaponDataReader((const char*)handweaponData, handweaponDataSize);
			if (!mHandWeaponDatabase->Deserialize(handweaponDataReader))
				result = false;

		}
		/// 
		if (result)
		{
			for (const auto& handweapon : mHandWeaponDatabase->handweapon)
			{

				HandWeapon* v = new HandWeapon;
				hash_t icon;
				icon = handweapon.icons_.front();
				if(!mSprites)
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

				v->AttachNum = handweapon.NumAttach_;

				for (int i = 0; i < handweapon.LootId_.size(); i++)
					v->AttachAble.emplace(handweapon.LootId_[i], 1);
				mHandWeapons.emplace(hash_(handweapon.names_.c_str()), v);
			}
		}
		{
			if (handweaponSprite) { memset((void*)handweaponSprite, 0, spriteSize);	free((void*)handweaponSprite); }
			if (handweaponJson) { memset((void*)handweaponJson, 0, jsonSize);		free((void*)handweaponJson); }
			if (handweaponData) { memset((void*)handweaponData, 0, handweaponDataSize);		free((void*)handweaponData); }

			if (mSprites)
				delete mSprites;
			if (mHandWeaponDatabase)
				delete mHandWeaponDatabase;
		}
		return result;
	}


	HandWeaponManager::HandWeaponManager()
		:mLoaded(false),
		mTexture(),
		mHandWeaponDatabase(),
		mSprites()
	{

	}

	HandWeaponManager::~HandWeaponManager()
	{
		for (const auto& handweapon : mHandWeapons)
		{
			if (handweapon.second != nullptr)
				delete handweapon.second;
		}

	}
}