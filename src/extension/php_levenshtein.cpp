#include <string>
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

PHP_METHOD (Levenshtein, __get);

PHP_METHOD (Levenshtein, __set);

PHP_METHOD (Levenshtein, __call);

PHP_METHOD (Levenshtein, getStorage);

PHP_METHOD (Levenshtein, setStorage);
/* ############################################################################################## */
#if ZEND_MODULE_API_NO < 20151012

void levenshtein_store_dtor(void *object, zend_object_handle handle TSRMLS_DC) 
{
    levenshtein_object *obj = (levenshtein_object *) object;
    zend_objects_destroy_object((zend_object *) object, handle TSRMLS_CC);
    FREE_HASHTABLE(obj->std.properties);
    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, NULL, 0);
}

void levenshtein_free_storage(void *object TSRMLS_DC) 
{
    levenshtein_object *obj = (levenshtein_object *) object;
    delete obj->levenshtein;
    obj->levenshtein = NULL;
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value storage_create_handler(zend_class_entry *type TSRMLS_DC) 
{
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

zend_object_value costs_create_handler(zend_class_entry *type TSRMLS_DC) 
{
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


HashTable *levenshtein_object_get_properties(zval *object TSRMLS_DC)
{    
    HashTable *props = zend_std_get_properties(object TSRMLS_DC);
    Levenshtein *levenshtein;
    levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
    levenshtein = obj->levenshtein;

    zval *val;
    MAKE_STD_ZVAL(val);
    val = levenshtein_get_distance(levenshtein, object);
    zend_hash_add(props, "distance", sizeof("distance"), &val, sizeof(zval*), NULL);
    MAKE_STD_ZVAL(val);
    val = levenshtein_get_path(levenshtein, object);
    zend_hash_add(props, "path", sizeof("path"), &val, sizeof(zval*), NULL);
    MAKE_STD_ZVAL(val);
    val = levenshtein_get_searches(levenshtein, object);
    zend_hash_add(props, "searches", sizeof("searches"), &val, sizeof(zval*), NULL);
    MAKE_STD_ZVAL(val);
    val = levenshtein_get_blocks(levenshtein, object);
    zend_hash_add(props, "blocks", sizeof("blocks"), &val, sizeof(zval*), NULL);
    return props;
}

HashTable *levenshtein_object_get_debug_info(zval *object, int *is_temp TSRMLS_DC)
{
    HashTable *props = zend_std_get_debug_info(object, is_temp TSRMLS_DC);
    return props;
}

#else

zend_object *storage_create_handler(zend_class_entry *type) 
{
    storage_object *intern;
    intern = (storage_object *) ecalloc(1, sizeof(storage_object) + zend_object_properties_size(type));
    zend_object_std_init(&intern->std, type);
    object_properties_init(&intern->std, type);
    intern->std.handlers = &storage_object_handlers;
    return &intern->std;
}

zend_object *costs_create_handler(zend_class_entry *type ) 
{
    costs_object *intern;
    intern = (costs_object *) ecalloc(1, sizeof(costs_object) + zend_object_properties_size(type));
    zend_object_std_init(&intern->std, type);
    object_properties_init(&intern->std, type);
    intern->std.handlers = &costs_object_handlers;
    return &intern->std;
}

zend_object *levenshtein_create_handler(zend_class_entry *type) 
{
    levenshtein_object *intern;
    intern = (levenshtein_object *) ecalloc(1, sizeof(levenshtein_object) + zend_object_properties_size(type));
    zend_object_std_init(&intern->std, type);
    object_properties_init(&intern->std, type);
    intern->std.handlers = &levenshtein_object_handlers;
    return &intern->std;
}

HashTable *levenshtein_object_get_properties(zval *object)
{    
    HashTable *props = zend_std_get_properties(object);

    Levenshtein *levenshtein = Z_LEVENSHTEINOBJ_P(object)->levenshtein;

    zval *val = (zval*) ecalloc(1, sizeof(zval));
    val = levenshtein_get_distance(levenshtein, object);
    zend_hash_add(props, ZVAL_FROM_CSTR("distance"), val);
    val = levenshtein_get_path(levenshtein, object);
    zend_hash_add(props, ZVAL_FROM_CSTR("path"), val);
    val = levenshtein_get_searches(levenshtein, object);
    zend_hash_add(props, ZVAL_FROM_CSTR("searches"), val);
    val = levenshtein_get_blocks(levenshtein, object);
    zend_hash_add(props, ZVAL_FROM_CSTR("blocks"), val);

    return props;
}

HashTable *levenshtein_object_get_debug_info(zval *object, int *is_temp){
    // printf("%s\n", "levenshtein_object_get_debug_info");    
    HashTable *props = zend_std_get_debug_info(object, is_temp);
    
    return props;
}


static inline levenshtein_object *levenshtein_object_from_obj(zend_object *obj)
{
    return (levenshtein_object*)((char*)(obj) - XtOffsetOf(levenshtein_object, std));
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

ZEND_BEGIN_ARG_INFO_EX(arginfo__call, 0, 0, 2)
                ZEND_ARG_INFO(0, name)
                ZEND_ARG_INFO(0, arguments)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, 1, 0)
               ZEND_ARG_INFO(0, string)
               ZEND_ARG_INFO(0, pattern)
               ZEND_ARG_INFO(0, cost_ins)
               ZEND_ARG_INFO(0, cost_rep)
               ZEND_ARG_INFO(0, cost_del)
ZEND_END_ARG_INFO()
/* #############################   Method list ########################################### */
const zend_function_entry levenshtein_methods[] = {
        PHP_ME(Levenshtein, __construct, arginfo__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
        PHP_ME(Levenshtein, __destruct, NULL, ZEND_ACC_PUBLIC)
        PHP_ME(Levenshtein, __get, arginfo__get, ZEND_ACC_PUBLIC)
        PHP_ME(Levenshtein, __set, arginfo__set, ZEND_ACC_PUBLIC)
        PHP_ME(Levenshtein, __call, arginfo__call, ZEND_ACC_PUBLIC)
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
PHP_MINIT_FUNCTION (nfilin_fuzzy_match) 
{
    zend_class_entry lce;
    INIT_NS_CLASS_ENTRY(lce, "Nfilin\\Libs", "Levenshtein", levenshtein_methods);

#if ZEND_MODULE_API_NO < 20151012 
    levenshtein_ce = zend_register_internal_class(&lce TSRMLS_CC);
    (*levenshtein_ce).create_object = levenshtein_create_handler;
    memcpy(&levenshtein_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    levenshtein_object_handlers.clone_obj = NULL;
    storage_object_handlers.clone_obj = NULL;
    costs_object_handlers.clone_obj = NULL;
    levenshtein_object_handlers.get_properties = levenshtein_object_get_properties;    
    levenshtein_object_handlers.get_debug_info = levenshtein_object_get_debug_info;
#else
    levenshtein_ce = zend_register_internal_class(&lce);   
    (*levenshtein_ce).create_object = levenshtein_create_handler;
    memcpy(&levenshtein_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    levenshtein_object_handlers.offset = XtOffsetOf(levenshtein_object, std);
    levenshtein_object_handlers.get_properties = levenshtein_object_get_properties;    
    levenshtein_object_handlers.get_debug_info = levenshtein_object_get_debug_info;
#endif 
    
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


#if ZEND_MODULE_API_NO < 20151012 
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
#else
    zend_declare_property_null(levenshtein_ce, "storage", sizeof("storage") - 1, ZEND_ACC_PRIVATE );
    zend_declare_property_null(levenshtein_ce, "costs", sizeof("costs") - 1, ZEND_ACC_PUBLIC );

    zend_declare_property_double(storage_ce, "cost_ins", sizeof("cost_ins") - 1, 1, ZEND_ACC_PUBLIC);
    zend_declare_property_double(storage_ce, "cost_rep", sizeof("cost_rep") - 1, 1, ZEND_ACC_PUBLIC);
    zend_declare_property_double(storage_ce, "cost_del", sizeof("cost_del") - 1, 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(storage_ce, "pattern", sizeof("pattern") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(storage_ce, "string", sizeof("string") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(storage_ce, "path", sizeof("path") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(storage_ce, "blocks", sizeof("blocks") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(storage_ce, "searches", sizeof("searches") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(storage_ce, "lv", sizeof("lv") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(storage_ce, "match_map", sizeof("match_map") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(storage_ce, "distance", sizeof("distance") - 1, ZEND_ACC_PUBLIC);

    zend_declare_property_double(costs_ce, "insert", sizeof("insert") - 1, 1, ZEND_ACC_PUBLIC);
    zend_declare_property_double(costs_ce, "replace", sizeof("replace") - 1, 1, ZEND_ACC_PUBLIC);
    zend_declare_property_double(costs_ce, "delete", sizeof("delete") - 1, 1, ZEND_ACC_PUBLIC);
#endif


    // levenshtein_object_handlers.clone_obj = NULL;
    // storage_object_handlers.clone_obj = NULL;
    // costs_object_handlers.clone_obj = NULL;
    
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
PHP_METHOD (Levenshtein, __construct) 
{
    Levenshtein *levenshtein = NULL;
    zval *object = getThis();
    zval *string, *pattern, rv;
    double cost_ins = 1, cost_rep = 1, cost_del = 1;
    zval *storage, *arr, *val, *prop, *costs;

#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(string);
    MAKE_STD_ZVAL(pattern);    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|zzddd", &pattern, &string, &cost_ins, &cost_rep, &cost_del) == FAILURE) {
        RETURN_NULL();
    }

    levenshtein = new Levenshtein();

    levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    costs = ZEND_READ_PROPERTY(levenshtein_ce, object, "costs");

    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "pattern", val);
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "path", val);
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "blocks", val);
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "searches", val);
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "lv", val);
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "match_map", val);

    // MAKE_STD_ZVAL(arr);
    // MAKE_STD_ZVAL(val);
    // object_init_ex(arr, storage_ce);
    // array_init(val);
    // zend_update_property(storage_ce, arr, "path", sizeof("path") - 1, val TSRMLS_DC);
    // MAKE_STD_ZVAL(val);
    // array_init(val);
    // zend_update_property(storage_ce, arr, "blocks", sizeof("blocks") - 1, val TSRMLS_DC);
    // MAKE_STD_ZVAL(val);
    // array_init(val);
    // zend_update_property(storage_ce, arr, "searches", sizeof("searches") - 1, val TSRMLS_DC);
    // MAKE_STD_ZVAL(val);
    // array_init(val);
    // zend_update_property(storage_ce, arr, "lv", sizeof("lv") - 1, val TSRMLS_DC);
    // MAKE_STD_ZVAL(val);
    // array_init(val);
    // zend_update_property(storage_ce, arr, "match_map", sizeof("match_map") - 1, val TSRMLS_DC);
    zend_update_property_null(storage_ce, arr, "distance", sizeof("distance") - 1 TSRMLS_DC);
    zend_update_property_double(storage_ce, arr, "cost_ins", sizeof("cost_ins") - 1, cost_ins  TSRMLS_DC);
    zend_update_property_double(storage_ce, arr, "cost_del", sizeof("cost_del") - 1, cost_del  TSRMLS_DC);
    zend_update_property_double(storage_ce, arr, "cost_rep", sizeof("cost_rep") - 1, cost_rep  TSRMLS_DC);
    zend_update_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, arr TSRMLS_DC);

    arr = NULL;
    INIT_ZOBJECT(costs_ce, arr);
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
#else
    string = (zval*)ecalloc(1, sizeof(zval));
    pattern = (zval*)ecalloc(1, sizeof(zval));
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|zzddd", &pattern, &string, &cost_ins, &cost_rep, &cost_del) == FAILURE) {
        RETURN_NULL();
    }

    levenshtein = new Levenshtein();    

    levenshtein_object *obj = Z_LEVENSHTEINOBJ_P(object);

    obj->levenshtein = levenshtein;

    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    costs = ZEND_READ_PROPERTY(levenshtein_ce, object, "costs");

    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "pattern", val);
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "path", val);
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "blocks", val);
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "searches", val);
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "lv", val);
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "match_map", val);

    zend_update_property_null(storage_ce, arr, "distance", sizeof("distance") - 1);
    zend_update_property_double(storage_ce, arr, "cost_ins", sizeof("cost_ins") - 1, cost_ins);
    zend_update_property_double(storage_ce, arr, "cost_del", sizeof("cost_del") - 1, cost_del);
    zend_update_property_double(storage_ce, arr, "cost_rep", sizeof("cost_rep") - 1, cost_rep);
    
    zend_update_property(levenshtein_ce, object, "storage", sizeof("storage") - 1, arr);
    
    arr = NULL;
    INIT_ZOBJECT(costs_ce, arr);
    zend_update_property_double(costs_ce, arr, "insert", sizeof("insert") - 1, cost_ins);
    zend_update_property_double(costs_ce, arr, "delete", sizeof("delete") - 1, cost_del);
    zend_update_property_double(costs_ce, arr, "replace", sizeof("replace") - 1, cost_rep);
    zend_update_property(levenshtein_ce, object, "costs", sizeof("costs") - 1, arr);

    if (Z_TYPE_P(string) == IS_STRING) {
        zend_update_property(levenshtein_ce, object, "string", sizeof("string") - 1, string);
    }

    if (Z_TYPE_P(pattern) == IS_STRING) {
        levenshtein->setPattern(Z_STRVAL_P(pattern));
        levenshtein_update_pattern(levenshtein, object);
    } else if (Z_TYPE_P(pattern) == IS_ARRAY) {
        levenshtein->setPattern(Z_ARRVAL_P(pattern));
        levenshtein_update_pattern(levenshtein, object);
    }    
#endif
    
}

// PHP_METHOD (Levenshtein, setString) {

//     Levenshtein *levenshtein;
//     zval *object = getThis();
// #if ZEND_MODULE_API_NO < 20151012
//     levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
// #else
//     levenshtein_object *obj = Z_LEVENSHTEINOBJ_P(object);
// #endif
//     levenshtein = obj->levenshtein;
//     if (levenshtein == NULL)
//         zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
//     char *str = NULL;
//     int str_len = 0;
//     if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {

//         RETURN_NULL();
//     }
//     levenshtein->setString(str);

// #if ZEND_MODULE_API_NO < 20151012    
//     if (str != NULL) {
//         zend_update_property_string(levenshtein_ce, object, "string", sizeof("string") - 1, str TSRMLS_DC);
//     }
// #endif    

//     *return_value = *object;
//     zval_copy_ctor(return_value);
// }

// PHP_METHOD (Levenshtein, getString) {

//     Levenshtein *levenshtein;
//     zval *object = getThis();
// #if ZEND_MODULE_API_NO < 20151012
//     levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
// #else
//     levenshtein_object *obj = Z_LEVENSHTEINOBJ_P(object);
// #endif
//     levenshtein = obj->levenshtein;
//     if (levenshtein == NULL)
//         zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
//     zval *str;
// #if ZEND_MODULE_API_NO < 20151012
//     str = zend_read_property(levenshtein_ce, object, "string", sizeof("string") - 1, 1 TSRMLS_CC);
// #else
//     zval rv;
//     str = zend_read_property(levenshtein_ce, object, "string", sizeof("string") - 1, 1, &rv);
// #endif
//     RETURN_ZVAL(str, 1, 0);
// }

// PHP_METHOD (Levenshtein, setPattern) {

//     Levenshtein *levenshtein;
//     zval *object = getThis();
// #if ZEND_MODULE_API_NO < 20151012
//     levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
// #else
//     levenshtein_object *obj = Z_LEVENSHTEINOBJ_P(object);
// #endif
//     levenshtein = obj->levenshtein;
//     if (levenshtein == NULL)
//         zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
//     char *str;
//     int str_len;
//     if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {

//         RETURN_NULL();
//     }
//     levenshtein->setPattern(str);
//     levenshtein_update_pattern(levenshtein, object);
//     *return_value = *object;
//     levenshtein->flushProcessed();
//     zval_copy_ctor(return_value);
// }

// PHP_METHOD (Levenshtein, getPattern) {

//     Levenshtein *levenshtein;
//     zval *object = getThis();
// #if ZEND_MODULE_API_NO < 20151012
//     levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
// #else
//     levenshtein_object *obj = Z_LEVENSHTEINOBJ_P(object);
// #endif
//     levenshtein = obj->levenshtein;
//     if (levenshtein == NULL)
//         zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
//     if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {

//         RETURN_NULL();
//     }
//     zval *pattern = levenshtein->getPattern();
//     *return_value = *pattern;
//     zval_copy_ctor(return_value);
// }

void levenshtein_update_pattern(Levenshtein *levenshtein, zval *object) {

    zval *storage, *prop, *pattern;
#if ZEND_MODULE_API_NO < 20151012
    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    pattern = levenshtein->getPattern();
    zend_update_property(levenshtein_ce, object, "pattern", sizeof("pattern") - 1, pattern TSRMLS_CC);
    zend_update_property(storage_ce, storage, "pattern", sizeof("pattern") - 1, pattern TSRMLS_CC);
#else
    zval rv;
    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    pattern = levenshtein->getPattern();
    zend_update_property(levenshtein_ce, object, "pattern", sizeof("pattern") - 1, pattern);
    zend_update_property(storage_ce, storage, "pattern", sizeof("pattern") - 1, pattern);
#endif
    
}

void levenshtein_reset_pattern(Levenshtein *levenshtein, zval *object) {

    zval *storage, *prop, *pattern;

    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    pattern = ZEND_READ_PROPERTY(storage_ce, storage, "pattern");

    levenshtein->clearPattern();
    switch (Z_TYPE_P(pattern)) {

        case IS_ARRAY:
            levenshtein->setPattern(Z_ARRVAL_P(pattern), 0);
            break;
    }
    pattern = ZEND_READ_PROPERTY(storage_ce, storage, "pattern");
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
            ZVAL_NULL(val);
    }
}

