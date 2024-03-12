#include <signal_handler.hh>

#include <unistd.h>
#include <signal.h>
#include <execinfo.h>

#include <iostream>

namespace flame {

std::string getThisExecutableFullPath() {
  char * linkname = new char[1024];
  ssize_t size = readlink("/proc/self/exe", linkname, 1023);
  if(size > 0) {
    linkname[size] = '\0';
    return std::string(linkname, size);
  } else {
    return std::string();
  }
}

#define MAX_STACK_FRAMES 64

void posixPrintStackTrace(int err) {
  void *stackTraces[MAX_STACK_FRAMES];
  int i, traceSize = 0;
  char **messages = (char **)NULL;
  
  traceSize = backtrace(stackTraces, MAX_STACK_FRAMES);
  messages = backtrace_symbols(stackTraces, traceSize);

  for(i = 0; i < traceSize; ++i) {
    std::cerr << messages[i] << std::endl;
  }
  
  if(messages) { free(messages); } 
}

void posixSignalHandler(int sig, siginfo_t *siginfo, void *context) {
  switch(sig) {
  case SIGSEGV:
    std::cerr << "Caught SIGSEGV: Segmentation Fault" << std::endl;
    break;
  case SIGINT:
    std::cerr << "Caught SIGINT: Interactive attention signal, (usually ctrl+c)" << std::endl;
    break;
  case SIGFPE:
    switch(siginfo->si_code) {
    case FPE_INTDIV:
      std::cerr << "Caught SIGFPE: (integer divide by zero)" << std::endl;
      break;
    case FPE_INTOVF:
      std::cerr << "Caught SIGFPE: (integer overflow)" << std::endl;
      break;
    case FPE_FLTDIV:
      std::cerr << "Caught SIGFPE: (floating-point divide by zero)" << std::endl;
      break;
    case FPE_FLTOVF:
      std::cerr << "Caught SIGFPE: (floating-point overflow)" << std::endl;
      break;
    case FPE_FLTUND:
      std::cerr << "Caught SIGFPE: (floating-point underflow)" << std::endl;
      break;
    case FPE_FLTRES:
      std::cerr << "Caught SIGFPE: (floating-point inexact result)" << std::endl;
      break;
    case FPE_FLTINV:
      std::cerr << "Caught SIGFPE: (floating-point invalid operation)" << std::endl;
      break;
    case FPE_FLTSUB:
      std::cerr << "Caught SIGFPE: (subscript out of range)" << std::endl;
      break;
    default:
      std::cerr << "Caught SIGFPE: Arithmetic Exception" << std::endl;
      break;
    }
    break;
  case SIGILL:
    switch(siginfo->si_code) {
    case ILL_ILLOPC:
      std::cerr << "Caught SIGILL: (illegal opcode)" << std::endl;
      break;
    case ILL_ILLOPN:
      std::cerr << "Caught SIGILL: (illegal operand)" << std::endl;
      break;
    case ILL_ILLADR:
      std::cerr << "Caught SIGILL: (illegal addressing mode)" << std::endl;
      break;
    case ILL_ILLTRP:
      std::cerr << "Caught SIGILL: (illegal trap)" << std::endl;
      break;
    case ILL_PRVOPC:
      std::cerr << "Caught SIGILL: (privileged opcode)" << std::endl;
      break;
    case ILL_PRVREG:
      std::cerr << "Caught SIGILL: (privileged register)" << std::endl;
      break;
    case ILL_COPROC:
      std::cerr << "Caught SIGILL: (coprocessor error)" << std::endl;
      break;
    case ILL_BADSTK:
      std::cerr << "Caught SIGILL: (internal stack error)" << std::endl;
      break;
    default:
      std::cerr << "Caught SIGILL: Illegal Instruction" << std::endl;
      break;
    }
    break;
  case SIGTERM:
    std::cerr << "Caught SIGTERM: a termination request was sent to the program" << std::endl;
    break;
  case SIGABRT:
    std::cerr << "Caught SIGABRT: usually caused by an abort() or assert()" << std::endl;
    break;
  default:
    break;
  }
  
  posixPrintStackTrace(1);
}

static uint8_t alternate_stack[65536];

void setSignalhandler() {
  stack_t ss = {};
  ss.ss_sp = (void *)alternate_stack;
  ss.ss_size = 65536;
  ss.ss_flags = 0;
  
  if(sigaltstack(&ss, NULL)) {
    std::cerr << "Could not register signal alternate stack" << std::endl;
  }
  
  struct sigaction sigAction = {};
  sigAction.sa_sigaction = posixSignalHandler;
  sigemptyset(&sigAction.sa_mask);
  sigAction.sa_flags = SA_SIGINFO | SA_ONSTACK;
  
  if(sigaction(SIGSEGV, &sigAction, NULL)) {
    std::cerr << "Could not register signal handler for SIGSEGV" << std::endl;
  }
  
  if(sigaction(SIGFPE, &sigAction, NULL)) {
    std::cerr << "Could not register signal handler for SIGFPE" << std::endl;
  }
  
  if(sigaction(SIGINT, &sigAction, NULL)) {
    std::cerr << "Could not register signal handler for SIGINT" << std::endl;
  }
  
  if(sigaction(SIGILL, &sigAction, NULL)) {
    std::cerr << "Could not register signal handler for SIGILL" << std::endl;
  }
  
  if(sigaction(SIGTERM, &sigAction, NULL)) {
    std::cerr << "Could not register signal handler for SIGTERM" << std::endl;
  }
  
  if(sigaction(SIGABRT, &sigAction, NULL)) {
    std::cerr << "Could not register signal handler for SIGABRT" << std::endl;
  }
}

} // end: namespace flame
