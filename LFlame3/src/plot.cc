#include <flame.hh>
#include <plot.hh>

#include <mpi.h>
#include <glog/logging.h>

#include <sstream>

namespace flame {

// =============================================================================

PrintParameterDB::PrintParameterDB() : printHeader(true) {
}

void PrintParameterDB::clear() {
  name.clear();
  value.clear();
}

void PrintParameterDB::add(int const root, std::string const & n, double const v) {
  double val = v;
  MPI_Bcast(&val, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
  name.push_back(n);
  value.push_back(val);
}

std::ostream & PrintParameterDB::print(int const timeStep, std::ostream & s) {
  int const nvalues = value.size();
  
  if(printHeader) {
    s << "timeStep ";
    for(int i = 0; i < nvalues; ++i) {
      s << name[i] << ' ';
    }
    s << std::endl;
    printHeader = false;
  }
  
  s << timeStep << ' ';
  for(int i = 0; i < nvalues; ++i) {
    s << value[i] << ' ';
  }
  s << std::endl;
  
  return s;
}

// =============================================================================

PrintParameterFile printParameterFile;
PrintParameterDB printParameterDB;

// =============================================================================

std::string PrintSettings::toString() const {
  std::ostringstream ss;
  ss << *this;
  return ss.str();
}

void PrintSettings::fromString(std::string const & str) {
  std::istringstream ss(str);
  ss >> *this;
}

int PrintSettings::fromOptionsList(options_list const & ol, std::string & err) {
  int error = 0;
  std::vector<std::string> parameters;
  int frequency;
  std::string filename;
  
  options_list::option_namelist nl = ol.getOptionNameList();
  options_list::option_namelist::iterator iter = nl.begin();
  
  while(iter != nl.end()) {
    std::string optionName = *iter;
    if(optionName == "parameters") {
      if(ol.getOptionValueType("parameters") == Loci::STRING) {
        std::string vars, var;
        ol.getOption("parameters", vars);
        for(std::string::iterator i = vars.begin(); i != vars.end(); ++i) {
          if(*i == ',') {
            if(var.size() > 0) parameters.push_back(var);
            var.clear();
          } else if(!std::isspace(*i)) {
            var.push_back(*i);
          }
        }
        if(var.size() > 0) parameters.push_back(var);
      } else {
        err += "[Expected STRING type for parameters option]";
        ++error;
      }
    } else if(optionName == "frequency") {
      if(ol.getOptionValueType("frequency") == Loci::REAL) {
        double value;
        ol.getOption("frequency", value);
        frequency = (int)value;
      } else {
        err += "[Expected REAL type for frequency option]";
        ++error;
      }
    } else if(optionName == "filename") {
      if(ol.getOptionValueType("filename") == Loci::STRING) {
        ol.getOption("filename", filename);
      } else {
        err += "[Expected STRING type for filename option]";
        ++error;
      }
    } else {
      err += "[Unknown option in \"printOptions\": " + optionName + "]";
      ++error;
    }
    ++iter;
  }
  
  if(!error) {
    this->parameters = parameters;
    this->frequency = frequency;
    this->filename = filename;
  }
  
  return error;
}

std::ostream & operator<<(std::ostream & s, PrintSettings const & rhs) {
  int const nParameters = rhs.parameters.size();
  
  s << nParameters << " ";
  for(int i = 0; i < nParameters; ++i) {
    s << rhs.parameters[i] << " ";
  }
  
  s << rhs.frequency << " ";
  
  s << rhs.filename << " ";
  
  return s;
}

std::istream & operator>>(std::istream & s, PrintSettings & rhs) {
  int nParameters;
  
  s >> nParameters;
  rhs.parameters.resize(nParameters);
  for(int i = 0; i < nParameters; ++i) {
    s >> rhs.parameters[i];
  }
  
  s >> rhs.frequency;
  
  s >> rhs.filename;
  
  return s;
}

// =============================================================================

std::string PlotSettings::toString() const {
  std::ostringstream ss;
  ss << *this;
  return ss.str();
}

void PlotSettings::fromString(std::string const & str) {
  std::istringstream ss(str);
  ss >> *this;
}

int PlotSettings::fromOptionsList(options_list const & ol, std::string & err) {
  int error = 0;
  std::vector<int> frequencies;
  std::vector<int> counts;
  std::vector<std::string> nodalVariables;
  
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
            if(var.size() > 0) nodalVariables.push_back(var);
            var.clear();
          } else if(!std::isspace(*i)) {
            var.push_back(*i);
          }
        }
        if(var.size() > 0) nodalVariables.push_back(var);
      } else {
        err += "[Expected STRING type for nodalVariables option.]";
        ++error;
      }
    } else if(optionName == "boundaryVariables") {
      if(ol.getOptionValueType("boundaryVariables") == Loci::STRING) {
        std::string vars, var;
        ol.getOption("boundaryVariables", vars);
        for(std::string::iterator i = vars.begin(); i != vars.end(); ++i) {
          if(*i == ',') {
            if(var.size() > 0) boundaryVariables.push_back(var);
            var.clear();
          } else if(!std::isspace(*i)) {
            var.push_back(*i);
          }
        }
        if(var.size() > 0) boundaryVariables.push_back(var);
      } else {
        err += "[Expected STRING type for boundaryVariables option.]";
        ++error;
      }
    } else if(optionName == "frequencies") {
      if(ol.getOptionValueType("frequencies") == Loci::LIST) {
        options_list::arg_list valueList;
        ol.getOption("frequencies", valueList);
        int size = valueList.size();
        for(int i = 0; i < size; ++i) {
          if(valueList[i].type_of() == Loci::REAL) {
            double value;
            valueList[i].get_value(value);
            frequencies.push_back((int)value);
          } else {
            err += "[Expected REAL type for items in frequencies option]";
            ++error;
          }
        }
      } else {
        err += "[Expected LIST type for frequencies option]";
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
            err += "[Expected REAL type for items in counts option]";
            ++error;
          }
        }
      } else {
        err += "[Expected LIST type for counts option]";
        ++error;
      }
    } else {
      err += "[Unknown option in \"plotOptions\": " + optionName + "]";
      ++error;
    }
    ++iter;
  }
  
  if(frequencies.size() != counts.size()) {
    err += "[frequencies and counts LISTs in plotOptions should have the same number of items]";
    ++error;
  }
  
  if(!error) {
    this->frequencies = frequencies;
    this->counts = counts;
    this->nodalVariables = nodalVariables;
  }
  
  return error;
}

