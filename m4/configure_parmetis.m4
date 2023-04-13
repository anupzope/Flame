AC_DEFUN([CONFIGURE_PARMETIS], [
AC_PREREQ([2.50])

AC_ARG_VAR([PARMETIS_LIBS],[PARMETIS libraries])
AC_ARG_VAR([PARMETIS_LDFLAGS],[PARMETIS link flags])
AC_ARG_VAR([PARMETIS_CFLAGS],[PARMETIS C compilation flags])
AC_ARG_VAR([PARMETIS_CXXFLAGS],[PARMETIS C++ compilation flags])
AC_ARG_VAR([PARMETIS_CPPFLAGS],[PARMETIS preprocessor flags])

AC_ARG_WITH(
[parmetis],
[
  AS_HELP_STRING(
    [--with-parmetis=[auto/yes/no/PATH]],
    [compile with PARMETIS support. Default: auto]
  )
],
[],
[with_parmetis=auto]
)

AS_CASE($with_parmetis,
  [no], [
    _configure_parmetis_req=no
    _configure_parmetis_path=no
  ],
  [yes], [
    _configure_parmetis_req=yes
    _configure_parmetis_path=auto
  ],
  [auto], [
    _configure_parmetis_req=auto
    _configure_parmetis_path=auto
  ],
  [/*], [
    _configure_parmetis_req=yes
    AS_IF(
      [test -z "$with_parmetis"],
      [_configure_parmetis_path=auto],
      [_configure_parmetis_path=$with_parmetis]
    )
  ],
  [
    _configure_parmetis_req=no
    _configure_parmetis_path=no
  ]
)

_configure_parmetis_library_found=no
_configure_parmetis_header_found=no

AS_IF(
  [test "x$_configure_parmetis_req" = "xyes" || test "x$_configure_parmetis_req" = "xauto"],
  [
    dnl Detect path to pm_parmetis
    AS_IF(
      [test "x$_configure_parmetis_path" = "xauto"],
      [AC_PATH_PROGS(
        [_configure_parmetis_full_path],
        [pm_parmetis pm_pometis pm_dglpart],
        [notfound]
      )],
      [AC_PATH_PROGS(
        [_configure_parmetis_full_path],
        [pm_parmetis pm_pometis pm_dglpart],
        [notfound],
        [$_configure_parmetis_path]
      )]
    )
    
    AS_IF(
      [test "x$_configure_parmetis_full_path" = "xnotfound"],
      [
        AC_MSG_WARN([PARMETIS installation not found])
      ],
      [
        _configure_parmetis_path=`AS_DIRNAME(["$_configure_parmetis_full_path"])`
        _configure_parmetis_path=`AS_DIRNAME(["$_configure_parmetis_path"])`
        
        AC_MSG_NOTICE([Checking PARMETIS at $_configure_parmetis_path])
        
        AC_LANG_PUSH([C])

        save_CC="$CC"
        save_LIBS="$LIBS"
        save_LDFLAGS="$LDFLAGS"
        save_CFLAGS="$CFLAGS"
        save_CXXFLAGS="$CXXFLAGS"

        CC="mpicc"
        LIBS="$save_LIBS"
        LDFLAGS="-L$_configure_parmetis_path/lib $save_LDFLAGS"
        CFLAGS="-I$_configure_parmetis_path/include $CFLAGS"
        CXXFLAGS="-I$_configure_parmetis_path/include $CFLAGS"
        
        AC_CHECK_LIB(
          [parmetis],
          [ParMETIS_V3_PartKway],
          [
            _configure_parmetis_library_found="yes"
            PARMETIS_LIBS="-lparmetis $_configure_parmetis_path/lib/libGKlib.a -lm"
            PARMETIS_LDFLAGS="-L$_configure_parmetis_path/lib"
          ],
          [
            _configure_parmetis_library_found="no"
          ],
	  [$_configure_parmetis_path/lib/libGKlib.a -lm]
        )
        
        AS_IF(
          [test "x$_configure_parmetis_library_found" = "xno"],
          [
            LIBS="$save_LIBS"
            LDFLAGS="-L$_configure_parmetis_path/lib64 $save_LDFLAGS"
            
            AC_CHECK_LIB(
              [parmetis],
              [ParMETIS_V3_PartKway],
              [
                _configure_parmetis_library_found="yes"
                PARMETIS_LIBS="-lparmetis $_configure_parmetis_path/lib64/libGKlib.a -lm"
                PARMETIS_LDFLAGS="-L$_configure_parmetis_path/lib"
              ],
              [
                _configure_parmetis_library_found="no"
              ],
	      [$_configure_parmetis_path/lib64/libGKlib.a -lm]
            )
          ]
        )
        
        AC_CHECK_HEADER(
          [parmetis.h],
          [
            _configure_parmetis_header_found="yes"
            PARMETIS_CFLAGS="-I$_configure_parmetis_path/include"
            PARMETIS_CXXFLAGS="-I$_configure_parmetis_path/include"
            PARMETIS_CPPFLAGS=""
          ],
          [
            _configure_parmetis_header_found="no"
          ],
          [AC_INCLUDES_DEFAULT]
        )

        CC="$save_CC"
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
AM_CONDITIONAL([HAVE_PARMETIS],[test "x$_configure_parmetis_library_found" = "xyes" && test "x$_configure_parmetis_header_found" = "xyes"])

AS_IF(
  [test "x$_configure_parmetis_library_found" = "xyes" && test "x$_configure_parmetis_header_found" = "xyes"], [
    dnl for configure use
    use_parmetis="yes"
    
    dnl for C preprocessor
    AC_DEFINE([HAVE_PARMETIS], [1], [Defined if PARMETIS library is detected.])
  ],
  [
    dnl for configure use
    use_parmetis="no"
    
    dnl reset
    PARMETIS_LIBS=""
    PARMETIS_LDFLAGS=""
    PARMETIS_CFLAGS=""
    PARMETIS_CXXFLAGS=""
    PARMETIS_CPPFLAGS=""
  ]
)

AS_IF(
  [test "x$_configure_parmetis_req" = "xyes"],
  [AS_IF([test "x$use_parmetis" = "xyes"],[],[AC_MSG_FAILURE([PARMETIS requested, but not found])])],
  [test "x$_configure_parmetis_req" = "xauto"],
  [AS_IF([test "x$use_parmetis" = "xyes"],[],[AC_MSG_WARN([PARMETIS not found, will not use PARMETIS])])],
  [test "x$_configure_parmetis_req" = "xno"],
  [AC_MSG_NOTICE([PARMETIS not requested])] 
)

])
