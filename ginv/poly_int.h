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

#ifndef GINV_POLY_INT_H
#define GINV_POLY_INT_H

#include "util/allocator.h"
#include "util/list.h"
#include "util/integer.h"
#include "monom.h"
#include "poly.h"
#include "wrap.h"
#include "janet.h"

#include "config.h"

namespace GInv {

class PolyInt: public Poly {
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
    Term(Allocator* allocator, Monom::Variable v, const Term& a):
        mMonom(allocator, v, a.mMonom),
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


  Allocator*   mAllocator;
  List<Term*>  mHead;

  void clear();

  void redTail(List<Term*>::Iterator i1, const PolyInt& a);

public:
  class ConstIterator {
    friend class PolyInt;

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

  PolyInt()=delete;
  PolyInt(const PolyInt& a)=delete;
  PolyInt(PolyInt&& a):
      Poly(a),
      mAllocator(a.mAllocator),
      mHead(std::move(a.mHead)) {
    a.mAllocator = nullptr;
    assert(assertValid());
  }
  PolyInt(Allocator* allocator, const PolyInt& a);
//   :
//       Poly(a.mOrder, a.mSize),
//       mAllocator(allocator),
//       mHead(allocator, a.mHead) {
//     assert(assertValid());
//   }
  PolyInt(Allocator* allocator, int order, int size):
      Poly(order, size),
      mAllocator(allocator),
      mHead(allocator) {
  }
  PolyInt(Allocator* allocator, int order, int size, const char *integer):
      Poly(order, size),
      mAllocator(allocator),
      mHead(allocator) {
    mHead.push(new(allocator) Term(allocator, size, integer));
    assert(assertValid());
  }
  PolyInt(Allocator* allocator, int order, const Monom& a):
      Poly(order, a.size()),
      mAllocator(allocator),
      mHead(allocator) {
    mHead.push(new(allocator) Term(allocator, a));
    assert(assertValid());
  }
  PolyInt(Allocator* allocator, Monom::Variable v, const PolyInt& a);
  ~PolyInt() {
    if (mAllocator)
      clear();
  }

  void swap(PolyInt& a);
  void operator=(PolyInt &&a);
  void operator=(const PolyInt &a);

  ConstIterator begin() const { return mHead.begin(); }
  operator bool() const { return mHead; }
  bool isZero() const { return !mHead; }
  int length() const { return mHead.length(); }
  int maxPos() const;
  int degree() const {
    assert(mHead);
    return mHead.head()->mMonom.degree();
  }
//   int cmp(const PolyInt& a) const {
//     assert(mHead);
//     assert(a.mHead);
//     return mCmp1(lm(), a.lm());
//   }
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
  void add(const PolyInt &a);
  void sub(const char* hex);
  void sub(const PolyInt &a);
  void mult(const char* hex);
  void mult(const PolyInt &a);
  void pow(int deg);

  void reduction(const PolyInt& a);
  void redMaybe(const PolyInt& a);
  bool isPp() const;
  void pp();

  template <typename D> int nf(D &a);
  template <typename D> int nfTail(D &a);
  template <typename D> bool isNf(D &a) const;

  friend PolyInt operator-(PolyInt&& a) {
    PolyInt r(std::move(a));
    r.minus();
    return r;
  }
  friend inline PolyInt operator+(PolyInt&& a, const PolyInt& b) {
    PolyInt r(std::move(a));
    r.add(b);
    return r;
  }
  friend PolyInt operator-(PolyInt&& a, const PolyInt& b) {
    PolyInt r(std::move(a));
    r.sub(b);
    return r;
  }
  friend PolyInt operator*(PolyInt&& a, const PolyInt& b) {
    PolyInt r(std::move(a));
    r.mult(b);
    return r;
  }
  PolyInt pow(PolyInt&& a, int deg) {
    PolyInt r(std::move(a));
    r.pow(deg);
    return r;
  }

  friend std::ostream& operator<<(std::ostream& out, const PolyInt &a);

  bool assertValid() const;
};


class GCPolyInt: protected AllocatorPtr, public PolyInt {
public:
  GCPolyInt(const GCPolyInt &a):
      AllocatorPtr(),
      PolyInt(allocator(), a) {
  }
  GCPolyInt(const PolyInt &a):
      AllocatorPtr(),
      PolyInt(allocator(), a) {
  }
  GCPolyInt(Monom::Variable var, const PolyInt &a):
      AllocatorPtr(),
      PolyInt(allocator(), var, a) {
  }
  ~GCPolyInt() {}

  void swap(GCPolyInt &a) {
    AllocatorPtr::swap(a);
    PolyInt::swap(a);
  }

  void reallocate() {
#ifdef GINV_UTIL_ALLOCATOR
    if (allocator()->isGC()) {
      Allocator::timerCont();
      GCPolyInt a(*this);
      swap(a);
      Allocator::timerStop();
    }
#endif // GINV_UTIL_ALLOCATOR
  }
};

class WrapPolyInt: public Wrap {
  GCPolyInt    mPoly;

public:
  WrapPolyInt()=delete;
  WrapPolyInt(const WrapPolyInt &a)=delete;
  WrapPolyInt(const PolyInt &a):
      Wrap(a.lm()),
      mPoly(a) {
  }
  WrapPolyInt(Monom::Variable var, WrapPolyInt *a):
      Wrap(var, a),
      mPoly(var, a->mPoly) {
  }
  ~WrapPolyInt() {}

