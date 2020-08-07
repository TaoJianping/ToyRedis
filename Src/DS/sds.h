//
// Created by tao on 2020/7/29.
//

#ifndef TOYREDIS_SDS_H
#define TOYREDIS_SDS_H


#include <cstddef>

const int SDS_MAX_PREALLOC = 1024 * 1024;


class sds {
private:
    int freeSpace;
    int len;
    char* buf;
public:
    char* data();
    sds(const void *init, size_t initlen);
    sds();
    sds(const char *init);
    ~sds();

    sds* dup();
    size_t clear();
    size_t sdsavail();
    char* makeRoomFor(size_t addLen);
    size_t sdsLen();
    void sdsgrowzero(size_t len);
    char* sdsRemoveFreeSpace();
    size_t sdsAllocSize();
    void sdsIncrLen(int incr);
    char* sdscatlen(const void *t, size_t len);
    char* sdscat(const char *t);
};


#endif //TOYREDIS_SDS_H
