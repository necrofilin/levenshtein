#ifndef ZEND_2_TO_3_FIX
#define ZEND_2_TO_3_FIX

#include <zend_hash.h>

#ifdef ZEND_ENGINE_2
	#include <zend_engine_2_functions.h>
#endif

#ifdef ZEND_ENGINE_3
	#include <zend_engine_3_functions.h>
#endif

#endif
