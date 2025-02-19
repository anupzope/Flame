#ifndef FLAME_INITIAL_CONDITIONS_HH
#define FLAME_INITIAL_CONDITIONS_HH

#include <Loci.h>

namespace flame {

// =============================================================================

struct TGVICParameters {
  Loci::vector3d<double> Xref;
  Loci::vector3d<double> Lref;
  Loci::vector3d<double> Kref;
  double Pref;
  double Tref;
  double rhoref;
  double Uref;
  double Mref;
  int rhorefSpecified;
  int UrefSpecified;
};

struct IsentropicVortexICParameters {
  Loci::vector3d<double> origin;
  Loci::vector3d<double> offsets;
  double scale;
  double strength;
  double spread;
  int thermoSpec;
  double temperature;
  double pressure;
  double density;
  int velSpec;
  Loci::vector3d<double> Mach;
  Loci::vector3d<double> velocity;
};

// =============================================================================

} // end: namespace flame

namespace Loci {

// =============================================================================

template<> struct data_schema_traits<flame::TGVICParameters> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  
  static DatatypeP get_type() {
    CompoundDatatypeP ct = CompoundFactory(flame::TGVICParameters());
    LOCI_INSERT_TYPE(ct, flame::TGVICParameters, Xref);
    LOCI_INSERT_TYPE(ct, flame::TGVICParameters, Lref);
    LOCI_INSERT_TYPE(ct, flame::TGVICParameters, Kref);
    LOCI_INSERT_TYPE(ct, flame::TGVICParameters, Pref);
    LOCI_INSERT_TYPE(ct, flame::TGVICParameters, Tref);
    LOCI_INSERT_TYPE(ct, flame::TGVICParameters, rhoref);
    LOCI_INSERT_TYPE(ct, flame::TGVICParameters, Uref);
    LOCI_INSERT_TYPE(ct, flame::TGVICParameters, Mref);
    LOCI_INSERT_TYPE(ct, flame::TGVICParameters, rhorefSpecified);
    LOCI_INSERT_TYPE(ct, flame::TGVICParameters, UrefSpecified);
    return DatatypeP(ct);
  }
};

template<> struct data_schema_traits<flame::IsentropicVortexICParameters> {
  typedef IDENTITY_CONVERTER Schema_Converter;

  static DatatypeP get_type() {
    CompoundDatatypeP ct = CompoundFactory(flame::IsentropicVortexICParameters());
    LOCI_INSERT_TYPE(ct, flame::IsentropicVortexICParameters, origin);
    LOCI_INSERT_TYPE(ct, flame::IsentropicVortexICParameters, offsets);
    LOCI_INSERT_TYPE(ct, flame::IsentropicVortexICParameters, scale);
    LOCI_INSERT_TYPE(ct, flame::IsentropicVortexICParameters, strength);
    LOCI_INSERT_TYPE(ct, flame::IsentropicVortexICParameters, spread);
    LOCI_INSERT_TYPE(ct, flame::IsentropicVortexICParameters, thermoSpec);
    LOCI_INSERT_TYPE(ct, flame::IsentropicVortexICParameters, temperature);
    LOCI_INSERT_TYPE(ct, flame::IsentropicVortexICParameters, pressure);
    LOCI_INSERT_TYPE(ct, flame::IsentropicVortexICParameters, density);
    LOCI_INSERT_TYPE(ct, flame::IsentropicVortexICParameters, velSpec);
    LOCI_INSERT_TYPE(ct, flame::IsentropicVortexICParameters, Mach);
    LOCI_INSERT_TYPE(ct, flame::IsentropicVortexICParameters, velocity);
    return DatatypeP(ct);
  }
};

// =============================================================================

} // end: namespace Loci

#endif // #ifndef FLAME_INITIAL_CONDITIONS_HH
