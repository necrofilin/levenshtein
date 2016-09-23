#include <zend_types.h>
#include "php_levenshtein.h"

zend_object_handlers levenshtein_object_handlers;
zend_object_handlers storage_object_handlers;
zend_object_handlers costs_object_handlers;
zend_class_entry *levenshtein_ce;
zend_class_entry *storage_ce;
zend_class_entry *costs_ce;
/* #############################   Method declaration ########################################### */
PHP_METHOD (Levenshtein, __construct);

PHP_METHOD (Levenshtein, __destruct);

PHP_METHOD (Levenshtein, setString);

PHP_METHOD (Levenshtein, getString);

PHP_METHOD (Levenshtein, setPattern);

PHP_METHOD (Levenshtein, getPattern);

PHP_METHOD (Levenshtein, getDistance);

PHP_METHOD (Levenshtein, getStorage);

PHP_METHOD (Levenshtein, __get);

PHP_METHOD (Levenshtein, __set);
/* ############################################################################################## */
#if ZEND_MODULE_API_NO < 20151012
void levenshtein_store_dtor(void *object, zend_object_handle handle TSRMLS_DC) {
    levenshtein_object *obj = (levenshtein_object *) object;
    zend_objects_destroy_object((zend_object *) object, handle TSRMLS_CC);
    FREE_HASHTABLE(obj->std.properties);
    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, NULL, 0);
}
void levenshtein_free_storage(void *object TSRMLS_DC) {
    levenshtein_object *obj = (levenshtein_object *) object;
    delete obj->levenshtein;
    obj->levenshtein = NULL;
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}
#endif
#if ZEND_MODULE_API_NO < 20151012
zend_object_value storage_create_handler(zend_class_entry *type TSRMLS_DC) {
    zend_object_value retval;
    storage_object *obj = (storage_object *) emalloc(sizeof(storage_object));
    memset(obj, 0, sizeof(storage_object));
    zend_object_std_init(&obj->std, type TSRMLS_CC);
    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);
    retval.handle = zend_objects_store_put(obj, NULL, NULL, NULL TSRMLS_CC);
    retval.handlers = &storage_object_handlers;
    return retval;
}
zend_object_value costs_create_handler(zend_class_entry *type TSRMLS_DC) {
    zend_object_value retval;
    costs_object *obj = (costs_object *) emalloc(sizeof(costs_object));
    memset(obj, 0, sizeof(costs_object));
    zend_object_std_init(&obj->std, type TSRMLS_CC);
    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);
    retval.handle = zend_objects_store_put(obj, NULL, NULL, NULL TSRMLS_CC);
    retval.handlers = &costs_object_handlers;
    return retval;
}
zend_object_value levenshtein_create_handler(zend_class_entry *type TSRMLS_DC) {
    zend_object_value retval;
    levenshtein_object *obj = (levenshtein_object *) emalloc(sizeof(levenshtein_object));
    memset(obj, 0, sizeof(levenshtein_object));
    zend_object_std_init(&obj->std, type TSRMLS_CC);
    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);
    retval.handle = zend_objects_store_put(obj, levenshtein_store_dtor, levenshtein_free_storage, NULL TSRMLS_CC);
    retval.handlers = &levenshtein_object_handlers;
    return retval;
}
#else

zend_object *storage_create_handler(zend_class_entry *type TSRMLS_DC) {
    storage_object *intern;
    intern = (storage_object *) ecalloc(1, sizeof(storage_object) + zend_object_properties_size(type));
    zend_object_std_init(&intern->zo, type);
    object_properties_init(&intern->zo, type);
    intern->zo.handlers = &storage_object_handlers;
    return &intern->zo;
}

zend_object *costs_create_handler(zend_class_entry *type TSRMLS_DC) {
    costs_object *intern;
    intern = (costs_object *) ecalloc(1, sizeof(costs_object) + zend_object_properties_size(type));
    zend_object_std_init(&intern->zo, type);
    object_properties_init(&intern->zo, type);
    intern->zo.handlers = &costs_object_handlers;
    return &intern->zo;
}

zend_object *levenshtein_create_handler(zend_class_entry *type TSRMLS_DC) {
    levenshtein_object *intern;
    intern = (levenshtein_object *) ecalloc(1, sizeof(levenshtein_object) + zend_object_properties_size(type));
    zend_object_std_init(&intern->zo, type);
    object_properties_init(&intern->zo, type);
    intern->zo.handlers = &levenshtein_object_handlers;
    return &intern->zo;
}
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_void, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_set, 0, 0, 1)
                ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo__get, 0, 0, 1)
                ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo__set, 0, 0, 2)
                ZEND_ARG_INFO(0, key)
                ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
//ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, 1, 0)
//                ZEND_ARG_INFO(0, string)
//                ZEND_ARG_INFO(0, pattern)
//                ZEND_ARG_INFO(0, cost_ins)
//                ZEND_ARG_INFO(0, cost_rep)
//                ZEND_ARG_INFO(0, cost_del)
//ZEND_END_ARG_INFO()
/* #############################   Method list ########################################### */
const zend_function_entry levenshtein_methods[] = {
        PHP_ME(Levenshtein, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
        PHP_ME(Levenshtein, __destruct, NULL, ZEND_ACC_PUBLIC)
        PHP_ME(Levenshtein, __get, arginfo__get, ZEND_ACC_PUBLIC)
        PHP_ME(Levenshtein, __set, arginfo__set, ZEND_ACC_PUBLIC)
        PHP_ME(Levenshtein, getStorage, arginfo_void, ZEND_ACC_PUBLIC)
        {NULL, NULL, NULL}
};
const zend_function_entry storage_methods[] = {
        {NULL, NULL, NULL}
};
const zend_function_entry costs_methods[] = {
        {NULL, NULL, NULL}
};
/* ####################################################################################### */
PHP_MINIT_FUNCTION (nfilin_fuzzy_match) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Nfilin\\Libs", "Levenshtein", levenshtein_methods);
    levenshtein_ce = zend_register_internal_class(&ce TSRMLS_CC);
    (*levenshtein_ce).create_object = levenshtein_create_handler;
    memcpy(&levenshtein_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    zend_class_entry sce;
    INIT_NS_CLASS_ENTRY(sce, "Nfilin\\Libs", "Storage", storage_methods);
    storage_ce = zend_register_internal_class(&sce);
    (*storage_ce).create_object = storage_create_handler;
    memcpy(&storage_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    zend_class_entry cce;
    INIT_NS_CLASS_ENTRY(cce, "Nfilin\\Libs", "Costs", costs_methods);
    costs_ce = zend_register_internal_class(&cce);
    (*costs_ce).create_object = costs_create_handler;
    memcpy(&costs_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    zend_declare_property_string(levenshtein_ce, "string", sizeof("string") - 1, "", ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(levenshtein_ce, "pattern", sizeof("pattern") - 1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(levenshtein_ce, "storage", sizeof("storage") - 1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(levenshtein_ce, "costs", sizeof("costs") - 1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_double(storage_ce, "cost_ins", sizeof("cost_ins") - 1, 1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_double(storage_ce, "cost_rep", sizeof("cost_rep") - 1, 1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_double(storage_ce, "cost_del", sizeof("cost_del") - 1, 1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(storage_ce, "pattern", sizeof("pattern") - 1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(storage_ce, "path", sizeof("path") - 1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(storage_ce, "blocks", sizeof("blocks") - 1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(storage_ce, "searches", sizeof("searches") - 1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(storage_ce, "lv", sizeof("lv") - 1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(storage_ce, "match_map", sizeof("match_map") - 1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(storage_ce, "distance", sizeof("distance") - 1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_double(costs_ce, "insert", sizeof("insert") - 1, 1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_double(costs_ce, "replace", sizeof("replace") - 1, 1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_double(costs_ce, "delete", sizeof("delete") - 1, 1, ZEND_ACC_PUBLIC TSRMLS_CC);
    levenshtein_object_handlers.clone_obj = NULL;
    storage_object_handlers.clone_obj = NULL;
    costs_object_handlers.clone_obj = NULL;
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION (nfilin_fuzzy_match) {
    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION (nfilin_fuzzy_match) {
    return SUCCESS;
}

zend_module_entry nfilin_fuzzy_match_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
        STANDARD_MODULE_HEADER,
#endif
        PHP_NFILIN_FUZZY_MATCH_EXTNAME,
        NULL,                               /* Functions */
        PHP_MINIT(nfilin_fuzzy_match),
        PHP_MSHUTDOWN(nfilin_fuzzy_match),  /* MSHUTDOWN */
        NULL,                               /* RINIT */
        PHP_RSHUTDOWN(nfilin_fuzzy_match),  /* RSHUTDOWN */
        NULL,                               /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
        PHP_NFILIN_FUZZY_MATCH_EXTVER,
#endif
        STANDARD_MODULE_PROPERTIES
};
ZEND_GET_MODULE(nfilin_fuzzy_match)
/* #############################   Method implementation ########################################### */
PHP_METHOD (Levenshtein, __construct) {
    Levenshtein *levenshtein = NULL;
    zval *object = getThis();
    zval *string, *pattern, rv;
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(string);
#endif
#if ZEND_MODULE_API_NO < 20151012 
    
    MAKE_STD_ZVAL(pattern);
    
#endif
    double cost_ins = 1, cost_rep = 1, cost_del = 1;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|zzddd", &pattern, &string, &cost_ins, &cost_rep,
                              &cost_del) == FAILURE) {
        RETURN_NULL();
    }
    levenshtein = new Levenshtein();
    zval *storage, *arr, *val, *prop, *costs;
#if ZEND_MODULE_API_NO < 20151012
    levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1 TSRMLS_CC);
    costs = zend_read_property(levenshtein_ce, object, "costs", sizeof("costs") - 1, 1 TSRMLS_CC);
#else
    levenshtein_object *obj = (levenshtein_object *) Z_OBJ_P(object);
    //storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1 TSRMLS_CC);
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1, &rv);
    costs = zend_read_property(levenshtein_ce, object, "costs", sizeof("costs") - 1, 1, &rv);
#endif
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(arr);
#endif
    object_init_ex(arr, storage_ce);
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(val);
#endif
    array_init(val);
    zend_update_property(storage_ce, arr, "pattern", sizeof("pattern") - 1, val TSRMLS_DC);
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(val);
#endif
    array_init(val);
    zend_update_property(storage_ce, arr, "path", sizeof("path") - 1, val TSRMLS_DC);
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(val);
#endif
    array_init(val);
    zend_update_property(storage_ce, arr, "blocks", sizeof("blocks") - 1, val TSRMLS_DC);
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(val);
#endif
    array_init(val);
    zend_update_property(storage_ce, arr, "searches", sizeof("searches") - 1, val TSRMLS_DC);
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(val);
#endif
    array_init(val);
    zend_update_property(storage_ce, arr, "lv", sizeof("lv") - 1, val TSRMLS_DC);
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(val);
#endif
    array_init(val);
    zend_update_property(storage_ce, arr, "match_map", sizeof("match_map") - 1, val TSRMLS_DC);
    zend_update_property_null(storage_ce, arr, "distance", sizeof("distance") - 1 TSRMLS_DC);
    zend_update_property_double(storage_ce, arr, "cost_ins", sizeof("cost_ins") - 1, cost_ins  TSRMLS_DC);
    zend_update_property_double(storage_ce, arr, "cost_del", sizeof("cost_del") - 1, cost_del  TSRMLS_DC);
    zend_update_property_double(storage_ce, arr, "cost_rep", sizeof("cost_rep") - 1, cost_rep  TSRMLS_DC);
    zend_update_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, arr TSRMLS_DC);
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(arr);
#endif
    object_init_ex(arr, costs_ce);
    zend_update_property_double(costs_ce, arr, "insert", sizeof("insert") - 1, cost_ins  TSRMLS_DC);
    zend_update_property_double(costs_ce, arr, "delete", sizeof("delete") - 1, cost_del  TSRMLS_DC);
    zend_update_property_double(costs_ce, arr, "replace", sizeof("replace") - 1, cost_rep  TSRMLS_DC);
    zend_update_property(levenshtein_ce, object, "costs", sizeof("costs") - 1, arr TSRMLS_DC);
    if (Z_TYPE_P(string) == IS_STRING) {
        zend_update_property(levenshtein_ce, object, "string", sizeof("string") - 1, string TSRMLS_DC);
    }
    if (Z_TYPE_P(pattern) == IS_STRING) {
        levenshtein->setPattern(Z_STRVAL_P(pattern));
        levenshtein_update_pattern(levenshtein, object);
    } else if (Z_TYPE_P(pattern) == IS_ARRAY) {
        levenshtein->setPattern(Z_ARRVAL_P(pattern));
        levenshtein_update_pattern(levenshtein, object);
    }
    obj->levenshtein = levenshtein;
}

PHP_METHOD (Levenshtein, setString) {
    Levenshtein *levenshtein;
    zval *object = getThis();
#if ZEND_MODULE_API_NO < 20151012
    levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
#else
    levenshtein_object *obj = (levenshtein_object *) Z_OBJ_P(object);
#endif
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL)
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
    char *str = NULL;
    int str_len = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
        RETURN_NULL();
    }
    levenshtein->setString(str);
    if (str != NULL) {
        zend_update_property_string(levenshtein_ce, object, "string", sizeof("string") - 1, str TSRMLS_DC);
    }
    *return_value = *object;
    zval_copy_ctor(return_value);
}

PHP_METHOD (Levenshtein, getString) {
    Levenshtein *levenshtein;
    zval *object = getThis();
#if ZEND_MODULE_API_NO < 20151012
    levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
#else
    levenshtein_object *obj = (levenshtein_object *) Z_OBJ_P(object);
#endif
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL)
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
    zval *str;
#if ZEND_MODULE_API_NO < 20151012
    str = zend_read_property(levenshtein_ce, object, "string", sizeof("string") - 1, 1 TSRMLS_CC);
#else
    zval rv;
    str = zend_read_property(levenshtein_ce, object, "string", sizeof("string") - 1, 1, &rv);
#endif
    RETURN_ZVAL(str, 1, 0);
}

PHP_METHOD (Levenshtein, setPattern) {
    Levenshtein *levenshtein;
    zval *object = getThis();
#if ZEND_MODULE_API_NO < 20151012
    levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
#else
    levenshtein_object *obj = (levenshtein_object *) Z_OBJ_P(object);
#endif
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL)
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
    char *str;
    int str_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
        RETURN_NULL();
    }
    levenshtein->setPattern(str);
    levenshtein_update_pattern(levenshtein, object);
    *return_value = *object;
    levenshtein->flushProcessed();
    zval_copy_ctor(return_value);
}

PHP_METHOD (Levenshtein, getPattern) {
    Levenshtein *levenshtein;
    zval *object = getThis();
#if ZEND_MODULE_API_NO < 20151012
    levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
#else
    levenshtein_object *obj = (levenshtein_object *) Z_OBJ_P(object);
#endif
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL)
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        RETURN_NULL();
    }
    zval *pattern = levenshtein->getPattern();
    *return_value = *pattern;
    zval_copy_ctor(return_value);
}

void levenshtein_update_pattern(Levenshtein *levenshtein, zval *object) {
    zval *storage, *prop, *pattern;
#if ZEND_MODULE_API_NO < 20151012
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1 TSRMLS_CC);
#else
    zval rv;
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1, &rv);
#endif
    pattern = levenshtein->getPattern();
    zend_update_property(levenshtein_ce, object, "pattern", sizeof("pattern") - 1, pattern TSRMLS_CC);
    zend_update_property(storage_ce, storage, "pattern", sizeof("pattern") - 1, pattern TSRMLS_CC);
}

void levenshtein_reset_pattern(Levenshtein *levenshtein, zval *object) {
    zval *storage, *prop, *pattern;

#if ZEND_MODULE_API_NO < 20151012
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1 TSRMLS_CC);
    pattern = zend_read_property(storage_ce, storage, "pattern", sizeof("pattern") - 1, 1 TSRMLS_CC);
#else
    zval rv;
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1, &rv);
    pattern = zend_read_property(storage_ce, storage, "pattern", sizeof("pattern") - 1, 1, &rv);
#endif

    levenshtein->clearPattern();
    switch (Z_TYPE_P(pattern)) {
        case IS_ARRAY:
            levenshtein->setPattern(Z_ARRVAL_P(pattern), 0);
            break;
    }
#if ZEND_MODULE_API_NO < 20151012
    pattern = zend_read_property(storage_ce, storage, "pattern", sizeof("pattern") - 1, 1 TSRMLS_CC);
#else
    pattern = zend_read_property(storage_ce, storage, "pattern", sizeof("pattern") - 1, 1, &rv);
#endif
    switch (Z_TYPE_P(pattern)) {
        case IS_STRING:
            levenshtein->setPattern(Z_STRVAL_P(pattern));
            levenshtein_update_pattern(levenshtein, object);
            break;
        case IS_ARRAY:
            levenshtein->setPattern(Z_ARRVAL_P(pattern));
            break;
        case IS_NULL:
            HashTable *pat;
            ALLOC_HASHTABLE(pat);
            zend_hash_init(pat, 0, NULL, NULL, 0);
            levenshtein->setPattern(pat);
            levenshtein_update_pattern(levenshtein, object);
            break;
        default:
            levenshtein_update_pattern(levenshtein, object);
            return;
    }
}

void Z2DOUBLE(zval *val) {
    double dval;
    switch (Z_TYPE_P(val)) {
        case IS_LONG:
            dval = (double) Z_LVAL_P(val);
            ZVAL_DOUBLE(val, dval);
            zval_copy_ctor(val);
            break;
        case IS_STRING:
            dval = atof(Z_STRVAL_P(val));
            ZVAL_DOUBLE(val, dval);
            zval_copy_ctor(val);
            break;
        case IS_DOUBLE:
            break;
        default:
#if ZEND_MODULE_API_NO < 20151012
            Z_TYPE_P(val) = IS_NULL;
#else
            val->u1.v.type = IS_NULL;
#endif
    }
}

void levenshtein_reset_data_from_storage(Levenshtein *levenshtein, zval *object) {
    levenshtein_reset_pattern(levenshtein, object);
    zval *storage, *prop, *val, *costs;
    char flush = 0;
#if ZEND_MODULE_API_NO < 20151012
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1 TSRMLS_CC);
    val = zend_read_property(storage_ce, storage, "distance", sizeof("distance") - 1, 1 TSRMLS_CC);
#else
    zval rv;
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1, &rv);
    val = zend_read_property(storage_ce, storage, "distance", sizeof("distance") - 1, 1, &rv);
#endif
    if (Z_TYPE_P(val) == IS_DOUBLE) {
        levenshtein->setDistance(Z_DVAL_P(val));
    } else {
        levenshtein->flushProcessed();
        flush = 1;
    }
#if ZEND_MODULE_API_NO < 20151012
    val = zend_read_property(storage_ce, storage, "cost_ins", sizeof("cost_ins") - 1, 1 TSRMLS_CC);
#else
    val = zend_read_property(storage_ce, storage, "cost_ins", sizeof("cost_ins") - 1, 1, &rv);
#endif
    if (Z_TYPE_P(val) == IS_DOUBLE) {
        levenshtein->setCostIns(Z_DVAL_P(val));
    } else {
        levenshtein->setCostIns(1);
        levenshtein->flushProcessed();
        flush = 1;
    }
#if ZEND_MODULE_API_NO < 20151012
    val = zend_read_property(storage_ce, storage, "cost_rep", sizeof("cost_rep") - 1, 1 TSRMLS_CC);
#else
    val = zend_read_property(storage_ce, storage, "cost_rep", sizeof("cost_rep") - 1, 1, &rv);
#endif
    if (Z_TYPE_P(val) == IS_DOUBLE) {
        levenshtein->setCostRep(Z_DVAL_P(val));
    } else {
        levenshtein->setCostRep(1);
        levenshtein->flushProcessed();
        flush = 1;
    }
#if ZEND_MODULE_API_NO < 20151012
    val = zend_read_property(storage_ce, storage, "cost_del", sizeof("cost_del") - 1, 1 TSRMLS_CC);
#else
    val = zend_read_property(storage_ce, storage, "cost_del", sizeof("cost_del") - 1, 1, &rv);
#endif
    if (Z_TYPE_P(val) == IS_DOUBLE) {
        levenshtein->setCostDel(Z_DVAL_P(val));
    } else {
        levenshtein->setCostDel(1);
        levenshtein->flushProcessed();
        flush = 1;
    }

#if ZEND_MODULE_API_NO < 20151012
    costs = zend_read_property(levenshtein_ce, object, "costs", sizeof("costs") - 1, 1 TSRMLS_CC);
    val = zend_read_property(costs_ce, costs, "insert", sizeof("insert") - 1, 1 TSRMLS_CC);
#else
    costs = zend_read_property(levenshtein_ce, object, "costs", sizeof("costs") - 1, 1, &rv);
    val = zend_read_property(costs_ce, costs, "insert", sizeof("insert") - 1, 1, &rv);
#endif
    Z2DOUBLE(val);
    if (Z_TYPE_P(val) == IS_DOUBLE) {
        if (Z_DVAL_P(val) != levenshtein->getCostIns()) {
            zend_update_property(storage_ce, storage, "cost_ins", sizeof("cost_ins") - 1, val TSRMLS_CC);
            levenshtein->flushProcessed();
            flush = 1;
        }
        levenshtein->setCostIns(Z_DVAL_P(val));
    } else {
        levenshtein->flushProcessed();
        flush = 1;
    }
#if ZEND_MODULE_API_NO < 20151012
    val = zend_read_property(costs_ce, costs, "replace", sizeof("replace") - 1, 1 TSRMLS_CC);
#else
    val = zend_read_property(costs_ce, costs, "replace", sizeof("replace") - 1, 1, &rv);
#endif
    Z2DOUBLE(val);
    if (Z_TYPE_P(val) == IS_DOUBLE) {
        if (Z_DVAL_P(val) != levenshtein->getCostRep()) {
            zend_update_property(storage_ce, storage, "cost_rep", sizeof("cost_rep") - 1, val TSRMLS_CC);
            levenshtein->flushProcessed();
            flush = 1;
        }
        levenshtein->setCostRep(Z_DVAL_P(val));
    } else {
        levenshtein->flushProcessed();
        flush = 1;
    }
#if ZEND_MODULE_API_NO < 20151012
    val = zend_read_property(costs_ce, costs, "replace", sizeof("replace") - 1, 1 TSRMLS_CC);
#else
    val = zend_read_property(costs_ce, costs, "replace", sizeof("replace") - 1, 1, &rv);
#endif
    Z2DOUBLE(val);
    if (Z_TYPE_P(val) == IS_DOUBLE) {
        if (Z_DVAL_P(val) != levenshtein->getCostDel()) {
            zend_update_property(storage_ce, storage, "cost_del", sizeof("cost_del") - 1, val TSRMLS_CC);
            levenshtein->flushProcessed();
            flush = 1;
        }
        levenshtein->setCostDel(Z_DVAL_P(val));
    } else {
        levenshtein->flushProcessed();
        flush = 1;
    }
    if (!flush) {
#if ZEND_MODULE_API_NO < 20151012
        levenshtein->setLV(zend_read_property(storage_ce, storage, "lv", sizeof("lv") - 1, 1 TSRMLS_CC));
        levenshtein->setMap(zend_read_property(storage_ce, storage, "match_map", sizeof("match_map") - 1, 1 TSRMLS_CC));
        levenshtein->setPath(zend_read_property(storage_ce, storage, "path", sizeof("path") - 1, 1 TSRMLS_CC));
        levenshtein->setBlocks(zend_read_property(storage_ce, storage, "blocks", sizeof("blocks") - 1, 1 TSRMLS_CC));
        levenshtein->setSearches(
                zend_read_property(storage_ce, storage, "searches", sizeof("searches") - 1, 1 TSRMLS_CC));
#else
        levenshtein->setLV(zend_read_property(storage_ce, storage, "lv", sizeof("lv") - 1, 1, &rv));
        levenshtein->setMap(zend_read_property(storage_ce, storage, "match_map", sizeof("match_map") - 1, 1, &rv));
        levenshtein->setPath(zend_read_property(storage_ce, storage, "path", sizeof("path") - 1, 1, &rv));
        levenshtein->setBlocks(zend_read_property(storage_ce, storage, "blocks", sizeof("blocks") - 1, 1, &rv));
        levenshtein->setSearches(
                zend_read_property(storage_ce, storage, "searches", sizeof("searches") - 1, 1, &rv));
#endif
    }
}

zval *levenshtein_get_distance(Levenshtein *levenshtein, zval *object) {
    zval *string, *pattern, *ret;
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(ret);
#endif
    zval *storage, *prop, *val;
#if ZEND_MODULE_API_NO < 20151012
    string = zend_read_property(levenshtein_ce, object, "string", sizeof("string") - 1, 1 TSRMLS_CC);
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1 TSRMLS_CC);
#else
    zval rv;
    string = zend_read_property(levenshtein_ce, object, "string", sizeof("string") - 1, 1, &rv);
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1, &rv);
#endif
    if (Z_TYPE_P(string) == IS_NULL) {
        zend_update_property(storage_ce, storage, "distance", sizeof("distance") - 1, ret TSRMLS_CC);
        return ret;
    }
    levenshtein->setString(Z_STRVAL_P(string));
    levenshtein_reset_data_from_storage(levenshtein, object);
    ZVAL_DOUBLE(ret, levenshtein->getDistance());
    zend_update_property(storage_ce, storage, "distance", sizeof("distance") - 1, ret TSRMLS_CC);
    zend_update_property(storage_ce, storage, "lv", sizeof("lv") - 1, levenshtein->getLV() TSRMLS_CC);
    zend_update_property(storage_ce, storage, "match_map", sizeof("match_map") - 1, levenshtein->getMap() TSRMLS_CC);
    return ret;
}

