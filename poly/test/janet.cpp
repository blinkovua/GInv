/***************************************************************************
 *   Copyright (C) 2018 by Blinkov Yu. A.                                  *
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

#include "poly/janet.h"

using namespace GInv;

class JanetPoly: public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(JanetPoly);
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

CPPUNIT_TEST_SUITE_REGISTRATION(JanetPoly);

void JanetPoly::setUp() {
}

void JanetPoly::tearDown() {
}


void JanetPoly::test1() {
  const int n(5);
  Allocator allocator[1];
  Monom v[]={
    {allocator, 0, n, -1},
    {allocator, 1, n, -1},
    {allocator, 2, n, -1},
    {allocator, 3, n, -1},
    {allocator, 4, n, -1},
  };
  List<Wrap*> Q(allocator);

  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[0])*v[1]*v[4]));
  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[1])*v[2]));
  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[0])*v[2]*v[2]*v[2]));

  for(List<Wrap*>::ConstIterator j(Q.begin()); j; ++j)
    std::cerr << j.data()->lm() << std::endl;
  std::cerr << Q.length() << std::endl;

  Janet janet(allocator);
  List<Wrap*> T(allocator);
  while(Q) {
    for(List<Wrap*>::Iterator j(Q.begin()); j; j.del()) {
      if (janet.find(j.data()->lm()))
        allocator->destroy(j.data());
      else {
        janet.insert(j.data());
        T.push(j.data());
      }
    }
    List<Wrap*> tmp(allocator);
    for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
      for(int k=0; k < j.data()->lm().size(); k++)
        if (j.data()->NM(k) && !j.data()->build(k)) {
          Wrap *w = new(allocator) Wrap(allocator, k, j.data());
//           std::cerr << j.data()->lm() << " -> " << w->lm() << std::endl;
          tmp.push(w);
        }
    tmp.swap(Q);
  }

#ifdef GINV_POLY_GRAPHVIZ
  janet.draw("pdf", "janet_test1.pdf");
#endif // GINV_POLY_GRAPHVIZ

  for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
    for(int v=0; v < n; v++) {
      Monom m(allocator, v, j.data()->lm());
      CPPUNIT_ASSERT(janet.find(m));
    }

//   std::cerr << "Janet = " << janet.find(Monom(allocator, v[0])*v[1]*v[2]*v[2]*v[2])->lm() << std::endl;
  CPPUNIT_ASSERT(janet.find(Monom(allocator, v[0])*v[1]*v[2]*v[2]*v[2]));
  CPPUNIT_ASSERT(T.length() == janet.size());

//   std::cerr << "Janet = " << T.length() << std::endl;
  for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
    allocator->destroy(j.data());
}

void JanetPoly::test2() {
  const int n(6);
  Allocator allocator[1];
  Monom v[]={
    {allocator, 0, n, -1},
    {allocator, 1, n, -1},
    {allocator, 2, n, -1},
    {allocator, 3, n, -1},
    {allocator, 4, n, -1},
    {allocator, 5, n, -1},
  };
  List<Wrap*> Q(allocator);

//   [1 0 0 0 0 1]
  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[0])*v[5]));
//   [0 1 0 0 2 1]
  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[1])*v[4]*v[4]*v[5]));
//   [0 1 4 0 0 2]
  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[1])*Monom(allocator, v[2], 4)*v[5]*v[5]));
//   [0 2 1 0 0 0]
  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[1], 2)*v[2]));
//   [1 0 0 1 0 0]
  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[0])*v[3]));
//   [0 0 2 1 3 1]
  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[2], 2)*v[3]*Monom(allocator, v[4], 3)*v[5]));

//   for(List<Wrap*>::ConstIterator j(Q.begin()); j; ++j)
//     std::cerr << j.data()->lm() << std::endl;
//   std::cerr << Q.length() << std::endl;

  Janet janet(allocator);
  List<Wrap*> T(allocator);
  while(Q) {
    for(List<Wrap*>::Iterator j(Q.begin()); j; j.del()) {
      if (janet.find(j.data()->lm()))
        allocator->destroy(j.data());
      else {
        janet.insert(j.data());
        T.push(j.data());
      }
    }
    List<Wrap*> tmp(allocator);
    for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
      for(int k=0; k < j.data()->lm().size(); k++)
        if (j.data()->NM(k) && !j.data()->build(k)) {
          Wrap *w = new(allocator) Wrap(allocator, k, j.data());
          tmp.push(w);
        }
    tmp.swap(Q);
  }
#ifdef GINV_POLY_GRAPHVIZ
  janet.draw("pdf", "janet_test2.pdf");
#endif // GINV_POLY_GRAPHVIZ

  for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j) {
    Allocator allocator[1];
    for(int v=0; v < n; v++) {
      Monom m(allocator, v, j.data()->lm());
      CPPUNIT_ASSERT(janet.find(j.data()->lm()));
    }
  }

  CPPUNIT_ASSERT(T.length() == janet.size());
//   std::cerr << "Janet = " << T.length() << std::endl;
  for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
    allocator->destroy(j.data());
}

void JanetPoly::test3() {
  Allocator allocator[1];
  const int n=30;
  const int s=5, d1=2, d2=7;
  Monom::rand_init(s, d1, d2);
  List<Wrap*> Q(allocator);
  for(int i=0; i < n; i++)
    Q.push(new(allocator) Wrap(allocator, Monom(allocator, Monom::next(allocator))));

  for(List<Wrap*>::ConstIterator j(Q.begin()); j; ++j)
    std::cerr << j.data()->lm() << std::endl;
  std::cerr << Q.length() << std::endl;

  Janet janet(allocator);
  List<Wrap*> T(allocator);
  while(Q) {
    for(List<Wrap*>::Iterator j(Q.begin()); j; j.del()) {
      if (janet.find(j.data()->lm()))
        allocator->destroy(j.data());
      else {
        janet.insert(j.data());
        T.push(j.data());
      }
    }
    List<Wrap*> tmp(allocator);
    for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
      for(int k=0; k < j.data()->lm().size(); k++)
        if (j.data()->NM(k) && !j.data()->build(k)) {
          Wrap *w = new(allocator) Wrap(allocator, k, j.data());
//           std::cerr << j.data()->lm() << " -> " << w->lm() << std::endl;
          tmp.push(w);
        }
    tmp.swap(Q);
  }
#ifdef GINV_POLY_GRAPHVIZ
  janet.draw("pdf", "janet_test3.pdf");
#endif // GINV_POLY_GRAPHVIZ


  for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j) {
    Allocator allocator[1];
    for(int v=0; v < s; v++) {
      Monom m(allocator, v, j.data()->lm());
      CPPUNIT_ASSERT(janet.find(j.data()->lm()));
    }
  }


  CPPUNIT_ASSERT(T.length() == janet.size());
  std::cerr << "Janet = " << T.length() << std::endl;
  for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
    allocator->destroy(j.data());
}


void JanetPoly::test4() {
  const int n(5);
  Allocator allocator[1];
  Monom v[]={
    {allocator, 0, n, -1},
    {allocator, 1, n, -1},
    {allocator, 2, n, -1},
    {allocator, 3, n, -1},
    {allocator, 4, n, -1},
  };
  List<Wrap*> Q(allocator);

  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[2])));
  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[0])*v[1]*v[4]));
  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[1])*v[2]));
  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[0])*v[2]*v[2]*v[2]));

  for(List<Wrap*>::ConstIterator j(Q.begin()); j; ++j)
    std::cerr << j.data()->lm() << std::endl;
  std::cerr << Q.length() << std::endl;

  Janet janet(allocator);
  List<Wrap*> T(allocator);
  while(Q) {
    for(List<Wrap*>::Iterator j(Q.begin()); j; j.del()) {
      if (janet.find(j.data()->lm()))
        allocator->destroy(j.data());
      else {
        janet.insert(j.data());
        T.push(j.data());
      }
    }
    List<Wrap*> tmp(allocator);
    for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
      for(int k=0; k < j.data()->lm().size(); k++)
        if (j.data()->NM(k) && !j.data()->build(k)) {
          Wrap *w = new(allocator) Wrap(allocator, k, j.data());
//           std::cerr << j.data()->lm() << " -> " << w->lm() << std::endl;
          tmp.push(w);
        }
    tmp.swap(Q);
  }

#ifdef GINV_POLY_GRAPHVIZ
  janet.draw("pdf", "janet_test4.pdf");
#endif // GINV_POLY_GRAPHVIZ

  for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
    for(int v=0; v < n; v++) {
      Monom m(allocator, v, j.data()->lm());
      CPPUNIT_ASSERT(janet.find(m));
    }

//   std::cerr << "Janet = " << janet.find(Monom(allocator, v[0])*v[1]*v[2]*v[2]*v[2])->lm() << std::endl;
  CPPUNIT_ASSERT(janet.find(Monom(allocator, v[0])*v[1]*v[2]*v[2]*v[2]));
  CPPUNIT_ASSERT(T.length() == janet.size());

//   std::cerr << "Janet = " << T.length() << std::endl;
  for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
    allocator->destroy(j.data());
}

void JanetPoly::test5() {
  const int n(5);
  Allocator allocator[1];
  Monom v[]={
    {allocator, 0, n, -1},
    {allocator, 1, n, -1},
    {allocator, 2, n, -1},
    {allocator, 3, n, -1},
    {allocator, 4, n, -1},
  };
  List<Wrap*> Q(allocator);

  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[0])*v[1]*v[4]));
  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[1])*v[2]));
  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[0])*v[2]*v[2]*v[2]));
  Q.push(new(allocator) Wrap(allocator, Monom(allocator, v[2])));

  for(List<Wrap*>::ConstIterator j(Q.begin()); j; ++j)
    std::cerr << j.data()->lm() << std::endl;
  std::cerr << Q.length() << std::endl;

  Janet janet(allocator);
  List<Wrap*> T(allocator);
  while(Q) {
    for(List<Wrap*>::Iterator j(Q.begin()); j; j.del()) {
      if (janet.find(j.data()->lm()))
        allocator->destroy(j.data());
      else {
        janet.insert(j.data());
        T.push(j.data());
      }
    }
    List<Wrap*> tmp(allocator);
    for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
      for(int k=0; k < j.data()->lm().size(); k++)
        if (j.data()->NM(k) && !j.data()->build(k)) {
          Wrap *w = new(allocator) Wrap(allocator, k, j.data());
//           std::cerr << j.data()->lm() << " -> " << w->lm() << std::endl;
          tmp.push(w);
        }
    tmp.swap(Q);
  }

#ifdef GINV_POLY_GRAPHVIZ
  janet.draw("pdf", "janet_test4.pdf");
#endif // GINV_POLY_GRAPHVIZ

  for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
    for(int v=0; v < n; v++) {
      Monom m(allocator, v, j.data()->lm());
      CPPUNIT_ASSERT(janet.find(m));
    }

//   std::cerr << "Janet = " << janet.find(Monom(allocator, v[0])*v[1]*v[2]*v[2]*v[2])->lm() << std::endl;
  CPPUNIT_ASSERT(janet.find(Monom(allocator, v[0])*v[1]*v[2]*v[2]*v[2]));
  CPPUNIT_ASSERT(T.length() == janet.size());

//   std::cerr << "Janet = " << T.length() << std::endl;
  for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
    allocator->destroy(j.data());
}
