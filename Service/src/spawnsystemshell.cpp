//========= Copyright Brandon Keyes, All rights reserved. ============//
//
// Purpose: Spawns a Windows command line shell as "NT AUTHORITY\SYSTEM" for
// administrative purposes.
//
//====================================================================//

#include "spawnsystemshell.h"
#include <Windows.h>
#include <TlHelp32.h>

//----------------------------------------------------------------------
// All Windows services execute with with the session ID of 0 which is non interactive. We want our service
// interactive but it is not possible to change a process's session ID, therefore we will spawn a command line interface
// as SYSTEM by getting our token and changing it's session ID to the logged in user's session ID and then starting our
// process with CreateProcessAsUser()
//----------------------------------------------------------------------
int spawnSystemShell ()
{
    // Handles and variables
    HANDLE handle_user_process;
    HANDLE handle_current_process;
    HANDLE handle_process_token;
    HANDLE handle_duplicated_token;
    HANDLE handle_user_token;
    DWORD user_session_id;
    int status;

    // SECURITY_ATTRIBUTES structure for our new token
    SECURITY_ATTRIBUTES security_attributes;
    security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    security_attributes.lpSecurityDescriptor = NULL; // Default SECURITY_DESCRIPTOR
    security_attributes.bInheritHandle = true;

    // Get a handle to out current process
    handle_current_process = GetCurrentProcess();

    // Get a handle to our process's token for duplication
    OpenProcessToken(handle_current_process, TOKEN_DUPLICATE, &handle_process_token);

    // Duplicate token with either MAXIMUM_ALLOWED or TOKEN_ALL_ACCESS for modifying the session ID
    DuplicateTokenEx(handle_process_token, MAXIMUM_ALLOWED, &security_attributes, SecurityImpersonation, TokenPrimary, &handle_duplicated_token); // Or use NULL for lpTokenAttributes

    // Before we can edit our token we need to get the session ID of the logged on user.
    // We will attempt to get this from the token that explorer.exe is running under
    // We could also enumerate through all sessions with WTSEnumerateSessions() and
    // check if a session is active if it's state == WTSActive
	PROCESSENTRY32 process_entry;
    process_entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE process_snapshots = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (Process32First(process_snapshots, &process_entry) == TRUE)
    {
        while (Process32Next(process_snapshots, &process_entry) == TRUE)
        {
            if (wcscmp(process_entry.szExeFile, L"explorer.exe") == 0)
            {
                handle_user_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_entry.th32ProcessID);
                OpenProcessToken(handle_user_process, TOKEN_QUERY, &handle_user_token);

                // Get user's session ID
                DWORD token_length;
                GetTokenInformation(handle_user_token, TokenSessionId, &user_session_id, sizeof(DWORD), &token_length);
            }
        }
    }

    // Set our token's session ID to the user's
    SetTokenInformation(handle_duplicated_token, TokenSessionId, &user_session_id, sizeof(DWORD));

    // Finally spawn cmd.exe as SYSTEM in the user's session
	STARTUPINFO startup_info;
	ZeroMemory(&startup_info, sizeof(STARTUPINFO));
	startup_info.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION process_information;
    status = CreateProcessAsUser(handle_duplicated_token, L"C:\\Windows\\System32\\cmd.exe", NULL, NULL, NULL, true, NORMAL_PRIORITY_CLASS, NULL, NULL, &startup_info, &process_information);

    if (status == 0)
    {
      return GetLastError();
    }
    return 0;
}
