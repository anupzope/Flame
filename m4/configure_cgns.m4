AC_DEFUN([CONFIGURE_CGNS], [
AC_PREREQ([2.50])

AC_ARG_VAR([CGNS_LIBS],[CGNS libraries])
AC_ARG_VAR([CGNS_LDFLAGS],[CGNS link flags])
AC_ARG_VAR([CGNS_CFLAGS],[CGNS C compilation flags])
AC_ARG_VAR([CGNS_CXXFLAGS],[CGNS C++ compilation flags])
AC_ARG_VAR([CGNS_CPPFLAGS],[CGNS preprocessor flags])

AC_ARG_WITH(
[cgns],
[
  AS_HELP_STRING(
    [--with-cgns=[auto/yes/no/PATH]],
    [compile with CGNS support. Default: auto]
  )
],
[],
[with_cgns=auto]
)

AS_CASE($with_cgns,
  [no], [
    _configure_cgns_req=no
    _configure_cgns_path=no
  ],
  [yes], [
    _configure_cgns_req=yes
    _configure_cgns_path=auto
  ],
  [auto], [
    _configure_cgns_req=auto
    _configure_cgns_path=auto
  ],
  [/*], [
    _configure_cgns_req=yes
    AS_IF(
      [test -z "$with_cgns"],
      [_configure_cgns_path=auto],
      [_configure_cgns_path=$with_cgns]
    )
  ],
  [
    _configure_cgns_req=no
    _configure_cgns_path=no
  ]
)

_configure_cgns_library_found=no
_configure_cgns_header_found=no

AS_IF(
  [test "x$_configure_cgns_req" = "xyes" || test "x$_configure_cgns_req" = "xauto"],
  [
    dnl Detect path of cgnsdiff
    AS_IF(
      [test "x$_configure_cgns_path" = "xauto"],
      [AC_PATH_PROGS(
        [_configure_cgns_full_path],
        [cgnsdiff cgnslist cgnsnames cgnscheck],
        [notfound]
      )],
      [AC_PATH_PROGS(
        [_configure_cgns_full_path],
        [cgnsdiff cgnslist cgnsnames cgnscheck],
        [notfound],
        [$_configure_cgns_path]
      )]
    )
    
    AS_IF(
      [test "x$_configure_cgns_full_path" = "xnotfound"],
      [
        AC_MSG_WARN([CGNS installation not found])
      ],
      [
        _configure_cgns_path=`AS_DIRNAME(["$_configure_cgns_full_path"])`
        _configure_cgns_path=`AS_DIRNAME(["$_configure_cgns_path"])`
        
        AC_MSG_NOTICE(Checking cgns at $_configure_cgns_path)
        
        AC_LANG_PUSH([C])
        
        save_LIBS="$LIBS"
        save_LDFLAGS="$LDFLAGS"
        save_CFLAGS="$CFLAGS"
        save_CXXFLAGS="$CXXFLAGS"
        
        LIBS="$LIBS"
        LDFLAGS="-L$_configure_cgns_path/lib $LDFLAGS"
        CFLAGS="-I$_configure_cgns_path/include $CFLAGS"
        CXXFLAGS="-I$_configure_cgns_path/include $CXXFLAGS"
        
        AC_CHECK_LIB(
          [cgns],
          [cg_open],
          [
            _configure_cgns_library_found="yes"
            CGNS_LIBS="-lcgns"
            CGNS_LDFLAGS="-L$_configure_cgns_path/lib"
          ],
          [
            _configure_cgns_library_found="no"
          ]
        )
        
        AS_IF(
          [test "x$_configure_cgns_library_found" = "xno"],
          [
            LIBS="$save_LIBS"
            LDFLAGS="-L$_configure_cgns_path/lib64 $save_LDFLAGS"
            
            AC_CHECK_LIB(
              [cgns],
              [cg_open],
              [
                _configure_cgns_library_found="yes"
                CGNS_LIBS="-lcgns"
                CGNS_LDFLAGS="-L$_configure_cgns_path/lib64"
              ],
              [
                _configure_cgns_library_found="no"
              ]
            )
          ]
        )
        
        AC_CHECK_HEADER(
          [cgnslib.h],
          [
            _configure_cgns_header_found="yes"
            CGNS_CFLAGS="-I$_configure_cgns_path/include"
            CGNS_CXXFLAGS="-I$_configure_cgns_path/include"
            CGNS_CPPFLAGS=""
          ],
          [
            _configure_cgns_header_found="no"
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
AM_CONDITIONAL([HAVE_CGNS],[test "x$_configure_cgns_library_found" = "xyes" && test "x$_configure_cgns_header_found" = "xyes"])

AS_IF(
  [test "x$_configure_cgns_library_found" = "xyes" && test "x$_configure_cgns_header_found" = "xyes"], [
    dnl for configure use
    use_cgns=yes
    
    dnl for C preprocessor
    AC_DEFINE([HAVE_CGNS], [1], [Defined if CGNS library is detected.])
  ],
  [
    dnl for configure use
    use_cgns=no
    
    dnl reset
    CGNS_LIBS=""
    CGNS_LDFLAGS=""
    CGNS_CFLAGS=""
    CGNS_CXXFLAGS=""
    CGNS_CPPFLAGS=""
  ]
)

AS_IF(
  [test "x$_configure_cgns_req" = "xyes"],
  [AS_IF([test "x$use_cgns" = "xyes"],[],[AC_MSG_FAILURE([CGNS requested, but not found])])],
  [test "x$_configure_cgns_req" = "xauto"],
  [AS_IF([test "x$use_cgns" = "xyes"],[],[AC_MSG_WARN([CGNS not found, will not use CGNS])])],
  [test "x$_configure_cgns_req" = "xno"],
  [AC_MSG_NOTICE([CGNS not requested])]
)

])