zval *levenshtein_get_path(Levenshtein *levenshtein, zval *object) {
    zval *distance, *ret, *blocks, *searches;
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(ret);
#endif
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(blocks);
#endif
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(searches);
#endif
    zval *storage, *prop, *val;
#if ZEND_MODULE_API_NO < 20151012
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1 TSRMLS_CC);
#else
    zval rv;
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1, &rv);
#endif
    distance = levenshtein_get_distance(levenshtein, object);
    if (Z_TYPE_P(distance) != IS_DOUBLE) {
        zend_update_property(storage_ce, storage, "path", sizeof("path") - 1, ret TSRMLS_CC);
        zend_update_property(storage_ce, storage, "blocks", sizeof("blocks") - 1, blocks TSRMLS_CC);
        zend_update_property(storage_ce, storage, "searches", sizeof("searches") - 1, searches TSRMLS_CC);
        return ret;
    }
    ret = levenshtein->getPath();
    blocks = levenshtein->getBlocks();
    searches = levenshtein->getSearches();
    zend_update_property(storage_ce, storage, "blocks", sizeof("blocks") - 1, blocks TSRMLS_CC);
    zend_update_property(storage_ce, storage, "path", sizeof("path") - 1, ret TSRMLS_CC);
    zend_update_property(storage_ce, storage, "searches", sizeof("searches") - 1, searches TSRMLS_CC);
    return ret;
}

