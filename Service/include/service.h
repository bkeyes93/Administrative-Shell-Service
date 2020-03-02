//========= Copyright Brandon Keyes, All rights reserved. ============//
//
// Purpose: Declare service related data and functions.
//
//====================================================================//

#ifndef SERVICE_H
#define SERVICE_H

#include <Windows.h>

#define SERVICE_NAME L"Administrative Shell" // Modify for custom service name.

int startService ();
VOID WINAPI serviceMain (DWORD argc, LPTSTR *argv);
DWORD WINAPI serviceCtrlHandler (DWORD ctrl_command, DWORD event_type, LPVOID event_data, LPVOID context);

#endif
