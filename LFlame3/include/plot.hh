#ifndef FLAME_PLOT_HH
#define FLAME_PLOT_HH

#include <utils.hh>

#include <string>
#include <vector>

#include <Loci.h>

namespace flame {

// =============================================================================

struct PrintSettings {
  std::vector<std::string> parameters;
  int frequency;
  std::string filename;
  
  std::string toString() const;
  void fromString(std::string const & str);
  int fromOptionsList(options_list const & ol, std::string & err);
};

std::ostream & operator<<(std::ostream & s, PrintSettings const & rhs);
std::istream & operator>>(std::istream & s, PrintSettings & rhs);

// =============================================================================

struct PlotSettings {
  std::vector<int> frequencies;
  std::vector<int> counts;
  std::vector<std::string> nodalVariables;
  std::vector<std::string> boundaryVariables;
  
  std::string toString() const;
  void fromString(std::string const & str);
  int fromOptionsList(options_list const & ol, std::string & err);
};

std::ostream & operator<<(std::ostream & s, PlotSettings const & rhs);
std::istream & operator>>(std::istream & s, PlotSettings & rhs);

// =============================================================================

} // end: namespace flame

namespace Loci {

// =============================================================================

class PrintSettingsSchemaConverter {
  flame::PrintSettings & ref;
  std::string fmt;
  
public:
  explicit PrintSettingsSchemaConverter(flame::PrintSettings & ref) : ref(ref) {
    fmt = ref.toString();
  }
  
  int getSize() const {
    return fmt.size();
  }
  
  void getState(char * buf, int & size) {
    size = getSize();
    for(int i = 0; i < size; ++i) {
      buf[i] = fmt[i];
    }
  }
  
  void setState(char * buf, int size) {
    ref.fromString(std::string(buf));
  }
};

template<>
struct data_schema_traits<flame::PrintSettings> {
  typedef USER_DEFINED_CONVERTER Schema_Converter;
  typedef char Converter_Base_Type;
  typedef PrintSettingsSchemaConverter Converter_Type;
};

// =============================================================================

class PlotSettingsSchemaConverter {
  flame::PlotSettings & ref;
  std::string fmt;
  
public:
  explicit PlotSettingsSchemaConverter(flame::PlotSettings & iref) : ref(iref) {
    fmt = ref.toString();
  }
  
  int getSize() const {
    return fmt.size();
  }
  
  void getState(char * buf, int & size) {
    size = getSize();
    for(int i = 0; i < size; ++i) {
      buf[i] = fmt[i];
    }
  }
  
  void setState(char * buf, int size) {
    ref.fromString(std::string(buf));
  }
};

template<>
struct data_schema_traits<flame::PlotSettings> {
  typedef USER_DEFINED_CONVERTER Schema_Converter;
  typedef char Converter_Base_Type;
  typedef PlotSettingsSchemaConverter Converter_Type;
};

// =============================================================================

} // end: namespace Loci

