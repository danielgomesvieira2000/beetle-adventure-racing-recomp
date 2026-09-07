// bar_crash.cpp — symbolized crash backtrace (debug aid). Installs an unhandled-exception filter that, on
// an access violation, walks the faulting thread's stack via DbgHelp (needs beetle-adventure-racing-recomp.pdb next to the exe)
// and prints symbol names to stderr. Used to locate where the cooperative-preemption yield faults.
//
// Windows-only: this uses DbgHelp + SetUnhandledExceptionFilter, which have no portable equivalent.
// CMake compiles this TU on every platform, so the whole body is guarded — on non-Windows it becomes
// an empty object, and main.cpp only calls bar_install_crash_handler() under #ifdef _WIN32.
#ifdef _WIN32
#include <cstdio>
#include <windows.h>
#include <dbghelp.h>

static LONG WINAPI bar_crash_filter(EXCEPTION_POINTERS* ep) {
    HANDLE proc = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
    SymInitialize(proc, nullptr, TRUE);

    std::fprintf(stderr, "\n[beetle-adventure-racing-recomp] *** CRASH code=0x%08lx addr=%p ***\n",
                 ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);

    CONTEXT ctx = *ep->ContextRecord;
    STACKFRAME64 sf{};
    sf.AddrPC.Offset    = ctx.Rip; sf.AddrPC.Mode    = AddrModeFlat;
    sf.AddrFrame.Offset = ctx.Rbp; sf.AddrFrame.Mode = AddrModeFlat;
    sf.AddrStack.Offset = ctx.Rsp; sf.AddrStack.Mode = AddrModeFlat;

    char buf[sizeof(SYMBOL_INFO) + 512];
    SYMBOL_INFO* sym = reinterpret_cast<SYMBOL_INFO*>(buf);
    sym->SizeOfStruct = sizeof(SYMBOL_INFO);
    sym->MaxNameLen = 511;

    for (int i = 0; i < 48; i++) {
        if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, proc, thread, &sf, &ctx, nullptr,
                         SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
            break;
        if (sf.AddrPC.Offset == 0) break;
        DWORD64 disp = 0;
        if (SymFromAddr(proc, sf.AddrPC.Offset, &disp, sym))
            std::fprintf(stderr, "  #%2d %s +0x%llx\n", i, sym->Name, (unsigned long long)disp);
        else
            std::fprintf(stderr, "  #%2d 0x%llx\n", i, (unsigned long long)sf.AddrPC.Offset);
    }
    std::fflush(stderr);
    return EXCEPTION_EXECUTE_HANDLER;   // let the process die after printing
}

void bar_install_crash_handler() {
    SetUnhandledExceptionFilter(bar_crash_filter);
}
#endif // _WIN32
