#pragma once

//#define DEBUG

#ifdef DEBUG

#define LOG(msg) std::cout << "[ShadowVMT] " << msg << "\n";

#endif // DEBUG

#ifndef LOG

#define LOG(msg)

#endif // !LOG


class ShadowVMT
{
private:
	std::unique_ptr<std::uintptr_t[]> pShadowTable;
	std::uintptr_t* pOriginalTable = nullptr;
	std::uintptr_t** pVtablePtr = nullptr;
public:
	ShadowVMT(void* instance) {
		if (instance == nullptr)
			throw std::invalid_argument("Instance pointer is null");

		pOriginalTable = *(std::uintptr_t**)instance;

		size_t sz = 0;
		while (pOriginalTable[sz])
			sz++;

		if (sz == 0)
			throw std::runtime_error("Original VMT size is zero");

		pShadowTable = std::make_unique<std::uintptr_t[]>(sz);
		std::memcpy(pShadowTable.get(), pOriginalTable, sz * sizeof(std::uintptr_t));
		LOG("Copied VMT (size: " << sz << ")");
		pVtablePtr = (std::uintptr_t**)instance;
		pOriginalTable = *pVtablePtr;
		*pVtablePtr = pShadowTable.get();
		LOG("Hooked VMT");
	}
	~ShadowVMT() {
		Restore();
		LOG("Destructor called");
	}

	bool HookMethod(int idx, uintptr_t pMethod) {
		if (!pOriginalTable[idx])
			return false;

		pShadowTable[idx] = pMethod;
		LOG("Hooked method at index: " << idx << " (" << pMethod << ")");

		return true;
	}

	bool UnhookMethod(int idx) {
		pShadowTable[idx] = pOriginalTable[idx];
		LOG("Unhooked method at index: " << idx << " (" << pOriginalTable[idx] << ")");
		return true;
	}

	bool Restore() {
		*pVtablePtr = pOriginalTable;
		LOG("Restored original VMT")
		return true;
	}

	template <typename T, typename... Arguments>
	auto CallOriginal(int idx, Arguments... args) -> T {
		LOG("Calling original function at index: " << idx << " (" << pOriginalTable[idx] << ")")
		auto addr = pOriginalTable[idx];
		using FunctionPtr = T(*)(Arguments...);
		auto ogFunc = reinterpret_cast<FunctionPtr>(addr);
		return ogFunc(args...);
	}
};