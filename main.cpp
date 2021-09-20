#define CATCH_CONFIG_MAIN

#include <cstdlib>
#include "Catch.hpp"
#include "consist.hpp"
#include <list>
#include <thread>
#include <chrono>
#include <vector>

using namespace std;

#define fori(n) for(int i = 0; i < n; i++)

/*int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}*/

TEST_CASE("pushFront(), pushBack()") {
    consList<int> l;
    l.pushFront(3);
    int val = 2;
    l.pushFront(val);
    l.pushFront(1);
    val = 0;
    l.pushFront(val);
    REQUIRE(l.getSize() == 4);
    l.pushBack(4);
    val = 5;
    l.pushBack(val);
    l.pushBack(6);
    val = 7;
    l.pushBack(val);
    REQUIRE(l.getSize() == 8);
}

TEST_CASE("consList(), pushFront(), pushPack(), getSize()") {
    int n = 5;
    int m = 5;
    consList<int> list1;
    list<int> list2;
    int val = 0;
    list1.pushFront(0);
    list1.pushFront(val);
    list2.push_front(0);
    list2.push_front(val);
    fori(n) {
        list1.pushBack(1);
        list2.push_back(1);
    }
    fori(n) {
        list1.pushBack(val);
        list2.push_back(val);
    }
    REQUIRE(list1.getSize() == n + m + 2);
    REQUIRE(list2.size() == list1.getSize());
}

TEST_CASE("*iter, begin(), end()") {
    consList<int> list{1, 2, 3, 4, 5};
    auto iterBegin = list.begin();
    auto iterEnd = list.end();
    auto p = *iterBegin;
    REQUIRE(p == 1);
    REQUIRE(*iterBegin == 1);
    REQUIRE(*iterEnd == 0);
    list.pushFront(9);
    REQUIRE(*iterBegin == 1);
    iterBegin = list.begin();
    REQUIRE(*iterBegin == 9);
    list.pushBack(6);
    REQUIRE(*iterEnd == 0);
    iterEnd--;
    REQUIRE(*iterEnd == 6);
}


TEST_CASE("++iter") {
    consList<int> l1{1, 2, 3, 4, 5};
    auto iter = l1.begin();
    ++iter;
    REQUIRE(*iter == 2);
}

TEST_CASE("iter++") {
    consList<int> l1{1, 2, 3, 4, 5};
    auto iter = l1.begin();
    iter++;
    REQUIRE(*iter == 2);
}

TEST_CASE("*(++iter)") {
    consList<int> l1{1, 2, 3, 4, 5};
    auto iter = l1.begin();
    REQUIRE(*(++iter) == 2);
    REQUIRE(*iter == 2);
}

TEST_CASE("*(iter++)") {
    consList<int> l1{1, 2, 3, 4, 5};
    auto iter = l1.begin();
    REQUIRE(*(iter++) == 1);
    REQUIRE(*iter == 2);
}

TEST_CASE("+-iter+-") {
    consList<int> l1{1, 2, 3, 4, 5};
    auto iter = l1.begin();
    REQUIRE(*(++iter) == 2);
    REQUIRE(*(--iter) == 1);
    REQUIRE(*(iter++) == 1);
    REQUIRE(*iter == 2);
    REQUIRE(*(iter--) == 2);
    REQUIRE(*iter == 1);
}

TEST_CASE("insert") {
    consList<int> list{0, 0, 0, 0, 0};
    consIterator<int> iter1 = list.begin();
    consIterator<int> iter2 = list.begin();
    iter1 = list.insert(iter1, 1);
    REQUIRE(list.getSize() == 6);
    REQUIRE(*iter1 == 1);
    REQUIRE(*iter2 == 0);
    consIterator<int> iter3 = list.begin();
    REQUIRE(*iter3 == 1);
    REQUIRE(*iter3 == *(--iter2));
    REQUIRE(*iter3 == *(iter2++));
    REQUIRE(*(++iter1) == *iter2);
    REQUIRE(iter1 == iter2);
    iter2++;
    REQUIRE(iter1 != iter2);

}

TEST_CASE("erase easy") {
    consList<int> list{1, 2, 3, 4, 5};
    consIterator<int> iter1 = list.begin();
    consIterator<int> iter2 = list.begin();
    ++iter1;
    ++iter2;
    list.erase(iter1);
    REQUIRE(*iter1 == 2);
    REQUIRE(*++iter1 == 3);
    REQUIRE(*iter2 == 2);
    REQUIRE(*(--iter1) == 1);
    REQUIRE(*(--iter2) == 1);
    REQUIRE(list.getSize() == 4);
}

