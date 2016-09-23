#ifndef PHP_NFILIN_FUZZY_MATCH_H
#define PHP_NFILIN_FUZZY_MATCH_H

#define PHP_NFILIN_FUZZY_MATCH_EXTNAME  "nfilin_fuzzy_match"
#define PHP_NFILIN_FUZZY_MATCH_EXTVER   "1.0.0"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include "php.h"
#include "zend_exceptions.h"

#ifdef ZTS
#include "TSRM.h"
#endif
}

extern zend_error_handling levenshtein_original_error_handling;
extern zend_module_entry levenshtein_module_entry;
extern zend_object_handlers levenshtein_object_handlers;
extern zend_object_handlers storage_object_handlers;
extern zend_object_handlers costs_object_handlers;
extern zend_class_entry *levenshtein_ce_exception;
extern zend_class_entry *storage_ce_exception;
extern zend_class_entry *costs_ce_exception;
extern zend_class_entry *levenshtein_ce;
extern zend_class_entry *storage_ce;
extern zend_class_entry *costs_ce;

extern void Z2DOUBLE(zval *val);

#ifdef PHP_WIN32
    #define PHP_NFILIN_FUZZY_MATCH_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
    #define PHP_NFILIN_FUZZY_MATCH_API __attribute__ ((visibility("default")))
#else
    #define PHP_NFILIN_FUZZY_MATCH_API
#endif

#include "storage.h"
#include "costs.h"
#include "levenshtein.h"

struct levenshtein_object {
    zend_object std;
    Levenshtein *levenshtein;
};

struct storage_object {
    zend_object std;
    Storage *storage;
};

struct costs_object {
    zend_object std;
    Costs *costs;
};


extern void levenshtein_reset_pattern(Levenshtein *levenshtein, zval *object);

extern void levenshtein_update_pattern(Levenshtein *levenshtein, zval *object);

#endif