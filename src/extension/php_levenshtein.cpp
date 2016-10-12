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
/* ############################################################################################## */

#ifdef ZEND_ENGINE_2

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
#endif

#ifdef ZEND_ENGINE_3

static inline levenshtein_object *levenshtein_object_from_obj(zend_object *obj)
{
    return (levenshtein_object*)((char*)(obj) - XtOffsetOf(levenshtein_object, std));
}

#endif

HashTable *levenshtein_object_get_properties(zval *object TSRMLS_DC)
{    
    HashTable *props;
    zval *val, *zProps;
    zval *prop;
    MAKE_STD_ZVAL(zProps);
    array_init(zProps);
    props =  Z_ARRVAL_P(zProps);
    levenshtein_object *obj = Z_LEVENSHTEINOBJ_P(object);
    Levenshtein *levenshtein = obj->levenshtein;

    ZEND_HASH_ADD_U(props, "string", levenshtein->getString());
    ZEND_HASH_ADD_U(props, "pattern", levenshtein->getPattern());
    ZEND_HASH_ADD_U(props, "distance", levenshtein_get_distance(levenshtein, object));
    ZEND_HASH_ADD_U(props, "path", levenshtein_get_path(levenshtein, object));
    ZEND_HASH_ADD_U(props, "searches", levenshtein_get_searches(levenshtein, object));
    ZEND_HASH_ADD_U(props, "blocks", levenshtein_get_blocks(levenshtein, object));

    return props;
}

HashTable *levenshtein_object_get_debug_info(zval *object, int *is_temp TSRMLS_DC)
{
    HashTable *props = zend_std_get_debug_info(object, is_temp TSRMLS_DC);
    return props;
}

FN_OBJECT_CREATE_HANDLER(storage)
{
    OBJECT_CREATE_HANDLER_RETURN_TYPE() retval;
    MEMSET_OBJECT(storage, obj, type);
    INIT_OBJECT_CREATE_HANDLER(obj, type, NULL, NULL, NULL, storage_object_handlers, &retval);
    return INIT_OBJECT_CREATE_HANDLER_RETURN(obj, retval);
}

FN_OBJECT_CREATE_HANDLER(costs)
{
    OBJECT_CREATE_HANDLER_RETURN_TYPE() retval;
    MEMSET_OBJECT(costs, obj, type);
    INIT_OBJECT_CREATE_HANDLER(obj, type, NULL, NULL, NULL, costs_object_handlers, &retval);
    return INIT_OBJECT_CREATE_HANDLER_RETURN(obj, retval);
}

FN_OBJECT_CREATE_HANDLER(levenshtein)
{
    OBJECT_CREATE_HANDLER_RETURN_TYPE() retval;
    MEMSET_OBJECT(levenshtein, obj, type);
    INIT_OBJECT_CREATE_HANDLER(obj, type, levenshtein_store_dtor, levenshtein_free_storage, NULL, levenshtein_object_handlers, &retval);
    return INIT_OBJECT_CREATE_HANDLER_RETURN(obj, retval);
}

/* ###########################   Argument descriptions  ################################## */
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
    INIT_NS_CLASS(levenshtein, "Nfilin\\Libs", "Levenshtein");
    levenshtein_object_handlers.get_properties = levenshtein_object_get_properties;
    levenshtein_object_handlers.get_debug_info = levenshtein_object_get_debug_info;
#ifdef ZEND_ENGINE_3
    levenshtein_object_handlers.offset = XtOffsetOf(levenshtein_object, std);