zval *levenshtein_get_blocks(Levenshtein *levenshtein, zval *object) {
    zval *distance, *ret, *blocks, *searches;
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(ret);
#endif
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(searches);
#endif
    zval *storage, *prop, *val;
#if ZEND_MODULE_API_NO < 20151012
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1 TSRMLS_CC);
#else
    zval rv;
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1, &rv);
#endif
    distance = levenshtein_get_distance(levenshtein, object);
    if (Z_TYPE_P(distance) != IS_DOUBLE) {
        zend_update_property(storage_ce, storage, "path", sizeof("path") - 1, ret TSRMLS_CC);
        zend_update_property(storage_ce, storage, "blocks", sizeof("blocks") - 1, blocks TSRMLS_CC);
        zend_update_property(storage_ce, storage, "searches", sizeof("searches") - 1, searches TSRMLS_CC);
        return blocks;
    }
    ret = levenshtein->getPath();
    blocks = levenshtein->getBlocks();
    searches = levenshtein->getSearches();
    zend_update_property(storage_ce, storage, "blocks", sizeof("blocks") - 1, blocks TSRMLS_CC);
    zend_update_property(storage_ce, storage, "path", sizeof("path") - 1, ret TSRMLS_CC);
    zend_update_property(storage_ce, storage, "searches", sizeof("searches") - 1, searches TSRMLS_CC);
    return blocks;
}

