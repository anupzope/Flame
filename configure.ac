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

dnl Detect and configure glog
CONFIGURE_GLOG
AS_IF([test "x$have_glog" != "xyes"],[AC_MSG_ERROR([glog is required])])

dnl Detect and configure libxml2
CONFIGURE_XML2

dnl Detect and configure MPI
CONFIGURE_MPI

dnl Detect and configure HDF5
dnl AX_LIB_HDF5([parallel])
dnl subsequent configures require use_hdf5
dnl AS_IF(
dnl   [test "x$with_hdf5" = "xyes"],
dnl   [use_hdf5="yes"],
dnl   [use_hdf5="no"]
dnl )
CONFIGURE_HDF5

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

AC_CONFIG_FILES([Makefile LFlame3/Makefile LFlame3-Tests/Makefile LFlame3-Tests/2D-Shock-Tube/Makefile])
AC_OUTPUT
