//========= Copyright Brandon Keyes, All rights reserved. ============//
//
// Purpose: Set up named pipes for service communication.
//
//====================================================================//

#include "namedpipe.h"
#include <Windows.h>

HANDLE named_pipe_handle;

int createNamedPipe () 
{
	// Define the security attributes for our named pipe.
	// We need to set this so anyone can connect to our named pipe.
	SECURITY_DESCRIPTOR security_descriptor;
	InitializeSecurityDescriptor(&security_descriptor, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&security_descriptor, TRUE, NULL, FALSE); // NULL DACL allows access to EVERYONE.
	SECURITY_ATTRIBUTES security_attributes;
	security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	security_attributes.lpSecurityDescriptor = &security_descriptor;
	security_attributes.bInheritHandle = true;

	// Create named pipe.
	named_pipe_handle = CreateNamedPipe(L"\\\\.\\pipe\\shell", PIPE_ACCESS_INBOUND, PIPE_TYPE_BYTE | PIPE_NOWAIT, 1, 1, 1, 0, &security_attributes);
	
	// TODO: Add error checking for if CreateNamedPipe() == INVALID_HANDLE_VALUE.

	return 0;
}

bool isShellEnabled()
{
	unsigned char status = 0x00;
	DWORD read;
	if (ReadFile(named_pipe_handle, &status, 1, &read, NULL))
	{
		// If we have recieved data then reset named pipe by disconnecting client and allowing re-connection.
		DisconnectNamedPipe(named_pipe_handle);
		ConnectNamedPipe(named_pipe_handle, NULL);

		// If correct byte is sent then enable shell.
		if (status == 0xFF)
			return true;
	}
	return false;
}