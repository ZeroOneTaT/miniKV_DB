/*
 * @Author: ZeroOneTaT
 * @Date: 2023-05-28 17:46:34
 * @LastEditTime: 2023-05-30 18:01:21
 * @FilePath: /miniKV/src/memtable/skiplist.h
 * @Description: 跳表实现
 *
 * ********************************
 *  该模块实现借鉴于leveldb
 * ********************************
 *
 * Copyright (c) 2023 by ZeroOneTaT, All Rights Reserved.
 */

#include <memory>
#include <vector>
#include <cstdlib>
#include <utility>
#include <iostream>
#include <optional>
#include <cassert>

#include "../log/log.h"
#include "../memory/default_alloc.h"
#include "random.h"

#ifndef MINIKVDB_SKIPLIST_H
#define MINIKVDB_SKIPLIST_H

namespace minikvdb
{
    template <typename Key, typename Value, class Comparator>
    class SkipList
    {
        class Node;

    public:
        /**
         * @description:                            显示调用SkipList构造函数
         * @param {Comparator} cmp                  key比较函数
         * @param {shared_ptr<DefaultAlloc>} alloc  内存分配器
         * @return {*}
         */
        explicit SkipList(Comparator cmp, std::shared_ptr<DefaultAlloc> alloc);

        // 删除拷贝构造函数
        SkipList(const SkipList &) = delete;
        SkipList &operator=(const SkipList &) = delete;

        /**
         * @description:                key-vaue插入函数, key存在则修改value
         * @param {Key} &key            key
         * @param {Value} &value        value
         * @return {*}
         */
        void Insert(const Key &key, const Value &value);

        /**
         * @description:                删除key对应的value
         * @param {Key} &key            key
         * @return {*}
         */
        void Delete(const Key &key);

        /**
         * @description:                检查是否存在key
         * @param {Key} &key            key
         * @return {*}                  true/false
         */
        bool Contains(const Key &key);

        /**
         * @description:                key-value查找函数
         * @param {Key} &key            key
         * @return {*}                  存在返回value，不存在返回nullopt
         */
        std::optional<Value> Get(const Key &key);

        // 仅用于DEBUG：打印表
        void OnlyUsedForDebugging_Print_()
        {
            auto p = head_->next[0];
            std::cout << "============= DEBUG =============" << std::endl;
            for (int i = 0; i < size; ++i)
            {
                std::cout << "key_" << i << " = " << p->key << std::endl;
                p = p->next[0];
            }
            std::cout << "============= DEBUG =============" << std::endl;
        }

        inline int GetSize() { return size; }

        inline int64_t GetMemUsage() { return mem_usage; }

        /*
         * skiplist迭代器，主供MemTable中的MemeIterator调用
         */
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
         * @description:    随机生成level
         * @return {*}      随机level
         */
        int RandomLevel();

        /**
         * @description:    获取当前最大level
         * @return {*}      maxheight
         */
        int GetCurrentHeight();

        /**
         * @description:                    找到key结点的前缀结点，也就是找到key的待插入位置
         * @param {Key} &key                key
         * @param {vector<Node *>} &prev    key前缀结点
         * @return {*}
         */
        void FindPrevNode(const Key &key, std::vector<Node *> &prev);

        /**
         * @description:                    新建一个结点
         * @param {Key} &key                新结点的key
         * @param {int} level               新结点level
         * @param {Value} &value            新结点value
         * @return {*}
         */
        inline Node *NewNode(const Key &key, int level, const Value &value);

    private:
        enum
        {
            kMaxHeight = 12 // skiplist最大高度
        };

        Node *head_; // 头结点，高度为kMaxHeight，不存数据

        std::shared_ptr<DefaultAlloc> alloc;

        int max_level;             // 当前表的最大高度节点
        int64_t size = 0;          // 表中数据量(kv键值对数量)
        int64_t mem_usage = 0;     // kv键值对所占用的内存大小，单位：Byte
        Comparator const compare_; // 比较函数
        Random rand_;              // 随机数
    };

    /*================================================================
    *  skiplist 结点 Node 定义
    ================================================================*/

    template <typename Key, typename Value, class Comparator>
    class SkipList<Key, Value, Comparator>::Node
    {
    public:
        Node() = delete;

        Node(const Key &key, int level, const Value &value) : key(key), value(value)
        {
            next.resize(level, nullptr);
        }

        ~Node() = default;

        inline int GetLevel() { return next.size(); }

        const Key key;
        Value value;
        std::vector<Node *> next;
    };