#endif

    INIT_NS_CLASS(storage, "Nfilin\\Libs", "Storage");

    INIT_NS_CLASS(costs, "Nfilin\\Libs", "Costs");

    ZEND_DECLARE_PROPERTY_NULL(levenshtein_ce, "storage",  ZEND_ACC_PRIVATE);
    ZEND_DECLARE_PROPERTY_NULL(levenshtein_ce, "costs",  ZEND_ACC_PUBLIC);

    ZEND_DECLARE_PROPERTY_DOUBLE(storage_ce, "cost_ins", 1, ZEND_ACC_PUBLIC);
    ZEND_DECLARE_PROPERTY_DOUBLE(storage_ce, "cost_rep", 1, ZEND_ACC_PUBLIC);
    ZEND_DECLARE_PROPERTY_DOUBLE(storage_ce, "cost_del", 1, ZEND_ACC_PUBLIC);
    ZEND_DECLARE_PROPERTY_NULL(storage_ce, "pattern",  ZEND_ACC_PUBLIC);
    ZEND_DECLARE_PROPERTY_NULL(storage_ce, "path",  ZEND_ACC_PUBLIC);
    ZEND_DECLARE_PROPERTY_NULL(storage_ce, "blocks",  ZEND_ACC_PUBLIC);
    ZEND_DECLARE_PROPERTY_NULL(storage_ce, "searches",  ZEND_ACC_PUBLIC);
    ZEND_DECLARE_PROPERTY_NULL(storage_ce, "lv",  ZEND_ACC_PUBLIC);
    ZEND_DECLARE_PROPERTY_NULL(storage_ce, "match_map",  ZEND_ACC_PUBLIC);
    ZEND_DECLARE_PROPERTY_NULL(storage_ce, "distance",  ZEND_ACC_PUBLIC);

    ZEND_DECLARE_PROPERTY_DOUBLE(costs_ce, "insert", 1, ZEND_ACC_PUBLIC);
    ZEND_DECLARE_PROPERTY_DOUBLE(costs_ce, "replace", 1, ZEND_ACC_PUBLIC);
    ZEND_DECLARE_PROPERTY_DOUBLE(costs_ce, "delete", 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION (nfilin_fuzzy_match)
{
    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION (nfilin_fuzzy_match)
{
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

    MAKE_PERSISTENT_ZVAL(string);
    MAKE_PERSISTENT_ZVAL(pattern);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|zzddd", &pattern, &string, &cost_ins, &cost_rep, &cost_del) == FAILURE) {
        RETURN_NULL();
    }

    levenshtein = new Levenshtein();

    levenshtein_object *obj = Z_LEVENSHTEINOBJ_P(object);

    obj->levenshtein = levenshtein;

    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    if (Z_TYPE_P(storage) != IS_OBJECT) {
        object_init_ex(storage, storage_ce);
    }
    costs = ZEND_READ_PROPERTY(levenshtein_ce, object, "costs");
    if (Z_TYPE_P(costs) != IS_OBJECT) {
        object_init_ex(costs, costs_ce);
    }
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "pattern", val);
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "path", val);
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "blocks", val);
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "searches", val);
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "lv", val);
    FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, arr, "match_map", val);

    ZEND_UPDATE_PROPERTY_NULL(storage_ce, arr, "distance");
    ZEND_UPDATE_PROPERTY_DOUBLE(storage_ce, arr, "cost_ins", cost_ins);
    ZEND_UPDATE_PROPERTY_DOUBLE(storage_ce, arr, "cost_del", cost_del);
    ZEND_UPDATE_PROPERTY_DOUBLE(storage_ce, arr, "cost_rep", cost_rep);

    ZEND_UPDATE_PROPERTY(levenshtein_ce, object, "storage", storage);

    arr = NULL;
    INIT_ZOBJECT(costs_ce, arr);
    ZEND_UPDATE_PROPERTY_DOUBLE(costs_ce, arr, "insert", cost_ins);
    ZEND_UPDATE_PROPERTY_DOUBLE(costs_ce, arr, "delete", cost_del);
    ZEND_UPDATE_PROPERTY_DOUBLE(costs_ce, arr, "replace", cost_rep);
    ZEND_UPDATE_PROPERTY(levenshtein_ce, object, "costs", arr);

    if (Z_TYPE_P(string) == IS_STRING) {
        ZEND_UPDATE_PROPERTY(levenshtein_ce, object, "string", string);
    }

    if (Z_TYPE_P(pattern) == IS_STRING) {
        levenshtein->setPattern(Z_STRVAL_P(pattern));
        levenshtein_update_pattern(levenshtein, object);
    } else if (Z_TYPE_P(pattern) == IS_ARRAY) {
        levenshtein->setPattern(Z_ARRVAL_P(pattern));
        levenshtein_update_pattern(levenshtein, object);
    }
}

