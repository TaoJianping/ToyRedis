//
// Created by tao on 2020/7/29.
//

#include <cstring>
#include <cstdlib>
#include <assert.h>
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
    sds *ret = new sds(this->data(), strlen(this->data()));
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


size_t sds::sdsavail() {
    return this->freeSpace;
}


/*
 * 分配空间函数
 *      addlen参数你可以理解为,希望当前的可用空间至少多少
 * 分配策略
 *      1. 当原来的长度 + 希望分配的空间　< 1MB，就简单的　(当原来的长度 + 希望分配的空间) * 2
 *      2. 如果大于1MB，就是(当原来的长度 + 希望分配的空间) + 1MB
 * */
char *sds::makeRoomFor(size_t addlen) {
    // if you have enough space, just return
    if (this->freeSpace >= addlen)
        return nullptr;

    // 你要分配的空间的基值 是原来的以使用的长度 + 要分配的空间
    // 然后在这个基础上使用分配策略
    int newLen = this->len + addlen;
    if (newLen < SDS_MAX_PREALLOC) {
        newLen *= 2;
    } else {
        newLen += SDS_MAX_PREALLOC;
    }

    char *newRoom = new char[newLen + 1]();
    memcpy(newRoom, this->buf, this->len);
    delete[] this->buf;

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
void sds::sdsgrowzero(size_t targetLen) {
    size_t curLen = this->sdsLen();

    if (targetLen < curLen)
        return;
    char *newPtr = this->makeRoomFor(targetLen - curLen);
    memset(newPtr + curLen, 0, targetLen - curLen + 1);

    // update property
//    this->len = targetLen;
//    this->freeSpace = targetLen - this.
}


/*
 * 回收 sds 中的空闲空间，
 * 回收不会对 sds 中保存的字符串内容做任何修改。
 *
 * 返回值
 *  sds ：内存调整后的 sds
 *
 * 复杂度
 *  T = O(N)
 * Reallocate the sds string so that it has no free space at the end. The
 * contained string remains not altered, but next concatenation operations
 * will require a reallocation.
 *
 * After the call, the passed sds string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 * */
char *sds::sdsRemoveFreeSpace() {
    if (this->freeSpace == 0)
        return nullptr;

    char *newPtr = new char[this->sdsLen() + 1]();
    this->freeSpace = 0;
    memcpy(newPtr, this->buf, this->sdsLen());
    delete[] this->buf;
    this->buf = newPtr;
    return nullptr;
}

size_t sds::sdsAllocSize() {
    return this->sdsLen() + this->freeSpace + 1;
}


/* Increment the sds length and decrements the left free space at the
 * end of the string according to 'incr'. Also set the null term
 * in the new end of the string.
 *
 * 根据 incr 参数，增加 sds 的长度，缩减空余空间，
 * 并将 \0 放到新字符串的尾端
 *
 * This function is used in order to fix the string length after the
 * user calls sdsMakeRoomFor(), writes something after the end of
 * the current string, and finally needs to set the new length.
 *
 * 这个函数是在调用 sdsMakeRoomFor() 对字符串进行扩展，
 * 然后用户在字符串尾部写入了某些内容之后，
 * 用来正确更新 free 和 len 属性的。
 *
 * Note: it is possible to use a negative increment in order to
 * right-trim the string.
 *
 * 如果 incr 参数为负数，那么对字符串进行右截断操作。
 *
 * Usage example:
 *
 * Using sdsIncrLen() and sdsMakeRoomFor() it is possible to mount the
 * following schema, to cat bytes coming from the kernel to the end of an
 * sds string without copying into an intermediate buffer:
 *
 * 以下是 sdsIncrLen 的用例：
 *
 * oldlen = sdslen(s);
 * s = sdsMakeRoomFor(s, BUFFER_SIZE);
 * nread = read(fd, s+oldlen, BUFFER_SIZE);
 * ... check for nread <= 0 and handle it ...
 * sdsIncrLen(s, nread);
 *
 * 复杂度
 *  T = O(1)
 */
void sds::sdsIncrLen(int incr) {
    // 确保 sds 空间足够
    assert(this->freeSpace >= incr);

    // 更新属性
    this->len += incr;
    this->freeSpace -= incr;

    // 这个 assert 其实可以忽略
    // 因为前一个 assert 已经确保 sh->free - incr >= 0 了
    assert(this->freeSpace >= 0);

    // 放置新的结尾符号
    this->buf[this->len] = '\0';
}


/*
 * 将长度为 len 的字符串 t 追加到 sds 的字符串末尾
 *
 * 返回值
 *  sds ：追加成功返回新 sds ，失败返回 NULL
 *
 * 复杂度
 *  T = O(N)
 * Append the specified binary-safe string pointed by 't' of 'len' bytes to the
 * end of the specified sds string 's'.
 *
 * After the call, the passed sds string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 * */
char* sds::sdscatlen(const void *t, size_t length) {
    size_t curLen = this->sdsLen();
    this->makeRoomFor(length);

    memcpy(this->buf + curLen, t, length);
    this->len = curLen + length;
    this->freeSpace = this->freeSpace - length;

    this->buf[this->len] = 0;

    return this->buf;
}

char *sds::sdscat(const char *t) {
    return this->sdscatlen(t, strlen(t));
}


