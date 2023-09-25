#ifndef FLAME_FLUX_HH
#define FLAME_FLUX_HH

#include <Loci.h>

namespace flame {

void AUSMPlusUpFluxIdealGas(
  Loci::Array<double, 5> & flux,
  Loci::vector3d<double> const & Ul, double const Pgl, double const Tl,
  Loci::vector3d<double> const & Ur, double const Pgr, double const Tr,
  double const area_sada, Loci::vector3d<double> const & area_n, double const Pambient,
  double const Rtilde, double const gamma, double const Minf
);

void AUSMPlusUpFluxMultiSpeciesIdealGas(
  double * flux,
  int const Ns,
  double const * Yl, Loci::vector3d<double> const & Ul, double const Pgl, double const Tl,
  double const Rtildel, double const gammal,
  double const * Yr, Loci::vector3d<double> const & Ur, double const Pgr, double const Tr,
  double const Rtilder, double const gammar,
  double const area_sada, Loci::vector3d<double> const & area_n, double const Pambient,
  double const Minf
);

}

#endif // end: #ifndef FLAME_FLUX_HH