std::ostream & operator<<(std::ostream & s, PlotSettings const & rhs) {
  int const nLevels = rhs.counts.size();
  int const nNodalVariables = rhs.nodalVariables.size();
  int const nBoundaryVariables = rhs.boundaryVariables.size();
  
  s << nLevels << " ";
  for(int i = 0; i < nLevels; ++i) {
    s << rhs.frequencies[i] << " " << rhs.counts[i] << " ";
  }
  
  s << nNodalVariables << " ";
  for(int i = 0; i < nNodalVariables; ++i) {
    s << rhs.nodalVariables[i] << " ";
  }
  
  s << nBoundaryVariables << " ";
  for(int i = 0; i < nBoundaryVariables; ++i) {
    s << rhs.boundaryVariables[i] << " ";
  }
  
  return s;
}

std::istream & operator>>(std::istream & s, PlotSettings & rhs) {
  int nLevels, nNodalVariables, nBoundaryVariables;
  
  s >> nLevels;
  rhs.frequencies.resize(nLevels);
  rhs.counts.resize(nLevels);
  for(int i = 0; i < nLevels; ++i) {
    s >> rhs.frequencies[i] >> rhs.counts[i];
  }
  
  s >> nNodalVariables;
  rhs.nodalVariables.resize(nNodalVariables);
  for(int i = 0; i < nNodalVariables; ++i) {
    s >> rhs.nodalVariables[i];
  }
  
  s >> nBoundaryVariables;
  rhs.boundaryVariables.resize(nBoundaryVariables);
  for(int i = 0; i < nBoundaryVariables; ++i) {
    s >> rhs.boundaryVariables[i];
  }
  
  return s;
}

