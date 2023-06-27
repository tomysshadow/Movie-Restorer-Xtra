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

#define INITGUID 1

#include "shared.h"
#include "tool.h"
#include "Extender.h"

#pragma function(strlen)
#include "moastdif.h"
#include "mmixtool.h"
#include "driservc.h"
#include "xmmvalue.h"
#include "xclassver.h"
#include "moatry.h"

#include <windows.h>




/* Begin Xtra */
// this is where the structure of the Xtra is defined
// this says that this is a Standard Xtra and
// that said Standard Xtra class provides the
// interfaces for IMoaRegister and IMoaMmXScript, as well as
// says which version of the XDK we're using
#define XTRA_VERSION_NUMBER XTRA_CLASS_VERSION

BEGIN_XTRA
BEGIN_XTRA_DEFINES_CLASS(TStdXtra, XTRA_CLASS_VERSION)
CLASS_DEFINES_INTERFACE(TStdXtra, IMoaRegister, XTRA_VERSION_NUMBER)
CLASS_DEFINES_INTERFACE(TStdXtra, IMoaMmXTool, XTRA_VERSION_NUMBER)
END_XTRA_DEFINES_CLASS
END_XTRA

// when the Standard Xtra is Created, we Query for Interfaces which
// are provided by Director - similar to how Director will
// query this Standard Xtra for the IMoaRegister and IMoaMmXScript interfaces
// that it provides to Director for its use
STDMETHODIMP_(MoaError) MoaCreate_TStdXtra(TStdXtra* This) {
	moa_try

	ThrowNull(This);

	ThrowErr(This->pCallback->QueryInterface(&IID_IMoaMmValue, (PPMoaVoid)&This->moaMmValueInterfacePointer));
	ThrowNull(This->moaMmValueInterfacePointer);

	ThrowErr(This->pCallback->QueryInterface(&IID_IMoaDrPlayer, (PPMoaVoid)&This->moaDrPlayerInterfacePointer));
	ThrowNull(This->moaDrPlayerInterfacePointer);

	moa_catch
	moa_catch_end
	moa_try_end
}

// when the Standard Xtra is Destroyed, it'll Release the
// interfaces we Queried for from Director before
STDMETHODIMP_(void) MoaDestroy_TStdXtra(TStdXtra* This) {
	moa_try

	ThrowNull(This);

	if (This->moaMmValueInterfacePointer) {
		This->moaMmValueInterfacePointer->Release();
		This->moaMmValueInterfacePointer = NULL;
	}

	if (This->moaDrPlayerInterfacePointer) {
		This->moaDrPlayerInterfacePointer->Release();
		This->moaDrPlayerInterfacePointer = NULL;
	}

	moa_catch
	moa_catch_end
	// we use moa_try_end_void instead of moa_try_end here since this method returns a void type
	moa_try_end_void
}




// this says that the Constructor/Deconstructor for
// the IMoaRegister interface provided by
// the Standard Xtra class will not do anything
STD_INTERFACE_CREATE_DESTROY(TStdXtra, IMoaRegister)

// this allows Director to
// Query and Release the IMoaRegister interface provided
// by the Standard Xtra class
BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaRegister)
END_DEFINE_CLASS_INTERFACE

