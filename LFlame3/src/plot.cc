#include <flame.hh>
#include <plot.hh>

#include <mpi.h>

#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>

#include <sstream>

namespace flame {

// =============================================================================

int PrintParameterFile::create(char const * filename) {
  int error = 0;
  
  if(file == nullptr) {
    LOG(INFO) << "Opening file '" << filename << "' for appending";
    fileName = filename;
    file = new std::ofstream(filename, std::ios_base::app);
    file->precision(15);
  }
  
  if(file->is_open()) {
    if(!(*file)) {
      LOG(ERROR) << "File '" << fileName << "' is in bad state";
      error = 2;
    }
  } else {
    LOG(ERROR) << "File '" << fileName << "' is not open";
    error = 1;
  }
  
  return error;
}

// =============================================================================

PrintParameterDB::PrintParameterDB() : printHeader(true) {
}

void PrintParameterDB::clear() {
  name.clear();
  value.clear();
}

void PrintParameterDB::add(std::string const & n, double const v) {
  double val = v;
  name.push_back(n);
  value.push_back(val);
}

std::ostream & PrintParameterDB::print(
  int const timeStep, double const stime, std::ostream & s
) {
  int const nvalues = value.size();
  
  if(printHeader) {
    s << "timeStep stime ";
    for(int i = 0; i < nvalues; ++i) {
      s << name[i] << ' ';
    }
    s << std::endl;
    printHeader = false;
  }
  
  s << timeStep << ' ' << stime << ' ';
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
  
  std::ostringstream errmsg;
  
  std::vector<std::string> params;
  int freq = 0;
  std::string fname = "integrated_parameters.txt";
  
  options_list::option_namelist li = ol.getOptionNameList();
  
  for(auto const & optName : li) {
    if(optName == "parameters") {
      Loci::option_value_type type = ol.getOptionValueType(optName);
      if(type == Loci::LIST) {
        options_list::arg_list args;
        ol.getOption(optName, args);
        int const sz = args.size();
        for(int i = 0; i < sz; ++i) {
          if(args[i].type_of() == Loci::STRING || args[i].type_of() == Loci::NAME) {
            std::string value;
            args[i].get_value(value);
            params.push_back(value);
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
            if(var.size() > 0) params.push_back(var);
            var.clear();
          } else if(!std::isspace(*i)) {
            var.push_back(*i);
          }
        }
        if(var.size() > 0) params.push_back(var);
      } else {
        errmsg << "[" << optName << " must be of type NAME or STRING or LIST]";
        ++error;
      }
    } else if(optName == "frequency") {
      Loci::option_value_type type = ol.getOptionValueType(optName);
      if(type == Loci::REAL) {
        double value;
        ol.getOption(optName, value);
        freq = (int)value;
      } else {
        errmsg << "[" << optName << " must be of type REAL]";
        ++error;
      }
    } else if(optName == "filename") {
      Loci::option_value_type type = ol.getOptionValueType(optName);
      if(type == Loci::STRING) {
        ol.getOption(optName, fname);
      } else {
        errmsg << "[" << optName << " must be of type STRING]";
        ++error;
      }
    } else {
      errmsg << "[unknown option " + optName + "]";
      ++error;
    }
  }

  if(error) {
    err = errmsg.str();
  } else {
    parameters = params;
    frequency = freq;
    filename = fname;
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
  
  std::ostringstream errmsg;
  
  std::vector<int> freq;
  std::vector<int> cnts;
  std::vector<int> timeSteps;
  std::vector<int> nfreq;
  std::vector<int> ncnts;
  std::vector<int> nTimeSteps;
  std::vector<int> bfreq;
  std::vector<int> bcnts;
  std::vector<int> bTimeSteps;
  std::vector<std::string> nvars;
  std::vector<std::string> bvars;
  
  options_list::option_namelist li = ol.getOptionNameList();
  
  for(auto const & optName : li) {
    if(optName == "nodalVariables") {
      std::vector<std::string> values;
      std::ostringstream ss;
      int err = getOptionValues(ol, optName, 0, std::numeric_limits<int>::max(), values, ss);
      
      if(err == 0) {
        nvars.swap(values);
      } else {
        errmsg << ss.str();
        ++error;
      }
    } else if(optName == "boundaryVariables") {
      std::vector<std::string> values;
      std::ostringstream ss;
      int err = getOptionValues(ol, optName, 0, std::numeric_limits<int>::max(), values, ss);
      
      if(err == 0) {
        bvars.swap(values);
      } else {
        errmsg << ss.str();
        ++error;
      }
    } else if(optName == "boundaryFrequencies") {
      std::vector<int> values;
      std::ostringstream ss;
      int err = getOptionValues(ol, optName, 0, std::numeric_limits<int>::max(), values, ss);
      
      if(err == 0) {
        bool hasError = false;
        for(size_t i = 0; i < values.size(); ++i) {
          if(values[i] <= 0) {
            errmsg << "[element " << i << " of " << optName
              << " must be a non-zero positive integer]";
            ++error;
            hasError = true;
          }
        }
        if(!hasError) {
          bfreq.swap(values);
        }
      } else {
        errmsg << ss.str();
        ++error;
      }
    } else if(optName == "boundaryCounts") {
      std::vector<int> values;
      std::ostringstream ss;
      int err = getOptionValues(ol, optName, 0, std::numeric_limits<int>::max(), values, ss);
      
      if(err == 0) {
        bool hasError = false;
        for(size_t i = 0; i < values.size(); ++i) {
          if(values[i] <= 0) {
            errmsg << "[element " << i << " of " << optName
              << " must be a non-zero positive integer]";
            ++error;
            hasError = true;
          }
        }
        if(!hasError) {
          bcnts.swap(values);
        }
      } else {
        errmsg << ss.str();
        ++error;
      }
    } else if(optName == "nodalFrequencies") {
      std::vector<int> values;
      std::ostringstream ss;
      int err = getOptionValues(ol, optName, 0, std::numeric_limits<int>::max(), values, ss);
      
      if(err == 0) {
        bool hasError = false;
        for(size_t i = 0; i < values.size(); ++i) {
          if(values[i] <= 0) {
            errmsg << "[element " << i << " of " << optName
              << " must be a non-zero positive integer]";
            ++error;
            hasError = true;
          }
        }
        if(!hasError) {
          nfreq.swap(values);
        }
      } else {
        errmsg << ss.str();
        ++error;
      }
    } else if(optName == "nodalCounts") {
      std::vector<int> values;
      std::ostringstream ss;
      int err = getOptionValues(ol, optName, 0, std::numeric_limits<int>::max(), values, ss);
      
      if(err == 0) {
        bool hasError = false;
        for(size_t i = 0; i < values.size(); ++i) {
          if(values[i] <= 0) {
            errmsg << "[element " << i << " of " << optName
              << " must be a non-zero positive integer]";
            ++error;
            hasError = true;
          }
        }
        if(!hasError) {
          ncnts.swap(values);
        }
      } else {
        errmsg << ss.str();
        ++error;
      }
    } else if(optName == "frequencies") {
      std::vector<int> values;
      std::ostringstream ss;
      int err = getOptionValues(ol, optName, 0, std::numeric_limits<int>::max(), values, ss);
      
      if(err == 0) {
        bool hasError = false;
        for(size_t i = 0; i < values.size(); ++i) {
          if(values[i] <= 0) {
            errmsg << "[element " << i << " of " << optName
              << " must be a non-zero positive integer]";
            ++error;
            hasError = true;
          }
        }
        if(!hasError) {
          freq.swap(values);
        }
      } else {
        errmsg << ss.str();
        ++error;
      }
    } else if(optName == "counts") {
      std::vector<int> values;
      std::ostringstream ss;
      int err = getOptionValues(ol, optName, 0, std::numeric_limits<int>::max(), values, ss);
      
      if(err == 0) {
        bool hasError = false;
        for(size_t i = 0; i < values.size(); ++i) {
          if(values[i] <= 0) {
            errmsg << "[element " << i << " of " << optName
              << " must be a non-zero positive integer]";
            ++error;
            hasError = true;
          }
        }
        if(!hasError) {
          cnts.swap(values);
        }
      } else {
        errmsg << ss.str();
        ++error;
      }
    } else if(optName == "timeSteps") {
      std::vector<int> values;
      std::ostringstream ss;
      int err = getOptionValues(ol, optName, 0, std::numeric_limits<int>::max(), values, ss);
      if(err == 0) {
        bool hasError = false;
        for(size_t i = 0; i < values.size(); ++i) {
          if(values[i] <= 0) {
            errmsg << "[element " << i << " of " << optName
              << " must be a non-zero positive integer]";
            ++error;
            hasError = true;
          }
        }
        if(!hasError) {
          timeSteps.swap(values);
        }
      } else {
        errmsg << ss.str();
	++error;
      }
    } else if(optName == "nodalTimeSteps") {
      std::vector<int> values;
      std::ostringstream ss;
      int err = getOptionValues(ol, optName, 0, std::numeric_limits<int>::max(), values, ss);
      if(err == 0) {
	bool hasError = false;
	for(size_t i = 0; i < values.size(); ++i) {
	  if(values[i] <= 0) {
	    errmsg << "[element " << i << " of " << optName
		   << " must be a non-zero positive integer]";
	    ++error;
	    hasError = true;
	  }
	}
	if(!hasError) {
	  nTimeSteps.swap(values);
	}
      } else {
	errmsg << ss.str();
	++error;
      }
    } else if(optName == "boundaryTimeSteps") {
      std::vector<int> values;
      std::ostringstream ss;
      int err = getOptionValues(ol, optName, 0, std::numeric_limits<int>::max(), values, ss);
      if(err == 0) {
	bool hasError = false;
	for(size_t i = 0; i < values.size(); ++i) {
	  if(values[i] <= 0) {
	    errmsg << "[element " << i << " of " << optName
		   << " must be a non-zero positive integer]";
	    ++error;
	    hasError = true;
	  }
	}
	if(!hasError) {
	  bTimeSteps.swap(values);
	}
      } else {
	errmsg << ss.str();
	++error;
      }
    } else {
      errmsg << "[unknown option " + optName + "]";
      ++error;
    }
  }
  
  if(nfreq.size() != ncnts.size()) {
    errmsg << "[nodalFrequencies and nodalCounts must have the same number of elements]";
    ++error;
  }
  
  if(bfreq.size() != bcnts.size()) {
    errmsg << "[boundaryFrequencies and boundaryCounts must have the same number of elements]";
    ++error;
  }
  
  if(freq.size() != cnts.size()) {
    errmsg << "[frequencies and counts must have the same number of elements]";
    ++error;
  }
  
  if(error) {
    err = errmsg.str();
  } else {
    if(bcnts.size() > 0) {
      boundaryFrequencies.swap(bfreq);
      boundaryCounts.swap(bcnts);
    } else {
      boundaryFrequencies = freq;
      boundaryCounts = cnts;
    }
    
    if(ncnts.size() > 0) {
      nodalFrequencies.swap(nfreq);
      nodalCounts.swap(ncnts);
    } else {
      nodalFrequencies = freq;
      nodalCounts = cnts;
    }

    if(bTimeSteps.size() > 0) {
      boundaryTimeSteps.swap(bTimeSteps);
    } else {
      boundaryTimeSteps = timeSteps;
    }
    
    if(nTimeSteps.size() > 0) {
      nodalTimeSteps.swap(nTimeSteps);
    } else {
      nodalTimeSteps = timeSteps;
    }
    
    nodalVariables.swap(nvars);
    boundaryVariables.swap(bvars);
  }
  
  return error;
}

std::ostream & operator<<(std::ostream & s, PlotSettings const & rhs) {
  int const nNodalLevels = rhs.nodalCounts.size();
  int const nNodalTimeSteps = rhs.nodalTimeSteps.size();
  int const nNodalVariables = rhs.nodalVariables.size();
  int const nBoundaryLevels = rhs.boundaryCounts.size();
  int const nBoundaryTimeSteps = rhs.boundaryTimeSteps.size();
  int const nBoundaryVariables = rhs.boundaryVariables.size();
  
  s << nNodalLevels << " ";
  for(int i = 0; i < nNodalLevels; ++i) {
    s << rhs.nodalFrequencies[i] << " " << rhs.nodalCounts[i] << " ";
  }
  
  s << nNodalTimeSteps << " ";
  for(int i = 0; i < nNodalTimeSteps; ++i) {
    s << rhs.nodalTimeSteps[i] << " ";
  }
  
  s << nNodalVariables << " ";
  for(int i = 0; i < nNodalVariables; ++i) {
    s << rhs.nodalVariables[i] << " ";
  }
  
  s << nBoundaryLevels << " ";
  for(int i = 0; i < nBoundaryLevels; ++i) {
    s << rhs.boundaryFrequencies[i] << " " << rhs.boundaryCounts[i] << " ";
  }

  s << nBoundaryTimeSteps << " ";
  for(int i = 0; i < nBoundaryTimeSteps; ++i) {
    s << rhs.boundaryTimeSteps[i] << " ";
  }
  
  s << nBoundaryVariables << " ";
  for(int i = 0; i < nBoundaryVariables; ++i) {
    s << rhs.boundaryVariables[i] << " ";
  }
  
  return s;
}

std::istream & operator>>(std::istream & s, PlotSettings & rhs) {
  int nNodalLevels, nNodalTimeSteps, nNodalVariables;
  int nBoundaryLevels, nBoundaryTimeSteps, nBoundaryVariables;
  
  s >> nNodalLevels;
  rhs.nodalFrequencies.resize(nNodalLevels);
  rhs.nodalCounts.resize(nNodalLevels);
  for(int i = 0; i < nNodalLevels; ++i) {
    s >> rhs.nodalFrequencies[i] >> rhs.nodalCounts[i];
  }

  s >> nNodalTimeSteps;
  rhs.nodalTimeSteps.resize(nNodalTimeSteps);
  for(int i = 0; i < nNodalTimeSteps; ++i) {
    s >> rhs.nodalTimeSteps[i];
  }
  
  s >> nNodalVariables;
  rhs.nodalVariables.resize(nNodalVariables);
  for(int i = 0; i < nNodalVariables; ++i) {
    s >> rhs.nodalVariables[i];
  }
  
  s >> nBoundaryLevels;
  rhs.boundaryFrequencies.resize(nBoundaryLevels);
  rhs.boundaryCounts.resize(nBoundaryLevels);
  for(int i = 0; i < nBoundaryLevels; ++i) {
    s >> rhs.boundaryFrequencies[i] >> rhs.boundaryCounts[i];
  }

  s >> nBoundaryTimeSteps;
  rhs.boundaryTimeSteps.resize(nBoundaryTimeSteps);
  for(int i = 0; i < nBoundaryTimeSteps; ++i) {
    s >> rhs.boundaryTimeSteps[i];
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
  //std::cout << "filename = " << filename << std::endl;
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
  //std::cout << "filename = " << filename << std::endl;
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
  //std::cout << "filename = " << filename << std::endl;
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

//class SurfaceBoundaryScalar : public Loci::pointwise_rule {
//  std::string var_name;
//  std::string value_name;
//  Loci::const_param<std::string> boundaryName;
//  Loci::const_store<double> var;
//  Loci::const_param<std::string> caseName;
//  Loci::const_param<std::string> plotPostfix;
//  Loci::param<bool> OUTPUT;
//  
//public:
//  SurfaceBoundaryScalar(char const * vname, char const * valname);
//  virtual void compute(Loci::sequence const & seq);
//};

BoundaryScalarOutput::BoundaryScalarOutput(char const * vname, char const * valname) {
  var_name = string(vname);
  value_name = string(valname);
  std::string constraintName = std::string("plotBoundary_") + value_name;
  
  name_store(var_name, var);
  name_store("plotBoundaryPostfix", plotPostfix);
  name_store("OUTPUT", OUTPUT);
  name_store("caseName", caseName);
  
  conditional("doBoundaryPlot");
  
  constraint("ci->vol");
  constraint(constraintName);
  
  input(var_name);
  input("caseName");
  input("plotBoundaryPostfix");
  
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
  name_store("plotBoundaryPostfix", plotPostfix);
  name_store("OUTPUT", OUTPUT);
  name_store("caseName", caseName);
  
  conditional("doBoundaryPlot");
  
  constraint("ci->vol");
  constraint(constraintName);
  
  input(var_name);
  input("caseName");
  input("plotBoundaryPostfix");
  
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
