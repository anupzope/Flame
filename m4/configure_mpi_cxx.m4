# Provides configuration option: --with-mpi=no/yes/auto/path-to-mpicc.
# 
# --with-mpi=no: MPI is not configured
# 
# --with-mpi=yes: Tries to find MPI based on location of mpicc in
# $PATH. Configuation fails if MPI can not be found.
# 
# --with-mpi=auto: Tries to find MPI based on location of mpicc in
# $PATH. Provides warning if MPI can not be found, and continues with the
# remaining configuration
# 
# --with-mpi=path-to-mpicc: Tries to find MPI based path-to-mpicc. Configuation
# --fails if MPI can not be found.
# 
# --with-mpi: same as --with-mpi=auto
# 
# Upon success, this script
# 1. defines HAVE_MPI preprocessor symbol
# 2. defines HAVE_MPI conditional for Makefile.am
# 3. defines use_mpi=yes/no for configure.ac usage
# 4. defines MPI_CXXFLAGS for configure.ac and Makefile.am
# 5. defines MPI_LIBS for configure.ac and Makefile.am

AC_DEFUN([CONFIGURE_MPI_CXX], [
AC_PREREQ(2.50)

AC_ARG_VAR([MPI_LIBS],[MPI libraries])
AC_ARG_VAR([MPI_LDFLAGS],[MPI link flags])
AC_ARG_VAR([MPI_CFLAGS],[MPI C compilation flags])
AC_ARG_VAR([MPI_CXXFLAGS],[MPI C++ compilation flags])
AC_ARG_VAR([MPI_CPPFLAGS],[MPI preprocessor flags])

AC_ARG_WITH(
mpi,
[
  AS_HELP_STRING(
    [--with-mpi],
    [compile with MPI support. If none is found, MPI is not used. Default: auto]
  )
],
[],
[with_mpi=auto]
)

AS_CASE($with_mpi,
  [no], [
    _configure_mpi_cxx_req=no
    _configure_mpi_cxx_path=no
  ],
  [yes], [
    _configure_mpi_cxx_req=yes
    _configure_mpi_cxx_path=auto
  ],
  [auto],[
    _configure_mpi_cxx_req=auto
    _configure_mpi_cxx_path=auto
  ],
  [/*], [
    _configure_mpi_cxx_req=yes
    AS_IF(
      [test -z "$with_mpi"],
      [_configure_mpi_cxx_path=auto],
      [_configure_mpi_cxx_path=$with_mpi]
    )
  ],
  [
    _configure_mpi_cxx_req=no
    _configure_mpi_cxx_path=no
  ]
)

_configure_mpi_cxx_library_found=no
_configure_mpi_cxx_header_found=no

AS_IF(
  [test "x$_configure_mpi_cxx_req" = "xyes" || test "x$_configure_mpi_cxx_req" = "xauto"],
  [
    dnl detect path of the mpi compiler
    AS_IF(
      [test "x$_configure_mpi_cxx_path" = "xauto"],
      [AC_PATH_PROGS(
        [_configure_mpi_cxx_full_path],
        [mpic++ mpicxx mpicc],
        [notfound]
      )],
      [AC_PATH_PROGS(
        [_configure_mpi_cxx_full_path],
        [mpic++ mpicxx mpicc],
        [notfound],
        [$_configure_mpi_cxx_path]
      )]
    )
    
    AS_IF(
      [test "x$_configure_mpi_cxx_full_path" = "xnotfound"],
      [
        AC_MSG_WARN([MPI installation not found])
      ],
      [
        _configure_mpi_cxx_path=`AS_DIRNAME(["$_configure_mpi_cxx_full_path"])`
        _configure_mpi_cxx_path=`AS_DIRNAME(["$_configure_mpi_cxx_path"])`
        
        AC_MSG_NOTICE(Checking MPI at $_configure_mpi_cxx_path)
        
        AC_LANG_PUSH([C])
        
        save_LIBS="$LIBS"
        save_LDFLAGS="$LDFLAGS"
        save_CFLAGS="$CFLAGS"
        save_CXXFLAGS="$CXXFLAGS"
        
        LIBS="$LIBS"
        LDFLAGS="-L$_configure_mpi_cxx_path/lib $LDFLAGS"
        CFLAGS="-I$_configure_mpi_cxx_path/include $CFLAGS"
        CXXFLAGS="-I$_configure_mpi_cxx_path/include $CXXFLAGS"
        
        AC_CHECK_LIB(
          [mpi],
          [MPI_Init],
          [
            _configure_mpi_cxx_library_found="yes"
            MPI_LIBS="-lmpi"
            MPI_LDFLAGS="-L$_configure_mpi_cxx_path/lib"
          ],
          [
            _configure_mpi_cxx_library_found="no"
          ]
        )
        
        AS_IF(
          [test "x$_configure_mpi_cxx_library_found" = "xno"],
          [
            LIBS="$save_LIBS"
            LDFLAGS="-L$_configure_mpi_cxx_path/lib64 $save_LDFLAGS"
            
            AC_CHECK_LIB(
              [mpi],
              [MPI_Init],
              [
                _configure_mpi_cxx_library_found="yes"
                MPI_LIBS="-lmpi"
                MPI_LDFLAGS="-L$_configure_mpi_cxx_path/lib64"
              ],
              [
                _configure_mpi_cxx_library_found="no"
              ],
            )
          ]
        )
        
        AC_CHECK_HEADER(
          [mpi.h],
          [
            _configure_mpi_cxx_header_found="yes"
            MPI_CFLAGS="-I$_configure_mpi_cxx_path/include"
            MPI_CXXFLAGS="-I$_configure_mpi_cxx_path/include"
            MPI_CPPFLAGS=""
          ],
          [
            _configure_mpi_cxx_header_found="no"
          ],
          [AC_INCLUDES_DEFAULT]
        )
        
        LIBS="$save_LIBS"
        LDFLAGS="$save_LDFLAGS"
        CFLAGS="$save_CFLAGS"
        CXXFLAGS="$save_CXXFLAGS"
        
        AC_LANG_POP([C])
      ]
    )
  ]
)

dnl for automake use
AM_CONDITIONAL([HAVE_MPI],[test "x$_configure_mpi_cxx_library_found" = "xyes" && test "x$_configure_mpi_cxx_header_found" = "xyes"])

AS_IF(
  [test "x$_configure_mpi_cxx_library_found" = "xyes" && test "x$_configure_mpi_cxx_header_found" = "xyes"],
  [
    dnl for configure use
    use_mpi=yes
    
    dnl for C preprocessor
    AC_DEFINE([HAVE_MPI], [1], [Defined if MPI library is detected.])
  ],
  [
    dnl for configure use
    use_mpi=no
    
    dnl reset
    MPI_LIBS=""
    MPI_LDFLAGS=""
    MPI_CXXFLAGS=""
    MPI_CPPFLAGS=""
  ]
)

AS_IF(
  [test "x$_configure_mpi_cxx_req" = "xyes"],
  [AS_IF([test "x$use_mpi" = "xyes"],[],[AC_MSG_FAILURE([MPI requested, but not found])])],
  [test "x$_configure_mpi_cxx_req" = "xauto"],
  [AS_IF([test "x$use_mpi" = "xyes"],[],[AC_MSG_WARN([MPI not found, will not use MPI])])],
  [test "x$_configure_mpi_cxx_req" = "xno"],
  [AC_MSG_NOTICE([MPI not requested])]
)

])