void levenshtein_reset_data_from_storage(Levenshtein *levenshtein, zval *object) 
{

    levenshtein_reset_pattern(levenshtein, object);
    zval *storage, *prop, *val, *costs;
    char flush = 0;
    
    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    val = ZEND_READ_PROPERTY(storage_ce, storage, "distance");
    if (Z_TYPE_P(val) == IS_DOUBLE) {

        levenshtein->setDistance(Z_DVAL_P(val));
    } else {
        levenshtein->flushProcessed();
        flush = 1;
    }

    val = ZEND_READ_PROPERTY(storage_ce, storage, "cost_ins");

    if (Z_TYPE_P(val) == IS_DOUBLE) {
        levenshtein->setCostIns(Z_DVAL_P(val));
    } else {
        levenshtein->setCostIns(1);
        levenshtein->flushProcessed();
        flush = 1;
    }

    val = ZEND_READ_PROPERTY(storage_ce, storage, "cost_rep");

    if (Z_TYPE_P(val) == IS_DOUBLE) {
        levenshtein->setCostRep(Z_DVAL_P(val));
    } else {
        levenshtein->setCostRep(1);
        levenshtein->flushProcessed();
        flush = 1;
    }

    val = ZEND_READ_PROPERTY(storage_ce, storage, "cost_del");

    if (Z_TYPE_P(val) == IS_DOUBLE) {
        levenshtein->setCostDel(Z_DVAL_P(val));
    } else {
        levenshtein->setCostDel(1);
        levenshtein->flushProcessed();
        flush = 1;
    }

    costs = ZEND_READ_PROPERTY(levenshtein_ce, object, "costs");
    val = ZEND_READ_PROPERTY(costs_ce, costs, "insert");

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

    val = ZEND_READ_PROPERTY(costs_ce, costs, "replace");

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

    val = ZEND_READ_PROPERTY(costs_ce, costs, "replace");

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
        levenshtein->setLV(ZEND_READ_PROPERTY(storage_ce, storage, "lv"));
        levenshtein->setMap(ZEND_READ_PROPERTY(storage_ce, storage, "match_map"));
        levenshtein->setPath(ZEND_READ_PROPERTY(storage_ce, storage, "path"));
        levenshtein->setBlocks(ZEND_READ_PROPERTY(storage_ce, storage, "blocks"));
        levenshtein->setSearches(ZEND_READ_PROPERTY(storage_ce, storage, "searches"));
    }
}

