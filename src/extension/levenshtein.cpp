#include <zend_types.h>
#include "levenshtein.h"
#include <typeinfo>

const char DEBUG = 0;


Levenshtein::Levenshtein()
{
    this->pattern_len = 0;
    this->string = "";
    FLUSH_VECTOR(this->pattern);
    this->cost_ins = 1;
    this->cost_del = 1;
    this->cost_rep = 1;

    this->distance = 0;
    FLUSH_VECTOR(this->path);
    FLUSH_VECTOR(this->lv);
    FLUSH_VECTOR(this->match_map);
    FLUSH_VECTOR(this->blocks);
    FLUSH_VECTOR(this->searches);
    this->m = 0;
    this->n = 0;
}

void Levenshtein::flushProcessed()
{
    this->clearLV();
    this->clearMap();
    this->clearPath();
    this->distance = 0;
}

void Levenshtein::setPattern(HashTable *pattern)
{
    this->setPattern(pattern, 1);
}

void Levenshtein::setPattern(HashTable *pattern, char flush_if_changed)
{
    if (this->pattern.empty()) {
        this->pattern.resize(zend_hash_num_elements(pattern) + 1, "");
    }

    int i;
    zval *data;
    char keep = 1, *pat_el;

    for (i = 0, zend_hash_internal_pointer_reset(pattern); ZEND_HASH_HAS_MORE_ELEMENTS(pattern) == SUCCESS; ++i, zend_hash_move_forward(pattern)) 
    {
        data = ZEND_HASH_GET_CURRENT_DATA(pattern);
        pat_el = Z_STRVAL_P(data);

        if (!flush_if_changed || this->pattern[i].empty() || this->pattern[i].compare(pat_el) != 0) {
            this->pattern[i].assign(pat_el, Z_STRLEN_P(data));
            keep = 0;
        }
    }

    this->pattern_len = i;

    this->m = i;
    if (!keep && flush_if_changed) {
        this->flushProcessed();
    }

}


void Levenshtein::setString(zval *value)
{
    char buffer[32];
    switch(Z_TYPE_P(value)){
#ifdef ZEND_ENGINE_2
        case IS_BOOL:
            this->setString(Z_BVAL_P(value) ? (char*)"TRUE" : (char*)"FALSE");
            break;
#endif
#ifdef ZEND_ENGINE_3
        case IS_FALSE:
            this->setString((char*)"FALSE");
            break;
        case IS_TRUE:
            this->setString((char*)"TRUE");
            break;
#endif
        case IS_LONG:
            snprintf(buffer, sizeof(buffer), "%d", Z_LVAL_P(value));
            this->setString(buffer);
            break;            
        case IS_DOUBLE:
            snprintf(buffer, sizeof(buffer), "%g", Z_DVAL_P(value));
            this->setString(buffer);
            break;
        case IS_STRING:
            this->setString(Z_STRVAL_P(value));;
            break;
        }
}

void Levenshtein::setString(char *str)
{
    if (this->string.empty() || this->string.compare(str) != 0) {
        this->flushProcessed();
        this->string.assign(str);
    }
}

zval *Levenshtein::getString()
{
    zval *zvt;
    MAKE_STD_ZVAL(zvt);
    ZVAL_STRING_EX(zvt, this->string.c_str());
    return zvt;
}

zval *Levenshtein::getPattern()
{
    int i;

    zval *zv, *zvt;
    MAKE_STD_ZVAL(zv);
    array_init(zv);

    for (i = 0; i < this->pattern_len; ++i) {
        MAKE_PERSISTENT_ZVAL(zvt);
        ZVAL_STRING_EX(zvt, this->pattern[i].c_str());
        ZEND_HASH_UPDATE_INDEX(Z_ARRVAL_P(zv), i, zvt);
    }
    return zv;
}

zval *Levenshtein::getBlocks()
{

    if (this->blocks.empty()) {
        zval *path = this->getPath();
    }

    int i;
    zval *zv, *zvt;
    MAKE_STD_ZVAL(zv);
    array_init(zv);

    for (i = 0; i <= this->pattern_len; ++i) {
        MAKE_PERSISTENT_ZVAL(zvt);
        ZVAL_STRING_EX(zvt, this->blocks[i].c_str());
        ZEND_HASH_UPDATE_INDEX(Z_ARRVAL_P(zv), i, zvt);
    }

    return zv;
}

