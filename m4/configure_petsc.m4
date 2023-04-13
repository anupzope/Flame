AC_DEFUN([CONFIGURE_PETSC], [
AC_PREREQ([2.50])

AC_ARG_VAR([PETSC_LIBS],[PETSc libraries])
AC_ARG_VAR([PETSC_LDFLAGS],[PETSc link flags])
AC_ARG_VAR([PETSC_CFLAGS],[PETSc C compilation flags])
AC_ARG_VAR([PETSC_CXXFLAGS],[PETSc C++ compilation flags])
AC_ARG_VAR([PETSC_CPPFLAGS],[PETSc preprocessor flags])

AC_ARG_WITH(
[petsc],
[
  AS_HELP_STRING(
    [--with-petsc=[auto/yes/no/PATH]],
    [compile with PETSc support. Default: auto]
  )
],
[],
[with_petsc=auto]
)

AS_CASE($with_petsc,
  [no], [
    _configure_petsc_req=no
    _configure_petsc_path=no
  ],
  [yes], [
    _configure_petsc_req=yes
    _configure_petsc_path=auto
  ],
  [auto], [
    _configure_petsc_req=auto
    _configure_petsc_path=auto
  ],
  [/*], [
    _configure_petsc_req=yes
    AS_IF(
      [test -z "$with_petsc"],
      [_configure_petsc_path=auto],
      [_configure_petsc_path=$with_petsc]
    )
  ],
  [
    _configure_petsc_req=no
    _configure_petsc_path=no
  ]
)

_configure_petsc_library_found=no
_configure_petsc_header_found=no

AS_IF(
  [test "x$_configure_petsc_req" = "xyes" || test "x$_configure_petsc_req" = "xauto"],
  [
    AS_VAR_IF(
      [PETSC_DIR],
      [],
      [_configure_petsc_full_path="notfound"],
      [_configure_petsc_full_path="$PETSC_DIR"]
    )
    
    AS_IF(
      [test "x$_configure_petsc_full_path" = "xnotfound"],
      [
        AC_MSG_WARN([PETSc installation not found])
      ],
      [
        _configure_petsc_path="$_configure_petsc_full_path"
        
        AC_MSG_NOTICE([Checking PETSc at $_configure_petsc_path])
        
        AC_LANG_PUSH([C])

        save_CC="$CC"
        save_LIBS="$LIBS"
        save_LDFLAGS="$LDFLAGS"
        save_CFLAGS="$CFLAGS"
        save_CXXFLAGS="$CXXFLAGS"

        CC="mpicc"
        LIBS="$save_LIBS"
        LDFLAGS="-L$_configure_petsc_path/lib $save_LDFLAGS"
        CFLAGS="-I$_configure_petsc_path/include $CFLAGS"
        CXXFLAGS="-I$_configure_petsc_path/include $CFLAGS"
        
        AC_CHECK_LIB(
          [petsc],
          [PetscInitialize],
          [
            _configure_petsc_library_found="yes"
            PETSC_LIBS="-lpetsc"
            PETSC_LDFLAGS="-L$_configure_petsc_path/lib"
          ],
          [
            _configure_petsc_library_found="no"
          ]
        )
        
        AS_IF(
          [test "x$_configure_petsc_library_found" = "xno"],
          [
            LIBS="$save_LIBS"
            LDFLAGS="-L$_configure_petsc_path/lib64 $save_LDFLAGS"
            
            AC_CHECK_LIB(
              [petsc],
              [PetscInitialize],
              [
                _configure_petsc_library_found="yes"
                PETSC_LIBS="-lpetsc"
                PETSC_LDFLAGS="-L$_configure_petsc_path/lib"
              ],
              [
                _configure_petsc_library_found="no"
              ]
            )
          ]
        )
        
        AC_CHECK_HEADER(
          [petsc.h],
          [
            _configure_petsc_header_found="yes"
            PETSC_CFLAGS="-I$_configure_petsc_path/include"
            PETSC_CXXFLAGS="-I$_configure_petsc_path/include"
            PETSC_CPPFLAGS=""
          ],
          [
            _configure_petsc_header_found="no"
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
AM_CONDITIONAL([HAVE_PETSC],[test "x$_configure_petsc_library_found" = "xyes" && test "x$_configure_petsc_header_found" = "xyes"])

AS_IF(
  [test "x$_configure_petsc_library_found" = "xyes" && test "x$_configure_petsc_header_found" = "xyes"], [
    dnl for configure use
    use_petsc="yes"
    
    dnl for C preprocessor
    AC_DEFINE([HAVE_PETSC], [1], [Defined if PETSc library is detected.])
  ],
  [
    dnl for configure use
    use_petsc="no"
    
    dnl reset
    PETSC_LIBS=""
    PETSC_LDFLAGS=""
    PETSC_CFLAGS=""
    PETSC_CXXFLAGS=""
    PETSC_CPPFLAGS=""
  ]
)

AS_IF(
  [test "x$_configure_petsc_req" = "xyes"],
  [AS_IF([test "x$use_petsc" = "xyes"],[],[AC_MSG_FAILURE([PETSc requested, but not found])])],
  [test "x$_configure_petsc_req" = "xauto"],
  [AS_IF([test "x$use_petsc" = "xyes"],[],[AC_MSG_WARN([PETSc not found, will not use PETSc])])],
  [test "x$_configure_petsc_req" = "xno"],
  [AC_MSG_NOTICE([PETSc not requested])] 
)

])