    /*================================================================
    *  skiplist 迭代器功能实现
    ================================================================*/

    template <typename Key, typename Value, class Comparator>
    void SkipList<Key, Value, Comparator>::SkipListIterator::MoveToFirst()
    {
        node = list_->head_->next[0];
    }

    template <typename Key, typename Value, class Comparator>
    void SkipList<Key, Value, Comparator>::SkipListIterator::Next()
    {
        assert(Valid());
        node = node->next[0]; // 遍历肯定是在跳表最底层进行遍历，所以是0
    }

    template <typename Key, typename Value, class Comparator>
    const Key &SkipList<Key, Value, Comparator>::SkipListIterator::key()
    {
        assert(Valid());
        return node->key;
    }

    template <typename Key, typename Value, class Comparator>
    const Value &SkipList<Key, Value, Comparator>::SkipListIterator::value()
    {
        assert(Valid());
        return node->value;
    }

    template <typename Key, typename Value, class Comparator>
    bool SkipList<Key, Value, Comparator>::SkipListIterator::Valid()
    {
        return node != nullptr;
    }

    template <typename Key, typename Value, class Comparator>
    SkipList<Key, Value, Comparator>::SkipListIterator::SkipListIterator(const SkipList *list) : list_(list)
    {
        node = nullptr;
    }

    /*================================================================
    *  skiplist 主要功能实现
    ================================================================*/

    template <typename Key, typename Value, class Comparator>
    std::optional<Value> SkipList<Key, Value, Comparator>::Get(const Key &key)
    {
        int level = GetCurrentHeight() - 1;
        auto cur = head_;
        while (true)
        {
            auto next = cur->next[level];
            if (next == nullptr)
            {
                if (level == 0)
                {
                    // 遍历到这里说明key不存在
                    return std::nullopt;
                }
                else
                {
                    --level;
                }
            }
            else
            {
                if (compare_(next->key, key) == 0)
                {
                    return next->value; // 找到了
                }
                else if (compare_(next->key, key) < 0)
                {
                    cur = next;
                }
                else if (compare_(next->key, key) > 0)
                {
                    if (level == 0)
                    {
                        // 遍历到这里说明key不存在
                        return std::nullopt;
                    }
                    else
                    {
                        --level; // 在非最底层遇到了大于key的数，应该下降
                    }
                }
            }
        }
    }

    template <typename Key, typename Value, class Comparator>
    void SkipList<Key, Value, Comparator>::Delete(const Key &key)
    {
        if (Contains(key) == false)
        {
            LOG_WARN("%s", ("The value you want to delete does not exist. Key={}", key));
            printf("The value you want to delete does not exist. Key={}", key);
            return;
        }
        --size;

        // std::vector<Node *> prev(GetCurrentHeight(), nullptr);
        std::vector<Node *> prev(kMaxHeight, nullptr);

        int level = GetCurrentHeight() - 1;
        auto cur = head_;
        int level_of_target_node = -1; // 目标节点的层数
        while (true)
        {
            auto next = cur->next[level];
            if (next == nullptr)
            {
                if (level == 0)
                {
                    LOG_ERROR("%s", "A error point.");
                    printf("A error point.");
                    break; // 遍历完成. 实际上这个分支不可能到达
                }
                else
                {
                    --level;
                }
            }
            else
            {
                if (compare_(next->key, key) == 0)
                {
                    level_of_target_node = next->GetLevel();
                    prev[level] = cur;
                    --level;
                    if (level < 0)
                    {
                        break;
                    }
                }
                else if (compare_(next->key, key) < 0)
                {
                    cur = next;
                }
                else if (compare_(next->key, key) > 0)
                {
                    prev[level] = cur;
                    --level;
                    if (level < 0)
                    {
                        break;
                    }
                }
            }
        }

        // 更新内存占用
        // mem_usage -= sizeof(key);
        // mem_usage -= sizeof(prev[0]->next[0]->value); // prev[0]->next[0]指向待删除的节点
        mem_usage -= (typeid(key) == typeid(std::string) ? key.size() : sizeof(key));
        mem_usage -= (typeid(prev[0]->next[0]->value) == typeid(std::string) ? prev[0]->next[0]->value.size() : sizeof(prev[0]->next[0]->value));

        for (int i = 0; i < level_of_target_node; ++i)
        {
            if (prev[i] != nullptr)
            {
                assert(prev[i]->next[i] != nullptr);
                prev[i]->next[i] = prev[i]->next[i]->next[i];
            }
        }
    }

