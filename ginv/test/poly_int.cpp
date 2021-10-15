/***************************************************************************
 *   Copyright (C) 2015 by Blinkov Yu. A.                                  *
 *   BlinkovUA@info.sgu.ru                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/config/SourcePrefix.h>

#include "ginv/poly_int.h"

using namespace GInv;

class TestPolyInt: public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(TestPolyInt);
  CPPUNIT_TEST(test1);
//   CPPUNIT_TEST(test2);
//   CPPUNIT_TEST(test3);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void test1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestPolyInt);

void TestPolyInt::setUp() {
}

void TestPolyInt::tearDown() {
}


void TestPolyInt::test1() {
  Allocator allocator[1];
  const int n=8;
  Monom v[]={
    {allocator, 0, n, -1},
    {allocator, 1, n, -1},
    {allocator, 2, n, -1},
    {allocator, 3, n, -1},
    {allocator, 4, n, -1},
    {allocator, 5, n, -1},
    {allocator, 6, n, -1},
    {allocator, 7, n, -1},
  };
  PolyInt p[]={
    {allocator, PolyInt::TOP | PolyInt::lex, v[0]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[1]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[2]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[3]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[4]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[5]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[6]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[7]},
  };

  CPPUNIT_ASSERT(p[0].length() == 1);
//   PolyInt sum(allocator, u + v + z);
  p[5].add(p[0]);
//   z.mult(v);
//   z.add(v);
//   z.add(v);
//   z.add(y);
  p[5].add("-0x12368");
  std::cerr << p[5] << std::endl;
}