zval *Levenshtein::getSearches()
{
    if (this->blocks.empty()) {
        zval *path = this->getPath();
    }
    int i;
    zval *zv, *zvt;
    MAKE_STD_ZVAL(zv);
    array_init(zv);
    for (i = 0; i <= this->pattern_len; ++i) {
        MAKE_PERSISTENT_ZVAL(zvt);
        ZVAL_STRING_EX(zvt, this->searches[i].c_str());
        ZEND_HASH_UPDATE_INDEX(Z_ARRVAL_P(zv), i, zvt);
    }
    return zv;
}

void Levenshtein::initLV()
{

    int i, j;
    this->clearLV();

    this->lv.resize(this->m + 1, std::vector<double>(this->n + 1));

    this->lv[0][0] = 0;
    for (j = 1; j <= this->n; ++j) {
        this->lv[0][j] = this->lv[0][j - 1] + this->insertCost(i, j);
    }

    for (i = 1; i <= this->m; ++i) {
        this->lv[i][0] = this->lv[i - 1][0] + this->deleteCost(i, j);
    }

}

void Levenshtein::initMap()
{

    int i, j;
    this->clearMap();
    this->match_map.resize(this->m + 1, std::vector<char>(this->n + 1));

}

double Levenshtein::insertCost(int j)
{
    return this->insertCost(0, j);
}


double Levenshtein::insertCost(int i, int j)
{
    return this->isNoise(j) && this->isNoise(j - 1)
        ? 0
        : this->cost_ins / (this->isNoise(j) || this->isPatternNoise(i) ? 2.5 : 1);
}

double Levenshtein::deleteCost(int i, int j, char allow_singlechar_noise)
{
    return this->cost_del / (this->isPatternNoise(i) || ((this->isMultichar(i) || allow_singlechar_noise) && this->isNoise(j)) ? 2.5 : 1);
}


double Levenshtein::replaceCost(int i, int j)
{
    double cost = this->cost_rep / (this->isNoise(j) && this->isPatternNoise(i) ? 3 : 1);
    return cost;
}

double Levenshtein::getDistance()
{
    if (!this->lv.empty()) {
        return this->distance;
    }
    this->m = this->pattern_len;
    if (this->string.empty()) {
        this->n = 0;
        this->initLV();
        this->initMap();
        return this->m;
    }
    this->n = this->string.size();
    this->initLV();
    this->initMap();
    int i, j;

    for (i = 1; i <= this->m; ++i) {

        for (j = 1; j <= this->n; ++j) {

            this->lv[i][j] = MIN(
                    this->del(i, j),
                    MIN(
                            this->ins(i, j),
                            this->rep(i, j)
                    )
            );
        }
    }

    this->distance = this->lv[this->m][this->n];
    return this->distance;
}


double Levenshtein::del(int i, int j)
{
    return this->lv[i - 1][j] + this->deleteCost(i, j);
}

double Levenshtein::ins(int i, int j)
{
    return this->lv[i][j - 1] + this->insertCost(i, j);
}

double Levenshtein::rep(int i, int j)
{
    char mt = this->matchPattern(i, j);
    return MIN(
        this->repSimple(i, j),
        MIN(
            this->repMultichar(i, j),
            this->repNonRequired(i, j)
        )
    );
}

double Levenshtein::repSimple(int i, int j)
{
    return this->lv[i - 1][j - 1] + (this->match_map[i][j] ? 0 : this->replaceCost(i, j));
}

char Levenshtein::isPattern(int i)
{
    return i > 0 && i <= this->m ? this->pattern[i - 1].size() == 2 : 0;
}

char Levenshtein::isMultichar(int i)
{

    if (!this->isPattern(i))
        return 0;

    switch (this->pattern[i - 1][1]) {
        case 'o':
        case 'd':
        case 's':
            return 1;
        default:
            return 0;
    }
}

char Levenshtein::isNonRequired(int i)
{
    if (!this->isPattern(i))
        return 0;

    switch (this->pattern[i - 1][1]) {
        case 'N':
        case 'Z':
        case 'L':
            return 1;
            break;
        case '-':
            return i == this->m;
            break;
        default:
            return 0;
    }
}

double Levenshtein::repMultichar(int i, int j)
{
    if (this->isMultichar(i)) {
        return this->lv[i][j - 1] + (this->match_map[i][j] ? 0 : this->replaceCost(i, j));
    } else {
        return 32767;
    }
}

double Levenshtein::repNonRequired(int i, int j)
{
    if (this->isNonRequired(i)) {
        return MIN(this->lv[i - 1][j - 1], this->lv[i - 1][j]);
    } else {
        return 32767;
    }
}

