#ifndef FLAME_LOGGER_HH
#define FLAME_LOGGER_HH

#include <ostream>

namespace flame {

class Logger {
  char * filename;
  std::ostream * out;
  
  template<typename T, typename... Args>
  void arg_loop(T const & arg, Args const & ... args) {
    (*out) << arg;
    arg_loop(args...);
  }
  
  template<typename T>
  void arg_loop(T const & arg) {
    (*out) << arg << std::endl;
  }
  
public:
  Logger() : filename(nullptr), out(nullptr) {
  }
  
  ~Logger();
  
  void initialize(char const * filename, int rank);
  
  template<typename... Args>
  void debug(Args const & ... args) {
    (*out) << "DEBUG: ";
    arg_loop(args...);
  }
  
  template<typename... Args>
  void info(Args const & ... args) {
    (*out) << "INFO: ";
    arg_loop(args...);
  }
  
  template<typename... Args>
  void warning(Args const & ... args) {
    (*out) << "WARN: ";
    arg_loop(args...);
  }
  
  template<typename... Args>
  void error(Args const & ... args) {
    (*out) << "ERROR: ";
    arg_loop(args...);
  }
  
  template<typename... Args>
  void severe(Args const & ... args) {
    (*out) << "FATAL: ";
    arg_loop(args...);
  }
};

extern Logger logger;

} // end: namespace flame

#endif // #ifndef FLAME_LOGGER_HH
