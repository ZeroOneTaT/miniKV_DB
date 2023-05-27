/*
 * @Author: ZeroOneTaT
 * @Date: 2023-05-27 20:10:36
 * @LastEditTime: 2023-05-27 20:12:53
 * @FilePath: /miniKV/src/memory/default_alloc.cc
 * @Description: 默认内存分配函数实现
 *
 * Copyright (c) 2023 by ZeroOneTaT, All Rights Reserved.
 */

#include "default_alloc.h"

namespace minikvdb
{
    void *DefaultAlloc::Allocate(int32_t n)
    {
        return malloc(n);
    }

    void DefaultAlloc::Deallocate(void *p, int32_t n)
    {
        free(p);
    }

    void *DefaultAlloc::Reallocate(void *p, int32_t old_size, int32_t new_size)
    {
        return realloc(p, new_size);
    }
}