void levenshtein_flush_processed(Levenshtein *levenshtein, zval *object) {

    levenshtein->flushProcessed();

#if ZEND_MODULE_API_NO < 20151012
    zval *storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");

    zend_update_property_null(storage_ce, storage, "path", sizeof("path") - 1 TSRMLS_DC);
    zend_update_property_null(storage_ce, storage, "blocks", sizeof("blocks") - 1 TSRMLS_DC);
    zend_update_property_null(storage_ce, storage, "searches", sizeof("searches") - 1 TSRMLS_DC);
    zend_update_property_null(storage_ce, storage, "lv", sizeof("lv") - 1 TSRMLS_DC);
    zend_update_property_null(storage_ce, storage, "match_map", sizeof("match_map") - 1 TSRMLS_DC);
    zend_update_property_null(storage_ce, storage, "distance", sizeof("distance") - 1 TSRMLS_DC);
#else    
    zval *storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");

    zend_update_property_null(storage_ce, storage, "path", sizeof("path") - 1);
    zend_update_property_null(storage_ce, storage, "blocks", sizeof("blocks") - 1);
    zend_update_property_null(storage_ce, storage, "searches", sizeof("searches") - 1);
    zend_update_property_null(storage_ce, storage, "lv", sizeof("lv") - 1);
    zend_update_property_null(storage_ce, storage, "match_map", sizeof("match_map") - 1);
    zend_update_property_null(storage_ce, storage, "distance", sizeof("distance") - 1);
#endif

}

