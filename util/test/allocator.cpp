/***************************************************************************
 *   Copyright (C) 2009 by Blinkov Yu. A.                                  *
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
#include <ctime>

#include "util/allocator.h"

using namespace GInv;

class TestAllocator: public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(TestAllocator);
  CPPUNIT_TEST(test1);
  CPPUNIT_TEST(test2);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void test1();
  void test2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestAllocator);

void TestAllocator::setUp() {
  //srandom(time(NULL));
}

void TestAllocator::tearDown() {
}

inline int r() {
  int r = rand() / 10000;
  return (r > 0) ? r: rand() % 10000;
}

class Array {
  Allocator* mAllocator;
  size_t     mDim;
  double*    mArray;

public:
  Array(Allocator* allocator):
      mAllocator(allocator),
      mDim(r()),
      mArray(NULL) {
    mArray = new(mAllocator) double[mDim];
  }
  ~Array() {
    mAllocator->dealloc(mArray, mDim);
  }
  size_t size() const { return sizeof(double)*mDim; }
};


void TestAllocator::test1() {
  {
  Allocator allocator[1];
  Array a(allocator);
  Array b(allocator);
  Array c(allocator);
  Array d(allocator);
  }
  CPPUNIT_ASSERT(Allocator::currMemory() == 0);
}

void TestAllocator::test2() {
  {
  Allocator allocator[1];
  const int n=1000;
  Array *a[n];
  for(int i=0; i < n; i++)
    a[i] = new(allocator) Array(allocator);
  for(int i=0; i < n; i++)
    allocator->destroy(a[i]);
  CPPUNIT_ASSERT(allocator->size() == 0);
  }
  CPPUNIT_ASSERT(Allocator::currMemory() == 0);
}

