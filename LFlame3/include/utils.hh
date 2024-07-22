#ifndef FLAME_LFLAME_UTILS_HH
#define FLAME_LFLAME_UTILS_HH

#include <Loci.h>

#include <vector>
#include <string>

namespace flame {

std::string nameOptionType(Loci::option_value_type type);

int getArgValue(
  Loci::option_values const & arg,
  std::string & value,
  std::ostream & errmsg
);

int getArgValue(
  Loci::option_values const & arg,
  std::string const & unit, double & value,
  std::ostream & errmsg
);

int getArgValues(
  Loci::option_values const & arg,
  int const minValues, int const maxValues,
  std::vector<std::string> & values,
  std::ostream & errmsg
);

int getArgValues(
  Loci::option_values const & arg,
  int const minValues, int const maxValues,
  std::vector<int> & values,
  std::ostream & errmsg
);

int getArgValues(
  Loci::option_values const & arg,
  std::vector<std::string> const & units,
  int const minValues, int const maxValues,
  std::vector<double> & values,
  std::ostream & errmsg
);

int getOptionValue(
  options_list const & ol, std::string const & optName,
  std::string const & unit, double & value,
  std::ostream & errmsg
);

int getOptionValues(
  options_list const & ol, std::string const & optName,
  int const minValues, int const maxValues,
  std::vector<std::string> & values,
  std::ostream & errmsg
);

int getOptionValues(
  options_list const & ol, std::string const & optName,
  int const minValues, int const maxValues,
  std::vector<int> & values,
  std::ostream & errmsg
);

int getOptionValues(
  options_list const & ol, std::string const & optName,
  std::vector<std::string> const & units,
  int const minValues, int const maxValues,
  std::vector<double> & values,
  std::ostream & errmsg
);

enum ThermoSpecType {
  THERMO_SPEC_PT, // pressure-temperature
  THERMO_SPEC_DT, // density-temperature
  THERMO_SPEC_DP  // density-pressure
};

std::ostream & operator<<(std::ostream & s, ThermoSpecType const & obj);
std::istream & operator>>(std::istream & s, ThermoSpecType & obj);

struct ThermoSpec {
  ThermoSpecType type;
  double pressure;
  double temperature;
  double density;
};

std::ostream & operator<<(std::ostream & s, ThermoSpec const & obj);
std::istream & operator>>(std::istream & s, ThermoSpec & obj);

int getThermoSpec(
  options_list::arg_list const & args,
  std::string const & olName,
  std::string const & pressureOptName,
  std::string const & temperatureOptName,
  std::string const & densityOptName,
  ThermoSpec & spec,
  std::ostream & errmsg
);

int getThermoSpec(
  options_list const & ol,
  std::string const & olName,
  std::string const & pressureOptName,
  std::string const & temperatureOptName,
  std::string const & densityOptName,
  ThermoSpec & spec,
  std::ostream & errmsg
);

enum VelSpecType {
  VEL_SPEC_VELOCITY, // Velocity
  VEL_SPEC_MACH,     // Mach number
};

std::ostream & operator<<(std::ostream & s, VelSpecType const & obj);
std::istream & operator>>(std::istream & s, VelSpecType & obj);

struct VelSpec {
  VelSpecType type;
  Loci::vector3d<double> direction;
  double scale;
};

std::ostream & operator<<(std::ostream & s, VelSpec const & obj);
std::istream & operator>>(std::istream & s, VelSpec & obj);

int getVelSpec(
  options_list::arg_list const & args,
  std::string const & olName,
  std::string const & velocityOptName,
  std::string const & MachOptName,
  VelSpec & spec,
  std::ostream & errmsg
);

int getVelSpec(
  options_list const & ol,
  std::string const & olName,
  std::string const & velocityOptName,
  std::string const & MachOptName,
  VelSpec & spec,
  std::ostream & errmsg
);

enum SpeciesSpecType {
  SPECIES_SPEC_X, // molar fractions
  SPECIES_SPEC_Y  // mass fractions
};

std::ostream & operator<<(std::ostream & s, SpeciesSpecType const & obj);
std::istream & operator>>(std::istream & s, SpeciesSpecType & obj);

struct UnityFractions {
  std::vector<double> data;
};

std::ostream & operator<<(std::ostream & s, UnityFractions const & obj);
std::istream & operator>>(std::istream & s, UnityFractions & obj);

struct SpeciesSpec {
  SpeciesSpecType type;
  UnityFractions fractions;
};

std::ostream & operator<<(std::ostream & s, SpeciesSpec const & obj);
std::istream & operator>>(std::istream & s, SpeciesSpec & obj);

class UnityFractionsConverter {
  UnityFractions & rObj;
  
public:
  explicit UnityFractionsConverter(UnityFractions & obj) : rObj(obj) {
  }
  