// the IMoaRegister interface is expected
// to implement a method called Register which
// will be called by Director after it Queries for
// this interface
// Director will call this method and
// pass in the arguments for it, which are
// pointers that we can point to data
// that will tell Director what this Xtra does
STDMETHODIMP TStdXtra_IMoaRegister::Register(PIMoaCache cacheInterfacePointer, PIMoaXtraEntryDict xtraEntryDictInterfacePointer) {
	moa_try

	ThrowNull(cacheInterfacePointer);
	ThrowNull(xtraEntryDictInterfacePointer);

	// register the Lingo Xtra
	PIMoaRegistryEntryDict registryEntryDictInterfacePointer = NULL;
	ThrowErr(cacheInterfacePointer->AddRegistryEntry(xtraEntryDictInterfacePointer, &CLSID_TStdXtra, &IID_IMoaMmXTool, &registryEntryDictInterfacePointer));
	ThrowNull(registryEntryDictInterfacePointer);

	// register Standard Tool Entries
	const char* displayNameString = "Enable";
	const char* displayCategoryString = "Movie Restorer 1.5.5";
	MoaBoolParam alwaysEnabled = TRUE;

	// private entries
	ThrowErr(registryEntryDictInterfacePointer->Put(kMoaMmDictType_DisplayNameString, displayNameString, 0, kMoaMmDictKey_DisplayNameString));
	ThrowErr(registryEntryDictInterfacePointer->Put(kMoaMmDictType_DisplayCategoryString, displayCategoryString, 0, kMoaMmDictKey_DisplayCategoryString));
	ThrowErr(registryEntryDictInterfacePointer->Put(kMoaDrDictType_AlwaysEnabled, &alwaysEnabled, 0, kMoaDrDictKey_AlwaysEnabled));

	moa_catch
	moa_catch_end
	moa_try_end
}




// this says that the Constructor/Deconstructor for
// the IMoaMmXScript interface provided by
// the Standard Xtra class will not do anything
STD_INTERFACE_CREATE_DESTROY(TStdXtra, IMoaMmXTool)

// this allows Director to
// Query and Release the IMoaMmXScript interface provided
// by the Standard Xtra class
BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaMmXTool)
END_DEFINE_CLASS_INTERFACE

// the IMoaMmXScript interface is expected
// to implement a method called Call which
// will be called by Director when
// a Lingo Script attempts to call
// one of the handlers
// defined in the Method Table which
// was registered in the Register method
// implemented by the IMoaRegister interface
// Director will call this method and
// pass in the argument for it, which is
// a pointer with data about the call, including
// which handler was called and the arguments passed
// to the handler from the Lingo Script that attempted
// to call it
STDMETHODIMP TStdXtra_IMoaMmXTool::Invoke() {
	moa_try

	// we arrive here from Tools first
	ThrowErr(XToolExtender());

	moa_catch
	moa_catch_end
	moa_try_end
}

STDMETHODIMP TStdXtra_IMoaMmXTool::GetEnabled(PMoaDrEnabledState pEnabledState) {
	moa_try

	/* This method is called by the host application to find out if
	our tool is enabled or not.  Disabled tools will appear
	differently in the user interface (such as a dimmed-out
	menu item or button), and can't be selected by the user.

	In this example, we have registered a TRUE value for the
	"alwaysEnabled" registry key.  This means that the host
	app will assume we're always enabled, and will never
	call this method.  This is done for performance and
	memory considerations: if a tool is marked as
	"alwaysEnabled", the code for the tool doesn't have
	to be loaded until the tool is selected (otherwise,
	it must be loaded as soon as the user clicks on the menu
	bar, since its GetEnabled() method must be called.

	So, if you're always enabled, declare "alwaysEnabled"
	as TRUE in your IMoaRegister::Register() method;
	if your enable state changes dynamically, declare
	"alwaysEnabled" as FALSE and set *pEnabledState to the
	appropriate value here when called.

	We're always enabled, so it doesn't matter what we do
	here. */

	ThrowNull(pEnabledState);

	/*
	* --> insert additional code -->
	*/

	*pEnabledState = kMoaDrEnabledState_Enabled;

	moa_catch
	moa_catch_end
	moa_try_end
}




// these methods are called from the Call method
// implemented by the IMoaMmXScript interface
// and correspond to handlers in Lingo upon
// the Method Table being registered by
// the Register method implemented by
// the IMoaRegister interface
MoaError TStdXtra_IMoaMmXTool::XToolExtender(PIMoaDrMovie moaDrMovieInterfacePointer) {
	moa_try

	ThrowNull(moaDrMovieInterfacePointer);

	if (!extender(moaDrMovieInterfacePointer, pObj->moaMmValueInterfacePointer, pObj->pCallback)) {
		terminateCurrentProcess();
		Throw(kMoaErr_InternalError);
	}

	if (!callLingoAlert("Movie Restorer is now enabled.", moaDrMovieInterfacePointer, pObj->moaMmValueInterfacePointer)) {
		terminateCurrentProcess();
		Throw(kMoaErr_InternalError);
	}

	moa_catch
	moa_catch_end
	moa_try_end
}

