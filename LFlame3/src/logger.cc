#include <logger.hh>

#include <iostream>
#include <sstream>
#include <fstream>

#define FLAME_DEBUG 0
#define FLAME_INFO 1
#define FLAME_WARNING 2
#define FLAME_ERROR 3
#define FLAME_SEVERE 4

#ifndef FLAME_LOG_LEVEL
#define FLAME_LOG_LEVEL FLAME_DEBUG
#endif

namespace flame {

Logger::~Logger() {
  if(out && (out != &std::cout || out != &std::cerr)) {
    delete out;
  }
}

void Logger::initialize(char const * filename, int rank) {
  if(!out) {
    if(filename) {
      std::stringstream ss;
      ss << filename << "." << rank << ".log";
      out = new std::ofstream(ss.str());
    } else {
      out = &std::cout;
    }
  }
}

/*std::ostream & Logger::info() {
  (*out) << "INFO: ";
  return *out;
}

std::ostream & Logger::warning() {
  (*out) << "WARN: ";
  return *out;
}

std::ostream & Logger::error() {
  (*out) << "ERROR: ";
  return *out;
}

std::ostream & Logger::severe() {
  (*out) << "FATAL: ";
  return *out;
}*/

Logger logger;

} // end: namespace flame
