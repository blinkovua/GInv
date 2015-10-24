/***************************************************************************
 *   Copyright (C) 2011 by Blinkov Yu. A.                                  *
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

#include "util/stack.h"

using namespace GInv;

class TestStack: public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(TestStack);
  CPPUNIT_TEST(test1);
  CPPUNIT_TEST(test2);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void test1();
  void test2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestStack);

void TestStack::setUp() {
}

void TestStack::tearDown() {
}

void TestStack::test1() {
  Allocator allocator[1];
  Stack<int> a(2, allocator);
  CPPUNIT_ASSERT(!a);
  CPPUNIT_ASSERT(a.deep() == 0);
  for(int i=1; i < 12; i++) {
    a.push(i);
    CPPUNIT_ASSERT(a);
    CPPUNIT_ASSERT(a.deep() == i);
    for(int j=0; j < i; j++)
      CPPUNIT_ASSERT(a[j] == j+1);
  }
  Stack<int> b(a, allocator);
  for(int i=11; i > 0; i--) {
    CPPUNIT_ASSERT(b);
    CPPUNIT_ASSERT(b.deep() == i);
    for(int j=0; j < i; j++)
      CPPUNIT_ASSERT(b[j] == j+1);
    b.pop();
  }
  CPPUNIT_ASSERT(!b);
  CPPUNIT_ASSERT(b.deep() == 0);
}

void TestStack::test2() {
  Allocator allocator[1];
  Stack<int> a(24, allocator);
  CPPUNIT_ASSERT(!a);
  CPPUNIT_ASSERT(a.deep() == 0);
  for(int i=1; i < 96; i++) {
    a.push(i);
    CPPUNIT_ASSERT(a);
    CPPUNIT_ASSERT(a.deep() == i);
    for(int j=0; j < i; j++)
      CPPUNIT_ASSERT(a[j] == j+1);
  }
  Stack<int> b(4, allocator);
  b = a;
  for(int i=95; i > 0; i--) {
    CPPUNIT_ASSERT(b);
    CPPUNIT_ASSERT(b.deep() == i);
    for(int j=0; j < i; j++)
      CPPUNIT_ASSERT(b[j] == j+1);
    b.pop();
  }
  CPPUNIT_ASSERT(!b);
  CPPUNIT_ASSERT(b.deep() == 0);
  a.swap(b);
  CPPUNIT_ASSERT(!a);
  CPPUNIT_ASSERT(a.deep() == 0);
  for(int i=95; i > 0; i--) {
    CPPUNIT_ASSERT(b);
    CPPUNIT_ASSERT(b.deep() == i);
    for(int j=0; j < i; j++)
      CPPUNIT_ASSERT(b[j] == j+1);
    b.pop();
  }
  CPPUNIT_ASSERT(!b);
  CPPUNIT_ASSERT(b.deep() == 0);
}
