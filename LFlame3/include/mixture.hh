#ifndef LFLAME3_MIXTURE_HH
#define LFLAME3_MIXTURE_HH

#include <Loci.h>

#include <vector>
#include <string>

namespace flame {

// =============================================================================

struct ConstantViscosity {
  double value;
};

struct SutherlandViscosity {
  double refTemperature, refViscosity, refConstant;
};

struct ConstantConductivity {
  double value;
};

struct SutherlandConductivity {
  double refTemperature, refConductivity, refConstant;
};

struct ConstantDiffusivity {
  double value;
};

struct SchmidtNumber {
  double value;
};

struct CaloricallyPerfectThermochemistry {
  double specificHeat;
};

struct NASA9Thermochemistry {
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

enum DiffusivityModel {
  DIFFUSIVITY_CONSTANT,
  DIFFUSIVITY_SCHMIDT,
  DIFFUSIVITY_NONE
};

enum ThermochemistryModel {
  THERMOCHEMISTRY_CALORICALLY_PERFECT,
  THERMOCHEMISTRY_NASA9,
  THERMOCHEMISTRY_NONE
};

struct Mixture {
  int nSpecies;

  char speciesName[FLAME_MAX_NSPECIES][FLAME_NAME_MAX_LENGTH];
  int hasSpeciesName[FLAME_MAX_NSPECIES];

  double molecularWeight[FLAME_MAX_NSPECIES];
  int hasMolecularWeight[FLAME_MAX_NSPECIES];

  // Viscosity parameters
  ViscosityModel viscosityModel[FLAME_MAX_NSPECIES];
  int hasViscosityModel[FLAME_MAX_NSPECIES];

  ConstantViscosity constantViscosity[FLAME_MAX_NSPECIES];
  int hasConstantViscosity[FLAME_MAX_NSPECIES];

  SutherlandViscosity sutherlandViscosity[FLAME_MAX_NSPECIES];
  int hasSutherlandViscosity[FLAME_MAX_NSPECIES];

  // Conductivity parameters
  ConductivityModel conductivityModel[FLAME_MAX_NSPECIES];
  int hasConductivityModel[FLAME_MAX_NSPECIES];

  ConstantConductivity constantConductivity[FLAME_MAX_NSPECIES];
  int hasConstantConductivity[FLAME_MAX_NSPECIES];

  SutherlandConductivity sutherlandConductivity[FLAME_MAX_NSPECIES];
  int hasSutherlandConductivity[FLAME_MAX_NSPECIES];

  // Species diffusivity parameters
  DiffusivityModel diffusivityModel[FLAME_MAX_NSPECIES];
  int hasDiffusivityModel[FLAME_MAX_NSPECIES];

  ConstantDiffusivity constantDiffusivity[FLAME_MAX_NSPECIES];
  int hasConstantDiffusivity[FLAME_MAX_NSPECIES];

  SchmidtNumber schmidtNumber[FLAME_MAX_NSPECIES];
  int hasSchmidtNumber[FLAME_MAX_NSPECIES];

  // Thermochemistry parameters
  ThermochemistryModel thermochemistryModel[FLAME_MAX_NSPECIES];
  int hasThermochemistryModel[FLAME_MAX_NSPECIES];

  CaloricallyPerfectThermochemistry caloricallyPerfectThermochemistry[FLAME_MAX_NSPECIES];
  int hasCaloricallyPerfectThermochemistry[FLAME_MAX_NSPECIES];

  NASA9Thermochemistry nasa9Thermochemistry[FLAME_MAX_NSPECIES];
  int hasNasa9Thermochemistry[FLAME_MAX_NSPECIES];

