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

#include "util/rbtree.h"

using namespace GInv;

class TestRBTree: public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(TestRBTree);
  CPPUNIT_TEST(test1);
  CPPUNIT_TEST(test2);
  CPPUNIT_TEST(test3);
  CPPUNIT_TEST(test4);
  CPPUNIT_TEST(test5);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void test1();
  void test2();
  void test3();
  void test4();
  void test5();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestRBTree);

void TestRBTree::setUp() {
}

void TestRBTree::tearDown() {
}

inline int compare(int a, int b) {
  if (a > b)
    return 1;
  else if (a < b)
    return -1;
  else
    return 0;
}

typedef RBTree<int, compare> RB;
void TestRBTree::test1() {
  Allocator allocator[1];
  RB a(allocator);
  CPPUNIT_ASSERT(!a);
  int data[]={1, 6, 8, 11, 13, 15, 17, 22, 25, 27};
  RB::Color color[]={RB::Black, RB::Black, RB::Black, RB::Black, RB::Black, RB::Black, RB::Black, RB::Red, RB::Black, RB::Red};

  for(int i=0; i < 10; i++)
    a.insert(data[i]);
  for(int i=0; i < 10; i++)
    CPPUNIT_ASSERT(a.find(data[i]).color() == color[i]);

  for(int i=1; i < 10; i+=2)
    a.remove(data[i]);
  for(int i=0; i < 10; i+=2)
    CPPUNIT_ASSERT(a.find(data[i]).color() == RB::Black
                   || data[i] == 8);
}

void TestRBTree::test2() {
  Allocator allocator[1];
  RB a(allocator);
  CPPUNIT_ASSERT(!a);
  int data[]={13, 8, 17, 1, 11, 15, 25, 6, 22, 27};
  RBTree<int, compare>::Color color[]={RB::Black, RB::Red, RB::Red, RB::Black, RB::Black, RB::Black, RB::Black, RB::Red, RB::Red, RB::Red};

  for(int i=0; i < 10; i++)
    a.insert(data[i]);
  for(int i=0; i < 10; i++)
    CPPUNIT_ASSERT(a.find(data[i]).color() == color[i]);

  for(int i=1; i < 10; i+=2)
    a.remove(data[i]);
  for(int i=0; i < 10; i+=2)
    CPPUNIT_ASSERT(a.find(data[i]).color() == RB::Black
                   || data[i] == 22);
}

void TestRBTree::test3() {
  Allocator allocator[1];
  RB a(allocator);
  CPPUNIT_ASSERT(!a);
  int data[]={27, 25, 22, 17, 15, 13, 11, 8, 6, 1};
  RB::Color color[]={RB::Black, RB::Black, RB::Black, RB::Black, RB::Black, RB::Black, RB::Black, RB::Red, RB::Black, RB::Red};

  for(int i=0; i < 10; i++)
    a.insert(data[i]);

  for(int i=0; i < 10; i++)
    CPPUNIT_ASSERT(a.find(data[i]).color() == color[i]);

  for(int i=1; i < 10; i+=2)
    a.remove(data[i]);
  for(int i=0; i < 10; i+=2)
    CPPUNIT_ASSERT(a.find(data[i]).color() == RB::Black
                   || data[i] == 11 || data[i] == 27);
}

void TestRBTree::test4() {
  Allocator allocator[1];
  RB a(allocator);
  CPPUNIT_ASSERT(!a);
  int data[]={1, 6, 8, 11, 13, 15, 17, 22, 25, 27};
  RB::Color color[]={RB::Black, RB::Black, RB::Black, RB::Black, RB::Black, RB::Black, RB::Black, RB::Red, RB::Black, RB::Red};

  for(int i=0; i < 10; i++)
    a.insert(data[i]);
  for(int i=1; i < 10; i++)
    CPPUNIT_ASSERT(a.findPrev(data[i]).key() == data[i-1]);
  for(int i=0; i < 10; i++)
    CPPUNIT_ASSERT(a.findPrev(data[i] + 1).key() == data[i]);
  CPPUNIT_ASSERT(!a.findPrev(data[0]));
}

void TestRBTree::test5() {
  Allocator allocator[1];
  RB a(allocator);
  CPPUNIT_ASSERT(!a);
  int data[]={1, 6, 8, 11, 13, 15, 17, 22, 25, 27};
  RB::Color color[]={RB::Black, RB::Black, RB::Black, RB::Black, RB::Black, RB::Black, RB::Black, RB::Red, RB::Black, RB::Red};

  for(int i=0; i < 10; i++)
    a.insert(data[i]);
  for(int i=0; i < 9; i++)
    CPPUNIT_ASSERT(a.findNext(data[i]).key() == data[i+1]);
  for(int i=0; i < 10; i++)
    CPPUNIT_ASSERT(a.findNext(data[i] - 1).key() == data[i]);
  CPPUNIT_ASSERT(!a.findNext(data[9]));
}
