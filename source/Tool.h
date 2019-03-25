/*
ADOBE SYSTEMS INCORPORATED
Copyright 1994 - 2008 Adobe Macromedia Software LLC
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/

#define _CRT_NO_VA_START_VALIDATION

#ifndef _H_Script
#define _H_Script

#include <windows.h>
#include <Dbghelp.h>
#include <string.h>

#include "moastdif.h"
#include "xmmvalue.h"

#include "mmixtool.h"




enum MODULE_DIRECTOR_VERSION {
	MODULE_DIRECTOR_INCOMPATIBLE = -1,
	MODULE_DIRECTOR_8,
	MODULE_DIRECTOR_85,
	MODULE_DIRECTOR_851,
	MODULE_DIRECTOR_9,
	MODULE_DIRECTOR_10,
	MODULE_DIRECTOR_101,
	MODULE_DIRECTOR_1011,
	MODULE_DIRECTOR_11,
	MODULE_DIRECTOR_1103,
	MODULE_DIRECTOR_115,
	MODULE_DIRECTOR_1158,
	MODULE_DIRECTOR_1159,
	MODULE_DIRECTOR_1165,
	MODULE_DIRECTOR_12
};




// {FAB4E3BC-2BF4-4015-BE20-43832A5A5F37}
DEFINE_GUID(CLSID_TStdXtra,
			0xfab4e3bc, 0x2bf4, 0x4015, 0xbe, 0x20, 0x43, 0x83, 0x2a, 0x5a, 0x5f, 0x37);

EXTERN_BEGIN_DEFINE_CLASS_INSTANCE_VARS(TStdXtra)
	PIMoaMmValue moaMmValueInterfacePointer;
	PIMoaMmUtils2 moaMmUtilsInterfacePointer;
	PIMoaDrPlayer moaDrPlayerInterfacePointer;
EXTERN_END_DEFINE_CLASS_INSTANCE_VARS

EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaRegister)
	EXTERN_DEFINE_METHOD(MoaError, Register, (PIMoaCache, PIMoaXtraEntryDict))
EXTERN_END_DEFINE_CLASS_INTERFACE

EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaMmXTool)
	EXTERN_DEFINE_METHOD(MoaError, Invoke, ())
	EXTERN_DEFINE_METHOD(MoaError, GetEnabled, (PMoaDrEnabledState))
	private:
	EXTERN_DEFINE_METHOD(MoaError, XToolExtender, (PIMoaDrMovie))
	EXTERN_DEFINE_METHOD(MoaError, XToolExtender, ())
EXTERN_END_DEFINE_CLASS_INTERFACE




typedef DWORD VIRTUAL_ADDRESS;
typedef DWORD VIRTUAL_SIZE;
typedef DWORD RELATIVE_VIRTUAL_ADDRESS;
typedef DWORD EXTENDED_CODE_ADDRESS;

inline size_t stringSize(const char* string) {
	return strlen(string) + 1;
}

inline bool stringsEqual(const char* leftHandSide, const char* rightHandSide) {
	return !strcmp(leftHandSide, rightHandSide);
}

inline bool memoryEqual(const void* buffer, const void* buffer2, size_t bufferSize) {
	return !memcmp(buffer, buffer2, bufferSize);
}

bool shiftMemory(size_t bufferSize, const void* buffer, size_t sourceSize, const void* source, unsigned int shift, bool direction) {
	if (source < buffer || (char*)source + sourceSize > (char*)buffer + bufferSize) {
		return false;
	}
	
	size_t destinationSize = (char*)buffer + bufferSize - source;
	char* destination = (char*)source;

	if (!direction) {
		destination -= shift;
	} else {
		destination += shift;
	}
	
	if (destination < buffer || destination + sourceSize > (char*)buffer + bufferSize) {
		return false;
	}
	return !memmove_s(destination, destinationSize, source, sourceSize);
}




inline bool callLingoQuit(PIMoaMmValue moaMmValueInterfacePointer, PIMoaDrMovie moaDrMovieInterfacePointer) {
	// we can't just use Win32 API's TerminateProcess because it causes a long pause before closing the movie improperly
	// so we use Lingo's Quit handler
	MoaError err = kMoaErr_NoErr;
	MoaMmSymbol quitSymbol;

	if (!moaMmValueInterfacePointer || !moaDrMovieInterfacePointer) {
		return false;
	}

	err = moaMmValueInterfacePointer->StringToSymbol("Quit", &quitSymbol);

	if (err != kMoaErr_NoErr) {
		return false;
	}

	err = moaDrMovieInterfacePointer->CallHandler(quitSymbol, 0, NULL, NULL);

	if (err != kMoaErr_NoErr) {
		return false;
	}
	return true;
}

inline bool callLingoAlert(PIMoaMmValue moaMmValueInterfacePointer, PIMoaDrMovie moaDrMovieInterfacePointer, ConstPMoaChar message) {
	// we can't just use Win32 API's MessageBox or it'll cause graphical glitches from the movie not being paused
	// so we use Lingo's Alert handler (as per the XDK's recommendation)
	MoaError err = kMoaErr_NoErr;
	MoaMmSymbol alertSymbol;
	MoaMmValue messageValue = kVoidMoaMmValueInitializer;

	if (!moaMmValueInterfacePointer || !moaDrMovieInterfacePointer || !message) {
		return false;
	}

	err = moaMmValueInterfacePointer->StringToSymbol("Alert", &alertSymbol);

	if (err != kMoaErr_NoErr) {
		return false;
	}

	err = moaMmValueInterfacePointer->StringToValue(message, &messageValue);

	if (err != kMoaErr_NoErr) {
		return false;
	}

	err = moaDrMovieInterfacePointer->CallHandler(alertSymbol, 1, &messageValue, NULL);

	if (err != kMoaErr_NoErr) {
		return false;
	}
	return true;
}

inline bool callLingoAlertXtraMissing(PIMoaMmValue moaMmValueInterfacePointer, PIMoaDrMovie moaDrMovieInterfacePointer, ConstPMoaChar message) {
	if (!moaMmValueInterfacePointer || !moaDrMovieInterfacePointer || !message) {
		return false;
	}

	if (!callLingoAlert(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, message)) {
		return false;
	}

	if (!callLingoAlert(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, "An Xtra that the Movie Restorer Xtra requires is missing.")) {
		return false;
	}
	return true;
}

inline bool callLingoAlertComponentCorrupted(PIMoaMmValue moaMmValueInterfacePointer, PIMoaDrMovie moaDrMovieInterfacePointer, ConstPMoaChar message) {
	if (!moaMmValueInterfacePointer || !moaDrMovieInterfacePointer || !message) {
		return false;
	}

	if (!callLingoAlert(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, message)) {
		return false;
	}

	if (!callLingoAlert(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, "A Component that the Movie Restorer Xtra requires is corrupted, garbled or tampered with. Please use the official Components provided by Macromedia/Adobe.")) {
		return false;
	}
	return true;
}

inline bool callLingoAlertIncompatibleDirectorVersion(PIMoaMmValue moaMmValueInterfacePointer, PIMoaDrMovie moaDrMovieInterfacePointer, ConstPMoaChar message) {
	if (!moaMmValueInterfacePointer || !moaDrMovieInterfacePointer || !message) {
		return false;
	}

	if (!callLingoAlert(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, message)) {
		return false;
	}

	if (!callLingoAlert(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, "The Movie Restorer Xtra has determined it is incompatible with this Director version.")) {
		return false;
	}
	return true;
}

inline bool callLingoAlertAntivirus(PIMoaMmValue moaMmValueInterfacePointer, PIMoaDrMovie moaDrMovieInterfacePointer, ConstPMoaChar message) {
	if (!moaMmValueInterfacePointer || !moaDrMovieInterfacePointer || !message) {
		return false;
	}

	if (!callLingoAlert(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, message)) {
		return false;
	}

	if (!callLingoAlert(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, "Your Antivirus may be preventing the Movie Restorer Xtra from working properly.")) {
		return false;
	}
	return true;
}




inline EXTENDED_CODE_ADDRESS createExtendedCodeAddress(HMODULE moduleHandle, RELATIVE_VIRTUAL_ADDRESS relativeVirtualAddress) {
	return (VIRTUAL_ADDRESS)moduleHandle + relativeVirtualAddress;
}

bool testSectionAddress(PIMoaMmValue moaMmValueInterfacePointer, PIMoaDrMovie moaDrMovieInterfacePointer, HMODULE moduleHandle, VIRTUAL_ADDRESS virtualAddress, VIRTUAL_SIZE virtualSize) {
	if (!moaMmValueInterfacePointer || !moaDrMovieInterfacePointer || !moduleHandle) {
		return false;
	}

	if (!moduleHandle) {
		//callLingoAlertXtraMissing(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, "Failed to Get Module Handle");
		return false;
	}

	PIMAGE_NT_HEADERS imageNTHeader = ImageNtHeader(moduleHandle);

	if (!imageNTHeader) {
		//callLingoAlert(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, "Failed to Get Image NT Header");
		return false;
	}

	PIMAGE_SECTION_HEADER imageSectionHeader = (PIMAGE_SECTION_HEADER)(imageNTHeader + 1);

	if (!imageSectionHeader) {
		imageNTHeader = NULL;
		//callLingoAlert(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, "Failed to Get Image Section Header");
		return false;
	}

	for (WORD i = 0;i < imageNTHeader->FileHeader.NumberOfSections;i++) {
		if (virtualAddress >= (VIRTUAL_ADDRESS)moduleHandle + imageSectionHeader->VirtualAddress && virtualAddress + virtualSize <= (VIRTUAL_ADDRESS)moduleHandle + imageSectionHeader->VirtualAddress + imageSectionHeader->Misc.VirtualSize) {
			imageNTHeader = NULL;
			imageSectionHeader = NULL;
			return true;
		}
		imageSectionHeader++;
	}

	// not dangerous, so let the caller decide whether or not to quit
	imageNTHeader = NULL;
	imageSectionHeader = NULL;
	return false;
}

bool unprotectCode(PIMoaMmValue moaMmValueInterfacePointer, PIMoaDrMovie moaDrMovieInterfacePointer, HMODULE moduleHandle, VIRTUAL_ADDRESS virtualAddress, VIRTUAL_SIZE virtualSize, DWORD &oldProtect) {
	if (!moaMmValueInterfacePointer || !moaDrMovieInterfacePointer || !moduleHandle) {
		return false;
	}

	if (!testSectionAddress(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, moduleHandle, virtualAddress, virtualSize)) {
		return false;
	}

	if (!virtualAddress || !virtualSize || !VirtualProtect((LPVOID)virtualAddress, virtualSize, PAGE_EXECUTE_READWRITE, &oldProtect)) {
		return false;
	}

	// get Basic Memory Information
	MEMORY_BASIC_INFORMATION memoryBasicInformation;
	if (VirtualQuery((LPCVOID)virtualAddress, &memoryBasicInformation, sizeof(memoryBasicInformation)) != sizeof(memoryBasicInformation)
		|| !memoryBasicInformation.Protect
		|| memoryBasicInformation.Protect & PAGE_NOACCESS
		|| memoryBasicInformation.Protect & PAGE_EXECUTE) {
		// dangerous - we unprotected the code but we can't query it, so quit
		callLingoAlertAntivirus(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, "Failed to Get Basic Memory Information");
		callLingoQuit(moaMmValueInterfacePointer, moaDrMovieInterfacePointer);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}
	return true;
}

bool protectCode(PIMoaMmValue moaMmValueInterfacePointer, PIMoaDrMovie moaDrMovieInterfacePointer, HMODULE moduleHandle, VIRTUAL_ADDRESS virtualAddress, VIRTUAL_SIZE virtualSize, DWORD &oldProtect) {
	if (!moaMmValueInterfacePointer || !moaDrMovieInterfacePointer || !moduleHandle) {
		return false;
	}

	if (!testSectionAddress(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, moduleHandle, virtualAddress, virtualSize)) {
		callLingoAlertComponentCorrupted(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, "Failed to Test Section Address");
		return false;
	}

	if (!virtualAddress || !virtualSize || !VirtualProtect((LPVOID)virtualAddress, virtualSize, oldProtect, &oldProtect)) {
		callLingoAlertAntivirus(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, "Failed to Protect Code");
		return false;
	}
	return true;
}

bool flushCode(PIMoaMmValue moaMmValueInterfacePointer, PIMoaDrMovie moaDrMovieInterfacePointer, HMODULE moduleHandle, VIRTUAL_ADDRESS virtualAddress, VIRTUAL_SIZE virtualSize) {
	if (!moaMmValueInterfacePointer || !moaDrMovieInterfacePointer || !moduleHandle) {
		return false;
	}

	if (!testSectionAddress(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, moduleHandle, virtualAddress, virtualSize)) {
		callLingoAlertComponentCorrupted(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, "Failed to Test Section Address");
		return false;
	}

	if (!FlushInstructionCache(GetCurrentProcess(), (LPCVOID)virtualAddress, virtualSize)) {
		callLingoAlertAntivirus(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, "Failed to Flush Code");
		return false;
	}
	return true;
}

bool testCode(PIMoaMmValue moaMmValueInterfacePointer, PIMoaDrMovie moaDrMovieInterfacePointer, HMODULE moduleHandle, RELATIVE_VIRTUAL_ADDRESS relativeVirtualAddress, VIRTUAL_SIZE virtualSize, unsigned char code[]) {
	if (!moaMmValueInterfacePointer || !moaDrMovieInterfacePointer || !moduleHandle) {
		return false;
	}

	VIRTUAL_ADDRESS virtualAddress = (VIRTUAL_ADDRESS)moduleHandle + relativeVirtualAddress;
	DWORD oldProtect = 0;

	if (!unprotectCode(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, moduleHandle, virtualAddress, virtualSize, oldProtect)) {
		return false;
	}

	bool result = memoryEqual((const void*)virtualAddress, (const void*)code, virtualSize);

	if (!protectCode(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, moduleHandle, virtualAddress, virtualSize, oldProtect)) {
		callLingoQuit(moaMmValueInterfacePointer, moaDrMovieInterfacePointer);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}
	return result;
}

bool extendCode(PIMoaMmValue moaMmValueInterfacePointer, PIMoaDrMovie moaDrMovieInterfacePointer, HMODULE moduleHandle, RELATIVE_VIRTUAL_ADDRESS relativeVirtualAddress, void* code, bool call = false) {
	if (!moaMmValueInterfacePointer || !moaDrMovieInterfacePointer || !moduleHandle) {
		return false;
	}

	VIRTUAL_ADDRESS virtualAddress = (VIRTUAL_ADDRESS)moduleHandle + relativeVirtualAddress;
	VIRTUAL_SIZE virtualSize = 5;
	DWORD oldProtect = 0;

	if (!unprotectCode(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, moduleHandle, virtualAddress, virtualSize, oldProtect)) {
		return false;
	}

	*(PBYTE)virtualAddress = 0xE9 - call;
	*(VIRTUAL_ADDRESS*)((PBYTE)virtualAddress + 1) = (VIRTUAL_ADDRESS)code - virtualAddress - virtualSize;

	if (!flushCode(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, moduleHandle, virtualAddress, virtualSize)) {
		callLingoQuit(moaMmValueInterfacePointer, moaDrMovieInterfacePointer);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}

	if (!protectCode(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, moduleHandle, virtualAddress, virtualSize, oldProtect)) {
		callLingoQuit(moaMmValueInterfacePointer, moaDrMovieInterfacePointer);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}
	return true;
}

bool extendCode(PIMoaMmValue moaMmValueInterfacePointer, PIMoaDrMovie moaDrMovieInterfacePointer, HMODULE moduleHandle, RELATIVE_VIRTUAL_ADDRESS relativeVirtualAddress) {
	if (!moaMmValueInterfacePointer || !moaDrMovieInterfacePointer || !moduleHandle) {
		return false;
	}

	VIRTUAL_ADDRESS virtualAddress = (VIRTUAL_ADDRESS)moduleHandle + relativeVirtualAddress;
	VIRTUAL_SIZE virtualSize = 1;
	DWORD oldProtect = 0;

	if (!unprotectCode(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, moduleHandle, virtualAddress, virtualSize, oldProtect)) {
		return false;
	}

	*(PBYTE)virtualAddress = 0x90;

	if (!flushCode(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, moduleHandle, virtualAddress, virtualSize)) {
		callLingoQuit(moaMmValueInterfacePointer, moaDrMovieInterfacePointer);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}

	if (!protectCode(moaMmValueInterfacePointer, moaDrMovieInterfacePointer, moduleHandle, virtualAddress, virtualSize, oldProtect)) {
		callLingoQuit(moaMmValueInterfacePointer, moaDrMovieInterfacePointer);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}
	return true;
}




bool extender(PIMoaMmValue moaMmValueInterfacePointer, PIMoaDrMovie moaDrMovieInterfacePointer);

#endif