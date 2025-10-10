#pragma once
#include "Helper/Misc/ZydisWrapper.h"
#include "Helper/Asm/AsmFactory.hpp"
#include "Helper/Misc/DynImport.hpp"
#include <functional>

namespace IronMan::Analysis
{
	static NTSTATUS QueryMemory(ptr_t lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer)
	{
		SetLastNtStatus(STATUS_SUCCESS);
		SAFE_CALL(VirtualQuery, reinterpret_cast<LPCVOID>(lpAddress), lpBuffer, sizeof(MEMORY_BASIC_INFORMATION));
		return LastNtStatus();
	}


	template <typename K, typename T>
	class SelfMap
	{
	public:

		auto begin() { return m_data.begin(); }
		auto begin() const { return m_data.begin(); }
		auto end() { return m_data.end(); }
		auto end() const { return m_data.end(); }
		auto empty() const { return m_data.empty(); }


		T* find(const K& k)
		{
			for (unsigned int i = 0; i < m_data.size(); i++)
			{
				if (m_data[i].first == k)
				{
					return &(m_data[i].second);
				}
			}
			return NULL;
		}

		T* findValue(std::function<bool(const T& value)> f)
		{
			for (int i = (m_data.size() - 1); i >= 0; --i)
			{
				if (f(m_data[i].second))
				{
					return &(m_data[i].second);
				}
			}
			return NULL;
		}


		void insert(const K& f, const T& s)
		{
			T* p = find(f);
			if (p == NULL)
			{
				m_data.push_back(std::pair<K, T>(f, s));
			}
			else
			{
				*p = s;
			}
		}

		void removed(const K& k)
		{
			unsigned int index = 0;
			for (unsigned int i = 0; i < m_data.size(); i++)
			{
				if (m_data[i].first == k)
				{
					index = i;
					break;
				}
			}
			m_data.erase(m_data.begin() + index);
		}

		T& operator [](const K& key)
		{
			if (this->find(key) != NULL)
			{
				return *this->find(key);
			}
			else
			{
				this->insert(key, T());
			}

			return *this->find(key);
		}

	private:
		std::vector<std::pair<K, T>> m_data;
	};
	class AnalysisBase
	{
	public:
		explicit AnalysisBase(ptr_t base, ptr_t size);
		AnalysisBase(const AnalysisBase& that) = delete;
		virtual ~AnalysisBase();
		virtual bool Analyse() = 0;
		virtual bool Build(std::function< void(void*)> callback = nullptr) = 0;
		virtual const ptr_t GetEntryPoint() const { return 0; }
		inline const std::vector<AsmHelperPtr>& GetDecryptFunctions() const { return mBuildResult; }


	protected:
		AnalysisBase() {}

	protected:

		ZydisRegister GpRegTo64(ZydisRegister reg);
		asmjit::GpReg ToGpReg(ZydisRegister reg);

		bool inRange(uint64_t addr, uint64_t min, uint64_t max) const
		{
			return addr >= min && addr <= max;
		}

		bool inRange(ptr_t addr) const
		{
			return addr >= mBase && addr < mBase + mSize;
		}

		const unsigned char* translateAddr(ptr_t addr) const
		{
			return inRange(addr) ? mData + (addr - mBase) : nullptr;
		}
	protected:
		ptr_t mBase;
		ptr_t mSize;
		unsigned char* mData;
		Zydis mCp;
		std::vector<AsmHelperPtr> mBuildResult;
	};
}
