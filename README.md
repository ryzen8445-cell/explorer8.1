# Explorer8.1

An experimental low-level reverse-engineering framework dedicated to backporting the native, untouched **Windows 8.1 Desktop Shell and File Explorer environment (NT 6.3)** to run seamlessly alongside modern kernels on Windows 10 and Windows 11.

Unlike standard skinning suites or theme packages, this project isolates real legacy Microsoft user-experience subsystems inside dedicated `.local` application isolation boundaries.

## ⚠️ Current Development Status & Roadblocks

The project is currently tracking active debugging sessions inside isolated test environments to circumvent core kernel loader mismatches between NT 6.3 binaries and modern NT 10.0+ systems. 

Our dynamic binary analysis inside `x64dbg` and Microsoft Sysinternals `ProcMon` has mapped out the exact sequential checkpoints causing initialization failures:

### 1. The CFG Mitigation Block (`ntdll.dll`)
* **Symptom:** Silent failure or instant termination during initial process thread orchestration.
* **Mechanism:** Modern `ntdll.dll` triggers an internal Control Flow Guard (CFG) validation check via `ntdll!LdrpValidateUserCallTarget`. Because the function entry points within the native legacy `ExplorerFrame.dll` and `explorer.exe` do not correlate with modern pre-compiled CFG exception bitmap tables, the OS flags the indirect call as an exploit hazard and executes an abort sequence (`int3` trap) after calling `ZwQueryInformationThread`.
* **Current Debugger Bypass:** Overwriting the validation loop header with an explicit `xor eax, eax` / `ret` patch fakes a successful validation status (`STATUS_SUCCESS = 0x00000000`) and passes execution control back to the loader.

### 2. Core Infrastructure Initialization Failures (`0xc0000142`)
Once the kernel validation trap is suppressed, the Windows Loader (`Ldr`) cascadingly executes module initialization routines (`DllMain EntryPoints`). The process currently hits a hard **`STATUS_DLL_INIT_FAILED (0xc0000142)`** abort immediately after mapping critical system dependencies:

* **`umpdc.dll` (User Mode Power Service Client):** Internal initialization queries fail when clashing with the host's modern power management state structures.
* **`imm32.dll` (Input Method Manager):** Fails during layout/language framework environment setup due to heavily refactored internal structural changes inside the modern Thread Environment Block (TEB).

Stubbing these entry points directly with a blind success signal (`mov eax, 1` / `ret`) clears the initialization loop but triggers immediate downstream null pointer dereferences (`0xC0000005`), as the shell relies on these populated tables to paint the UI thread.

## 🛠️ Planned Engineering & Architecture Solutions

To resolve the `0xc0000142` loop without breaking core subsystem prerequisites, we are actively shifting away from runtime debugger manipulation toward an automated codebase approach:

* **C++ Custom Loader Stub:** Engineering an application bootstrapper designed to instantiate the environment process suspended, allocate the required structural parameters inside target memory, and safely initialize execution.
* **API Set & TEB Spoofing Hooks:** Building lightweight proxy DLLs designed to sit within the `.local` directory boundary, intercepting `ntdll` queries to dynamically fake a legacy NT 6.3 context environment for incoming shell system requests.

## 🤝 Contributing & Collaboration

If you have deep experience with Windows Internals, kernel-level thread structures, or writing custom compatibility wrappers and hooks, we need your eyes on this! 

Feel free to open an Issue or pull request with your findings. You can also join our active development discussions over on the **WinClassic Forums**.
