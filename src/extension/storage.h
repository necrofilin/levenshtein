#ifndef STORAGE_H
#define STORAGE_H

#include <php.h>

class Storage {
public:
    Storage();

private:
    char **pattern;
    int pattern_len;

    double distance;
    char **path;
    char **blocks;
    char **searches;

    double **lv;
    char **match_map;

    double cost_ins;
    double cost_del;
    double cost_rep;
};

#endif