zval *levenshtein_get_searches(Levenshtein *levenshtein, zval *object) {
    zval *distance, *ret, *blocks, *searches;
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(ret);
#endif
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(searches);
#endif
    zval *storage, *prop, *val;

#if ZEND_MODULE_API_NO < 20151012
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1 TSRMLS_CC);
#else
    zval rv;
    storage = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 1, &rv);
#endif
    distance = levenshtein_get_distance(levenshtein, object);
    if (Z_TYPE_P(distance) != IS_DOUBLE) {
        zend_update_property(storage_ce, storage, "path", sizeof("path") - 1, ret TSRMLS_CC);
        zend_update_property(storage_ce, storage, "blocks", sizeof("blocks") - 1, blocks TSRMLS_CC);
        zend_update_property(storage_ce, storage, "searches", sizeof("searches") - 1, searches TSRMLS_CC);
        return searches;
    }
    ret = levenshtein->getPath();
    blocks = levenshtein->getBlocks();
    searches = levenshtein->getSearches();
    zend_update_property(storage_ce, storage, "blocks", sizeof("blocks") - 1, blocks TSRMLS_CC);
    zend_update_property(storage_ce, storage, "path", sizeof("path") - 1, ret TSRMLS_CC);
    zend_update_property(storage_ce, storage, "searches", sizeof("searches") - 1, searches TSRMLS_CC);
    return searches;
}

