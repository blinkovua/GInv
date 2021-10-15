/***************************************************************************
 *   Copyright (C) 2018 by Blinkov Yu.A.                                   *
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

#include "./gb.h"

namespace GInv {

const int GB::sMaxDegree = 20;

GB::GB(Allocator* allocator):
    mAllocator(allocator),
    mSize(0),
    mMaxDegree(sMaxDegree),
    mForest(new(mAllocator) Janet*[mMaxDegree]) {
  for(int i=0; i < mMaxDegree; i++)
    mForest[i] = new(mAllocator) Janet(mAllocator);
}

GB::~GB() {
  for(int i=0; i < mMaxDegree; i++)
    mAllocator->destroy(mForest[i]);
  mAllocator->dealloc(mForest, mMaxDegree);
}

Wrap* GB::find(const Monom &m) const {
  Wrap *r=nullptr;
  for(int i=0; i < mMaxDegree; i++) {
    Wrap *tmp=mForest[i]->find(m);
    if (tmp) {
      int v=0;
      for(; v < m.size(); v++)
        if (m[v] > tmp->lm()[v] && tmp->NMd(v))
          break;
      if (v == m.size())
        r = tmp;
    }
    if (r)
      break;
  }
  return r;
}

void GB::insert(Wrap *wrap) {
  assert(find(wrap->lm()) == nullptr);
  int d=wrap->ansector().degree();
  if (d >= mMaxDegree) {
    int d12=d*13/10;
    Janet** tmp=new(mAllocator) Janet*[d12];
    for(int i=0; i < mMaxDegree; i++)
      tmp[i] = mForest[i];
    for(int i=mMaxDegree; i < d12; i++)
      tmp[i] = new(mAllocator) Janet(mAllocator);
    mAllocator->dealloc(mForest, mMaxDegree);
    mMaxDegree = d12;
    mForest = tmp;
  }
  assert(d < mMaxDegree);
  for(int i=0; i < d; i++)
    mForest[i]->setMNprec(wrap);
  mForest[d]->insert(wrap);
  for(int i=d; i < mMaxDegree; i++)
    mForest[i]->setMNsucc(wrap);
  ++mSize;
}

}
