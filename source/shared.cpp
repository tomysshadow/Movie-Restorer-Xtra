#include "shared.h"
#include <string>
#include <sstream>
#include <windows.h>

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