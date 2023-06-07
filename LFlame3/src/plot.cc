#include <plot.hh>

#include <sstream>

namespace flame {

std::string PlotSettings::toString() const {
  std::stringstream ss;
  int nLevels = counts.size();
  int nVariables = variables.size();
  ss << frequency << " ";
  ss << nLevels << " ";
  for(int i = 0; i < nLevels; ++i) {
    ss << counts[i] << " ";
  }
  ss << nVariables << " ";
  for(int i = 0; i < nVariables; ++i) {
    ss << variables[i];
    if(i < nVariables-1) ss << " ";
  }
  
  return ss.str();
}

void PlotSettings::fromString(std::string const & str) {
  std::istringstream ss(str);
  int nLevels, nVariables;
  ss >> frequency;
  ss >> nLevels;
  counts.resize(nLevels);
  for(int i = 0; i < nLevels; ++i) {
    ss >> counts[i];
  }
  
  ss >> nVariables;
  variables.resize(nVariables);
  for(int i = 0; i < nVariables; ++i) {
    ss >> variables[i];
  }
}

int PlotSettings::fromOptionsList(options_list const & ol, std::string & err) {
  int error = 0;
  int frequency = 0;
  std::vector<int> counts;
  std::vector<std::string> variables;
  
  options_list::option_namelist nl = ol.getOptionNameList();
  options_list::option_namelist::iterator iter = nl.begin();
  
  while(iter != nl.end()) {
    std::string optionName = *iter;
    if(optionName == "nodalVariables") {
      if(ol.getOptionValueType("nodalVariables") == Loci::STRING) {
        std::string vars, var;
        ol.getOption("nodalVariables", vars);
        for(std::string::iterator i = vars.begin(); i != vars.end(); ++i) {
          if(*i == ',') {
            if(var.size() > 0) variables.push_back(var);
            var.clear();
          } else if(!std::isspace(*i)) {
            var.push_back(*i);
          }
        }
        if(var.size() > 0) variables.push_back(var);
      } else {
        err += "[Expected STRING type of nodalVariables option.]";
        ++error;
      }
    } else if(optionName == "frequency") {
      if(ol.getOptionValueType("frequency") == Loci::REAL) {
        double value;
        ol.getOption("frequency", value);
        frequency = (int)value;
      } else {
        err += "[Expected REAL type of frequency option]";
        ++error;
      }
    } else if(optionName == "counts") {
      if(ol.getOptionValueType("counts") == Loci::LIST) {
        options_list::arg_list valueList;
        ol.getOption("counts", valueList);
        int size = valueList.size();
        for(int i = 0; i < size; ++i) {
          if(valueList[i].type_of() == Loci::REAL) {
            double value;
            valueList[i].get_value(value);
            counts.push_back((int)value);
          } else {
            err += "[Expected REAL type for items in count option]";
            ++error;
          }
        }
      } else {
        err += "[Expected LIST type of count option]";
        ++error;
      }
    } else {
      err += "[Unknown option in \"plotOptions\": " + optionName + "]";
      ++error;
    }
    ++iter;
  }
  
  this->frequency = frequency;
  this->counts = counts;
  this->variables = variables;
  
  return error;
}

std::ostream & operator<<(std::ostream & s, PlotSettings const & rhs) {
  s << rhs.toString();
  return s;
}

std::istream & operator>>(std::istream & s, PlotSettings & rhs) {
  int nLevels, nVariables;
  s >> rhs.frequency;
  s >> nLevels;
  rhs.counts.resize(nLevels);
  for(int i = 0; i < nLevels; ++i) {
    s >> rhs.counts[i];
  }
  s >> nVariables;
  rhs.variables.resize(nVariables);
  for(int i = 0; i < nVariables; ++i) {
    s >> rhs.variables[i];
  }
  return s;
}

void dumpScalar(
  sequence const & seq,
  const_store<float> & c2n,
  std::string const & caseName,
  std::string const & plotPostfix,
  std::string const & type,
  std::string const & sname
) {
  std::stringstream ss;
  ss << "output/" << sname << "_" << type << "." << plotPostfix
    << "_" << caseName;
  std::string filename = ss.str();
  hid_t fileId = Loci::hdf5CreateFile(
    filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT
  );
  Loci::writeContainer(fileId, sname, c2n.Rep());
  Loci::hdf5CloseFile(fileId);
}

NodalScalarOutput::NodalScalarOutput(
  const char * vname, const char * valname
) {
  var_name = std::string(vname);
  value_name = std::string(valname);
  std::string var_name_time = var_name + "{n}"; 
  std::string constraintName = std::string("plotNodal_") + value_name;
  name_store(var_name_time, c2n);
  name_store("caseName", caseName);
  name_store("plotPostfix{n}",plotPostfix);
  name_store("OUTPUT{n}",OUTPUT);
  
  conditional("doPlot{n}");
  constraint("pos{n}");
  input("caseName");
  input("plotPostfix{n}");
  input(var_name_time);
  constraint(constraintName);
  output("OUTPUT{n}");
}

void NodalScalarOutput::compute(const sequence &seq) {
  dumpScalar(seq, c2n, *plotPostfix,*caseName,"sca", value_name);
}

} // end: namespace flame
