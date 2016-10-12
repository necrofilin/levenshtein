#ifndef NF_FUNCTIONS_H
#define NF_FUNCTIONS_H

#include <zend_hash.h>


#define FLUSH_VECTOR(vect) \
    (vect).erase((vect).begin(),(vect).end())

#include <zend_engine_2_functions.h>
#include <zend_engine_3_functions.h>

#endif
