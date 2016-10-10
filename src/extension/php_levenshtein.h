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

#if ZEND_MODULE_API_NO < 20151012
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

#define Z_LEVENSHTEINOBJ_P(zv) (levenshtein_object *) zend_object_store_get_object(zv TSRMLS_CC)

#else

struct levenshtein_object {
    Levenshtein *levenshtein;
    zend_object std;
};

struct storage_object {
    Storage *storage;
    zend_object std;
};

struct costs_object {
    Costs *costs;
    zend_object std;
};

static inline levenshtein_object *levenshtein_object_from_obj(zend_object *obj);

#define Z_LEVENSHTEINOBJ_P(zv) levenshtein_object_from_obj(Z_OBJ_P((zv)))


#endif


extern void levenshtein_reset_pattern(Levenshtein *levenshtein, zval *object);

extern void levenshtein_update_pattern(Levenshtein *levenshtein, zval *object);



zval *levenshtein_get_distance(Levenshtein *levenshtein, zval *object);
zval *levenshtein_get_path(Levenshtein *levenshtein, zval *object);
zval *levenshtein_get_blocks(Levenshtein *levenshtein, zval *object);
zval *levenshtein_get_searches(Levenshtein *levenshtein, zval *object);

#if ZEND_MODULE_API_NO < 20151012

#define FN_OBJECT_CREATE_HANDLER(name) \
    zend_object_value name ## _create_handler(zend_class_entry *type TSRMLS_DC) 

#define INIT_OBJECT_CREATE_HANDLER(obj, class_entry, dtor, storage, clone, handlers_p, retval) \
    {\
        zend_object_std_init(&obj->std, class_entry TSRMLS_CC);\
        \
        ALLOC_HASHTABLE(obj->std.properties);\
        zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);\
        object_properties_init(&obj->std, class_entry);\
        (*retval).handle = zend_objects_store_put(obj, dtor, storage, clone TSRMLS_CC);\
        \
        (*retval).handlers = (const zend_object_handlers*)(&handlers_p);\
    }
#define INIT_OBJECT_CREATE_HANDLER_RETURN(obj, retval) \
    retval

#define MAKE_PERSISTENT_ZVAL(val) MAKE_STD_ZVAL(val)

#define ZEND_READ_PROPERTY(class_entry, object, property) zend_read_property(class_entry, object, property, sizeof(property) - 1, 1 TSRMLS_CC)

#define INIT_ZOBJECT(class_entry, val) \
    if(val == NULL) { \
        MAKE_STD_ZVAL(val); \
        object_init_ex(val, class_entry);\
    }

#define FLUSH_OBJECT_PROPERTY_ARRAY(class_entry, object, property, buff) \
    INIT_ZOBJECT(class_entry, object); \
    MAKE_STD_ZVAL(buff); \
    array_init(buff); \
    zend_update_property(class_entry, object, property, sizeof(property) - 1, buff TSRMLS_DC)

#define ZEND_HASH_GET_CURRENT_DATA(ht, val)\
        MAKE_STD_ZVAL(val); \
        zend_hash_get_current_data(ht, (void**)val)
#define ZVAL_STRING_EX(val, str) \
        ZVAL_STRING(prop, "blocks", 1);

#else

#define FN_OBJECT_CREATE_HANDLER(name) \
    zend_object* name##_create_handler(zend_class_entry *type) 

#define INIT_OBJECT_CREATE_HANDLER(obj, class_entry, dtor, storage, clone, handlers, retval) \
    {\
        zend_object_std_init(&obj->std, class_entry TSRMLS_CC);\
        \
        object_properties_init(&obj->std, class_entry);\
        \
        obj->std.handlers = &handlers;\
    }

#define INIT_OBJECT_CREATE_HANDLER_RETURN(obj, retval) \
    &obj->std

#define ZEND_READ_PROPERTY(class_entry, object, property) zend_read_property(class_entry, object, property, sizeof(property) - 1, 1, (zval*)ecalloc(1, sizeof(zval)))

#define MAKE_PERSISTENT_ZVAL(val)\
    val = (zval*)ecalloc(1, sizeof(zval)); \
    ZVAL_NULL(val)

#define MAKE_STD_ZVAL(val) \
    if(val != NULL) \
        printf("######FREE %p\n", val); \
        try{\
            efree(val); \
        } catch (int e){\
            \
        }\
    val = (zval*)ecalloc(1, sizeof(zval)); \
    ZVAL_NULL(val)

#define ZVAL_FROM_CSTR(c_str) \
   zend_string_init(c_str, strlen(c_str), 0)

#define INIT_ZOBJECT(class_entry, val) \
    if(val == NULL) { \
        MAKE_STD_ZVAL(val); \
        object_init_ex(val, class_entry);\
    }

#define FLUSH_OBJECT_PROPERTY_ARRAY(class_entry, object, property, buff) \
    printf("FLUSH: %s\n", property); \
    INIT_ZOBJECT(class_entry, object); \
    MAKE_STD_ZVAL(buff); \
    array_init(buff); \
    zend_update_property(class_entry, object, property, sizeof(property) - 1, buff)

#define ZEND_HASH_GET_CURRENT_DATA(ht, val)\
        val = zend_hash_get_current_data(ht)
    
#define ZVAL_STRING_EX(val, str) \
        ZVAL_STRING(prop, "blocks");
#endif

#endif