void update_storage_property(Levenshtein *levenshtein, zval *object, char *property, zval *value) {
#if ZEND_MODULE_API_NO < 20151012
    zval *storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");    
    zend_update_property(storage_ce, storage, property, strlen(property), value TSRMLS_DC);
#else    
    zval *storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");    
    zend_update_property(storage_ce, storage, property, strlen(property), value);
#endif    
}


zval *levenshtein_get_distance(Levenshtein *levenshtein, zval *object) {
    zval *string, *pattern, *ret;
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(ret);
    zval *storage, *prop, *val;
    string = ZEND_READ_PROPERTY(levenshtein_ce, object, "string");
    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    
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
#else
    ret = (zval*)ecalloc(1, sizeof(zval));
    ZVAL_NULL(ret);
    zval *storage, *prop, *val;
    zval rv;
    string = ZEND_READ_PROPERTY(levenshtein_ce, object, "string");
    // printf("GET DIST FOR: `%s`\n", Z_STRVAL_P(string));
    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    
    if (Z_TYPE_P(string) == IS_NULL) {

        zend_update_property(storage_ce, storage, "distance", sizeof("distance") - 1, ret TSRMLS_CC);
        return ret;
    }
    levenshtein->setString(Z_STRVAL_P(string));    
    levenshtein_reset_data_from_storage(levenshtein, object);
    
    ZVAL_DOUBLE(ret, levenshtein->getDistance());
    zend_update_property(storage_ce, storage, "distance", sizeof("distance") - 1, ret);
    
    zval *lv = levenshtein->getLV();

    
    zend_update_property(storage_ce, storage, "lv", sizeof("lv") - 1, lv);
    
    zend_update_property(storage_ce, storage, "match_map", sizeof("match_map") - 1, levenshtein->getMap());
#endif
       
    
    
    return ret;
}

