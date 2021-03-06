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

#include <catch2/catch.hpp>
#include "testing_util.h"

TEST_CASE("Test container types with static item types") {
    SECTION("Tuple of ints") {
        auto t = InstructionGraphTest("def f():\n"
                              "  x = (1, 2, 3)\n"
                              "  a, b, c = x\n"
                              "  return a + b + c","assert_int_tuple");

        t.assertInstruction(6, UNPACK_SEQUENCE, 3, false);// == should be unboxed, len is know
        t.assertInstruction(8, STORE_FAST, 1, true);// == should be unboxed, len is know
        t.assertInstruction(10, STORE_FAST, 2, true);// == should be unboxed, len is know
        t.assertInstruction(12, STORE_FAST, 3, true);// == should be unboxed, len is know
    }
    SECTION("Tuple of floats") {
        auto t = InstructionGraphTest("def f():\n"
                                      "  x = (1., 2., 3.)\n"
                                      "  a, b, c = x\n"
                                      "  return a + b + c","assert_float_tuple");

        t.assertInstruction(6, UNPACK_SEQUENCE, 3, false);// == should be unboxed, len is know
        t.assertInstruction(8, STORE_FAST, 1, true);// == should be unboxed, len is know
        t.assertInstruction(10, STORE_FAST, 2, true);// == should be unboxed, len is know
        t.assertInstruction(12, STORE_FAST, 3, true);// == should be unboxed, len is know
    }
}
