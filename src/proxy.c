#include <windows.h>

// Global storage tracking our native system version call
typedef DWORD (WINAPI *pGetFileVersionInfoSizeA)(LPCSTR, LPDWORD);
pGetFileVersionInfoSizeA pOriginalGetFileVersionInfoSizeA = NULL;

void* pRealOrdinal240 = NULL;
void* pRealOrdinal250 = NULL;
void* pRealOrdinal290 = NULL;

// Raw export stub functions to bind against our numeric definitions
void __attribute__((naked)) ExportStub240(void) { __asm__ volatile("jmp *%0" : : "m"(pRealOrdinal240)); }
void __attribute__((naked)) ExportStub250(void) { __asm__ volatile("jmp *%0" : : "m"(pRealOrdinal250)); }
void __attribute__((naked)) ExportStub290(void) { __asm__ volatile("jmp *%0" : : "m"(pRealOrdinal290)); }

void InjectSubsystemContext(void) {
    // 1. Map the authentic Windows 8.1 legacy target module
    HMODULE hReal = LoadLibraryA("shunimpl_real.dll");
    if (hReal) {
        pRealOrdinal240 = (void*)GetProcAddress(hReal, (LPCSTR)240);
        pRealOrdinal250 = (void*)GetProcAddress(hReal, (LPCSTR)250);
        pRealOrdinal290 = (void*)GetProcAddress(hReal, (LPCSTR)290);
    }

    // 2. Resolve and link the native system version library for default operations
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
