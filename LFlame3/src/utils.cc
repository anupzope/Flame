#include <utils.hh>

#include <iomanip>
#include <sstream>

namespace flame {

std::string nameOptionType(Loci::option_value_type type) {
  std::string typeName = "unknown";
  switch(type) {
  case Loci::NOT_ASSIGNED:
    typeName = "NOT_ASSIGNED";
    break;
  case Loci::REAL:
    typeName = "REAL";
    break;
  case Loci::NAME:
    typeName = "NAME";
    break;
  case Loci::FUNCTION:
    typeName = "FUNCTION";
    break;
  case Loci::LIST:
    typeName = "LIST";
    break;
  case Loci::STRING:
    typeName = "STRING";
    break;
  case Loci::BOOLEAN:
    typeName = "BOOLEAN";
    break;
  case Loci::UNIT_VALUE:
    typeName = "UNIT_VALUE";
    break;
  case Loci::NAME_ASSIGN:
    typeName = "NAME_ASSIGN";
    break;
  }

  return typeName;
}

int getArgValue(
  Loci::option_values const & arg,
  std::string & value,
  std::ostream & errmsg
) {
  int error = 0;

  std::string argName;
  arg.get_value(argName);

  Loci::option_value_type type = arg.type_of();

  if(type == Loci::NAME_ASSIGN) {
    Loci::option_values::value_list_type argvalues;
    arg.get_value(argvalues);
    int sz = argvalues.size();
    if(sz == 1) {
      if(argvalues[0].type_of() == Loci::STRING || argvalues[0].type_of() == Loci::NAME) {
        argvalues[0].get_value(value);
      } else {
        errmsg << "[argument " << argName << " must be a STRING or a NAME]";
        ++error;
      }
    } else {
      errmsg << "[argument " << argName << " must have only one value]";
      ++error;
    }
  } else {
    errmsg << "[argument " << argName << " must be a NAME_ASSIGN]";
    ++error;
  }

  return error;
}

int getArgValue(
  Loci::option_values const & arg,
  std::string const & unit, double & value,
  std::ostream & errmsg
) {
  int error = 0;

  std::string argName;
  arg.get_value(argName);

  Loci::option_value_type type = arg.type_of();

  if(type == Loci::NAME_ASSIGN) {
    Loci::option_values::value_list_type argvalues;
    arg.get_value(argvalues);
    int sz = argvalues.size();
    if(sz == 1) {
      if(argvalues[0].type_of() == Loci::UNIT_VALUE) {
        if(!unit.empty()) {
          Loci::UNIT_type ut;
          argvalues[0].get_value(ut);
          if(ut.is_compatible(unit)) {
            value = ut.get_value_in(unit);
          } else {
            errmsg << "[unit of argument " << argName
                   << " must be compatible with " << unit << "]";
            ++error;
          }
        } else {
          errmsg << "[argument " << argName << " must be unit-less]";
          ++error;
        }
      } else if(argvalues[0].type_of() == Loci::REAL) {
        argvalues[0].get_value(value);
      } else {
        errmsg << "[argument " << argName << " must be a UNIT_VALUE or a REAL]";
        ++error;
      }
    } else {
      errmsg << "[argument " << argName << " must have only one value]";
      ++error;
    }
  } else {
    errmsg << "[argument " << argName << " must be a NAME_ASSIGN]";
    ++error;
  }

  return error;
}

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

int getArgValues(
  Loci::option_values const & arg,
  int const minValues, int const maxValues,
  std::vector<std::string> & values,
  std::ostream & errmsg
) {
  values.clear();
  int error = 0;

  std::string argName;
  arg.get_value(argName);

  Loci::option_value_type type = arg.type_of();

  if(type == Loci::NAME_ASSIGN) {
    Loci::option_values::value_list_type argValues;
    arg.get_value(argValues);
    int const sz = argValues.size();
    if(sz != 1) {
      errmsg << "[argument " << argName << " must have only one value]";
      ++error;
    } else {
      if(argValues[0].type_of() == Loci::LIST) {
        Loci::option_values::value_list_type listValues;
        argValues[0].get_value(listValues);
        int const lsz = listValues.size();
        if(minValues <= lsz && lsz <= maxValues) {
          for(int i = 0; i < lsz; ++i) {
            if(listValues[i].type_of() == Loci::STRING || listValues[i].type_of() == Loci::NAME) {
              std::string value;
              listValues[i].get_value(value);
              values.push_back(value);
            } else {
              errmsg << "[element " << i << " of argument " << argName
                     << " LIST must be of type STRING or NAME]";
              ++error;
            }
          }
        } else {
          errmsg << "[argument " << argName << " must be a LIST of size ["
                 << minValues << ", " << maxValues << "]]";
          ++error;
        }
      } else if(argValues[0].type_of() == Loci::STRING || argValues[0].type_of() == Loci::NAME) {
        std::string vars, var;
        argValues[0].get_value(vars);
        for(std::string::iterator i = vars.begin(); i != vars.end(); ++i) {
          if(*i == ',') {
            if(var.size() > 0) values.push_back(var);
            var.clear();
          } else if(!std::isspace(*i)) {
            var.push_back(*i);
          }
        }
        if(var.size() > 0) values.push_back(var);
        
        int const sz = values.size();
        if(sz < minValues || sz > maxValues) {
          values.clear();
          errmsg << "[value of argument " << argName << " must be a csv STRING or NAME of size ["
                 << minValues << ", " << maxValues << "]";
          ++error;
        }
      } else {
        errmsg << "[value of argument " << argName
               << " must be of type LIST or STRING or NAME]";
        ++error;
      }
    }
  } else {
    errmsg << "[argument " << argName << " must be a NAME_ASSIGN]";
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
    if(minValues <= sz && sz <= maxValues) {
      for(int i = 0; i < sz; ++i) {
        if(args[i].type_of() == Loci::STRING || args[i].type_of() == Loci::NAME) {
          std::string value;
          args[i].get_value(value);
          values.push_back(value);
        } else {
          errmsg << "[element " << i << " of option " << optName
                 << " LIST must be of type STRING or NAME]";
          ++error;
        }
      }
    } else {
      errmsg << "[value of option " << optName
             << " must be a LIST of size ["
             << minValues << ", " << maxValues << "]]";
      ++error;
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

    int const sz = values.size();
    if(sz < minValues || sz > maxValues) {
      values.clear();
      errmsg << "[value of option " << optName
             << " must be a csv STRING or NAME of size ["
             << minValues << ", " << maxValues << "]";
      ++error;
    }
  } else {
    errmsg << "[value of option " << optName
           << " must be of type STRING or NAME or LIST]";
    ++error;
  }

  return error;
}

int getArgValues(
  Loci::option_values const & arg,
  int const minValues, int const maxValues,
  std::vector<int> & values,
  std::ostream & errmsg
) {
  values.clear();
  int error = 0;

  std::string argName;
  arg.get_value(argName);

  Loci::option_value_type type = arg.type_of();

  if(type == Loci::NAME_ASSIGN) {
    Loci::option_values::value_list_type argValues;
    arg.get_value(argValues);
    int const sz = argValues.size();
    if(sz != 1) {
      errmsg << "[argument " << argName << " must have only one value]";
      ++error;
    } else {
      if(argValues[0].type_of() == Loci::LIST) {
        Loci::option_values::value_list_type listValues;
        argValues[0].get_value(listValues);
        int const lsz = listValues.size();
        if(minValues <= lsz && lsz <= maxValues) {
          for(int i = 0; i < lsz; ++i) {
            if(listValues[i].type_of() == Loci::REAL) {
              double value;
              listValues[i].get_value(value);
              double rvalue = std::round(value);
              if(rvalue == value) {
                values.push_back((int)rvalue);
              } else {
                errmsg << "[element " << i << " of argument " << argName
                       << " LIST must have an integral value]";
                ++error;
              }
            } else {
              errmsg << "[element " << i << " of argument " << argName
                     << " LIST must be of type REAL]";
              ++error;
            }
          }
        } else {
          errmsg << "[argument " << argName << " must be a LIST of size ["
                 << minValues << ", " << maxValues << "]]";
          ++error;
        }
      } else if(argValues[0].type_of() == Loci::REAL) {
        if(minValues <= 1 && 1 <= maxValues) {
          double value;
          argValues[0].get_value(value);
          double rvalue = std::round(value);
          if(rvalue == value) {
            values.push_back((int)rvalue);
          } else {
            errmsg << "[argument " << argName << " must have an integral value]";
            ++error;
          }
        } else {
          errmsg << "[argument " << argName << " must be a LIST of size ["
                 << minValues << ", " << maxValues << "]]";
          ++error;
        }
      } else {
        errmsg << "[argument " << argName << " must be of type LIST or REAL]";
        ++error;
      }
    }
  } else {
    errmsg << "[argument " << argName << " must be a NAME_ASSIGN]";
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
            errmsg << "[element " << i << " of option " << optName
              << " LIST must have an integral value]";
            ++error;
          }
        } else {
          errmsg << "[element " << i << " of option " << optName
            << " LIST must be of type REAL]";
          ++error;
        }
      }
    } else {
      errmsg << "[option " << optName << " must be a LIST of size ["
        << minValues << ", " << maxValues << "]]";
      ++error;
    }
  } else if(type == Loci::REAL) {
    if(minValues <= 1 && 1 <= maxValues) {
      double value;
      ol.getOption(optName, value);
      double rvalue = std::round(value);
      if(rvalue == value) {
        values.push_back((int)rvalue);
      } else {
        errmsg << "[option " << optName << " must have an integral value]";
        ++error;
      }
    } else {
      errmsg << "[option " << optName << " must be a LIST of size ["
             << minValues << ", " << maxValues << "]]";
      ++error;
    }
  } else {
    errmsg << "[value of option " << optName
           << " must be of type LIST or REAL]";
    ++error;
  }

  return error;
}

