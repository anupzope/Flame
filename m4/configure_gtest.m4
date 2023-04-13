AC_DEFUN([CONFIGURE_GTEST],[
AC_PREREQ(2.50)

AC_ARG_WITH(
  [gtest],
  [AS_HELP_STRING([--with-gtest],[build with gtest for tests])],
  [],
  [with_gtest=no]
)

AS_CASE($with_gtest,
  [yes],
  [PKG_CHECK_MODULES(
    [GTEST],
    [gtest >= 1.13.0],
    [
      use_gtest=yes
      AC_DEFINE([HAVE_GTEST],[1],[Defined to 1 if gtest is available])
      AC_MSG_NOTICE([HAVE_GTEST=$HAVE_GTEST])
      AC_MSG_NOTICE([GTEST_CFLAGS=$GTEST_CFLAGS])
      AC_MSG_NOTICE([GTEST_LIBS=$GTEST_LIBS])
      AC_MSG_NOTICE([GTEST_LDFLAGS=$GTEST_LDFLAGS])
    ],
    [HAVE_GTEST=0]
  )
  ],
  [use_gtest=no]
)

AM_CONDITIONAL([HAVE_GTEST],[test "x$use_gtest" = "xyes"])

])
