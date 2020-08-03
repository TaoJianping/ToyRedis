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
    static inline size_t sdsavail(const sds& s);
    char* makeRoomFor(size_t addLen);
    inline size_t sdsLen();
    void sdsgrowzero(size_t len);
};


#endif //TOYREDIS_SDS_H
