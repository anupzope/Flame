AC_DEFUN([CONFIGURE_XML2], [
AC_PREREQ([2.50])


AC_ARG_VAR([XML2_LIBS],[libxml2 libraries])
AC_ARG_VAR([XML2_CFLAGS],[libxml2 C compilation flags])

AC_ARG_WITH(
[xml2],
[
  AS_HELP_STRING(
    [--with-xml2=[yes/no]],
    [compile with libxml2 support. Default: yes]
  )
],
[],
[with_xml2=yes]
)

AS_CASE($with_xml2,
  [no],
  [
    use_xml2="no"
  ],
  [yes],
  [
    PKG_CHECK_MODULES(
      XML2, libxml-2.0,
      [
        use_xml2="yes"
        AC_DEFINE([HAVE_XML2], [1], [Defined if XML2 library is found])
      ],
      [
        use_xml2="no"
      ]
    )
  ]
)

])