zval *Levenshtein::getLV()
{
    int i, j;
    zval *zv, *zvi, *zvj;

    MAKE_STD_ZVAL(zv);
    if (this->m == 0) {
        ZVAL_NULL(zv);
        return zv;
    }

    array_init(zv);

    for (i = 0; i <= this->m; ++i) {
        MAKE_PERSISTENT_ZVAL(zvi);
        array_init(zvi);
        for (j = 0; j <= this->n; ++j) {
            MAKE_PERSISTENT_ZVAL(zvj);
            ZVAL_DOUBLE(zvj, this->lv[i][j]);
            ZEND_HASH_UPDATE_INDEX(Z_ARRVAL_P(zvi), j, zvj);
        }
        ZEND_HASH_UPDATE_INDEX(Z_ARRVAL_P(zv), i, zvi);
    }

    return zv;
}

zval *Levenshtein::getMap()
{
    int i, j;
    zval *zv, *zvi, *zvj;
    MAKE_STD_ZVAL(zv);
    if (this->m == 0) {
        ZVAL_NULL(zv);
        return zv;
    }
    array_init(zv);

    for (i = 0; i <= this->m; ++i) {
        MAKE_PERSISTENT_ZVAL(zvi);
        array_init(zvi);

        for (j = 0; j <= this->n; ++j) {
            MAKE_PERSISTENT_ZVAL(zvj);
            ZVAL_LONG(zvj, this->match_map[i][j]);
            ZEND_HASH_UPDATE_INDEX(Z_ARRVAL_P(zvi), j, zvj);
        }
        ZEND_HASH_UPDATE_INDEX(Z_ARRVAL_P(zv), i, zvi);
    }

    return zv;
}

void Levenshtein::clearPattern()
{
    FLUSH_VECTOR(this->pattern);
    this->pattern_len = 0;
    this->m = 0;
}

void Levenshtein::clearLV()
{
    int cap = this->lv.capacity(), size = this->lv.size();
    FLUSH_VECTOR(this->lv);
}

void Levenshtein::clearSearches()
{
    FLUSH_VECTOR(this->searches);
}

void Levenshtein::clearBlocks()
{
    FLUSH_VECTOR(this->blocks);
}

void Levenshtein::clearMap()
{
    FLUSH_VECTOR(this->match_map);
}

void Levenshtein::clearPath()
{
    FLUSH_VECTOR(this->path);
    FLUSH_VECTOR(this->blocks);
}

void Levenshtein::setDistance(double distance)
{
    this->distance = distance;
}

void Levenshtein::setLV(zval *lv)
{
    if (Z_TYPE_P(lv) != IS_ARRAY) {
        this->clearLV();
        return;
    }

    HashTable *ht = Z_ARRVAL_P(lv), *row_ht;
    int len = zend_hash_num_elements(ht), i;
    this->clearLV();
    if (len < this->m || len == 0) {
        return;
    }
    this->lv.resize(len + 1, std::vector<double>(0));
    zval *row, *cell;

    int j;

    for (i = 0, zend_hash_internal_pointer_reset(ht); ZEND_HASH_HAS_MORE_ELEMENTS(ht) == SUCCESS; ++i, zend_hash_move_forward(ht)) {
        row = ZEND_HASH_GET_CURRENT_DATA(ht);
        if (Z_TYPE_P(row) == IS_ARRAY) {
            row_ht = Z_ARRVAL_P(row);
            this->lv[i].resize(zend_hash_num_elements(row_ht) + 1, 0);

            for (j = 0, zend_hash_internal_pointer_reset(row_ht); ZEND_HASH_HAS_MORE_ELEMENTS(row_ht) == SUCCESS; ++j, zend_hash_move_forward(row_ht)) {
                cell = ZEND_HASH_GET_CURRENT_DATA(row_ht);
                this->lv[i][j] = Z_DVAL_P(cell);
            }
        } else {
            return this->flushProcessed();
        }
    }
}

