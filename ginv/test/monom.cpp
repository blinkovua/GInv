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

#include "ginv/monom.h"

using namespace GInv;

class TestMonom: public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(TestMonom);
//   CPPUNIT_TEST(test1);
//   CPPUNIT_TEST(test2);
//   CPPUNIT_TEST(test3);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void test1();
  void test2();
  void test3();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestMonom);

void TestMonom::setUp() {
}

void TestMonom::tearDown() {
}


void TestMonom::test1() {
  Allocator allocator[1];
  Monom u(allocator, 0, 5,-1),
        v(allocator, 1, 5,-1),
        x(allocator, 2, 5,-1),
        y(allocator, 3, 5,-1),
        z(allocator, 4, 5,-1);
  Monom m(Monom(allocator, z, 4)*x*y);
  CPPUNIT_ASSERT(m.degree() == 6);
}

void TestMonom::test2() {
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
  for(int i=0; i < n; i++) {
    CPPUNIT_ASSERT(v[i]);
    CPPUNIT_ASSERT(!v[i].isZero());
    CPPUNIT_ASSERT(v[i].degree() == 1);
    CPPUNIT_ASSERT(v[i][i] == 1);
  }
  for(int i=1; i < n-1; i++) {
    Monom m(Monom(allocator, v[i+1], 7)*v[i]*v[i-1]*v[i]);
    m.setPos(12);
    CPPUNIT_ASSERT(!m.isZero());
    CPPUNIT_ASSERT(m.pos() == 12);
    CPPUNIT_ASSERT(m.degree() == 10);
    CPPUNIT_ASSERT(m.divisiable(v[i]));
    CPPUNIT_ASSERT(m.divisiable(v[i-1]));
    CPPUNIT_ASSERT(m.divisiable(Monom(allocator, v[i], 2)));
    CPPUNIT_ASSERT(m.divisiable(Monom(allocator, v[i+1], 7)));
    Monom d(allocator, m, Monom(allocator, v[i])*Monom(allocator, v[i+1], 2), true);
    CPPUNIT_ASSERT(d.pos() == 12);
    CPPUNIT_ASSERT(d.degree() == 7);
    CPPUNIT_ASSERT(d[i-1] == 1);
    CPPUNIT_ASSERT(d[i+1] == 5);
    CPPUNIT_ASSERT(d[i] == 1);
  }
}

void TestMonom::test3() {
  Allocator allocator[1];
  const int n=30;
  const int s=5, d1=2, d2=7;
  Monom::rand_init(s, d1, d2);
  Monom m(allocator, s, -1);
  for(int i=0; i < n; i++) {
    Monom::rand_next(m);
    CPPUNIT_ASSERT(d1 <= m.degree() && m.degree() <= d2);
//     std::cerr << m << std::endl;
  }
}
