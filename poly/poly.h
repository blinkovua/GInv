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
#include "monom.h"
#include "integer.h"
#include "janet.h"

#include "config.h"

namespace GInv {

class Poly {
protected:
  struct Term {
    Monom    mMonom;
    Integer  mCoeff;
  };

  Allocator*   mAllocator;
  List<Term*>  mHead;

  void minus();
  void add(const Poly &a);
  void sub(const Poly &a);
  void mult(const Poly &a);
  void pow(int deg);
  
public:
  class ConstIterator: {
    List<Term*>::ConstIterator mConstIt;

    ConstIterator(const List<Term*>::Iterator& constIt):
        mConstIt(constIt) {}

  public:
    ~ConstIterator() {}

    const Coeff& coeff() const { return mConstIt.data()->mCoeff; }
    const Monom& monom() const { return mConstIt.data()->mMonom; }
    operator bool() const { return mConstIt; }
    bool operator!=(const ConstIterator& a) { return mConstIt != a.mConstIt; }
    bool operator==(const ConstIterator& a) { return mConstIt == a.mConstIt; }
    void operator++() {
      assert(mConstIt);
      ++mConstIt;
    }
  };
  
  Poly()=delete;
  Poly(const Poly& a)=delete;
  Poly(Poly&& a):
      mAllocator(a.mAllocator),
      mHead(a.mAllocator, a.mHead) {
  }
  Poly(Allocator* allocator, Variable v, int size, int pos):
      mAllocator(allocator),
      mHead(allocator) {
    mHead.push(new(allocator) Term(allocator, v, size, pos));
  }
  Poly(Allocator* allocator, const Monom& a):
      mAllocator(allocator),
      mHead(allocator) {
    mHead.push(new(allocator) Term(allocator, a));
  }
  Poly(Allocator* allocator, const Integer& a):
      mAllocator(allocator),
      mHead(allocator) {
    mHead.push(new(allocator) Term(allocator, a));
  }
  Poly(Allocator* allocator, const Poly& a);
  ~Poly();

  void swap(Poly& a);
  void operator=(Poly &&a) {
  void operator=(const Poly &a);
  
  ConstIterator begin() const { return mHead; }
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
  const Monom& lc() const { 
    assert(mHead);
    return mHead.head()->mCoeff;
  }

  friend Poly operator-(Poly&& a) {
    Poly r(std::move(a));
    r.minus();
    return std::move(r);
  }
  friend Poly operator+(Poly&& a, const Poly& b) {
    Poly r(std::move(a));
    r.add(b);
    return std::move(r);
  }
  friend Poly operator-(Poly&& a, const Poly& b) {
    Poly r(std::move(a));
    r.sub(b);
    return std::move(r);
  }
  friend Poly operator*(Poly&& a, const Poly& b) {
    Poly r(std::move(a));
    r.mult(b);
    return std::move(r);
  }
  Poly pow(Poly&& a, int deg) {
    Poly r(std::move(a));
    r.pow(a, deg);
    return std::move(r);
  }
  
  void nf(Janet &a);
  void nfTail(Janet &a);
  bool isPp() const;
  void pp();

  friend std::ostream& operator<<(std::ostream& out, const Poly &a);

  bool assertValid() const;
};


}

#endif // GINV_POLY_POLY_H