    template <typename Key, typename Value, class Comparator>
    bool SkipList<Key, Value, Comparator>::Contains(const Key &key)
    { // 存在key则返回true
        int level = GetCurrentHeight() - 1;
        auto cur = head_;
        while (true)
        {
            auto next = cur->next[level];
            if (next == nullptr)
            {
                if (level == 0)
                {
                    return false; // 已经遍历完成
                }
                else
                {
                    --level;
                }
            }
            else
            {
                if (compare_(next->key, key) == 0)
                {
                    return true;
                }
                else if (compare_(next->key, key) < 0)
                {
                    cur = next;
                }
                else if (compare_(next->key, key) > 0)
                {
                    if (level == 0)
                    {
                        return false; // 只有在最后一层，遇到大于key的时候才可以认为没找到
                    }
                    else
                    {
                        --level; // 在非最底层遇到了大于key的数，应该下降
                    }
                }
            }
        }
    }

    template <typename Key, typename Value, class Comparator>
    void SkipList<Key, Value, Comparator>::Insert(const Key &key, const Value &value)
    {
        if (Contains(key))
        {
            LOG_WARN("%s", ("A duplicate key was inserted. Key={}", key));
            printf("A duplicate key was inserted. Key={}\n", key);
            return;
        }
        // 更新size
        ++size;

        // 更新mem_usage
        // mem_usage += sizeof(key);
        // mem_usage += sizeof(value);
        mem_usage += (typeid(key) == typeid(std::string) ? key.size() : sizeof(key));
        mem_usage += (typeid(value) == typeid(std::string) ? value.size() : sizeof(value));

        // std::vector<Node *> prev(GetCurrentHeight(), nullptr);
        std::vector<Node *> prev(kMaxHeight, nullptr);

        // 找到key的前缀节点，并且存到prev中
        FindPrevNode(key, prev);
        int level_of_new_node = RandomLevel();
        max_level = std::max(level_of_new_node, max_level); // 更新最大高度
        auto newNode = NewNode(key, level_of_new_node, value);

        for (int i = 0; i < newNode->GetLevel(); ++i)
        {
            if (prev[i] == nullptr)
            {
                newNode->next[i] = nullptr;
                head_->next[i] = newNode;
            }
            else
            {
                newNode->next[i] = prev[i]->next[i];
                prev[i]->next[i] = newNode;
            }
        }
    }

    template <typename Key, typename Value, class Comparator>
    int SkipList<Key, Value, Comparator>::GetCurrentHeight()
    {
        return max_level;
    }

    template <typename Key, typename Value, class Comparator>
    typename SkipList<Key, Value, Comparator>::Node *SkipList<Key, Value, Comparator>::NewNode(const Key &key, int level, const Value &value)
    {
        // todo: 不确定FreeListAllocate实现有没有问题，
        //  所以此处先使用系统allocator，稳定了再换。
        return new Node(key, level, value);
    }

    template <typename Key, typename Value, class Comparator>
    void SkipList<Key, Value, Comparator>::FindPrevNode(
        const Key &key, std::vector<Node *> &prev)
    {
        int level = GetCurrentHeight() - 1;
        auto cur = head_;
        while (true)
        {
            auto next_node = cur->next[level];
            if (next_node == nullptr || compare_(next_node->key, key) >= 0)
            {
                prev[level] = cur;
                if (level > 0)
                {
                    --level; // 遍历到了非最底层的终点，下降一层继续遍历
                }
                else
                {
                    return;
                }
            }
            else
            { // next_node != nullptr && next_node->key < key
                cur = next_node;
            }
        }
    }

    template <typename Key, typename Value, class Comparator>
    int SkipList<Key, Value, Comparator>::RandomLevel()
    {

        static const unsigned int kBranching = 4;
        int level = 1;
        while (level < kMaxHeight && rand_.OneIn(kBranching))
        {
            ++level;
        }
        assert(level > 0);
        assert(level <= kMaxHeight);
        return level;
    }

    template <typename Key, typename Value, class Comparator>
    SkipList<Key, Value, Comparator>::SkipList(Comparator cmp, std::shared_ptr<DefaultAlloc> alloc)
        : compare_(cmp),
          alloc(std::move(alloc)),
          rand_(0xdeadbeef)
    {
        head_ = NewNode(Key(), kMaxHeight, Value());
        max_level = 1;
        size = 0;
        mem_usage = 0;
        Log::get_instance()->init("./MinikvLog", 0, 2000, 800000);
    }
}
#endif