zval *levenshtein_get_path(Levenshtein *levenshtein, zval *object) {
    zval *distance, *ret, *blocks, *searches;
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(ret);
    MAKE_STD_ZVAL(blocks);
    MAKE_STD_ZVAL(searches);
    zval *storage, *prop, *val;
    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
#else
    ret = (zval*)ecalloc(1, sizeof(zval));
    blocks = (zval*)ecalloc(1, sizeof(zval));
    searches = (zval*)ecalloc(1, sizeof(zval));
    zval *storage, *prop, *val;
    zval rv;
    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
#endif
    
    distance = levenshtein_get_distance(levenshtein, object);
    // printf("DIST TYPE: %d : %d\n", Z_TYPE_P(distance), IS_DOUBLE);
    if (Z_TYPE_P(distance) != IS_DOUBLE) {
        zend_update_property(storage_ce, storage, "path", sizeof("path") - 1, ret TSRMLS_CC);
        zend_update_property(storage_ce, storage, "blocks", sizeof("blocks") - 1, blocks TSRMLS_CC);
        zend_update_property(storage_ce, storage, "searches", sizeof("searches") - 1, searches TSRMLS_CC);
        return ret;
    }
    // printf("DIST: %g\n", Z_DVAL_P(distance));
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
    MAKE_STD_ZVAL(blocks);
    MAKE_STD_ZVAL(searches);
    zval *storage, *prop, *val;
    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    
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
#else
    ret = (zval*)ecalloc(1, sizeof(zval));
    searches = (zval*)ecalloc(1, sizeof(zval));
    blocks = (zval*)ecalloc(1, sizeof(zval));
    zval *storage, *prop, *val;
    zval rv;
    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    
    distance = levenshtein_get_distance(levenshtein, object);
    if (Z_TYPE_P(distance) != IS_DOUBLE) {
        // printf("%s\n", "----update path");
        zend_update_property(storage_ce, storage, "path", sizeof("path") - 1, ret);
        // printf("%s\n", "----update blocks");
        zend_update_property(storage_ce, storage, "blocks", sizeof("blocks") - 1, blocks);
        // printf("%s\n", "----update searches");
        zend_update_property(storage_ce, storage, "searches", sizeof("searches") - 1, searches);
        // printf("%s\n", "RETURN");
        return blocks;
    }
    ret = levenshtein->getPath();
    blocks = levenshtein->getBlocks();
    searches = levenshtein->getSearches();
    // printf("%s\n", "update block, path, searches");
    zend_update_property(storage_ce, storage, "blocks", sizeof("blocks") - 1, blocks);
    zend_update_property(storage_ce, storage, "path", sizeof("path") - 1, ret);
    zend_update_property(storage_ce, storage, "searches", sizeof("searches") - 1, searches);
#endif
    return blocks;
}