  int getSize() {
    return rObj.data.size();
  }
  
  void getState(double * buf, int & size) {
    size = getSize();
    for(int i = 0; i < size; ++i) {
      buf[i] = rObj.data[i];
    }
  }
  
  void setState(double * buf, int size) {
    rObj.data.resize(size);
    for(int i = 0; i < size; ++i) {
      rObj.data[i] = buf[i];
    }
  }
};

class SpeciesSpecConverter {
  SpeciesSpec & rObj;
  
public:
  explicit SpeciesSpecConverter(SpeciesSpec & obj) : rObj(obj) {
  }
  
  int getSize() {
    int count = rObj.fractions.data.size();
    return count + 1;
  }
  
  void getState(double * buf, int & size) {
    size = getSize();
    
    buf[0] = (double)(rObj.type);
    for(int i = 0; i < size-1; ++i) {
      buf[i+1] = rObj.fractions.data[i];
    }
  }
  
  void setState(double * buf, int size) {
    rObj.type = (SpeciesSpecType)buf[0];
    rObj.fractions.data.resize(size-1);
    for(int i = 0; i < size-1; ++i) {
      rObj.fractions.data[i] = buf[i+1];
    }
  }
};

class SpeciesSpecVectorConverter {
  std::vector<SpeciesSpec> & rObj;

public:
  explicit SpeciesSpecVectorConverter(std::vector<SpeciesSpec> & obj) : rObj(obj) {
  }

  int getSize() {
    int nobj = rObj.size();
    int count = 0;
    for(int i = 0; i < nobj; ++i) {
      count += rObj[i].fractions.data.size() + 2;
    }
    return count + 1;
  }

  void getState(double * buf, int & size) {
    size = getSize();

    int nobj = rObj.size();
    buf[0] = (double)nobj;
    for(int i = 0, k = 1; i < nobj; ++i) {
      buf[k++] = (double)(rObj[i].type);
      int nfrac = rObj[i].fractions.data.size();
      buf[k++] = (double)nfrac;
      for(int j = 0; j < nfrac; ++j) {
        buf[k++] = rObj[i].fractions.data[j];
      }
    }
  }

  void setState(double * buf, int size) {
    int nobj = (int)buf[0];
    rObj.resize(nobj);
    for(int i = 0, k = 1; i < nobj; ++i) {
      rObj[i].type = (SpeciesSpecType)(buf[k++]);
      int nfrac = (int)(buf[k++]);
      rObj[i].fractions.data.resize(nfrac);
      for(int j = 0; j < nfrac; ++j) {
        rObj[i].fractions.data[j] = buf[k++];
      }
    }
  }
};

int getSpeciesSpec(
  options_list::arg_list const & args,
  std::string const & olName,
  std::string const & molarOptName,
  std::string const & massOptName,
  int const count,
  SpeciesSpec & spec,
  std::ostream & errmsg
);

int getSpeciesSpec(
  options_list const & ol,
  std::string const & olName,
  std::string const & molarOptName,
  std::string const & massOptName,
  int const count,
  SpeciesSpec & spec,
  std::ostream & errmsg
);

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
);

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
);

enum GeomType {
  GEOM_LEFT_PLANE,
  GEOM_RIGHT_PLANE,
  GEOM_SPHERE,
  GEOM_CONVEX_POLYHEDRON,
  GEOM_BOX
};

class ICRegionGeom : public Loci::CPTR_type {
public:
  virtual GeomType type() const = 0;

  virtual bool in(double x, double y, double z) const = 0;
};

int getRegionGeom(
  std::string const & functionName, Loci::options_list::arg_list const & args,
  ICRegionGeom ** geom, std::ostream & errmsg
);

int getRegionGeom(
  Loci::options_list const & ol, std::string const & optName,
  ICRegionGeom ** geom, std::ostream & errmsg
);

class SphereICRegionGeom : public ICRegionGeom {
  double cx, cy, cz, r, r2;

public:
  void setCenter(double cx, double cy, double cz) {
    this->cx = cx;
    this->cy = cy;
    this->cz = cz;
  }

  void setRadius(double r) {
    this->r = r;
    this->r2 = r*r;
  }

  GeomType type() const override;

  bool in(double x, double y, double z) const override;
};

class ConvexPolyhedronICRegionGeom : public ICRegionGeom {
  std::vector<double> nx, ny, nz;
  std::vector<double> px, py, pz;

public:
  void addPlane(
    double px, double py, double pz, double nx, double ny, double nz
  ) {
    this->px.push_back(px);
    this->py.push_back(py);
    this->pz.push_back(pz);
    this->nx.push_back(nx);
    this->ny.push_back(ny);
    this->nz.push_back(nz);
  }

  GeomType type() const override;

  bool in(double x, double y, double z) const override;
};

