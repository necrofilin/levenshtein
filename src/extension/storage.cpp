#include "storage.h"
#include <stdlib.h>
#include <string.h>

Storage::Storage() {

    this->pattern_len = 0;
    this->pattern = NULL;

    this->cost_ins = 1;
    this->cost_del = 1;
    this->cost_rep = 1;

    this->distance = 0;
    this->path = NULL;
    this->blocks = NULL;
    this->searches = NULL;

    this->lv = NULL;
    this->match_map = NULL;

}