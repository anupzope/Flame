#include <restart.hh>

#include <sstream>

namespace flame {

std::string RestartSettings::toString() const {
  std::ostringstream ss;
  ss << *this;
  return ss.str();
}

void RestartSettings::fromString(std::string const & str) {
  std::istringstream ss(str);
  ss >> *this;
}

int RestartSettings::fromOptionsList(options_list const & ol, std::string & err) {
  int error = 0;
  
  std::ostringstream errmsg;
  
  std::vector<int> freq;
  std::vector<int> cnts;
  
  options_list::option_namelist li = ol.getOptionNameList();
  for(auto const & optName : li) {
    if(optName == "frequencies") {
      Loci::option_value_type type = ol.getOptionValueType(optName);
      if(type == Loci::LIST) {
        options_list::arg_list args;
        ol.getOption(optName, args);
        int const sz = args.size();
        for(int i = 0; i < sz; ++i) {
          if(args[i].type_of() == Loci::REAL) {
            double value;
            args[i].get_value(value);
            int intVal = (int)value;
            if(intVal > 0) {
              freq.push_back(intVal);
            } else {
              errmsg << "[elements of " << optName << " must be positive value/s]";
              ++error;
            }
          } else {
            errmsg << "[elements of " << optName << " must be of type REAL]";
            ++error;
          }
        }
      } else if(type == Loci::REAL) {
        double value;
        ol.getOption(optName, value);
        int intVal = (int)value;
        if(intVal > 0) {
          freq.push_back(intVal);
        } else {
          errmsg << "[" << optName << " must have a positive value]";
          ++error;
        }
      } else {
        errmsg << "[" << optName << " must be of type REAL or LIST]";
        ++error;
      }
    } else if(optName == "counts") {
      Loci::option_value_type type = ol.getOptionValueType(optName);
      if(type == Loci::LIST) {
        options_list::arg_list args;
        ol.getOption(optName, args);
        int const sz = args.size();
        for(int i = 0; i < sz; ++i) {
          if(args[i].type_of() == Loci::REAL) {
            double value;
            args[i].get_value(value);
            int intVal = (int)value;
            if(intVal > 0) {
              cnts.push_back(intVal);
            } else {
              errmsg << "[" << optName << " must have a positive value/s]";
              ++error;
            }
          } else {
            errmsg << "[" << optName << " must be of type REAL]";
            ++error;
          }
        }
      } else if(type == Loci::REAL) {
        double value;
        ol.getOption(optName, value);
        int intVal = (int)value;
        if(intVal > 0) {
          cnts.push_back(intVal);
        } else {
          errmsg << "[" << optName << " must have a positive value]";
          ++error;
        }
      } else {
        errmsg << "[" << optName << " must be of type REAL or LIST]";
        ++error;
      }
    } else {
      errmsg << "[unknown option " << optName << "]";
      ++error;
    }
  }
  
  if(freq.size() != cnts.size()) {
    errmsg << "[freqeuncies and counts must have the same number of elements]";
    ++error;
  }
  
  if(error) {
    err = errmsg.str();
  } else {
    frequencies = freq;
    counts = cnts;
  }
  
  return error;
}

std::ostream & operator<<(std::ostream & s, RestartSettings const & rhs) {
  int nLevels = rhs.frequencies.size();
  
  s << nLevels << ' ';
  for(int i = 0; i < nLevels; ++i) {
    s << rhs.frequencies[i] << ' ' << rhs.counts[i] << ' ';
  }
  
  return s;
}

std::istream & operator>>(std::istream & s, RestartSettings & rhs) {
  int nLevels;
  
  s >> nLevels;
  rhs.frequencies.resize(nLevels);
  rhs.counts.resize(nLevels);
  for(int i = 0; i < nLevels; ++i) {
    s >> rhs.frequencies[i] >> rhs.counts[i];
  }
  
  return s;
}

} // end: namespace flame