int getArgValues(
  Loci::option_values const & arg,
  std::vector<std::string> const & units,
  int const minValues, int const maxValues,
  std::vector<double> & values,
  std::ostream & errmsg
) {
  values.clear();
  int error = 0;

  std::string argName;
  arg.get_value(argName);

  Loci::option_value_type type = arg.type_of();

  if(type == Loci::NAME_ASSIGN) {
    Loci::option_values::value_list_type argValues;
    arg.get_value(argValues);
    int const sz = argValues.size();
    if(sz != 1) {
      errmsg << "[argument " << argName << " must have only one value]";
      ++error;
    } else {
      if(argValues[0].type_of() == Loci::LIST) {
        Loci::option_values::value_list_type listValues;
        argValues[0].get_value(listValues);
        int const lsz = listValues.size();
        if(minValues <= lsz && lsz <= maxValues) {
          for(int i = 0; i < lsz; ++i) {
            if(listValues[i].type_of() == Loci::UNIT_VALUE) {
              if(units.size() > (size_t)i && !units[i].empty()) {
                Loci::UNIT_type ut;
                listValues[i].get_value(ut);
                if(ut.is_compatible(units[i])) {
                  double value = ut.get_value_in(units[i]);
                  values.push_back(value);
                } else {
                  errmsg << "[unit of element " << i << " of argument " << argName
                         << " LIST must be compatible with " << units[i] << "]";
                  ++error;
                }
              } else {
                errmsg << "[element " << i << " of argument " << argName
                       << " LIST must be unit-less]";
                ++error;
              }
            } if(listValues[i].type_of() == Loci::REAL) {
              double value;
              listValues[i].get_value(value);
              values.push_back(value);
            } else {
              errmsg << "[element " << i << " of argument " << argName
                     << " LIST must be of type REAL]";
              ++error;
            }
          }
        } else {
          errmsg << "[argument " << argName << " must be a LIST of size ["
                 << minValues << ", " << maxValues << "]]";
          ++error;
        }
      } else if(argValues[0].type_of() == Loci::UNIT_VALUE) {
        if(minValues <= 1 && 1 <= maxValues) {
          if(units.size() > 0 && !units[0].empty()) {
            Loci::UNIT_type ut;
            argValues[0].get_value(ut);
            if(ut.is_compatible(units[0])) {
              double value = ut.get_value_in(units[0]);
              values.push_back(value);
            } else {
              errmsg << "[unit of argument " << argName
                     << " must be compatible with " << units[0] << "]";
              ++error;
            }
          } else {
            errmsg << "[argument " << argName << " must be unit-less]";
            ++error;
          }
        } else {
          errmsg << "[argument " << argName << " must be a LIST of size ["
                 << minValues << ", " << maxValues << "]]";
          ++error;
        }
      } else if(argValues[0].type_of() == Loci::REAL) {
        if(minValues <= 1 && 1 <= maxValues) {
          double value;
          argValues[0].get_value(value);
          values.push_back(value);
        } else {
          errmsg << "[argument " << argName << " must be a LIST of size ["
                 << minValues << ", " << maxValues << "]]";
          ++error;
        }
      } else {
        errmsg << "[argument " << argName << " must be of type LIST or REAL]";
        ++error;
      }
    }
  } else {
    errmsg << "[argument " << argName << " must be a NAME_ASSIGN]";
    ++error;
  }

  return error;
}

