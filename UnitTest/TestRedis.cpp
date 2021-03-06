//
// Created by tao on 2020/7/30.
//

#include "TestRedis.h"
#include "gtest/gtest.h"
#include "DS/sds.h"

TEST(sds, testInit) {
    const char * testString = "hello";
    auto s = new sds(testString, 5);
    int ret = strcmp(testString, s->data());
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(5, strlen(s->data()));
}

TEST(sds, testInit1) {
    const char * testString = "hello";
    auto s = new sds(testString, 3);
    int ret = strcmp("hel", s->data());
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(3, strlen(s->data()));
}

TEST(sds, testClear1) {
    const char * testString = "hello";
    auto s = new sds(testString, 5);
    int ret = strcmp("hello", s->data());
    EXPECT_EQ(ret, 0);
    int num = s->clear();
    EXPECT_EQ(num, 5);
    EXPECT_EQ(*(s->data()), '\0');
}

TEST(sds, testMakeRoomFor) {
    const char * testString = "hello";
    auto s = new sds(testString, 5);
    char* ret = s->makeRoomFor(100);
    int ans = strcmp(testString, ret);
    EXPECT_EQ(ans, 0);
    EXPECT_EQ(s->sdsLen(), 5);
    int a = s->sdsavail();
    EXPECT_EQ(a, 205);
}


TEST(sds, testsdscatlen) {
    const char * testString = "hello";
    auto s = new sds(testString, 5);
    auto testC = "test";
    auto buf = s->sdscatlen(testC, 4);
    int ans = strcmp("hellotest", buf);
    EXPECT_EQ(ans, 0);
    EXPECT_EQ(9, s->sdsLen());
}
