//========= Copyright Brandon Keyes, All rights reserved. ============//
//
// Purpose: Tell service, using a named pipe, to launch a shell.
//
//====================================================================//

#include <iostream>
#include <Windows.h>

int main (int argc, char *argv[])
{
	std::cout << "Starting Administrative Shell..." << std::endl;
	HANDLE named_pipe_handle;
	named_pipe_handle = CreateFile(L"\\\\.\\pipe\\shell", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	unsigned char status = 0xFF;
	DWORD read;
	std::cout << "Writing 0xFF to pipe: \\\\.\\pipe\\shell" << std::endl;
	WriteFile(named_pipe_handle, &status, 1, &read, NULL);

	CloseHandle(named_pipe_handle);
	return 0;
}