  void reallocate() {
#ifdef GINV_UTIL_ALLOCATOR
    mPoly.reallocate();
#endif // GINV_UTIL_ALLOCATOR
  }

  PolyInt& poly() { return mPoly; }
  const PolyInt& poly() const { return mPoly; }
};

typedef GC<List<WrapPolyInt*> > GCListWrapPolyInt;

template <typename D> int PolyInt::nf(D &a) {
  int r=0;
  if (!isZero()) {
    WrapPolyInt *wrap = a.find(lm());
    while(wrap) {
      reduction(wrap->poly());
      ++r;
      if (isZero())
        break;
      wrap = a.find(lm());
    }
  }
  return r;
}

template <typename D> int PolyInt::nfTail(D &a) {
  if (isZero())
    return 0;

  int r = 0;
  if (!isZero()) {
    WrapPolyInt *wrap = nullptr;
    List<Term*>::Iterator i(mHead.begin());
    do {
      ++i;
      if (!i)
        break;
      wrap = a.find(i.data()->mMonom);
    }
    while(wrap == nullptr);

    if (wrap) {
      do {
        redTail(i, wrap->poly());
        ++r;
        while(i) {
          wrap = a.find(i.data()->mMonom);
          if (wrap)
            break;
          ++i;
        }
      }
      while(i);
    }
  }
  return r;
}

template <typename D> bool PolyInt::isNf(D &a) const {
  List<Term*>::ConstIterator i(mHead.begin());
  if (i) {
    ++i;
    while(i) {
      if (a.find(i.data()->mMonom))
        break;
      ++i;
    }
  }
  return !i;
}

class JanetPolyInt {
  WrapPolyInt*         mOneWrap;

  GCListWrapPolyInt    mQ;
  GCListWrapPolyInt    mT;
  int                  mPos;
  GCJanet*             mJanet;

  Timer                mTimer;
  int                  mReduction;
  int                  mMaxEqualLm;
  int                  mCritI;
  int                  mCritII;
  int                  mIsZeroNf;
  
  void setPos();
  void setOne(int order, int size);
  void prolong(WrapPolyInt *w);
  bool assertT() const;
  
  static void insert(GCListWrapPolyInt &lst, WrapPolyInt *w);
  static void merge(GCListWrapPolyInt &lst, GCListWrapPolyInt &add);
  static bool assertSort(GCListWrapPolyInt &lst);

public:
  explicit JanetPolyInt():
      mOneWrap(nullptr),
      mQ(),
      mT(),
      mPos(-2),
      mJanet(nullptr),
      mTimer(),
      mReduction(0),
      mMaxEqualLm(0),
      mCritI(0),
      mCritII(0),
      mIsZeroNf(0) {
  }
  ~JanetPolyInt() {
    delete mOneWrap;
    for(GCListWrapPolyInt::ConstIterator j(mQ.begin()); j; ++j)
      delete j.data();
    for(GCListWrapPolyInt::ConstIterator j(mT.begin()); j; ++j)
      delete j.data();
    delete[] mJanet;
  }

  bool comparable(const PolyInt& a) const {
    return (!mQ && !mT) ||
      (mQ && mQ.head()->poly().comparable(a)) ||
      (mT && mT.head()->poly().comparable(a)) ||
      (mOneWrap && mOneWrap->poly().comparable(a));
  }
  void push(const PolyInt& a) {
    assert(comparable(a));
    if (mOneWrap == nullptr && a)
      insert(mQ, new WrapPolyInt(a));
  }


  void algorithmTQ();
  void algorithmTQ1();
  void algorithmBlockTQ();
  void build() {
    Allocator::setLimitMemory(size_t(6000)*1024*1024);
    algorithmTQ1();
  }

  bool isOne() const { return mOneWrap; }

  int size() const {  return (mOneWrap)? 1: mT.length(); }
  GCListWrapPolyInt::ConstIterator begin() const {
    return mT.begin();
  }
  WrapPolyInt* find(const Monom &m) const {
    if (mOneWrap)
      return mOneWrap;
    else if (mJanet == nullptr || mPos < m.pos())
      return nullptr;
    else
      return (WrapPolyInt*)mJanet[m.pos()+1].find(m);
  }
  void nf(PolyInt& a) const {
    assert(comparable(a));
    a.nf(*this);
    a.nfTail(*this);
    a.pp();
  }
  int maxPos() const { return mPos; }
  const Janet& janet(int pos) const {
    assert(mJanet != nullptr);
    assert(-1 <= pos && pos <= mPos);
    return mJanet[pos+1];
  }

  const Timer& timer() const { return mTimer; }
  int reduction() const { return mReduction; }
  int maxEqualLm() const { return mMaxEqualLm; }
  int critI() const { return mCritI; }
  int critII() const { return mCritII; }
  int isZeroNf() const { return mIsZeroNf; }
};

}

#endif // GINV_POLY_INT_H
