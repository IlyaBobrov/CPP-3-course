//
// Created by илья on 05.03.2021.
//

#ifndef PROGRAM_FILES_CPP__CONSIST_HPP
#define PROGRAM_FILES_CPP__CONSIST_HPP

#include <iostream>
#include <queue>

template<typename T>
class consList;

template<typename T>
class consIterator;

//---node---------------------------------------------------------------------------------------------------------------

template<typename T>
class consNode {
public:
//    consNode() : next(nullptr), prev(nullptr), data(nullptr), ref_count(0) {};
    consNode() = default;

//    consNode(T data) : next(nullptr), prev(nullptr), data(data), ref_count(0) {}
    consNode(T data, int ref_count) :
            data(std::move(data)), prev(this), next(this), deleted(false), ref_count(ref_count) {}

    consNode(T data, consList<T> *list) : consNode(data, 2) {}
//    consNode(const consNode<T> &) = delete;

//    void operator=(const consNode<T> &) = delete;

private:
    T data;
    consNode *next, *prev;
    bool deleted{};
    unsigned int ref_count{};

    friend class consList<T>;

    friend class consIterator<T>;
};

//---iterator-----------------------------------------------------------------------------------------------------------

template<typename T>
class consIterator {
public:

//    consIterator() noexcept = default;

    consIterator(const consIterator &iter) : current(iter.current) {
        consList<T>::refCountInc(current);
    }

    explicit consIterator(consNode<T> *newNode) : current(newNode) {
        consList<T>::refCountInc(current);
    }

    ~consIterator() {
        if (current) {
            consList<T>::refCountDec(current);
            if (current->ref_count == 0) current = nullptr;
        }
    }

    consIterator &operator=(const consIterator &right) {
        consList<T>::refCountDec(current);
        this->current = right.current;
        consList<T>::refCountInc(current);
        return *this;
    }

    T &operator*() const {
        /*if (current->deleted) throw (std::out_of_range("Error 1: Invalid index!"));*/
        return current->data;
    }

    /*T *operator->() const {
        if (current->deleted)
            throw (std::out_of_range("Error 2: Invalid index!"));
        return &(current->data);
    }*/

    consIterator operator++() {
        if (!current->next) throw (std::out_of_range("Error 3: Invalid index!"));
        current = current->next;
        if (current) {
            while (this->current->deleted && current->next) current = current->next;
        }
        return *this;
    }

    consIterator operator++(T) {
        if (!current->next) throw (std::out_of_range("Error 4: Invalid index!"));
        consIterator iter(current);
        if (iter.current->next) {
            current = iter.current->next;
            while (current->deleted && current->next) current = current->next;
        }
        return iter;
    }

    consIterator operator--() {
        if (!current->prev->prev) throw (std::out_of_range("Error 5: Invalid index!"));
        if (current) {
            current = current->prev;
            while (this->current->deleted && current->prev) current = current->prev;
        }
        return *this;
    }

    consIterator operator--(T) {
        if (!current->prev->prev) throw (std::out_of_range("Error 6: Invalid index!"));
        consIterator iter(current);
        if (iter.current->prev) {
            current = iter.current->prev;
            while (current->deleted && current->prev) current = current->prev;
        }
        return iter;
    }

    friend bool operator==(const consIterator<T> &a, const consIterator<T> &b) {
        return a.current == b.current;
    }

    friend bool operator!=(const consIterator<T> &a, const consIterator<T> &b) {
        return !(a == b);
    }

    explicit operator bool() const {
        return current;
    }

//    consIterator operator=() const;

private:
    consList<T> *container;
    consNode<T> *current;

    friend class consList<T>;
};

//---list---------------------------------------------------------------------------------------------------------------

template<typename T>
class consList {
public:
    consList() : Head(nullptr), Tail(nullptr), size(0) {
        Tail = new consNode<T>();
        Head = new consNode<T>();
        Tail->prev = Head;
        Head->next = Tail;
        refCountInc(Head);
        refCountInc(Head);
        refCountInc(Tail);
        refCountInc(Tail);
    }

//    consList(const consList &iter) = delete;

//    consList(consList &&list) = delete;

    consList(std::initializer_list<T> list) : consList() {
        for (auto iter = list.begin(); iter < list.end(); iter++)
            pushBack(*iter);
    }

    ~consList() {
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
                if (cur->prev->ref_count == 0) deleted_nodes.push(cur->next);
                nodeDie = deleted_nodes.front();
                deleted_nodes.pop();
                delete (nodeDie);
                cur = deleted_nodes.front();
            }
        }

        /*auto cur = node;
        std::stack<consNode<T> *> deleted_nodes;
        deleted_nodes.push(cur);
        while (!deleted_nodes.empty()) {
            cur = deleted_nodes.top();
            deleted_nodes.pop();
            cur->ref_count--;
            if (cur->ref_count == 0) {
                deleted_nodes.push(cur);
                if (cur->next) {
                    deleted_nodes.push(cur->next);
                } else {
                    if (cur->prev) {
                        deleted_nodes.push(cur->prev);
                    } else {
                        deleted_nodes.pop();
                        delete (cur);
                    }
                }
            }
        }*/

        /*if (!node) return;
        node->ref_count--;
        //while
        if (node->ref_count == 0) {
            if (node->next)
                refCountDec(node->next);
            if (node->prev)
                refCountDec(node->prev);
            delete (node);
        }*/
    }

//    consList &operator=(const consList &list) = delete;

//    consList &operator=(consList &&list) = delete;

    //todo
    consIterator<T> insert(consIterator<T> iter, T data) {
        if (!iter) return iter;
        consNode<T> *newNode = new consNode<T>(std::move(data), 2);
        newNode->prev = iter.current->prev;
        newNode->next = iter.current;
        iter.current->prev->next = newNode;
        iter.current->prev = newNode;
        consIterator<T> newIter(newNode);
        size++;
        return newIter;
    }

    void pushBack(const T &data) {
        pushBack(T(data));
    }

    void pushBack(T &&data) {
        consIterator<T> iter(Tail);
        insert(iter, data);
    }

    void pushFront(const T &data) {
        pushFront(T(data));
    }

    void pushFront(T &&data) {
        consIterator<T> iter(Head->next);
        insert(iter, data);
    }

    consIterator<T> erase(consIterator<T> iter) {
        consIterator<T> iterOut(iter);
        iterOut++;
        refCountInc(iter.current->next);
        refCountInc(iter.current->prev);
        iter.current->prev->next = iter.current->next;
        iter.current->next->prev = iter.current->prev;
        size--;
        iter.current->deleted = true;
        refCountDec(iter.current);
        refCountDec(iter.current);
        return iterOut;
    }

    consIterator<T> begin() noexcept {
        consIterator<T> iter(Head->next);
        return iter;
    }

    consIterator<T> end() noexcept {
        consIterator<T> iter(Tail);
        return iter;
    }

    unsigned int getSize() {
        return size;
    }

private:
    consNode<T> *Head, *Tail;
    unsigned int size;

    friend class consIterator<T>;
};


#endif //PROGRAM_FILES_CPP__CONSIST_HPP
