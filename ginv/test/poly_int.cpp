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

#include "ginv/poly_int.h"

using namespace GInv;

class TestPolyInt: public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(TestPolyInt);
  CPPUNIT_TEST(test1);
  CPPUNIT_TEST(test2);
  CPPUNIT_TEST(test3);
  CPPUNIT_TEST(test4);
  CPPUNIT_TEST(test5);
//   CPPUNIT_TEST(test6);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void test1();
  void test2();
  void test3();
  void test4();
  void test5();
  void test6();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestPolyInt);

void TestPolyInt::setUp() {
<<<<<<< HEAD
  Allocator::setLimitMemory(size_t(26000)*1024*1024);
=======
  Allocator::setLimitMemory(size_t(2000)*1024*1024);
>>>>>>> 9f97713d90249ae23691184059766526013b535a
}

void TestPolyInt::tearDown() {
}


void TestPolyInt::test1() {
  Allocator allocator[1];
  const int n=7;
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
  PolyInt p[]={
    {allocator, PolyInt::TOP | PolyInt::lex, v[0]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[1]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[2]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[3]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[4]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[5]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[6]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[7]},
  };

  CPPUNIT_ASSERT(p[0].length() == 1);
  p[5].add(p[0]);
  p[5].add("-0x12368");
  std::cerr << p[5] << std::endl;
  WrapPolyInt w1(p[5]);
  w1.setNM(2);
  std::cerr << w1 << std::endl;
  WrapPolyInt w2(2, &w1);
  std::cerr << w2 << std::endl;

  std::cerr << (p[2] + p[5]) << std::endl;
}

void TestPolyInt::test2() {
  Allocator allocator[1];
  const int n=5;
  Monom v[]={
    {allocator, 0, n, -1},
    {allocator, 1, n, -1},
    {allocator, 2, n, -1},
    {allocator, 3, n, -1},
    {allocator, 4, n, -1},
  };
  PolyInt p[]={
    {allocator, PolyInt::TOP | PolyInt::lex, v[0]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[1]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[2]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[3]},
    {allocator, PolyInt::TOP | PolyInt::lex, v[4]},
  };

  JanetPolyInt basis;

//   basis.push(PolyInt(allocator, 2, p[0]));
  basis.push(p[0]);
  p[0].add("0x163256172");
  basis.push(p[0]);
  basis.push(p[1]);
  basis.push(p[2]);
  basis.build();
  std::cerr << "size - " << basis.size() << std::endl;
  for(GCListWrapPolyInt::ConstIterator k(basis.begin()); k; ++k)
    std::cerr << *k.data() << std::endl;
}

void TestPolyInt::test3() {
  Allocator allocator[1];
  const int n=5;
  Monom v[]={
    {allocator, 0, n, -1},
    {allocator, 1, n, -1},
    {allocator, 2, n, -1},
    {allocator, 3, n, -1},
    {allocator, 4, n, -1},
  };
  PolyInt p[]={
    {allocator, PolyInt::TOP | PolyInt::deglex, v[0]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[1]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[2]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[3]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[4]},
  };
  PolyInt q[]={
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
  };

  for(int i=0; i < n; i++)
    q[0].add(p[i]);
  std::cerr << q[0] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    q[1].add(tmp);
  }
  std::cerr << q[1] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    tmp.mult(p[(i+2) % n]);
    q[2].add(tmp);
  }
  std::cerr << q[2] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    tmp.mult(p[(i+2) % n]);
    tmp.mult(p[(i+3) % n]);
    q[3].add(tmp);
  }
  std::cerr << q[3] << std::endl;
  q[4].add(p[0]);
  for(int i=1; i < n; i++)
    q[4].mult(p[i]);
  q[4].sub("0x1");
  std::cerr << q[4] << std::endl;


  JanetPolyInt basis;
  for(int i=0; i < n; i++)
    basis.push(q[i]);

  basis.build();
  std::cerr << "size - " << basis.size() << std::endl;
  basis.janet(-1).draw("pdf", "cyclic5.pdf");

