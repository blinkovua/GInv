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

#include "util/timer.h"
#include "util/list.h"
#include "util/randpermutation.h"

#include "ginv/janet.h"
#include "ginv/mcompletion.h"

using namespace GInv;

class JanetPoly: public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(JanetPoly);
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

CPPUNIT_TEST_SUITE_REGISTRATION(JanetPoly);

void JanetPoly::setUp() {
}

void JanetPoly::tearDown() {
}


void JanetPoly::test1() {
  const int l=99,  // число экспериментов
            n=15,  // число мономов
            s=10,  // число переменных
            d1=3,  // минимальная степень переменной
            d2=10; // максимальная степень переменной

  Monom::rand_init(s, d1, d2);

  for(int i=0; i < l; i++) {
//     std::cerr << i << ' ';
    Allocator allocator[1];
    List<Wrap*> Q(allocator);
    Janet janet(allocator);
    Monom m(allocator, s, -1);
    for(int k=0; k < n; k++) {
      Monom::rand_next(m);
      Wrap *w=new(allocator) Wrap(m);
//       std::cerr << w->lm() << std::endl;
      if (janet.find(w->lm()))
        allocator->destroy(w);
      else {
//         std::cerr << w->lm() << std::endl;
        janet.insert(w);
        Q.push(w);
      }
    }
//     std::cerr << "AAA" << std::endl;

    for(List<Wrap*>::ConstIterator j(Q.begin()); j; ++j)
      CPPUNIT_ASSERT(janet.find(j.data()->lm()) == j.data());

    MCompletion basis(allocator, MCompletion::lex);
    for(List<Wrap*>::ConstIterator j(Q.begin()); j; ++j)
      basis.push(Monom(allocator, j.data()->lm()));

    CPPUNIT_ASSERT(janet.size() == basis.length());

    for(List<Wrap*>::ConstIterator j(basis.T().begin()); j; ++j) {
      const Wrap* wrap=janet.find(j.data()->lm());
      CPPUNIT_ASSERT(wrap->lm().lex(j.data()->lm()) == 0);
      for(int v=0; v < s; v++)
        CPPUNIT_ASSERT(wrap->NM(v) == j.data()->NM(v));
    }

#ifdef GINV_GRAPHVIZ
    if (i+1 == l)
      janet.draw("pdf", "janet_test1.pdf");
#endif // GINV_GRAPHVIZ

    for(List<Wrap*>::ConstIterator j(Q.begin()); j; ++j)
        allocator->destroy(j.data());
  }
//   std::cerr << std::endl;
}

void JanetPoly::test2() {
  const int l=99,  // число экспериментов
            s=3,    // число переменных
            n=5,   // число мономов
            d1=3,  // минимальная степень переменной
            d2=5; // максимальная степень переменной

  Monom::rand_init(s, d1, d2);

  for(int i=0; i < l; i++) {
//     std::cerr << i << ' ';
    Allocator allocator[1];
    List<Wrap*> Q(allocator);
    MCompletion basis(allocator, MCompletion::lex);
    Monom m(allocator, s, -1);
    for(int k=0; k < n; k++) {
      Monom::rand_next(m);
      Wrap *w=new(allocator) Wrap(m);
//       std::cerr << w->lm() << std::endl;
      if (basis.find(w->lm()))
        allocator->destroy(w);
      else {
//         std::cerr << w->lm() << std::endl;
        ListWrap::Iterator j(Q.begin());
        while(j && j.data()->lm().lex(w->lm()) < 0)
          ++j;
        j.insert(w);
        basis.push(w->lm());
      }
    }

    basis.build();

    Janet janet(allocator);
    List<Wrap*> T(allocator);
    bool first=true;
    bool draw=false;
    while(Q) {
      for(List<Wrap*>::Iterator j(Q.begin()); j; j.del()) {
        if (janet.find(j.data()->lm()))
          allocator->destroy(j.data());
        else {
          janet.insert(j.data());
          ListWrap::Iterator k(T.begin());
          while(k && k.data()->lm().lex(j.data()->lm()) < 0)
            ++k;
          k.insert(j.data());
        }
      }
//       if (!first)
//         break;
#ifdef GINV_GRAPHVIZ
      if (first && i+1 == l) {
        janet.draw("pdf", "janet_test21b.pdf");
        first = false;
      }
#endif // GINV_GRAPHVIZ
      List<Wrap*> tmp(allocator);
      for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
        for(int k=0; k < j.data()->lm().size(); k++)
          if (j.data()->NM(k) && !j.data()->build(k)) {
            Wrap *w = new(allocator) Wrap(k, j.data());
            ListWrap::Iterator j(tmp.begin());
            while(j && j.data()->lm().lex(w->lm()) < 0)
              ++j;
            j.insert(w);
          }
      tmp.swap(Q);
      first = false;
    }

//     std::cerr << janet.size() << " " << basis.length() << std::endl;

//     for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j) {
//       const Wrap* wrap=basis.find(j.data()->lm());
//       CPPUNIT_ASSERT(wrap->lm().lex(j.data()->lm()) == 0);
//       if (wrap) {
//       for(int v=0; v < s; v++)
//         if (wrap->NM(v) != j.data()->NM(v)) {
//           std::cerr << *wrap << "-" << *j.data() << std::endl;
//           draw = true;
//           break;
//         }
//       }
//       else
//         std::cerr << "*" << *j.data() << std::endl;
//     }
//     if (draw) {
//       janet.draw("pdf", "janet_test2e.pdf");
//       for(List<Wrap*>::ConstIterator j(basis.T().begin()); j; ++j)
//         std::cerr << "-" << *j.data() << std::endl;
//       assert(false);
//     }

    for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j) {
      Allocator allocator1[1];
      for(int v=0; v < s; v++) {
        Monom m(allocator1, v, j.data()->lm());
        if (janet.find(m) == nullptr) {
          janet.draw("pdf", "janet_test21e.pdf");
          std::cerr << j.data()->lm() << m << std::endl;
          assert(false);
        }
      }
    }

#ifdef GINV_GRAPHVIZ
    if (i+1 == l)
      janet.draw("pdf", "janet_test21e.pdf");
#endif // GINV_GRAPHVIZ

    CPPUNIT_ASSERT(T.length() == janet.size());
//   std::cerr << "Janet = " << T.length() << std::endl;
    for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
      allocator->destroy(j.data());
  }
