#include <utils.hh>

#include <iomanip>
#include <sstream>

namespace flame {

int getOptionValue(
  options_list const & ol, std::string const & optName,
  std::string const & unit, double & value,
  std::ostream & errmsg
) {
  int error = 0;
  
  Loci::option_value_type type = ol.getOptionValueType(optName);
  
  if(type == Loci::UNIT_VALUE) {
    if(!unit.empty()) {
      Loci::UNIT_type ut;
      ol.getOption(optName, ut);
      if(ut.is_compatible(unit)) {
        value = ut.get_value_in(unit);
      } else {
        errmsg << "[unit of option " << optName
          << " must be compatible with " << unit << "]";
        ++error;
      }
    } else {
      errmsg << "[option " << optName << " must be unit-less]";
      ++error;
    }
  } else if(type == Loci::REAL) {
    ol.getOption(optName, value);
  } else {
    errmsg << "[option " << optName << " must be a UNIT_VALUE or a REAL]";
    ++error;
  }
  
  return error;
}

// -----------------------------------------------------------------------------

int getOptionValues(
  options_list const & ol, std::string const & optName,
  std::vector<std::string> const & units,
  int const minValues, int const maxValues,
  std::vector<double> & values,
  std::ostream & errmsg
) {
  values.clear();
  
  int error = 0;
  Loci::option_value_type type = ol.getOptionValueType(optName);
  
  if(type == Loci::LIST) {
    options_list::arg_list args;
    ol.getOption(optName, args);
    int const sz = args.size();
    if(minValues <= sz && sz <= maxValues) {
      for(int i = 0; i < sz; ++i) {
        if(args[i].type_of() == Loci::UNIT_VALUE) {
          if(units.size() > i && !units[i].empty()) {
            Loci::UNIT_type ut;
            args[i].get_value(ut);
            if(ut.is_compatible(units[i])) {
              double value = ut.get_value_in(units[i]);
              values.push_back(value);
            } else {
              errmsg << "[unit of component " << i << " of " << optName
                << " must be compatible with " << units[i] << "]";
            }
          } else {
            errmsg << "[component " << i << " of " << optName
              << " must be unit-less]";
            ++error;
          }
        } else if(args[i].type_of() == Loci::REAL) {
          double value;
          args[i].get_value(value);
          values.push_back(value);
        } else {
          errmsg << "[component " << i << " of " << optName
            << " must be a UNIT_VALUE or REAL]";
        }
      }
    } else {
      errmsg << "[option " << optName << " must be a LIST of size ["
        << minValues << ", " << maxValues << "]]";
      ++error;
    }
  } else if(type == Loci::UNIT_VALUE) {
    if(minValues <= 1 && 1 <= maxValues) {
      if(units.size() > 0 && !units[0].empty()) {
        Loci::UNIT_type ut;
        ol.getOption(optName, ut);
        if(ut.is_compatible(units[0])) {
          double value = ut.get_value_in(units[0]);
          values.push_back(value);
        } else {
          errmsg << "[unit of option " << optName
            << " must be compatible with " << units[0] << "]";
          ++error;
        }
      } else {
        errmsg << "[option " << optName << " must be unit-less]";
        ++error;
      }
    } else {
      errmsg << "[option " << optName << " must have only one value]";
      ++error;
    }
  } else if(type == Loci::REAL) {
    if(minValues <= 1 && 1 <= maxValues) {
      double value;
      ol.getOption(optName, value);
      values.push_back(value);
    } else {
      errmsg << "[option " << optName << " must have only one value]";
      ++error;
    }
  } else {
  }
  
  return error;
}

} // end: namespace flame

namespace Loci {

STDStringVectorSchemaConverter::STDStringVectorSchemaConverter(
  std::vector<std::string> & ref
) : ref(ref) {
  std::ostringstream ss;
  for(size_t i = 0; i < ref.size(); ++i) {
    ss << std::quoted(ref[i]) << " ";
  }
  str = ss.str();
}

void STDStringVectorSchemaConverter::setState(char * buf, int size) {
  ref.resize(size);
  std::string str(buf);
  std::istringstream ss(str);
  for(int i = 0; i < size; ++i) {
    ss >> std::quoted(ref[i]);
  }
}

} // end: namespace Loci
