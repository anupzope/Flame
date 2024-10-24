#ifndef LFLAME3_EOS_HH
#define LFLAME3_EOS_HH

namespace flame {

inline
double mixture_mW_from_X_sW(
  int const Ns, double const * X, double const * speciesW
) {
  double mixtureW = 0.0;
  for(int i = 0; i < Ns; ++i) {
    mixtureW += X[i]*speciesW[i];
  }
  return mixtureW;
}

inline
void mixture_Y_from_X_sW_mW(
  int const Ns, double * Y, double const * X,
  double const * speciesW, double const mixtureW
) {
  for(int i = 0; i < Ns; ++i) {
    Y[i] = X[i]*speciesW[i]/mixtureW;
  }
}

inline
double mixture_mW_from_Y_sW(
  int const Ns, double const * Y, double const * speciesW
) {
  double mixtureWinv = 0.0;
  for(int i = 0; i < Ns; ++i) {
    mixtureWinv += Y[i]/speciesW[i];
  }
  return 1.0/mixtureWinv;
}

inline
void mixture_X_from_Y_sW_mW(
  int const Ns, double * X, double const * Y,
  double const * speciesW, double const mixtureW
) {
  for(int i = 0; i < Ns; ++i) {
    X[i] = Y[i]*mixtureW/speciesW[i];
  }
}

inline
double mixture_Cp_from_sCp_Y(int const Ns, double const * sCp, double const * Y) {
  double mixtureCp = 0.0;
  for(int i = 0; i < Ns; ++i) {
    mixtureCp += sCp[i]*Y[i];
  }
  return mixtureCp;
}

inline
double mixture_H_from_sH_Y(int const Ns, double const * sH, double const * Y) {
  double mixtureH = 0.0;
  for(int i = 0; i < Ns; ++i) {
    mixtureH += sH[i]*Y[i];
  }
  return mixtureH;
}

inline
double eos_TP_a_from_Cp_R_T(double const Cp, double const R, double const T) {
  return sqrt((Cp/(Cp-R))*R*T);
}

inline
double eos_TP_a_from_gamma_R_T(double const gamma, double const R, double const T) {
  return sqrt(gamma*R*T);
}

inline
double eos_TP_a_from_Cp_R_r_P(double const Cp, double const R, double const r, double const P) {
  return sqrt(Cp/(Cp-R)*P/r);
}

inline
double eos_TP_a_from_gamma_r_P(double const gamma, double const r, double const P) {
  return sqrt(gamma*P/r);
}

inline
double eos_TP_Cv_from_Cp_R(double const Cp, double const R) {
  return Cp-R;
}

inline
double eos_TP_Cv_from_gamma_R(double const gamma, double const R) {
  return R/(gamma-1);
}

inline
double eos_TP_Cv_from_gamma_Cp(double const gamma, double const Cp) {
  return Cp/gamma;
}

inline
double eos_TP_gamma_from_Cp_R(double const Cp, double const R) {
  return Cp/(Cp-R);
}

inline
double eos_TP_gamma_from_Cp_Cv(double const Cp, double const Cv) {
  return Cp/Cv;
}

inline
double eos_TP_gamma_from_Cv_R(double const Cv, double const R) {
  return (Cv+R)/Cv;
}

inline
double eos_TP_P_from_r_T_R(double const r, double const T, double const R) {
  return r*R*T;
}

inline
double eos_TP_r_from_P_T_R(double const P, double const T, double const R) {
  return P/(R*T);
}

inline
double eos_TP_T_from_r_P_R(double const r, double const P, double const R) {
  return P/(r*R);
}

} // end: namespace flame

#endif // #ifndef LFLAME3_EOS_HH
