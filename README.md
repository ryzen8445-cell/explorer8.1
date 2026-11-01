# Explorer8.1

An experimental, low-level reverse-engineering framework dedicated to backporting the native, untouched Windows 8.1 Desktop Shell and File Explorer environment (NT 6.3) to run natively on modern Windows 10 and Windows 11 kernels.

Rather than relying on visual skinning tools or UI wrappers, this project utilizes local module isolation and custom proxy hooks to safely initialize real legacy UX subsystems alongside modern security environments.

## 🚀 Recent Breakthroughs & Milestones
During our recent dynamic binary analysis and debugging sessions inside isolated sandboxes, we achieved a monumental milestone: The legacy shell successfully broke through early loader checks, initialized its core subsystems, and actively mapped its primary window station interface onto the host system (evidenced by the visual rendering of the OS desktop watermark layer).

This was accomplished by transitioning away from manual runtime debugger interventions inside `x64dbg` and moving toward an automated, compiled memory hook architecture. 

Furthermore, we have successfully resolved the **C++ Name Mangling Blocker** that previously corrupted the Export Address Table (EAT). By shifting our compilation pipeline to pure C via the GCC toolchain (`w64devkit`), we can natively export true, nameless numeric ordinal slots with hidden text records (`NONAME`), establishing a reliable binary bridge to legacy components.

## Integrated Codebase Features
*   **Automated CFG Bypass:** The custom proxy layer dynamically scans memory space to identify `ntdll!LdrpValidateUserCallTarget`. It programmatically overrides the Control Flow Guard verification loop header with an inline binary patch (`0x31 0xC0 0xC3` / `xor eax, eax ; ret`), automatically reporting a flawless validation status (`STATUS_SUCCESS`) on every execution path.
*   **Initialization Loop Mitigation:** By embedding code-level interception, the framework successfully satisfies the strict environment queries of cascading dependencies (such as `umpdc.dll` and `imm32.dll`), breaking the application out of its persistent `STATUS_DLL_INIT_FAILED (0xc0000142)` roadblock.
*   **Pure C Ordinal Proxying:** A stable, high-performance C-linkage architecture utilizing raw x64 naked assembly jumps (`__attribute__((naked))`) to intercept, filter, and pass hardware registers straight to underlying libraries without stack frame distortion.

## 🛑 Current Active Development Blocker: Kernel Loader Isolation Policy
With the initial function mapping traps cleared and the UI engine attempting to mount, the architecture is currently tracking a deep system constraint where the modern kernel environment implements rigid internal security policies specifically targeting `explorer.exe`.

### The Shell Undo Implementation Barrier (`shunimpl.dll`)
*   **Symptom:** A hard process termination throwing `STATUS_ORDINAL_NOT_FOUND (0xc0000138)` when executing natively or attempting to swap the active shell via `taskkill /f /im explorer.exe`.
*   **The Root Cause:** Live debugging inside `x64dbg` revealed that the modern NT kernel loader enforces strict security mitigations on `explorer.exe` that **completely ignore local application search paths (`.local` isolation) and standard App Paths registry overrides.** Even when our custom proxy is bundled perfectly in the local folder, the loader forces dependency resolution straight into `C:\Windows\System32` or the secure Side-by-Side assembly store (`WinSxS`). Because the host operating system lacks `shunimpl.dll` entirely, the lookup terminates immediately.

## 🛠️ Current Engineering Roadmap
To conquer this kernel-level protection barrier, the framework is moving away from basic folder-level side-loading and shifting toward **Advanced PE Binary Patching**:

1.  **Dependency Renaming:** We are utilizing Portable Executable editing tools (e.g., *CFF Explorer* or *PEview*) to alter the Import Directory Table of the legacy `explorer.exe`. We are modifying the hardcoded lookup string from `shunimpl.dll` to a unique, non-protected user identifier: **`shunim8.dll`** (exactly 12 characters to maintain strict byte alignment).
2.  **The Twin-DLL Bridge Architecture:** Maintained inside the localized workspace environment as two distinct elements:
    *   `shunim8.dll` (Our custom-compiled pure C proxy wrapper compiled via GCC)
    *   `shunimpl_real.dll` (The genuine, untouched binary extracted from the Windows 8.1 ISO providing the necessary native ordinal table context)
3.  **Dynamic Ordinal Passthrough:** Programming the C proxy layer to intercept the initial `DllMain` (Ordinal 1) thread attachment to deploy our background kernel patches, while utilizing an explicit ordinal mapping router (`shunimpl.def`) to forward all complex shell navigation and array queries directly to the authentic 8.1 engine asset.

## 🤝 Contributing & Collaboration
If you possess deep experience with Windows Internals, PE header export table mapping, or writing production-grade proxy hook wrappers that interface directly with legacy NT architectures, we need your input!

Feel free to open an Issue, review our current debugging logs, or submit a pull request. Active technical discussion is maintained across the repository and over on the WinClassic Forums.