// =============================================================================

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
  std::cout << "filename = " << filename << std::endl;
  hid_t fileId = Loci::hdf5CreateFile(
    filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT
  );
  Loci::writeContainer(fileId, sname, c2n.Rep());
  Loci::hdf5CloseFile(fileId);
}

// =============================================================================

NodalScalarOutput::NodalScalarOutput(
  char const * vname, char const * valname
) {
  var_name = std::string(vname);
  value_name = std::string(valname);
  std::string constraintName = std::string("plotNodal_") + value_name;
  
  name_store(var_name, c2n);
  name_store("caseName", caseName);
  name_store("plotPostfix", plotPostfix);
  name_store("OUTPUT", OUTPUT);
  
  conditional("doPlot");
  
  constraint("pos");
  constraint(constraintName);
  
  input("caseName");
  input("plotPostfix");
  input(var_name);
  
  output("OUTPUT");
  
  disable_threading();
}

void NodalScalarOutput::compute(sequence const & seq) {
  dumpScalar(seq, c2n, *caseName, *plotPostfix, "sca", value_name);
}

// =============================================================================

NodalVecComponentsOutput::NodalVecComponentsOutput(
  char const * vname, char const * valname, char const * cnames,
  char const * extraConstraints
) {
  var_name = std::string(vname);
  value_name = std::string(valname);
  component_names = std::string(cnames);
  std::string constraintName = std::string("plotNodal_") + value_name;
  
  name_store(var_name, c2n);
  name_store(component_names, componentNames);
  name_store("caseName", caseName);
  name_store("plotPostfix", plotPostfix);
  name_store("OUTPUT", OUTPUT);
  
  conditional("doPlot");
  
  constraint("pos");
  constraint(constraintName);
  constraint(extraConstraints);
  
  input("caseName");
  input("plotPostfix");
  input(var_name);
  input(component_names);
  
  output("OUTPUT");
  
  disable_threading();
}

void NodalVecComponentsOutput::compute(sequence const & seq) {
  std::vector<std::string> const & names = *componentNames;
  int const vs = c2n.vecSize();
  
  if(names.size() != vs) {
    LOG(ERROR) << "NodalVecComponentsOutput: vector size and component "
      "name size does not match";
    Loci::Abort();
  }
  
  entitySet dom(seq);
  store<float> tmp;
  tmp.allocate(dom);
  
  for(int i = 0; i < vs; ++i) {
    FORALL(dom, ii) {
      tmp[ii] = c2n[ii][i];
    } ENDFORALL;
    
    std::ostringstream ss;
    ss << value_name << "" << names[i];
    
    Loci::const_store<float> st;
    st.setRep(tmp.Rep());
    
    dumpScalar(seq, st, *caseName, *plotPostfix, "sca", ss.str());
  }
}

// =============================================================================

void dumpVector(
  sequence const & seq,
  const_store<Loci::vector3d<float> > & c2n,
  std::string const & caseName,
  std::string const & plotPostfix,
  std::string const & type,
  std::string const & sname
) {
  std::stringstream ss;
  ss << "output/" << sname << "_" << type << "." << plotPostfix
    << "_" << caseName;
  std::string filename = ss.str();
  std::cout << "filename = " << filename << std::endl;
  hid_t fileId = Loci::hdf5CreateFile(
    filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT
  );
  Loci::writeContainer(fileId, sname, c2n.Rep());
  Loci::hdf5CloseFile(fileId);
}

NodalVectorOutput::NodalVectorOutput(
  char const * vname, char const * valname
) {
  var_name = std::string(vname);
  value_name = std::string(valname);
  std::string constraintName = std::string("plotNodal_") + value_name;
  
  name_store(var_name, c2n);
  name_store("caseName", caseName);
  name_store("plotPostfix", plotPostfix);
  name_store("OUTPUT", OUTPUT);
  
  conditional("doPlot");
  
  constraint("pos");
  constraint(constraintName);
  
  input("caseName");
  input("plotPostfix");
  input(var_name);
  
  output("OUTPUT");
  
  disable_threading();
}

void NodalVectorOutput::compute(sequence const & seq) {
  dumpVector(seq, c2n, *caseName, *plotPostfix, "vec", value_name);
}

