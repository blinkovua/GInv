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
  assert(comparable(a));
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
  assert(assertValid());
}

void PolyInt::redTail(List<Term*>::Iterator i1, const PolyInt& a) {
  assert(comparable(a));
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
  assert(assertValid());
}

void PolyInt::redMaybe(const PolyInt& a) {
  assert(!isZero() && a);
  List<Term*>::Iterator i(mHead.begin());
  while(i && mCmp1(i.data()->mMonom, a.lm()) > 0)
    ++i;
  if (i && i.data()->mMonom == a.lm())
    redTail(i, a);
}

bool PolyInt::isPp() const {
  if (!mHead)
    return true;
  else {
    Allocator allocator[1];
    List<Term*>::ConstIterator i(mHead.begin());
    if (i.data()->mCoeff.isAbsOne())
      return true;
    Integer g(allocator, i.data()->mCoeff);
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
    Allocator allocator[1];
    List<Term*>::ConstIterator i(mHead.begin());
    if (i.data()->mCoeff.isAbsOne())
      return;
    Integer g(allocator, i.data()->mCoeff);
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
    assert(isPp());
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

void JanetPolyInt::setPos() {
  int pos=-1;
  for(GCListWrapPolyInt::ConstIterator j(mQ.begin()); j; ++j) {
    int p=j.data()->poly().maxPos();
    pos = (p > pos)? p: pos;
  }
  if (pos > mPos) {
    GCJanet *tmp=new GCJanet[pos+2];
    int i;
    for(i=0; i < mPos+2; i++)
      tmp[i].swap(mJanet[i]);
    delete[] mJanet;
    mPos = pos;
    mJanet = tmp;
  }
}

void JanetPolyInt::setOne(int order, int size) {
  assert(mOneWrap == nullptr);
  for(GCListWrapPolyInt::Iterator j(mQ.begin()); j; j.del())
    delete j.data();
  for(GCListWrapPolyInt::Iterator j(mT.begin()); j; j.del())
    delete j.data();

  delete[] mJanet;
  mJanet = nullptr;
  
  Allocator allocator[1];
  mOneWrap = new WrapPolyInt(PolyInt(allocator, order, size, "0x1"));  
}

void JanetPolyInt::prolong(WrapPolyInt *w) {
  Allocator allocator[1];
  for(int i=0; i < w->lm().size(); i++)
    if (w->NM(i) && !w->build(i)) {
      Monom m(allocator, i, w->lm());
      const WrapPolyInt *d=find(m);
      if (d && m.degree() >=
        (w->ansector().degree() + d->ansector().degree())) {
        ++mCritI;
        w->setBuild(i);
      }
      else if (d && m.degree() > w->ansector().lcm(d->ansector())) {
        ++mCritII;
        w->setBuild(i);
      }
      else
        insert(mQ, new WrapPolyInt(i, w));
    }  
}

bool JanetPolyInt::assertT() const {
  if (mT) {
    GCListWrapPolyInt::ConstIterator i(mT.begin());
    GCListWrapPolyInt::ConstIterator prev(i);
    ++i;
    while(i) {
      if (prev.data()->poly().lm().deglex(i.data()->poly().lm()) >= 0)
        return false;
      if (!i.data()->poly().isNf(*this))
        return false;
      if (!i.data()->poly().isPp())
        return false;
      prev = i;
      ++i;
    }
  }
  return true;  
}
  
void JanetPolyInt::insert(GCListWrapPolyInt &lst, WrapPolyInt *w) {
  assert(!w->poly().isZero());
  GCListWrapPolyInt::Iterator j(lst.begin());
  int n=w->poly().norm();
  while(j && (w->poly().lm().deglex(j.data()->poly().lm()) > 0))
    ++j;
  j.insert(w);
}

void JanetPolyInt::merge(GCListWrapPolyInt &lst, GCListWrapPolyInt &add) {
  
}


bool JanetPolyInt::assertSort(GCListWrapPolyInt &lst) {
  if (lst) {
    GCListWrapPolyInt::ConstIterator i(lst.begin());
    GCListWrapPolyInt::ConstIterator prev(i);
    ++i;
    while(i) {
      if (prev.data()->poly().lm().deglex(i.data()->poly().lm()) > 0)
        return false;
      prev = i;
      ++i;
    }
  }
  return true;
}

void JanetPolyInt::algorithmBlockTQ() {
  if (mOneWrap)
      return;

  setPos();

  int degPref=0;
  int critI=0;
  mTimer.start();
  while(mQ) {
    int size=mQ.head()->poly().size();
    int order=mQ.head()->poly().order();

    std::cerr << "1 mQ - " << mQ.length() << " mT - " << mT.length() << std::endl;

    int deg=mQ.head()->poly().degree();
    GCListWrapPolyInt::Iterator j(mQ.begin());
    GCListWrapPolyInt tmp;
    while(j && j.data()->poly().lm().degree() <= deg) {
      int r=j.data()->poly().nf(*this);
      if (j.data()->poly().isZero()) {
        std::cerr << "1 =0" << std::endl;
        delete j.data();
      }
      else {
        if (j.data()->poly().degree() == 0) {
          for(GCListWrapPolyInt::Iterator j(tmp.begin()); j; j.del())
            delete j.data();

          setOne(order, size);
          std::cerr << "deg - 0" << *mOneWrap << std::endl;
          break;
        }
        
        r = j.data()->poly().nfTail(*this);
        if (r) {
          mReduction += r;
          j.data()->poly().pp();
          j.data()->reallocate();
        }
        insert(tmp, j.data());
        std::cerr << "1 "<< j.data()->poly().lm()  << " " << j.data()->poly().length()
          << " " << j.data()->poly().norm() << std::endl;
      }
      j.del();
    }
    assert(assertSort(mQ));
    assert(assertSort(tmp));

std::cerr << "A" << std::endl;

    if (mOneWrap)
      break;

    if (degPref == 0)
      degPref = deg;
    
    if (!tmp)
      continue;
    
    GCListWrapPolyInt::Iterator k1(tmp.begin());
    assert(k1);
    do {
      std::cerr << "AA - " << k1.data()->poly().lm() << std::endl;
      deg = k1.data()->poly().lm().degree();
      GCListWrapPolyInt::Iterator k(k1);
      ++k;
      while(k && deg == k.data()->poly().lm().degree()) {
        ++k;
      }
      j = mQ.begin();
      while(k) {
        j.insert(k.data());
        k.del();
        ++j;
      }
      assert(assertSort(mQ));
      assert(assertSort(tmp));
      
//       std::cerr << "AAAAA len - " << std::endl;
      
      GCListWrapPolyInt tmp2;
      k = k1;
      do {
        GCListWrapPolyInt::Iterator j(k);
        ++j;
        while(j && j.data()->poly().lm() == k.data()->poly().lm()) {
          tmp2.push(j.data());
          j.del();
        }
        if (tmp2) 
          break;
        else {
          while(j) {
            j.data()->poly().redMaybe(k.data()->poly());
            assert(!j.data()->poly().isZero());
            ++j;
          }
          ++k;
          j = k;
        }
      } while(k);
      
      if (!tmp2)
        break;
      else {
        tmp2.push(k.data());
        k.del();
        std::cerr << "AAA len - " << tmp2.length() << std::endl;
        k = tmp2.begin();
//         std::cerr << "AAA poly - " << k.data()->poly() << std::endl;
        j = k;
        ++j;
        while(j) {
//           std::cerr << "AAA poly - " << j.data()->poly() << std::endl;
          assert(j.data()->poly().lm() == k.data()->poly().lm());
          if (j.data()->poly().norm() < k.data()->poly().norm())
            k = j;
          ++j;
        }
        
        WrapPolyInt *w=k.data();
        k.del();
//         std::cerr << "AAA poly k - " << w->poly() << std::endl;
        insert(tmp, w);
        j = tmp2.begin();
        while(j) {
//           std::cerr << "AAA poly j - " << j.data()->poly() << std::endl;
          assert(j.data()->poly());
          j.data()->poly().reduction(w->poly());
          if (j.data()->poly().isZero())
            delete j.data();
          else
            insert(tmp, j.data());
          ++j;
        }
        assert(assertSort(tmp));
        
        k1 = tmp.begin();
        assert(k1);
      }
    } while(k1);

    GCListWrapPolyInt::Iterator k(tmp.begin());
    while(k) {
      k.data()->update(k.data()->poly().lm());
      k.data()->poly().pp();
      mJanet[k.data()->lm().pos()+1].insert(k.data());
      ++k;
    }

//     if (degPref > deg) {
//       delete[] mJanet;
//       mJanet = new GCJanet[mPos+2];
//       for(GCListWrapPolyInt::Iterator j(mT.begin()); j; j.del()) {
//         j.data()->clearNM();
//         mQ.push(j.data());
//       }
//     }
    degPref = deg;


    std::cerr << "2 mQ - " << mQ.length() << " mT - " << mT.length() << std::endl;

    j = tmp.begin();
    k = mT.begin();
    while(k && j) {
      if (j.data()->lm().deglex(k.data()->lm()) < 0) {
        assert(j.data()->poly().isNf(*this));
        k.insert(j.data());
        ++j;
      }
      else {
        int r=k.data()->poly().nfTail(*this);
        if (r) {
          mReduction += r;
          k.data()->poly().pp();
          k.data()->reallocate();
        }
      }
      if (k.data()->lm().degree() <= deg)
        prolong(k.data());
      ++k;
    }
    while(j) {
      assert(j.data()->poly().isNf(*this));
      k.insert(j.data());
      ++j;
      if (k.data()->lm().degree() <= deg)
        prolong(k.data());
      ++k;
    }
    while(k) {
      int r=k.data()->poly().nfTail(*this);
      if (r) {
        mReduction += r;
        k.data()->poly().pp();
        k.data()->reallocate();
      }
      if (k.data()->lm().degree() <= deg)
        prolong(k.data());
      ++k;
    }
    assert(assertT());


    std::cerr << "critI - " <<  mCritI << " critII - " <<  mCritII << 
    " mQ - " << mQ.length() << " mT - " << mT.length() << std::endl;
  }
  mTimer.stop();
  std::cerr << timer() << std::endl;

  std::cerr << "Allocator::maxMemory() - "<< Allocator::maxMemory() << std::endl;
  std::cerr << "Allocator::currMemory() - "<< Allocator::currMemory() << std::endl;
  std::cerr << "Allocator::timer() - "<< Allocator::timer() << std::endl;
}

void JanetPolyInt::algorithmTQ1() {
  if (mOneWrap)
      return;

  setPos();

  int degPref=0;
  int critI=0;
  mTimer.start();
  while(mQ) {
    int size=mQ.head()->poly().size();
    int order=mQ.head()->poly().order();

    std::cerr << "1 mQ - " << mQ.length() << " mT - " << mT.length() << std::endl;

    int deg=mQ.head()->poly().degree();
    GCListWrapPolyInt::Iterator j(mQ.begin());
    GCListWrapPolyInt tmp;
    while(j && j.data()->poly().lm().degree() <= deg) {
//     while(j) {
      int r=j.data()->poly().nf(*this);
      if (j.data()->poly().isZero()) {
//         std::cerr << "1 =0" << std::endl;
        delete j.data();
      }
      else {
        if (j.data()->poly().degree() == 0) {
          for(GCListWrapPolyInt::Iterator j(tmp.begin()); j; j.del())
            delete j.data();

          setOne(order, size);
          std::cerr << "deg - 0" << *mOneWrap << std::endl;
          break;
        }
        
        if (r) {
          mReduction += r;
          j.data()->poly().pp();
          j.data()->reallocate();
        }
        insert(tmp, j.data());
//         std::cerr << "1 "<< j.data()->poly().lm()  << " " << j.data()->poly().length()
//           << " " << j.data()->poly().norm() << std::endl;
      }
      j.del();
    }
    assert(assertSort(mQ));
    assert(assertSort(tmp));
    
    if (mOneWrap)
      break;

    if (degPref == 0)
      degPref = deg;
    
    if (!tmp)
      continue;

    WrapPolyInt *w=tmp.pop();
    int r=w->poly().nfTail(*this);
    if (r) {
      mReduction += r;
      w->poly().pp();
      w->reallocate();
    }
    GCListWrapPolyInt::Iterator k(tmp.begin());
    int maxEqualLm=1;
    while(k && k.data()->poly().lm() == w->poly().lm()) {
      ++maxEqualLm;
      int r=k.data()->poly().nfTail(*this);
      if (r) {
        mReduction += r;
        k.data()->poly().pp();
        k.data()->reallocate();
      }
      if (k.data()->poly().norm() < w->poly().norm()) {
        auto w1=w;
        w = k.data();
        k.data() = w1;
      }
      ++k;
    }
    if (mMaxEqualLm < maxEqualLm)
      mMaxEqualLm = maxEqualLm;
    if (maxEqualLm > 1) {
      std::cerr << "maxEqualLm - " << maxEqualLm << std::endl;
      k = tmp.begin();
      while(k && k.data()->poly().lm() == w->poly().lm()) {
        k.data()->poly().reduction(w->poly());
        if (k.data()->poly().isZero()) {
          delete k.data();
          k.del();
        }
        else {
          k.data()->poly().pp();
          k.data()->reallocate();
          ++k;
        }
      }
      j = mQ.begin();
      while(k) {
        j.insert(k.data());
        k.del();
        ++j;
      }
      insert(mQ, w);
      k = tmp.begin();
      while(k) {
        insert(mQ, k.data());
        k.del();
      }
      assert(assertSort(mQ));
      continue;
    }

    j = mQ.begin();
    k = tmp.begin();
    while(k) {
      j.insert(k.data());
      k.del();
      ++j;
    }
    assert(assertSort(mQ));

    w->update(w->poly().lm());
    mJanet[w->lm().pos()+1].insert(w);

//     if (degPref > deg) {
//       delete[] mJanet;
//       mJanet = new GCJanet[mPos+2];
//       for(GCListWrapPolyInt::Iterator j(mT.begin()); j; j.del()) {
//         j.data()->clearNM();
//         mQ.push(j.data());
//       }
//     }
    degPref = deg;


    std::cerr << "2 mQ - " << mQ.length() << " mT - " << mT.length() 
      << " " << w->lm()  << " " << w->poly().length()
      << " " << w->poly().norm() << std::endl;


    if (!mT)
      mT.push(w);
    else {
      GCListWrapPolyInt::Iterator k(mT.begin());
      while(k) {
        if (w && w->poly().lm().deglex(k.data()->poly().lm()) < 0) {
          k.insert(w);
          w = nullptr;
        }
        int r=k.data()->poly().nfTail(*this);
        if (r) {
          mReduction += r;
          k.data()->poly().pp();
          k.data()->reallocate();
        }
        if (k.data()->lm().degree() <= deg)
          prolong(k.data());
        ++k;
        if (w && !k ) {
          k.insert(w);
          w = nullptr;
        }
      }
    }
    assert(assertT());

    std::cerr << "critI - " <<  mCritI << " critII - " <<  mCritII << 
    " mQ - " << mQ.length() << " mT - " << mT.length() << std::endl;
  }
  mTimer.stop();
  std::cerr << timer() << std::endl;

  std::cerr << "Allocator::maxMemory() - "<< Allocator::maxMemory() << std::endl;
  std::cerr << "Allocator::currMemory() - "<< Allocator::currMemory() << std::endl;
  std::cerr << "Allocator::timer() - "<< Allocator::timer() << std::endl;
  
  std::cerr << "maxEqualLm() - "<< maxEqualLm() << std::endl;
}

void JanetPolyInt::algorithmTQ() {
  if (mOneWrap)
      return;

  setPos();

  int degPref=0;
  int critI=0;
  mTimer.start();
  while(mQ) {
    int size=mQ.head()->poly().size();
    int order=mQ.head()->poly().order();

    std::cerr << "1 mQ - " << mQ.length() << " mT - " << mT.length() << std::endl;

    int deg=mQ.head()->poly().degree();
    GCListWrapPolyInt::Iterator j(mQ.begin());
    GCListWrapPolyInt tmp;
    while(j && j.data()->poly().lm().degree() <= deg) {
      int r=j.data()->poly().nf(*this);
      if (j.data()->poly().isZero()) {
        std::cerr << "1 =0" << std::endl;
        delete j.data();
      }
      else {
        if (j.data()->poly().degree() == 0) {
          for(GCListWrapPolyInt::Iterator j(tmp.begin()); j; j.del())
            delete j.data();

          setOne(order, size);
          std::cerr << "deg - 0" << *mOneWrap << std::endl;
          break;
        }
        
        if (r) {
          mReduction += r;
          j.data()->poly().pp();
          j.data()->reallocate();
        }
        insert(tmp, j.data());
        std::cerr << "1 "<< j.data()->poly().lm()  << " " << j.data()->poly().length()
          << " " << j.data()->poly().norm() << std::endl;
      }
      j.del();
    }
    assert(assertSort(mQ));
    assert(assertSort(tmp));
    
    if (mOneWrap)
      break;

    if (degPref == 0)
      degPref = deg;
    
    if (!tmp)
      continue;

    WrapPolyInt *w=tmp.pop();
//     int r=w->poly().nfTail(*this);
//     if (r) {
//       mReduction += r;
//       w->poly().pp();
//       w->reallocate();
//     }
    GCListWrapPolyInt::Iterator k(tmp.begin());
    int maxEqualLm=1;
    while(k && k.data()->poly().lm() == w->poly().lm()) {
      ++maxEqualLm;
//       int r=k.data()->poly().nfTail(*this);
//       if (r) {
//         mReduction += r;
//         k.data()->poly().pp();
//         k.data()->reallocate();
//       }
      if (k.data()->poly().norm() < w->poly().norm()) {
        auto w1=w;
        w = k.data();
        k.data() = w1;
      }
      ++k;
    }
    assert(assertSort(tmp));
    if (mMaxEqualLm < maxEqualLm)
      mMaxEqualLm = maxEqualLm;

    j = mQ.begin();
    k = tmp.begin();
    while(k) {
      j.insert(k.data());
      k.del();
      ++j;
    }
     assert(assertSort(mQ));

    w->update(w->poly().lm());
    mJanet[w->lm().pos()+1].insert(w);

//     if (degPref > deg) {
//       delete[] mJanet;
//       mJanet = new GCJanet[mPos+2];
//       for(GCListWrapPolyInt::Iterator j(mT.begin()); j; j.del()) {
//         j.data()->clearNM();
//         mQ.push(j.data());
//       }
//     }
    degPref = deg;


    std::cerr << "2 mQ - " << mQ.length() << " mT - " << mT.length() 
      << " " << w->lm()  << " " << w->poly().length()
      << " " << w->poly().norm() << std::endl;


    if (!mT)
      mT.push(w);
    else {
      GCListWrapPolyInt::Iterator k(mT.begin());
      while(k) {
        if (w && w->poly().lm().deglex(k.data()->poly().lm()) < 0) {
          k.insert(w);
          w = nullptr;
        }
        int r=k.data()->poly().nfTail(*this);
        if (r) {
          mReduction += r;
          k.data()->poly().pp();
          k.data()->reallocate();
        }
        if (k.data()->lm().degree() <= deg)
          prolong(k.data());
        ++k;
        if (w && !k ) {
          k.insert(w);
          w = nullptr;
        }
      }
    }
    assert(assertT());

    std::cerr << "critI - " <<  mCritI << " critII - " <<  mCritII << 
    " mQ - " << mQ.length() << " mT - " << mT.length() << std::endl;
  }
  mTimer.stop();
  std::cerr << timer() << std::endl;

  std::cerr << "Allocator::maxMemory() - "<< Allocator::maxMemory() << std::endl;
  std::cerr << "Allocator::currMemory() - "<< Allocator::currMemory() << std::endl;
  std::cerr << "Allocator::timer() - "<< Allocator::timer() << std::endl;
  
  std::cerr << "maxEqualLm() - "<< maxEqualLm() << std::endl;
}

}
