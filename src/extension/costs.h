#ifndef COSTS_H
#define COSTS_H

#include <php.h>

class Costs {
public:
    Costs();

private:
    double insert;
    double del;
    double replace;
};

#endif