int getScaledVector3D(
  Loci::option_values const & arg,
  std::string const & unit,
  Loci::vector3d<double> & vec,
  std::ostream & errmsg
) {
  Loci::vector3d<double> temp(0.0, 0.0, 0.0);
  int error = 0;

  std::string argName;
  arg.get_value(argName);

  Loci::option_value_type type = arg.type_of();

  if(type == Loci::NAME_ASSIGN) {
    Loci::option_values::value_list_type argValues;
    arg.get_value(argValues);
    int const sz1 = argValues.size();
    if(sz1 != 1) {
      errmsg << "[argument " << argName << " must have only one value]";
      ++error;
    } else {
      if(argValues[0].type_of() == Loci::LIST) {
        options_list::arg_list args;
        argValues[0].get_value(args);
        int const sz = args.size();
        if(sz <= 4) {
          if(sz == 4) {
            // first get the magnitude
            double magnitude = 0.0;
            if(args[3].type_of() == Loci::UNIT_VALUE) {
              if(unit.empty()) {
                errmsg << "[element 4 of argument " << argName
                  << " LIST must be unit-less]";
                ++error;
              } else {
                Loci::UNIT_type ut;
                args[3].get_value(ut);
                if(ut.is_compatible(unit)) {
                  magnitude = ut.get_value_in(unit);
                } else {
                  errmsg << "[unit of element 4 of argument " << argName
                    << " LIST must be compatible with " << unit << "]";
                  ++error;
                }
              }
            } else if(args[3].type_of() == Loci::REAL) {
              args[3].get_value(magnitude);
            } else {
              errmsg << "[element 4 of argument " << argName
                << " must be a REAL or LIST]";
              ++error;
            }
	    
            // Then get the direction components
            for(int i = 0; i < 3; ++i) {
              if(args[i].type_of() == Loci::REAL) {
                args[i].get_value(temp[i]);
              } else {
                errmsg << "[element " << i << " of argument " << argName
                  << " must be a REAL]";
                ++error;
              }
            }
	    
            // Multiply the normalized vector with the magnitude.
            temp /= magnitude*norm(temp);
          } else {
            for(int i = 0; i < 3; ++i) {
              if(i < sz) {
                if(args[i].type_of() == Loci::UNIT_VALUE) {
                  if(unit.empty()) {
                    errmsg << "[element " << i << " of argument " << argName
                      << " LIST must be unit-less]";
                    ++error;
                  } else {
                    Loci::UNIT_type ut;
                    args[i].get_value(ut);
                    if(ut.is_compatible(unit)) {
                      temp[i] = ut.get_value_in(unit);
                    } else {
                      errmsg << "[unit of element " << i << " of argument " << argName
                        << " LIST must be compatible with " << unit << "]";
                      ++error;
                    }
                  }
                } else if(args[i].type_of() == Loci::REAL) {
                  args[i].get_value(temp[i]);
                } else {
                  errmsg << "[element " << i << " of argument " << argName
                      << " a REAL or LIST]";
                  ++error;
                }
              }
            }
          }
        } else {
          errmsg << "[argument " << argName << " must be a LIST of size <= 4]";
          ++error;
        }
      } else if(argValues[0].type_of() == Loci::UNIT_VALUE) {
        if(unit.empty()) {
          errmsg << "[argument " << argName << " must be unit-less]";
          ++error;
        } else {
          Loci::UNIT_type ut;
          argValues[0].get_value(ut);
          if(ut.is_compatible(unit)) {
            temp[0] = ut.get_value_in(unit);
          } else {
            errmsg << "[unit of argument " << argName
              << " value must be compatible with " << unit << "]";
            ++error;
          }
        }
      } else if(argValues[0].type_of() == Loci::REAL) {
        argValues[0].get_value(temp[0]);
      }
    }
  } else {
    errmsg << "[argument " << argName << " must be a NAME_ASSIGN]";
    ++error;
  }

  if(!error) vec = temp;

  return error;
}