void Levenshtein::setMap(zval *map)
{
    if (Z_TYPE_P(map) != IS_ARRAY) {
        this->clearMap();
        return;
    }

    HashTable *ht = Z_ARRVAL_P(map), *row_ht;
    int len = zend_hash_num_elements(ht), i;
    this->clearMap();
    if (len < this->m || len == 0) {

        return;
    }
    this->match_map.resize(len + 1, std::vector<char>(this->n + 1));
    zval *row, *cell;
    int j;

    for (i = 0, zend_hash_internal_pointer_reset(ht); ZEND_HASH_HAS_MORE_ELEMENTS(ht) == SUCCESS; ++i, zend_hash_move_forward(ht)) {
        row = ZEND_HASH_GET_CURRENT_DATA(ht);
        if (Z_TYPE_P(row) == IS_ARRAY) {
            row_ht = Z_ARRVAL_P(row);
            this->match_map[i].resize(zend_hash_num_elements(row_ht) + 1, 0);
            for (j = 0, zend_hash_internal_pointer_reset(row_ht); ZEND_HASH_HAS_MORE_ELEMENTS(row_ht) == SUCCESS; ++j, zend_hash_move_forward(row_ht)) {
                cell = ZEND_HASH_GET_CURRENT_DATA(row_ht);
                this->match_map[i][j] = (char) Z_LVAL_P(cell);
            }
        } else {
            return this->flushProcessed();
        }
    }
}

void Levenshtein::pushPath(int i, char pre)
{
    this->path[i].insert(0, 1, pre);
}

void Levenshtein::shiftPath(int i)
{
    int len = this->path[i].size();
    this->path[i].erase(0, 1);
}

char equals(double a, double b)
{
    return fabs(a - b) < 1e-6;
}

double Levenshtein::prev(int i, int j)
{
    double prev, delta, cval, rep_val, del_rep_val, ins_val;
    prev = 0xFFFF;
    cval = this->lv[i][j];
    if (i > 0 && j > 0) 
    {
        rep_val = this->lv[i - 1][j - 1];
        delta = cval - rep_val;
        if (equals(delta, 0) || equals(delta, this->replaceCost(i, j))) {
            prev = MIN(prev, rep_val);
        }
    }
    if (j > 0) {
        del_rep_val = this->lv[i][j - 1];
        delta = cval - del_rep_val;
        if (equals(delta, 0) || equals(delta, this->deleteCost(i, j)) || equals(delta, this->deleteCost(i, j, 1))) 
        {
            prev = MIN(prev, del_rep_val);
        }
    }
    if (i > 0) {
        ins_val = this->lv[i - 1][j];
        delta = cval - ins_val;
        if ((delta == 0 && this->isNonRequired(i)) || (this->isMultichar(i) && equals(delta, this->replaceCost(i, j))) || equals(delta, this->insertCost(i, j))) {
            prev = MIN(prev, ins_val);
        }
    }
    return prev;
}

