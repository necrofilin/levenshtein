
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

#define ZEND_HASH_UPDATE_INDEX(ht, idx, val) \
	zend_hash_index_update(ht, idx, val)

#define INIT_ZOBJECT(class_entry, val) \
     if(val == NULL) { \
     	MAKE_STD_ZVAL(val); \
     	object_init_ex(val, class_entry);\
     }

#define ZEND_READ_PROPERTY(class_entry, object, property) \
	zend_read_property(class_entry, object, property, sizeof(property) - 1, 1, (zval*)ecalloc(1, sizeof(zval)))

#define FLUSH_OBJECT_PROPERTY_ARRAY(class_entry, object, property, buff) \
		INIT_ZOBJECT(class_entry, object); \
		MAKE_PERSISTENT_ZVAL(buff); \
		array_init(buff); \
		zend_update_property(class_entry, object, property, sizeof(property) - 1, buff)

#define FLUSH_OBJECT_PROPERTY_NULL(class_entry, object, property) \
		zend_update_property_null(class_entry, object, property, sizeof(property) - 1); \
		{\
			zval *zProp; \
			MAKE_STD_ZVAL(zProp); \
			ZVAL_STRING(zProp, property); \
			Z_OBJ_HANDLER_P(object, unset_property)(object, zProp, NULL); \
		}

#define FN_OBJECT_CREATE_HANDLER(name) \
     zend_object* name##_create_handler(zend_class_entry *type) 		

#define OBJECT_SIZE(object_type, class_entry) \
     sizeof(object_type) + zend_object_properties_size(class_entry)		
 		
#define INIT_OBJECT_CREATE_HANDLER(obj, class_entry, dtor, storage, clone, handlers_p, retval) \
     {\
         zend_object_std_init(&obj->std, class_entry); \
         object_properties_init(&obj->std, class_entry); \
         obj->std.handlers = &handlers_p; \
     }		
 	
#define OBJECT_CREATE_HANDLER_RETURN_TYPE() zend_object 		
 		
#define INIT_OBJECT_CREATE_HANDLER_RETURN(obj, retval) \
     &obj->std