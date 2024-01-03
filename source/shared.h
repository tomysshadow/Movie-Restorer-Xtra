#pragma once
#define _WIN32_WINNT 0x0501
#include <windows.h>

typedef DWORD VIRTUAL_ADDRESS;
typedef DWORD VIRTUAL_SIZE;
typedef DWORD RELATIVE_VIRTUAL_ADDRESS;

typedef DWORD EXTENDED_CODE_ADDRESS;

typedef BYTE CODE1;
typedef WORD CODE2;

inline bool stringNullOrEmpty(const char* str) {
	return !str || !*str;
}

inline size_t stringSize(const char* str) {
	return strlen(str) + 1;
}

inline bool stringEquals(const char* str, const char* str2) {
	return !strcmp(str, str2);
}

inline bool stringEqualsCaseInsensitive(const char* str, const char* str2) {
	return !_stricmp(str, str2);
}

inline bool memoryEquals(const void* mem, const void* mem2, size_t size) {
	return !memcmp(mem, mem2, size);
}

inline bool memoryShift(void *mem, size_t memSize, void *sourceMem, size_t sourceMemSize, size_t shift, bool direction) {
	#pragma warning(push)
	#pragma warning(disable : 4133)
	if (sourceMem < mem || (char*)sourceMem + sourceMemSize > (char*)mem + memSize) {
		return false;
	}

	size_t destinationSize = (char*)mem + memSize - sourceMem;
	char* destination = (char*)sourceMem;

	if (direction) {
		destination += shift;
	} else {
		destination -= shift;
	}

	if (destination < mem || destination + destinationSize > (char*)mem + memSize) {
		return false;
	}
	return !memmove_s(destination, destinationSize, sourceMem, sourceMemSize);
	#pragma warning(pop)
}

bool showLastError(LPCSTR errorMessage);
BOOL terminateCurrentProcess();
bool testSectionAddress(HMODULE moduleHandle, VIRTUAL_ADDRESS virtualAddress, VIRTUAL_SIZE virtualSize = 1);