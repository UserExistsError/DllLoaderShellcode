#include <Windows.h>
#include <stdio.h>


BOOL ReadFileData(WCHAR *filename, BYTE **buff, DWORD *size);

const IMAGE_NT_HEADERS * GetNtHeader(const BYTE *image, const DWORD imageSize)
{
	const IMAGE_DOS_HEADER *dosHeader = (IMAGE_DOS_HEADER*)image;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		wprintf(L"Invalid DOS header\n");
		return NULL;
	}
	const IMAGE_NT_HEADERS *ntHeader = (IMAGE_NT_HEADERS*)((ULONG_PTR)image + dosHeader->e_lfanew);
	if ((BYTE*)ntHeader < image) {
		wprintf(L"Invalid NT header\n");
		return NULL;
	}
	if ((BYTE*)ntHeader > (image + imageSize - sizeof(IMAGE_NT_HEADERS))) {
		wprintf(L"Invalid NT header\n");
		return NULL;
	}
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
		wprintf(L"Invalid NT header\n");
		return NULL;
	}
	return ntHeader;
}

int wmain(int argc, WCHAR *argv[])
{
	if (argc < 3) {
		wprintf(L"usage: Extract.exe <PEFILE> <SHELLCODE>\n");
		return 1;
	}
	
	DWORD imageSize = 0;
	BYTE *image = NULL;
	if (!ReadFileData(argv[1], &image, &imageSize)) {
		wprintf(L"Failed to read file: %s\n", argv[1]);
		return 1;
	}

	// get .text section
	const IMAGE_NT_HEADERS *ntHeader = GetNtHeader(image, imageSize);
	if (ntHeader == NULL)
		return 1;

	IMAGE_SECTION_HEADER *section = IMAGE_FIRST_SECTION(ntHeader);
	IMAGE_SECTION_HEADER *codeSection = NULL;
	for (size_t i = 0; i < ntHeader->FileHeader.NumberOfSections; i++, section++) {
		if ((BYTE*)section > (image + imageSize - sizeof(IMAGE_SECTION_HEADER))) {
			wprintf(L"Invalid section header\n");
			return 1;
		}
		if (memcmp(".text\x00", section->Name, 6) == 0) {
			codeSection = section;
			break;
		}
	}
	if (codeSection == NULL) {
		wprintf(L"Failed to find code section\n");
		return 1;
	}

	// write .text section to file
	const BYTE *text = image + codeSection->PointerToRawData;
	if (text < image) {
		wprintf(L"Invalid .text section\n");
		return 1;
	}
	if ((text + codeSection->Misc.VirtualSize < text) || (text + codeSection->Misc.VirtualSize) > (image + imageSize)) {
		wprintf(L"Invalid .text section\n");
		return 1;
	}
	HANDLE hFile = CreateFileW(argv[2], GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		wprintf(L"Failed to open file: %s\n", argv[2]);
		return 1;
	}
	DWORD numWritten = 0;
	DWORD numLeft = codeSection->Misc.VirtualSize;
	BYTE *textPtr = (BYTE*)text;
	while (numLeft > 0) {
		if (!WriteFile(hFile, textPtr, numLeft, &numWritten, NULL)) {
			wprintf(L"Failed to write to file: %s\n", argv[2]);
			return 1;
		}
		numLeft -= numWritten;
		textPtr += numWritten;
	}
	wprintf(L"Wrote %lu bytes to %s\n", codeSection->Misc.VirtualSize, argv[2]);
	return 0;
}

