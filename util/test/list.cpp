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

#include "util/list.h"

using namespace GInv;

class TestList: public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(TestList);
  CPPUNIT_TEST(test1);
  CPPUNIT_TEST(test2);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void test1();
  void test2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestList);

void TestList::setUp() {
}

void TestList::tearDown() {
}

void TestList::test1() {
  Allocator allocator[1];
  List<int> a(allocator);
  CPPUNIT_ASSERT(!a);
  List<int>::Iterator i(a.begin());
  a.begin().insert(1);
  CPPUNIT_ASSERT(a.begin().data() == 1);
  a.begin().insert(-1);
  CPPUNIT_ASSERT(a.begin().data() == -1);
  CPPUNIT_ASSERT(a.length() == 2);
  a.begin().del();
  CPPUNIT_ASSERT(a.begin().data() == 1);
}

void TestList::test2() {
  typedef GC<List<int> > GCListInt;
  GCListInt a;
  CPPUNIT_ASSERT(!a);
  GCListInt::Iterator i(a.begin());

  for(int k=0; k < 19; k++)
    a.begin().insert(k+1);
  CPPUNIT_ASSERT(a.length() == 19);
  for(int k=19; k > 0; k--)
    CPPUNIT_ASSERT(a.pop() == k);
  CPPUNIT_ASSERT(a.length() == 0);

  for(int k=0; k < 19; k++)
    a.push(k+1);
  CPPUNIT_ASSERT(a.length() == 19);
  for(int k=19; k > 0; k--)
    CPPUNIT_ASSERT(a.pop() == k);
  CPPUNIT_ASSERT(a.length() == 0);
}