zval *Levenshtein::getPath()
{
    int i = this->m;
    int j = this->n;
    if (this->blocks.empty()) {
        this->clearBlocks();
        this->clearSearches();
        this->blocks.resize(this->m + 1, "");
        this->searches.resize(this->m + 1, "");
    }
    if (this->path.empty()) {
        this->path.resize(this->m + 1, "");
        if (i > 0) {
            double prev, delta, cval, rep_val, del_rep_val, ins_val, rc;
            char match, imc;
            int skips = 0;

            int path_stor[this->n][2];

            int stor_i = 0xFFFF, stor_j = 0xFFFF, stor_pl = 0, k, ti, rp;

            long debug_pointer = 0;

            while (i >= 0 && j >= 0) {
                ++debug_pointer;
                if (debug_pointer > 1000)
                    break;
                if (this->isNonRequired(i)) {
                    if (stor_i == 0xFFFF) {
                        stor_i = i;
                        stor_j = j;
                        stor_pl = this->path[i].size();
                    }
                    path_stor[j][0] = i;
                    path_stor[j][1] = this->path[i].size();
                } else if (!this->isNonRequired(i) && stor_i != 0xFFFF) {
                    if (!this->match_map[i][j]) {
                        for (k = j + 1; k <= stor_j && stor_j != 0xFFFF; ++k) {
                            if (this->match_map[i][k]) {
                                prev = this->prev(i, k);
                                if (equals(prev, this->lv[i - 1][k - 1]) || (this->isMultichar(i) && equals(prev, this->lv[i][k - 1]))) {
                                    rp = path_stor[k][0];
                                    for (ti = i + 1; ti < rp; ti++) {
                                        this->path[ti].clear();
                                    }
                                    this->path[rp].erase(0, this->path[rp].size() - path_stor[k][1]);
                                    j = k;
                                    stor_j = 0xFFFF;
                                }
                            }
                        }
                    }
                    stor_i = 0xFFFF;
                    stor_j = 0xFFFF;
                }
                prev = this->prev(i, j);
                cval = this->lv[i][j];
                if (i > 0 && j > 0) {
                    rep_val = this->lv[i - 1][j - 1];
                }
                if (j > 0) {
                    del_rep_val = this->lv[i][j - 1];
                }
                if (i > 0) {
                    ins_val = this->lv[i - 1][j];
                }
                match = this->match_map[i][j];
                imc = this->isMultichar(i);
                if (match && imc && equals(this->lv[i][j - 1], prev)) {
                    skips = 0;
                    this->applyPattern(i, j, "M");
                    --j;
                } else if (match && equals(this->lv[i - 1][j - 1], prev)) {
                    skips = 0;
                    this->applyPattern(i, j, "M");
                    --j;
                    --i;
                } else if ( i > 0 && j > 1 && this->isNonRequired(i) && equals(this->lv[i - 1][j - 1], prev)) {
                    ++skips;
                    this->applyPattern(i, j, "D");
                    --j;
                    --i;
                } else if (i > 0 && this->match_map[i - 1][j] && this->isPatternNoise(i) && equals(ins_val, prev)) {
                    this->applyPattern(i, j, "I");
                    --i;
                } else if (j > 0 && imc && equals(del_rep_val, prev)) {
                    skips = 0;
                    this->applyPattern(i, j, "D");
                    --j;
                } else if (j > 0 && i > 0 && equals(rep_val, prev)) {
                    skips = 0;
                    this->applyPattern(i, j, "R");
                    --j;
                    --i;
                } else if (i > 0 && this->isNonRequired(i) && equals(ins_val, prev)) {
                    ++skips;
                    --i;
                } else if (i > 0 && equals(ins_val, prev)) {
                    skips = 0;
                    this->applyPattern(i, j, "I");
                    --i;
                } else if (j > 0 && equals(del_rep_val, prev)) {
                    skips = 0;
                    this->applyPattern(i, j, "D");
                    --j;
                } else if (j > 0) {
                    skips = 0;
                    this->applyPattern(i, j, "D");
                    --j;
                } else if (i > 0) {
                    skips = 0;
                    this->applyPattern(i, -1, "I");
                    --i;
                } else {
                    break;
                }
            }
        } else {
            while (j > 0) {
                this->path[i].insert(0, "D");
                --j;
            }
        }
    }

    zval *retval;
    MAKE_STD_ZVAL(retval);
    array_init(retval);
    zval *ht_val;

    for (i = 0; i <= this->m; ++i) {
        MAKE_PERSISTENT_ZVAL(ht_val);
        ZVAL_STRING_EX(ht_val, this->path[i].empty() ? "" : this->path[i].c_str());
        ZEND_HASH_UPDATE_INDEX(Z_ARRVAL_P(retval), i, ht_val);
    }

    return retval;
}

void Levenshtein::setPath(zval *path)
{
    if (Z_TYPE_P(path) != IS_ARRAY || this->lv.empty()) {
        this->clearPath();
        return;
    }
    HashTable *ht = Z_ARRVAL_P(path);;
    int len = zend_hash_num_elements(ht), i;
    this->clearPath();
    if (len < this->m || len == 0) {
        return;
    }
    this->path.resize(len + 1, "");
    zval *data;

    for (i = 0, zend_hash_internal_pointer_reset(ht); ZEND_HASH_HAS_MORE_ELEMENTS(ht) == SUCCESS; ++i, zend_hash_move_forward(ht)) {
        data = ZEND_HASH_GET_CURRENT_DATA(ht);
        this->path[i].assign(Z_STRVAL_P(data), Z_STRLEN_P(data));
    }
}

void Levenshtein::setBlocks(zval *blocks)
{
    if (Z_TYPE_P(blocks) != IS_ARRAY || this->lv.empty()) {
        this->clearBlocks();
        this->clearSearches();
        return;
    }
    HashTable *ht = Z_ARRVAL_P(blocks);
    int len = zend_hash_num_elements(ht), i;
    this->clearBlocks();
    if (len < this->m || len == 0) {

        return;
    }
    this->blocks.resize(len + 1, "");
    zval *data;

    for (i = 0, zend_hash_internal_pointer_reset(ht); ZEND_HASH_HAS_MORE_ELEMENTS(ht) == SUCCESS; ++i, zend_hash_move_forward(ht)) {
        data = ZEND_HASH_GET_CURRENT_DATA(ht);
        this->blocks[i].assign(Z_STRVAL_P(data), Z_STRLEN_P(data));

    }
}

