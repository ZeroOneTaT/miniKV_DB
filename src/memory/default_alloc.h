/*
 * @Author: ZeroOneTaT
 * @Date: 2023-05-27 17:41:30
 * @LastEditTime: 2023-05-27 20:18:19
 * @FilePath: /miniKV/src/memory/default_alloc.h
 * @Description: 默认内存分配管理
 *
 * Copyright (c) 2023 by ZeroOneTaT, All Rights Reserved.
 */
#ifndef MINIKVDB_DEFAULT_ALLOC_H
#define MINIKVDB_DEFAULT_ALLOC_H

#include <cstdlib>

namespace minikvdb
{

    class DefaultAlloc
    {
    public:
        DefaultAlloc() = default;

        ~DefaultAlloc() = default;

        /**
         * @description:        内存分配函数
         * @param {int32_t} n   分配内存size
         * @return {*}          已分配内存
         */
        void *Allocate(int32_t n);

        /**
         * @description:        内存释放函数
         * @param {void} *p     已分配地址
         * @param {int32_t} n   已分配内存size
         * @return {*}
         */
        void Deallocate(void *p, int32_t n);

        /**
         * @description:                内存扩容函数
         * @param {void} *p             已分配地址
         * @param {int32_t} old_size    已分配内存size
         * @param {int32_t} new_size    扩容内存size
         * @return {*}
         */
        void *Reallocate(void *p, int32_t old_size, int32_t new_size);
    };
}

#endif