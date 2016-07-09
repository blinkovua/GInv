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

#include "util/randpermutation.h"

using namespace GInv;

class TestRandPermutation: public CPPUNIT_NS::TestFixture {
  int mN;
  int mK;
  CPPUNIT_TEST_SUITE(TestRandPermutation);
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

CPPUNIT_TEST_SUITE_REGISTRATION(TestRandPermutation);

void TestRandPermutation::setUp() {
  mN = 20;
  mK = mN*mN;
}

void TestRandPermutation::tearDown() {
}

void TestRandPermutation::test1() {
  std::random_device rd;
  std::default_random_engine gen(rd());

  RandPermutation p(mN);
  for(int i=1; i < mN; i++)
    CPPUNIT_ASSERT(p[i] == i);

  for(int k=0; k < mK; k++) {
    p(gen);
    for(int i=1; i < mN; i++) {
      CPPUNIT_ASSERT(0 <= p[i] < mN);
      for(int j=i+1; j < mN; j++)
        CPPUNIT_ASSERT(p[i] != p[j]);
    }
  }
}

void TestRandPermutation::test2() {
  std::random_device rd;
  std::mt19937 gen(rd());

  RandPermutation p(mN);
  for(int i=1; i < mN; i++)
    CPPUNIT_ASSERT(p[i] == i);

  for(int k=0; k < mK; k++) {
    p(gen);
    for(int i=1; i < mN; i++) {
      CPPUNIT_ASSERT(0 <= p[i] < mN);
      for(int j=i+1; j < mN; j++)
        CPPUNIT_ASSERT(p[i] != p[j]);
    }
  }
}

void TestRandPermutation::test3() {
  std::random_device rd;
  std::default_random_engine gen(rd());

  RandPermutation p(100000);
  for(int i=1; i < mN; i++)
    CPPUNIT_ASSERT(p[i] == i);

  p(gen);
  for(int i=1; i < mN; i++) {
    CPPUNIT_ASSERT(0 <= p[i] < mN);
    for(int j=i+1; j < mN; j++)
      CPPUNIT_ASSERT(p[i] != p[j]);
  }
}
