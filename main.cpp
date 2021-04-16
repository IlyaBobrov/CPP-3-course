
// Created by илья on 04.03.2021.


#define CATCH_CONFIG_MAIN

#include <cstdlib>
#include "Catch.hpp"
#include "Consist.hpp"
#include <list>

using namespace std;

#define fori(n) for(int i = 0; i < n; i++)

TEST_CASE("consList(), pushFront(), pushPack(), getSize()") {
    int n = 10;
    consList<int> list1;
    list<int> list2;
    int val = 0;
    list1.pushFront(val);
    list2.push_front(val);
    fori(n) {
        list1.pushBack(1);
        list2.push_back(1);
    }
    REQUIRE(list1.getSize() == n + 1);
    REQUIRE(list2.size() == list1.getSize());
}

TEST_CASE("consList l1{1,2,3,4,5}") {
    consList<int> l1{1, 2, 3, 4, 5};
    REQUIRE(l1.getSize() == 5);
}

TEST_CASE("*iter") {
    consList<int> list{1, 2, 3, 4, 5};
    consIterator<int> iterBegin = list.begin();
    consIterator<int> iterEnd = list.end();
    REQUIRE(*iterBegin == 1);
    REQUIRE(*--iterEnd == 5);
}

TEST_CASE("+-iter+-") {
    consList<int> l1{1, 2, 3, 4, 5};
    consIterator<int> iter = l1.begin();
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


