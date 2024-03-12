#ifndef LFLAME3_MIXTURE_HH
#define LFLAME3_MIXTURE_HH

#include <Loci.h>

#include <vector>
#include <string>

namespace flame {

// =============================================================================

struct ConstantViscosityModel {
  double viscosity;
};

struct SutherlandViscosityModel {
  double refTemperature, refViscosity, refConstant;
};

struct ConstantConductivityModel {
  double conductivity;
};

struct SutherlandConductivityModel {
  double refTemperature, refConductivity, refConstant;
};

struct CaloricallyPerfectThermochemistryModel {
  double specificHeat;
};

struct NASA9ThermochemistryModel {
  double tRange[3];
  double cpCoeff[18];
  double hCoeff[18];
  double sCoeff[18];
};

enum ViscosityModel {
  VISCOSITY_CONSTANT,
  VISCOSITY_SUTHERLAND,
  VISCOSITY_NONE
};

enum ConductivityModel {
  CONDUCTIVITY_CONSTANT,
  CONDUCTIVITY_SUTHERLAND,
  CONDUCTIVITY_NONE
};

enum ThermochemistryModel {
  THERMOCHEMISTRY_CALORICALLY_PERFECT,
  THERMOCHEMISTRY_NASA9,
  THERMOCHEMISTRY_NONE
};

struct Mixture {
  int nSpecies;
  char speciesName[FLAME_MAX_NSPECIES][FLAME_NAME_MAX_LENGTH];
  double molecularWeight[FLAME_MAX_NSPECIES];
  ViscosityModel viscosityModel[FLAME_MAX_NSPECIES];
  ConductivityModel conductivityModel[FLAME_MAX_NSPECIES];
  ThermochemistryModel thermochemistryModel[FLAME_MAX_NSPECIES];
  ConstantViscosityModel constantViscosity[FLAME_MAX_NSPECIES];
  SutherlandViscosityModel sutherlandViscosity[FLAME_MAX_NSPECIES];
  ConstantConductivityModel constantConductivity[FLAME_MAX_NSPECIES];
  SutherlandConductivityModel sutherlandConductivity[FLAME_MAX_NSPECIES];
  CaloricallyPerfectThermochemistryModel caloricallyPerfectThermochemistry[FLAME_MAX_NSPECIES];
  NASA9ThermochemistryModel nasa9Thermochemistry[FLAME_MAX_NSPECIES];

  void clear();
};

// =============================================================================

char const * getViscosityModelName(ViscosityModel m);

char const * getConductivityModelName(ConductivityModel m);

char const * getThermochemistryModelName(ThermochemistryModel m);

// =============================================================================

std::ostream & operator<<(std::ostream & s, Mixture const & mix);

int parseFromXML(
  std::string const & mixtureFile, Mixture & mixture, std::ostream & msg
);

// =============================================================================

} // end: namespace flame

namespace Loci {

template<>
struct data_schema_traits<flame::ConstantViscosityModel> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    CompoundDatatypeP cmpd = CompoundFactory(flame::ConstantViscosityModel());
    LOCI_INSERT_TYPE(cmpd, flame::ConstantViscosityModel, viscosity);
    return DatatypeP(cmpd);
  }
};

template<>
struct data_schema_traits<flame::SutherlandViscosityModel> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    CompoundDatatypeP cmpd = CompoundFactory(flame::SutherlandViscosityModel());
    LOCI_INSERT_TYPE(cmpd, flame::SutherlandViscosityModel, refTemperature);
    LOCI_INSERT_TYPE(cmpd, flame::SutherlandViscosityModel, refViscosity);
    LOCI_INSERT_TYPE(cmpd, flame::SutherlandViscosityModel, refConstant);
    return DatatypeP(cmpd);
  }
};
      
template<>
struct data_schema_traits<flame::ConstantConductivityModel> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    CompoundDatatypeP cmpd = CompoundFactory(flame::ConstantConductivityModel());
    LOCI_INSERT_TYPE(cmpd, flame::ConstantConductivityModel, conductivity);
    return DatatypeP(cmpd);
  }
};

template<>
struct data_schema_traits<flame::SutherlandConductivityModel> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    CompoundDatatypeP cmpd = CompoundFactory(flame::SutherlandConductivityModel());
    LOCI_INSERT_TYPE(cmpd, flame::SutherlandConductivityModel, refTemperature);
    LOCI_INSERT_TYPE(cmpd, flame::SutherlandConductivityModel, refConductivity);
    LOCI_INSERT_TYPE(cmpd, flame::SutherlandConductivityModel, refConstant);
    return DatatypeP(cmpd);
  }
};

