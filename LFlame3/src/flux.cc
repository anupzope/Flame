#include <flux.hh>

namespace flame {

void AUSMPlusUpFluxIdealGas(
  Loci::Array<double, 5> & flux,
  Loci::vector3d<double> const & Ul, double const Pgl, double const Tl,
  Loci::vector3d<double> const & Ur, double const Pgr, double const Tr,
  double const area_sada, Loci::vector3d<double> const & area_n, double const Pambient,
  double const Rtilde, double const Cp, double const Minf
) {
  double const Pl = Pgl+Pambient;
  double const Pr = Pgr+Pambient;
  
  double const rl = Pl/(Rtilde*Tl);
  double const rr = Pr/(Rtilde*Tr);
  
  double const hl = Cp*Tl;
  double const hr = Cp*Tr;
  
  double const Cv = Cp-Rtilde;
  double const gamma = Cp/Cv;
  
  double const gm1 = gamma-1.0;
  double const gp1 = gamma+1.0;
  
  double const Ulmag2 = dot(Ul, Ul);
  double const Urmag2 = dot(Ur, Ur);
  
  double const Unl = dot(Ul, area_n);
  double const Unr = dot(Ur, area_n);
  
  //double const h0l = Rtilde*Tl*gamma/gm1+0.5*Ulmag2;
  //double const h0r = Rtilde*Tr*gamma/gm1+0.5*Urmag2;
  double const h0l = hl+0.5*Ulmag2;
  double const h0r = hr+0.5*Urmag2;
  
  //double const e0l = h0l - Rtilde*Tl;
  //double const e0r = h0r - Rtilde*Tr;
  
  double const clstar2 = 2.0*gm1/gp1*h0l;
  double const crstar2 = 2.0*gm1/gp1*h0r;
  
  double const clstar = sqrt(clstar2);
  double const crstar = sqrt(crstar2);
  
  // This defnition leads to entropy violation.
  // See paper "A sequel to AUSM, Part II", Liou, 2006
  //real const cltilde = clstar2/max(fabs(Unl), clstar);
  //real const crtilde = crstar2/max(fabs(Unr), crstar);
  
  // This definition is the entropy fix, according to the paper.
  double const cltilde = Unl > clstar ? clstar2/Unl : clstar2/clstar; // clstar2/max(Unl, clstar);
  double const crtilde = (-Unr) > crstar ? crstar2/(-Unr) : crstar2/crstar; // crstar2/max(-Unr, crstar);
  
  double const chalf = cltilde < crtilde ? cltilde : crtilde; // min(cltilde, crtilde);
  
  double const Minf2 = Minf*Minf;
  double const Mavg2 = 0.5*(Unl*Unl+Unr*Unr)/(chalf*chalf);
  double const M02 = Mavg2 > 1.0 ? 1.0 : (Minf2 > 1.0 ? 1.0 : Mavg2 > Minf2 ? Mavg2 : Minf2); // min(1.0, max(Mavg2, Minf*Minf));
  double const M0 = sqrt(M02);
  double const fa = M0*(2.0-M0);
  
  // some constants
  double const Kp = 0.25;
  double const Ku = 0.75;
  double const sigma = 1.0;
  double const beta = 1.0/8.0;
  //double const alpha = 3.0/16.0;
  double const alpha = 3.0/16.0*(-4.0+5.0*fa*fa);
  
  double const rhalf = 0.5*(rl+rr);
  
  double const Ml = Unl/chalf;
  double const Mr = Unr/chalf;
  
  double Mlp, Plp;
  double Pp, Pm;
  if(Ml < -1.0) {
    Mlp = 0.0;
    Pp = 0.0;
    Plp = 0.0;
  } else if(Ml <= 1.0) {
    double const tmp1 = Ml+1.0, tmp2 = Ml*Ml-1.0;
    double const tmp12 = tmp1*tmp1, tmp22 = tmp2*tmp2;
    Mlp = 0.25*tmp12+beta*tmp22;
    Pp = (0.25*tmp12*(2.0-Ml)+alpha*Ml*tmp22);
    Plp = Pl*Pp;
  } else {
    Mlp = Ml;
    Pp = 1.0;
    Plp = Pl;
  }
  
  double Mrm, Prm;
  if(Mr < -1.0) {
    Mrm = Mr;
    Pm = 1.0;
    Prm = Pr;
  } else if(Mr <= 1.0) {
    double const tmp1 = Mr-1.0, tmp2 = Mr*Mr-1.0;
    double const tmp12 = tmp1*tmp1, tmp22 = tmp2*tmp2;
    Mrm = -0.25*tmp12-beta*tmp22;
    Pm = (0.25*tmp12*(2.0+Mr)-alpha*Mr*tmp22);
    Prm = Pr*Pm;
  } else {
    Mrm = 0.0;
    Pm = 0.0;
    Prm = 0.0;
  }
  
  //T const Mp = -Kp/fa*max(1.0-sigma*Mavg2, 0.0)*(Pr-Pl)/(rhalf*chalf*chalf);
  double const Fa = 1.0-sigma*Mavg2;
  double const Mp = -Kp/fa*(Fa > 0.0 ? Fa : 0.0)*(Pr-Pl)/(rhalf*chalf*chalf);
  double const Pu = -Ku*Pp*Pm*(rl+rr)*fa*chalf*(Unr-Unl);
  
  double const Mhalf = Mlp+Mrm+Mp;
  double const Phalf = Plp+Prm+Pu;
  
  if(Mhalf >= 0.0) {
    double const ut = Mhalf*chalf; //-us_n;
    double const mdot = area_sada*rl*ut;
    double pg = Phalf-Pambient;
    
    flux[0] = mdot*Ul.x+area_sada*pg*area_n.x;
    flux[1] = mdot*Ul.y+area_sada*pg*area_n.y;
    flux[2] = mdot*Ul.z+area_sada*pg*area_n.z;
    //flux[3] = mdot*e0l+area_sada*Pl*Unl;
    flux[3] = mdot*h0l;
    flux[4] = mdot;
  } else {
    double const ut = Mhalf*chalf; //-us_n;
    double const mdot = area_sada*rr*ut;
    double pg = Phalf-Pambient;
    
    flux[0] = mdot*Ur.x+area_sada*pg*area_n.x;
    flux[1] = mdot*Ur.y+area_sada*pg*area_n.y;
    flux[2] = mdot*Ur.z+area_sada*pg*area_n.z;
    //flux[3] = mdot*e0r+area_sada*Pr*Unr;
    flux[3] = mdot*h0r;
    flux[4] = mdot;
  }
}

void AUSMPlusUpFluxMultiSpeciesIdealGas(
  double * flux,
  int const Ns,
  double const * Yl, Loci::vector3d<double> const & Ul, double const Pgl, double const Tl,
  double const Rtildel, double const Cpl,
  double const * Yr, Loci::vector3d<double> const & Ur, double const Pgr, double const Tr,
  double const Rtilder, double const Cpr,
  double const area_sada, Loci::vector3d<double> const & area_n, double const Pambient,
  double const Minf
) {
  double const Pl = Pgl+Pambient;
  double const Pr = Pgr+Pambient;
  
  double const rl = Pl/(Rtildel*Tl);
  double const rr = Pr/(Rtilder*Tr);
  
  double const hl = Cpl*Tl;
  double const hr = Cpr*Tr;
  
  double const Cvl = Cpl-Rtildel;
  double const Cvr = Cpr-Rtilder;
  
  double const gammal = Cpl/Cvl;
  double const gammar = Cpr/Cvr;
  
  double const gm1l = gammal-1.0;
  double const gm1r = gammar-1.0;
  
  double const gp1l = gammal+1.0;
  double const gp1r = gammar+1.0;
  
  double const Ulmag2 = dot(Ul, Ul);
  double const Urmag2 = dot(Ur, Ur);
  
  double const Unl = dot(Ul, area_n);
  double const Unr = dot(Ur, area_n);
  
  //double const h0l = Rtildel*Tl*gammal/gm1l+0.5*Ulmag2;
  //double const h0r = Rtilder*Tr*gammar/gm1r+0.5*Urmag2;
  double const h0l = hl+0.5*Ulmag2;
  double const h0r = hr+0.5*Urmag2;
  
  //double const e0l = h0l - Rtildel*Tl;
  //double const e0r = h0r - Rtilder*Tr;
  
  double const clstar2 = 2.0*gm1l/gp1l*h0l;
  double const crstar2 = 2.0*gm1r/gp1r*h0r;
  
  double const clstar = sqrt(clstar2);
  double const crstar = sqrt(crstar2);
  
  // This defnition leads to entropy violation.
  // See paper "A sequel to AUSM, Part II", Liou, 2006
  //real const cltilde = clstar2/max(fabs(Unl), clstar);
  //real const crtilde = crstar2/max(fabs(Unr), crstar);
  
  // This definition is the entropy fix, according to the paper.
  double const cltilde = Unl > clstar ? clstar2/Unl : clstar2/clstar; // clstar2/max(Unl, clstar);
  double const crtilde = (-Unr) > crstar ? crstar2/(-Unr) : crstar2/crstar; // crstar2/max(-Unr, crstar);
  
  double const chalf = cltilde < crtilde ? cltilde : crtilde; // min(cltilde, crtilde);
  
  double const Minf2 = Minf*Minf;
  double const Mavg2 = 0.5*(Unl*Unl+Unr*Unr)/(chalf*chalf);
  double const M02 = Mavg2 > 1.0 ? 1.0 : (Minf2 > 1.0 ? 1.0 : Mavg2 > Minf2 ? Mavg2 : Minf2); // min(1.0, max(Mavg2, Minf*Minf));
  double const M0 = sqrt(M02);
  double const fa = M0*(2.0-M0);
  
  // some constants
  double const Kp = 0.25;
  double const Ku = 0.75;
  double const sigma = 1.0;
  double const beta = 1.0/8.0;
  //double const alpha = 3.0/16.0;
  double const alpha = 3.0/16.0*(-4.0+5.0*fa*fa);
  
  double const rhalf = 0.5*(rl+rr);
  
  double const Ml = Unl/chalf;
  double const Mr = Unr/chalf;
  
  double Mlp, Plp;
  double Pp, Pm;
  if(Ml < -1.0) {
    Mlp = 0.0;
    Pp = 0.0;
    Plp = 0.0;
  } else if(Ml <= 1.0) {
    double const tmp1 = Ml+1.0, tmp2 = Ml*Ml-1.0;
    double const tmp12 = tmp1*tmp1, tmp22 = tmp2*tmp2;
    Mlp = 0.25*tmp12+beta*tmp22;
    Pp = (0.25*tmp12*(2.0-Ml)+alpha*Ml*tmp22);
    Plp = Pl*Pp;
  } else {
    Mlp = Ml;
    Pp = 1.0;
    Plp = Pl;
  }
  
  double Mrm, Prm;
  if(Mr < -1.0) {
    Mrm = Mr;
    Pm = 1.0;
    Prm = Pr;
  } else if(Mr <= 1.0) {
    double const tmp1 = Mr-1.0, tmp2 = Mr*Mr-1.0;
    double const tmp12 = tmp1*tmp1, tmp22 = tmp2*tmp2;
    Mrm = -0.25*tmp12-beta*tmp22;
    Pm = (0.25*tmp12*(2.0+Mr)-alpha*Mr*tmp22);
    Prm = Pr*Pm;
  } else {
    Mrm = 0.0;
    Pm = 0.0;
    Prm = 0.0;
  }
  
  //T const Mp = -Kp/fa*max(1.0-sigma*Mavg2, 0.0)*(Pr-Pl)/(rhalf*chalf*chalf);
  double const Fa = 1.0-sigma*Mavg2;
  double const Mp = -Kp/fa*(Fa > 0.0 ? Fa : 0.0)*(Pr-Pl)/(rhalf*chalf*chalf);
  double const Pu = -Ku*Pp*Pm*(rl+rr)*fa*chalf*(Unr-Unl);
  
  double const Mhalf = Mlp+Mrm+Mp;
  double const Phalf = Plp+Prm+Pu;
  
  if(Mhalf >= 0.0) {
    double const ut = Mhalf*chalf; //-us_n;
    double const mdot = area_sada*rl*ut;
    double pg = Phalf-Pambient;
    
    flux[0] = mdot*Ul.x+area_sada*pg*area_n.x;
    flux[1] = mdot*Ul.y+area_sada*pg*area_n.y;
    flux[2] = mdot*Ul.z+area_sada*pg*area_n.z;
    //flux[3] = mdot*e0l+area_sada*Pl*Unl;
    flux[3] = mdot*h0l;
    flux[4] = mdot;
    
    for(int i = 0; i < Ns-1; ++i) {
      flux[5+i] = mdot*Yl[i];
    }
  } else {
    double const ut = Mhalf*chalf; //-us_n;
    double const mdot = area_sada*rr*ut;
    double pg = Phalf-Pambient;
    
    flux[0] = mdot*Ur.x+area_sada*pg*area_n.x;
    flux[1] = mdot*Ur.y+area_sada*pg*area_n.y;
    flux[2] = mdot*Ur.z+area_sada*pg*area_n.z;
    //flux[3] = mdot*e0r+area_sada*Pr*Unr;
    flux[3] = mdot*h0r;
    flux[4] = mdot;
    
    for(int i = 0; i < Ns-1; ++i) {
      flux[5+i] = mdot*Yr[i];
    }
  }
}

} // end: namespace flame
