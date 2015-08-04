# Configure paths for libogg
# Jack Moffitt <jack@icecast.org> 10-21-2000
# Shamelessly stolen from Owen Taylor and Manish Singh

dnl AM_PATH_OGG([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl Test for libogg, and define OGG_CFLAGS and OGG_LIBS
dnl
AC_DEFUN(AM_PATH_OGG,
[dnl 
dnl Get the cflags and libraries
dnl
AC_ARG_WITH(ogg,[  --with-ogg=PFX   Prefix where libogg is installed (optional)], ogg_prefix="$withval", ogg_prefix="")
AC_ARG_WITH(ogg-libraries,[  --with-ogg-libraries=DIR   Directory where libogg library is installed (optional)], ogg_libraries="$withval", ogg_libraries="")
AC_ARG_WITH(ogg-includes,[  --with-ogg-includes=DIR   Directory where libogg header files are installed (optional)], ogg_includes="$withval", ogg_includes="")
AC_ARG_ENABLE(oggtest, [  --disable-oggtest       Do not try to compile and run a test Ogg program],, enable_oggtest=yes)

  if test "x$ogg_libraries" != "x" ; then
    OGG_LIBS="-L$ogg_libraries"
  elif test "x$ogg_prefix" != "x" ; then
    OGG_LIBS="-L$ogg_prefix/lib"
  elif test "x$prefix" != "xNONE" ; then
    OGG_LIBS="-L$prefix/lib"
  fi

  OGG_LIBS="$OGG_LIBS -logg"

  if test "x$ogg_includes" != "x" ; then
    OGG_CFLAGS="-I$ogg_includes"
  elif test "x$ogg_prefix" != "x" ; then
    OGG_CFLAGS="-I$ogg_prefix/include"
  elif test "x$prefix" != "xNONE"; then
    OGG_CFLAGS="-I$prefix/include"
  fi

  AC_MSG_CHECKING(for Ogg)
  no_ogg=""


  if test "x$enable_oggtest" = "xyes" ; then
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    CFLAGS="$CFLAGS $OGG_CFLAGS"
    LIBS="$LIBS $OGG_LIBS"
dnl
dnl Now check if the installed Ogg is sufficiently new.
dnl
      rm -f conf.oggtest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogg/ogg.h>

int main ()
{
  system("touch conf.oggtest");
  return 0;
}

],, no_ogg=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
  fi

  if test "x$no_ogg" = "x" ; then
     AC_MSG_RESULT(yes)
     ifelse([$1], , :, [$1])     
  else
     AC_MSG_RESULT(no)
     if test -f conf.oggtest ; then
       :
     else
       echo "*** Could not run Ogg test program, checking why..."
       CFLAGS="$CFLAGS $OGG_CFLAGS"
       LIBS="$LIBS $OGG_LIBS"
       AC_TRY_LINK([
#include <stdio.h>
#include <ogg/ogg.h>
],     [ return 0; ],
       [ echo "*** The test program compiled, but did not run. This usually means"
       echo "*** that the run-time linker is not finding Ogg or finding the wrong"
       echo "*** version of Ogg. If it is not finding Ogg, you'll need to set your"
       echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
       echo "*** to the installed location  Also, make sure you have run ldconfig if that"
       echo "*** is required on your system"
       echo "***"
       echo "*** If you have an old version installed, it is best to remove it, although"
       echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
       [ echo "*** The test program failed to compile or link. See the file config.log for the"
       echo "*** exact error that occured. This usually means Ogg was incorrectly installed"
       echo "*** or that you have moved Ogg since it was installed. In the latter case, you"
       echo "*** may want to edit the ogg-config script: $OGG_CONFIG" ])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
     OGG_CFLAGS=""
     OGG_LIBS=""
     ifelse([$2], , :, [$2])
  fi
  AC_SUBST(OGG_CFLAGS)
  AC_SUBST(OGG_LIBS)
  rm -f conf.oggtest
])

# Configure paths for libvorbis
# Jack Moffitt <jack@icecast.org> 10-21-2000
# Shamelessly stolen from Owen Taylor and Manish Singh

dnl AM_PATH_VORBIS([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl Test for libvorbis, and define VORBIS_CFLAGS and VORBIS_LIBS
dnl
AC_DEFUN(AM_PATH_VORBIS,
[dnl 
dnl Get the cflags and libraries
dnl
AC_ARG_WITH(vorbis,[  --with-vorbis=PFX   Prefix where libvorbis is installed (optional)], vorbis_prefix="$withval", vorbis_prefix="")
AC_ARG_WITH(vorbis-libraries,[  --with-vorbis-libraries=DIR   Directory where libvorbis library is installed (optional)], vorbis_libraries="$withval", vorbis_libraries="")
AC_ARG_WITH(vorbis-includes,[  --with-vorbis-includes=DIR   Directory where libvorbis header files are installed (optional)], vorbis_includes="$withval", vorbis_includes="")
AC_ARG_ENABLE(vorbistest, [  --disable-vorbistest       Do not try to compile and run a test Vorbis program],, enable_vorbistest=yes)

  if test "x$vorbis_libraries" != "x" ; then
    VORBIS_LIBS="-L$vorbis_libraries"
  elif test "x$vorbis_prefix" != "x" ; then
    VORBIS_LIBS="-L$vorbis_prefix/lib"
  elif test "x$prefix" != "xNONE"; then
    VORBIS_LIBS="-L$prefix/lib"
  fi

  VORBISFILE_LIBS="$VORBIS_LIBS -lvorbisfile"
  VORBISENC_LIBS="$VORBIS_LIBS -lvorbisenc"
  VORBIS_LIBS="$VORBIS_LIBS -lvorbis -lm"

  if test "x$vorbis_includes" != "x" ; then
    VORBIS_CFLAGS="-I$vorbis_includes"
  elif test "x$vorbis_prefix" != "x" ; then
    VORBIS_CFLAGS="-I$vorbis_prefix/include"
  elif test "x$prefix" != "xNONE"; then
    VORBIS_CFLAGS="-I$prefix/include"
  fi


  AC_MSG_CHECKING(for Vorbis)
  no_vorbis=""


  if test "x$enable_vorbistest" = "xyes" ; then
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    CFLAGS="$CFLAGS $VORBIS_CFLAGS $OGG_CFLAGS"
    LIBS="$LIBS $VORBIS_LIBS $OGG_LIBS"
dnl
dnl Now check if the installed Vorbis is sufficiently new.
dnl
      rm -f conf.vorbistest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vorbis/codec.h>

int main ()
{
  system("touch conf.vorbistest");
  return 0;
}

],, no_vorbis=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
  fi

  if test "x$no_vorbis" = "x" ; then
     AC_MSG_RESULT(yes)
     ifelse([$1], , :, [$1])     
  else
     AC_MSG_RESULT(no)
     if test -f conf.vorbistest ; then
       :
     else
       echo "*** Could not run Vorbis test program, checking why..."
       CFLAGS="$CFLAGS $VORBIS_CFLAGS"
       LIBS="$LIBS $VORBIS_LIBS $OGG_LIBS"
       AC_TRY_LINK([
#include <stdio.h>
#include <vorbis/codec.h>
],     [ return 0; ],
       [ echo "*** The test program compiled, but did not run. This usually means"
       echo "*** that the run-time linker is not finding Vorbis or finding the wrong"
       echo "*** version of Vorbis. If it is not finding Vorbis, you'll need to set your"
       echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
       echo "*** to the installed location  Also, make sure you have run ldconfig if that"
       echo "*** is required on your system"
       echo "***"
       echo "*** If you have an old version installed, it is best to remove it, although"
       echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
       [ echo "*** The test program failed to compile or link. See the file config.log for the"
       echo "*** exact error that occured. This usually means Vorbis was incorrectly installed"
       echo "*** or that you have moved Vorbis since it was installed." ])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
     VORBIS_CFLAGS=""
     VORBIS_LIBS=""
     VORBISFILE_LIBS=""
     VORBISENC_LIBS=""
     ifelse([$2], , :, [$2])
  fi
  AC_SUBST(VORBIS_CFLAGS)
  AC_SUBST(VORBIS_LIBS)
  AC_SUBST(VORBISFILE_LIBS)
  AC_SUBST(VORBISENC_LIBS)
  rm -f conf.vorbistest
])


dnl AM_PATH_CURL([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl Test for libcurl, and define CURL_CFLAGS and CURL_LIBS
dnl
AC_DEFUN(AM_PATH_CURL,
[dnl 
dnl Get the cflags and libraries
dnl
AC_ARG_WITH(curl,[  --with-curl=PFX   Prefix where libcurl is installed (optional)], curl_prefix="$withval", curl_prefix="")
AC_ARG_WITH(curl-libraries,[  --with-curl-libraries=DIR   Directory where libcurl library is installed (optional)], curl_libraries="$withval", curl_libraries="")
AC_ARG_WITH(curl-includes,[  --with-curl-includes=DIR   Directory where libcurl header files are installed (optional)], curl_includes="$withval", curl_includes="")
AC_ARG_ENABLE(curltest, [  --disable-curltest       Do not try to compile and run a test libcurl program],, enable_curltest=yes)

if test "x$curl_prefix" != "xno" ; then

  if test "x$curl_libraries" != "x" ; then
    CURL_LIBS="-L$curl_libraries"
  elif test "x$curl_prefix" != "x" ; then
    CURL_LIBS="-L$curl_prefix/lib"
  elif test "x$prefix" != "xNONE" ; then
    CURL_LIBS="-L$prefix/lib"
  fi

  CURL_LIBS="$CURL_LIBS -lcurl"

  if test "x$curl_includes" != "x" ; then
    CURL_CFLAGS="-I$curl_includes"
  elif test "x$curl_prefix" != "x" ; then
    CURL_CFLAGS="-I$curl_prefix/include"
  elif test "x$prefix" != "xNONE"; then
    CURL_CFLAGS="-I$prefix/include"
  fi

  AC_MSG_CHECKING(for libcurl)
  no_curl=""


  if test "x$enable_curltest" = "xyes" ; then
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    CFLAGS="$CFLAGS $CURL_CFLAGS"
    LIBS="$LIBS $CURL_LIBS"
dnl
dnl Now check if the installed libcurl is sufficiently new.
dnl
      rm -f conf.curltest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

int main ()
{
  system("touch conf.curltest");
  return 0;
}

],, no_curl=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
  fi

  if test "x$no_curl" = "x" ; then
     AC_MSG_RESULT(yes)
     ifelse([$1], , :, [$1])     
  else
     AC_MSG_RESULT(no)
     if test -f conf.curltest ; then
       :
     else
       echo "*** Could not run libcurl test program, checking why..."
       CFLAGS="$CFLAGS $CURL_CFLAGS"
       LIBS="$LIBS $CURL_LIBS"
       AC_TRY_LINK([
#include <stdio.h>
#include <curl/curl.h>
],     [ return 0; ],
       [ echo "*** The test program compiled, but did not run. This usually means"
       echo "*** that the run-time linker is not finding libcurl or finding the wrong"
       echo "*** version of libcurl. If it is not finding libcurl, you'll need to set your"
       echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
       echo "*** to the installed location  Also, make sure you have run ldconfig if that"
       echo "*** is required on your system"
       echo "***"
       echo "*** If you have an old version installed, it is best to remove it, although"
       echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
       [ echo "*** The test program failed to compile or link. See the file config.log for the"
       echo "*** exact error that occured. This usually means libcurl was incorrectly installed"
       echo "*** or that you have moved libcurl since it was installed." ])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
     CURL_CFLAGS=""
     CURL_LIBS=""
     ifelse([$2], , :, [$2])
  fi
  AC_DEFINE(HAVE_CURL, 1, [Define if you have libcurl.])
else
  CURL_CFLAGS=""
  CURL_LIBS=""
fi
  AC_SUBST(CURL_CFLAGS)
  AC_SUBST(CURL_LIBS)
  rm -f conf.curltest
])

AC_DEFUN([AM_PATH_MAD],
[
AC_ARG_WITH(mad-prefix,[  --with-mad-prefix=PFX   Prefix where libmad is installed (optional)],
            mad_prefix="$withval", mad_prefix="")
AC_ARG_ENABLE(madtest, [  --disable-madtest       Do not try to compile and run the libmad test program],
   , enable_madtest=yes)

  MAD_LIBS="-lmad"
  MAD_CFLAGS=""
  if test -n "$mad_prefix"; then
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$mad_prefix/lib"
    MAD_CFLAGS="-I$mad_prefix/include"
    MAD_LIBS="-L$mad_prefix/lib $MAD_LIBS"
  fi

  min_mad_version=ifelse([$1], , 0.12.0, $1)
  AC_MSG_CHECKING([for libmad (>= $min_mad_version)])
  if test -n "$enable_madtest"; then
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    CFLAGS="$CFLAGS $MAD_CFLAGS"
    LIBS="$MAD_LIBS $LIBS"
dnl
dnl Now check if the installed madlib is sufficiently new. 
dnl
    AC_TRY_RUN([
#include <mad.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, patch;
  char *tmp_version;

  tmp_version = strdup("$min_mad_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &patch) != 3) {
     printf("%s, bad version string\n", "$min_mad_version");
     exit(1);
   }

   if (    (MAD_VERSION_MAJOR > major) 
       || ((MAD_VERSION_MAJOR == major) && (MAD_VERSION_MINOR > minor)) 
       || ((MAD_VERSION_MAJOR == major) && (MAD_VERSION_MINOR == minor) && (MAD_VERSION_PATCH >= patch)))
     {
       return 0;
     }
   else
     {
       printf("\n*** An old version of libmad (%s) was found.\n", MAD_VERSION);
       printf("*** This program requires at least version %s. The latest version of\n", "$min_mad_version");
       printf("*** libmad is always available from http://mad.sourceforge.net/\n");
     }
   return 1;
}
]
, dnl do nothing if TRY_RUN worked
, dnl define mad_failure if TRY_RUN failed
  mad_failure=yes
,
[
echo $ac_n "cross compiling; cannot test version... $ac_c"
          AC_TRY_LINK([#include <stdio.h>
                       #include <stdlib.h>
                       #include <mad.h>],
                      [ return MAD_VERSION_MAJOR; ],,
                      [ mad_failure=yes ]) 
])

    CFLAGS="$ac_save_CFLAGS"
    LIBS="$ac_save_LIBS"
  fi

  dnl
  dnl See is the program failed
  dnl
  if test "$mad_failure" = "yes"; then
     AC_MSG_RESULT(no)
     CFLAGS="$ac_save_CFLAGS"
     LIBS="$ac_save_LIBS"
     MAD_CFLAGS=""
     MAD_LIBS=""
     ifelse([$2], , :, [$2])
  else
     AC_MSG_RESULT(yes)
     AC_DEFINE(HAVE_MAD_H)
     AC_DEFINE(HAVE_LIBMAD)
     ifelse([$1], , :, [$1])
  fi

  AC_SUBST(MAD_CFLAGS)
  AC_SUBST(MAD_LIBS)
])

dnl AM_PATH_LAME([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl Test for liblame, and define LAME_CFLAGS and LAME_LIBS
dnl
AC_DEFUN(AM_PATH_LAME,
[dnl 
dnl Get the cflags and libraries
dnl
AC_ARG_WITH(lame,[  --with-lame=PFX   Prefix where liblame is installed (optional)], lame_prefix="$withval", lame_prefix="")
AC_ARG_WITH(lame-libraries,[  --with-lame-libraries=DIR   Directory where liblame library is installed (optional)], lame_libraries="$withval", lame_libraries="")
AC_ARG_WITH(lame-includes,[  --with-lame-includes=DIR   Directory where liblame header files are installed (optional)], lame_includes="$withval", lame_includes="")
AC_ARG_ENABLE(lametest, [  --disable-lametest       Do not try to compile and run a test liblame program],, enable_lametest=yes)

if test "x$lame_prefix" != "xno" ; then

  if test "x$lame_libraries" != "x" ; then
    LAME_LIBS="-L$lame_libraries"
  elif test "x$lame_prefix" != "x" ; then
    LAME_LIBS="-L$lame_prefix/lib"
  elif test "x$prefix" != "xNONE" ; then
    LAME_LIBS="-L$prefix/lib"
  fi

  LAME_LIBS="$LAME_LIBS -lmp3lame -lm"

  if test "x$lame_includes" != "x" ; then
    LAME_CFLAGS="-I$lame_includes"
  elif test "x$lame_prefix" != "x" ; then
    LAME_CFLAGS="-I$lame_prefix/include"
  elif test "x$prefix" != "xNONE"; then
    LAME_CFLAGS="-I$prefix/include"
  fi

  AC_MSG_CHECKING(for liblame)
  no_lame=""


  if test "x$enable_lametest" = "xyes" ; then
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    CFLAGS="$CFLAGS $LAME_CFLAGS"
    LIBS="$LIBS $LAME_LIBS"
dnl
dnl Now check if the installed liblame is sufficiently new.
dnl
      rm -f conf.lametest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lame/lame.h>

int main ()
{
  system("touch conf.lametest");
  return 0;
}

],, no_lame=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
  fi

  if test "x$no_lame" = "x" ; then
     AC_MSG_RESULT(yes)
     ifelse([$1], , :, [$1])     
  else
     AC_MSG_RESULT(no)
     if test -f conf.lametest ; then
       :
     else
       echo "*** Could not run liblame test program, checking why..."
       CFLAGS="$CFLAGS $LAME_CFLAGS"
       LIBS="$LIBS $LAME_LIBS"
       AC_TRY_LINK([
#include <stdio.h>
#include <lame/lame.h>
],     [ return 0; ],
       [ echo "*** The test program compiled, but did not run. This usually means"
       echo "*** that the run-time linker is not finding liblame or finding the wrong"
       echo "*** version of liblame. If it is not finding liblame, you'll need to set your"
       echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
       echo "*** to the installed location  Also, make sure you have run ldconfig if that"
       echo "*** is required on your system"
       echo "***"
       echo "*** If you have an old version installed, it is best to remove it, although"
       echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
       [ echo "*** The test program failed to compile or link. See the file config.log for the"
       echo "*** exact error that occured. This usually means liblame was incorrectly installed"
       echo "*** or that you have moved liblame since it was installed." ])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
     LAME_CFLAGS=""
     LAME_LIBS=""
     ifelse([$2], , :, [$2])
  fi
  AC_DEFINE(HAVE_LAME, 1, [Define if you have liblame.])
else
  LAME_CFLAGS=""
  LAME_LIBS=""
fi
  AC_SUBST(LAME_CFLAGS)
  AC_SUBST(LAME_LIBS)
  rm -f conf.lametest
])

AC_DEFUN(AM_OPTIONS_WXCONFIG,
[
   AC_ARG_WITH(wx-prefix, [  --with-wx-prefix=PREFIX   Prefix where wxWindows is installed (optional)],
               wx_config_prefix="$withval", wx_config_prefix="")
   AC_ARG_WITH(wx-exec-prefix,[  --with-wx-exec-prefix=PREFIX Exec prefix where wxWindows is installed (optional)],
               wx_config_exec_prefix="$withval", wx_config_exec_prefix="")
   AC_ARG_WITH(wx-exec-prefix,[  --with-wx-config=CONFIG   wx-config script to use (optional)],
               wx_config_name="$withval", wx_config_name="")
])

dnl ---------------------------------------------------------------------------
dnl AM_PATH_WXCONFIG(VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
dnl Test for wxWindows, and define WX_C*FLAGS, WX_LIBS and WX_LIBS_STATIC
dnl (the latter is for static linking against wxWindows). Set WX_CONFIG_NAME
dnl environment variable to override the default name of the wx-config script
dnl to use. Set WX_CONFIG_PATH to specify the full path to wx-config - in this
dnl case the macro won't even waste time on tests for its existence.
dnl ---------------------------------------------------------------------------

dnl
dnl Get the cflags and libraries from the wx-config script
dnl
AC_DEFUN(AM_PATH_WXCONFIG,
[
  dnl do we have wx-config name: it can be wx-config or wxd-config or ...
  if test x${WX_CONFIG_NAME+set} != xset ; then
     WX_CONFIG_NAME=wx-config
  fi
  if test "x$wx_config_name" != x ; then
     WX_CONFIG_NAME="$wx_config_name"
  fi

  dnl deal with optional prefixes
  if test x$wx_config_exec_prefix != x ; then
     wx_config_args="$wx_config_args --exec-prefix=$wx_config_exec_prefix"
     if test x${WX_CONFIG_NAME+set} != xset ; then
        WX_CONFIG_PATH=$wx_config_exec_prefix/bin/$WX_CONFIG_NAME
     fi
  fi
  if test x$wx_config_prefix != x ; then
     wx_config_args="$wx_config_args --prefix=$wx_config_prefix"
     if test x${WX_CONFIG_NAME+set} != xset ; then
        WX_CONFIG_PATH=$wx_config_prefix/bin/$WX_CONFIG_NAME
     fi
  fi

  dnl don't search the PATH if we already have the full name
  if test "x$WX_CONFIG_PATH" = "x" ; then
    AC_PATH_PROG(WX_CONFIG_PATH, $WX_CONFIG_NAME, no)
  fi

  if test "$WX_CONFIG_PATH" != "no" ; then
    WX_VERSION=""
    no_wx=""

    min_wx_version=ifelse([$1], ,2.2.1,$1)
    AC_MSG_CHECKING(for wxWindows version >= $min_wx_version)

    WX_CONFIG_WITH_ARGS="$WX_CONFIG_PATH $wx_config_args"

    WX_VERSION=`$WX_CONFIG_WITH_ARGS --version`
    wx_config_major_version=`echo $WX_VERSION | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    wx_config_minor_version=`echo $WX_VERSION | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    wx_config_micro_version=`echo $WX_VERSION | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    wx_requested_major_version=`echo $min_wx_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    wx_requested_minor_version=`echo $min_wx_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    wx_requested_micro_version=`echo $min_wx_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    wx_ver_ok=""
    if test $wx_config_major_version -gt $wx_requested_major_version; then
      wx_ver_ok=yes
    else
      if test $wx_config_major_version -eq $wx_requested_major_version; then
         if test $wx_config_minor_version -gt $wx_requested_minor_version; then
            wx_ver_ok=yes
         else
            if test $wx_config_minor_version -eq $wx_requested_minor_version; then
               if test $wx_config_micro_version -ge $wx_requested_micro_version; then
                  wx_ver_ok=yes
               fi
            fi
         fi
      fi
    fi

    if test "x$wx_ver_ok" = x ; then
      no_wx=yes
    else
      WX_LIBS=`$WX_CONFIG_WITH_ARGS --libs`
     dnl WX_LIBS_STATIC=`$WX_CONFIG_WITH_ARGS --static --libs`

      dnl starting with version 2.2.6 wx-config has --cppflags argument
      wx_has_cppflags=""
      if test $wx_config_major_version -gt 2; then
        wx_has_cppflags=yes
      else
        if test $wx_config_major_version -eq 2; then
           if test $wx_config_minor_version -gt 2; then
              wx_has_cppflags=yes
           else
              if test $wx_config_minor_version -eq 2; then
                 if test $wx_config_micro_version -ge 6; then
                    wx_has_cppflags=yes
                 fi
              fi
           fi
        fi
      fi

      if test "x$wx_has_cppflags" = x ; then
         dnl no choice but to define all flags like CFLAGS
         WX_CFLAGS=`$WX_CONFIG_WITH_ARGS --cflags`
         WX_CPPFLAGS=$WX_CFLAGS
         WX_CXXFLAGS=$WX_CFLAGS

         WX_CFLAGS_ONLY=$WX_CFLAGS
         WX_CXXFLAGS_ONLY=$WX_CFLAGS
      else
         dnl we have CPPFLAGS included in CFLAGS included in CXXFLAGS
         WX_CPPFLAGS=`$WX_CONFIG_WITH_ARGS --cppflags`
         WX_CXXFLAGS=`$WX_CONFIG_WITH_ARGS --cxxflags`
         WX_CFLAGS=`$WX_CONFIG_WITH_ARGS --cflags`

         WX_CFLAGS_ONLY=`echo $WX_CFLAGS | sed "s@^$WX_CPPFLAGS *@@"`
         WX_CXXFLAGS_ONLY=`echo $WX_CXXFLAGS | sed "s@^$WX_CFLAGS *@@"`
      fi
    fi

    if test "x$no_wx" = x ; then
       AC_MSG_RESULT(yes (version $WX_VERSION))
       ifelse([$1], , :, [$1])
    else
       if test "x$WX_VERSION" = x; then
dnl no wx-config at all
  AC_MSG_RESULT(no)
       else
  AC_MSG_RESULT(no (version $WX_VERSION is not new enough))
       fi

       WX_CFLAGS=""
       WX_CPPFLAGS=""
       WX_CXXFLAGS=""
       WX_LIBS=""
       WX_LIBS_STATIC=""
       ifelse([$2], , :, [$2])
    fi
  fi

  AC_SUBST(WX_CPPFLAGS)
  AC_SUBST(WX_CFLAGS)
  AC_SUBST(WX_CXXFLAGS)
  AC_SUBST(WX_CFLAGS_ONLY)
  AC_SUBST(WX_CXXFLAGS_ONLY)
  AC_SUBST(WX_LIBS)
  AC_SUBST(WX_LIBS_STATIC)
  AC_SUBST(WX_VERSION)
])