//   std::cerr << std::endl;
}


inline int lex(const Wrap& a, const Wrap& b) {
  return a.lm().lex(b.lm());
}

void JanetPoly::test3() {
  const int l=10,  // число экспериментов
            p=10,   // число перестановок
            n=5,   // число мономов
            s=3,    // число переменных
            d1=3,   // минимальная степень переменной
            d2=10;  // максимальная степень переменной

  RandPermutation_mt19937 per(s);
  Monom::rand_init(s, d1, d2);

  int sumJanet=0;
  int sumLex=0;
  Timer timerJanet, timerLex;;
   for(int i=0; i < l; i++) {
    Allocator allocator[1];
    List<Wrap*> Qbase(allocator);
    Monom m(allocator, s, -1);
    for(int k=0; k < n; k++) {
      Monom::rand_next(m);
      Wrap *w=new(allocator) Wrap(m);
      Qbase.push(w);
    }

    for(int p1=0; p1 < p; p1++) {
      timerJanet.cont();
      {
        Allocator allocator[1];
        List<Wrap*> Q(allocator);
        Monom m(allocator, s, -1);
        for(List<Wrap*>::ConstIterator j(Qbase.begin()); j; ++j) {
          m.permutation(j.data()->lm(), per);
          Wrap *w=new(allocator) Wrap(m);
          Q.push(w);
        }

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
                Wrap *w = new(allocator) Wrap(k, j.data());
                tmp.push(w);
              }
          tmp.swap(Q);
        }
        for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j) {
          for(int v=0; v < s; v++) {
            Monom m(allocator, v, j.data()->lm());
            CPPUNIT_ASSERT(janet.find(m));
          }
        }


        assert(T.length() == janet.size());
//         std::cout << "Janet = " << T.length() << "  " << std::endl;
//         for(List<Wrap*>::ConstIterator k(T.begin()); k; ++k)
//           std::cerr << *k.data() << std::endl;
        sumJanet += T.length();

        for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
          allocator->destroy(j.data());
      }
      timerJanet.stop();

      timerLex.cont();
      {
        Allocator allocator[1];
        MCompletion basis(allocator, MCompletion::lex);
        Monom m(allocator, s, -1);
        for(List<Wrap*>::ConstIterator j(Qbase.begin()); j; ++j) {
          m.permutation(j.data()->lm(), per);
          basis.push(m);
        }

        basis.build();

//         std::cerr << "lex = " << basis.T().length() << std::endl;
        sumLex += basis.length();
      }
      timerLex.stop();

      per.next();
    }


    for(List<Wrap*>::ConstIterator j(Qbase.begin()); j; ++j)
      allocator->destroy(j.data());
  }
//   std::cerr << "Janet - " << sumJanet << std::endl;
//   std::cerr << timerJanet << std::endl;
//   std::cerr << "Lex - " << sumLex << std::endl;
//   std::cerr << timerLex << std::endl;

}
