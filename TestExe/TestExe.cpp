#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>

/*
Execute shellcode from file.
*/

BOOL ReadFileData(WCHAR *filename, BYTE **buff, DWORD *size);

int wmain(int argc, WCHAR *argv[])
{
	if (argc < 2) {
		wprintf(L"usage: TestExe.exe <SHELLCODE>\n");
		return 1;
	}

	BYTE *code = NULL;
	DWORD codeSize = 0;
	if (!ReadFileData(argv[1], &code, &codeSize)) {
		wprintf(L"Failed to read file: %s\n", argv[1]);
		return 1;
	}
	BYTE *rwx = (BYTE*)VirtualAlloc(0, codeSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (rwx == NULL) {
		wprintf(L"Failed to allocate shellcode buffer\n");
		return 1;
	}

	CopyMemory(rwx, code, codeSize);
	FARPROC fp = (FARPROC)rwx;
	fp();
	return 0;
}