//   std::cerr << std::endl;
//   for(GCListWrapPolyInt::ConstIterator j(basis.begin()); j; ++j) {
//     std::cerr << j.data()->multi() - j.data()->lm().degree() << ", ";
//   }
//   std::cerr << std::endl;
//   for(GCListWrapPolyInt::ConstIterator j(basis.begin()); j; ++j) {
//     std::cerr << j.data()->multi() << ", ";
//   }
//   std::cerr << std::endl;
//   JanetPolyInt basis1;
//   for(GCListWrapPolyInt::ConstIterator k(basis.begin()); k; ++k)
//     basis1.push(k.data()->poly());
//   basis1.build();
//   std::cerr << "size - " << basis1.size() << std::endl;
//   basis1.janet(-1).draw("pdf", "cyclic5a.pdf");
}

void TestPolyInt::test4() {
  Allocator allocator[1];
  const int n=6;
  Monom v[]={
    {allocator, 0, n, -1},
    {allocator, 1, n, -1},
    {allocator, 2, n, -1},
    {allocator, 3, n, -1},
    {allocator, 4, n, -1},
    {allocator, 5, n, -1},
  };
  PolyInt p[]={
    {allocator, PolyInt::TOP | PolyInt::deglex, v[0]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[1]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[2]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[3]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[4]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[5]},
  };
  PolyInt q[]={
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
  };

  for(int i=0; i < n; i++)
    q[0].add(p[i]);
  std::cerr << q[0] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    q[1].add(tmp);
  }
  std::cerr << q[1] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    tmp.mult(p[(i+2) % n]);
    q[2].add(tmp);
  }
  std::cerr << q[2] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    tmp.mult(p[(i+2) % n]);
    tmp.mult(p[(i+3) % n]);
    q[3].add(tmp);
  }
  std::cerr << q[3] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    tmp.mult(p[(i+2) % n]);
    tmp.mult(p[(i+3) % n]);
    tmp.mult(p[(i+4) % n]);
    q[4].add(tmp);
  }
  std::cerr << q[4] << std::endl;
  q[5].add(p[0]);
  for(int i=1; i < n; i++)
    q[5].mult(p[i]);
  q[5].sub("0x1");
  std::cerr << q[5] << std::endl;


  JanetPolyInt basis;
  for(int i=0; i < n; i++)
    basis.push(q[i]);

  basis.build();
  std::cerr << "size - " << basis.size() << std::endl;
  basis.janet(-1).draw("pdf", "cyclic6.pdf");

  std::cerr << std::endl;
  for(GCListWrapPolyInt::ConstIterator j(basis.begin()); j; ++j) {
    std::cerr << j.data()->multi() - j.data()->lm().degree() << ", ";
  }
  std::cerr << std::endl;
  for(GCListWrapPolyInt::ConstIterator j(basis.begin()); j; ++j) {
    std::cerr << j.data()->multi() << ", ";
  }
  std::cerr << std::endl;
//   JanetPolyInt basis1;
//   for(GCListWrapPolyInt::ConstIterator k(basis.begin()); k; ++k)
//     basis1.push(k.data()->poly());
//   basis1.build();
//   std::cerr << "size - " << basis1.size() << std::endl;
//   basis1.janet(-1).draw("pdf", "cyclic6a.pdf");
}

