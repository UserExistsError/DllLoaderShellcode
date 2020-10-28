#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "ReflectiveLoader.h"

/*
Execute shellcode from file.
*/

BOOL ReadFileData(WCHAR *filename, BYTE **buff, DWORD *size);

int SetOrdinal(BYTE* code, DWORD codeSize, DWORD ordinal)
{
	if (ordinal <= 0 || ordinal > 65535) {
		wprintf(L"Invalid ordinal. Must be [1, 65535]");
		return 0;
	}
	for (DWORD i = 0; i < codeSize - 3; i++) {
		if (*(DWORD*)(code + i) == ORDINAL_PLACEHOLDER) {
			wprintf(L"Found ordinal placeholder at offset 0x%lx", i);
			DWORD* dwCodePtr = (DWORD*)(code + i);
			*dwCodePtr = ordinal;
			return 1;
		}
	}
	wprintf(L"Failed to find ordinal placeholder: 0x%lx", ORDINAL_PLACEHOLDER);
	return 0;
}

int wmain(int argc, WCHAR *argv[])
{
	if (argc < 2) {
		wprintf(L"usage: TestExe.exe <SHELLCODE> [ORDINAL]\n");
		return 1;
	}

	BYTE *code = NULL;
	DWORD codeSize = 0;
	if (!ReadFileData(argv[1], &code, &codeSize)) {
		wprintf(L"Failed to read file: %s\n", argv[1]);
		return 1;
	}
	if (argc == 3) {
		WCHAR* endPtr = argv[2];
		DWORD ordinal = wcstoul(argv[2], &endPtr, 10);
		if (endPtr[0] != L'\0') {
			wprintf(L"Failed to parse ordinal from input: %s", argv[2]);
			return 1;
		}
		if (!SetOrdinal(code, codeSize, ordinal)) {
			return 1;
		}
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