void levenshtein_update_pattern(Levenshtein *levenshtein, zval *object)
{
    zval *pattern;
    pattern = levenshtein->getPattern();
    ZEND_UPDATE_PROPERTY(levenshtein_ce, object, "pattern", pattern);
    zval *storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    ZEND_UPDATE_PROPERTY(storage_ce, storage, "pattern", pattern);
}

void levenshtein_reset_pattern(Levenshtein *levenshtein, zval *object)
{
    zval *storage, *pattern;

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
            array_init(pattern);
            levenshtein->setPattern(Z_ARRVAL_P(pattern));
            levenshtein_update_pattern(levenshtein, object);
            break;
        default:
            levenshtein_update_pattern(levenshtein, object);
            return;
    }
}

void Z2DOUBLE(zval *val)
{
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
    zval *storage, *val, *costs;
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
            ZEND_UPDATE_PROPERTY(storage_ce, storage, "cost_ins", val);
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

            ZEND_UPDATE_PROPERTY(storage_ce, storage, "cost_rep", val);
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

            ZEND_UPDATE_PROPERTY(storage_ce, storage, "cost_del", val);
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

void levenshtein_flush_processed(Levenshtein *levenshtein, zval *object)
{
    levenshtein->flushProcessed();

    zval *storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    FLUSH_OBJECT_PROPERTY_NULL(storage_ce, storage, "path");
    FLUSH_OBJECT_PROPERTY_NULL(storage_ce, storage, "blocks");
    FLUSH_OBJECT_PROPERTY_NULL(storage_ce, storage, "searches");
    FLUSH_OBJECT_PROPERTY_NULL(storage_ce, storage, "lv");
    FLUSH_OBJECT_PROPERTY_NULL(storage_ce, storage, "match_map");
    FLUSH_OBJECT_PROPERTY_NULL(storage_ce, storage, "distance");

    FLUSH_OBJECT_PROPERTY_NULL(levenshtein_ce, object, "path");
    FLUSH_OBJECT_PROPERTY_NULL(levenshtein_ce, object, "blocks");
    FLUSH_OBJECT_PROPERTY_NULL(levenshtein_ce, object, "searches");
    FLUSH_OBJECT_PROPERTY_NULL(levenshtein_ce, object, "distance");
}

void update_storage_property(Levenshtein *levenshtein, zval *object, char *property, zval *value) 
{
    ZEND_UPDATE_PROPERTY(storage_ce, ZEND_READ_PROPERTY(levenshtein_ce, object, "storage"), property, value);
}

zval *levenshtein_get_distance(Levenshtein *levenshtein, zval *object)
{
    zval *string, *ret, *storage;

    MAKE_STD_ZVAL(ret);

    string = ZEND_READ_PROPERTY(levenshtein_ce, object, "string");
    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    
    if (Z_TYPE_P(string) == IS_NULL) {
        ZEND_UPDATE_PROPERTY(storage_ce, storage, "distance", ret);
        return ret;
    }

    levenshtein->setString(Z_STRVAL_P(string));
    
    levenshtein_reset_data_from_storage(levenshtein, object);
    
    ZVAL_DOUBLE(ret, levenshtein->getDistance());
    ZEND_UPDATE_PROPERTY(storage_ce, storage, "distance", ret);
    ZEND_UPDATE_PROPERTY(storage_ce, storage, "lv", levenshtein->getLV());
    ZEND_UPDATE_PROPERTY(storage_ce, storage, "match_map", levenshtein->getMap());

    return ret;
}

zval *levenshtein_get_path(Levenshtein *levenshtein, zval *object)
{
    zval *ret, *blocks, *searches, *storage;

    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    if (Z_TYPE_P(levenshtein_get_distance(levenshtein, object)) != IS_DOUBLE) {
        FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, storage, "path", ret);
        FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, storage, "blocks", blocks);
        FLUSH_OBJECT_PROPERTY_ARRAY(storage_ce, storage, "searches", searches);
        return ret;
    }

    ret = levenshtein->getPath();
    blocks = levenshtein->getBlocks();
    searches = levenshtein->getSearches();

    ZEND_UPDATE_PROPERTY(storage_ce, storage, "blocks", blocks);
    ZEND_UPDATE_PROPERTY(storage_ce, storage, "path", ret);
    ZEND_UPDATE_PROPERTY(storage_ce, storage, "searches", searches);
    return ret;
}