zval *levenshtein_get_searches(Levenshtein *levenshtein, zval *object) {
    // printf("%s\n", "levenshtein_get_searches");
    zval *distance, *ret, *blocks, *searches;
#if ZEND_MODULE_API_NO < 20151012
    MAKE_STD_ZVAL(ret);
    MAKE_STD_ZVAL(searches);
    zval *storage, *prop, *val;
    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
#else
    ret = (zval*)ecalloc(1, sizeof(zval));
    searches = (zval*)ecalloc(1, sizeof(zval));
    zval *storage, *prop, *val;
    zval rv;
    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
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

// PHP_METHOD (Levenshtein, getDistance) {

//     Levenshtein *levenshtein;
//     zval *object = getThis();
// #if ZEND_MODULE_API_NO < 20151012
//     levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
// #else
//     levenshtein_object *obj = Z_LEVENSHTEINOBJ_P(object);
// #endif
//     levenshtein = obj->levenshtein;
//     if (levenshtein == NULL)
//         zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
//     if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {

//         RETURN_NULL();
//     }
//     *return_value = *levenshtein_get_distance(levenshtein, object);
//     levenshtein->flushProcessed();
//     zval_copy_ctor(return_value);
// }

zval *levenshtein__get(Levenshtein *levenshtein, zval *object, std::string key){
    zval *ret;
    // printf("__get: %s\n", key.c_str());
    if (key.compare("distance") == 0) {
        ret = levenshtein_get_distance(levenshtein, object);
    } else if (key.compare("path") == 0) {
        ret = levenshtein_get_path(levenshtein, object);
    } else if (key.compare("blocks") == 0) {
        ret = levenshtein_get_blocks(levenshtein, object);
    } else if (key.compare("searches") == 0) {
        ret = levenshtein_get_searches(levenshtein, object);
    } else if (key.compare("pattern") == 0) {
        ret = levenshtein->getPattern();
        // printf("PATT N: %d\n", zend_hash_num_elements(Z_ARRVAL_P(ret)));
    } else if (key.compare("string") == 0){
        ret = levenshtein->getString();
    } else {
        ret = (zval*)ecalloc(1, sizeof(zval));
        ZVAL_NULL(ret);
    }
    levenshtein->flushProcessed();

    // printf("__get: end:  %s\n", key.c_str());
    return ret;
}

void levenshtein__set(Levenshtein *levenshtein, zval *object, std::string key, zval *value){
    zval *ret;
    // printf("__set: %s\n", key.c_str());
    if (key.compare("pattern") == 0) {
        switch(Z_TYPE_P(value)){
            case IS_ARRAY:
                levenshtein->setPattern(Z_ARRVAL_P(value));                
                update_storage_property(levenshtein, object, (char *)"pattern", levenshtein->getPattern());
                levenshtein_flush_processed(levenshtein, object);
                break;                
            case IS_STRING:
                levenshtein->setPattern(Z_STRVAL_P(value));
                update_storage_property(levenshtein, object, (char *)"pattern", levenshtein->getPattern());
                levenshtein_flush_processed(levenshtein, object);
                break;
        }
    } else if (key.compare("string") == 0){
                char buffer[32];
        switch(Z_TYPE_P(value)){

#if ZEND_MODULE_API_NO < 20151012            
            case IS_BOOL:
                levenshtein->setString(Z_BVAL_P(value)?(char*)"TRUE":(char*)"FALSE");
                levenshtein_flush_processed(levenshtein, object);
                break;
#else            
            case IS_FALSE:
                levenshtein->setString((char*)"FALSE");
                levenshtein_flush_processed(levenshtein, object);
                break;
            case IS_TRUE:
                levenshtein->setString((char*)"TRUE");
                levenshtein_flush_processed(levenshtein, object);
                break;
#endif                
            case IS_LONG:
                snprintf(buffer, sizeof(buffer), "%d", Z_LVAL_P(value));
                levenshtein->setString(buffer);
                levenshtein_flush_processed(levenshtein, object);
                break;            
            case IS_DOUBLE:
                snprintf(buffer, sizeof(buffer), "%g", Z_DVAL_P(value));
                levenshtein->setString(buffer);
                levenshtein_flush_processed(levenshtein, object);
                break;
            case IS_STRING:
                levenshtein->setString(Z_STRVAL_P(value));;
                levenshtein_flush_processed(levenshtein, object);
                break;
        }
    }

    // printf("__set: end:  %s\n", key.c_str());
}

PHP_METHOD (Levenshtein, __get) {

    Levenshtein *levenshtein;
    zval *object = getThis();
#if ZEND_MODULE_API_NO < 20151012
    levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
        RETURN_NULL();
    }
#else
    levenshtein_object *obj = Z_LEVENSHTEINOBJ_P(object);
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0);
        RETURN_NULL();
    }
