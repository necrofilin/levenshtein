#define MAKE_PERSISTENT_ZVAL(val) \
	MAKE_STD_ZVAL(val)

#define ZVAL_STRING_EX(val, string) \
	ZVAL_STRING(val, string, 1)

#define ZEND_HASH_HAS_MORE_ELEMENTS(ht) \
	zend_hash_get_current_data(ht, (void**)ecalloc(1,sizeof(zval*)))

static inline zval *zend_engine_3__zend_hash_get_current_data(HashTable *ht){
	zval **ppData;
	ppData = (zval**)ecalloc(1, sizeof(zval*));
	zend_hash_get_current_data(ht, (void**)&ppData);
	return *ppData;
}

#define ZEND_HASH_GET_CURRENT_DATA(ht) \
	zend_engine_3__zend_hash_get_current_data(ht)

#define ZEND_HASH_ADD(ht, key, val) \
	zend_hash_update(ht, key, sizeof(key) - 1, val, sizeof(zval*), NULL)

#define ZEND_HASH_UPDATE_INDEX(ht, idx, val) \
	zend_hash_index_update(ht, idx, &val, sizeof(zval), NULL)

#define INIT_ZOBJECT(class_entry, val) \
     if(val == NULL) { \
     	MAKE_STD_ZVAL(val); \
     	object_init_ex(val, class_entry);\
     }

#define ZEND_READ_PROPERTY(class_entry, object, property) \
	zend_read_property(class_entry, object, property, sizeof(property) - 1, 1 TSRMLS_CC)

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

#define OBJECT_SIZE(object_type, class_entry) \
     sizeof(object_type)		
 		
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
 		
#define OBJECT_CREATE_HANDLER_RETURN_TYPE() zend_object_value