zval *levenshtein_get_blocks(Levenshtein *levenshtein, zval *object)
{
    zval *distance, *path, *blocks, *searches;
    MAKE_STD_ZVAL(path);
    MAKE_STD_ZVAL(blocks);
    MAKE_STD_ZVAL(searches);
    zval *storage, *prop, *val;
    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    
    distance = levenshtein_get_distance(levenshtein, object);
    if (Z_TYPE_P(distance) != IS_DOUBLE) {

        ZEND_UPDATE_PROPERTY(storage_ce, storage, "path", path);
        ZEND_UPDATE_PROPERTY(storage_ce, storage, "blocks", blocks);
        ZEND_UPDATE_PROPERTY(storage_ce, storage, "searches", searches);
        return blocks;
    }
    path = levenshtein->getPath();
    blocks = levenshtein->getBlocks();
    searches = levenshtein->getSearches();
    ZEND_UPDATE_PROPERTY(storage_ce, storage, "blocks", blocks);
    ZEND_UPDATE_PROPERTY(storage_ce, storage, "path", path);
    ZEND_UPDATE_PROPERTY(storage_ce, storage, "searches", searches);

    return blocks;
}

zval *levenshtein_get_searches(Levenshtein *levenshtein, zval *object)
{
    zval *path, *blocks, *searches;
    zval *storage, *prop, *val;
    storage = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");

    if (Z_TYPE_P(levenshtein_get_distance(levenshtein, object)) != IS_DOUBLE) {
        ZEND_UPDATE_PROPERTY(storage_ce, storage, "path", path);
        ZEND_UPDATE_PROPERTY(storage_ce, storage, "blocks", blocks);
        ZEND_UPDATE_PROPERTY(storage_ce, storage, "searches", searches);

        return searches;
    }

    path = levenshtein->getPath();
    blocks = levenshtein->getBlocks();
    searches = levenshtein->getSearches();
    ZEND_UPDATE_PROPERTY(storage_ce, storage, "blocks", blocks);
    ZEND_UPDATE_PROPERTY(storage_ce, storage, "path", path);
    ZEND_UPDATE_PROPERTY(storage_ce, storage, "searches", searches);
    return searches;
}

zval *levenshtein__get(Levenshtein *levenshtein, zval *object, std::string key)
{
    zval *ret;
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
    } else if (key.compare("string") == 0) {
        ret = levenshtein->getString();
    } else if (key.compare("storage") == 0) {
        ret = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");
    } else {
        char buff[255];
        snprintf(buff, sizeof(buff), "Unknown property `%s`.", key.c_str());
        ZEND_THROW_EXCEPTION(buff);
        ret = (zval*)ecalloc(1, sizeof(zval));
        ZVAL_NULL(ret);
    }

    levenshtein->flushProcessed();

    return ret;
}

void levenshtein__set(Levenshtein *levenshtein, zval *object, std::string key, zval *value)
{
    zval *ret;
    if (key.compare("pattern") == 0) {
        levenshtein->setPattern(value);
        update_storage_property(levenshtein, object, (char *)"pattern", levenshtein->getPattern());
        levenshtein_flush_processed(levenshtein, object);
    } else if (key.compare("string") == 0) {
        levenshtein->setString(value);
        levenshtein_flush_processed(levenshtein, object);
    }

}

PHP_METHOD (Levenshtein, __get)
{
    Levenshtein *levenshtein;
    zval *object = getThis();
    levenshtein_object *obj = Z_LEVENSHTEINOBJ_P(object);
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL) {
        ZEND_THROW_EXCEPTION( "Levenshtein object not initialized");
        RETURN_NULL();
    }
    
    std::string sKey = "";
