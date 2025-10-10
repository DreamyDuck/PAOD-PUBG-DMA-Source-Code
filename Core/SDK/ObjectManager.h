#pragma once
#include "Include/WinHeaders.hpp"

namespace IronMan::Core::SDK
{
	struct OMFClassBaseChain
	{
	public:
		ptr_t ClassBaseChainArray;
		int32_t NumClassBasesInChainMinusOne;
		inline OMFClassBaseChain()
		{
			ClassBaseChainArray = 0;
			NumClassBasesInChainMinusOne = 0;
		}
		inline OMFClassBaseChain(ptr_t a, int32_t b)
		{
			ClassBaseChainArray = a;
			NumClassBasesInChainMinusOne = b;
		}
	};

	struct MapClassBaseChain
	{
		ptr_t Actor;
		OMFClassBaseChain Chian;
		ptr_t Value;
		inline MapClassBaseChain(ptr_t a, OMFClassBaseChain b, ptr_t c)
		{
			Actor = a;
			Chian = b;
			Value = c;
		}
		inline MapClassBaseChain()
		{
			Actor = 0;
			Chian.ClassBaseChainArray = 0;
			Chian.NumClassBasesInChainMinusOne = 0;
			Value = 0;
		}
	};

	class ObjectManager
	{
	public:
		static bool Initialize();
		static bool InitializeScatterHandle();

		DECLSPEC_NOINLINE static auto GetObjects()
		{
			unique_readguard<WfirstRWLock> rlock(_rwLock);
			return cachedObjects;
		}

		DECLSPEC_NOINLINE static void SetObjects(std::unordered_map <ptr_t, std::shared_ptr<AActor>>& objects)
		{
			unique_writeguard<WfirstRWLock> wlock(_rwLock);
			cachedObjects = objects;
			objects.clear();
		}

		DECLSPEC_NOINLINE static void AddObject(AActor& objects, UObjectType Type)
		{
			EraseObject(objects);
			auto managedObject = PrepareCreateObjectPointerFromObject(objects, Type);
			if (!managedObject)
				return;
			managedObject->SetAddObject(true);
			unique_writeguard<WfirstRWLock> wlock(_rwLock);
			cachedObjects.emplace((ptr_t)managedObject->GetPtr(), std::shared_ptr<AActor>(managedObject));
		}

		DECLSPEC_NOINLINE static void EraseObject(AActor& objects)
		{
			unique_writeguard<WfirstRWLock> wlock(_rwLock);
			auto FindResult = cachedObjects.find((ptr_t)objects->GetPtr());
			if (FindResult != cachedObjects.end())
			{
				FindResult->second->Release();
				cachedObjects.erase(FindResult);
			}
		}

	private:
		static class AActor* PrepareCreateObjectPointerFromObject(class AActor& gameObj, UObjectType Type);
		static class AActor* CreateObjectPointerFromObject(class AActor& gameObj);
		static class UActorComponent* CreateComponentObjectPointerFromObject(class UActorComponent& gameObj);
		static DWORD UpdateWorldThread(LPVOID lpParam);
		static DWORD UpdateThread(LPVOID lpParam);
		static void CacheObjects();
		static void GameUpdate();
		static void GameWorld();
	private:
		static HANDLE _hThread;
		static bool _bWork;
		static WfirstRWLock _rwLock;
	public:
		static std::unordered_map <ptr_t, std::shared_ptr<AActor>> cachedObjects;
	};
}