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
	MODULE_DIRECTOR_12
};




// {FAB4E3BC-2BF4-4015-BE20-43832A5A5F37}
DEFINE_GUID(CLSID_TStdXtra,
			0xfab4e3bc, 0x2bf4, 0x4015, 0xbe, 0x20, 0x43, 0x83, 0x2a, 0x5a, 0x5f, 0x37);

EXTERN_BEGIN_DEFINE_CLASS_INSTANCE_VARS(TStdXtra)
	PIMoaMmUtils2 pMoaMmUtilsInterface;
	PIMoaMmValue pMoaMmValueInterface;
	PIMoaDrPlayer pMoaDrPlayerInterface;
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




inline bool callLingoQuit(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface) {
	// we can't just use Win32 API's TerminateProcess because it causes a long pause before closing the movie improperly
	// so we use Lingo's Quit handler
	MoaError err = kMoaErr_NoErr;
	MoaMmSymbol quitSymbol;

	err = pMoaMmValueInterface->StringToSymbol("Quit", &quitSymbol);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	err = pMoaDrMovieInterface->CallHandler(quitSymbol, 0, NULL, NULL);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	return true;
}

inline bool callLingoAlert(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, ConstPMoaChar message) {
	// we can't just use Win32 API's MessageBox or it'll cause graphical glitches from the movie not being paused
	// so we use Lingo's Alert handler (as per the XDK's recommendation)
	MoaError err = kMoaErr_NoErr;
	MoaMmSymbol alertSymbol;
	MoaMmValue messageValue = kVoidMoaMmValueInitializer;

	err = pMoaMmValueInterface->StringToSymbol("Alert", &alertSymbol);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	err = pMoaMmValueInterface->StringToValue(message, &messageValue);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	err = pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &messageValue, NULL);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	return true;
}

inline bool callLingoAlertXtraMissing(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, ConstPMoaChar message) {
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, message)) {
		return false;
	}
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, "An Xtra that the Movie Restorer Xtra requires is missing.")) {
		return false;
	}
	return true;
}

inline bool callLingoAlertComponentCorrupted(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, ConstPMoaChar message) {
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, message)) {
		return false;
	}
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, "A Component that the Movie Restorer Xtra requires is corrupted, garbled or tampered with. Please use the official Components provided by Macromedia/Adobe.")) {
		return false;
	}
	return true;
}

inline bool callLingoAlertIncompatibleDirectorVersion(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, ConstPMoaChar message) {
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, message)) {
		return false;
	}
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, "The Movie Restorer Xtra has determined it is incompatible with this Director version.")) {
		return false;
	}
	return true;
}

inline bool callLingoAlertAntivirus(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, ConstPMoaChar message) {
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, message)) {
		return false;
	}
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, "Your Antivirus may be preventing the Movie Restorer Xtra from working properly.")) {
		return false;
	}
	return true;
}




inline DWORD createExtendedCodeAddress(HANDLE moduleHandle, DWORD address) {
	return (DWORD)moduleHandle + address;
}

bool getSectionAddressAndSize(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, HANDLE moduleHandle, DWORD virtualAddress, DWORD virtualSize) {
	if (!moduleHandle) {
		//callLingoAlertXtraMissing(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Get Module Handle");
		return false;
	}
	PIMAGE_NT_HEADERS imageNtHeader = ImageNtHeader(moduleHandle);
	if (!imageNtHeader) {
		//callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Get Image NT Header");
		return false;
	}
	PIMAGE_SECTION_HEADER imageSectionHeader = (PIMAGE_SECTION_HEADER)(imageNtHeader + 1);
	if (!imageSectionHeader) {
		imageNtHeader = NULL;
		//callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Get Image Section Header");
		return false;
	}
	for (WORD i = 0;i < imageNtHeader->FileHeader.NumberOfSections;i++) {
		if (virtualAddress >= (DWORD)moduleHandle + imageSectionHeader->VirtualAddress && virtualAddress + virtualSize <= (DWORD)moduleHandle + imageSectionHeader->VirtualAddress + imageSectionHeader->Misc.VirtualSize) {
			imageNtHeader = NULL;
			imageSectionHeader = NULL;
			return true;
		}
		imageSectionHeader++;
	}
	// not dangerous, so let the caller decide whether or not to quit
	imageNtHeader = NULL;
	imageSectionHeader = NULL;
	return false;
}