PHP_METHOD (Levenshtein, getDistance) {
    Levenshtein *levenshtein;
    zval *object = getThis();
#if ZEND_MODULE_API_NO < 20151012
    levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
#else
    levenshtein_object *obj = (levenshtein_object *) Z_OBJ_P(object);
#endif
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL)
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
        RETURN_NULL();
    }
    *return_value = *levenshtein_get_distance(levenshtein, object);
    levenshtein->flushProcessed();
    zval_copy_ctor(return_value);
}

PHP_METHOD (Levenshtein, __get) {
    Levenshtein *levenshtein;
    zval *object = getThis();
#if ZEND_MODULE_API_NO < 20151012
    levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
#else
    levenshtein_object *obj = (levenshtein_object *) Z_OBJ_P(object);
#endif
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL)
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
    char *key;
    int key_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
        RETURN_NULL();
    }
    if (key_len == 8 && strncmp(key, "distance", 8) == 0) {
        *return_value = *levenshtein_get_distance(levenshtein, object);
        zval_copy_ctor(return_value);
    } else if (key_len == 4 && strncmp(key, "path", 4) == 0) {
        *return_value = *levenshtein_get_path(levenshtein, object);
        zval_copy_ctor(return_value);
    } else if (key_len == 6 && strncmp(key, "blocks", 6) == 0) {
        *return_value = *levenshtein_get_blocks(levenshtein, object);
        zval_copy_ctor(return_value);
    } else if (key_len == 8 && strncmp(key, "searches", 8) == 0) {
        *return_value = *levenshtein_get_searches(levenshtein, object);
        zval_copy_ctor(return_value);
    } else {
        RETURN_NULL();
    }
    levenshtein->flushProcessed();
}

