/***************************************************************************
 *   Copyright (C) 2016 by Blinkov Yu. A. and Fokin P. V.                  *
 *   BlinkovUA@info.sgu.ru and fokinpv@gmail.com                           *
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

#ifndef GINV_BPOLY_H
#define GINV_BPOLY_H

#include <cassert>

#include "util/list.h"
#include "config.h"
#include "bmonom.h"

namespace GInv {

class BPoly {
protected:
  Allocator*           mAllocator;
  List<const BMonom*>  mList;

  inline void clear() {
    while(mList) {
      mAllocator->destroy(mList.pop());
    }
  }

  void add1(BPoly& a);

public:
  class ConstIterator {
    friend class BPoly;
    List<const BMonom*>::ConstIterator mIterator;

    inline ConstIterator(const List<const BMonom*>::ConstIterator& a):
        mIterator(a) {}

  public:
    inline ConstIterator():
        mIterator() {}
    inline ConstIterator(const ConstIterator& a):
        mIterator(a.mIterator) {}
    inline ~ConstIterator() {}

    inline void operator=(const ConstIterator& a) { mIterator = a.mIterator; }

    inline operator bool() const { return mIterator.operator bool(); }
    inline bool operator!=(const ConstIterator& a) { mIterator != a.mIterator; }
    inline bool operator==(const ConstIterator& a) { mIterator == a.mIterator; }


    inline void operator++() {  mIterator.operator++(); }

    inline const BMonom& monom() const { return *mIterator.data(); }
  };

  BPoly() = delete;
  BPoly(const BPoly& a) = delete;
  BPoly(Allocator *allocator):
      mAllocator(allocator),
      mList(allocator) {
  }
  BPoly(const BPoly& a, Allocator *allocator);
//   BPoly(const ZDD::ConstIterator a, Allocator *allocator);
  ~BPoly() { clear(); }

  void canonicalize();

  inline void swap(BPoly &a) {
    auto tmp=mAllocator;
    mAllocator = a.mAllocator;
    a.mAllocator = tmp;

    mList.swap(a.mList);
  }
  void operator=(const BPoly &a);

  inline operator bool() const { return mList; }
  inline size_t length() const { return mList.length(); }
  inline bool isZero() const { return !mList; }
  inline bool isOne() const { return mList && mList.head()->isZero(); }
  inline const BMonom& lm() const { assert(mList); return *mList.head(); }
  bool tailOne() const;

  inline ConstIterator begin() const { return mList.begin(); }

  void setOne() {
    clear();
    mList.push(new(mAllocator) BMonom(mAllocator));
  }
  void set(BMonom::Variable v, bool negation) {
    clear();
    if (negation)
      mList.push(new(mAllocator) BMonom(mAllocator));
    mList.push(new(mAllocator) BMonom(v, mAllocator));
  }
  void set(const BMonom& a) {
    clear();
    mList.push(new(mAllocator) BMonom(a, mAllocator));
  }

  void mult(const BPoly& a, BMonom::Variable var);
  void mult(const BPoly& a, const BMonom& m);

  void addOne();
  void add(const BPoly& a);
//   inline void addmult(BMonom::Variable var) {
//     BPoly tmp(mAllocator);
//     tmp.mult(*this, var);
//     add1(tmp);
//   }
  inline void addmult(const BPoly& a, BMonom::Variable var) {
    BPoly tmp(mAllocator);
    tmp.mult(a, var);
    add1(tmp);
  }
  inline void addmult(const BPoly& a, const BMonom& m) {
    BPoly tmp(mAllocator);
    tmp.mult(a, m);
    add1(tmp);
  }

  void OR(BMonom::Variable var, bool negation=false);
  template <typename D> void nf(const D& a) {
    assert(mList);
    BPoly *f=a.find(mList.head());
    if (f) {
      BMonom m(mAllocator);
      do {
        assert(mList);
        assert(f);
        m.div(*mList.head(), *f->mList.head());
        if (m.degree() == 0)
          add(*f);
        else if (m.degree() == 1)
          addmult(*f, m[0]);
        else
          addmult(*f, m);
        if (mList)
          f = a.find(mList.head());
        else
          break;
      } while(f);
    }
  }
  template <typename D> bool isNfTail(const D& a) const {
    assert(mList);
    List<const BMonom*>::ConstIterator i(mList.begin());
    ++i;
    while(i && !a.find(i.data()))
      ++i;
    return !i;
  }
  template <typename D> void nfTail(const D& a) {
    assert(mList);
    BMonom m(mAllocator);
    List<const BMonom*>::Iterator i(mList.begin());
    ++i;
    while(i) {
      BPoly *f=a.find(i.data());
      if (f == nullptr)
        ++i;
      else {
        m.div(*i.data(), *f->mList.head());
        if (m.degree() == 0)
          add(*f);
        else if (m.degree() == 1)
          addmult(*f, m[0]);
        else
          addmult(*f, m);
      }
    }
    assert(isNfTail(a));
  }

  friend std::ostream& operator<<(std::ostream& out, const BPoly &a);

  bool assertValid() const;
};

typedef GC<BPoly> BPolyGC;

// class AutoReduce {
// public:
//   typedef GC<List<DegRevLexGC*> > Container;
//
// private:
//   int        mSize;
//   Container* mContainer;
//
// public:
//   AutoReduce(int vars):
//     mSize(vars),
//     mContainer(new Container[mSize]) {
//   }
//   ~AutoReduce();
//
//   int size() const { return mSize; }
//   Container& operator[](int i) const {
//     assert(0 < i && i <= mSize);
//     return mContainer[i-1];
//   }
//   void add(DegRevLexGC* p);
//
//   BPoly* find(const BPoly::BMonom* m) const;
//   bool autoReduce();
// };

}

#endif // GINV_BPOLY_H