void Levenshtein::setSearches(zval *searches) 
{
    if (Z_TYPE_P(searches) != IS_ARRAY || this->lv.empty()) {
        this->clearBlocks();
        this->clearSearches();
        return;
    }
    HashTable *ht = Z_ARRVAL_P(searches);;
    int len = zend_hash_num_elements(ht), i;
    this->clearSearches();
    if (len < this->m || len == 0) {

        this->clearBlocks();
        return;
    }
    this->searches.resize(len + 1, "");
    zval *data;

    for (i = 0, zend_hash_internal_pointer_reset(ht); ZEND_HASH_HAS_MORE_ELEMENTS(ht) == SUCCESS; ++i, zend_hash_move_forward(ht)) {
        data = ZEND_HASH_GET_CURRENT_DATA(ht);
        this->searches[i].assign(Z_STRVAL_P(data), Z_STRLEN_P(data));
    }
}


void Levenshtein::setCostIns(double cost)
{
    this->cost_ins = cost;
}

void Levenshtein::setCostDel(double cost)
{
    this->cost_del = cost;
}

void Levenshtein::setCostRep(double cost)
{
    this->cost_rep = cost;
}

double Levenshtein::getCostIns()
{
    return this->cost_ins;
}

double Levenshtein::getCostDel()
{
    return this->cost_del;
}

double Levenshtein::getCostRep()
{
    return this->cost_rep;
}

char Levenshtein::isPatternNoise(int i)
{
    if (i < 1 || i > this->m) {
        return 0;
    }
    if (this->isPattern(i)) {
        return this->isNoise(this->pattern[i - 1][1]);
    } else {
        return this->isNoise(this->pattern[i - 1][0]);
    }
}

char Levenshtein::isNoise(int j)
{
    if (j < 1 || j > this->n) {
        return 0;
    }
    return this->isNoise(this->string[j - 1]);
}

char Levenshtein::isNoise(char *str) 
{
    if (strlen(str) > 1) {
        return 0;
    }
    return this->isNoise(str[0]);
}

char Levenshtein::isNoise(char c) 
{
    switch (c) {
        case ' ':
        case '?':
        case '.':
        case ',':
        case ':':
        case '-':
        case '_':
        case ';':
        case '\'':
        case '"':
        case '`':
        case '<':
        case '>':
        case '*':
        case '(':
        case ')':
        case '|':
        case 'I':
        case '{':
        case '}':
        case '%':
        case '#':
        case '~':
        case 'x':
            return 1;
            break;
        default:
            return 0;
    }
}

char isDigit(std::string str) 
{
    return str.size() == 1 && isDigit(str[0]);
}

char isDigit(char c)
{
    switch (c) {
        case 'o':
        case 'Q':
        case 'O':
        case 'B':
        case 'D':
        case 'I':
        case 'c':
        case 'C':
        case '|':
        case 'g':
            return 1;
        default:
            return isdigit(c) != 0;
    }
}

char isAlpha(std::string str)
{
    return str.size() == 1 && isAlpha(str[0]);
}

char isAlpha(char c)
{
    switch (c) {
        case '0':
        case '8':
        case '4':
        case '5':
        case '|':
            return 1;
        default:
            return isalpha(c) != 0;
    }
}

char isAlphaDigit(char c)
{
    switch (c) {
        case '|':
            return 1;
        default:
            return isdigit(c) != 0 || isalpha(c) != 0;
    }
}


void Levenshtein::setPattern(zval *value)
{
    char buffer[32];
    switch(Z_TYPE_P(value)){
            case IS_ARRAY:
                this->setPattern(Z_ARRVAL_P(value));
                break;
            case IS_STRING:
                this->setPattern(Z_STRVAL_P(value));
                break;
#ifdef ZEND_ENGINE_2
            case IS_BOOL:
                this->setPattern(Z_BVAL_P(value)?(char*)"TRUE":(char*)"FALSE");
                break;
#endif
#ifdef ZEND_ENGINE_3
            case IS_FALSE:
                this->setPattern((char*)"FALSE");
                break;
            case IS_TRUE:
                this->setPattern((char*)"TRUE");
                break;
#endif                
            case IS_LONG:
                snprintf(buffer, sizeof(buffer), "%d", Z_LVAL_P(value));
                this->setPattern(buffer);
                break;
            case IS_DOUBLE:
                snprintf(buffer, sizeof(buffer), "%g", Z_DVAL_P(value));
                this->setPattern(buffer);
                break;
        }
}