TEST_CASE("erase consist--") {
    consList<int> list{1, 2, 3, 4, 5};
    consIterator<int> iter1 = list.begin();
    consIterator<int> iter2 = list.begin();
    consIterator<int> iter3 = list.begin();
    ++iter1;
    ++iter1; //стоит на 3
    ++iter2; //стоит на 2
    ++iter3;
    ++iter3; //стоит на 3
    list.erase(iter1); //удаляем 3
    list.erase(iter2); //удаляем 2
    REQUIRE(*iter1 == 3);
    REQUIRE(*iter2 == 2);
    REQUIRE(*iter3 == 3);
    REQUIRE(list.getSize() == 3);

    iter2 = list.begin();
    REQUIRE(*iter2 == 1);
    REQUIRE(*iter1 == 3);
    REQUIRE(*iter3 == 3);

    REQUIRE(*(++iter2) == 4);
    REQUIRE(*(--iter3) == 1);
    REQUIRE(*iter1 == 3);
    REQUIRE(*(++iter3) == 4);
    REQUIRE(*(++iter1) == 4);
}

TEST_CASE("erase consist++") {
    consList<int> list{1, 2, 3, 4, 5};
    consIterator<int> iter1 = list.begin();
    consIterator<int> iter2 = list.begin();
    consIterator<int> iter3 = list.begin();
    consIterator<int> iter4 = list.begin();
    ++iter1;
    ++iter1; //стоит на 3
    ++iter2; //стоит на 2
    ++iter3;
    ++iter3;
    ++iter3; //стоит на 4
    ++iter4;
    ++iter4; //стоит на 3
    list.erase(iter1); //удаляем 3
    list.erase(iter2); //удаляем 2
    list.erase(iter3); //удаляем 4
    REQUIRE(*iter1 == 3);
    REQUIRE(*iter4 == 3);
    REQUIRE(*iter2 == 2);
    REQUIRE(*iter3 == 4);
    REQUIRE(list.getSize() == 2);

    iter2 = list.begin();
    iter3 = list.end();
    --iter3;
    REQUIRE(*iter1 == 3);
    REQUIRE(*iter4 == 3);
    REQUIRE(*iter2 == 1);
    REQUIRE(*iter3 == 5);

    REQUIRE(*(++iter2) == 5);
    REQUIRE(*(--iter3) == 1);

    REQUIRE(*(--iter4) == 1);
    REQUIRE(*(++iter1) == 5);
}

TEST_CASE("operator==/!=") {
    consList<int> list{1, 2, 3, 4, 5};
    auto iter1 = list.begin();
    auto iter2 = list.begin();
    REQUIRE(iter1 == iter2);
    iter2++;
    REQUIRE(iter1 != iter2);
    REQUIRE(iter1.refCount() == 3);
    REQUIRE(iter2.refCount() == 3);
    iter1++;
    REQUIRE(iter1.refCount() == 4);
}

TEST_CASE("Thread push and size") {
    consList<int> list;
    std::vector<std::thread> vectorDebug;
    int threads = 100;
    int n = 100;
    for (int i = 0; i < threads; i++) {
        vectorDebug.push_back(std::thread([&]() {
            auto it = list.begin();
            for (unsigned long j = 0l; j < n; j++) {
                list.pushBack(1);
            }
        }));
    }
    for (int i = 0; i < threads; i++) {
        vectorDebug[i].join();
    }
    REQUIRE(list.getSize() == (threads * n));
}

TEST_CASE("Check speed single and multi threads") {
    consList<int> list{1, 2, 3, 4, 5};
    std::vector<std::thread> vectorDebug;
    int threads = 5;
    int operations = 5000000;
    CHECK(operations % threads == 0);
    auto startMultiThreaded = std::chrono::high_resolution_clock::now();
    for (int j = 0; j < threads; j++) {
        vectorDebug.push_back(std::thread([&]() {
            for (unsigned long i = 0l; i < operations / threads; i++) {
                list.getSize();
                list.getSize();
                list.getSize();
                list.getSize();
                list.getSize();
                list.getSize();
                list.empty();
                list.empty();
                list.empty();
                list.empty();
                list.empty();
                list.empty();
            }
        }));
    }
    auto endMultiThread = std::chrono::high_resolution_clock::now();
    for (int j = 0; j < threads; j++) {
        vectorDebug[j].join();
    }
    auto startSingleThread = std::chrono::high_resolution_clock::now();
    for (unsigned long i = 0l; i < operations; i++) {
        list.getSize();
        list.getSize();
        list.getSize();
        list.getSize();
        list.getSize();
        list.getSize();
        list.getSize();
        list.empty();
        list.empty();
        list.empty();
        list.empty();
        list.empty();
        list.empty();
        list.empty();
    }
    auto endSingleThread = std::chrono::high_resolution_clock::now();
    auto timeMultiThread = std::chrono::duration_cast<std::chrono::microseconds>(endMultiThread - startMultiThreaded);
    auto timeSingleThread = std::chrono::duration_cast<std::chrono::microseconds>(endSingleThread - startSingleThread);
    REQUIRE(timeSingleThread > timeMultiThread);
}