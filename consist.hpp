//
// Created by илья on 12.05.2021.
//

#ifndef PROJECTCONSIST4_CONSIST_HPP
#define PROJECTCONSIST4_CONSIST_HPP

//
// Created by илья on 05.03.2021.
//

#include <iostream>
#include <queue>

#include <numeric>
#include <cmath>
#include <memory>

#include <stdexcept>
#include <shared_mutex>
#include <mutex>
#include <iostream>

template<typename T>
class consList;

template<typename T>
class consIterator;

//---node---------------------------------------------------------------------------------------------------------------

template<typename T>
class consNode {
public:

    consNode() = default;

    consNode(T data, int ref_count) :
            data(std::move(data)), prev(this), next(this), deleted(false), ref_count(ref_count) {}

    consNode(T data, consList<T> *list) : consNode(data, 2) {}

    consNode(const consNode<T> &) = delete;

    ~consNode() = default;

    void operator=(const consNode<T> &) = delete;

private:
    T data;
    consNode<T> *next, *prev;
    bool deleted{};
    unsigned int ref_count{};

    friend class consList<T>;

    friend class consIterator<T>;
};

//---iterator-----------------------------------------------------------------------------------------------------------

template<typename T>
class consIterator {
public:

    consIterator<T>() noexcept = default;

    consIterator<T>(const consIterator<T> &iter) : current(iter.current), container(iter.container) {
        container->refCountInc(current);
    }

    consIterator<T>(consNode<T> *_newNode, consList<T> *_list) : current(_newNode), container(_list) {
        container->refCountInc(current);
    }

    ~consIterator<T>() {
        if (current) {
            container->refCountDec(current);
            if (current->ref_count == 0) current = nullptr;
        }
    }

    consIterator<T> &operator=(const consIterator<T> &right) {
        if (container != right.container)
            std::unique_lock<std::shared_mutex> lock2(right.container->mutex_, std::try_to_lock);
        std::unique_lock<std::shared_mutex> lock1(container->mutex_, std::try_to_lock);
        consList<T>::refCountDec(current);
        this->current = right.current;
        consList<T>::refCountInc(current);
        return *this;
    }

    T &operator*() const {
        std::shared_lock<std::shared_mutex> lock(container->mutex_);
        /*if (current->deleted) throw (std::out_of_range("Error 1: Invalid index!"));*/
        return current->data;
    }

    T *operator->() const {
        std::shared_lock<std::shared_mutex> lock(container->mutex_);
        /*if (current->deleted) throw (std::out_of_range("Error 2: Invalid index!"));*/
        return &(current->data);
    }

    //prefix
    consIterator<T> &operator++() {
        std::unique_lock<std::shared_mutex> lock(container->mutex_);
        if (!current->next) throw (std::out_of_range("Error 3: Invalid index!"));
        consNode<T> *prevNode = current;
        consNode<T> *newNode = current->next;
        if (newNode)
            while (newNode->deleted && newNode->next)
                newNode = newNode->next;
        container->refCountInc(newNode);
        this->current = newNode;
        container->refCountDec(prevNode);
        return *this;
    }

    //postfix
    consIterator operator++(int) {
        std::unique_lock<std::shared_mutex> lock(container->mutex_);
        if (!current->next) throw (std::out_of_range("Error 4: Invalid index!"));
        consNode<T> *prevNode = current;
        consIterator<T> iter(*this);
        if (current->next) {
            consNode<T> *newNode = current->next;
            if (newNode)
                while (newNode->deleted && newNode->next)
                    newNode = newNode->next;
            container->refCountInc(newNode);
            this->current = newNode;
            container->refCountDec(prevNode);
        }
        return iter;
    }

    //prefix
    consIterator<T> operator--() {
        std::unique_lock<std::shared_mutex> lock(container->mutex_);
        if (!current->prev->prev) throw (std::out_of_range("Error 5: Invalid index!"));
        consNode<T> *prevNode = current;
        consNode<T> *newNode = current->prev;
        if (newNode)
            while (newNode->deleted && newNode->prev)
                newNode = newNode->prev;
        container->refCountInc(newNode);
        this->current = newNode;
        container->refCountDec(prevNode);
        return *this;
    }

    //postfix
    consIterator operator--(int) {
        std::unique_lock<std::shared_mutex> lock(container->mutex_);
        if (!current->prev->prev) throw (std::out_of_range("Error 6: Invalid index!"));
        consNode<T> *prevNode = current;
        consNode<T> *newNode = current->prev;
        consIterator<T> new_iter(*this);
        if (newNode)
            while (newNode->deleted && newNode->prev)
                newNode = newNode->prev;
        container->refCountInc(newNode);
        this->current = newNode;
        container->refCountDec(prevNode);
        return new_iter;
    }

    friend bool operator==(const consIterator<T> &a, const consIterator<T> &b) {
        if (a.container != b.container)
            std::shared_lock<std::shared_mutex> lock2(a.container->mutex_);
        std::shared_lock<std::shared_mutex> lock1(b.container->mutex_);
        return a.current == b.current;
    }

