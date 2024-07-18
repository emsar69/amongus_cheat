#pragma once
#include <cstdint>
#include <Windows.h>
#include <vector>
#include <Psapi.h>

class Memory {
private:
	HANDLE handle;
		
public:

	Memory() {
		this->handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	}

	uintptr_t ReadByPointers(uintptr_t base, std::vector<uintptr_t> offsets) {
		uintptr_t calculated = base;
		if (calculated == 0) return 0;

		for (uintptr_t offset : offsets) {
			calculated = *(uintptr_t*)calculated + offset;
		}
		return calculated;
	}

	HANDLE getHandle() {
		return this->handle;
	}

	~Memory() {
		CloseHandle(this->handle);
	}
};