void Levenshtein::setPattern(char *pattern) 
{
    this->flushProcessed();
    int p_len = strlen(pattern);
    this->clearPattern();
    this->pattern.resize(p_len + 1, "");
    long i = 0;
    long j = 0;
    char el[3];
    while (i < p_len) {
        if (pattern[i] == '%') {
            switch (pattern[i + 1]) {
                case 'o':/* one or more digits */
                case 'O':/* exactly one digit, filled with 0 if not present */
                case 'd':/* one or more digits, without leading zeroes */
                case 's':/* one or more of any symbols */
                case 'D':/* exactly one digit, empty if not present */
                case 'S':/* exactly one symbol */
                case 'N':/* one digit or nothing */
                case 'Z':/* one symbol or nothing */
                case 'L':/* one english letter or nothing */
                case 'E':/* exactly one english letter */
                case 'A':/* number or english letter */
                case '-':/* dash or space */
                    this->pattern[j].assign(1, '%');
                    this->pattern[j].append(1, pattern[++i]);
                    ++j;
                    break;
                case '%':
                    ++i;
                default:
                    this->pattern[j] = "\%";
                    ++j;
            }
        } else {
            this->pattern[j].assign(1, pattern[i]);
            ++j;
        }
        ++i;
    }
    this->pattern_len = j;
    this->m = j;
}

void Levenshtein::applyPattern(int i, int j, std::string action)
{
    std::string tstr = " ", ostr = "";
    char c = j > 0 && action.compare("I") != 0 ? this->string[j - 1] : '\0';
    if (action.compare("D") == 0) {
        if (c != '\0') this->searches[i].insert(0, 1, c);
        this->path[i].insert(0, action);
    } else if (this->isPattern(i)) {
        switch (this->pattern[i - 1][1]) {
            case 'o':/* one or more digits */
                tstr = "0";
                ostr.assign(1, toDigit(c));
                if (!isDigit(ostr)) { ostr = tstr; }
                if (ostr.compare("0") == 0) {
                    if (this->blocks[i].size() == 0) {
                        this->blocks[i].insert(0, "0");
                        if (c != '\0') this->searches[i].insert(0, 1, c);
                        this->path[i].insert(0, action);
                    }
                } else {
                    this->blocks[i].insert(0, ostr);
                    if (c != '\0') {
                        this->searches[i].insert(0, 1, c);
                    }
                    this->path[i].insert(0, action);
                }
                break;
            case 'O':/* exactly one digit, filled with 0 if not present */
                tstr = "0";
                ostr.assign(1, toDigit(c));
                if (!isDigit(ostr)) {
                    ostr = tstr; 
                }
                this->blocks[i].insert(0, ostr);
                if (c != '\0') {
                    this->searches[i].insert(0, 1, c);
                }
                this->path[i].insert(0, action);
                break;
            case 'N':/* one digit or nothing */
            case 'd':/* one or more digits, without leading zeroes */
            case 'D':/* exactly one digit, empty if not present */
                tstr = "";
                ostr.assign(1, toDigit(c));
                if (!isDigit(ostr)) {
                    ostr = tstr; 
                }
                if (ostr.size() > 0) {
                    this->blocks[i].insert(0, ostr);
                    if (c != '\0') {
                        this->searches[i].insert(0, 1, c);
                    }
                    this->path[i].insert(0, action);
                }
                break;
            case 'L':/* one english letter or nothing */
                tstr = "";
            case 'E':/* exactly one english letter */
                ostr.assign(1, toAlpha(c));
                if (!isAlpha(ostr)) {
                    ostr = tstr; 
                }
                if (ostr.size() > 0) {
                    this->blocks[i].insert(0, ostr);
                    if (c != '\0') {
                        this->searches[i].insert(0, 1, c);
                    }
                    this->path[i].insert(0, action);
                }
                break;
            case '-':/* dash or space */
                ostr.assign(1, toDash(c));
                this->blocks[i].insert(0, ostr);
                if (c != '\0') {
                    this->searches[i].insert(0, 1, c);
                }
                this->path[i].insert(0, action);
                break;
            case 'A':/* number or english letter */
            case 'Z':/* one symbol or nothing */
                tstr = "";
            case 's':/* one or more of any symbols */
            case 'S':/* exactly one symbol */
                if (c == '\0') {
                    ostr = tstr;
                } else {
                    ostr.assign(1, c);
                }
                if (ostr.size() > 0) {
                    this->blocks[i].insert(0, ostr);
                    if (c != '\0') {
                        this->searches[i].insert(0, 1, c);
                    }
                    this->path[i].insert(0, action);
                }
                break;
            default:
                ostr.assign(1, this->pattern[i - 1][1]);
                this->blocks[i].insert(0, ostr);
                if (c != '\0') {
                    this->searches[i].insert(0, 1, c);
                }
                this->path[i].insert(0, action);
        }
    } else if (i > 0) {
        ostr = this->pattern[i - 1];
        this->blocks[i].insert(0, ostr);
        if (c != '\0') {
            this->searches[i].insert(0, 1, c);
        }
        this->path[i].insert(0, action);
    }
}

