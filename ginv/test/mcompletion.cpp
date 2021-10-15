/***************************************************************************
 *   Copyright (C) 2019 by Blinkov Yu. A.                                  *
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

#include "ginv/mcompletion.h"

using namespace GInv;

class MCompletionPoly: public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(MCompletionPoly);
//   CPPUNIT_TEST(test1);
//   CPPUNIT_TEST(test6);
//   CPPUNIT_TEST(test7);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void test1();
  void test6();
  void test7();
};

CPPUNIT_TEST_SUITE_REGISTRATION(MCompletionPoly);

void MCompletionPoly::setUp() {
}

void MCompletionPoly::tearDown() {
}

void MCompletionPoly::test1() {
  const int n(5);
  Allocator allocator[1];
  Monom v[]={
    {allocator, 0, n, -1},
    {allocator, 1, n, -1},
    {allocator, 2, n, -1},
    {allocator, 3, n, -1},
    {allocator, 4, n, -1},
  };

// 11001 01100 10300
  Monom m[]={
    {allocator, Monom(allocator, v[0])*v[1]*v[4]},
    {allocator, Monom(allocator, v[1])*v[2]},
    {allocator, Monom(allocator, v[0])*v[2]*v[2]*v[2]},
  };

  MCompletion basis(allocator, MCompletion::GB);
  for(int i=0; i < sizeof(m)/sizeof(Monom); i++)
    basis.push(m[i]);
//   for(const Monom &i; m)
//     basis.push(i);
  basis.build();
  for(ListWrap::ConstIterator j(basis.T().begin()); j; ++j)
    std::cout << *j.data() << std::endl;

  std::cout << "size = " << basis.T().length() << std::endl;
}

void MCompletionPoly::test6() {
  const int n(3);
  Allocator allocator[1];
  Monom v[]={
    {allocator, 1, n, -1},
    {allocator, 0, n, -1},
//     {allocator, 1, n, -1},
    {allocator, 2, n, -1},
  };

//{201,120,400,103,040}
  Monom m[]={
    {allocator, Monom(allocator, v[0], 2)*v[2]},
    {allocator, Monom(allocator, v[1], 2)*v[0]},
    {allocator, Monom(allocator, v[0], 4)},
    {allocator, Monom(allocator, v[2], 3)*v[0]},
    {allocator, Monom(allocator, v[1], 4)}
  };

  MCompletion basis(allocator, MCompletion::GB);
  for(int i=0; i < sizeof(m)/sizeof(Monom); i++)
    basis.push(m[i]);
//   for(const Monom &i; m)
//     basis.push(i);
  basis.build();
  for(ListWrap::ConstIterator j(basis.T().begin()); j; ++j)
    std::cout << *j.data() << std::endl;

  std::cout << "size = " << basis.T().length() << std::endl;
}

void MCompletionPoly::test7() {
  const int n(5);
  Allocator allocator[1];
  Monom v[]={
    {allocator, 0, n, -1},
    {allocator, 1, n, -1},
    {allocator, 2, n, -1},
    {allocator, 3, n, -1},
    {allocator, 4, n, -1},
  };

// {22001, 02101, 01010,00200, 00112}
  Monom m[]={
    {allocator, Monom(allocator, v[0], 2)*Monom(allocator, v[1], 2)*v[4]},
    {allocator, Monom(allocator, v[1], 2)*v[2]*v[4]},
    {allocator, Monom(allocator, v[1])*v[3]},
    {allocator, Monom(allocator, v[2], 2)},
    {allocator, Monom(allocator, v[4], 2)*v[2]*v[3]}
  };

  MCompletion basis(allocator, MCompletion::GB);
  for(int i=0; i < sizeof(m)/sizeof(Monom); i++)
    basis.push(m[i]);
  basis.build();
  for(ListWrap::ConstIterator j(basis.T().begin()); j; ++j)
    std::cout << *j.data() << std::endl;

  std::cout << "size = " << basis.T().length() << std::endl;
}

