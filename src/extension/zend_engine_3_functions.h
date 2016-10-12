#ifndef ZEND_ENGINE_3_FUNCTIONS_H
#define ZEND_ENGINE_3_FUNCTIONS_H

#ifdef ZEND_ENGINE_3

#define MAKE_PERSISTENT_ZVAL(val) \
    val = (zval*)ecalloc(1, sizeof(zval));\
    ZVAL_NULL(val)

#define MAKE_STD_ZVAL(val) \
    if(val != NULL) efree(val); \
    MAKE_PERSISTENT_ZVAL(val)

#define ZVAL_STRING_EX(val, string) \
    ZVAL_STRING(val, string)

#define ZEND_HASH_HAS_MORE_ELEMENTS(ht) \
    zend_hash_has_more_elements(ht)

#define ZEND_HASH_GET_CURRENT_DATA(ht) \
    zend_hash_get_current_data(ht)

#define ZEND_HASH_ADD(ht, key, val) \
    zend_hash_update(ht, zend_string_init(key, sizeof(key) - 1, 0), *val)

#define ZEND_HASH_ADD_U(ht, key, val) \
    { \
        zval *tmp; \
        MAKE_STD_ZVAL(tmp); \
        tmp = val; \
        ZEND_HASH_ADD(ht, key, &tmp); \
    }

#define ZEND_HASH_UPDATE_INDEX(ht, idx, val) \
    zend_hash_index_update(ht, idx, val)

#define INIT_ZOBJECT(class_entry, val) \
     if(val == NULL) { \
        MAKE_STD_ZVAL(val); \
        object_init_ex(val, class_entry); \
     }

#define ZEND_DECLARE_PROPERTY_NULL(class_entry, property, access) \
    zend_declare_property_null(class_entry, property, sizeof(property) - 1, access)

#define ZEND_DECLARE_PROPERTY_DOUBLE(class_entry, property, value, access) \
    zend_declare_property_double(class_entry, property, sizeof(property) - 1, value, access)

#define ZEND_READ_PROPERTY(class_entry, object, property) \
     zend_read_property(class_entry, object, property, sizeof(property) - 1, 1, (zval*)ecalloc(1, sizeof(zval)))

#define ZEND_UPDATE_PROPERTY(class_entry, object, property, value) \
    zend_update_property(class_entry, object, property, sizeof(property) - 1, value);

#define ZEND_UPDATE_PROPERTY_NULL(class_entry, object, property) \
    zend_update_property_null(class_entry, object, property, sizeof(property) - 1);

#define ZEND_UPDATE_PROPERTY_DOUBLE(class_entry, object, property, value) \
    zend_update_property_double(class_entry, object, property, sizeof(property) - 1, value);

#define FLUSH_OBJECT_PROPERTY_ARRAY(class_entry, object, property, buff) \
    INIT_ZOBJECT(class_entry, object); \
    MAKE_PERSISTENT_ZVAL(buff); \
    array_init(buff); \
    zend_update_property(class_entry, object, property, sizeof(property) - 1, buff)

#define FLUSH_OBJECT_PROPERTY_NULL(class_entry, object, property) \
    zend_update_property_null(class_entry, object, property, sizeof(property) - 1); \
    { \
        zval *zProp; \
        MAKE_STD_ZVAL(zProp); \
        ZVAL_STRING(zProp, property); \
        Z_OBJ_HANDLER_P(object, unset_property)(object, zProp, NULL); \
    }

#define FN_OBJECT_CREATE_HANDLER(name) \
     zend_object* name##_create_handler(zend_class_entry *type)

#define MEMSET_OBJECT(name, var_name, type) \
    name ## _object *var_name = (name ## _object *) ecalloc(1, OBJECT_SIZE(name ## _object, type)); \
    memset(var_name, 0, sizeof(name ## _object))

#define OBJECT_SIZE(object_type, class_entry) \
     sizeof(object_type) + zend_object_properties_size(class_entry)

#define INIT_OBJECT_CREATE_HANDLER(obj, class_entry, dtor, storage, clone, handlers_p, retval) \
    { \
        zend_object_std_init(&obj->std, class_entry); \
        object_properties_init(&obj->std, class_entry); \
        obj->std.handlers = &handlers_p; \
    }

#define OBJECT_CREATE_HANDLER_RETURN_TYPE() \
    zend_object

#define F_NAME(name, fn) \
     name ## fn

#define INIT_NS_CLASS(name, namespace, class_name) \
     zend_class_entry F_NAME(name, ce); \
     INIT_NS_CLASS_ENTRY(F_NAME(name, ce), namespace, class_name, F_NAME(name, _methods)); \
     F_NAME(name, _ce) = zend_register_internal_class(&F_NAME(name, ce)); \
     (*F_NAME(name, _ce)).create_object = F_NAME(name, _create_handler);\
     memcpy(&F_NAME(name, _object_handlers), zend_get_std_object_handlers(), sizeof(zend_object_handlers))

#define INIT_OBJECT_CREATE_HANDLER_RETURN(obj, retval) \
     &obj->std

#define UNSET_PROPERTY(object, property) \
     { \
        zval *prop; \
        MAKE_STD_ZVAL(prop); \
        ZVAL_STRING(prop, property); \
        Z_OBJ_HT_P(object)->unset_property(object, prop, NULL); \
     }

#define ZEND_THROW_EXCEPTION(message) \
     zend_throw_exception(zend_exception_get_default(), message, 0)


#define DECLARE_OBJECT_STRUCT(name, class_name) \
     struct name ## _object { \
        class_name *name; \
        zend_object std; \
     }

#endif

#endif