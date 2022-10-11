#include "shared.h"
#include "Extender.h"
#include <windows.h>
#include <Dbghelp.h>

bool testSectionAddress(HMODULE moduleHandle, VIRTUAL_ADDRESS virtualAddress, VIRTUAL_SIZE virtualSize) {
	if (!moduleHandle) {
		//showLastError("moduleHandle must not be NULL");
		return false;
	}

	PIMAGE_NT_HEADERS imageNTHeaders = ImageNtHeader(moduleHandle);

	if (!imageNTHeaders) {
		//showLastError("Failed to Image NT Header");
		return false;
	}

	PIMAGE_SECTION_HEADER imageSectionHeader = (PIMAGE_SECTION_HEADER)(imageNTHeaders + 1);

	if (!imageSectionHeader) {
		//showLastError("imageSectionHeader must not be NULL");
		imageNTHeaders = NULL;
		return false;
	}

	for (WORD i = 0; i < imageNTHeaders->FileHeader.NumberOfSections; i++) {
		if (virtualAddress >= (VIRTUAL_ADDRESS)moduleHandle + imageSectionHeader->VirtualAddress && virtualAddress + virtualSize < (VIRTUAL_ADDRESS)moduleHandle + imageSectionHeader->VirtualAddress + imageSectionHeader->Misc.VirtualSize) {
			imageNTHeaders = NULL;
			imageSectionHeader = NULL;
			return true;
		}

		imageSectionHeader++;
	}

	imageNTHeaders = NULL;
	imageSectionHeader = NULL;
	return false;
}

bool unprotectCode(HMODULE moduleHandle, VIRTUAL_ADDRESS codeVirtualAddress, VIRTUAL_SIZE codeVirtualSize, DWORD &oldProtect) {
	if (!moduleHandle) {
		//showLastError("moduleHandle must not be NULL");
		return false;
	}

	if (!testSectionAddress(moduleHandle, codeVirtualAddress, codeVirtualSize)) {
		//showLastError("Failed to Test Section Address");
		return false;
	}

	if (!VirtualProtect((LPVOID)codeVirtualAddress, codeVirtualSize, PAGE_EXECUTE_READWRITE, &oldProtect)) {
		//showLastError("Failed to Unprotect Code");
		return false;
	}
	return true;
}

bool protectCode(HMODULE moduleHandle, VIRTUAL_ADDRESS codeVirtualAddress, VIRTUAL_SIZE codeVirtualSize, DWORD &oldProtect) {
	if (!moduleHandle) {
		//showLastError("moduleHandle must not be NULL");
		return false;
	}

	if (!testSectionAddress(moduleHandle, codeVirtualAddress, codeVirtualSize)) {
		//showLastError("Failed to Test Section Address");
		return false;
	}

	if (!VirtualProtect((LPVOID)codeVirtualAddress, codeVirtualSize, oldProtect, &oldProtect)) {
		//showLastError("Failed to Protect Code");
		return false;
	}
	return true;
}

bool flushCode(HMODULE moduleHandle, VIRTUAL_ADDRESS codeVirtualAddress, VIRTUAL_SIZE codeVirtualSize) {
	if (!moduleHandle) {
		//showLastError("moduleHandle must not be NULL");
		return false;
	}

	if (!testSectionAddress(moduleHandle, codeVirtualAddress, codeVirtualSize)) {
		//showLastError("Failed to Test Section Address");
		return false;
	}

	if (!FlushInstructionCache(GetCurrentProcess(), (LPCVOID)codeVirtualAddress, codeVirtualSize)) {
		//showLastError("Failed to Flush Code");
		return false;
	}
	return true;
}

