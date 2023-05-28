/*
 * @Author: ZeroOneTaT
 * @Date: 2023-05-28 17:56:43
 * @LastEditTime: 2023-05-28 20:14:46
 * @FilePath: /miniKV/src/memtable/skiplist.h
 * @Description: 跳表功能模块
 *
 * ********************************
 *  线程安全
 *  借鉴于leveldb: https://github.com/google/leveldb/blob/main/db/skiplist.h
 * ********************************
 *
 * Copyright (c) 2023 by ZeroOneTaT, All Rights Reserved.
 */

#ifndef MINIKVDB_SKIPLIST_H
#define MINIKVDB_SKIPLIST_H

#include <atomic>
#include <cassert>
#include <cstdlib>

#include "random.h"
#include "memory/default_alloc.h"
#include "log/log.h"

namespace minikvdb
{
    // 跳表类
    template <typename Key, typename Value, class Comparator>
    class SkipList
    {
    private:
        struct Node;

    public:
        /**
         * @description:                    SkipList显式构造函数
         * @param {Comparator} cmp          key值比较函数
         * @param {DefaultAlloc} arena      内存分配器，这里使用的是DefaultAlloc
         * @return {*}
         */
        explicit SkipList(Comparator cmp, std::shared_ptr<DefaultAlloc> arena);

        // 删除复制拷贝函数
        SkipList(const SkipList &) = delete;
        SkipList &operator=(const SkipList &) = delete;

        /**
         * @description:            插入函数
         * @param {Key} &key        key
         * @param {Value} &value    value
         * @return {*}
         */
        void Insert(const Key &key, const Value &value);

        /**
         * @description:            删除函数
         * @param {Key} &key        key
         * @return {*}
         */
        void Delete(const Key &key);

        /**
         * @description:            检查key是否存在
         * @param {Key} &key        key
         * @return {*}              存在返回true，否则返回false
         */
        bool Contains(const Key &key);

        /**
         * @description:            查询函数
         * @param {Key} &key        key
         * @return {*}              存在返回value，否则返回nullopt
         */
        std::optional<Value> Get(const Key &key);

        /**
         * @description:            获取skiplist元素数量
         * @return {*}              skiplist元素数量
         */
        inline int GetSize() { return size_; }

        /**
         * @description:            获取skiplist已用空间
         * @return {*}              skiplist已用空间
         */
        inline int64_t GetMemUsage() { return mem_usage_; }

    public:
        // 迭代skiplist，主要是给MemTable中的MemeIterator调用
        class SkipListIterator
        {
        public:
            explicit SkipListIterator(const SkipList *list);

            // 如果当前iter指向的位置有效，则返回true
            bool Valid();

            const Key &key();

            const Value &value();

            void Next();

            // todo: Prev暂时不支持，需要修改底层的跳变api，后续有空再说
            void Prev() = delete;

            // 将当前node移到表头
            // 必须要先调用此函数才可以进行迭代
            void MoveToFirst();

        private:
            const SkipList *list_;
            Node *node; // 当前iter指向的节点
        };

    private:
        /**
         * @description:            获取插入值key的随机高度
         * @return {*}
         */
        int RandomLevel();

        /**
         * @description:            获取当前skiplist最大高度
         * @return {*}              max_height_
         */
        inline int GetMaxHeight() const
        {
            return max_height_.load(std::memory_order_relaxed);
        }

        /**
         * @description:                    找到key结点的前缀节点，也就是找到key的待插入位置
         * @param {Key} &key                待插入结点
         * @param {vector<Node *>} &prev    待插入结点前向结点
         * @return {*}
         */
        void FindPrevNode(const Key &key, std::vector<Node *> &prev);

        /**
         * @description:                    创建新结点
         * @param {Key} &key                结点key值
         * @param {int} level               结点层高
         * @param {Value} &value            结点value
         * @return {*}                      返回新创建结点
         */
        inline Node *NewNode(const Key &key, int level, const Value &value);

    private:
        // skiplist理论最大高度
        enum
        {
            kMaxHeight = 12
        };

        Node *head_;                          // 头结点，高度为kMaxHeight，不存数据
        std::shared_ptr<DefaultAlloc> arena_; // 内存分配管理器
        Comparator const compare_;            // 比较器
        std::atomic<int> max_height_;         // 跳表最大高度
        std::atomic<int64_t> size_ = 0;       // 表中数据量(kv键值对数量)
        std::atomic<int64_t> mem_usage_ = 0;  // kv键值对所占用的内存大小，单位：Byte

        Random rnd_; // 随机数，用于插入数据
    };

    // 实现细节

    template <typename Key, typename Value, class Comparator>
    // 跳表结点
    struct SkipList<Key, Value, Comparator>::Node
    {
    public:
        Node() = delete;

        explicit Node(const Key &key, int level, const Value &value) : key(key), value(value)
        {
            next.resize(level, nullptr);
        }

        ~Node() = default;

        inline int GetLevel() { return next.size(); }

        const Key key;
        Value value;
        // 原子操作
        std::atomic<std::vector<Node *>> next;
    };
}

#endif