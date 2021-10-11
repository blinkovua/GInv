/***************************************************************************
 *   Copyright (C) 2021 by Blinkov Yu. A.                                  *
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

#ifndef GINV_POLY_POLY_H
#define GINV_POLY_POLY_H

#include "util/allocator.h"
#include "util/list.h"
#include "util/integer.h"
#include "monom.h"
#include "janet.h"

#include "config.h"

namespace GInv {

class Poly {
public:
  enum Pos {TOP=0x10,
            POT=0x20
  };
  enum Order {lex=   0x0,
              deglex=0x1,
              alex=  0x2
  };

protected:
  struct Term {
    Monom    mMonom;
    Integer  mCoeff;

    Term()=delete;
    Term(const Term& a)=delete;
    Term(Allocator* allocator, const Term& a):
        mMonom(allocator, a.mMonom),
        mCoeff(allocator, a.mCoeff) {
    }
    Term(Allocator* allocator, int size):
        mMonom(allocator, size, -1),
        mCoeff(allocator) {
      mCoeff.set_ui(1ul);
    }
    Term(Allocator* allocator, int size, const char* hex):
        mMonom(allocator, size, -1),
        mCoeff(allocator, hex) {
    }
    Term(Allocator* allocator, Monom::Variable v, int size, int pos):
        mMonom(allocator, v, size, pos),
        mCoeff(allocator) {
      mCoeff.set_ui(1ul);
    }
    Term(Allocator* allocator, const Monom& a):
        mMonom(allocator, a),
        mCoeff(allocator) {
      mCoeff.set_ui(1ul);
    }
    Term(Allocator* allocator, const Monom& a, const Monom& b):
        mMonom(allocator, a, b),
        mCoeff(allocator) {
      mCoeff.set_ui(1ul);
    }
    ~Term() {}
  };

  static int TOPlex(const Monom& a, const Monom& b);
  static int TOPdeglex(const Monom& a, const Monom& b);
  static int TOPalex(const Monom& a, const Monom& b);

  static int TOPlex2(const Monom& a, const Monom& b, const Monom& c);
  static int TOPdeglex2(const Monom& a, const Monom& b, const Monom& c);
  static int TOPalex2(const Monom& a, const Monom& b, const Monom& c);

  static int POTlex(const Monom& a, const Monom& b);
  static int POTdeglex(const Monom& a, const Monom& b);
  static int POTalex(const Monom& a, const Monom& b);

  static int POTlex2(const Monom& a, const Monom& b, const Monom& c);
  static int POTdeglex2(const Monom& a, const Monom& b, const Monom& c);
  static int POTalex2(const Monom& a, const Monom& b, const Monom& c);

  Allocator*   mAllocator;
  int          mOrder;
  int          mSize;
  List<Term*>  mHead;
  int (*mCmp1)(const Monom&, const Monom&);
  int (*mCmp2)(const Monom&, const Monom&, const Monom&);

  void setOrder(int order);
  void clear();

  void redTail(List<Term*>::Iterator i1, const Poly& a);

public:
  class ConstIterator {
    friend class Poly;

    List<Term*>::ConstIterator mConstIt;

    ConstIterator(const List<Term*>::ConstIterator &i):
        mConstIt(i) {}

  public:
    ConstIterator(): mConstIt() {}
    ~ConstIterator() {}

    const Integer& coeff() const { return mConstIt.data()->mCoeff; }
    const Monom& monom() const { return mConstIt.data()->mMonom; }
    operator bool() const { return mConstIt; }
    bool operator!=(const ConstIterator& a) { return mConstIt != a.mConstIt; }
    bool operator==(const ConstIterator& a) { return mConstIt == a.mConstIt; }
    void operator++() {
      assert(mConstIt);
      ++mConstIt;
    }
  };

  bool compare(const Poly& a) const {
    return mSize == a.mSize && mOrder == a.mOrder;
  }

  Poly()=delete;
  Poly(const Poly& a)=delete;
  Poly(Poly&& a):
      mAllocator(a.mAllocator),
      mOrder(a.mOrder),
      mSize(a.mSize),
      mHead(a.mAllocator),
      mCmp1(a.mCmp1),
      mCmp2(a.mCmp2) {
    a.mAllocator = nullptr;
    assert(assertValid());
  }
  Poly(Allocator* allocator, int order, int size):
      mAllocator(allocator),
      mOrder(order),
      mSize(size),
      mHead(allocator) {
    assert(size > 0);
    setOrder(order);
    assert(assertValid());
  }
  Poly(Allocator* allocator, int order, int size, const char *integer):
      mAllocator(allocator),
      mOrder(order),
      mSize(size),
      mHead(allocator) {
    assert(size > 0);
    setOrder(order);
    mHead.push(new(allocator) Term(allocator, size, integer));
    assert(assertValid());
  }
  Poly(Allocator* allocator, int order, const Monom& a):
      mAllocator(allocator),
      mOrder(order),
      mSize(a.size()),
      mHead(allocator) {
    setOrder(order);
    mHead.push(new(allocator) Term(allocator, a));
    assert(assertValid());
  }
  Poly(Allocator* allocator, const Poly& a);
  ~Poly() {
    if (mAllocator)
      clear();
  }

  void swap(Poly& a);
  void operator=(Poly &&a);
  void operator=(const Poly &a);

  int order() const { return mOrder; }
  int size() const { return mSize; }

  ConstIterator begin() const { return mHead.begin(); }
  operator bool() const { return mHead; }
  bool isZero() const { return !mHead; }
  int length() const { return mHead.length(); }
  int degree() const {
    assert(mHead);
    return mHead.head()->mMonom.degree();
  }
  int norm() const;
  const Monom& lm() const {
    assert(mHead);
    return mHead.head()->mMonom;
  }
  const Integer& lc() const {
    assert(mHead);
    return mHead.head()->mCoeff;
  }

  void minus();
  void add(const char* hex);
  void add(const Poly &a);
  void sub(const char* hex);
  void sub(const Poly &a);
  void mult(const char* hex);
  void mult(const Poly &a);
  void pow(int deg);

  void reduction(const Poly& a);
  void nf(Janet &a);
  void nfTail(Janet &a);
  bool isPp() const;
  void pp();

  friend Poly operator-(Poly&& a) {
    Poly r(std::move(a));
    r.minus();
    return r;
  }
  friend Poly operator+(Poly&& a, const Poly& b) {
    Poly r(std::move(a));
    r.add(b);
    return r;
  }
  friend Poly operator-(Poly&& a, const Poly& b) {
    Poly r(std::move(a));
    r.sub(b);
    return r;
  }
  friend Poly operator*(Poly&& a, const Poly& b) {
    Poly r(std::move(a));
    r.mult(b);
    return r;
  }
  Poly pow(Poly&& a, int deg) {
    Poly r(std::move(a));
    r.pow(deg);
    return r;
  }


  friend std::ostream& operator<<(std::ostream& out, const Poly &a);

  bool assertValid() const;
};


}

#endif // GINV_POLY_POLY_H