bool testCode(HMODULE moduleHandle, RELATIVE_VIRTUAL_ADDRESS codeRelativeVirtualAddress, VIRTUAL_SIZE codeVirtualSize, CODE1* testedCode) {
	if (!moduleHandle) {
		//showLastError("moduleHandle must not be NULL");
		return false;
	}

	if (!testedCode) {
		//showLastError("code must not be NULL");
		return false;
	}

	VIRTUAL_ADDRESS codeVirtualAddress = (VIRTUAL_ADDRESS)moduleHandle + codeRelativeVirtualAddress;

	DWORD oldProtect = 0;

	if (!unprotectCode(moduleHandle, codeVirtualAddress, codeVirtualSize, oldProtect)) {
		//showLastError("Failed to Unprotect Code");
		return false;
	}

	void* codePointer = (void*)codeVirtualAddress;

	if (!codePointer) {
		//showLastError("codePointer must not be zero");
		return false;
	}

	bool result = memoryEqual((void*)codePointer, testedCode, codeVirtualSize);

	codePointer = NULL;

	if (!protectCode(moduleHandle, codeVirtualAddress, codeVirtualSize, oldProtect)) {
		//showLastError("Failed to Protect Code");
		return false;
	}
	return result;
}

bool extendCode(HMODULE moduleHandle, RELATIVE_VIRTUAL_ADDRESS codeRelativeVirtualAddress, void* extendedCode, bool call) {
	if (!moduleHandle) {
		//showLastError("moduleHandle must not be NULL");
		return false;
	}

	if (!extendedCode) {
		//showLastError("extendedCode must not be zero");
		return false;
	}

	VIRTUAL_ADDRESS codeVirtualAddress = (VIRTUAL_ADDRESS)moduleHandle + codeRelativeVirtualAddress;
	const VIRTUAL_SIZE CODE_VIRTUAL_SIZE = 5;

	DWORD oldProtect = 0;

	if (!unprotectCode(moduleHandle, codeVirtualAddress, CODE_VIRTUAL_SIZE, oldProtect)) {
		//showLastError("Failed to Unprotect Code");
		return false;
	}

	CODE1* codePointer = (CODE1*)codeVirtualAddress;

	if (!codePointer) {
		//showLastError("codePointer must not be NULL");
		return false;
	}

	const CODE1 INSTRUCTIONS_JMP = 0xE9;
	const CODE1 INSTRUCTIONS_CALL = 0xE8;

	*(VIRTUAL_ADDRESS*)(codePointer + 1) = (VIRTUAL_ADDRESS)extendedCode - codeVirtualAddress - CODE_VIRTUAL_SIZE;
	*codePointer = call ? INSTRUCTIONS_CALL : INSTRUCTIONS_JMP;
	codePointer = NULL;

	if (!protectCode(moduleHandle, codeVirtualAddress, CODE_VIRTUAL_SIZE, oldProtect)) {
		//showLastError("Failed to Protect Code");
		return false;
	}

	if (!flushCode(moduleHandle, codeVirtualAddress, CODE_VIRTUAL_SIZE)) {
		//showLastError("Failed to Flush Code");
		return false;
	}
	return true;
}

bool extendCode(HMODULE moduleHandle, RELATIVE_VIRTUAL_ADDRESS codeRelativeVirtualAddress) {
	if (!moduleHandle) {
		//showLastError("moduleHandle must not be NULL");
		return false;
	}

	VIRTUAL_ADDRESS codeVirtualAddress = (VIRTUAL_ADDRESS)moduleHandle + codeRelativeVirtualAddress;
	const VIRTUAL_SIZE CODE_VIRTUAL_SIZE = 1;

	DWORD oldProtect = 0;

	if (!unprotectCode(moduleHandle, codeVirtualAddress, CODE_VIRTUAL_SIZE, oldProtect)) {
		//showLastError("Failed to Unprotect Code");
		return false;
	}

	CODE1* codePointer = (CODE1*)codeVirtualAddress;

	if (!codePointer) {
		//showLastError("codePointer must not be NULL");
		return false;
	}

	const CODE1 INSTRUCTIONS_NOP = 0x90;

	*codePointer = INSTRUCTIONS_NOP;
	codePointer = NULL;

	if (!protectCode(moduleHandle, codeVirtualAddress, CODE_VIRTUAL_SIZE, oldProtect)) {
		//showLastError("Failed to Protect Code");
		return false;
	}

	if (!flushCode(moduleHandle, codeVirtualAddress, CODE_VIRTUAL_SIZE)) {
		//showLastError("Failed to Flush Code");
		return false;
	}
	return true;
}