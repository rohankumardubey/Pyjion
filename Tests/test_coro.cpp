/*
* The MIT License (MIT)
*
* Copyright (c) Microsoft Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
*/

/**
  Test JIT code emission.
*/

#include <catch2/catch.hpp>
#include "testing_util.h"

TEST_CASE("Test yield/generators with YIELD_VALUE") {
    SECTION("common case") {
        auto t = EmissionTest("def f():\n"
                              "  def cr():\n"
                              "     yield 1\n"
                              "     yield 2\n"
                              "     yield 3\n"
                              "  gen = cr()\n"
                              "  return next(gen), next(gen), next(gen)\n");
        CHECK(t.returns() == "(1, 2, 3)");
    }

    SECTION("exhausted generator raises stopiteration case") {
        auto t = EmissionTest("def f():\n"
                              "  def cr():\n"
                              "     yield 1\n"
                              "     yield 2\n"
                              "  gen = cr()\n"
                              "  return next(gen), next(gen), next(gen)\n");
        CHECK(t.raises() == PyExc_StopIteration);
    }

    SECTION("test preservation of boxed variables") {
        auto t = EmissionTest("def f():\n"
                              "  def cr():\n"
                              "     x = '1'\n"
                              "     yield x\n"
                              "     x = '2'\n"
                              "     yield x\n"
                              "     x = '3'\n"
                              "     yield x\n"
                              "  gen = cr()\n"
                              "  return next(gen), next(gen), next(gen)\n");
        CHECK(t.returns() == "('1', '2', '3')");
    }
    SECTION("test preservation of unboxed variables") {
        auto t = EmissionTest("def f():\n"
                              "  def cr():\n"
                              "     x = 1\n"
                              "     yield x\n"
                              "     x = 2\n"
                              "     yield x\n"
                              "     x = 3\n"
                              "     yield x\n"
                              "  gen = cr()\n"
                              "  return next(gen), next(gen), next(gen)\n");
        CHECK(t.returns() == "(1, 2, 3)");
    }

    SECTION("test yield within branches.") {
        auto t = EmissionTest("def f():\n"
                              "  def cr():\n"
                              "     x = '2'\n"
                              "     if x == '2':\n"
                              "         yield 'a'\n"
                              "     else:\n"
                              "         yield 'b'\n"
                              "     yield 'c'\n"
                              "     x = x + '2'\n"
                              "     if x == '22':\n"
                              "         yield 'd'\n"
                              "     else:\n"
                              "         yield x\n"
                              "     yield 'c'\n"
                              "  gen = cr()\n"
                              "  return next(gen), next(gen), next(gen)\n");
        CHECK(t.returns() == "('a', 'c', 'd')");
    }

    SECTION("test yield within branches for boxable vars.") {
        auto t = EmissionTest("def f():\n"
                              "  def cr():\n"
                              "     x = 3\n"
                              "     if x == 3:\n"
                              "         yield 'a', x\n"
                              "     else:\n"
                              "         yield 'b'\n"
                              "     yield 'c'\n"
                              "     x = x + 1\n"
                              "     if x == 4:\n"
                              "         yield 'd'\n"
                              "     else:\n"
                              "         yield x\n"
                              "     yield 'c'\n"
                              "  gen = cr()\n"
                              "  return next(gen), next(gen), next(gen)\n");
        CHECK(t.returns() == "(('a', 3), 'c', 'd')");
    }
    SECTION("test yield within branches for boxable vars as iter.") {
        auto t = EmissionTest("def f():\n"
                              "  def cr():\n"
                              "     x = 2\n"
                              "     if x == 2:\n"
                              "         yield 'a'\n"
                              "     else:\n"
                              "         yield 'b'\n"
                              "     yield 'c'\n"
                              "     x = x + 2\n"
                              "     if x == 4:\n"
                              "         yield 'd'\n"
                              "     else:\n"
                              "         yield x\n"
                              "     yield 'c'\n"
                              "  return [x for x in cr()]\n");
        CHECK(t.returns() == "['a', 'c', 'd', 'c']");
    }

    SECTION("test range generator.") {
        auto t = EmissionTest("def f():\n"
                              "  def cr():\n"
                              "     for n in range(10):\n"
                              "         yield f'{n}!'\n"
                              "  return [x for x in cr()]\n");
        CHECK(t.returns() == "['0!', '1!', '2!', '3!', '4!', '5!', '6!', '7!', '8!', '9!']");
    }

    SECTION("test generator within generator.") {
        auto t = EmissionTest("def f():\n"
                              "  def evens(i):\n"
                              "     for n in range(10):\n"
                              "         if n % 2:\n"
                              "             yield n\n"
                              "  def tens():\n"
                              "     for n in evens(range(100)):\n"
                              "         if n % 10:\n"
                              "             yield f'{n}!'\n"
                              "  return [x for x in tens()]\n");
        CHECK(t.returns() == "['1!', '3!', '5!', '7!', '9!']");
    }
    SECTION("test nested yields and unpacker.") {
        auto t = EmissionTest("def f():\n"
                              "  def cr():\n"
                              "    for idx, letter in enumerate('banana'):\n"
                              "        if letter != 'a':\n"
                              "            yield idx\n"
                              "        elif letter == 'n':\n"
                              "            for x in range(5):\n"
                              "                yield idx\n"
                              "  return [x for x in cr()]\n");
        CHECK(t.returns() == "[0, 2, 4]");
    }
    SECTION("test non-enumerable.") {
        auto t = EmissionTest("def f():\n"
                              "  def cr():\n"
                              "    for x in 255:\n"
                              "        yield x\n"
                              "  return [x for x in cr()]\n");
        CHECK(t.raises() == PyExc_TypeError);
    }
    SECTION("test yielding a tuple.") {
        auto t = EmissionTest("def f():\n"
                              "  def cr():\n"
                              "     for n in range(10):\n"
                              "         yield f'{n}!', n\n"
                              "  return [x for x in cr()]\n");
        CHECK(t.returns() == "[('0!', 0), ('1!', 1), ('2!', 2), ('3!', 3), ('4!', 4), ('5!', 5), ('6!', 6), ('7!', 7), ('8!', 8), ('9!', 9)]");
    }
    SECTION("test yielding within a try statement.") {
        auto t = EmissionTest("def f():\n"
                              "  def cr():\n"
                              "     for n in range(10):\n"
                              "         try:\n"
                              "            yield f'{n}!', n\n"
                              "         except ZeroDivisionError:\n"
                              "            pass\n"
                              "  return [x for x in cr()]\n");
        CHECK(t.returns() == "[('0!', 0), ('1!', 1), ('2!', 2), ('3!', 3), ('4!', 4), ('5!', 5), ('6!', 6), ('7!', 7), ('8!', 8), ('9!', 9)]");
    }
    SECTION("test generator being closed and recreated on exhaustion") {
        auto t = EmissionTest("def f():\n"
                              "    def inner():\n"
                              "        names = ['__add__', '__alloc__',]\n"
                              "        for n in \"!@\":\n"
                              "            for letter in names:\n"
                              "                yield n + letter\n"
                              "\n"
                              "    return list(inner())");
        CHECK(t.returns() == "['!__add__', '!__alloc__', '@__add__', '@__alloc__']");
    }
    SECTION("test nested generator being closed and recreated on exhaustion") {
        auto t = EmissionTest("def f():\n"
                              "    def inner():\n"
                              "        names = ['__add__', '__alloc__',]\n"
                              "        for letter in names:\n"
                              "            yield letter\n"
                              "\n"
                              "    def outer():\n"
                              "        for n in \"!@\":\n"
                              "            for i in inner():\n"
                              "                yield n + i\n"
                              "    return [x for x in outer()]");
        CHECK(t.returns() == "['!__add__', '!__alloc__', '@__add__', '@__alloc__']");
    }
}