template<>
struct data_schema_traits<flame::CaloricallyPerfectThermochemistryModel> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    CompoundDatatypeP cmpd = CompoundFactory(flame::CaloricallyPerfectThermochemistryModel());
    LOCI_INSERT_TYPE(cmpd, flame::CaloricallyPerfectThermochemistryModel, specificHeat);
    return DatatypeP(cmpd);
  }
};

template<>
struct data_schema_traits<flame::NASA9ThermochemistryModel> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    flame::NASA9ThermochemistryModel m;

    CompoundDatatypeP cmpd = CompoundFactory(flame::NASA9ThermochemistryModel());

    {
      int rank = 1;
      int dim[] = {3};
      int size = sizeof(double)*3;
      DatatypeP atom = getLociType(m.tRange[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "tRange",
        offsetof(flame::NASA9ThermochemistryModel, tRange),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {18};
      int size = sizeof(double)*18;
      DatatypeP atom = getLociType(m.cpCoeff[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "cpCoeff",
        offsetof(flame::NASA9ThermochemistryModel, cpCoeff),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {18};
      int size = sizeof(double)*18;
      DatatypeP atom = getLociType(m.hCoeff[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "hCoeff",
        offsetof(flame::NASA9ThermochemistryModel, hCoeff),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {18};
      int size = sizeof(double)*18;
      DatatypeP atom = getLociType(m.sCoeff[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "sCoeff",
        offsetof(flame::NASA9ThermochemistryModel, sCoeff),
        DatatypeP(array_t)
      );
    }

    return DatatypeP(cmpd);
  }
};

template<>
struct data_schema_traits<flame::ViscosityModel> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    int a;
    return getLociType(a);
  }
};

template<>
struct data_schema_traits<flame::ConductivityModel> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    int a;
    return getLociType(a);
  }
};

template<>
struct data_schema_traits<flame::ThermochemistryModel> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    int a;
    return getLociType(a);
  }
};

template<>
struct data_schema_traits<flame::Mixture> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    flame::Mixture m;
    
    CompoundDatatypeP cmpd = CompoundFactory(flame::Mixture());
    LOCI_INSERT_TYPE(cmpd, flame::Mixture, nSpecies);

    {
      int rank = 2;
      int dim[] = {FLAME_MAX_NSPECIES, FLAME_NAME_MAX_LENGTH};
      int size = sizeof(char)*FLAME_MAX_NSPECIES*FLAME_NAME_MAX_LENGTH;
      DatatypeP atom = getLociType(m.speciesName[0][0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "speciesName",
        offsetof(flame::Mixture, speciesName),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(double)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.molecularWeight[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "molecularWeight",
        offsetof(flame::Mixture, molecularWeight),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(int)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.viscosityModel[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "viscosityModel",
        offsetof(flame::Mixture, viscosityModel),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(int)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.conductivityModel[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "conductivityModel",
        offsetof(flame::Mixture, conductivityModel),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(int)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.thermochemistryModel[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "thermochemistryModel",
        offsetof(flame::Mixture, thermochemistryModel),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(flame::ConstantViscosityModel)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.constantViscosity[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "constantViscosity",
        offsetof(flame::Mixture, constantViscosity),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(flame::SutherlandViscosityModel)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.sutherlandViscosity[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "sutherlandViscosity",
        offsetof(flame::Mixture, sutherlandViscosity),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(flame::ConstantConductivityModel)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.constantConductivity[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "constantConductivity",
        offsetof(flame::Mixture, constantConductivity),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(flame::SutherlandConductivityModel)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.sutherlandConductivity[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "sutherlandConductivity",
        offsetof(flame::Mixture, sutherlandConductivity),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(flame::CaloricallyPerfectThermochemistryModel)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.caloricallyPerfectThermochemistry[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "caloricallyPerfectThermochemistryModel",
        offsetof(flame::Mixture, caloricallyPerfectThermochemistry),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(flame::NASA9ThermochemistryModel)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.nasa9Thermochemistry[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "nasa9ThermochemistryModel",
        offsetof(flame::Mixture, nasa9Thermochemistry),
        DatatypeP(array_t)
      );
    }

    return DatatypeP(cmpd);
  }
};

} // end: namespace Loci

#endif // #ifndef LFLAME3_MIXTURE_HH
