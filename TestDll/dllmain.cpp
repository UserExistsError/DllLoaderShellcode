#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

VOID ExportOne()
{
    MessageBoxA(0, "Export @1", "", 0);
}

VOID ExportThree()
{
    MessageBoxA(0, "Export @3", "", 0);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		//OutputDebugStringA("DLL_PROCESS_ATTACH");
		MessageBoxA(0, "DllMain!", "", 0);
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