bool unprotectCode(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, HANDLE moduleHandle, DWORD virtualAddress, DWORD virtualSize, DWORD &lpflOldProtect) {
	if (!getSectionAddressAndSize(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize)) {
		return false;
	}

	if (!VirtualProtect((LPVOID)virtualAddress, virtualSize, PAGE_EXECUTE_READWRITE, &lpflOldProtect) || !virtualAddress || !virtualSize) {
		return false;
	}

	// get Basic Memory Information
	MEMORY_BASIC_INFORMATION memoryBasicInformation;
	if (VirtualQuery((LPCVOID)virtualAddress, &memoryBasicInformation, sizeof(memoryBasicInformation)) != sizeof(memoryBasicInformation)
		|| !memoryBasicInformation.Protect
		|| memoryBasicInformation.Protect & PAGE_NOACCESS
		|| memoryBasicInformation.Protect & PAGE_EXECUTE) {
		// dangerous - we unprotected the code but we can't query it, so quit
		callLingoAlertAntivirus(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Get Basic Memory Information");
		callLingoQuit(pMoaMmValueInterface, pMoaDrMovieInterface);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}
	return true;
}

bool protectCode(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, HANDLE moduleHandle, DWORD virtualAddress, DWORD virtualSize, DWORD &lpflOldProtect) {
	if (!getSectionAddressAndSize(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize)) {
		callLingoAlertComponentCorrupted(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Get Section Address And Size");
		return false;
	}

	if (!lpflOldProtect || !VirtualProtect((LPVOID)virtualAddress, virtualSize, lpflOldProtect, &lpflOldProtect)) {
		callLingoAlertAntivirus(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Protect Code");
		return false;
	}
	return true;
}

bool flushCode(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, HANDLE moduleHandle, DWORD virtualAddress, DWORD virtualSize) {
	if (!getSectionAddressAndSize(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize)) {
		callLingoAlertComponentCorrupted(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Get Section Address And Size");
		return false;
	}

	if (!FlushInstructionCache(GetCurrentProcess(), (LPCVOID)virtualAddress, virtualSize)) {
		callLingoAlertAntivirus(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Flush Code");
		return false;
	}
	return true;
}

bool testCode(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, HANDLE moduleHandle, DWORD relativeVirtualAddress, DWORD virtualSize, unsigned char code[]) {
	if (!moduleHandle) {
		return false;
	}
	DWORD virtualAddress = (DWORD)moduleHandle + relativeVirtualAddress;
	DWORD lpflOldProtect = 0;

	if (!unprotectCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize, lpflOldProtect)) {
		return false;
	}

	bool result = memoryEqual((const void*)virtualAddress, (const void*)code, virtualSize);

	if (!protectCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize, lpflOldProtect)) {
		callLingoQuit(pMoaMmValueInterface, pMoaDrMovieInterface);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}
	return result;
}

bool extendCode(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, HANDLE moduleHandle, DWORD relativeVirtualAddress, void* code, bool call = false) {
	if (!moduleHandle) {
		return false;
	}
	DWORD virtualAddress = (DWORD)moduleHandle + relativeVirtualAddress;
	DWORD virtualSize = 5;
	DWORD lpflOldProtect = 0;

	if (!unprotectCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize, lpflOldProtect)) {
		return false;
	}

	if (!call) {
		*(PBYTE)virtualAddress = 0xE9;
	} else {
		*(PBYTE)virtualAddress = 0x58;
	}

	*(PDWORD)((PBYTE)virtualAddress + 1) = (DWORD)code - virtualAddress - virtualSize;

	if (!flushCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize)) {
		callLingoQuit(pMoaMmValueInterface, pMoaDrMovieInterface);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}

	if (!protectCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize, lpflOldProtect)) {
		callLingoQuit(pMoaMmValueInterface, pMoaDrMovieInterface);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}
	return true;
}

bool extendCode(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, HANDLE moduleHandle, DWORD relativeVirtualAddress) {
	if (!moduleHandle) {
		return false;
	}
	DWORD virtualAddress = (DWORD)moduleHandle + relativeVirtualAddress;
	DWORD virtualSize = 1;
	DWORD lpflOldProtect = 0;

	if (!unprotectCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize, lpflOldProtect)) {
		return false;
	}

	*(PBYTE)virtualAddress = 0x90;

	if (!flushCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize)) {
		callLingoQuit(pMoaMmValueInterface, pMoaDrMovieInterface);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}

	if (!protectCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize, lpflOldProtect)) {
		callLingoQuit(pMoaMmValueInterface, pMoaDrMovieInterface);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}
	return true;
}




bool extender(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface);

#endif