namespace flame {

// =============================================================================

struct PrintParameterFile {
  std::ofstream * file;
  PrintParameterFile() : file(nullptr) { }
  ~PrintParameterFile() {
    if(file) {
      file->close();
    }
  }
};

extern PrintParameterFile printParameterFile;

// =============================================================================

class PrintParameterDB {
  std::vector<std::string> name;
  std::vector<double> value;
  bool printHeader;
  
public:
  PrintParameterDB();
  void clear();
  void add(int const root, std::string const & n, double const v);
  std::ostream & print(
    int const timeStep, double const stime, std::ostream & s
  );
};

extern PrintParameterDB printParameterDB;

// =============================================================================

void dumpScalar(
  sequence const & seq,
  const_store<float> & c2n,
  std::string const & caseName,
  std::string const & plotPostfix,
  std::string const & type,
  std::string const & sname
);

class NodalScalarOutput : public Loci::pointwise_rule {
  std::string var_name;
  std::string value_name;
  Loci::const_store<float> c2n;
  Loci::const_param<std::string> plotPostfix;
  Loci::const_param<std::string> caseName;
  Loci::param<bool> OUTPUT;
  
public:
  NodalScalarOutput(char const * vname, char const * valname);
  virtual void compute(Loci::sequence const & seq);
};

// =============================================================================

class NodalVecComponentsOutput : public Loci::pointwise_rule {
  std::string var_name;
  std::string value_name;
  std::string component_names;
  Loci::const_storeVec<float> c2n;
  Loci::const_param<std::vector<std::string> > componentNames;
  Loci::const_param<std::string> plotPostfix;
  Loci::const_param<std::string> caseName;
  Loci::param<bool> OUTPUT;
  
public:
  NodalVecComponentsOutput(
    char const * vname, char const * valname, char const * cnames,
    char const * extraConstraints
  );
  virtual void compute(Loci::sequence const & seq);
};

// =============================================================================

void dumpVector(
  sequence const & seq,
  const_store<Loci::vector3d<float> > & c2n,
  std::string const & caseName,
  std::string const & plotPostfix,
  std::string const & type,
  std::string const & sname
);

class NodalVectorOutput : public Loci::pointwise_rule {
  std::string var_name;
  std::string value_name;
  Loci::const_store<Loci::vector3d<float> > c2n;
  Loci::const_param<std::string> plotPostfix;
  Loci::const_param<std::string> caseName;
  Loci::param<bool> OUTPUT;
  
public:
  NodalVectorOutput(char const * vname, char const * valname);
  virtual void compute(Loci::sequence const & seq);
};

void dumpVar(
  sequence const & seq,
  Loci::storeRepP var,
  std::string const & caseName,
  std::string const & plotPostfix,
  std::string const & type,
  std::string const & sname
);

class ParamOutput : public Loci::singleton_rule {
  std::string var_name;
  std::string value_name;
  Loci::const_param<double> param;
  Loci::const_param<std::string> caseName;
  Loci::const_param<std::string> plotPostfix;
  Loci::param<bool> OUTPUT;
  
public:
  ParamOutput(char const * vname, char const * valname);
  virtual void compute(Loci::sequence const & seq);
};

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
//
//class SurfaceBoundaryVector : public Loci::pointwise_rule {
//  std::string var_name;
//  std::string value_name;
//  Loci::const_param<std::string> boundaryName;
//  Loci::const_store<Loci::vector3d<double> > var;
//  Loci::const_param<std::string> caseName;
//  Loci::const_param<std::string> plotPostfix;
//  Loci::param<bool> OUTPUT;
//  
//public:
//  SurfaceBoundaryVector(char const * vname, char const * valname);
//  virtual void compute(Loci::sequence const & seq);
//};

class BoundaryScalarOutput : public Loci::pointwise_rule {
  std::string var_name;
  std::string value_name;
  Loci::const_store<double> var;
  Loci::const_param<string> caseName;
  Loci::const_param<string> plotPostfix;
  Loci::param<bool> OUTPUT;
  
public:
  BoundaryScalarOutput(char const * vname, char const * valname);
  virtual void compute(Loci::sequence const & seq);
};

class BoundaryVectorOutput : public Loci::pointwise_rule {
  std::string var_name;
  std::string value_name;
  Loci::const_store<Loci::vector3d<double> > var;
  Loci::const_param<string> caseName;
  Loci::const_param<string> plotPostfix;
  Loci::param<bool> OUTPUT;
  
public:
  BoundaryVectorOutput(char const * vname, char const * valname);
  virtual void compute(Loci::sequence const & seq);
};

} // end: namespace flame

#define OUTPUT_SCALAR(X,Y) class OUT_##Y : public flame::NodalScalarOutput {\
public:\
  OUT_##Y() : flame::NodalScalarOutput(X,#Y){}\
};\
register_rule<OUT_##Y> register_OUT_##Y;

#define OUTPUT_VEC_COMPONENTS(X,Y,Z,C) class OUT_##Y : public flame::NodalVecComponentsOutput {\
public:\
  OUT_##Y() : flame::NodalVecComponentsOutput(X,#Y,Z,C){}\
};\
register_rule<OUT_##Y> register_OUT_##Y;

#define OUTPUT_VECTOR(X,Y) class OUT_##Y : public flame::NodalVectorOutput {\
public:\
  OUT_##Y() : flame::NodalVectorOutput(X,#Y){}\
};\
register_rule<OUT_##Y> register_OUT_##Y;

#define OUTPUT_PARAM(X,Y) class OUT_##Y : public flame::ParamOutput {\
public:\
  OUT_##Y() : flame::ParamOutput(X,#Y){}\
};\
register_rule<OUT_##Y> register_OUT_##Y;

#define OUTPUT_BOUNDARY_SCALAR(X,Y,Z) class OUTB_##Y : public flame::BoundaryScalarOutput {\
public:\
  OUTB_##Y() : flame::BoundaryScalarOutput(X,#Y){\
    constraint(Z);\
    constraint(X);\
  }\
};\
register_rule<OUTB_##Y> register_OUTB_##Y;

//class OUTBS_##Y : public SurfaceBoundaryScalar {\
//public:\
//  OUTBS_##Y() : flame::SurfaceBoundaryScalar(X,#Y) {\
//    constraint(Z);\
//    constraint(X);\
//  }\
//};\
//register_rule<OUTBS_##Y> register_OUTBS_##Y;

#define OUTPUT_BOUNDARY_VECTOR(X,Y,Z) class OUTB_##Y : public flame::BoundaryVectorOutput {\
public:\
  OUTB_##Y() : flame::BoundaryVectorOutput(X,#Y){\
    constraint(Z);\
    constraint(X);\
  }\
};\
register_rule<OUTB_##Y> register_OUTB_##Y;

//class OUTBS_##Y : public flame::SurfaceBoundaryVector {\
//public:
//  OUTBS_##Y() : SurfaceBoundaryVector(X,#Y) {\
//    constraint(Z);\
//    constraint(X);\
//  }\
//};\
//register_rule<OUTBS_##Y> register_OUTBS_##Y;

#endif // #ifndef FLAME_PLOT_HH
