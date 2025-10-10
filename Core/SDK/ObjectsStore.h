#pragma once
#include "CoreUObject/Object.h"
#include "CoreUObject/Class.h"

#include <unordered_map>

namespace IronMan::Core::SDK
{
	extern std::unordered_map<hash_t, UObject> ObjectCache;
	class ObjectsIterator;

	class TUObjectArray
	{
	public:
		UEEncryptedObjectProperty<FUObjectItem*, DecryptFunc::General> Objects;
		uint32_t NumElements;
		uint32_t MaxElements;
		uint64_t unk;//7.8 added
	};

	class ObjectsStore
	{
	public:

		/// <summary>
		/// Initializes this object.
		/// </summary>
		/// <returns>
		/// true if it succeeds, false if it fails.
		/// </returns>
		static bool Initialize();

		/// <summary>Gets the address of the global objects store.</summary>
		/// <returns>The address of the global objects store.</returns>
		static void* GetAddress();

		ObjectsIterator begin();

		ObjectsIterator begin() const;

		ObjectsIterator end();

		ObjectsIterator end() const;

		/// <summary>
		/// Gets the number of available objects.
		/// </summary>
		/// <returns>The number of objects.</returns>
		size_t GetObjectsNum() const;

		/// <summary>
		/// Gets the object item by id.
		/// </summary>
		/// <param name="id">The identifier.</param>
		/// <returns>The object.</returns>
		FUObjectItem GetItemById(size_t id) const;

		FUObjectItem GetItemByIdRealTime(size_t id) const;

		/// <summary>
		/// Gets the object by id.
		/// </summary>
		/// <param name="id">The identifier.</param>
		/// <returns>The object.</returns>
		UObject GetById(size_t id) const;

		/// <summary>
		/// Searches for the first object with the given name.
		/// </summary>
		/// <param name="name">The name of the class.</param>
		/// <returns>The found class which is not valid if no class could be found.</returns>
		template<typename T>
		T FindObject(const hash_t nameHash) const
		{
			try
			{
				auto cache = ObjectCache.find(nameHash);
				if (cache != ObjectCache.end())
				{
					return cache->second.Cast<T>();
				}
			}
			catch (...)
			{

			}
			return T(nullptr);
		}
		template<typename T, hash_t hash>
		T FindObject() const
		{
			return FindObject<T>(hash);
		}
		/// <summary>
		/// Searches for the first class with the given name.
		/// </summary>
		/// <param name="name">The name of the class.</param>
		/// <returns>The found class which is not valid if no class could be found.</returns>
		template<hash_t nameHash>
		UClass FindClass() const
		{
			return FindObject<UClass, nameHash>();
		}

		/// <summary>Count objects which have the same name and type.</summary>
		/// <typeparam name="T">Type of the object.</typeparam>
		/// <param name="name">The name to search for.</param>
		/// <returns>The number of objects which share a name.</returns>
		template<class T>
		size_t CountObjects(const std::string& name) const
		{
			static std::unordered_map<std::string, size_t> cache;

			auto it = cache.find(name);
			if (it != std::end(cache))
			{
				return it->second;
			}

			size_t count = 0;
			for (auto obj : *this)
			{
				if (obj.IsA<T>() && obj.GetName() == name)
				{
					++count;
				}
			}

			cache[name] = count;

			return count;
		}
	};

	/// <summary>Holds information about an object.</summary>
	struct UEObjectInfo
	{
		/// <summary>Zero-based index of the object in the global objects store.</summary>
		size_t Index;

		/// <summary>The object.</summary>
		UObject Object;
	};

	/// <summary>An iterator for objects.</summary>
	class ObjectsIterator : public std::iterator<std::forward_iterator_tag, UObject>
	{
		const ObjectsStore& store;
		size_t index;
		UObject current;

	public:

		/// <summary>Constructor.</summary>
		/// <param name="store">The store to iterate.</param>
		ObjectsIterator(const ObjectsStore& store);

		/// <summary>Constructor.</summary>
		/// <param name="store">The store to iterate.</param>
		/// <param name="index">Zero-based start index.</param>
		explicit ObjectsIterator(const ObjectsStore& store, size_t index);

		ObjectsIterator(const ObjectsIterator& other);
		ObjectsIterator(ObjectsIterator&& other) noexcept;

		ObjectsIterator& operator=(const ObjectsIterator& rhs);

		void swap(ObjectsIterator& other) noexcept;

		ObjectsIterator& operator++();

		ObjectsIterator operator++ (int);

		bool operator==(const ObjectsIterator& rhs) const;

		bool operator!=(const ObjectsIterator& rhs) const;

		UObject operator*() const;

		UObject operator->() const;
	};
}