  void clear();
  void clearSpecies(int idx);
};

// =============================================================================

char const * getViscosityModelName(ViscosityModel m);

char const * getConductivityModelName(ConductivityModel m);

char const * getDiffusivityModelName(DiffusivityModel m);

char const * getThermochemistryModelName(ThermochemistryModel m);

// =============================================================================

std::ostream & operator<<(std::ostream & s, Mixture const & mix);

int parseFromXML(
  std::string const & mixtureFile, Mixture & mixture, std::ostream & msg,
  std::string const & xsdDir = FLAME_DATA_DIR
);

// =============================================================================

} // end: namespace flame

namespace Loci {

template<>
struct data_schema_traits<flame::ConstantViscosity> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    CompoundDatatypeP cmpd = CompoundFactory(flame::ConstantViscosity());
    LOCI_INSERT_TYPE(cmpd, flame::ConstantViscosity, value);
    return DatatypeP(cmpd);
  }
};

template<>
struct data_schema_traits<flame::SutherlandViscosity> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    CompoundDatatypeP cmpd = CompoundFactory(flame::SutherlandViscosity());
    LOCI_INSERT_TYPE(cmpd, flame::SutherlandViscosity, refTemperature);
    LOCI_INSERT_TYPE(cmpd, flame::SutherlandViscosity, refViscosity);
    LOCI_INSERT_TYPE(cmpd, flame::SutherlandViscosity, refConstant);
    return DatatypeP(cmpd);
  }
};

template<>
struct data_schema_traits<flame::ConstantConductivity> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    CompoundDatatypeP cmpd = CompoundFactory(flame::ConstantConductivity());
    LOCI_INSERT_TYPE(cmpd, flame::ConstantConductivity, value);
    return DatatypeP(cmpd);
  }
};

template<>
struct data_schema_traits<flame::SutherlandConductivity> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    CompoundDatatypeP cmpd = CompoundFactory(flame::SutherlandConductivity());
    LOCI_INSERT_TYPE(cmpd, flame::SutherlandConductivity, refTemperature);
    LOCI_INSERT_TYPE(cmpd, flame::SutherlandConductivity, refConductivity);
    LOCI_INSERT_TYPE(cmpd, flame::SutherlandConductivity, refConstant);
    return DatatypeP(cmpd);
  }
};

template<>
struct data_schema_traits<flame::ConstantDiffusivity> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    CompoundDatatypeP cmpd = CompoundFactory(flame::ConstantDiffusivity());
    LOCI_INSERT_TYPE(cmpd, flame::ConstantDiffusivity, value);
    return DatatypeP(cmpd);
  }
};

template<>
struct data_schema_traits<flame::SchmidtNumber> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    CompoundDatatypeP cmpd = CompoundFactory(flame::SchmidtNumber());
    LOCI_INSERT_TYPE(cmpd, flame::SchmidtNumber, value);
    return DatatypeP(cmpd);
  }
};

template<>
struct data_schema_traits<flame::CaloricallyPerfectThermochemistry> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    CompoundDatatypeP cmpd = CompoundFactory(flame::CaloricallyPerfectThermochemistry());
    LOCI_INSERT_TYPE(cmpd, flame::CaloricallyPerfectThermochemistry, specificHeat);
    return DatatypeP(cmpd);
  }
};