char normalizeChar(char c)
{
    switch (c) {
        case 's':
        case '5':
        case 'S':
            return 's';
            break;
        case '0':
        case 'o':
        case 'c':
        case 'C':
        case 'O':
            return 'o';
        case '7':
        case 'Z':
            return '7';
            break;
        default:
            return tolower(c);
    }
}

char normalizedMatchChar(char pc, char sc)
{
    switch (pc) {
        case 's':
        case 'S':
            return toDigit(sc) == '5' || tolower(sc) == 's';
            break;
        case 'O':
        case 'o':
            return toDigit(sc) == '0' || tolower(sc) == 'o';
            break;
        case 'D':
            return sc == '0' || tolower(sc) == 'd';
            break;
        case 'c':
        case 'C':
            return sc == '0' || tolower(sc) == 'c';
            break;
        case 'B':
            return toDigit(sc) == '8' || tolower(sc) == 'b';
            break;
        case 'I':
        case 'i':
            return toDigit(sc) == '1' || tolower(sc) == 'i';
            break;
        case '0':
            return toDigit(sc) == '0';
            break;
        case '1':
            return toDigit(sc) == '1';
            break;
        case '4':
            return toDigit(sc) == '4';
            break;
        case '5':
            return toDigit(sc) == '5';
            break;
        case '8':
            return toDigit(sc) == '8';
            break;
        case '7':
            return toDigit(sc) == '7';
            break;
        case 'Z':
            return tolower(toAlpha(sc)) == 'z';
            break;
        default:
            return tolower(sc) == tolower(pc);
    }
}

char toDigit(char c)
{
    switch (c) {
        case 'B':
            return '8';
            break;
        case 's':
        case 'S':
            return '5';
            break;
        case 'D':
        case 'o':
        case 'O':
        case 'C':
        case 'c':
        case 'g':
        case 'Q':
            return '0';
            break;
        case '|':
        case 'I':
        case 'i':
            return 1;
            break;
        case 'A':
            return '4';
            break;
        case 'Z':
            return '7';
            break;
        default:
            return c;
    }
}

char toAlpha(char c)
{
    switch (c) {
        case '0':
            return 'O';
            break;
        case '1':
            return 'I';
            break;
        case '4':
            return 'A';
            break;
        case '5':
            return 'S';
            break;
        case '8':
            return 'B';
            break;
        case '7':
            return 'Z';
            break;
        default:
            return c;
    }
}

char toDash(char c)
{
    switch (c) {
        case '~':
        case '*':
        case '_':
        case '-':
            return '-';
            break;
        default:
            return ' ';
    }
}

char isDecimalSign(char c)
{
    switch (c) {
        case ' ':
        case '~':
        case '*':
        case '_':
        case '-':
            return 1;
        default:
            return 0;
    }
}


char Levenshtein::matchPattern(int i, int j)
{
    char s = this->string[j - 1];
    if (this->isPattern(i)) {
        switch (this->pattern[i - 1][1]) {
            case 'O':
            case 'o':
            case 'D':
            case 'd':
            case 'N':
                this->match_map[i][j] = isDigit(s);
                break;
            case 'E':
            case 'L':
                this->match_map[i][j] = isAlpha(s);
                break;
            case 'A':
                this->match_map[i][j] = isAlphaDigit(s);
                break;
            case '-':
                this->match_map[i][j] = isDecimalSign(s);
                break;
            case 'S':
            case 's':
            case 'Z':
                this->match_map[i][j] = 1;
                break;
            default:
                this->match_map[i][j] = normalizedMatchChar(this->pattern[i - 1][1], s);
        }
    } else {
        this->match_map[i][j] = normalizedMatchChar(this->pattern[i - 1][0], s);
    }
    return this->match_map[i][j];
}

void echo(char *str) {

}