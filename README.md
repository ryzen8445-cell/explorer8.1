# Explorer8.1

An experimental, low-level reverse-engineering framework dedicated to backporting the native, untouched **Windows 8.1 Desktop Shell and File Explorer environment (NT 6.3)** to run natively on modern Windows 10 and Windows 11 kernels.

Rather than relying on visual skinning tools or UI wrappers, this project utilizes local module isolation (`.local` redirection) and custom C++ proxy hooks to safely initialize real legacy UX subsystems alongside modern security environments.

## 🚀 Recent Breakthroughs & Milestones

During our recent dynamic binary analysis and debugging sessions inside isolated sandboxes, we achieved a monumental milestone: **The legacy shell successfully broke through early loader checks, initialized its core subsystems, and actively mapped its primary window station interface onto the host system (evidenced by the visual rendering of the OS desktop watermark layer).**

This was accomplished by transitioning away from manual runtime debugger interventions inside `x64dbg` and moving toward an automated, compiled C++ memory hook architecture.

### Integrated Codebase Features
* **Automated CFG Bypass:** The custom proxy layer dynamically scans memory space to identify `ntdll!LdrpValidateUserCallTarget`. It programmatically overrides the Control Flow Guard verification loop header with an inline binary patch (`0x31 0xC0 0xC3` / `xor eax, eax` ; `ret`), automatically reporting a flawless validation status (`STATUS_SUCCESS`) on every execution path.
* **Initialization Loop Mitigation:** By embedding code-level interception, the framework successfully satisfies the strict environment queries of cascading dependencies (such as `umpdc.dll` and `imm32.dll`), breaking the application out of its persistent `STATUS_DLL_INIT_FAILED (0xc0000142)` roadblock.

---

## 🛑 Current Active Development Blockers

With the initial loader traps cleared and the UI engine attempting to mount, the architecture is currently tracking a deep subsystem constraint where the modern kernel environment clashes with the legacy file manager's structural interface.

### The Shell Undo Implementation Barrier (`shunimpl.dll`)
* **Symptom:** A hard process termination throwing **`STATUS_ORDINAL_NOT_FOUND (0xc0000138)`** when executing natively or attempting to swap the active shell via `taskkill /f /im explorer.exe`.
* **The Root Cause:** Post-NT 6.3 operating systems completely refactored or integrated the Shell Undo Implementation functionalities natively into broader storage engines (`windows.storage.dll` / `shell32.dll`), rendering `shunimpl.dll` obsolete. The legacy `explorer.exe` demands access to this specific module, querying its internal architecture entirely via **nameless, raw numeric ordinals** (specifically targeting index locations such as `#240`, `#250`, and `#290`). 
* **The Iso-Extraction Strategy:** To conquer this missing structural layout, we successfully extracted the authentic, genuine version of `shunimpl.dll` from an official Windows 8.1 installation ISO payload to provide the necessary native ordinal table context.

---

## 🛠️ Current Engineering Roadmap

We are actively designing a robust **Twin-Module DLL Side-Loading / Proxying** pipeline to bridge the remaining ordinal gaps without triggering memory collision errors:

1. **The Twin-DLL Bridge Architecture:** Maintained inside the localized folder environment as two distinct elements:
   * `shunimpl.dll` (Our custom-compiled C++ proxy wrapper)
   * `shunimpl_real.dll` (The genuine, untouched binary extracted from the Windows 8.1 ISO)
2. **Dynamic Ordinal Passthrough:** Programming the C++ proxy layer to intercept the initial `DllMain` (`Ordinal 1`) thread attachment to automatically deploy our background kernel patches, while utilizing an explicit ordinal mapping router to forward all complex shell navigation and array queries directly to the authentic 8.1 engine asset.

## 🤝 Contributing & Collaboration

If you possess deep experience with Windows Internals, PE header export table mapping, or writing production-grade C++ proxy hook wrappers that interface directly with legacy NT architectures, we need your input! 

Feel free to open an Issue, review our current debugging logs, or submit a pull request. Active technical discussion is maintained across the repository and over on the **WinClassic Forums**.
