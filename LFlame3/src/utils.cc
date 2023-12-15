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
  
  int getOptionValues(
    options_list const & ol, std::string const & optName,
    int const minValues, int const maxValues,
    std::vector<std::string> & values,
    std::ostream & errmsg
  ) {
    values.clear();
    
    int error = 0;
    Loci::option_value_type type = ol.getOptionValueType(optName);
    
    if(type == Loci::LIST) {
      options_list::arg_list args;
      ol.getOption(optName, args);
      int const sz = args.size();
      for(int i = 0; i < sz; ++i) {
        if(args[i].type_of() == Loci::STRING || args[i].type_of() == Loci::NAME) {
          std::string value;
          args[i].get_value(value);
          values.push_back(value);
        } else {
          errmsg << "[elements of " << optName << " must be of type STRING or NAME]";
          ++error;
        }
      }
    } else if(type == Loci::STRING || type == Loci::NAME) {
      std::string vars, var;
      ol.getOption(optName, vars);
      for(std::string::iterator i = vars.begin(); i != vars.end(); ++i) {
        if(*i == ',') {
          if(var.size() > 0) values.push_back(var);
          var.clear();
        } else if(!std::isspace(*i)) {
          var.push_back(*i);
        }
      }
      if(var.size() > 0) values.push_back(var);
    } else {
      errmsg << "[" << optName << " must be of type STRING or NAME or LIST]";
      ++error;
    }
    
    return error;
  }
  
  int getOptionValues(
    options_list const & ol, std::string const & optName,
    int const minValues, int const maxValues,
    std::vector<int> & values,
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
          if(args[i].type_of() == Loci::REAL) {
            double value;
            args[i].get_value(value);
            double rvalue = std::round(value);
            if(rvalue == value) {
              values.push_back((int)rvalue);
            } else {
              errmsg << "[component " << i << " of " << optName
                << " must have an integral value]";
              ++error;
            }
          } else {
            errmsg << "[component " << i << " of " << optName
              << " must be of type REAL]";
            ++error;
          }
        }
      }
    } else if(type == Loci::REAL) {
      double value;
      ol.getOption(optName, value);
      double rvalue = std::round(value);
      if(rvalue == value) {
        values.push_back((int)rvalue);
      } else {
        errmsg << "[" << optName << " must have an integral value]";
        ++error;
      }
    } else {
      errmsg << "[" << optName << " must be of type LIST or REAL]";
      ++error;
    }
    
    return error;
  }
  
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
                ++error;
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
              << " must be of type UNIT_VALUE or REAL]";
            ++error;
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
      errmsg << "[option " << optName
        << " must be of type LIST, UNIT_VALUE or REAL]";
      ++error;
    }
    
    return error;
  }
  
  std::ostream & operator<<(std::ostream & s, ThermoSpecType const & obj) {
    s << ' ' << (int)obj << ' ';
    return s;
  }
  
  std::istream & operator>>(std::istream & s, ThermoSpecType & obj) {
    int value;
    s >> value;
    if(s) {
      obj = (ThermoSpecType)value;
    }
    return s;
  }
  
  std::ostream & operator<<(std::ostream & s, ThermoSpec const & obj) {
    s << ' ' << obj.type << ' ' << obj.pressure << ' '
      << obj.temperature << ' ' << obj.density << ' ';
    return s;
  }
  
  std::istream & operator>>(std::istream & s, ThermoSpec & obj) {
    ThermoSpecType type;
    double pressure, temperature, density;
    s >> type >> pressure >> temperature >> density;
    if(s) {
      obj.type = type;
      obj.pressure = pressure;
      obj.temperature = temperature;
      obj.density = density;
    }
    return s;
  }
  
  int getThermoSpec(
    options_list const & ol,
    std::string const & olName,
    std::string const & pressureOptName,
    std::string const & temperatureOptName,
    std::string const & densityOptName,
    ThermoSpec & spec,
    std::ostream & errmsg
  ) {
    unsigned int flags = 0;
    double pressure = 0.0, density = 0.0, temperature = 0.0;
    int error = 0;
    ThermoSpec tmp;
    
    if(ol.optionExists(pressureOptName)) {
      flags |= 0x1u;
      std::ostringstream ss;
      if(getOptionValue(ol, pressureOptName, "Pa", tmp.pressure, ss)) {
        errmsg << "[Error parsing " << olName << "::" << pressureOptName << " : "
          << ss.str() << "]";
        ++error;
      }
    }
    
    if(ol.optionExists(temperatureOptName)) {
      flags |= 0x2u;
      std::ostringstream ss;
      if(getOptionValue(ol, temperatureOptName, "K", tmp.temperature, ss)) {
        errmsg << "[Error parsing " << olName << "::" << temperatureOptName << " : "
          << ss.str() << "]";
        ++error;
      }
    }
    
    if(ol.optionExists(densityOptName)) {
      flags |= 0x4u;
      std::ostringstream ss;
      if(getOptionValue(ol, densityOptName, "kg/m/m/m", tmp.density, ss)) {
        errmsg << "[Error parsing " << olName << "::" << densityOptName << " : "
          << ss.str() << "]";
        ++error;
      }
    }
    
    switch(flags) {
    case 0x3u:
      tmp.type = THERMO_SPEC_PT;
      break;
    case 0x5u:
      tmp.type = THERMO_SPEC_DP;
      break;
    case 0x6u:
      tmp.type = THERMO_SPEC_DT;
      break;
    default:
      errmsg << "[Two thermodynamic variables are required for " << olName << "]";
      ++error;
      break;
    }
    
    if(!error) spec = tmp;
    
    return error;
  }
  
  std::ostream & operator<<(std::ostream & s, VelSpecType const & obj) {
    s << ' ' << (int)obj << ' ';
    return s;
  }
  
  std::istream & operator>>(std::istream & s, VelSpecType & obj) {
    int value;
    s >> value;
    if(s) {
      obj = (VelSpecType)value;
    }
    return s;
  }
  
  std::ostream & operator<<(std::ostream & s, VelSpec const & obj) {
    s << ' ' << obj.type << ' ' << obj.direction.x << ' ' << obj.direction.y
      << ' ' << obj.direction.z << ' ' << obj.scale << ' ';
    return s;
  }
  
  std::istream & operator>>(std::istream & s, VelSpec & obj) {
    VelSpecType type;
    Loci::vector3d<double> direction;
    double scale;
    s >> type >> direction.x >> direction.y >> direction.z >> scale;
    if(s) {
      obj.type = type;
      obj.direction = direction;
      obj.scale = scale;
    }
    return s;
  }
  
  int getVelSpec(
    options_list const & ol,
    std::string const & olName,
    std::string const & velocityOptName,
    std::string const & MachOptName,
    VelSpec & spec,
    std::ostream & errmsg
  ) {
    unsigned int flags = 0;
    Loci::vector3d<double> vector(0.0, 0.0, 0.0);
    double scale = 1.0;
    int error = 0;
    VelSpec tmp;
    
    if(ol.optionExists(velocityOptName)) {
      flags |= 0x1u;
      
      std::ostringstream ss;
      std::vector<double> values;
      if(getOptionValues(
        ol, velocityOptName, {"m/s", "m/s", "m/s", ""}, 1, 4, values, ss
      )) {
        errmsg << "[Error parsing " << olName << "::" << velocityOptName
          << " : " << ss.str() << "]";
        ++error;
      } else {
        int const sz = values.size();
        tmp.direction.x = (sz > 0 ? values[0] : 0.0);
        tmp.direction.y = (sz > 1 ? values[1] : 0.0);
        tmp.direction.z = (sz > 2 ? values[2] : 0.0);
        tmp.scale = (sz > 3 ? values[3] : 1.0);
      }
    }
    
    if(ol.optionExists(MachOptName)) {
      flags |= 0x2u;
      
      std::ostringstream ss;
      std::vector<double> values;
      if(getOptionValues(
        ol, MachOptName, {"", "", "", "", ""}, 1, 4, values, ss
      )) {
        errmsg << "[Error parsing " << olName << "::" << MachOptName
          << " : " << ss.str() << "]";
        ++error;
      } else {
        int const sz = values.size();
        tmp.direction.x = (sz > 0 ? values[0] : 0.0);
        tmp.direction.y = (sz > 1 ? values[1] : 0.0);
        tmp.direction.z = (sz > 2 ? values[2] : 0.0);
        tmp.scale = (sz > 3 ? values[3] : 1.0);
      }
    }
    
    switch(flags) {
    case 0x1u:
      tmp.type = VEL_SPEC_VELOCITY;
      break;
    case 0x2u:
      tmp.type = VEL_SPEC_MACH;
      break;
    default:
      errmsg << "[One velocity variable is required for " << olName << "]";
      ++error;
      break;
    }
    
    if(!error) spec = tmp;
    
    return error;
  }
  
  std::ostream & operator<<(std::ostream & s, SpeciesSpecType const & obj) {
    s << ' ' << (int)obj << ' ';
    return s;
  }
  
  std::istream & operator>>(std::istream & s, SpeciesSpecType & obj) {
    int value;
    s >> value;
    if(s) {
      obj = (SpeciesSpecType)value;
    }
    return s;
  }
  
  std::ostream & operator<<(std::ostream & s, UnityFractions const & obj) {
    int size = obj.data.size();
    s << ' ' << size << ' ';
    for(int i = 0; i < size; ++i) {
      s << obj.data[i] << ' ';
    }
    return s;
  }
  
  std::istream & operator>>(std::istream & s, UnityFractions & obj) {
    int size;
    std::vector<double> data;
    s >> size;
    if(s) {
      data.resize(size);
      for(int i = 0; i < size; ++i) {
        s >> data[i];
      }
      if(s) {
        obj.data = data;
      }
    }
    return s;
  }
  
  std::ostream & operator<<(std::ostream & s, SpeciesSpec const & obj) {
    s << ' ' << obj.type << ' ' << obj.fractions << ' ';
    return s;
  }
  
  std::istream & operator>>(std::istream & s, SpeciesSpec & obj) {
    SpeciesSpecType type;
    UnityFractions fractions;
    s >> type >> fractions;
    if(s) {
      obj.type = type;
      obj.fractions = fractions;
    }
    return s;
  }
  
  int getSpeciesSpec(
    options_list const & ol,
    std::string const & olName,
    std::string const & molarOptName,
    std::string const & massOptName,
    int const count,
    SpeciesSpec & spec,
    std::ostream & errmsg
  ) {
    unsigned int flags = 0;
    int error = 0;
    SpeciesSpec tmp;
    
    if(ol.optionExists(molarOptName)) {
      flags |= 0x1u;
      tmp.fractions.data.clear();
      
      std::ostringstream ss;
      if(getOptionValues(
        ol, molarOptName, std::vector<std::string>(), count-1, count,
        tmp.fractions.data, ss
      )) {
        errmsg << "[Error parsing " << olName << "::" << molarOptName
          << " : " << ss.str() << "]";
        ++error;
      } else {
        int const sz = tmp.fractions.data.size();
        double sum = 0.0;
        for(int i = 0; i < sz; ++i) {
          sum += tmp.fractions.data[i];
        }
        if(sz == count-1) {
          double const last = 1.0 - sum;
          if(last < 0.0) {
            errmsg << "[" << olName << "::" << molarOptName << " must sum to 1.0]";
            ++error;
          } else {
            tmp.fractions.data.push_back(last);
          }
        } else {
          if(sum != 1.0) {
            errmsg << "[" << olName << "::" << molarOptName << " must sum to 1.0]";
            ++error;
          }
        }
      }
    }
    
    if(ol.optionExists(massOptName)) {
      flags |= 0x2u;
      tmp.fractions.data.clear();
      
      std::ostringstream ss;
      if(getOptionValues(
        ol, massOptName, std::vector<std::string>(), count-1,
        count, tmp.fractions.data, ss
      )) {
        errmsg << "[Error parsing " << olName << "::" << massOptName
          << " : " << ss.str() << "]";
        ++error;
      } else {
        int const sz = tmp.fractions.data.size();
        double sum = 0.0;
        for(int i = 0; i < sz; ++i) {
          sum += tmp.fractions.data[i];
        }
        if(sz == count-1) {
          double const last = 1.0 - sum;
          if(last < 0.0) {
            errmsg << "[" << olName << "::" << massOptName << " must sum to 1.0]";
            ++error;
          } else {
            tmp.fractions.data.push_back(last);
          }
        } else {
          if(sum != 1.0) {
            errmsg << "[" << olName << "::" << massOptName << " must sum to 1.0]";
            ++error;
          }
        }
      }
    }
    
    switch(flags) {
    case 0x1u:
      tmp.type = SPECIES_SPEC_X;
      break;
    case 0x2u:
      tmp.type = SPECIES_SPEC_Y;
      break;
    default:
      errmsg << "[One fraction variable required for " << olName << "]";
      ++error;
      break;
    }
    
    if(!error) spec = tmp;
    
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
