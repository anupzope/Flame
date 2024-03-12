#ifndef LFLAME3_SIGNAL_HANDLER_HH
#define LFLAME3_SIGNAL_HANDLER_HH

#include <string>
#include <ostream>

#include <signal.h>

namespace flame {

std::string getThisExecutableFullPath();

void posixPrintStackTrace(int err);

void posixSignalHandler(int sig, siginfo_t *siginfo, void *context);

void setSignalhandler();

} // end: namespace flame

#endif // #ifndef LFLAME3_SIGNAL_HANDLER_HH
