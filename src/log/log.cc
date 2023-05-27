/*
 * @Author: ZeroOneTaT
 * @Date: 2023-05-27 21:25:01
 * @LastEditTime: 2023-05-27 23:38:30
 * @FilePath: /miniKV/src/log/log.cc
 * @Description: 日志模块实现
 *
 * Copyright (c) 2023 by ZeroOneTaT, All Rights Reserved.
 */

#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include "log.h"
#include <pthread.h>
using namespace std;

namespace minikvdb
{
    Log::Log()
    {
        m_count = 0;
        m_is_async = false;
    }

    Log::~Log()
    {
        if (m_fp != NULL)
        {
            fclose(m_fp);
        }
    }

    // Log初始化函数
    bool Log::init(const char *file_name, int close_log, int log_buf_size, int split_lines)
    {
        // 参数初始化
        m_close_log = close_log;
        m_log_buf_size = log_buf_size;
        m_buf = new char[m_log_buf_size];
        memset(m_buf, '\0', m_log_buf_size);
        m_split_lines = split_lines;

        time_t t = time(NULL);
        struct tm *sys_tm = localtime(&t);
        struct tm my_tm = *sys_tm;

        const char *p = strrchr(file_name, '/');
        char log_full_name[256] = {0};

        if (p == NULL)
        {
            snprintf(log_full_name, 255, "%d_%02d_%02d_%s", my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday, file_name);
        }
        else
        {
            strcpy(log_name, p + 1);
            strncpy(dir_name, file_name, p - file_name + 1);
            snprintf(log_full_name, 255, "%s%d_%02d_%02d_%s", dir_name, my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday, log_name);
        }

        m_today = my_tm.tm_mday;

        m_fp = fopen(log_full_name, "a");
        if (m_fp == NULL)
        {
            return false;
        }

        return true;
    }

    void Log::write_log(int level, const char *format, ...)
    {
        struct timeval now = {0, 0};
        gettimeofday(&now, NULL);
        time_t t = now.tv_sec;
        struct tm *sys_tm = localtime(&t);
        struct tm my_tm = *sys_tm;
        char s[16] = {0};

        // 根据不同log等级写log文件
        switch (level)
        {
        case 0:
            strcpy(s, "[debug]:");
            break;
        case 1:
            strcpy(s, "[info]:");
        case 2:
            strcpy(s, "[warn]:");
        case 3:
            strcpy(s, "[error]:");
        default:
            strcpy(s, "[info]:");
            break;
        }

        // 写入一个log，对m_count++, m_split_lines最大行数
        m_mutex.lock();
        m_count++;

        // 不是一天 或者 日志行数达到上限，开启新的日志文件进行写入
        if (m_today != my_tm.tm_mday || m_count % m_split_lines == 0)
        {
            char new_log[256] = {0};
            fflush(m_fp);
            fclose(m_fp);
            char tail[16] = {0};

            snprintf(tail, 16, "%d_%02d_%02d_", my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday);

            if (m_today != my_tm.tm_mday)
            {
                snprintf(new_log, 255, "%s%s%s", dir_name, tail, log_name);
                m_today = my_tm.tm_mday;
                m_count = 0;
            }
            else
            {
                snprintf(new_log, 255, "%s%s%s.%lld", dir_name, tail, log_name, m_count / m_split_lines);
            }
            m_fp = fopen(new_log, "a");
        }

        // 进行写入
        m_mutex.unlock();

        va_list valst;
        va_start(valst, format);

        string log_str;
        m_mutex.lock();

        // 写入的具体时间内容格式
        int n = snprintf(m_buf, 48, "%d-%02d-%02d %02d:%02d:%02d.%06ld %s ",
                         my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday,
                         my_tm.tm_hour, my_tm.tm_min, my_tm.tm_sec, now.tv_usec, s);

        int m = vsnprintf(m_buf + n, m_log_buf_size - 1, format, valst);
        m_buf[n + m] = '\n';
        m_buf[n + m + 1] = '\0';
        log_str = m_buf;

        m_mutex.unlock();

        m_mutex.lock();
        fputs(log_str.c_str(), m_fp);
        m_mutex.unlock();

        va_end(valst);
    }

    void Log::flush(void)
    {
        m_mutex.lock();
        // 强制刷新写入流缓冲区
        fflush(m_fp);
        m_mutex.unlock();
    }
}