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

inline size_t stringSize(const char* string) {
	return strlen(string) + 1;
}

inline bool stringsEqual(const char* leftHandSide, const char* rightHandSide) {
	return !strcmp(leftHandSide, rightHandSide);
}

inline bool stringsCaseInsensitiveEqual(const char* leftHandSide, const char* rightHandSide) {
	return !_stricmp(leftHandSide, rightHandSide);
}

inline bool memoryEqual(const void* buffer, const void* buffer2, size_t bufferSize) {
	return !memcmp(buffer, buffer2, bufferSize);
}

inline bool memoryShift(size_t bufferSize, void* buffer, size_t sourceSize, void* source, size_t shift, bool direction) {
	#pragma warning(push)
	#pragma warning(disable : 4133)
	if (source < buffer || (char*)source + sourceSize > (char*)buffer + bufferSize) {
		return false;
	}

	size_t destinationSize = (char*)buffer + bufferSize - source;
	char* destination = (char*)source;

	if (direction) {
		destination += shift;
	} else {
		destination -= shift;
	}

	if (destination < buffer || destination + destinationSize > (char*)buffer + bufferSize) {
		return false;
	}
	return !memmove_s(destination, destinationSize, source, sourceSize);
	#pragma warning(pop)
}

bool showLastError(LPCSTR errorMessage);
BOOL terminateCurrentProcess();
bool testSectionAddress(HMODULE moduleHandle, VIRTUAL_ADDRESS virtualAddress, VIRTUAL_SIZE virtualSize = 1);