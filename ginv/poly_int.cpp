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

#include "poly_int.h"

namespace GInv {

void PolyInt::clear() {
  List<Term*>::Iterator i(mHead.begin());
  while(i) {
    mAllocator->destroy(i.data());
    i.del();
  }
}

PolyInt::PolyInt(Allocator* allocator, Monom::Variable v, const PolyInt& a):
      Poly(a.mOrder, a.mSize),
      mAllocator(allocator),
      mHead(allocator) {
  List<Term*>::ConstIterator ia(a.mHead.begin());
  List<Term*>::Iterator i(mHead.begin());
  while(ia) {
    i.insert(new(mAllocator) Term(mAllocator, v, *ia.data()));
    ++ia;
    ++i;
  }
  assert(assertValid());
}

PolyInt::PolyInt(Allocator* allocator, const PolyInt& a):
      Poly(a.mOrder, a.mSize),
      mAllocator(allocator),
      mHead(allocator) {
  List<Term*>::ConstIterator ia(a.mHead.begin());
  List<Term*>::Iterator i(mHead.begin());
  while(ia) {
    i.insert(new(mAllocator) Term(mAllocator, *ia.data()));
    ++ia;
    ++i;
  }
  assert(assertValid());
}

void PolyInt::swap(PolyInt& a) {
  assert(this != &a);

  Poly::swap(a);

  auto tmp=mAllocator;
  mAllocator = a.mAllocator;
  a.mAllocator = tmp;

  mHead.swap(a.mHead);
}

void PolyInt::operator=(PolyInt &&a) {
  assert(this != &a);

  Poly::operator=(a);
  clear();
  swap(a);
}


void PolyInt::operator=(const PolyInt &a) {
  assert(this != &a);

  Poly::operator=(a);

  clear();

  List<Term*>::Iterator i(mHead.begin());
  List<Term*>::ConstIterator ia(a.mHead.begin());
  while(ia) {
    i.insert(new(mAllocator) Term(mAllocator, *ia.data()));
    ++ia;
    ++i;
  }
  assert(assertValid());
}

int PolyInt::maxPos() const {
  int pos=-1;
  List<Term*>::ConstIterator i(mHead.begin());
  while(i) {
    pos = (i.data()->mMonom.pos() > pos) ? i.data()->mMonom.pos(): pos;
    ++i;
  }
  return pos;
}

int PolyInt::norm() const {
  int sum=0;
  List<Term*>::ConstIterator i(mHead.begin());
  while(i) {
    sum += i.data()->mCoeff.norm();
    ++i;
  }
  return sum;
}


void PolyInt::minus() {
  List<Term*>::ConstIterator i(mHead.begin());
  while(i) {
    i.data()->mCoeff.set_neg();
    ++i;
  }
  assert(assertValid());
}

void PolyInt::add(const char* hex) {
  assert(mHead);
  List<Term*>::Iterator i(mHead.begin());
  assert(i);
  int size=i.data()->mMonom.size();
  while(i && (i.data()->mMonom.degree() > 0 ||
              i.data()->mMonom.pos() >= 0))
    ++i;
  if (!i)
    i.insert(new(mAllocator) Term(mAllocator, size, hex));
  else {
    assert(i.data()->mMonom.degree() == 0 &&
           i.data()->mMonom.pos() == -1);
    Integer a(mAllocator, hex);
    i.data()->mCoeff.add(a);
    if (i.data()->mCoeff.isZero()) {
      mAllocator->destroy(i.data());
      i.del();
    }
  }
  assert(assertValid());
}

void PolyInt::add(const PolyInt &a) {
  assert(comparable(a));
  List<Term*>::ConstIterator ia(a.mHead.begin());
  List<Term*>::Iterator i(mHead.begin());
  while(i && ia)
    switch(mCmp1(i.data()->mMonom, ia.data()->mMonom)) {
    case 1:
      ++i;
      break;
    case -1:
      i.insert(new(mAllocator) Term(mAllocator, *ia.data()));
      ++i;
      ++ia;
      break;
    default:
      i.data()->mCoeff.add(ia.data()->mCoeff);
      if (!i.data()->mCoeff.isZero())
        ++i;
      else {
        mAllocator->destroy(i.data());
        i.del();
      }
      ++ia;
    }

  while(ia) {
    i.insert(new(mAllocator) Term(mAllocator, *ia.data()));
    ++i;
    ++ia;
  }
  assert(assertValid());
}

void PolyInt::sub(const char* hex) {
  assert(mHead);
  List<Term*>::Iterator i(mHead.begin());
  assert(i);
  int size=i.data()->mMonom.size();
  while(i && (i.data()->mMonom.degree() > 0 ||
              i.data()->mMonom.pos() >= 0))
    ++i;
  if (!i) {
    i.insert(new(mAllocator) Term(mAllocator, size, hex));
    i.data()->mCoeff.set_neg();
  }
  else {
    assert(i.data()->mMonom.degree() == 0 &&
           i.data()->mMonom.pos() == -1);
    Integer a(mAllocator, hex);
    i.data()->mCoeff.sub(a);
    if (i.data()->mCoeff.isZero()) {
      mAllocator->destroy(i.data());
      i.del();
    }
  }
  assert(assertValid());
}

void PolyInt::sub(const PolyInt &a) {
  assert(comparable(a));
  List<Term*>::ConstIterator ia(a.mHead.begin());
  List<Term*>::Iterator i(mHead.begin());
  while(i && ia)
    switch(mCmp1(i.data()->mMonom, ia.data()->mMonom)) {
    case 1:
      ++i;
      break;
    case -1:
      i.insert(new(mAllocator) Term(mAllocator, *ia.data()));
      i.data()->mCoeff.set_neg();
      ++i;
      ++ia;
      break;
    default:
      i.data()->mCoeff.sub(ia.data()->mCoeff);
      if (!i.data()->mCoeff.isZero())
        ++i;
      else {
        mAllocator->destroy(i.data());
        i.del();
      }
      ++ia;
    }

  while(ia) {
    i.insert(new(mAllocator) Term(mAllocator, *ia.data()));
    i.data()->mCoeff.set_neg();
    ++i;
    ++ia;
  }
  assert(assertValid());
}

void PolyInt::mult(const char* hex) {
  if (mHead.length() == 0)
    ;
  else {
    Integer c(mAllocator, hex);
    if (c.isZero())
      clear();
    else {
      List<Term*>::ConstIterator i(mHead.begin());
      while(i) {
        i.data()->mCoeff.mult(c);
        ++i;
      }
    }
  }
  assert(assertValid());
}

void PolyInt::mult(const PolyInt &a) {
  assert(comparable(a));
  if (mHead.length() == 0)
    ;
  else if (a.mHead.length() == 0)
    clear();
  else if (mHead.length() == 1 || a.mHead.length() == 1) {
    List<Term*> tmp(mAllocator);
    List<Term*>::Iterator j(tmp.begin());
    if (mHead.length() == 1) {
      Monom &m=mHead.head()->mMonom;
      Integer &c=mHead.head()->mCoeff;

      List<Term*>::ConstIterator i(a.mHead.begin());
      while(i) {
        j.insert(new(mAllocator) Term(mAllocator, i.data()->mMonom, m));
        j.data()->mCoeff.mult(i.data()->mCoeff, c);
        ++i;
        ++j;
      }
    }
    else {
      assert(a.mHead.length() == 1);
      Monom &m=a.mHead.head()->mMonom;
      Integer &c=a.mHead.head()->mCoeff;

      List<Term*>::ConstIterator i(mHead.begin());
      while(i) {
        j.insert(new(mAllocator) Term(mAllocator, i.data()->mMonom, m));
        j.data()->mCoeff.mult(i.data()->mCoeff, c);
        ++i;
        ++j;
      }
    }
    mHead.swap(tmp);
    mAllocator->destroy(tmp.head());
  }
  else {
    List<Term*> sum(mAllocator);
    List<Term*>::ConstIterator i(mHead.begin());
    assert(i);
    do {
      Monom &m=i.data()->mMonom;
      Integer &c=i.data()->mCoeff;

      List<Term*> tmp(mAllocator);
      List<Term*>::Iterator j(tmp.begin());
      List<Term*>::ConstIterator l(a.mHead.begin());
      while(l) {
        j.insert(new(mAllocator) Term(mAllocator, l.data()->mMonom, m));
        j.data()->mCoeff.mult(l.data()->mCoeff, c);
        ++l;
        ++j;
      }
      ++i;

      List<Term*>::Iterator k(sum.begin());
      j = tmp.begin();
      while(k && j)
        switch(mCmp1(k.data()->mMonom, j.data()->mMonom)) {
        case 1:
          ++k;
          break;
        case -1:
          k.insert(j.get());
          ++k;
          break;
        default:
          k.data()->mCoeff.add(j.data()->mCoeff);
          if (!k.data()->mCoeff.isZero())
            ++k;
          else {
            mAllocator->destroy(k.data());
            k.del();
          }
          mAllocator->destroy(j.data());
          j.del();
        }

      while(j) {
        k.insert(j.get());
        ++k;
      }
    } while(i);
    mHead.swap(sum);
    List<Term*>::Iterator k(sum.begin());
    while(k) {
      mAllocator->destroy(k.data());
      k.del();
    }
  }
  assert(assertValid());
}

void PolyInt::pow(int deg) {
  assert(deg >= 0);
  if (deg == 1)
    ;
  else if (deg == 0)
    clear();
  else {
    assert(deg > 1);
    PolyInt tmp(mAllocator, *this);
    for(--deg; deg > 0; deg--)
      tmp.mult(*this);
    swap(tmp);
  }
}

void PolyInt::reduction(const PolyInt& a) {
  assert(mCmp1 == a.mCmp1 && mCmp2 == a.mCmp2);
  assert(lm().divisiable(a.lm()));

  Allocator allocator[1];

  Monom m2(allocator, lm());
  m2.div(a.lm());

  Integer tmp1(allocator), tmp2(allocator), k1(allocator), k2(allocator);
  tmp1.gcd(lc(), a.lc());
  k1.div(a.lc(), tmp1);
  k2.div(lc(), tmp1);
  k2.set_neg();

  List<Term*>::Iterator i(mHead.begin());
  List<Term*>::ConstIterator ia(a.mHead.begin());

  mAllocator->destroy(i.data());
  i.del();
  ++ia;
  while(i && ia) {
    switch(mCmp2(i.data()->mMonom, ia.data()->mMonom, m2)) {
    case 1:
      i.data()->mCoeff.mult(k1);
      ++i;
      break;
    case -1:
      i.insert(new(mAllocator) Term(mAllocator, ia.data()->mMonom, m2));
      i.data()->mCoeff.mult(ia.data()->mCoeff, k2);
      ++i;
      ++ia;
      break;
    default:
      tmp1.mult(i.data()->mCoeff, k1);
      tmp2.mult(ia.data()->mCoeff, k2);
      i.data()->mCoeff.add(tmp1, tmp2);
      if (i.data()->mCoeff.isZero()) {
        mAllocator->destroy(i.data());
        i.del();
      }
      else
        ++i;
      ++ia;
    }
  }
  while(i) {
    i.data()->mCoeff.mult(k1);
    ++i;
  }
  while(ia) {
    i.insert(new(mAllocator) Term(mAllocator, ia.data()->mMonom, m2));
    i.data()->mCoeff.mult(ia.data()->mCoeff, k2);
    ++i;
    ++ia;
  }
}

void PolyInt::redTail(List<Term*>::Iterator i1, const PolyInt& a) {
  assert(mCmp1 == a.mCmp1 && mCmp2 == a.mCmp2);
  assert(i1.data()->mMonom.divisiable(a.lm()));

  Allocator allocator[1];

  Monom m2(allocator, i1.data()->mMonom);
  m2.div(a.lm());

  Integer tmp1(allocator), tmp2(allocator), k1(allocator), k2(allocator);
  tmp1.gcd(i1.data()->mCoeff, a.lc());
  k1.div(a.lc(), tmp1);
  k2.div(i1.data()->mCoeff, tmp1);
  k2.set_neg();

  List<Term*>::Iterator i(mHead.begin());
  while(i != i1) {
    assert(i);
    i.data()->mCoeff.mult(k1);
    ++i;
  }

  List<Term*>::ConstIterator ia(a.mHead.begin());

  mAllocator->destroy(i.data());
  i.del();
  ++ia;
  while(i && ia) {
    switch(mCmp2(i.data()->mMonom, ia.data()->mMonom, m2)) {
    case 1:
      i.data()->mCoeff.mult(k1);
      ++i;
      break;
    case -1:
      i.insert(new(mAllocator) Term(mAllocator, ia.data()->mMonom, m2));
      i.data()->mCoeff.mult(ia.data()->mCoeff, k2);
      ++i;
      ++ia;
      break;
    default:
      tmp1.mult(i.data()->mCoeff, k1);
      tmp2.mult(ia.data()->mCoeff, k2);
      i.data()->mCoeff.add(tmp1, tmp2);
      if (i.data()->mCoeff.isZero()) {
        mAllocator->destroy(i.data());
        i.del();
      }
      else
        ++i;
      ++ia;
    }
  }
  while(i) {
    i.data()->mCoeff.mult(k1);
    ++i;
  }
  while(ia) {
    i.insert(new(mAllocator) Term(mAllocator, ia.data()->mMonom, m2));
    i.data()->mCoeff.mult(ia.data()->mCoeff, k2);
    ++i;
    ++ia;
  }
}

bool PolyInt::isPp() const {
  if (!mHead)
    return true;
  else {
    List<Term*>::ConstIterator i(mHead.begin());
    if (i.data()->mCoeff.isAbsOne())
      return true;
    Integer g(mAllocator, i.data()->mCoeff);
    ++i;
    while(i) {
      g.gcd(i.data()->mCoeff);
      if (g.isAbsOne())
        return true;
      ++i;
    }
    return false;
  }
}

void PolyInt::pp() {
  if (mHead) {
    List<Term*>::ConstIterator i(mHead.begin());
    if (i.data()->mCoeff.isAbsOne())
      return;
    Integer g(mAllocator, i.data()->mCoeff);
    ++i;
    while(i) {
      g.gcd(i.data()->mCoeff);
      if (g.isAbsOne())
        return;
      ++i;
    }

    i = mHead.begin();
    while(i) {
      i.data()->mCoeff.div(g);
      ++i;
    }
  }
}

std::ostream& operator<<(std::ostream& out, const PolyInt &a) {
  PolyInt::ConstIterator i(a.begin());
  if (!i)
    out << '0';
  else {
    assert(i);
    out << i.monom() << '*' << i.coeff().get_str(10);
    ++i;
    while(i) {
      out << " + "<< i.monom() << '*' << i.coeff().get_str(10);
      ++i;
    }
  }
  return out;
}

bool PolyInt::assertValid() const {
  if (mHead) {
    List<Term*>::ConstIterator i(mHead.begin());
    if (i.data()->mCoeff.isZero())
      return false;
    List<Term*>::ConstIterator prev(i);
    ++i;
    while(i) {
      if (mCmp1(i.data()->mMonom, prev.data()->mMonom) >= 0)
        return false;
      if (i.data()->mCoeff.isZero())
        return false;
      prev = i;
      ++i;
    }
  }
  return true;
}

void JanetPolyInt::build() {
  if (mOneWrap)
      return;

  std::cerr << "START" << std::endl;
  int pos=-1;
  for(GCListWrapPolyInt::ConstIterator j(mQ.begin()); j; ++j) {
    int p=j.data()->poly().maxPos();
    pos = (p > pos)? p: pos;
  }
  std::cerr << "pos - " << pos << std::endl;
  if (pos > mPos) {
    GCJanet *tmp=new GCJanet[pos+2];
    int i;
    for(i=0; i < mPos+2; i++)
      tmp[i].swap(mJanet[i]);
    delete[] mJanet;
    mPos = pos;
    mJanet = tmp;
  }
  std::cerr << "mPos - " << mPos << std::endl;

  int degPref=0;
  while(mQ) {
    int size=mQ.head()->poly().size();
    int order=mQ.head()->poly().order();

    std::cerr << "1 mQ - " << mQ.length() << " mT - " << mT.length() << std::endl;

    int deg=mQ.head()->poly().degree();
    GCListWrapPolyInt::Iterator j(mQ.begin());
    while(j) {
      mReduction += j.data()->poly().nf(*this);
      if (j.data()->poly().isZero()) {
        delete j.data();
        j.del();
      }
      else {
        int d=j.data()->poly().degree();
//         std::cerr << d << " AA " << j.data()->poly() << std::endl;
        deg = (deg > d)? d: deg;
        j.data()->poly().pp();
        j.data()->reallocate();
        ++j;
      }
    }

    std::cerr << "deg - " << deg << std::endl;

    if (degPref == 0)
      degPref = deg;

    j = mQ.begin();
    while(j) {
//       std::cerr << j.data()->poly() << std::endl;
      if (j.data()->poly().degree() > deg)
        ++j;
      else {
        mReduction += j.data()->poly().nf(*this);
        if (j.data()->poly().isZero())
          delete j.data();
        else {
          if (j.data()->poly().degree() == 0) {
            assert(mOneWrap == nullptr);
            for(GCListWrapPolyInt::Iterator j(mQ.begin()); j; j.del())
              delete j.data();
            for(GCListWrapPolyInt::Iterator j(mT.begin()); j; j.del())
              delete j.data();
            delete[] mJanet;
            mJanet = nullptr;
            Allocator allocator[1];
            mOneWrap = new WrapPolyInt(PolyInt(allocator, order, size, "0x1"));
            std::cerr << "deg - 0" << *mOneWrap << std::endl;
            break;
          }

//           std::cerr << "A" << std::endl;
          mReduction += j.data()->poly().nfTail(*this);
//           std::cerr << "AA" << std::endl;
          j.data()->poly().pp();
//           std::cerr << "AAA" << std::endl;
          j.data()->update(j.data()->poly().lm());
//           std::cerr << "AAAA" << std::endl;
          j.data()->reallocate();
//           std::cerr << "AAAAA " << j.data()->poly() << std::endl;
          mJanet[j.data()->lm().pos()+1].insert(j.data());
//           std::cerr << "AAAAAA" << std::endl;
          mT.push(j.data());
//           std::cerr << "AAAAAAA" << std::endl;
        }
        j.del();
      }
    }
    
    if (degPref > deg) {
      delete[] mJanet;
      mJanet = new GCJanet[mPos+2];
      for(GCListWrapPolyInt::Iterator j(mT.begin()); j; j.del()) {
//         for(int i=0; i < size; i++)
//           j.data()->setNM(i) = false;
        mQ.push(j.data());
      }
    }
    degPref = deg;

    if (mOneWrap)
      break;

    std::cerr << "2 mQ - " << mQ.length() << " mT - " << mT.length() << std::endl;

    for(GCListWrapPolyInt::ConstIterator k(mT.begin()); k; ++k) {
      mReduction += k.data()->poly().nfTail(*this);
      k.data()->poly().pp();
      k.data()->reallocate();
      if (k.data()->lm().degree() <= deg+1)
        for(int i=0; i < size; i++)
          if (k.data()->NM(i) && !k.data()->build(i))
            mQ.push(new WrapPolyInt(i, k.data()));
    }

    std::cerr << "3 mQ - " << mQ.length() << " mT - " << mT.length() << std::endl;

//     break;
  }
}

}
