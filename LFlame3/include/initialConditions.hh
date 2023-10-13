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
    return DatatypeP(ct);
  }
};

// =============================================================================

} // end: namespace Loci

#endif // #ifndef FLAME_INITIAL_CONDITIONS_HH