MoaError TStdXtra_IMoaMmXTool::XToolExtender() {
	PIMoaDrMovie moaDrMovieInterfacePointer = NULL;

	moa_try

	// get the Active Movie (so we can call a Lingo Handler in it if we need to)
	ThrowErr(pObj->moaDrPlayerInterfacePointer->GetActiveMovie(&moaDrMovieInterfacePointer));
	
	ThrowErr(XToolExtender(moaDrMovieInterfacePointer));

	moa_catch
	moa_catch_end

	// GOTTA SWEEP SWEEP SWEEP
	if (moaDrMovieInterfacePointer) {
		moaDrMovieInterfacePointer->Release();
	}

	moa_try_end
}
/* End Xtra */




/* Begin Extender */
bool getInterfaceModuleHandle(HMODULE &moduleHandle, /*MODULE module, */PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaCallback callbackPointer) {
	moduleHandle = NULL;

	if (!moaDrMovieInterfacePointer || !callbackPointer) {
		return false;
	}

	//if (module == MODULE_DIRECTOR_API) {
	// turn the Director API Interface into a Module Handle
	PMoaVoid moaVoidPointer = *(PPMoaVoid)moaDrMovieInterfacePointer;

	if (moaVoidPointer) {
		bool result = false;

		if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, (LPCSTR)moaVoidPointer, &moduleHandle)) {
			goto error;
		}

		result = true;
		error:
		moaVoidPointer = NULL;
		return result;
	}
	/*
	} else {
		MoaError err = kMoaErr_BadClass;
		PIMoaMmXScript scriptXtraInstance = NULL;

		// load the Xtra Instance's Interface, then turn it into a Module Handle
		if (module == MODULE_NET_LINGO_XTRA) {
			err = callbackPointer->MoaCreateInstance(&CLSID_CNetLingo, &IID_IMoaMmXScript, (PPMoaVoid)&scriptXtraInstance);
		} else if (module == MODULE_SHOCKWAVE_3D_ASSET_XTRA) {
			err = callbackPointer->MoaCreateInstance(&CLSID_CShockwave3DAsset, &IID_IMoaMmXScript, (PPMoaVoid)&scriptXtraInstance);
		}

		if (err != kMoaErr_NoErr) {
			return false;
		}

		if (scriptXtraInstance) {
			bool result = false;

			PMoaVoid moaVoidPointer = *(PPMoaVoid)scriptXtraInstance;

			if (moaVoidPointer) {
				if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, (LPCSTR)moaVoidPointer, &moduleHandle)) {
					goto error2;
				}

				result = true;
				error2:
				moaVoidPointer = NULL;
			}

			err = scriptXtraInstance->Release();
			scriptXtraInstance = NULL;

			if (err != kMoaErr_NoErr) {
				result = false;
			}
			return result;
		}
	}
	*/
	return false;
}

struct ModuleDirectorVersionTest {
	RELATIVE_VIRTUAL_ADDRESS codeRelativeVirtualAddress;
	VIRTUAL_SIZE codeVirtualSize;
	CODE1* testedCode;
};

MODULE_DIRECTOR_VERSION getModuleDirectorVersion(HMODULE moduleHandle, const size_t MODULE_DIRECTOR_VERSION_TESTS_SIZE, ModuleDirectorVersionTest moduleDirectorVersionTests[]) {
	// performs a test to see if this is a supported Director version
	// it's a simple array of bytes search, nothing more
	// we support 8.5, 10 and 11.5, so we perform three tests
	for (size_t i = 0; i < MODULE_DIRECTOR_VERSION_TESTS_SIZE; i++) {
		if (moduleDirectorVersionTests[i].codeVirtualSize) {
			if (testCode(moduleHandle, moduleDirectorVersionTests[i].codeRelativeVirtualAddress, moduleDirectorVersionTests[i].codeVirtualSize, moduleDirectorVersionTests[i].testedCode)) {
				return (MODULE_DIRECTOR_VERSION)i;
			}
		}
	}
	return MODULE_DIRECTOR_INCOMPATIBLE;
}

