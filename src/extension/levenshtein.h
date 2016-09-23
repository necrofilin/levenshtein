#ifndef LEVENSHTEIN_H
#define LEVENSHTEIN_H

#include <php.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#define MIN(a, b) (((a)<(b))?(a):(b))
#define MAX(a, b) (((a)>(b))?(a):(b))

extern char equals(double a, double b);

extern char isDigit(char c);

extern char isAlpha(char c);

extern char isDigit(std::string str);

extern char isAlpha(std::string str);

extern char isAlphaDigit(char c);

extern char isDecimalSign(char c);

extern char normalizeChar(char c);

extern char toDigit(char c);

extern char toAlpha(char c);

extern char toDash(char c);

extern char *applyPattern(char *p, char c);

extern void applyPattern(std::string *block, std::string *path, std::string pattern, char search, std::string action);

extern void clearPointer(void **ptr, char type_l1);

extern void clearPointer(void **ptr, char type_l1, char type_l2, int len_l1);


extern void echo(char *str);

class Levenshtein {
public:
    Levenshtein();

    void setString(char *str);

    zval *getString();

    void clearPattern();

    void setPattern(char *pattern);

    void setPattern(HashTable *pattern);

    void setPattern(HashTable *pattern, char flush_if_changed);

    zval *getPattern();

    void flushProcessed();

    double getDistance();

    void setDistance(double distance);

    zval *getBlocks();

    void setBlocks(zval *blocks);

    zval *getSearches();

    void setSearches(zval *searches);

    zval *getLV();

    zval *getMap();

    void setLV(zval *lv);

    void setMap(zval *map);

    zval *getPath();

    void setPath(zval *path);

    void setCostIns(double cost);

    void setCostDel(double cost);

    void setCostRep(double cost);

    double getCostIns();

    double getCostDel();

    double getCostRep();

private:
    std::string string;
    std::vector<std::string> pattern;

    int pattern_len;
    double distance;

    int m;
    int n;

    std::vector<std::vector<double> > lv;
    std::vector<std::vector<char> > match_map;

    void initLV();

    void initMap();

    std::vector<std::string> path;
    std::vector<std::string> blocks;
    std::vector<std::string> searches;

    double cost_ins;
    double cost_del;
    double cost_rep;

    double del(int i, int j);

    double ins(int i, int j);

    double rep(int i, int j);

    char matchPattern(int i, int j);

    char isPattern(int i);

    char isMultichar(int i);

    char isNonRequired(int i);

    char isNoise(int j);

    char isPatternNoise(int i);

    char isNoise(char c);

    char isNoise(char *str);

    double insertCost(int j);

    double insertCost(int i, int j);

    double replaceCost(int i, int j);

    double deleteCost(int i, int j, char allow_singlechar_noise = 0);

    double repSimple(int i, int j);

    double repMultichar(int i, int j);

    double repNonRequired(int i, int j);

    void pushPath(int i, char pre);

    void shiftPath(int i);

    double prev(int i, int j);


    void applyPattern(int i, int j, std::string action);

    void clearMap();

    void clearLV();

    void clearPath();

    void clearBlocks();

    void clearSearches();
};


#endif