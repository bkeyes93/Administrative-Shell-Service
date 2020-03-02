//========= Copyright Brandon Keyes, All rights reserved. ============//
//
// Purpose: Set up and run the service.
//
//====================================================================//

#include "service.h"
#include "spawnsystemshell.h"
#include "namedpipe.h"
#include <Windows.h>

SERVICE_STATUS service_status;
bool service_stopped = false;

int startService ()
{
  // StartServiceCtrlDispatcher needs a NULL terminated SERVICE_TABLE_ENTRY array.
  // Could use initializer list instead.
  wchar_t service_name[] = SERVICE_NAME;
  SERVICE_TABLE_ENTRY service_table[2];
  service_table[0].lpServiceName = service_name;
  service_table[0].lpServiceProc = &serviceMain;
  service_table[1].lpServiceName = NULL;
  service_table[1].lpServiceProc = NULL;

  // Give the SCM our service's main function.
  if (StartServiceCtrlDispatcher(service_table) == 0)
  {
    return GetLastError();
  }
  return 0;
}

//----------------------------------------------------------------------
// Our service's main funtion and loop.
//----------------------------------------------------------------------
VOID WINAPI serviceMain (DWORD argc, LPTSTR *argv)
{
  // Register our service control handler with the SCM.
  SERVICE_STATUS_HANDLE service_status_handle;
  service_status_handle = RegisterServiceCtrlHandlerEx(SERVICE_NAME, &serviceCtrlHandler, NULL);

  // TODO: Add error checking for if service_status_handle == 0

  // Tell the SCM we are started.
  ZeroMemory(&service_status, sizeof(service_status));
  service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  service_status.dwCurrentState = SERVICE_RUNNING;
  service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP; // Only accept stop command from SCM.
  service_status.dwWin32ExitCode = NO_ERROR;
  service_status.dwCheckPoint = 0;
  SetServiceStatus(service_status_handle, &service_status);

  // TODO: Add error checking for if SetServiceStatus() == 0

  createNamedPipe(); // Create a named pipe for communication to service.

  // This is our service's main loop.
  while (!service_stopped)
  {
	  // Check if shell should be spawned.
	  if (isShellEnabled())
		  spawnSystemShell();
  }

  // Main loop has exited so tell the SCM we are stopped.
  service_status.dwCurrentState = SERVICE_STOPPED;
  service_status.dwControlsAccepted = 0; // Don't accept any commands from the SCM.
  service_status.dwWin32ExitCode = NO_ERROR;
  service_status.dwCheckPoint = 0;
  SetServiceStatus(service_status_handle, &service_status);
}

//----------------------------------------------------------------------
// This function responds to commands that the SCM
// sends to our service.
//----------------------------------------------------------------------
DWORD WINAPI serviceCtrlHandler (DWORD ctrl_command, DWORD event_type, LPVOID event_data, LPVOID context)
{
  switch (ctrl_command)
  {
    case SERVICE_CONTROL_STOP:
      if (service_status.dwCurrentState == SERVICE_RUNNING)
		service_stopped = true;
      break;
    default:
      break;
  }
  return NO_ERROR;
}