PHP_METHOD (Levenshtein, __set) {
    Levenshtein *levenshtein;
    zval *object = getThis();
#if ZEND_MODULE_API_NO < 20151012
    levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
#else
    levenshtein_object *obj = (levenshtein_object *) Z_OBJ_P(object);
#endif
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL)
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
    char *key;
    int key_len;
    zval *val;
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(val);
#endif
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &key, &key_len, &val) == FAILURE) {
        RETURN_NULL();
    }
}

PHP_METHOD (Levenshtein, __destruct) {
    zval *object = getThis();
    zval *val, *prop;
#if ZEND_MODULE_API_NO < 20151012
    val = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 0 TSRMLS_CC);
#else
    zval rv;
    val = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 0, &rv);
#endif
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(prop);
#endif

#if ZEND_MODULE_API_NO < 20151012
    ZVAL_STRING(prop, "pattern", 1);
#else
    ZVAL_STRING(prop, "pattern");
#endif

    Z_OBJ_HT_P(val)->unset_property(val, prop, NULL);
#if ZEND_MODULE_API_NO < 20151012
    ZVAL_STRING(prop, "path", 1);
#else
    ZVAL_STRING(prop, "path");
#endif
    Z_OBJ_HT_P(val)->unset_property(val, prop, NULL);
