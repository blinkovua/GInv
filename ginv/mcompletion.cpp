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

#include "mcompletion.h"

namespace GInv {

void MCompletion::insert(Wrap *w) {
  ListWrap::Iterator j(mT.begin());
  while(j && mCmp(*j.data(), *w) < 0) {
    for(int v=0; v < w->lm().size(); v++) {
      if (w->lm()[v] > j.data()->lm()[v]) {
        j.data()->setNM(v);
        break;
      }
    }
    ++j;
  }
  if (!j || mCmp(*j.data(), *w) > 0)
    j.insert(w);
  else
    mAllocator->destroy(w);
  ++j;
  while(j) {
    for(int v=0; v < w->lm().size(); v++) {
      if (j.data()->lm()[v] > w->lm()[v]) {
        w->setNM(v);
        break;
      }
    }
    ++j;
  }
}

void MCompletion::prolong() {
  for(ListWrap::ConstIterator j(mT.begin()); j; ++j)
    for(int k=0; k < j.data()->lm().size(); k++)
      if (j.data()->NM(k) && !j.data()->build(k)) {
        mQ.push(new(mAllocator) Wrap(k, j.data()));
      }
}


MCompletion::~MCompletion() {
  for(ListWrap::Iterator j(mQ.begin()); j; ++j)
    mAllocator->destroy(j.data());
  for(ListWrap::Iterator j(mT.begin()); j; ++j)
    mAllocator->destroy(j.data());
}

void MCompletion::build() {
  prolong();
  while(mQ) {
    for(ListWrap::Iterator j(mQ.begin()); j; j.del()) {
      if (find(j.data()->lm()))
        mAllocator->destroy(j.data());
      else
        insert(j.data());
    }
//     break;
    prolong();
  }
}

Wrap* MCompletion::find(const Monom& m) const {
  Wrap* r=nullptr;
  for(ListWrap::ConstIterator j(mT.begin()); j; ++j) {
    int v=0;
    for(; v < m.size(); v++) {
      if (j.data()->lm()[v] > m[v] ||
         (j.data()->lm()[v] < m[v] && j.data()->NM(v)))
        break;
    }
    if (v == m.size()) {
      r = j.data();
      break;
    }
  }
  return r;
}

void MJanet::clear(Allocator* allocator, Janet::ConstIterator j) {
  while(j) {
    if (j.wrap())
      allocator->destroy(j.wrap());
    if (j.nextVar())
      clear(allocator, j.nextVar());
    j.deg();
  }
}

void MJanet::prolong(Janet::ConstIterator j) {
  while(j) {
    if (j.wrap())
      for(int k=0; k < j.wrap()->lm().size(); k++)
        if (j.wrap()->NM(k) && !j.wrap()->build(k))
          mQ.push(new(mAllocator) Wrap(k, j.wrap()));
    if (j.nextVar())
      prolong(j.nextVar());
    j.deg();
  }
}

MJanet::~MJanet() {
//   std::cerr << "A " << mAllocator->size() << std::endl;
  for(ListWrap::Iterator j(mQ.begin()); j; ++j)
    mAllocator->destroy(j.data());
//   std::cerr << "B " << mAllocator->size() << std::endl;
  clear(mAllocator, mJanet.begin());
//   std::cerr << "C " << mAllocator->size() << std::endl;
}

void MJanet::build() {
  prolong(mJanet.begin());
  while(mQ) {
    for(ListWrap::Iterator j(mQ.begin()); j; j.del()) {
      if (mJanet.find(j.data()->lm()))
        mAllocator->destroy(j.data());
      else
        mJanet.insert(j.data());
    }
    prolong(mJanet.begin());
  }
}

}
