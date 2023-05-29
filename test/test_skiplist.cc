/*
 * @Author: ZeroOneTaT
 * @Date: 2023-05-29 16:44:56
 * @LastEditTime: 2023-05-29 17:24:43
 * @FilePath: /miniKV/test/test_skiplist.cc
 * @Description:  跳表测试模块
 *
 * Copyright (c) 2023 by ZeroOneTaT, All Rights Reserved.
 */

#include "../src/log/log.h"
#include "../src/memtable/skiplist.h"
#include "../src/memory/default_alloc.h"
#include <iostream>
using namespace std;

typedef uint64_t Key;
typedef string Value;

struct Comparator
{
    int operator()(const Key &a, const Key &b) const
    {
        if (a < b)
        {
            return -1;
        }
        else if (a > b)
        {
            return +1;
        }
        else
        {
            return 0;
        }
    }
};

int main(int argc, char **argv)
{
    auto alloc = std::make_shared<minikvdb::DefaultAlloc>();
    Comparator cmp;
    minikvdb::SkipList<Key, Value, Comparator> skiplist(cmp, alloc);

    printf("Start Testing MiniKVDB\n");
    skiplist.Insert(1, Value("hello"));
    skiplist.Insert(2, Value("world"));

    printf("size of skiplist: %d \n", skiplist.GetSize());
    std::cout << "Get value of key 1 :" << skiplist.Get(1).value() << std::endl;
    std::cout << "Get value of key 2 :" << skiplist.Get(2).value() << std::endl;

    skiplist.Delete(1);
    if (skiplist.Get(1) == std::nullopt)
        std::cout << "Value of key 1 is deleted" << std::endl;
    std::cout << "Get value of key 2 :" << skiplist.Get(2).value() << std::endl;

    return 0;
}