# Explorer8.1

An ambitious project aimed at bringing the native, untouched **Windows 8.1 File Explorer shell (`explorer.exe`) to Windows 10 22H2**—similar to what the `explorer7` (ex7forw8) project achieved for the Windows 7 shell.

This repository serves as an open sandbox and documentation base for reverse engineers and shell developers looking to resolve the deep kernel API mismatches between NT 6.3 (Windows 8.1) and modern Windows 10 branches.

## 🚀 Milestones Achieved So Far

1. **Binary Isolation:** Extracted untouched x64 Windows 8.1 core files (`explorer.exe`, `ExplorerFrame.dll`, and corresponding `.mui` language strings).
2. **DLL Search Path Redirection:** Implemented the DotLocal (`explorer.exe.local`) API-MS isolation trick. This successfully forces the execution profile to prioritize local 8.1 libraries over the host's `C:\Windows\System32` modules.
3. **Kernel Behavior Mapping:** Isolated the execution loop within a live kernel debugger (`x64dbg`) inside a Windows 10 guest environment.

## 🛑 The Current Blocker: `STATUS_DLL_INIT_FAILED`

When initiating execution of the local `explorer.exe` process under a clean sandbox (with the native Windows 10 shell stopped), the application terminates immediately with exception code:
`0xc0000142` (STATUS_DLL_INIT_FAILED)

### Debugger Observations (`x64dbg` / `ntdll.dll` tracing):
* The binary crashes during the early initialization thread loop.
* Dynamic analysis shows the final execution pointer (`RIP`) trapping an intentional abort sequence (`int3` breakpoint trap) inside `ntdll.dll`.
* This trap is triggered directly after an internal system query: `call <ntdll.ZwQueryInformationThread>`.
* **Root Cause:** The Windows 8.1 shell relies on structures within the Thread Environment Block (TEB) mapped to the NT 6.3 layout. When running on the modern Windows 10 kernel, the data returned by `ZwQueryInformationThread` causes an internal structural validation failure, resulting in an immediate process exit.

## 🛠️ Current Project Blueprint / Roadmap

To bypass this crash and move onto UI drawing initialization, the project framework requires a functional C++ hook proxy engine (forking or adapting the `explorerwrapper` template included).

The immediate engineering target is:
* Intercepting `ZwQueryInformationThread` / `NtQueryInformationThread` (specifically looking at ThreadInformationClass 9 / `ThreadQuerySetWin32StartAddress`).
* Spoofing/translating the thread start parameters passed back by the modern Windows 10 kernel to look identical to native NT 6.3 structures.

## 🤝 Call to Action for Contributors

If you are a reverse engineer, kernel tinker, or Windows shell enthusiast—**please fork this repository!** 

The raw binary assets and base wrapper hooks are organized inside the source folders. Take a snapshot of your Windows 10 dev VM, attach your debugger, and help patch the thread query loop to unlock "Explorer8".
