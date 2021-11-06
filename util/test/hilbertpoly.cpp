/***************************************************************************
 *   Copyright (C) 2021 by Blinkov Yu. A.                                  *
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

#include <cstdlib>
#include <cstring>
#include <ctime>

#include "util/hilbertpoly.h"

using namespace GInv;

class TestHilbertPoly: public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(TestHilbertPoly);
  CPPUNIT_TEST(test1);
  CPPUNIT_TEST(test2);
  CPPUNIT_TEST(test3);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void test1();
  void test2();
  void test3();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestHilbertPoly);

void TestHilbertPoly::setUp() {
}

void TestHilbertPoly::tearDown() {
}


void TestHilbertPoly::test1() {
  const int n=5;
  int md[]={4, 2, -3, 0, -4, -4, -4, -4, -4, -2, -2, -3, 
    -4, -5, -5, -5, -5, -4, -4, -4, -4, -4, -3, -2, -5, 
    -6, -6, -5, -6, -5, -5, -4, -5, -4, -4, -6, -6, -7, 
    -6, -6, -6, -6, -6, -5, -7, -7, -7, -7, -7, -7};
  int multi[]={5, 4, 0, 3, 0, 0, 0, 0, 0, 2, 2, 1, 1, 0, 
    0, 0, 0, 1, 1, 1, 1, 1, 2, 3, 1, 0, 0, 1, 0, 1, 1, 2, 
    1, 2, 2, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1};
  HilbertPoly hp(n);
  hp.binomial(n, n);
  HilbertPoly tmp(n);
  for (int i=0; i < sizeof(md)/sizeof(int); i++) {
    tmp.binomial(md[i], multi[i]);
    hp.sub(tmp);
  }
  CPPUNIT_ASSERT(strcmp(hp[0], "70") == 0);
  for(int j=1; j < n; j++)
    CPPUNIT_ASSERT(hp.isZero(j));
}

void TestHilbertPoly::test2() {
  const int n=6;
  int md[]={5, 3, -3, -2, -3, -3, 1, -4, -4, -4, -4, -4, 
    -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -3, 
    -4, -3, -4, -1, -1, -4, -5, -5, -5, -5, -5, -5, -5, 
    -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -4, -5, -4, 
    -5, -5, -4, -5, -4, -4, -5, -5, -5, -5, -5, -5, -5,
    -5, -4, -3, -4, -4, -5, -4, -3, -4, -4, -2, -2, -4,
    -6, -6, -6, -6, -6, -6, -6, -6, -6, -6, -6, -6, -6, 
    -6, -6, -6, -5, -6, -4, -6, -5, -6, -6, -5, -6, -5,
    -5, -6, -6, -6, -6, -6, -6, -5, -6, -4, -5, -5, -5,
    -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -4, -5, 
    -4, -4, -4, -3, -4, -2, -7, -7, -7, -7, -7, -7, -7, 
    -7, -6, -6, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, 
    -7, -7, -6, -6, -7, -5, -7, -5, -6, -7, -7, -6, -7,
    -7, -6, -6, -5, -5, -6, -5, -5, -6, -6, -5, -5, -5,
    -6, -6, -5, -5, -4, -4, -4, -8, -8, -8, -7, -7, -7,
    -7, -7, -7, -7, -8, -7, -8, -7, -8, -7, -7, -7, -7,
    -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -6, -7, -7, 
    -6, -7, -7, -7, -7, -6, -6, -6, -6, -6, -8, -8, -8, 
    -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, 
    -8, -9};
  int multi[]={6, 5, 0, 1, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 3, 3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1,
    1, 0, 1, 2, 1, 1, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 0, 0, 1, 0, 1, 1, 
    0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 3, 2, 4, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 2, 0, 2, 1, 0, 0, 1, 0, 0, 1, 1, 2, 2, 1, 2, 2, 
    1, 1, 2, 2, 2, 1, 1, 2, 2, 3, 3, 3, 0, 0, 0, 1, 1, 1, 
    1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  HilbertPoly hp(n);
  hp.binomial(n, n);
  HilbertPoly tmp(n);
  for (int i=0; i < sizeof(md)/sizeof(int); i++) {
    tmp.binomial(md[i], multi[i]);
    hp.sub(tmp);
  }
  CPPUNIT_ASSERT(strcmp(hp[0], "156") == 0);
  for(int j=1; j < n; j++)
    CPPUNIT_ASSERT(hp.isZero(j));
}

void TestHilbertPoly::test3() {
  const int n=5;
  int md[]={4, 2, -3, 0, -4, -4, -4, -4, -4, -2, -2, -3, 
    -4, -5, -5, -5, -5, -4, -4, -4, -4, -4, -3, -2, -5, 
    -6, -6, -5, -6, -5, -5, -4, -5, -4, -4, -6, -6, -7, 
    -6, -6, -6, -6, -6, -5, -7, -7, -7, -7, -7, -7};
  int multi[]={5, 4, 0, 3, 0, 0, 0, 0, 0, 2, 2, 1, 1, 0, 
    0, 0, 0, 1, 1, 1, 1, 1, 2, 3, 1, 0, 0, 1, 0, 1, 1, 2, 
    1, 2, 2, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1};
  HilbertPoly hp;
  hp.setDim(n);
  hp.binomial(n, n);
  HilbertPoly tmp(n);
  for (int i=0; i < sizeof(md)/sizeof(int); i++) {
    tmp.binomial(md[i], multi[i]);
    hp.sub(tmp);
  }
  std::cerr << hp << std::endl;
}
