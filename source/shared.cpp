#include "shared.h"
#include <string>
#include <sstream>
#include <windows.h>
#include <Dbghelp.h>

static LPCSTR MESSAGE_BOX_CAPTION = "LeechProtectionRemovalHelp Error";

bool showLastError(LPCSTR errorMessage) {
	if (!errorMessage) {
		return false;
	}

	std::ostringstream oStringStream;
	oStringStream << errorMessage << " (" << GetLastError() << ")";

	if (!MessageBox(NULL, oStringStream.str().c_str(), MESSAGE_BOX_CAPTION, MB_OK | MB_ICONERROR)) {
		return false;
	}
	return true;
}

BOOL terminateCurrentProcess() {
	return TerminateProcess(GetCurrentProcess(), GetLastError());
}

bool testSectionAddress(HMODULE moduleHandle, VIRTUAL_ADDRESS virtualAddress, VIRTUAL_SIZE virtualSize) {
	if (!moduleHandle) {
		//showLastError("moduleHandle must not be NULL");
		return false;
	}

	PIMAGE_DOS_HEADER imageDOSHeaderPointer = (PIMAGE_DOS_HEADER)moduleHandle;

	if (!imageDOSHeaderPointer) {
		//showLastError("imageDOSHeaderPointer must not be NULL");
		return false;
	}

	if (imageDOSHeaderPointer->e_magic != IMAGE_DOS_SIGNATURE) {
		//showLastError("e_magic must be IMAGE_DOS_SIGNATURE");
		goto error;
	}

	PIMAGE_NT_HEADERS imageNTHeadersPointer = ImageNtHeader(imageDOSHeaderPointer);

	if (!imageNTHeadersPointer) {
		//showLastError("imageNTHeadersPointer must not be NULL");
		goto error;
	}

	if (imageNTHeadersPointer->Signature != IMAGE_NT_SIGNATURE) {
		//showLastError("Signature must be IMAGE_NT_SIGNATURE");
		goto error2;
	}

	PIMAGE_SECTION_HEADER imageSectionHeaderPointer = (PIMAGE_SECTION_HEADER)(imageNTHeadersPointer + 1);

	if (!imageSectionHeaderPointer) {
		//showLastError("imageSectionHeaderPointer must not be NULL");
		return false;
	}

	VIRTUAL_ADDRESS moduleVirtualAddress = (VIRTUAL_ADDRESS)moduleHandle;

	for (WORD i = 0; i < imageNTHeadersPointer->FileHeader.NumberOfSections; i++) {
		if (virtualAddress >= moduleVirtualAddress + imageSectionHeaderPointer->VirtualAddress && virtualAddress + virtualSize <= moduleVirtualAddress + imageSectionHeaderPointer->VirtualAddress + imageSectionHeaderPointer->Misc.VirtualSize) {
			imageSectionHeaderPointer = NULL;
			imageNTHeadersPointer = NULL;
			imageDOSHeaderPointer = NULL;
			return true;
		}

		imageSectionHeaderPointer++;
	}

	imageSectionHeaderPointer = NULL;
	error2:
	imageNTHeadersPointer = NULL;
	error:
	imageDOSHeaderPointer = NULL;
	return false;
}