void dumpVar(
  sequence const & seq,
  Loci::storeRepP var,
  std::string const & caseName,
  std::string const & plotPostfix,
  std::string const & type,
  std::string const & sname
) {
  std::stringstream ss;
  ss << "output/" << sname << "_" << type << "." << plotPostfix
    << "_" << caseName;
  std::string filename = ss.str();
  std::cout << "filename = " << filename << std::endl;
  hid_t fileId = Loci::hdf5CreateFile(
    filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT
  );
  Loci::writeContainer(fileId, sname, var);
  Loci::hdf5CloseFile(fileId);
}

ParamOutput::ParamOutput(char const * vname, char const * valname) {
  var_name = std::string(vname);
  value_name = std::string(valname);
  std::string constraintName = std::string("plotParam_") + value_name;
  
  name_store(var_name, param);
  name_store("caseName", caseName);
  name_store("plotPostfix", plotPostfix);
  name_store("OUTPUT", OUTPUT);
  
  conditional("doPlot");
  
  constraint(constraintName);
  
  input("caseName");
  input("plotPostfix");
  input(var_name);
  output("OUTPUT");
  
  disable_threading();
}

void ParamOutput::compute(Loci::sequence const & seq) {
  Loci::param<float> tmp;
  *tmp = *param;
  dumpVar(seq, tmp.Rep(), *caseName, *plotPostfix, "par", "Pambient");
}

class SurfaceBoundaryScalar : public Loci::pointwise_rule {
  std::string var_name;
  std::string value_name;
  Loci::const_param<std::string> boundaryName;
  Loci::const_store<double> var;
  Loci::const_param<std::string> caseName;
  Loci::const_param<std::string> plotPostfix;
  Loci::param<bool> OUTPUT;
  
public:
  SurfaceBoundaryScalar(char const * vname, char const * valname);
  virtual void compute(Loci::sequence const & seq);
};

BoundaryScalarOutput::BoundaryScalarOutput(char const * vname, char const * valname) {
  var_name = string(vname);
  value_name = string(valname);
  std::string constraintName = std::string("plotBoundary_") + value_name;
  
  name_store(var_name, var);
  name_store("plotPostfix", plotPostfix);
  name_store("OUTPUT", OUTPUT);
  name_store("caseName", caseName);
  
  conditional("doPlot");
  
  constraint("ci->vol");
  constraint(constraintName);
  
  input(var_name);
  input("caseName");
  input("plotPostfix");
  
  output("OUTPUT");
  
  disable_threading();
}

void BoundaryScalarOutput::compute(Loci::sequence const & seq) {
  std::stringstream ss;
  ss << "output/" + value_name + "_bnd." << *plotPostfix << "_" << *caseName;
  std::string filename = ss.str();
  
  Loci::entitySet set(seq);
  hid_t fileId = Loci::createUnorderedFile(filename.c_str(), set);
  Loci::writeUnorderedStore(fileId, var, set, value_name.c_str());
  Loci::closeUnorderedFile(fileId);
}

BoundaryVectorOutput::BoundaryVectorOutput(char const * vname, char const * valname) {
  var_name = string(vname);
  value_name = string(valname);
  std::string constraintName = std::string("plotBoundary_") + value_name;
  
  name_store(var_name, var);
  name_store("plotPostfix", plotPostfix);
  name_store("OUTPUT", OUTPUT);
  name_store("caseName", caseName);
  
  conditional("doPlot");
  
  constraint("ci->vol");
  constraint(constraintName);
  
  input(var_name);
  input("caseName");
  input("plotPostfix");
  
  output("OUTPUT");
  
  disable_threading();
}

void BoundaryVectorOutput::compute(Loci::sequence const & seq) {
  std::stringstream ss;
  ss << "output/" + value_name + "_bndvec." << *plotPostfix << "_" << *caseName;
  std::string filename = ss.str();
  
  Loci::entitySet set(seq);
  hid_t fileId = Loci::createUnorderedFile(filename.c_str(), set);
  Loci::writeUnorderedStore(fileId, var, set, value_name.c_str());
  Loci::closeUnorderedFile(fileId);
}

} // end: namespace flame
