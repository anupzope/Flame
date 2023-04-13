AC_DEFUN([CONFIGURE_LOCI], [
AC_PREREQ([2.50])

AC_ARG_VAR([LOCI_LIBS],[Loci libraries])
AC_ARG_VAR([LOCI_LDFLAGS],[Loci link flags])
AC_ARG_VAR([LOCI_CXXFLAGS],[Loci C++ compilation flags])
AC_ARG_VAR([LOCI_CPPFLAGS],[Loci preprocessor flags])

AC_ARG_VAR([LOCI_DEP_LIBS],[Loci dependency libraries])
AC_ARG_VAR([LOCI_DEP_LDFLAGS],[Loci dependency link flags])
AC_ARG_VAR([LOCI_DEP_CXXFLAGS],[Loci dependency C++ compilation flags])
AC_ARG_VAR([LOCI_DEP_CPPFLAGS],[Loci dependency preprocessor flags])

AC_ARG_WITH(
[loci],
[
  AS_HELP_STRING(
    [--with-loci=[auto/yes/no/PATH]],
    [compile with Loci support. Default: auto]
  )
],
[],
[with_loci=auto]
)

AS_CASE($with_loci,
  [no], [
    _configure_loci_req=no
    _configure_loci_path=no
  ],
  [yes], [
    _configure_loci_req=yes
    _configure_loci_path=auto
  ],
  [auto], [
    _configure_loci_req=auto
    _configure_loci_path=auto
  ],
  [/*], [
    _configure_loci_req=yes
    AS_IF(
      [test -z "$with_loci"],
      [_configure_loci_path=auto],
      [_configure_loci_path=$with_loci]
    )
  ],
  [
    _configure_loci_req=no
    _configure_loci_path=no
  ]
)

_configure_loci_library_found=no
_configure_loci_header_found=no

AS_IF(
  [test "x$_configure_loci_req" = "xyes" || test "x$_configure_loci_req" = "xauto"],
  [
    AS_VAR_IF(
      [LOCI_BASE],
      [],
      [_configure_loci_full_path="notfound"],
      [_configure_loci_full_path="$LOCI_BASE"]
    )
    
    AS_IF(
      [test "x$_configure_loci_full_path" = "xnotfound"],
      [
        AC_MSG_WARN([Loci installation not found])
      ],
      [
        _configure_loci_path="$_configure_loci_full_path"
        
        AC_MSG_NOTICE([Checking Loci at $_configure_loci_path])
        
        AC_LANG_PUSH([C++])
        
        LOCI_DEP_LIBS=""
        LOCI_DEP_LDFLAGS=""
        LOCI_DEP_CXXFLAGS=""
        LOCI_DEP_CPPFLAGS=""
        
        m4_foreach_w(
          [dep], [petsc cgns hdf5 parmetis xml2],
          [
            AS_CASE(dep,
            [xml2],
            [
              AS_IF(
                [test "x$use_xml2" = "xyes"],
                [
		  LOCI_DEP_CPPFLAGS="-DUSE_LIBXML2 $LOCI_DEP_CPPFLAGS"
                  LOCI_DEP_CXXFLAGS="$XML2_CFLAGS $LOCI_DEP_CXXFLAGS"
                  LOCI_DEP_LIBS="$XML2_LIBS $LOCI_DEP_LIBS"
                ]
	      )
	    ],
            [parmetis],
            [
              AS_IF(
                [test "x$use_parmetis" = "xyes"],
                [
                  LOCI_DEP_CPPFLAGS="-DLOCI_USE_METIS $PARMETIS_CPPFLAGS $LOCI_DEP_CPPFLAGS"
                  LOCI_DEP_CXXFLAGS="$PARMETIS_CXXFLAGS $LOCI_DEP_CXXFLAGS"
                  LOCI_DEP_LDFLAGS="$PARMETIS_LDFLAGS $LOCI_DEP_LDFLAGS"
                  LOCI_DEP_LIBS="$PARMETIS_LIBS $LOCI_DEP_LIBS"
                ]
              )
            ],
            [hdf5],
            [
              AS_IF(
                [test "x$use_hdf5" = "xyes"],
                [
                  LOCI_DEP_CPPFLAGS="$HDF5_CPPFLAGS $LOCI_DEP_CPPFLAGS"
                  LOCI_DEP_CXXFLAGS="$HDF5_CXXFLAGS $LOCI_DEP_CXXFLAGS"
                  LOCI_DEP_LDFLAGS="$HDF5_LDFLAGS $LOCI_DEP_LDFLAGS"
                  LOCI_DEP_LIBS="$HDF5_LIBS $LOCI_DEP_LIBS"
                ]
              )
            ],
            [cgns],
            [
              AS_IF(
                [test "x$use_cgns" = "xyes"],
                [
                  LOCI_DEP_CPPFLAGS="-DUSE_CGNS $CGNS_CPPFLAGS $LOCI_DEP_CPPFLAGS"
                  LOCI_DEP_CXXFLAGS="$CGNS_CXXFLAGS $LOCI_DEP_CXXFLAGS"
                  LOCI_DEP_LDFLAGS="$CGNS_LDFLAGS $LOCI_DEP_LDFLAGS"
                  LOCI_DEP_LIBS="$CGNS_LIBS $LOCI_DEP_LIBS"
                ]
              )
            ],
            [petsc],
            [
              AS_IF(
                [test "x$use_petsc" = "xyes"],
                [
                  LOCI_DEP_CPPFLAGS="-DUSE_PETSC -DPETSC_USE_EXTERN_CXX $PETSC_CPPFLAGS $LOCI_DEP_CPPFLAGS"
                  LOCI_DEP_CXXFLAGS="$PETSC_CXXFLAGS $LOCI_DEP_CXXFLAGS"
                  LOCI_DEP_LDFLAGS="$PETSC_LDFLAGS $LOCI_DEP_LDFLAGS"
                  LOCI_DEP_LIBS="$PETSC_LIBS $LOCI_DEP_LIBS"
                ]
              )
            ],
            [AC_MSG_NOTICE([token=dep])]
            )
          ]
        )
        
        AC_MSG_NOTICE([LOCI_DEP_CPPFLAGS=$LOCI_DEP_CPPFLAGS])
        AC_MSG_NOTICE([LOCI_DEP_CXXFLAGS=$LOCI_DEP_CXXFLAGS])
        AC_MSG_NOTICE([LOCI_DEP_LDFLAGS=$LOCI_DEP_LDFLAGS])
        AC_MSG_NOTICE([LOCI_DEP_LIBS=$LOCI_DEP_LIBS])
        
        save_CC="$CC"
        save_CXX="$CXX"
        save_LIBS="$LIBS"
        save_LDFLAGS="$LDFLAGS"
        save_CFLAGS="$CFLAGS"
        save_CXXFLAGS="$CXXFLAGS"
        save_CPPFLAGS="$CPPFLAGS"
        
        CC="mpicc"
        CXX="mpicxx"
        LIBS="-lLoci -lTools -lsprng $LOCI_DEP_LIBS $save_LIBS"
        LDFLAGS="-L$_configure_loci_path/lib $LOCI_DEP_LDFLAGS $save_LDFLAGS"
        CFLAGS="-I$_configure_loci_path/include $LOCI_DEP_CXXFLAGS $CFLAGS"
        CXXFLAGS="-I$_configure_loci_path/include $LOCI_DEP_CXXFLAGS $CXXFLAGS"
        CPPFLAGS="$LOCI_DEP_CPPFLAGS $CPPFLAGS"
        
        AC_MSG_CHECKING([for -lLoci usability])
        AC_LINK_IFELSE(
          [AC_LANG_PROGRAM([#include <Loci.h>],[Loci::Init(0, 0);])],
          [
            AC_MSG_RESULT([yes])
            _configure_loci_library_found="yes"
            LOCI_LIBS="-lLoci -lTools -lsprng"
            LOCI_LDFLAGS="-L$_configure_loci_path/lib"
          ],
          [
            AC_MSG_RESULT([no])
            _configure_loci_library_found="no"
          ]
        )
        
        AC_CHECK_HEADER(
          [Loci.h],
          [
            _configure_loci_header_found="yes"
            LOCI_CXXFLAGS="-I$_configure_loci_path/include"
            LOCI_CPPFLAGS=""
          ],
          [
            _configure_loci_header_found="no"
          ],
          [AC_INCLUDES_DEFAULT]
        )
        
        CC="$save_CC"
        CXX="$save_CXX"
        LIBS="$save_LIBS"
        LDFLAGS="$save_LDFLAGS"
        CFLAGS="$save_CFLAGS"
        CXXFLAGS="$save_CXXFLAGS"
        CPPFLAGS="$save_CPPFLAGS"
        
        AC_LANG_POP([C++])
      ]
    )
  ]
)

dnl for automake use
AM_CONDITIONAL([HAVE_LOCI],[test "x$_configure_loci_library_found" = "xyes" && test "x$_configure_loci_header_found" = "xyes"])

AS_IF(
  [test "x$_configure_loci_library_found" = "xyes" && test "x$_configure_loci_header_found" = "xyes"],
  [
    dnl for configure use
    use_loci="yes"
    
    dnl for C preprocessor
    AC_DEFINE([HAVE_LOCI], [1], [Defined if Loci library is detected.])
  ],
  [
    dnl for configure use
    use_loci="no"
    
    dnl reset
    LOCI_LIBS=""
    LOCI_LDFLAGS=""
    LOCI_CXXFLAGS=""
    LOCI_CPPFLAGS=""
  ]
)

AS_IF(
  [test "x$_configure_loci_req" = "xyes"],
  [AS_IF([test "x$use_loci" = "xyes"],[],[AC_MSG_FAILURE([Loci requested, but not found])])],
  [test "x$_configure_loci_req" = "xauto"],
  [AS_IF([test "x$use_loci" = "xyes"],[],[AC_MSG_WARN([Loci not found, will not use Loci])])],
  [test "x$_configure_loci_req" = "xno"],
  [AC_MSG_NOTICE([Loci not requested])] 
)

])
