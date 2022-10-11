#pragma once
#include "shared.h"
#include <windows.h>

inline EXTENDED_CODE_ADDRESS makeExtendedCodeAddress(HMODULE moduleHandle, RELATIVE_VIRTUAL_ADDRESS relativeVirtualAddress) {
	return (EXTENDED_CODE_ADDRESS)moduleHandle + relativeVirtualAddress;
}

bool testCode(HMODULE moduleHandle, RELATIVE_VIRTUAL_ADDRESS codeRelativeVirtualAddress, VIRTUAL_SIZE codeVirtualSize, CODE1* testedCode);
bool extendCode(HMODULE moduleHandle, RELATIVE_VIRTUAL_ADDRESS codeRelativeVirtualAddress, void* extendedCode, bool call = false);
bool extendCode(HMODULE moduleHandle, RELATIVE_VIRTUAL_ADDRESS codeRelativeVirtualAddress);