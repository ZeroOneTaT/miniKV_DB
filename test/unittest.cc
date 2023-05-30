/*
 * @Author: ZeroOneTaT
 * @Date: 2023-05-30 16:05:12
 * @LastEditTime: 2023-05-30 16:05:43
 * @FilePath: /miniKV/test/unittest.cc
 * @Description:  综合测试模块
 *
 * Copyright (c) 2023 by ZeroOneTaT, All Rights Reserved.
 */
#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}