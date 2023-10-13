#ifndef FLAME_FLAME_HH
#define FLAME_FLAME_HH

#include <Loci.h>

#include <ostream>
#include <istream>

namespace flame {

// =============================================================================

struct SymmetricTensor {
  double xx, xy, xz, yy, yz, zz;
} ;

inline std::ostream & operator<<(std::ostream & s, SymmetricTensor const & ss) {
  s << ss.xx << ' ' << ss.xy << ' ' << ss.xz << ' ' <<
    ss.yy << ' ' << ss.yz << ' ' << ss.zz << std::endl;
  return s;
}

inline std::istream & operator>>(std::istream & s, SymmetricTensor & ss) {
  s >> ss.xx >> ss.xy >> ss.xz >> ss.yy >> ss.yz >> ss.zz;
  return s;
}

struct SkewSymmetricTensor {
  double xy, xz, yz;
};

inline std::ostream & operator<<(std::ostream & s, SkewSymmetricTensor const & ss) {
  s << ss.xy << ' ' << ss.xz << ' ' << ss.yz << std::endl;
  return s;
}

inline std::istream & operator>>(std::istream & s, SkewSymmetricTensor & ss) {
  s >> ss.xy >> ss.xz >> ss.yz;
  return s;
}

// =============================================================================

} // end: namespace flame

namespace Loci {

// =============================================================================

template<> struct data_schema_traits<flame::SymmetricTensor> {
  typedef IDENTITY_CONVERTER Schema_Converter ;
  static DatatypeP get_type() {
    CompoundDatatypeP ct = CompoundFactory(flame::SymmetricTensor());
    LOCI_INSERT_TYPE(ct, flame::SymmetricTensor, xx);
    LOCI_INSERT_TYPE(ct, flame::SymmetricTensor, xy);
    LOCI_INSERT_TYPE(ct, flame::SymmetricTensor, xz);
    LOCI_INSERT_TYPE(ct, flame::SymmetricTensor, yy);
    LOCI_INSERT_TYPE(ct, flame::SymmetricTensor, yz);
    LOCI_INSERT_TYPE(ct, flame::SymmetricTensor, zz);
    return DatatypeP(ct);
  }
};

template<> struct data_schema_traits<flame::SkewSymmetricTensor> {
  typedef IDENTITY_CONVERTER Schema_Converter;
  static DatatypeP get_type() {
    CompoundDatatypeP ct = CompoundFactory(flame::SkewSymmetricTensor());
    LOCI_INSERT_TYPE(ct, flame::SkewSymmetricTensor, xy);
    LOCI_INSERT_TYPE(ct, flame::SkewSymmetricTensor, xz);
    LOCI_INSERT_TYPE(ct, flame::SkewSymmetricTensor, yz);
    return DatatypeP(ct);
  }
};

// =============================================================================

} // end: namespace Loci

#endif // #ifndef FLAME_FLAME_HH
