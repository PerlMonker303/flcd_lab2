// lab2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SymbolTable.h"
#include <fstream>
#include "Compiler.h"
#include <assert.h>

// Questions:
// - none yet

// Todo:
// - write the documentation
// - work with () paranthesis

int main()
{
    Compiler* compiler = new Compiler("token.in", "syntax.in", "p2.in");
    compiler->displayCodes();
    compiler->scan();
    // compiler->displayPif();
    compiler->displaySymbolTable();
    compiler->writeToFiles("PIF.out", "ST.out", "result.out");

    
    /*
    // SymbolTable tests
    SymbolTable* st = new SymbolTable(4);

    // insert
    std::pair<int, int> p(1, 0);
    assert(st->insert("a") == p);
    p = std::pair<int, int>(2, 0);
    assert(st->insert("b") == p);
    p = std::pair<int, int>(0, 0);
    assert(st->insert("T") == p);
    p = std::pair<int, int>(3, 0);
    assert(st->insert("c") == p);
    p = std::pair<int, int>(-1, -1); // insert duplicate
    assert(st->insert("c") == p);
    st->print();
    assert(st->exists("a") == true);
    assert(st->exists("b") == true);
    assert(st->exists("T") == true);
    assert(st->exists("c") == true);
    assert(st->exists("d") == false);
    assert(st->getAllElements().size() == 4);

    // resize operation performed here
    st->insert("d");
    st->print();
    assert(st->exists("a") == true);
    assert(st->exists("b") == true);
    assert(st->exists("T") == true);
    assert(st->exists("c") == true);
    assert(st->exists("d") == true);
    assert(st->getAllElements().size() == 5);

    // getPosition
    std::pair<int, int> p10(1, 0);
    assert(st->getPosition("a") == p10);
    std::pair<int, int> p41(4, 1);
    assert(st->getPosition("d") == p41);

    // hash
    int r = ((int)'a' + (int)'b' + (int)'c')%st->getCapacity();
    assert(st->hash("abc") == r);
    */
}