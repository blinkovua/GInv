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

#include "poly/monom.h"

using namespace GInv;

class MonomPoly: public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(MonomPoly);
  CPPUNIT_TEST(test1);
  CPPUNIT_TEST(test2);
  CPPUNIT_TEST_SUITE_END();

  int n;

public:
  void setUp();
  void tearDown();

  void test1();
  void test2();
//   void test3();
//   void test4();
//   void test5();
//   void test6();
//   void test7();
//   void test8();
};

CPPUNIT_TEST_SUITE_REGISTRATION(MonomPoly);

void MonomPoly::setUp() {
  n = 9;
}

void MonomPoly::tearDown() {
}

void MonomPoly::test1() {
//   Allocator allocator[1];
//   Monom **v=new *Monom[n];
//   for(int i=0; i < n; i++) {
//     v[i] = new Monom(i, n, -1, allocator);
//     CPPUNIT_ASSERT(v[i]);
//     CPPUNIT_ASSERT(!v[i].isZero());
//     CPPUNIT_ASSERT(v[i].degree() == 1);
//     CPPUNIT_ASSERT(v[i][i] == 1);
//   }
//   for(int i=0; i < n; i++) {
//     Monom m;
//     m = v[i]*v[0]*v[i]*v[1].pow(7);
//     std::cerr << m << " -> ";
//     CPPUNIT_ASSERT(!m.isZero());
//     CPPUNIT_ASSERT(m.degree() == 10);
//     CPPUNIT_ASSERT(v[i] | m);
//     CPPUNIT_ASSERT(!(m | v[i]));
//     m = m / v[i];
//     m = m / v[0];
//     std::cerr << m << std::endl;
//   }
//   delete[] v;
}

void MonomPoly::test2() {
//   Allocator allocator[1];
//   Monom *v=new Monom[n];
//   for(int i=0; i < n; i++) {
//     v[i] = Monom(i, n, -1, allocator);
//     CPPUNIT_ASSERT(v[i]);
//     CPPUNIT_ASSERT(!v[i].isZero());
//     CPPUNIT_ASSERT(v[i].degree() == 1);
//     CPPUNIT_ASSERT(v[i][i] == 1);
//   }
//   for(int i=0; i < n; i++) {
//     Monom m;
//     m = v[i]*v[0]*v[i]*v[1].pow(7);
//     CPPUNIT_ASSERT(!m.isZero());
//     CPPUNIT_ASSERT(m.degree() == 10);
//   }
//   delete[] v;
}
