#include <windows.h>

// Prototype for native system version call
typedef DWORD (WINAPI *pGetFileVersionInfoSizeA)(LPCSTR, LPDWORD);
pGetFileVersionInfoSizeA pOriginalGetFileVersionInfoSizeA = NULL;

// In-Memory IAT Overwrite Engine
void PatchImportAddressTable(void) {
    HMODULE hExplorer = GetModuleHandleA(NULL); // Get current process base (explorer.exe)
    HMODULE hReal81 = LoadLibraryA("shunimpl_real.dll");
    if (!hExplorer || !hReal81) return;

    // Parse PE Headers to locate the Import Directory
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hExplorer;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hExplorer + dosHeader->e_lfanew);
    PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)hExplorer + 
        ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    // Loop through all imported DLL modules
    while (importDesc->Name) {
        LPCSTR pszModName = (LPCSTR)((BYTE*)hExplorer + importDesc->Name);
        
        // Match the obsolete target library name
        if (lstrcmpiA(pszModName, "shunimpl.dll") == 0) {
            PIMAGE_THUNK_DATA thunkILT = (PIMAGE_THUNK_DATA)((BYTE*)hExplorer + importDesc->Characteristics);
            PIMAGE_THUNK_DATA thunkIAT = (PIMAGE_THUNK_DATA)((BYTE*)hExplorer + importDesc->FirstThunk);

            // Loop through each function slot inside the module import table
            while (thunkIAT->u1.Function) {
                // Check if the import is tracked via raw numeric Ordinal index
                if (IMAGE_SNAP_BY_ORDINAL(thunkILT->u1.Ordinal)) {
                    DWORD ordinalNum = IMAGE_ORDINAL(thunkILT->u1.Ordinal);
                    
                    // Isolate target ordinals #240, #250, and #290
                    if (ordinalNum == 240 || ordinalNum == 250 || ordinalNum == 290) {
                        FARPROC pRealFunc = GetProcAddress(hReal81, (LPCSTR)(UINT_PTR)ordinalNum);
                        if (pRealFunc) {
                            // Temporarily strip write protection from the memory page block
                            DWORD oldProtect;
                            VirtualProtect(&thunkIAT->u1.Function, sizeof(ULONG_PTR), PAGE_READWRITE, &oldProtect);
                            
                            // Hard overwrite the IAT function pointer with our live 8.1 offset!
                            thunkIAT->u1.Function = (ULONG_PTR)pRealFunc;
                            
                            // Restore original page verification states
                            VirtualProtect(&thunkIAT->u1.Function, sizeof(ULONG_PTR), oldProtect, &oldProtect);
                        }
                    }
                }
                thunkILT++;
                thunkIAT++;
            }
            break;
        }
        importDesc++;
    }
}

void InjectSubsystemContext(void) {
    // 1. Force rewrite explorer's IAT memory tables to redirect missing components
    PatchImportAddressTable();

    // 2. Load the native system version library for default operations
    char sysPath[MAX_PATH];
    GetSystemDirectoryA(sysPath, MAX_PATH);
    strcat(sysPath, "\\version.dll");
    HMODULE hSysVersion = LoadLibraryA(sysPath);
    if (hSysVersion) {
        pOriginalGetFileVersionInfoSizeA = (pGetFileVersionInfoSizeA)GetProcAddress(hSysVersion, "GetFileVersionInfoSizeA");
    }
}

// System Proxy Export - Satisfies the initial hardcoded dependency check on boot
__declspec(dllexport) DWORD WINAPI GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle) {
    if (!pOriginalGetFileVersionInfoSizeA) return 0;
    return pOriginalGetFileVersionInfoSizeA(lptstrFilename, lpdwHandle);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            InjectSubsystemContext();
            break;
    }
    return TRUE;
}