#endif

    
    char *key;
    int key_len;
    std::string stdKey = "";
#if ZEND_MODULE_API_NO < 20151012    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
        RETURN_NULL();
    }
    stdKey.assign(key, key_len);
#else
    zend_string *zKey;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &zKey) == FAILURE) {
        RETURN_NULL();
    }

    
    key = (*zKey).val;
    key_len = (*zKey).len;
    stdKey.append(key, key_len);
    // printf("GET %s\n", key);
#endif    
    
    // printf("__GET: %s\n", stdKey.c_str());  
    *return_value = *levenshtein__get(levenshtein, object, stdKey);
    zval_copy_ctor(return_value);
    // printf("END __get\n");
}

PHP_METHOD (Levenshtein, __set) {
    Levenshtein *levenshtein;
    zval *object = getThis();
    std::string sKey = "";
#if ZEND_MODULE_API_NO < 20151012
    levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL)
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
    char *key;
    int key_len;
    zval *val;
    MAKE_STD_ZVAL(val);
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &key, &key_len, &val) == FAILURE) {
        RETURN_NULL();
    }
    sKey.assign(key, key_len);
#else
    levenshtein_object *obj = Z_LEVENSHTEINOBJ_P(object);
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL)
        zend_throw_exception(zend_exception_get_default(), "Levenshtein object not initialized", 0);
    zend_string *zKey;
    zval *val;
    val = (zval*)ecalloc(1, sizeof(zval));
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Sz", &zKey, &val) == FAILURE) {
        RETURN_NULL();
    }
    sKey.assign((*zKey).val, (*zKey).len);
#endif
    // printf("__set: %s\n", sKey.c_str());
    levenshtein__set(levenshtein, object, sKey, val);
    // printf("__set: END:  %s\n", sKey.c_str());
}

