/*
 * @Author: ZeroOneTaT
 * @Date: 2023-05-27 21:08:34
 * @LastEditTime: 2023-05-27 21:34:54
 * @FilePath: /miniKV/src/log/log.h
 * @Description: 日志模块定义
 *
 * Copyright (c) 2023 by ZeroOneTaT, All Rights Reserved.
 */

#ifndef MINIKVDB_LOG_H
#define MINIKVDB_LOG_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdarg.h>
#include <pthread.h>
#include "../utils/lock.h"

using namespace std;

namespace minikvdb
{
    // 单例模式创建日志
    class Log
    {
    public:
        // C++11以后,使用局部变量懒汉不用加锁
        static Log *get_instance()
        {
            static Log instance;
            return &instance;
        }

        /**
         * @description:                日志初始化函数
         * @param {char} *file_name     日志输出文件名
         * @param {int} close_log       日志关闭标志
         * @param {int} log_buf_size    日志缓冲区大小
         * @param {int} split_lines     最大行数
         * @return {*}
         */
        bool init(const char *file_name, int close_log, int log_buf_size = 8192, int split_lines = 5000000);

        /**
         * @description:            日志书写函数
         * @param {int} level       日志级别
         * @param {char} *format    可变参数
         * @return {*}
         */
        void write_log(int level, const char *format, ...);

        /**
         * @description:        日志实时刷新函数
         * @return {*}
         */
        void flush(void);

    private:
        Log();
        virtual ~Log();

    private:
        char dir_name[128]; // 路径名
        char log_name[128]; // log文件名
        int m_split_lines;  // 日志最大行数
        int m_log_buf_size; // 日志缓冲区大小
        long long m_count;  // 日志行数记录
        int m_today;        // 因为按天分类,记录当前时间是那一天
        FILE *m_fp;         // 打开log的文件指针
        char *m_buf;
        bool m_is_async;   // 是否同步标志位
        MutexLock m_mutex; // 互斥锁
        int m_close_log;   // 关闭日志
    };

#define LOG_DEBUG(format, ...)                                    \
    if (0 == m_close_log)                                         \
    {                                                             \
        Log::get_instance()->write_log(0, format, ##__VA_ARGS__); \
        Log::get_instance()->flush();                             \
    }
#define LOG_INFO(format, ...)                                     \
    if (0 == m_close_log)                                         \
    {                                                             \
        Log::get_instance()->write_log(1, format, ##__VA_ARGS__); \
        Log::get_instance()->flush();                             \
    }
#define LOG_WARN(format, ...)                                     \
    if (0 == m_close_log)                                         \
    {                                                             \
        Log::get_instance()->write_log(2, format, ##__VA_ARGS__); \
        Log::get_instance()->flush();                             \
    }
#define LOG_ERROR(format, ...)                                    \
    if (0 == m_close_log)                                         \
    {                                                             \
        Log::get_instance()->write_log(3, format, ##__VA_ARGS__); \
        Log::get_instance()->flush();                             \
    }
}

#endif