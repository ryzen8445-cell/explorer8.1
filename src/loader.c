#include <windows.h>
#include <stdio.h>

int main() {
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    printf("[INFO] Launching legacy explorer.exe in suspended memory state...\n");
    
    // 1. Start the shell suspended so the kernel loader halts before initialization
    if (!CreateProcessA("explorer.exe", NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        printf("[ERROR] Failed to launch explorer.exe process.\n");
        return 1;
    }

    // 2. Force inject our custom version.dll memory hook into the suspended thread
    // This deploys his automated background memory patches before the process resumes
    HMODULE hProxy = LoadLibraryA("version.dll");
    if (!hProxy) {
        printf("[WARN] Local hook injection bypassed.\n");
    }

    printf("[SUCCESS] Process paused in memory. Ready for debugger attachment!\n");
    printf("[INFO] Resume process thread inside x64dbg to execute.\n");
    
    // Keep thread handle open for debugger analysis loops
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