PHP_METHOD (Levenshtein, __destruct) {

    zval *object = getThis();
    zval *val, *prop;
#if ZEND_MODULE_API_NO < 20151012
    val = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");

    if (Z_TYPE_P(val) == IS_OBJECT){
        MAKE_STD_ZVAL(prop);
        ZVAL_STRING(prop, "pattern", 1);
        Z_OBJ_HT_P(val)->unset_property(val, prop, NULL);
        ZVAL_STRING(prop, "path", 1);
        Z_OBJ_HT_P(val)->unset_property(val, prop, NULL);
        ZVAL_STRING(prop, "blocks", 1);
        Z_OBJ_HT_P(val)->unset_property(val, prop, NULL);
        ZVAL_STRING(prop, "lv", 1);
        Z_OBJ_HT_P(val)->unset_property(val, prop, NULL);
        ZVAL_STRING(prop, "match_map", 1);
        Z_OBJ_HT_P(val)->unset_property(val, prop, NULL);
        ZVAL_NULL(val);
        val = ZEND_READ_PROPERTY(levenshtein_ce, object, "pattern");
        ZVAL_NULL(val);
        val = ZEND_READ_PROPERTY(levenshtein_ce, object, "costs");
        ZVAL_NULL(val);
    }    
#else
    zval rv;
    val = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    if (Z_TYPE_P(val) == IS_OBJECT){
        prop = (zval*)ecalloc(1, sizeof(zval));
        ZVAL_STRING(prop, "pattern");
        Z_OBJ_HT_P(val)->unset_property(val, prop, NULL);
        ZVAL_STRING(prop, "path");
        Z_OBJ_HT_P(val)->unset_property(val, prop, NULL);
        ZVAL_STRING(prop, "blocks");
        Z_OBJ_HT_P(val)->unset_property(val, prop, NULL);
        ZVAL_STRING(prop, "lv");
        Z_OBJ_HT_P(val)->unset_property(val, prop, NULL);
        ZVAL_STRING(prop, "match_map");
        Z_OBJ_HT_P(val)->unset_property(val, prop, NULL);
        ZVAL_NULL(val);
        val = ZEND_READ_PROPERTY(levenshtein_ce, object, "pattern");
        ZVAL_NULL(val);
        val = ZEND_READ_PROPERTY(levenshtein_ce, object, "costs");
        ZVAL_NULL(val);
    }
#endif

}

PHP_METHOD (Levenshtein, getStorage) {

    zval *object = getThis();
    zval *val;
    val = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    *return_value = *val;
    zval_copy_ctor(return_value);
}

void levenshtein_set_string(Levenshtein *levenshtein, std::string str){  
    // printf("%s\n", "levenshtein_set_string");  
    char *c_str = (char *) ecalloc(1, sizeof(char) * (str.length() + 1));
    strncpy(c_str, str.c_str(), str.length());
    c_str[str.length()] = '\0';
    levenshtein->setString(c_str);
}


PHP_METHOD (Levenshtein, __call) {

    Levenshtein *levenshtein;
    zval *object = getThis();

    std::string sName = "";
    HashTable *zArguments;
    ALLOC_HASHTABLE(zArguments);

#if ZEND_MODULE_API_NO < 20151012
    levenshtein_object *obj = (levenshtein_object *) zend_object_store_get_object(object TSRMLS_CC);
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL)
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
    char *name;
    int name_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sh", &name, &name_len, zArguments) == FAILURE) {

        RETURN_NULL();
    }

    sName.assign(name, name_len);
#else

    levenshtein_object *obj = Z_LEVENSHTEINOBJ_P(object);
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL)
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Levenshtein object not initialized", 0 TSRMLS_CC);
    zend_string *zName;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Sh", &zName, &zArguments) == FAILURE) {
        RETURN_NULL();        
    }
    sName.assign((*zName).val, (*zName).len);
#endif    
    
    // printf("__call: %s\n", sName.c_str());
    // printf("%d\n", zend_hash_num_elements(zArguments)) ;
    if (sName.substr(0,3).compare("get") == 0){
        std::string param = sName.substr(3);
        param[0] = tolower(param[0]);
        // printf("_C__GET: %s\n", param.c_str());  

        
        *return_value = *levenshtein__get(levenshtein, object, param);
        zval_copy_ctor(return_value);

    } else if (sName.substr(0,3).compare("set") == 0) {
        std::string param = sName.substr(3);
        param[0] = tolower(param[0]);
        // printf("_C__SET: %s\n", param.c_str());   
        if(zend_hash_num_elements(zArguments) > 0) {
            zend_hash_internal_pointer_reset(zArguments);

#if ZEND_MODULE_API_NO < 20151012           
            zval *value;
            MAKE_STD_ZVAL(value)
            zend_hash_get_current_data(zArguments, (void**)&value);
#else            
            zval *value = zend_hash_get_current_data(zArguments);
#endif            
            levenshtein__set(levenshtein, object, param, value);
        }

    }
}