template<>
struct data_schema_traits<flame::NASA9Thermochemistry> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    flame::NASA9Thermochemistry m;

    CompoundDatatypeP cmpd = CompoundFactory(flame::NASA9Thermochemistry());

    {
      int rank = 1;
      int dim[] = {3};
      int size = sizeof(double)*3;
      DatatypeP atom = getLociType(m.tRange[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "tRange",
        offsetof(flame::NASA9Thermochemistry, tRange),
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
        offsetof(flame::NASA9Thermochemistry, cpCoeff),
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
        offsetof(flame::NASA9Thermochemistry, hCoeff),
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
        offsetof(flame::NASA9Thermochemistry, sCoeff),
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
struct data_schema_traits<flame::DiffusivityModel> {
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
      int size = sizeof(int)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.hasSpeciesName[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "hasSpeciesName",
        offsetof(flame::Mixture, hasSpeciesName),
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
      DatatypeP atom = getLociType(m.hasMolecularWeight[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "hasMolecularWeight",
        offsetof(flame::Mixture, hasMolecularWeight),
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
      DatatypeP atom = getLociType(m.hasViscosityModel[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "hasViscosityModel",
        offsetof(flame::Mixture, hasViscosityModel),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(flame::ConstantViscosity)*FLAME_MAX_NSPECIES;
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
      int size = sizeof(int)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.hasConstantViscosity[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "hasConstantViscosity",
        offsetof(flame::Mixture, hasConstantViscosity),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(flame::SutherlandViscosity)*FLAME_MAX_NSPECIES;
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
      int size = sizeof(int)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.hasSutherlandViscosity[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "hasSutherlandViscosity",
        offsetof(flame::Mixture, hasSutherlandViscosity),
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
      DatatypeP atom = getLociType(m.hasConductivityModel[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "hasConductivityModel",
        offsetof(flame::Mixture, hasConductivityModel),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(flame::ConstantConductivity)*FLAME_MAX_NSPECIES;
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
      int size = sizeof(int)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.hasConstantConductivity[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "hasConstantConductivity",
        offsetof(flame::Mixture, hasConstantConductivity),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(flame::SutherlandConductivity)*FLAME_MAX_NSPECIES;
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
      int size = sizeof(int)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.hasSutherlandConductivity[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "hasSutherlandConductivity",
        offsetof(flame::Mixture, hasSutherlandConductivity),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(int)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.diffusivityModel[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "diffusivityModel",
        offsetof(flame::Mixture, diffusivityModel),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(int)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.hasDiffusivityModel[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "hasDiffusivityModel",
        offsetof(flame::Mixture, hasDiffusivityModel),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(flame::ConstantDiffusivity)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.constantDiffusivity[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "constantDiffusivity",
        offsetof(flame::Mixture, constantDiffusivity),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(int)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.hasConstantDiffusivity[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "hasConstantDiffusivity",
        offsetof(flame::Mixture, hasConstantDiffusivity),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(flame::SchmidtNumber)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.schmidtNumber[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "schmidtNumber",
        offsetof(flame::Mixture, schmidtNumber),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(int)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.hasSchmidtNumber[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "hasSchmidtNumber",
        offsetof(flame::Mixture, hasSchmidtNumber),
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
      int size = sizeof(int)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.hasThermochemistryModel[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "hasThermochemistryModel",
        offsetof(flame::Mixture, hasThermochemistryModel),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(flame::CaloricallyPerfectThermochemistry)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.caloricallyPerfectThermochemistry[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "caloricallyPerfectThermochemistry",
        offsetof(flame::Mixture, caloricallyPerfectThermochemistry),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(int)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.hasCaloricallyPerfectThermochemistry[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "hasCaloricallyPerfectThermochemistry",
        offsetof(flame::Mixture, hasCaloricallyPerfectThermochemistry),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(flame::NASA9Thermochemistry)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.nasa9Thermochemistry[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "nasa9Thermochemistry",
        offsetof(flame::Mixture, nasa9Thermochemistry),
        DatatypeP(array_t)
      );
    }

    {
      int rank = 1;
      int dim[] = {FLAME_MAX_NSPECIES};
      int size = sizeof(int)*FLAME_MAX_NSPECIES;
      DatatypeP atom = getLociType(m.hasNasa9Thermochemistry[0]);
      ArrayDatatypeP array_t = ArrayFactory(atom, size, rank, dim);
      cmpd->insert(
        "hasNasa9Thermochemistry",
        offsetof(flame::Mixture, hasNasa9Thermochemistry),
        DatatypeP(array_t)
      );
    }

    return DatatypeP(cmpd);
  }
};

} // end: namespace Loci

#endif // #ifndef LFLAME3_MIXTURE_HH
