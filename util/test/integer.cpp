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

#include "util/integer.h"

using namespace GInv;

class TestInteger: public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(TestInteger);
  CPPUNIT_TEST(test1);
//   CPPUNIT_TEST(test2);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void test1();
  void test2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestInteger);

void TestInteger::setUp() {
}

void TestInteger::tearDown() {
}


void TestInteger::test1() {
  Allocator allocator[1];

  Integer a(allocator);
  std::cout << a.get_str(16) << std::endl;
  a.set_si(-12652);
  std::cout << a.get_str(16) << std::endl;

  Integer b(allocator, "-01265216253615367125312376141540340646456346");
//   CPPUNIT_ASSERT(b.set_str("-12652162536153671253192376141540340646456346", 10) == 0);
  std::cout << b.get_str(10) << std::endl;
  std::cout << b.get_str(16) << std::endl;

  Integer c(allocator);
  c.add(b, b);
  std::cout << c.get_str(10) << std::endl;
}

void TestInteger::test2() {
}

