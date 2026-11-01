# Explorer8.1

An experimental, low-level reverse-engineering framework dedicated to backporting the native, untouched Windows 8.1 Desktop Shell and File Explorer environment (NT 6.3) to run natively on modern Windows 10 and Windows 11 kernels.

Rather than relying on visual skinning tools or UI wrappers, this project utilizes specialized dynamic runtime interception and memory instrumentation loops to safely initialize real legacy UX subsystems alongside modern security environments.

## 🚀 Recent Breakthroughs & Milestones
During our recent dynamic binary analysis and debugging sessions inside isolated sandboxes, we achieved a monumental milestone: The legacy shell successfully broke through early loader checks, initialized its core subsystems, and actively mapped its primary window station interface onto the host system (evidenced by the visual rendering of the OS desktop watermark layer).

This was accomplished by abandoning static binary modification strategies and deploying an automated, self-contained **Vectored Exception Handler (VEH)** proxy architecture injected through the native standard initialization path (`version.dll`).

Furthermore, we have fully integrated an automated **Cabinet Decompression Parser** right inside our localized build chain. This automatically resolves structural `STATUS_INVALID_IMAGE_FORMAT (0xc000012f)` loader traps by unsealing compressed symbol delivery archives natively before execution.

## Integrated Codebase Features
*   **Suspended State Process Launcher:** A dedicated utility (`loader.exe`) that handles target creation utilizing the `CREATE_SUSPENDED` flag. This halts the shell execution vector before the kernel can initialize its static Import Address Table (IAT) parsing check routine, allowing perfect runtime instrumentation attachment.
*   **Vectored Exception Handling (VEH) Interception:** A high-priority instrumentation loop hooked at user-mode runtime index 1. It directly accesses `PEXCEPTION_POINTERS->ContextRecord`, providing real-time logging, analysis, and execution redirection control over all 64-bit general-purpose CPU registers (`Rip`, `Rax`, etc.) on fault conditions.
*   **Automated CFG Bypass:** The custom proxy layer dynamically scans memory space to identify `ntdll!LdrpValidateUserCallTarget`. It programmatically overrides the Control Flow Guard verification loop header with an inline binary patch (`0x31 0xC0 0xC3` / `xor eax, eax ; ret`), automatically reporting a flawless validation status (`STATUS_SUCCESS`) on every execution path.
*   **Initialization Loop Mitigation:** By embedding code-level interception, the framework successfully satisfies the strict environment queries of cascading dependencies (such as `umpdc.dll` and `imm32.dll`), breaking the application out of its persistent `STATUS_DLL_INIT_FAILED (0xc0000142)` roadblock.

## 🛑 Current Active Development Blocker: Ordinal Routing Map
With the initial engine execution environment active, the system is resolving unhandled `STATUS_ORDINAL_NOT_FOUND (0xc0000138)` traps when internal components query the obsolete `shunimpl.dll` layer via nameless integer ordinals (`#240`, `#250`, `#290`). 

While our VEH code captures the faulting registers, we are mapping out the precise register routing logic required to hot-swap instructions back to the native uncompressed 8.1 subsystem module safely.

## 🛠️ Current Engineering Roadmap
Our development cycle is strictly focused on refining this self-contained memory tracking pipeline:

1.  **Automated Workspace Staging:** Maintaining a zero-configuration environment. The build loop handles the retrieval, decompression via `expand.exe`, and deployment of the genuine, untouched Windows 8.1 target asset (`shunimpl_real.dll`) straight out of Microsoft's public Symbol Server array.
2.  **VEH Register Redirection:** Coding explicit state-patch parameters directly inside our exception handler loop to trap faulting instruction pointers (`ContextRecord->Rip`) and cleanly bridge execution pipelines directly to the real 8.1 code offsets without distorting the underlying x64 stack frames.
3.  **Debugger Integration:** Maintaining absolute stability when running under `x64dbg` to catch multi-threaded window creation adjustments on the fly.

## 🏁 Deployment & Test Procedure
To evaluate this project inside your isolated virtual machine or Windows Sandbox test environment:

1. Place your original, untouched Windows 8.1 `explorer.exe` into the root directory.
2. Run `build.bat` via the `w64devkit` pure C toolchain to download dependencies and compile the workspace components.
3. Verify your project directory contains strictly:
   ```text
   📁 Explorer8.1/
   ├── 📄 explorer.exe       (Original, untouched)
   ├── 📄 version.dll        (Compiled VEH memory tracker)
   ├── 📄 loader.exe         (Compiled suspended-state launcher)
   └── 📄 shunimpl_real.dll  (Decompressed Microsoft symbol asset)
   ```
4. Execute the tracking loop from an Administrative Command Prompt container:
   ```cmd
   taskkill /f /im explorer.exe
   loader.exe
   ```

## 🤝 Contributing & Collaboration
If you possess deep experience with Windows Internals, VEH memory tracking, x64 assembly register manipulation, or structure layout routing, we need your input! 

Feel free to open an Issue, review our current debugging logs, or submit a pull request. Active technical discussion is maintained across the repository and over on the WinClassic Forums.