int getScaledVector3D(
  options_list const & ol, std::string const & optName,
  std::string const & unit,
  Loci::vector3d<double> & vec,
  std::ostream & errmsg
) {
  Loci::vector3d<double> temp(0.0, 0.0, 0.0);
  
  int error = 0;
  Loci::option_value_type type = ol.getOptionValueType(optName);
  
  if(type == Loci::LIST) {
    options_list::arg_list args;
    ol.getOption(optName, args);
    int const sz = args.size();
    if(sz <= 4) {
      if(sz == 4) {
        // First get the magnitude
        double magnitude = 0.0;
        if(args[3].type_of() == Loci::UNIT_VALUE) {
          if(unit.empty()) {
            errmsg << "[element 4 of option " << optName
              << " LIST must be unit-less]";
            ++error;
          } else {
            Loci::UNIT_type ut;
            args[3].get_value(ut);
            if(ut.is_compatible(unit)) {
              magnitude = ut.get_value_in(unit);
            } else {
              errmsg << "[unit of element 4 of option " << optName
                << " LIST must be compatible with " << unit << "]";
              ++error;
            }
          }
        } else if(args[3].type_of() == Loci::REAL) {
          args[3].get_value(magnitude);
        } else {
          errmsg << "[element 4 of option " << optName
              << " must be a REAL or LIST]";
          ++error;
        }
      
        // Then get the direction components
        for(int i = 0; i < 3; ++i) {
          if(args[i].type_of() == Loci::REAL) {
            double value;
            args[i].get_value(value);
            temp[i] = value;
          } else {
            errmsg << "[element " << i << " of option " << optName
              << " must be a REAL]";
            ++error;
          }
        }

        // Multiply the normalized vector with the magnitude.
        temp /= magnitude*norm(temp);
      } else {
        for(int i = 0; i < 3; ++i) {
          if(i < sz) {
            if(args[i].type_of() == Loci::UNIT_VALUE) {
              if(unit.empty()) {
                errmsg << "[element " << i << " of option " << optName
                  << " LIST must be unit-less]";
                ++error;
              } else {
                Loci::UNIT_type ut;
                args[i].get_value(ut);
                if(ut.is_compatible(unit)) {
                  double value = ut.get_value_in(unit);
                  temp[i] = value;
                } else {
                  errmsg << "[unit of element " << i << " of option " << optName
                    << " LIST must be compatible with " << unit << "]";
                  ++error;
                }
              }
            } else if(args[i].type_of() == Loci::REAL) {
              double value;
              args[i].get_value(value);
              temp[i] = value;
            } else {
              errmsg << "[element " << i << " of option " << optName
                  << " must be a REAL or LIST]";
              ++error;
            }
          }
        }
      }
    } else {
      errmsg << "[option " << optName << " must be a LIST of size <= 4]";
      ++error;
    }
  } else if(type == Loci::UNIT_VALUE) {
    if(unit.empty()) {
      errmsg << "[option " << optName << " must be unit-less]";
      ++error;
    } else {
      Loci::UNIT_type ut;
      ol.getOption(optName, ut);
      if(ut.is_compatible(unit)) {
        temp[0] = ut.get_value_in(unit);
      } else {
        errmsg << "[unit of option " << optName
          << " must be compatible with " << unit << "]";
        ++error;
      }
    }
  } else if(type == Loci::REAL) {
    ol.getOption(optName, temp[0]);
  } else {
    errmsg << "[option " << optName << " must be a LIST, UNIT_TYPE, or REAL]";
    ++error;
  }

  if(!error) vec = temp;

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
          if(units.size() > (size_t)i && !units[i].empty()) {
            Loci::UNIT_type ut;
            args[i].get_value(ut);
            if(ut.is_compatible(units[i])) {
              double value = ut.get_value_in(units[i]);
              values.push_back(value);
            } else {
              errmsg << "[unit of element " << i << " of option " << optName
                << " LIST must be compatible with " << units[i] << "]";
              ++error;
            }
          } else {
            errmsg << "[element " << i << " of option " << optName
              << " LIST must be unit-less]";
            ++error;
          }
        } else if(args[i].type_of() == Loci::REAL) {
          double value;
          args[i].get_value(value);
          values.push_back(value);
        } else {
          errmsg << "[element " << i << " of option " << optName
            << " LIST must be of type UNIT_VALUE or REAL]";
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
      errmsg << "[option " << optName << " must be a LIST of size ["
             << minValues << ", " << maxValues << "]]";
      ++error;
    }
  } else if(type == Loci::REAL) {
    if(minValues <= 1 && 1 <= maxValues) {
      double value;
      ol.getOption(optName, value);
      values.push_back(value);
    } else {
      errmsg << "[option " << optName << " must be a LIST of size ["
             << minValues << ", " << maxValues << "]]";
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
  options_list::arg_list const & args,
  std::string const & olName,
  std::string const & pressureOptName,
  std::string const & temperatureOptName,
  std::string const & densityOptName,
  ThermoSpec & spec,
  std::ostream & errmsg
) {
  unsigned int flags = 0;
  int error = 0;
  ThermoSpec tmp;

  for(auto const & arg : args) {
    std::string argName;
    arg.get_value(argName);

    if(argName == pressureOptName) {
      flags |= 0x1u;
      std::ostringstream ss;
      if(getArgValue(arg, "Pa", tmp.pressure, ss)) {
        errmsg << "[Error parsing " << olName << "::" << pressureOptName << " : "
               << ss.str() << "]";
        ++error;
      }
    }

    if(argName == temperatureOptName) {
      flags |= 0x2u;
      std::ostringstream ss;
      if(getArgValue(arg, "K", tmp.temperature, ss)) {
        errmsg << "[Error parsing " << olName << "::" << temperatureOptName << " : "
               << ss.str() << "]";
        ++error;
      }
    }

    if(argName == densityOptName) {
      flags |= 0x4u;
      std::ostringstream ss;
      if(getArgValue(arg, "kg/m/m/m", tmp.density, ss)) {
        errmsg << "[Error parsing " << olName << "::" << densityOptName << " : "
               << ss.str() << "]";
        ++error;
      }
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
  s << ' ' << obj.type << ' ' << obj.value.x << ' ' << obj.value.y
    << ' ' << obj.value.z << ' ';
  return s;
}

std::istream & operator>>(std::istream & s, VelSpec & obj) {
  VelSpecType type;
  Loci::vector3d<double> value;
  s >> type >> value.x >> value.y >> value.z;
  if(s) {
    obj.type = type;
    obj.value = value;
  }
  return s;
}

int getVelSpec(
  options_list::arg_list const & args,
  std::string const & olName,
  std::string const & velocityOptName,
  std::string const & MachOptName,
  VelSpec & spec,
  std::ostream & errmsg
) {
  unsigned int flags = 0;
  int error = 0;
  VelSpec tmp;

  for(auto const & arg : args) {
    std::string argName;
    arg.get_value(argName);

    if(argName == velocityOptName) {
      flags |= 0x1u;

      std::ostringstream ss;
      if(getScaledVector3D(
        arg, "m/s", tmp.value, ss
      )) {
        errmsg << "[Error parsing " << olName << "::" << velocityOptName
          << " : " << ss.str() << "]";
        ++error;
      }
    }

    if(argName == MachOptName) {
      flags |= 0x2u;

      std::ostringstream ss;
      if(getScaledVector3D(
        arg, "", tmp.value, ss
      )) {
        errmsg << "[Error parsing " << olName << "::" << MachOptName
          << " : " << ss.str() << "]";
        ++error;
      }
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

int getVelSpec(
  options_list const & ol,
  std::string const & olName,
  std::string const & velocityOptName,
  std::string const & MachOptName,
  VelSpec & spec,
  std::ostream & errmsg
) {
  unsigned int flags = 0;
  int error = 0;
  VelSpec tmp;

  if(ol.optionExists(velocityOptName)) {
    flags |= 0x1u;

    std::ostringstream ss;
    if(getScaledVector3D(
      ol, velocityOptName, "m/s", tmp.value, ss
    )) {
      errmsg << "[Error parsing " << olName << "::" << velocityOptName
        << " : " << ss.str() << "]";
      ++error;
    }
  }
  
  if(ol.optionExists(MachOptName)) {
    flags |= 0x2u;

    std::ostringstream ss;
    if(getScaledVector3D(
      ol, MachOptName, "", tmp.value, ss
    )) {
      errmsg << "[Error parsing " << olName << "::" << MachOptName
        << " : " << ss.str() << "]";
      ++error;
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
  options_list::arg_list const & args,
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

  for(auto const & arg : args) {
    std::string argName;
    arg.get_value(argName);

    if(argName == molarOptName) {
      flags |= 0x1u;
      tmp.fractions.data.clear();

      std::ostringstream ss;
      if(getArgValues(
        arg, std::vector<std::string>(), count-1, count,
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

    if(argName == massOptName) {
      flags |= 0x2u;
      tmp.fractions.data.clear();

      std::ostringstream ss;
      if(getArgValues(
        arg, std::vector<std::string>(), count-1,
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

int getStateSpec(
  options_list const & ol,
  std::string const & optName,
  std::string const & functionName,
  std::string const & pressureOptName,
  std::string const & temperatureOptName,
  std::string const & densityOptName,
  std::string const & velocityOptName,
  std::string const & MachOptName,
  std::string const & molarOptName,
  std::string const & massOptName,
  int const speciesCount,
  ThermoSpec & thermoSpec,
  VelSpec & velSpec,
  SpeciesSpec & speciesSpec,
  std::ostream & errmsg
) {
  int error = 0;

  if(ol.getOptionValueType(optName) == Loci::FUNCTION) {
    std::string fName;
    options_list::arg_list args;
    ol.getOption(optName, fName, args);
    if(fName == functionName) {
      if(getStateSpec(
        args, functionName,
        "pressure", "temperature", "density",
        "velocity", "Mach",
        "molarFractions", "massFractions",
        speciesCount,
        thermoSpec, velSpec, speciesSpec,
        errmsg
      )) {
        ++error;
      }
    } else {
      errmsg << "[" << optName << " must be a FUNCTION with name "
             << functionName << "]";
      ++error;
    }
  } else {
    errmsg << "[" << optName << " must be a FUNCTION]";
    ++error;
  }

  return error;
}

int getStateSpec(
  options_list::arg_list const & args,
  std::string const & functionName,
  std::string const & pressureOptName,
  std::string const & temperatureOptName,
  std::string const & densityOptName,
  std::string const & velocityOptName,
  std::string const & MachOptName,
  std::string const & molarOptName,
  std::string const & massOptName,
  int const speciesCount,
  ThermoSpec & thermoSpec,
  VelSpec & velSpec,
  SpeciesSpec & speciesSpec,
  std::ostream & errmsg
) {
  int error = 0;

  if(getThermoSpec(
    args, functionName, "pressure", "temperature", "density", thermoSpec, errmsg
  )) {
    ++error;
  }

  if(getVelSpec(
    args, functionName, "velocity", "Mach", velSpec, errmsg
  )) {
    ++error;
  }

  if(speciesCount > 1) {
    if(getSpeciesSpec(
      args, functionName, "molarFractions", "massFractions", speciesCount, speciesSpec, errmsg
    )) {
      ++error;
    }
  }

  return error;
}

int getRegionGeom(
  std::string const & functionName, Loci::options_list::arg_list const & args,
  ICRegionGeom ** geom, std::ostream & errmsg
) {
  int error = 0;
  *geom = nullptr;

  if(functionName == "leftPlane" || functionName == "rightPlane") {
    bool pointSpecified = false, normalSpecified = false;
    std::vector<double> point, normal;
    double priority = 0;
    options_list::arg_list::const_iterator aiter = args.begin();
    while(aiter != args.end()) {
      std::string argName;
      aiter->get_value(argName);
      if(argName == "point") {
        if(getArgValues(*aiter, {"m", "m", "m"}, 3, 3, point, errmsg)) {
          ++error;
        } else {
          pointSpecified = true;
        }
      } else if(argName == "normal") {
        if(getArgValues(*aiter, {"m", "m", "m"}, 3, 3, normal, errmsg)) {
          ++error;
        } else {
          normalSpecified = true;
        }
      } else if(argName == "priority") {
        if(getArgValue(*aiter, "", priority, errmsg)) {
          ++error;
        } else {
	  priority = max(0.0, priority);
	}
      }
      ++aiter;
    }
    if(pointSpecified && normalSpecified) {
      if(functionName == "leftPlane") {
        LeftPlaneICRegionGeom * leftPlane = new LeftPlaneICRegionGeom();
        leftPlane->setPoint(point[0], point[1], point[2]);
        leftPlane->setNormal(normal[0], normal[1], normal[2]);
        leftPlane->setPriority((int)priority);
        *geom = leftPlane;
      } else {
        RightPlaneICRegionGeom * rightPlane = new RightPlaneICRegionGeom();
        rightPlane->setPoint(point[0], point[1], point[2]);
        rightPlane->setNormal(normal[0], normal[1], normal[2]);
        rightPlane->setPriority((int)priority);
        *geom = rightPlane;
      }
    } else {
      errmsg << "[" << functionName << " requires point and normal]";
      ++error;
    }
  } else if(functionName == "sphere") {
    bool centerSpecified = false, radiusSpecified = false;
    std::vector<double> center;
    double radius, priority = 0;
    options_list::arg_list::const_iterator aiter = args.begin();
    while(aiter != args.end()) {
      std::string argName;
      aiter->get_value(argName);
      if(argName == "center") {
        if(getArgValues(*aiter, {"m", "m", "m"}, 3, 3, center, errmsg)) {
          ++error;
        } else {
          centerSpecified = true;
        }
      } else if(argName == "radius") {
        if(getArgValue(*aiter, "m", radius, errmsg)) {
          ++error;
        } else {
          radiusSpecified = true;
        }
      } else if(argName == "priority") {
        if(getArgValue(*aiter, "", priority, errmsg)) {
          ++error;
        } else {
	  priority = max(0.0, priority);
	}
      }
      ++aiter;
    }
    if(centerSpecified && radiusSpecified) {
      SphereICRegionGeom * sphere = new SphereICRegionGeom();
      sphere->setCenter(center[0], center[1], center[2]);
      sphere->setRadius(radius);
      sphere->setPriority((int)priority);
      *geom = sphere;
    } else {
      errmsg << "[" << functionName << " requires center and radius]";
      ++error;
    }
  } else if(functionName == "box") {
    bool corner1Specified = false, corner2Specified = false;
    std::vector<double> corner1, corner2;
    double priority = 0;
    options_list::arg_list::const_iterator aiter = args.begin();
    while(aiter != args.end()) {
      std::string argName;
      aiter->get_value(argName);
      if(argName == "corner1") {
        if(getArgValues(*aiter, {"m", "m", "m"}, 3, 3, corner1, errmsg)) {
          ++error;
        } else {
          corner1Specified = true;
        }
      } else if(argName == "corner2") {
        if(getArgValues(*aiter, {"m", "m", "m"}, 3, 3, corner2, errmsg)) {
          ++error;
        } else {
          corner2Specified = true;
        }
      } else if(argName == "priority") {
        if(getArgValue(*aiter, "", priority, errmsg)) {
          ++error;
        } else {
	  priority = max(0.0, priority);
	}
      }
      ++aiter;
    }
    if(corner1Specified && corner2Specified) {
      BoxICRegionGeom * box = new BoxICRegionGeom();
      box->setCorners(
        min(corner1[0], corner2[0]), min(corner1[1], corner2[1]), min(corner1[2], corner2[2]),
	max(corner1[0], corner2[0]), max(corner1[1], corner2[1]), max(corner1[2], corner2[2])
      );
      box->setPriority((int)priority);
      *geom = box;
    } else {
      errmsg << "[" << functionName << " requires corner1 and corner2]";
      ++error;
    }
  } else if(functionName == "convexPolyhedron") {
    std::set<std::string> names;
    std::map<std::string, bool> pointSpecified, normalSpecified;
    std::map<std::string, double> px, py, pz, nx, ny, nz;
    std::vector<double> point, normal;
    double priority = 0;
    options_list::arg_list::const_iterator aiter = args.begin();
    while(aiter != args.end()) {
      std::string argName;
      aiter->get_value(argName);
      if(argName.compare(0, 5, "point") == 0) {
        std::string name = argName.substr(5);
        if(name.size() > 0) {
          if(getArgValues(*aiter, {"m", "m", "m"}, 3, 3, point, errmsg)) {
            ++error;
          } else {
            names.insert(name);
            pointSpecified.insert(std::pair<std::string, bool>(name, true));
            px.insert(std::pair<std::string, double>(name, point[0]));
            py.insert(std::pair<std::string, double>(name, point[1]));
            pz.insert(std::pair<std::string, double>(name, point[2]));
          }
        }
      } else if(argName.compare(0, 6, "normal") == 0) {
        std::string name = argName.substr(6);
        if(name.size() > 0) {
          if(getArgValues(*aiter, {"m", "m", "m"}, 3, 3, normal, errmsg)) {
            ++error;
          } else {
            names.insert(name);
            normalSpecified.insert(std::pair<std::string, bool>(name, true));
            nx.insert(std::pair<std::string, double>(name, normal[0]));
            ny.insert(std::pair<std::string, double>(name, normal[1]));
            nz.insert(std::pair<std::string, double>(name, normal[2]));
          }
        }
      } else if(argName == "priority") {
        if(getArgValue(*aiter, "", priority, errmsg)) {
          ++error;
        } else {
	  priority = max(0.0, priority);
	}
      }
      ++aiter;
    }

    ConvexPolyhedronICRegionGeom * convexPolyhedron = new ConvexPolyhedronICRegionGeom;
    size_t size = names.size();
    std::set<std::string>::const_iterator niter = names.begin();
    bool allSpecified = true;
    while(niter != names.end()) {
      std::map<std::string, bool>::const_iterator ps = pointSpecified.find(*niter);
      std::map<std::string, bool>::const_iterator ns = normalSpecified.find(*niter);
      if(ps != pointSpecified.end() && ns != normalSpecified.end()) {
        convexPolyhedron->addPlane(
          px[*niter], py[*niter], pz[*niter],
          nx[*niter], ny[*niter], nz[*niter]
        );
      } else {
        errmsg << "[" << functionName << ", plane " << *niter
               << " requires point and normal]";
        allSpecified = false;
        ++error;
        break;
      }
      ++niter;
    }
    convexPolyhedron->setPriority((int)priority);

    if(allSpecified && size > 0) {
      *geom = convexPolyhedron;
    } else {
      delete convexPolyhedron;
    }
  } else {
    errmsg << "[unknown region type: " << functionName << "]";
    ++error;
  }

  return error;
}

int getRegionGeom(
  Loci::options_list const & ol, std::string const & optName,
  ICRegionGeom ** geom, std::ostream & errmsg
) {
  int error = 0;

  Loci::option_value_type type = ol.getOptionValueType(optName);
  if(type == Loci::FUNCTION) {
    std::string functionName;
    options_list::arg_list args;
    ol.getOption(optName, functionName, args);
    error = getRegionGeom(functionName, args, geom, errmsg);
  } else {
    errmsg << optName << " must be a FUNCTION";
    ++error;
  }

  return error;
}

GeomType SphereICRegionGeom::type() const {
  return GEOM_SPHERE;
}

bool SphereICRegionGeom::in(double x, double y, double z) const {
  double dx = x-cx;
  double dy = y-cy;
  double dz = z-cz;
  double dist2 = dx*dx+dy*dy+dz*dz;
  return dist2 <= r2;
}

GeomType ConvexPolyhedronICRegionGeom::type() const {
  return GEOM_CONVEX_POLYHEDRON;
}

bool ConvexPolyhedronICRegionGeom::in(double x, double y, double z) const {
  size_t size = px.size();
  size_t count = 0;
  for(size_t i = 0; i < size; ++i) {
    double dx = x-px[i];
    double dy = y-py[i];
    double dz = z-pz[i];
    double dot = dx*nx[i]+dy*ny[i]+dz*nz[i];
    count += dot <= 0.0;
  }
  return count == size;
}

GeomType BoxICRegionGeom::type() const {
  return GEOM_BOX;
}

bool BoxICRegionGeom::in(double x, double y, double z) const {
  return x >= px1 && x <= px2 && y >= py1 && y <= py2 && z >= pz1 && z <= pz2;
}

GeomType LeftPlaneICRegionGeom::type() const {
  return GEOM_LEFT_PLANE;
}

bool LeftPlaneICRegionGeom::in(double x, double y, double z) const {
  double dx = x-px;
  double dy = y-py;
  double dz = z-pz;
  double dot = dx*nx+dy*ny+dz*nz;
  return dot <= 0.0;
}

GeomType RightPlaneICRegionGeom::type() const {
  return GEOM_RIGHT_PLANE;
}

bool RightPlaneICRegionGeom::in(double x, double y, double z) const {
  double dx = x-px;
  double dy = y-py;
  double dz = z-pz;
  double dot = dx*nx+dy*ny+dz*nz;
  return dot >= 0.0;
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
