#ifndef ZEND_ENGINE_2_FUNCTIONS_H
#define ZEND_ENGINE_2_FUNCTIONS_H

#ifdef ZEND_ENGINE_2

#define MAKE_PERSISTENT_ZVAL(val) \
    MAKE_STD_ZVAL(val)

#define ZVAL_STRING_EX(val, string) \
    ZVAL_STRING(val, string, 1)

#define ZEND_HASH_HAS_MORE_ELEMENTS(ht) \
    zend_hash_get_current_data(ht, (void**)ecalloc(1,sizeof(zval*)))

static inline zval *zend_engine_3__zend_hash_get_current_data(HashTable *ht)
{
    zval **ppData;
    ppData = (zval**)ecalloc(1, sizeof(zval*));
    zend_hash_get_current_data(ht, (void**)&ppData);
    return *ppData;
}

#define ZEND_HASH_GET_CURRENT_DATA(ht) \
    zend_engine_3__zend_hash_get_current_data(ht)

#define ZEND_HASH_ADD(ht, key, val) \
    zend_hash_update(ht, key, sizeof(key) - 1, val, sizeof(zval*), NULL)

#define ZEND_HASH_ADD_U(ht, key, val) \
    { \
        zval *tmp; \
        MAKE_STD_ZVAL(tmp); \
        tmp = val; \
        ZEND_HASH_ADD(ht, key, &tmp); \
    }

#define ZEND_HASH_UPDATE_INDEX(ht, idx, val) \
    zend_hash_index_update(ht, idx, &val, sizeof(zval), NULL)

#define INIT_ZOBJECT(class_entry, val) \
    if(val == NULL) { \
        MAKE_STD_ZVAL(val); \
        object_init_ex(val, class_entry); \
    }

#define ZEND_DECLARE_PROPERTY_NULL(class_entry, property, access) \
    zend_declare_property_null(class_entry, property, sizeof(property) - 1, access TSRMLS_CC)

#define ZEND_DECLARE_PROPERTY_DOUBLE(class_entry, property, value, access) \
    zend_declare_property_double(class_entry, property, sizeof(property) - 1, value, access TSRMLS_CC)

#define ZEND_READ_PROPERTY(class_entry, object, property) \
    zend_read_property(class_entry, object, property, sizeof(property) - 1, 1 TSRMLS_CC)

#define ZEND_UPDATE_PROPERTY(class_entry, object, property, value) \
    zend_update_property(class_entry, object, property, sizeof(property) - 1, value TSRMLS_DC);

#define ZEND_UPDATE_PROPERTY_NULL(class_entry, object, property) \
    zend_update_property_null(class_entry, object, property, sizeof(property) - 1 TSRMLS_DC);

#define ZEND_UPDATE_PROPERTY_DOUBLE(class_entry, object, property, value) \
    zend_update_property_double(class_entry, object, property, sizeof(property) - 1, value TSRMLS_DC);

#define FLUSH_OBJECT_PROPERTY_ARRAY(class_entry, object, property, buff) \
    INIT_ZOBJECT(class_entry, object); \
    MAKE_STD_ZVAL(buff); \
    array_init(buff); \
    zend_update_property(class_entry, object, property, sizeof(property) - 1, buff TSRMLS_DC)

#define FLUSH_OBJECT_PROPERTY_NULL(class_entry, object, property) \
    zend_update_property_null(class_entry, object, property, sizeof(property) - 1 TSRMLS_DC); \
    {\
        zval *zProp; \
        MAKE_STD_ZVAL(zProp); \
        ZVAL_STRING(zProp, property, 1); \
        Z_OBJ_HANDLER_P(object, unset_property)(object, zProp, NULL); \
    }

#define FN_OBJECT_CREATE_HANDLER(name) \
    zend_object_value name ## _create_handler(zend_class_entry *type TSRMLS_DC)

#define MEMSET_OBJECT(name, var_name, type) \
    name ## _object *var_name = (name ## _object *) ecalloc(1, OBJECT_SIZE(name ## _object, type)); \
    memset(var_name, 0, sizeof(name ## _object))

#define OBJECT_SIZE(object_type, class_entry) \
    sizeof(object_type)

#define INIT_OBJECT_CREATE_HANDLER(obj, class_entry, dtor, storage, clone, handlers_p, retval) \
    {\
        zend_object_std_init(&obj->std, class_entry TSRMLS_CC); \
        \
        ALLOC_HASHTABLE(obj->std.properties); \
        zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0); \
        object_properties_init(&obj->std, class_entry); \
        (*retval).handle = zend_objects_store_put(obj, dtor, storage, clone TSRMLS_CC); \
        \
        (*retval).handlers = (const zend_object_handlers*)(&handlers_p); \
     }

#define INIT_OBJECT_CREATE_HANDLER_RETURN(obj, retval) \
     retval

#define OBJECT_CREATE_HANDLER_RETURN_TYPE() \
     zend_object_value

#define F_NAME(name, fn) \
     name ## fn

#define INIT_NS_CLASS(name, namespace, class_name) \
     zend_class_entry F_NAME(name, ce); \
     INIT_NS_CLASS_ENTRY(F_NAME(name, ce), namespace, class_name, F_NAME(name, _methods)); \
     F_NAME(name, _ce) = zend_register_internal_class(&F_NAME(name, ce) TSRMLS_CC); \
     (*F_NAME(name, _ce)).create_object = F_NAME(name, _create_handler);\
     memcpy(&F_NAME(name, _object_handlers), zend_get_std_object_handlers(), sizeof(zend_object_handlers))

#define UNSET_PROPERTY(object, property) \
     { \
        zval *prop; \
        MAKE_STD_ZVAL(prop); \
        ZVAL_STRING(prop, property, 1); \
        Z_OBJ_HT_P(object)->unset_property(object, prop, NULL); \
     }

#define ZEND_THROW_EXCEPTION(message) \
     zend_throw_exception(zend_exception_get_default(TSRMLS_C), message, 0 TSRMLS_CC)

#define DECLARE_OBJECT_STRUCT(name, class_name) \
     struct name ## _object { \
        zend_object std; \
        class_name *name; \
     }


#endif

#endif