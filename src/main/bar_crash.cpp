// bar_crash.cpp — symbolized crash backtrace (debug aid). Installs an unhandled-exception filter that, on
// an access violation, walks the faulting thread's stack via DbgHelp (needs beetle-adventure-racing-recomp.pdb next to the exe)
// and prints symbol names to stderr. Used to locate where the cooperative-preemption yield faults.
//
// Windows uses DbgHelp + SetUnhandledExceptionFilter. Linux has its own handler below (a fatal-signal
// handler printing backtrace_symbols_fd offsets for addr2line). Other platforms get an empty object, and
// main.cpp calls bar_install_crash_handler() only on Windows and Linux.
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
#elif defined(__linux__)
// Linux: a fatal-signal handler printing the faulting address and a backtrace. backtrace_symbols_fd
// writes "exe(function+0x..)" or "exe(+0x..)"; the offsets resolve to function and line with
//     addr2line -f -C -e beetle-adventure-racing-recomp 0x<offset>
// (or llvm-symbolizer). Only async-signal-tolerant calls in the handler, then the default action runs.
#include <csignal>
#include <cstdio>
#include <cstring>
#include <execinfo.h>
#include <initializer_list>
#include <unistd.h>

static void bar_crash_signal(int sig, siginfo_t *info, void *) {
    char line[160];
    int n = std::snprintf(line, sizeof(line), "\n[beetle-adventure-racing-recomp] *** CRASH signal=%d (%s) addr=%p ***\n",
        sig, strsignal(sig), (info != nullptr) ? info->si_addr : nullptr);
    if (n > 0) { ssize_t ignored = write(STDERR_FILENO, line, size_t(n)); (void)ignored; }
    void *frames[48];
    int count = backtrace(frames, 48);
    backtrace_symbols_fd(frames, count, STDERR_FILENO);
    signal(sig, SIG_DFL);
    raise(sig);
}

void bar_install_crash_handler() {
    struct sigaction action {};
    action.sa_sigaction = bar_crash_signal;
    action.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigemptyset(&action.sa_mask);
    for (int sig : { SIGSEGV, SIGBUS, SIGILL, SIGFPE }) {
        sigaction(sig, &action, nullptr);
    }
}
#endif // _WIN32 / __linux__