void TestPolyInt::test5() {
  Allocator allocator[1];
  const int n=7;

  Monom v[]={
    {allocator, 0, n, -1},
    {allocator, 1, n, -1},
    {allocator, 2, n, -1},
    {allocator, 3, n, -1},
    {allocator, 4, n, -1},
    {allocator, 5, n, -1},
    {allocator, 6, n, -1},
  };

  PolyInt p[]={
    {allocator, PolyInt::TOP | PolyInt::deglex, v[0]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[1]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[2]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[3]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[4]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[5]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[6]},
  };
  PolyInt q[]={
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
  };

  for(int i=0; i < n; i++)
    q[0].add(p[i]);
  std::cerr << q[0] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    q[1].add(tmp);
  }
  std::cerr << q[1] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    tmp.mult(p[(i+2) % n]);
    q[2].add(tmp);
  }
  std::cerr << q[2] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    tmp.mult(p[(i+2) % n]);
    tmp.mult(p[(i+3) % n]);
    q[3].add(tmp);
  }
  std::cerr << q[3] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    tmp.mult(p[(i+2) % n]);
    tmp.mult(p[(i+3) % n]);
    tmp.mult(p[(i+4) % n]);
    q[4].add(tmp);
  }
  std::cerr << q[4] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    tmp.mult(p[(i+2) % n]);
    tmp.mult(p[(i+3) % n]);
    tmp.mult(p[(i+4) % n]);
    tmp.mult(p[(i+5) % n]);
    q[5].add(tmp);
  }
  std::cerr << q[5] << std::endl;
  q[6].add(p[0]);
  for(int i=1; i < n; i++)
    q[6].mult(p[i]);
  q[6].sub("0x1");
  std::cerr << q[6] << std::endl;


  JanetPolyInt basis;
  for(int i=0; i < n; i++)
    basis.push(q[i]);

  basis.build();
  std::cerr << "size - " << basis.size() << std::endl;
  basis.janet(-1).draw("pdf", "cyclic7.pdf");

//   JanetPolyInt basis1;
//   for(GCListWrapPolyInt::ConstIterator k(basis.begin()); k; ++k)
//     basis1.push(k.data()->poly());
//   basis1.build();
//   std::cerr << "size - " << basis1.size() << std::endl;
//   basis1.janet(-1).draw("pdf", "cyclic7a.pdf");
}

void TestPolyInt::test6() {
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

  PolyInt p[]={
    {allocator, PolyInt::TOP | PolyInt::deglex, v[0]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[1]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[2]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[3]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[4]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[5]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[6]},
    {allocator, PolyInt::TOP | PolyInt::deglex, v[7]},
  };
  PolyInt q[]={
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
    {allocator, PolyInt::TOP | PolyInt::deglex, n},
  };

  for(int i=0; i < n; i++)
    q[0].add(p[i]);
  std::cerr << q[0] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    q[1].add(tmp);
  }
  std::cerr << q[1] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    tmp.mult(p[(i+2) % n]);
    q[2].add(tmp);
  }
  std::cerr << q[2] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    tmp.mult(p[(i+2) % n]);
    tmp.mult(p[(i+3) % n]);
    q[3].add(tmp);
  }
  std::cerr << q[3] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    tmp.mult(p[(i+2) % n]);
    tmp.mult(p[(i+3) % n]);
    tmp.mult(p[(i+4) % n]);
    q[4].add(tmp);
  }
  std::cerr << q[4] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    tmp.mult(p[(i+2) % n]);
    tmp.mult(p[(i+3) % n]);
    tmp.mult(p[(i+4) % n]);
    tmp.mult(p[(i+5) % n]);
    q[5].add(tmp);
  }
  std::cerr << q[5] << std::endl;
  for(int i=0; i < n; i++) {
    PolyInt tmp(allocator, p[i]);
    tmp.mult(p[(i+1) % n]);
    tmp.mult(p[(i+2) % n]);
    tmp.mult(p[(i+3) % n]);
    tmp.mult(p[(i+4) % n]);
    tmp.mult(p[(i+5) % n]);
    tmp.mult(p[(i+6) % n]);
    q[6].add(tmp);
  }
  std::cerr << q[6] << std::endl;
  q[7].add(p[0]);
  for(int i=1; i < n; i++)
    q[7].mult(p[i]);
  q[7].sub("0x1");
  std::cerr << q[7] << std::endl;


  JanetPolyInt basis;
  for(int i=0; i < n; i++)
    basis.push(q[i]);

  basis.build();
  std::cerr << "size - " << basis.size() << std::endl;
  basis.janet(-1).draw("pdf", "cyclic.pdf");

//   JanetPolyInt basis1;
//   for(GCListWrapPolyInt::ConstIterator k(basis.begin()); k; ++k)
//     basis1.push(k.data()->poly());
//   basis1.build();
//   std::cerr << "size - " << basis1.size() << std::endl;
//   basis1.janet(-1).draw("pdf", "cyclic7a.pdf");
}