#ifdef ZEND_ENGINE_2  
    char *key;
    int key_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
        RETURN_NULL();
    }
    sKey.assign(key, key_len);
#endif
#ifdef ZEND_ENGINE_3
    zend_string *zKey;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &zKey) == FAILURE) {
        RETURN_NULL();
    }    
    sKey.assign((*zKey).val, (*zKey).len);
#endif

    *return_value = *levenshtein__get(levenshtein, object, sKey);
    zval_copy_ctor(return_value);
}

PHP_METHOD (Levenshtein, __set)
{
    Levenshtein *levenshtein;
    zval *object = getThis();
    std::string sKey = "";
    zval *val;
    MAKE_PERSISTENT_ZVAL(val);
    levenshtein_object *obj = Z_LEVENSHTEINOBJ_P(object);
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL) {
        ZEND_THROW_EXCEPTION( "Levenshtein object not initialized");
    }
#ifdef ZEND_ENGINE_2
    char *key;
    int key_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &key, &key_len, &val) == FAILURE) {
        RETURN_NULL();
    }
    sKey.assign(key, key_len);
#endif
#ifdef ZEND_ENGINE_3
    zend_string *zKey;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &zKey, &val) == FAILURE) {
        RETURN_NULL();
    }
    sKey.assign((*zKey).val, (*zKey).len);
#endif

    levenshtein__set(levenshtein, object, sKey, val);
}

PHP_METHOD (Levenshtein, __destruct)
{
    zval *object = getThis();
    zval *buffer, *prop;
    buffer = ZEND_READ_PROPERTY(levenshtein_ce, object, "storage");

    if (Z_TYPE_P(buffer) == IS_OBJECT) {
        UNSET_PROPERTY(buffer, "pattern");
        UNSET_PROPERTY(buffer, "path");
        UNSET_PROPERTY(buffer, "blocks");
        UNSET_PROPERTY(buffer, "lv");
        UNSET_PROPERTY(buffer, "match_map");
        ZVAL_NULL(buffer);
    } 

    buffer = ZEND_READ_PROPERTY(levenshtein_ce, object, "pattern");
    ZVAL_NULL(buffer);
    buffer = ZEND_READ_PROPERTY(levenshtein_ce, object, "costs");
    ZVAL_NULL(buffer);

}

void levenshtein_set_string(Levenshtein *levenshtein, std::string str)
{  
    levenshtein->setString((char*)str.c_str());
}

PHP_METHOD (Levenshtein, __call)
{
    Levenshtein *levenshtein;
    zval *object = getThis();

    std::string sName = "";
    HashTable *zArguments;
    ALLOC_HASHTABLE(zArguments);

    levenshtein_object *obj = Z_LEVENSHTEINOBJ_P(object);
    levenshtein = obj->levenshtein;
    if (levenshtein == NULL) {
        ZEND_THROW_EXCEPTION( "Levenshtein object not initialized");
    }

#ifdef ZEND_ENGINE_2   
    char *name;
    int name_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sh", &name, &name_len, zArguments) == FAILURE) {

        RETURN_NULL();
    }
    sName.assign(name, name_len);
#endif
#ifdef ZEND_ENGINE_3    
    zend_string *zName;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Sh", &zName, &zArguments) == FAILURE) {
        RETURN_NULL();        
    }
    sName.assign((*zName).val, (*zName).len);
#endif    

    if (sName.substr(0,3).compare("get") == 0) {
        std::string param = sName.substr(3);
        param[0] = tolower(param[0]);

        *return_value = *levenshtein__get(levenshtein, object, param);
        zval_copy_ctor(return_value);

    } else if (sName.substr(0,3).compare("set") == 0) {
        std::string param = sName.substr(3);
        param[0] = tolower(param[0]);

        if (zend_hash_num_elements(zArguments) > 0) {
            zend_hash_internal_pointer_reset(zArguments);
            zval *value;
            value = ZEND_HASH_GET_CURRENT_DATA(zArguments);
            levenshtein__set(levenshtein, object, param, value);
        }

    }
}