class BoxICRegionGeom : public ICRegionGeom {
  double px1, py1, pz1, px2, py2, pz2;

public:
  void setCorners(
    double px1, double py1, double pz1, double px2, double py2, double pz2
  ) {
    this->px1 = (px1 < px2 ? px1 : px2);
    this->py1 = (py1 < py2 ? py1 : py2);
    this->pz1 = (pz1 < pz2 ? pz1 : pz2);
    this->px2 = (px1 < px2 ? px2 : px1);
    this->py2 = (py1 < py2 ? py2 : py1);
    this->pz2 = (pz1 < pz2 ? pz2 : pz1);
  }

  GeomType type() const override;

  bool in(double x, double y, double z) const override;
};
  
class LeftPlaneICRegionGeom : public ICRegionGeom {
  double nx, ny, nz;
  double px, py, pz;

public:
  void setNormal(double nx, double ny, double nz) {
    this->nx = nx;
    this->ny = ny;
    this->nz = nz;
  }

  void setPoint(double px, double py, double pz) {
    this->px = px;
    this->py = py;
    this->pz = pz;
  }

  GeomType type() const override;

  bool in(double x, double y, double z) const override;
};

class RightPlaneICRegionGeom : public ICRegionGeom {
  double nx, ny, nz;
  double px, py, pz;

public:
  void setNormal(double nx, double ny, double nz) {
    this->nx = nx;
    this->ny = ny;
    this->nz = nz;
  }

  void setPoint(double px, double py, double pz) {
    this->px = px;
    this->py = py;
    this->pz = pz;
  }

  GeomType type() const override;

  bool in(double x, double y, double z) const override;
};

} // end: namespace flame

namespace Loci {
  
template<> struct data_schema_traits<flame::ThermoSpecType> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  
  static DatatypeP get_type() {
    int value;
    return getLociType(value);
  }
};

template<> struct data_schema_traits<flame::VelSpecType> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  
  static DatatypeP get_type() {
    int value;
    return getLociType(value);
  }
};

template<> struct data_schema_traits<flame::SpeciesSpecType> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  
  static DatatypeP get_type() {
    int value;
    return getLociType(value);
  }
};

template<> struct data_schema_traits<flame::UnityFractions> {
  typedef USER_DEFINED_CONVERTER Schema_Converter;
  typedef double Converter_Base_Type;
  typedef flame::UnityFractionsConverter ConverterType;
};

template<> struct data_schema_traits<flame::ThermoSpec> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  
  static DatatypeP get_type() {
    flame::ThermoSpec obj = flame::ThermoSpec();
    CompoundDatatypeP ct = CompoundFactory(obj);
    LOCI_INSERT_TYPE(ct, flame::ThermoSpec, type);
    LOCI_INSERT_TYPE(ct, flame::ThermoSpec, pressure);
    LOCI_INSERT_TYPE(ct, flame::ThermoSpec, temperature);
    LOCI_INSERT_TYPE(ct, flame::ThermoSpec, density);
    return DatatypeP(ct);
  }
};

template<> struct data_schema_traits<flame::VelSpec> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  
  static DatatypeP get_type() {
    CompoundDatatypeP ct = CompoundFactory(flame::VelSpec());
    LOCI_INSERT_TYPE(ct, flame::VelSpec, type);
    LOCI_INSERT_TYPE(ct, flame::VelSpec, direction);
    LOCI_INSERT_TYPE(ct, flame::VelSpec, scale);
    return DatatypeP(ct);
  }
};

template<> struct data_schema_traits<flame::SpeciesSpec> {
  typedef USER_DEFINED_CONVERTER Schema_Converter;
  typedef double Converter_Base_Type;
  typedef flame::SpeciesSpecConverter Converter_Type;
};

template<> struct data_schema_traits<std::vector<flame::SpeciesSpec> > {
  typedef USER_DEFINED_CONVERTER Schema_Converter;
  typedef double Converter_Base_Type;
  typedef flame::SpeciesSpecVectorConverter Converter_Type;
};

class STDStringVectorSchemaConverter {
  std::vector<std::string> & ref;
  std::string str;
  
public:
  explicit STDStringVectorSchemaConverter(std::vector<std::string> & ref);
  
  int getSize() const { return str.size()+1; }
  
  void getState(char * buf, int & size) {
    size = str.size()+1;
    for(int i = 0; i < size-1; ++i) {
      buf[i] = str[i];
    }
    buf[size] = '\0';
  }
  
  void setState(char * buf, int size);
};

template<> struct data_schema_traits<std::vector<std::string> > {
  typedef USER_DEFINED_CONVERTER  Schema_Converter;
  typedef char Converter_Base_Type;
  typedef STDStringVectorSchemaConverter Converter_Type;
};

} // end: namespace Loci

#endif // end: #ifndef FLAME_LFLAME_UTILS_HH