bool extender(PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaMmValue moaMmValueInterfacePointer, PIMoaCallback callbackPointer) {
	if (!moaDrMovieInterfacePointer || !moaMmValueInterfacePointer || !callbackPointer) {
		return false;
	}

	HMODULE moduleHandle = NULL;

	if (!getInterfaceModuleHandle(moduleHandle, moaDrMovieInterfacePointer, callbackPointer)) {
		callLingoAlertXtraMissing("Failed to Get Interface Module Handle", moaDrMovieInterfacePointer, moaMmValueInterfacePointer);
		return false;
	}

	// this variable is true when we've successfully written everything required
	// the procedure is mostly the same for all modules
	bool codeExtended = false;
	MODULE_DIRECTOR_VERSION directorAPIDirectorVersion = MODULE_DIRECTOR_INCOMPATIBLE;

	// test the code
	{
		const VIRTUAL_SIZE DIRECTOR_API_VERSION_8_TESTED_CODE_SIZE = 3;
		CODE1 directorAPIVersion8TestedCode[DIRECTOR_API_VERSION_8_TESTED_CODE_SIZE] = {0xFF, 0x14, 0x87};

		const VIRTUAL_SIZE DIRECTOR_API_VERSION_85_TESTED_CODE_SIZE = 19;
		CODE1 directorAPIVersion85TestedCode[DIRECTOR_API_VERSION_85_TESTED_CODE_SIZE] = {0x8D, 0x48, 0xFF, 0x81, 0xF9, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0x87, 0x9E, 0x01, 0x00, 0x00, 0x33, 0xD2, 0x8A, 0x91};

		#define DIRECTOR_API_VERSION_851_TESTED_CODE_SIZE DIRECTOR_API_VERSION_85_TESTED_CODE_SIZE
		#define directorAPIVersion851TestedCode directorAPIVersion85TestedCode

		#define DIRECTOR_API_VERSION_9_TESTED_CODE_SIZE DIRECTOR_API_VERSION_85_TESTED_CODE_SIZE
		#define directorAPIVersion9TestedCode directorAPIVersion85TestedCode

		const VIRTUAL_SIZE DIRECTOR_API_VERSION_10_TESTED_CODE_SIZE = 18;
		CODE1 directorAPIVersion10TestedCode[DIRECTOR_API_VERSION_10_TESTED_CODE_SIZE] = {0x8D, 0x48, 0xFF, 0x81, 0xF9, 0xF2, 0x00, 0x00, 0x00, 0x0F, 0x87, 0x7B, 0x22, 0x00, 0x00, 0xFF, 0x24, 0x8D};

		#define DIRECTOR_API_VERSION_101_TESTED_CODE_SIZE DIRECTOR_API_VERSION_10_TESTED_CODE_SIZE
		#define directorAPIVersion101TestedCode directorAPIVersion10TestedCode

		#define DIRECTOR_API_VERSION_1011_TESTED_CODE_SIZE DIRECTOR_API_VERSION_10_TESTED_CODE_SIZE
		#define directorAPIVersion1011TestedCode directorAPIVersion10TestedCode

		const VIRTUAL_SIZE DIRECTOR_API_VERSION_11_TESTED_CODE_SIZE = 15;
		CODE1 directorAPIVersion11TestedCode[DIRECTOR_API_VERSION_11_TESTED_CODE_SIZE] = {0x81, 0xF9, 0xF2, 0x00, 0x00, 0x00, 0x0F, 0x87, 0x72, 0x1F, 0x00, 0x00, 0x0F, 0xB6, 0x89};

		#define DIRECTOR_API_VERSION_1103_TESTED_CODE_SIZE DIRECTOR_API_VERSION_11_TESTED_CODE_SIZE
		#define directorAPIVersion1103TestedCode directorAPIVersion11TestedCode

		#define DIRECTOR_API_VERSION_115_TESTED_CODE_SIZE DIRECTOR_API_VERSION_11_TESTED_CODE_SIZE
		#define directorAPIVersion115TestedCode directorAPIVersion11TestedCode

		const VIRTUAL_SIZE DIRECTOR_API_VERSION_1158_TESTED_CODE_SIZE = 15;
		CODE1 directorAPIVersion1158TestedCode[DIRECTOR_API_VERSION_1158_TESTED_CODE_SIZE] = {0x81, 0xF9, 0xF2, 0x00, 0x00, 0x00, 0x0F, 0x87, 0xE2, 0x1F, 0x00, 0x00, 0x0F, 0xB6, 0x89};

		const VIRTUAL_SIZE DIRECTOR_API_VERSION_1159_TESTED_CODE_SIZE = 14;
		CODE1 directorAPIVersion1159TestedCode[DIRECTOR_API_VERSION_1159_TESTED_CODE_SIZE] = {0x3D, 0xF3, 0x00, 0x00, 0x00, 0x0F, 0x87, 0x8C, 0x20, 0x00, 0x00, 0x0F, 0xB6, 0x88};

		const VIRTUAL_SIZE DIRECTOR_API_VERSION_12_TESTED_CODE_SIZE = 14;
		CODE1 directorAPIVersion12TestedCode[DIRECTOR_API_VERSION_12_TESTED_CODE_SIZE] = {0x3D, 0xF3, 0x00, 0x00, 0x00, 0x0F, 0x87, 0xE7, 0x1F, 0x00, 0x00, 0x0F, 0xB6, 0x88};
			
		const size_t DIRECTOR_API_DIRECTOR_VERSION_TESTS_SIZE = 14;
		ModuleDirectorVersionTest directorAPIVersionTests[DIRECTOR_API_DIRECTOR_VERSION_TESTS_SIZE] = {{0x000EEC9C, DIRECTOR_API_VERSION_8_TESTED_CODE_SIZE, directorAPIVersion8TestedCode}, {0x000C8440, DIRECTOR_API_VERSION_85_TESTED_CODE_SIZE, directorAPIVersion85TestedCode}, {0x000C84C0, DIRECTOR_API_VERSION_851_TESTED_CODE_SIZE, directorAPIVersion851TestedCode}, {0x000BC9A0, DIRECTOR_API_VERSION_9_TESTED_CODE_SIZE, directorAPIVersion9TestedCode}, {0x000D5684, DIRECTOR_API_VERSION_10_TESTED_CODE_SIZE, directorAPIVersion10TestedCode}, {0x000D6A74, DIRECTOR_API_VERSION_101_TESTED_CODE_SIZE, directorAPIVersion101TestedCode}, {0x000D6E24, DIRECTOR_API_VERSION_1011_TESTED_CODE_SIZE, directorAPIVersion1011TestedCode}, {0x0010ED0E, DIRECTOR_API_VERSION_11_TESTED_CODE_SIZE, directorAPIVersion11TestedCode}, {0x0010EC9E, DIRECTOR_API_VERSION_1103_TESTED_CODE_SIZE, directorAPIVersion1103TestedCode}, {0x001118FE, DIRECTOR_API_VERSION_115_TESTED_CODE_SIZE, directorAPIVersion115TestedCode}, {0x0011245E, DIRECTOR_API_VERSION_1158_TESTED_CODE_SIZE, directorAPIVersion1158TestedCode}, {0x001149E3, DIRECTOR_API_VERSION_1159_TESTED_CODE_SIZE, directorAPIVersion1159TestedCode}, {0x00000000, 0, {}}, {0x00034BF8, DIRECTOR_API_VERSION_12_TESTED_CODE_SIZE, directorAPIVersion12TestedCode}};
			
		directorAPIDirectorVersion = getModuleDirectorVersion(moduleHandle, DIRECTOR_API_DIRECTOR_VERSION_TESTS_SIZE, directorAPIVersionTests);
	}

	if (directorAPIDirectorVersion == MODULE_DIRECTOR_INCOMPATIBLE) {
		callLingoAlertIncompatibleDirectorVersion("Failed to Get Module Director Version", moaDrMovieInterfacePointer, moaMmValueInterfacePointer);
		return false;
	}

	// extend the code
	switch (directorAPIDirectorVersion) {
		case MODULE_DIRECTOR_8:
		// populate the Module Handle Written Code Return Addresses, now that they're relevant
		// here is where the real writes happen at the appropriate addresses
		for (DWORD i = 0;i < 2;i++) {
			if (!extendCode(moduleHandle, 0x0002F471 + i)) {
				goto error;
			}
		}

		if (!extendCode(moduleHandle, 0x000726DE, (CODE1*)moduleHandle + 0x00072724)) {
			goto error;
		}

		codeExtended = extendCode(moduleHandle, 0x0006D3E4, (CODE1*)moduleHandle + 0x0006D40A);
		break;
		case MODULE_DIRECTOR_85:
		for (DWORD i = 0;i < 2;i++) {
			if (!extendCode(moduleHandle, 0x00026818 + i)) {
				goto error;
			}
		}

		if (!extendCode(moduleHandle, 0x0005F406, (CODE1*)moduleHandle + 0x0005F41C)) {
			goto error;
		}

		codeExtended = extendCode(moduleHandle, 0x0005AF05, (CODE1*)moduleHandle + 0x0005AF22);
		break;
		case MODULE_DIRECTOR_851:
		for (DWORD i = 0;i < 2;i++) {
			if (!extendCode(moduleHandle, 0x0002686A + i)) {
				goto error;
			}
		}

		if (!extendCode(moduleHandle, 0x0005F43F, (CODE1*)moduleHandle + 0x0005F455)) {
			goto error;
		}

		codeExtended = extendCode(moduleHandle, 0x0005AF3E, (CODE1*)moduleHandle + 0x0005AF5B);
		break;
		case MODULE_DIRECTOR_9:
		for (DWORD i = 0;i < 2;i++) {
			if (!extendCode(moduleHandle, 0x00026857 + i)) {
				goto error;
			}
		}

		if (!extendCode(moduleHandle, 0x0005F44C, (CODE1*)moduleHandle + 0x0005F462)) {
			goto error;
		}

		codeExtended = extendCode(moduleHandle, 0x0005AF48, (CODE1*)moduleHandle + 0x0005AF65);
		break;
		case MODULE_DIRECTOR_10:
		for (DWORD i = 0;i < 2;i++) {
			if (!extendCode(moduleHandle, 0x000289A2 + i)) {
				goto error;
			}
		}

		if (!extendCode(moduleHandle, 0x000622C8, (CODE1*)moduleHandle + 0x000622DE)) {
			goto error;
		}

		codeExtended = extendCode(moduleHandle, 0x0005DD13, (CODE1*)moduleHandle + 0x0005DD30);
		break;
		case MODULE_DIRECTOR_101:
		for (DWORD i = 0; i < 2; i++) {
			if (!extendCode(moduleHandle, 0x00028E37 + i)) {
				goto error;
			}
		}

		if (!extendCode(moduleHandle, 0x000636D8, (CODE1*)moduleHandle + 0x000636EE)) {
			goto error;
		}

		codeExtended = extendCode(moduleHandle, 0x0005EF2A, (CODE1*)moduleHandle + 0x0005EF47);
		break;
		case MODULE_DIRECTOR_1011:
		for (DWORD i = 0; i < 2; i++) {
			if (!extendCode(moduleHandle, 0x00028E84 + i)) {
				goto error;
			}
		}

		if (!extendCode(moduleHandle, 0x0006372C, (CODE1*)moduleHandle + 0x00063742)) {
			goto error;
		}

		codeExtended = extendCode(moduleHandle, 0x0005EF82, (CODE1*)moduleHandle + 0x0005EF9F);
		break;
		case MODULE_DIRECTOR_11:
		for (DWORD i = 0;i < 2;i++) {
			if (!extendCode(moduleHandle, 0x000345FE + i)) {
				goto error;
			}
		}

		if (!extendCode(moduleHandle, 0x0008189D, (CODE1*)moduleHandle + 0x000818BD)) {
			goto error;
		}

		codeExtended = extendCode(moduleHandle, 0x00076FBC, (CODE1*)moduleHandle + 0x00076FE1);
		break;
		case MODULE_DIRECTOR_1103:
		for (DWORD i = 0;i < 2;i++) {
			if (!extendCode(moduleHandle, 0x0003448E + i)) {
				goto error;
			}
		}

		if (!extendCode(moduleHandle, 0x0008176D, (CODE1*)moduleHandle + 0x0008178D)) {
			goto error;
		}

		codeExtended = extendCode(moduleHandle, 0x00076E8C, (CODE1*)moduleHandle + 0x00076EB1);
		break;
		case MODULE_DIRECTOR_115:
		for (DWORD i = 0;i < 2;i++) {
			if (!extendCode(moduleHandle, 0x00034C4B + i)) {
				goto error;
			}
		}

		if (!extendCode(moduleHandle, 0x0008413D, (CODE1*)moduleHandle + 0x0008415D)) {
			goto error;
		}

		codeExtended = extendCode(moduleHandle, 0x000796DC, (CODE1*)moduleHandle + 0x00079701);
		break;
		case MODULE_DIRECTOR_1158:
		for (DWORD i = 0;i < 2;i++) {
			if (!extendCode(moduleHandle, 0x000351AE + i)) {
				goto error;
			}
		}

		if (!extendCode(moduleHandle, 0x00084A9D, (CODE1*)moduleHandle + 0x00084ABD)) {
			goto error;
		}

		codeExtended = extendCode(moduleHandle, 0x00079F6C, (CODE1*)moduleHandle + 0x00079F91);
		break;
		case MODULE_DIRECTOR_1159:
		for (DWORD i = 0;i < 2;i++) {
			if (!extendCode(moduleHandle, 0x000353FE + i)) {
				goto error;
			}
		}

		if (!extendCode(moduleHandle, 0x000855BD, (CODE1*)moduleHandle + 0x000855DD)) {
			goto error;
		}

		codeExtended = extendCode(moduleHandle, 0x0007AA6C, (CODE1*)moduleHandle + 0x0007AA91);
		break;
		case MODULE_DIRECTOR_12:
		for (DWORD i = 0;i < 2;i++) {
			if (!extendCode(moduleHandle, 0x0008673C + i)) {
				goto error;
			}
		}

		if (!extendCode(moduleHandle, 0x000D431D, (CODE1*)moduleHandle + 0x000D433C)) {
			goto error;
		}

		codeExtended = extendCode(moduleHandle, 0x000C9B51, (CODE1*)moduleHandle + 0x000C9B71);
	}

	// in case we failed and didn't catch it somehow
	if (!codeExtended) {
		error:
		callLingoAlertAntivirus("Failed to Extend Code", moaDrMovieInterfacePointer, moaMmValueInterfacePointer);
		return false;
	}
	return true;
}
/* End Extender */




/*
Psalm 150

Praise the Lord.

Praise God in his sanctuary;
	praise him in his mighty heavens.
Praise him for his acts of power;
	praise him for his surpassing greatness.
Praise him with the sounding of the trumpet,
	praise him with the harp and lyre,
praise him with timbrel and dancing,
	praise him with the strings and pipe,
praise him with the clash of cymbals,
	praise him with resounding cymbals.

Let everything that has breath praise the Lord.

Praise the Lord.
*/