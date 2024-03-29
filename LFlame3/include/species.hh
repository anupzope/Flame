#ifndef FLAME_LFLAME_SPECIES_HH
#define FLAME_LFLAME_SPECIES_HH

#include <Loci.h>

#include <istream>
#include <ostream>

namespace flame {

struct SpeciesProperties {
  double molecularWeight;
  double specificHeat_Constant;
  std::vector<std::array<double, 7> > specificHeat_NASA7;
  std::vector<std::pair<double, double> > specificHeat_NASA7TRange;
  double viscosity_Constant;
  std::array<double, 3> viscosity_SutherlandParameters;
  double conductivity_Constant;
  std::array<double, 3> conductivity_SutherlandParameters;
};

std::ostream & operator<<(std::ostream & s, SpeciesProperties const & obj);
std::istream & operator>>(std::istream & s, SpeciesProperties & obj);

std::ostream & serialize(std::ostream & s, SpeciesProperties const & obj);
std::istream & deserialize(std::istream & s, SpeciesProperties & obj);

} // end: namespace flame

namespace Loci {

template<>
struct data_schema_traits<flame::SpeciesProperties> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  
  static DatatypeP get_type() {
    CompoundDatatypeP cmpd = CompoundFactory(flame::SpeciesProperties());
    
    LOCI_INSERT_TYPE(cmpd, flame::SpeciesProperties, molecularWeight);
    LOCI_INSERT_TYPE(cmpd, flame::SpeciesProperties, specificHeat_Constant);
    LOCI_INSERT_TYPE(cmpd, flame::SpeciesProperties, specificHeat_NASA7);
    LOCI_INSERT_TYPE(cmpd, flame::SpeciesProperties, constantViscosity);
    
    {
      flame::SpeciesProperties obj;
      int rank = 1;
      int dim[] = {3};
      int sz = 3*sizeof(double);
      DatatypeP atom = getLociType(obj.sutherlandViscosityParameters[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, sz, rank, dim);
      cmpd->insert(
        "sutherlandViscosityParameters",
        offsetof(flame::SpeciesProperties, sutherlandViscosityParameters),
        DatatypeP(array_t)
      );
    }
    
    LOCI_INSERT_TYPE(cmpd, flame::SpeciesProperties, constantConductivity);
    
    {
      flame::SpeciesProperties obj;
      int rank = 1;
      int dim[] = {3};
      int sz = 3*sizeof(double);
      DatatypeP atom = getLociType(obj.sutherlandConductivityParameters[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, sz, rank, dim);
      cmpd->insert(
        "sutherlandConductivityParameters",
        offsetof(flame::SpeciesProperties, sutherlandConductivityParameters),
        DatatypeP(array_t)
      );
    }
    
    return DatatypeP(cmpd);
  }
};

} // end: namespace Loci

namespace flame {

//struct SimpleIdealGas {
//  char name[64];
//  double gamma; // specific gas constant
//  double W; // molecular weight
//};
//
//struct Species {
//  
//};

//std::ostream & serialize(std::ostream & s, SimpleIdealGas const & obj);
//std::istream & deserialize(std::istream & s, SimpleIdealGas & obj);

//std::ostream & operator<<(std::ostream & s, SimpleIdealGas const & rhs);
//std::istream & operator>>(std::istream & s, SimpleIdealGas & rhs);

} // end: namespace flame

namespace Loci {

//template<>
//struct data_schema_traits<flame::SimpleIdealGas> {
//  typedef IDENTITY_CONVERTER Schema_Converter;
//  static DatatypeP get_type() {
//    CompoundDatatypeP cmpd = CompoundFactory(flame::SimpleIdealGas());
//    
//    {
//      flame::SimpleIdealGas obj;
//      int rank = 1;
//      int dim[] = {64};
//      int sz = 64*sizeof(char);
//      DatatypeP atom = getLociType(obj.name[0]);
//      ArrayDatatypeP array_t = ArrayFactory(atom, sz, rank, dim);
//      cmpd->insert("name", offsetof(flame::SimpleIdealGas, name), DatatypeP(array_t));
//    }
//    
//    LOCI_INSERT_TYPE(cmpd, flame::SimpleIdealGas, gamma);
//    LOCI_INSERT_TYPE(cmpd, flame::SimpleIdealGas, W);
//    
//    return DatatypeP(cmpd);
//  }
//};

//class SimpleIdealGasSchemaConverter {
//  flame::SimpleIdealGas & ref;
//  std::string fmt;
//  
//public:
//  explicit SimpleIdealGasSchemaConverter(flame::SimpleIdealGas & ref) : ref(ref) {
//    std::ostringstream ss(fmt);
//    serialize(ss, ref);
//  }
//  
//  int getSize() const {
//    return fmt.size();
//  }
//  
//  void getState(char * buf, int & size) {
//    size = fmt.size();
//    for(int i = 0; i < size; ++i) {
//      buf[i] = fmt[i];
//    }
//  }
//  
//  void setState(char * buf, int size) {
//    fmt = std::string(buf);
//    std::istringstream ss;
//    deserialize(ss, ref);
//  }
//};

} // end: namespace Loci

#endif // #ifndef FLAME_LFLAME_SPECIES_HH
