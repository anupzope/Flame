dnl Specify prerequisite
AC_PREREQ([2.69])

dnl Initialize autoconf
AC_INIT([Flame], [0.1], [anupzope@gmail.com])

dnl Set macro directory
AC_CONFIG_MACRO_DIR([acm4])

dnl Set auxiliary directory
AC_CONFIG_AUX_DIR([build-aux])

dnl Initialize automake
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

dnl Detect C++ compiler
AC_PROG_CXX

dnl Ensure that compiler supports c++17
AX_CXX_COMPILE_STDCXX(17, [noext], [mandatory])

dnl Initialize libtool
LT_INIT

dnl Detect and configure gtest
CONFIGURE_GTEST

dnl Detect and configure libxml2
CONFIGURE_XML2

dnl Detect and configure MPI
CONFIGURE_MPI_CXX

dnl Detect and configure HDF5
AX_LIB_HDF5([parallel])
dnl subsequent configures require use_hdf5
AS_IF(
  [test "x$with_hdf5" = "xyes"],
  [use_hdf5="yes"],
  [use_hdf5="no"]
)

dnl Detect and configure METIS
CONFIGURE_METIS

dnl Detect and configure ParMETIS
CONFIGURE_PARMETIS

dnl Detect and configure CGNS
CONFIGURE_CGNS

dnl Detect and configure PETSc
CONFIGURE_PETSC

dnl Detect and configure Loci
CONFIGURE_LOCI

AC_CONFIG_FILES([Makefile LFlame3/Makefile])
AC_OUTPUT
