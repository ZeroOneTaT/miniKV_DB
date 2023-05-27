/*
 * @Author: ZeroOneTaT
 * @Date: 2023-05-27 20:55:19
 * @LastEditTime: 2023-05-27 21:00:45
 * @FilePath: /miniKV/src/utils/lock.h
 * @Description: 锁功能模块
 *
 * Copyright (c) 2023 by ZeroOneTaT, All Rights Reserved.
 */

#ifndef MINIKVDB_LOCK_H
#define MINIKVDB_LOCK_H

#include <mutex>
#include <atomic>

namespace minikvdb
{
    // 类似于lock_guard, T接受NullLock、MutexLock、SpinLock
    template <typename T>
    class ScopedLock
    {
    public:
        explicit ScopedLock(T &t) : local_lock(t)
        {
            local_lock.lock();
            is_locked = true;
        }

        ~ScopedLock()
        {
            unlock();
        }

        void lock()
        {
            if (!is_locked)
            {
                local_lock.lock();
                is_locked = true;
            }
        }

        void unlock()
        {
            if (is_locked)
            {
                local_lock.unlock();
                is_locked = false;
            }
        }

    private:
        T &local_lock;
        bool is_locked = false;
    };

    // 无锁
    class NullLock final
    {
    public:
        NullLock() = default;

        ~NullLock() = default;

        void lock() {}

        void unlock() {}
    };

    // 互斥锁
    class MutexLock final
    {
    public:
        MutexLock() = default;

        ~MutexLock() = default;

        void lock()
        {
            _mutex.lock();
        }

        void unlock()
        {
            _mutex.unlock();
        }

    private:
        std::mutex _mutex;
    };

    // 自旋锁
    class SpinLock final
    {
    public:
        SpinLock() = default;

        SpinLock(const SpinLock &) = delete;

        SpinLock &operator=(const SpinLock &) = delete;

        void lock()
        {
            while (flag.test_and_set())
                ;
        }

        void unlock()
        {
            flag.clear();
        }

    private:
        std::atomic_flag flag{0};
    };
}

#endif