#if ZEND_MODULE_API_NO < 20151012
    ZVAL_STRING(prop, "blocks", 1);
#else
    ZVAL_STRING(prop, "blocks");
#endif
    Z_OBJ_HT_P(val)->unset_property(val, prop, NULL);
#if ZEND_MODULE_API_NO < 20151012
    ZVAL_STRING(prop, "lv", 1);
#else
    ZVAL_STRING(prop, "lv");
#endif
    Z_OBJ_HT_P(val)->unset_property(val, prop, NULL);
#if ZEND_MODULE_API_NO < 20151012
    ZVAL_STRING(prop, "match_map", 1);
#else
    ZVAL_STRING(prop, "match_map");
#endif
    Z_OBJ_HT_P(val)->unset_property(val, prop, NULL);
    ZVAL_NULL(val);

#if ZEND_MODULE_API_NO < 20151012
    val = zend_read_property(levenshtein_ce, object, "pattern", sizeof("pattern") - 1, 0 TSRMLS_CC);
#else
    val = zend_read_property(levenshtein_ce, object, "pattern", sizeof("pattern") - 1, 0, &rv);
#endif
    ZVAL_NULL(val);

#if ZEND_MODULE_API_NO < 20151012
    val = zend_read_property(levenshtein_ce, object, "costs", sizeof("costs") - 1, 0 TSRMLS_CC);
#else
    val = zend_read_property(levenshtein_ce, object, "costs", sizeof("costs") - 1, 0, &rv);
#endif
    ZVAL_NULL(val);
}

PHP_METHOD (Levenshtein, getStorage) {
    zval *object = getThis();
    zval *val;
#if ZEND_MODULE_API_NO < 20151012
    val = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 0 TSRMLS_CC);
#else
    zval rv;
    val = zend_read_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, 0, &rv);
#endif
    *return_value = *val;
    zval_copy_ctor(return_value);
}