    friend bool operator!=(const consIterator<T> &a, const consIterator<T> &b) {
        if (a.container != b.container)
            std::shared_lock<std::shared_mutex> lock2(a.container->mutex_);
        std::shared_lock<std::shared_mutex> lock1(b.container->mutex_);
        return !(a == b);
    }

    operator bool() {
        std::shared_lock<std::shared_mutex> lock(container->mutex_);
        return current;
    }

    T get() {
        std::shared_lock<std::shared_mutex> lock(container->mutex_);
        return current->data;
    }

    void set(T _data) {
        std::unique_lock<std::shared_mutex> lock(container->mutex_);
        current->data = _data;
    }

    int refCount() {
        std::shared_lock<std::shared_mutex> lock(container->mutex_);
        return current->ref_count;
    }

private:
    consList<T> *container;
    consNode<T> *current;

    friend class consList<T>;
};

//---list---------------------------------------------------------------------------------------------------------------

template<typename T>
class consList {
public:
    std::shared_mutex mutex_;

    consList() : Head(nullptr), Tail(nullptr), size(0) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        Tail = new consNode<T>();
        Head = new consNode<T>();
        Tail->prev = Head;
        Head->next = Tail;
        refCountInc(Head);
        refCountInc(Head);
        refCountInc(Tail);
        refCountInc(Tail);
    }

    consList(const consList<T> &iter) = delete;

    consList(consList<T> &&list) = delete;

    consList(std::initializer_list<T> list) : consList() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        for (auto iter = list.begin(); iter < list.end(); iter++) {
            consIterator<T> iterTail(Tail, this);
            consList<T>::insert_logic(iterTail, *iter);
        }
    }

    ~consList<T>() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        consNode<T> *current = Head;
        while (current != nullptr) {
            consNode<T> *next = current->next;
            delete current;
            current = next;
        }
    }

    static void refCountInc(consNode<T> *node) {
        if (!node) return;
        node->ref_count++;
    }

    static void refCountDec(consNode<T> *node) {
        if (!node) return;
        auto cur = node;
        auto nodeDie = node;
        std::queue<consNode<T> *> deleted_nodes;
        cur->ref_count--;
        if (cur->ref_count == 0) {
            deleted_nodes.push(cur);
            while (!deleted_nodes.empty()) {
                cur->next->ref_count--;
                cur->prev->ref_count--;
                if (cur->next->ref_count == 0) deleted_nodes.push(cur->next);
                if (cur->prev->ref_count == 0) deleted_nodes.push(cur->prev);
                nodeDie = deleted_nodes.front();
                deleted_nodes.pop();
                delete (nodeDie);
                cur = deleted_nodes.front();
            }
        }
    }

    consList<T> &operator=(const consList &list) = delete;

    consList<T> &operator=(consList &&list) = delete;

    consIterator<T> insert(consIterator<T> iter, T data) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        return insert_logic(iter, data);
    }


    void pushBack(const T &data) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        consIterator<T> iter(Tail, this);
        insert_logic(iter, T(data));
    }

    void pushBack(T &&data) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        consIterator<T> iter(Tail, this);
        insert_logic(iter, data);
    }

    void pushFront(const T &data) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        consIterator<T> iter(Head->next, this);
        insert_logic(iter, T(data));
    }

    void pushFront(T &&data) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        consIterator<T> iter(Head->next, this);
        insert_logic(iter, data);
    }

    consIterator<T> erase(consIterator<T> iter) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        return erase_logic(iter);
    }

    consIterator<T> begin() noexcept {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        consIterator<T> iter(Head->next, this);
        return iter;
    }

    consIterator<T> end() noexcept {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        consIterator<T> iter(Tail, this);
        return iter;
    }

    std::size_t getSize() noexcept {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return size;
    }

    bool empty() noexcept {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return Head->next == Tail;
    }

private:
    consNode<T> *Head, *Tail;
    std::size_t size;

    consIterator<T> insert_logic(consIterator<T> iter, T data) {
        if (iter.current == nullptr) return iter;
        consNode<T> *newNode = new consNode<T>{std::move(data), 2};
        newNode->prev = iter.current->prev;
        newNode->next = iter.current;
        iter.current->prev->next = newNode;
        iter.current->prev = newNode;
        consIterator<T> newIter(newNode, this);
        size++;
        return newIter;
    }

    consIterator<T> erase_logic(consIterator<T> iter) {
        if (iter.current->deleted) return iter;
        auto outIter = consIterator<T>(iter.current->next, this);
        refCountInc(iter.current->next);
        refCountInc(iter.current->prev);
        if (iter.current == Head->next)
            Head->next = iter.current->next;
        else
            iter.current->prev->next = iter.current->next;
        if (iter.current == Tail->prev)
            Tail->prev = iter.current->prev;
        else
            iter.current->next->prev = iter.current->prev;
        size--;
        iter.current->deleted = true;
        refCountDec(iter.current);
        refCountDec(iter.current);
        return outIter;
    }

    friend class consIterator<T>;
};

#endif //PROJECTCONSIST4_CONSIST_HPP