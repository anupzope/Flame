AC_DEFUN([CONFIGURE_METIS], [
AC_PREREQ([2.50])

AC_ARG_VAR([METIS_LIBS],[METIS libraries])
AC_ARG_VAR([METIS_LDFLAGS],[METIS link flags])
AC_ARG_VAR([METIS_CFLAGS],[METIS C compilation flags])
AC_ARG_VAR([METIS_CXXFLAGS],[METIS C++ compilation flags])
AC_ARG_VAR([METIS_CPPFLAGS],[METIS preprocessor flags])

AC_ARG_WITH(
[metis],
[
  AS_HELP_STRING(
    [--with-metis=[auto/yes/no/PATH]],
    [compile with METIS support. Default: auto]
  )
],
[],
[with_metis=auto]
)

AS_CASE($with_metis,
  [no], [
    _configure_metis_req=no
    _configure_metis_path=no
  ],
  [yes], [
    _configure_metis_req=yes
    _configure_metis_path=auto
  ],
  [auto], [
    _configure_metis_req=auto
    _configure_metis_path=auto
  ],
  [/*], [
    _configure_metis_req=yes
    AS_IF(
      [test -z "$with_metis"],
      [_configure_metis_path=auto],
      [_configure_metis_path=$with_metis]
    )
  ],
  [
    _configure_metis_req=no
    _configure_metis_path=no
  ]
)

_configure_metis_library_found=no
_configure_metis_header_found=no

AS_IF(
  [test "x$_configure_metis_req" = "xyes" || test "x$_configure_metis_req" = "xauto"],
  [
    dnl Detect path to gpmetis
    AS_IF(
      [test "x$_configure_metis_path" = "xauto"],
      [AC_PATH_PROGS(
        [_configure_metis_full_path],
        [gpmetis ndmetis mpmetis m2gmetis graphchk cmpfillin],
        [notfound]
      )],
      [AC_PATH_PROGS(
        [_configure_metis_full_path],
        [gpmetis ndmetis mpmetis m2gmetis graphchk cmpfillin],
        [notfound],
        [$_configure_metis_path]
      )]
    )
    
    AS_IF(
      [test "x$_configure_metis_full_path" = "xnotfound"],
      [
        AC_MSG_WARN([METIS installation not found])
      ],
      [
        _configure_metis_path=`AS_DIRNAME(["$_configure_metis_full_path"])`
        _configure_metis_path=`AS_DIRNAME(["$_configure_metis_path"])`
        
        AC_MSG_NOTICE([Checking METIS at $_configure_metis_path])
        
        AC_LANG_PUSH([C])
        
        save_LIBS="$LIBS"
        save_LDFLAGS="$LDFLAGS"
        save_CFLAGS="$CFLAGS"
        save_CXXFLAGS="$CXXFLAGS"
        
        LIBS="$save_LIBS"
        LDFLAGS="-L$_configure_metis_path/lib $save_LDFLAGS"
        CFLAGS="-I$_configure_metis_path/include $CFLAGS"
        CXXFLAGS="-I$_configure_metis_path/include $CFLAGS"
        
        AC_CHECK_LIB(
          [metis],
          [METIS_Free],
          [
            _configure_metis_library_found="yes"
            METIS_LIBS="-lmetis $_configure_metis_path/lib/libGKlib.a -lm"
            METIS_LDFLAGS="-L$_configure_metis_path/lib"
          ],
          [
            _configure_metis_library_found="no"
          ],
	  [$_configure_metis_path/lib/libGKlib.a -lm]
        )
        
        AS_IF(
          [test "x$_configure_metis_library_found" = "xno"],
          [
            LIBS="$save_LIBS"
            LDFLAGS="-L$_configure_metis_path/lib64 $save_LDFLAGS"
            
            AC_CHECK_LIB(
              [metis],
              [METIS_Free],
              [
                _configure_metis_library_found="yes"
                METIS_LIBS="-lmetis $_configure_metis_path/lib64/libGKlib.a -lm"
                METIS_LDFLAGS="-L$_configure_metis_path/lib"
              ],
              [
                _configure_metis_library_found="no"
              ],
	      [$_configure_metis_path/lib64/libGKlib.a -lm]
            )
          ]
        )
        
        AC_CHECK_HEADER(
          [metis.h],
          [
            _configure_metis_header_found="yes"
            METIS_CFLAGS="-I$_configure_metis_path/include"
            METIS_CXXFLAGS="-I$_configure_metis_path/include"
            METIS_CPPFLAGS=""
          ],
          [
            _configure_metis_header_found="no"
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
AM_CONDITIONAL([HAVE_METIS],[test "x$_configure_metis_library_found" = "xyes" && test "x$_configure_metis_header_found" = "xyes"])

AS_IF(
  [test "x$_configure_metis_library_found" = "xyes" && test "x$_configure_metis_header_found" = "xyes"], [
    dnl for configure use
    use_metis="yes"
    
    dnl for C preprocessor
    AC_DEFINE([HAVE_METIS], [1], [Defined if METIS library is detected.])
  ],
  [
    dnl for configure use
    use_metis="no"
    
    dnl reset
    METIS_LIBS=""
    METIS_LDFLAGS=""
    METIS_CFLAGS=""
    METIS_CXXFLAGS=""
    METIS_CPPFLAGS=""
  ]
)

AS_IF(
  [test "x$_configure_metis_req" = "xyes"],
  [AS_IF([test "x$use_metis" = "xyes"],[],[AC_MSG_FAILURE([METIS requested, but not found])])],
  [test "x$_configure_metis_req" = "xauto"],
  [AS_IF([test "x$use_metis" = "xyes"],[],[AC_MSG_WARN([METIS not found, will not use METIS])])],
  [test "x$_configure_metis_req" = "xno"],
  [AC_MSG_NOTICE([METIS not requested])] 
)

])
