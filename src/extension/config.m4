PHP_ARG_ENABLE(nfilin_fuzzy_match, Enable fuzzy match support)

if test "$PHP_NFILIN_FUZZY_MATCH" = "yes"; then

   PHP_REQUIRE_CXX()
   PHP_SUBST(NFILIN_FUZZY_MATCH_SHARED_LIBADD)
   PHP_ADD_LIBRARY(stdc++, 1, NFILIN_FUZZY_MATCH_SHARED_LIBADD)

   AC_DEFINE(HAVE_NFILIN_FUZZY_MATCH, 1, [You have NecroFilin's fuzzy match extension])
   PHP_NEW_EXTENSION(nfilin_fuzzy_match, php_levenshtein.cpp levenshtein.cpp, $ext_shared)

fi
