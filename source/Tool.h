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

#include "shared.h"

#include "moastdif.h"
#include "mmixtool.h"
#include "driservc.h"
#include "xmmvalue.h"

#include <windows.h>




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
	PIMoaDrPlayer moaDrPlayerInterfacePointer = NULL;
	PIMoaMmValue moaMmValueInterfacePointer = NULL;
EXTERN_END_DEFINE_CLASS_INSTANCE_VARS

EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaRegister)
	EXTERN_DEFINE_METHOD(MoaError, Register, (PIMoaCache, PIMoaXtraEntryDict))
EXTERN_END_DEFINE_CLASS_INTERFACE

EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaMmXTool)
	EXTERN_DEFINE_METHOD(MoaError, Invoke, ())
	EXTERN_DEFINE_METHOD(MoaError, GetEnabled, (PMoaDrEnabledState))
	private:
	EXTERN_DEFINE_METHOD(MoaError, Extender, (PIMoaDrMovie))
	EXTERN_DEFINE_METHOD(MoaError, Extender, ())
EXTERN_END_DEFINE_CLASS_INTERFACE




inline bool callLingoQuit(PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaMmValue moaMmValueInterfacePointer) {
	// we can't just use Win32 API's TerminateProcess because it causes a long pause before closing the movie improperly
	// so we use Lingo's Quit handler
	if (!moaDrMovieInterfacePointer || !moaMmValueInterfacePointer) {
		return false;
	}

	MoaMmSymbol quitSymbol = 0;
	MoaError err = moaMmValueInterfacePointer->StringToSymbol("Quit", &quitSymbol);

	if (err != kMoaErr_NoErr) {
		return false;
	}

	err = moaDrMovieInterfacePointer->CallHandler(quitSymbol, 0, NULL, NULL);

	if (err != kMoaErr_NoErr) {
		return false;
	}
	return true;
}

inline bool callLingoAlert(ConstPMoaChar messageString, PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaMmValue moaMmValueInterfacePointer) {
	// we can't just use Win32 API's MessageBox or it'll cause graphical glitches from the movie not being paused
	// so we use Lingo's Alert handler (as per the XDK's recommendation)
	if (!messageString || !moaDrMovieInterfacePointer || !moaMmValueInterfacePointer) {
		return false;
	}

	MoaMmSymbol alertSymbol = 0;
	MoaError err = moaMmValueInterfacePointer->StringToSymbol("Alert", &alertSymbol);

	if (err != kMoaErr_NoErr) {
		return false;
	}

	MoaMmValue messageValue = kVoidMoaMmValueInitializer;
	err = moaMmValueInterfacePointer->StringToValue(messageString, &messageValue);

	if (err != kMoaErr_NoErr) {
		return false;
	}

	bool result = false;
	err = moaDrMovieInterfacePointer->CallHandler(alertSymbol, 1, &messageValue, NULL);

	if (err != kMoaErr_NoErr) {
		goto error;
	}
	result = true;
	error:
	err = moaMmValueInterfacePointer->ValueRelease(&messageValue);

	if (err != kMoaErr_NoErr) {
		result = false;
	}
	return result;
}

inline bool callLingoAlertXtraMissing(ConstPMoaChar messageString, PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaMmValue moaMmValueInterfacePointer) {
	if (!messageString || !moaDrMovieInterfacePointer || !moaMmValueInterfacePointer) {
		return false;
	}

	if (!callLingoAlert(messageString, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
		return false;
	}

	if (!callLingoAlert("An Xtra that the Movie Restorer Xtra requires, such as the NetLingo Xtra or Shockwave 3D Asset Xtra, is missing.", moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
		return false;
	}
	return true;
}

inline bool callLingoAlertIncompatibleDirectorVersion(ConstPMoaChar messageString, PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaMmValue moaMmValueInterfacePointer) {
	if (!messageString || !moaDrMovieInterfacePointer || !moaMmValueInterfacePointer) {
		return false;
	}

	if (!callLingoAlert(messageString, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
		return false;
	}

	if (!callLingoAlert("The Movie Restorer Xtra has determined it is incompatible with this Director version.", moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
		return false;
	}
	return true;
}

inline bool callLingoAlertAntivirus(ConstPMoaChar messageString, PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaMmValue moaMmValueInterfacePointer) {
	if (!messageString || !moaDrMovieInterfacePointer || !moaMmValueInterfacePointer) {
		return false;
	}

	if (!callLingoAlert(messageString, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
		return false;
	}

	if (!callLingoAlert("Your antivirus may be preventing the Movie Restorer Xtra from working properly.", moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
		return false;
	}
	return true;
}




bool extender(PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaMmValue moaMmValueInterfacePointer, PIMoaCallback callbackPointer);

#endif