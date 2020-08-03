//
// Created by tao on 2020/7/29.
//

#include <cstring>
#include <cstdlib>
#include "sds.h"

sds::sds(const void *init, size_t initlen) {
    // 如果init是个nullptr, 就分配一个全部为0的数组
    // 不是就不用管,毕竟我们后面要覆盖的
    if (init) {
        this->buf = new char[initlen + 1];
    } else {
        this->buf = new char[initlen + 1]();
    }

    // C 源码还有对这里的一个分配失败的判断
    // if (data == nullptr)
    //     return;
    this->len = initlen;
    this->freeSpace = 0;

    if (init && initlen)
        memcpy(this->buf, init, initlen);
    this->buf[initlen] = '\0';
}

char *sds::data() {
    return this->buf;
}

sds::sds() : sds("", 0) {

}

sds::sds(const char *init) {
    size_t initlen = (init == nullptr) ? 0 : strlen(init);
    // 如果init是个nullptr, 就分配一个全部为0的数组
    // 不是就不用管,毕竟我们后面要覆盖的
    if (init) {
        this->buf = new char[initlen + 1];
    } else {
        this->buf = new char[initlen + 1]();
    }

    // C 源码还有对这里的一个分配失败的判断
    // if (data == nullptr)
    //     return;
    this->len = initlen;
    this->freeSpace = 0;

    if (init && initlen)
        memcpy(this->buf, init, initlen);
    this->buf[initlen] = '\0';
}

sds *sds::dup() {
    sds* ret = new sds(this->data(), strlen(this->data()));
    return ret;
}

sds::~sds() {
    delete this->buf;
}


/*
 * sds的clear操作并不会free char[]的buffer,而是简单首位置'\0'
 * */
size_t sds::clear() {
    int deleteNum = this->len;
    this->freeSpace = this->freeSpace + this->len;
    this->len = 0;
    this->buf[0] = '\0';
    return deleteNum;
}

size_t sds::sdsavail(const sds &s) {
    return s.freeSpace;
}


/*
 * 分配空间函数
 *      addlen参数你可以理解为,希望当前的可用空间至少多少
 * 分配策略
 *      1. 当原来的长度 + 希望分配的空间　< 1MB，就简单的　(当原来的长度 + 希望分配的空间) * 2
 *      2. 如果大于1MB，就是(当原来的长度 + 希望分配的空间) + 1MB
 * */
char* sds::makeRoomFor(size_t addlen) {
    // if you have enough space, just return
    if (this->freeSpace >= addlen)
        return nullptr;

    int newLen = this->len + addlen;
    if (newLen < SDS_MAX_PREALLOC) {
        newLen *= 2;
    } else {
        newLen += SDS_MAX_PREALLOC;
    }

    char* newRoom = new char[newLen + 1]();
    memcpy(newRoom, this->buf, this->len);
    delete [] this->buf;

    this->buf = newRoom;
    this->freeSpace = newLen - this->len;

    return this->buf;
}

size_t sds::sdsLen() {
    return this->len;
}


/* Grow the sds to have the specified length. Bytes that were not part of
 * the original length of the sds will be set to zero.
 *
 * if the specified length is smaller than the current length, no operation
 * is performed. */
/*
 * 将 sds 扩充至指定长度，未使用的空间以 0 字节填充。
 *
 * 返回值
 *  sds ：扩充成功返回新 sds ，失败返回 NULL
 *
 * 复杂度：
 *  T = O(N)
 */
void sds::sdsgrowzero(size_t l) {
    size_t curLen = this->len;

    if (l < curLen)
        return;
    this->makeRoomFor(l - curLen);


}

