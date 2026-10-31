# Explorer8.1 (Windows 8.1 Shell Restoration Framework)

An ambitious reverse-engineering project and development sandbox aimed at backporting the complete, native **Windows 8.1 Desktop Shell and User Experience (UX) environment to modern Windows 10/11 branches**. 

Instead of relying on third-party visual skins or translation wrappers, this project framework is engineered to run original, untouched NT 6.3 user environment subsystems side-by-side with modern Windows kernels.

---

## 🗺️ The Full Shell Subsystem Roadmap

To completely restore the native Windows 8.1 look, feel, and functionality, the development pipeline targets the isolation, hooking, and redirection of these core environment blocks:

*   **File Explorer & Desktop Grid (Active Phase):** Isolation and execution of `explorer.exe` & `ExplorerFrame.dll`.
*   **Immersive Shell & Charms Bar:** Reverse-engineering `twinui.dll` and `ImmersiveShell.dll` to restore native Windows 8.1 modern app frames, side charms, and gesture mechanics.
*   **The Start Screen Subsystem:** Hooking and isolating `StartScreen.exe` to bring back the native NT 6.3 full-screen tile layout.
*   **Theme & Composition Engine:** Intercepting `uxtheme.dll` and `dwmapi.dll` components to enforce authentic 8.1 borders and Aero-lite window metrics without disrupting modern display drivers.

---

## 🚀 Active Workspace & Milestones

Currently, all active proxy wrappers, hook points, and detour logic are organized inside the **[`src/`](./src)** directory.

1.  **Binary Isolation:** Extracted untouched x64 Windows 8.1 core files (`explorer.exe`, `ExplorerFrame.dll`, and corresponding `.mui` language strings).
2.  **DLL Search Path Redirection:** Implemented the DotLocal (`explorer.exe.local`) API-MS isolation trick, forcing the host to prioritize local 8.1 libraries over native `C:\Windows\System32` modules.
3.  **Kernel Behavior Mapping:** Isolated the early execution loop within a live kernel debugger (`x64dbg`) inside a Windows 10 guest environment.

---

## 🛑 The Active Blocker: `STATUS_DLL_INIT_FAILED`

When initiating execution of the local Windows 8.1 shell process under a clean sandbox (with the host Windows 10 explorer shell suspended), the application terminates immediately with exception code: **`0xc0000142 (STATUS_DLL_INIT_FAILED)`**.

### Debugger Observations (`x64dbg` / `ntdll.dll` tracing):
*   The binary crashes during the early initialization thread loop before drawing any UI.
*   Dynamic analysis shows the final execution pointer (`RIP`) trapping an intentional abort sequence (`int3` breakpoint trap) inside `ntdll.dll`.
*   This trap triggers directly after an internal system query: `call <ntdll.ZwQueryInformationThread>`.
*   **Root Cause:** The Windows 8.1 shell relies on structures within the Thread Environment Block (TEB) mapped to the NT 6.3 memory layout. When executed on a modern Windows 10 kernel, the data returned by `ZwQueryInformationThread` causes an internal structural validation failure, triggering the protective process exit.

### Immediate Engineering Target:
We have set up the initial hooking blueprint inside **[`src/hooks.cpp`](./src/hooks.cpp)**. The target is intercepting `ZwQueryInformationThread` / `NtQueryInformationThread` (specifically looking at `ThreadInformationClass 9` / `ThreadQuerySetWin32StartAddress`) to spoof/translate the thread parameters returned by the modern kernel into native NT 6.3 structures.

---

## 🤝 Call to Action for Contributors

If you are a reverse engineer, kernel tinkerer, or Windows shell enthusiast—**please fork this repository!**

The project has graduated from static file hosting to an active C++ proxy framework. Take a snapshot of your Windows 10 dev VM, clone the repo, attach your debugger, and help patch the thread query loop to unlock the baseline environment for "Explorer8.1".
