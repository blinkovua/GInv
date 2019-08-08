/***************************************************************************
 *   Copyright (C) 2009 by Blinkov Yu. A.                                  *
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

#include "allocator.h"

namespace GInv {

#ifdef GINV_UTIL_ALLOCATOR

const size_t memoryPageSize=4096;

Allocator::Allocator(const Allocator& a) {
  mNodeAlloc = memoryPageSize;
  mAlloc = mNodeAlloc;
  mRoot = new Node(mNodeAlloc);
  sCurrMemory += mNodeAlloc;
  sMaxMemory = (sMaxMemory > sCurrMemory) ? sMaxMemory: sCurrMemory;
}

Allocator::~Allocator() {
  if (mSize != 0)
    std::cerr << "(mSize != 0) "<< mSize << std::endl;

  assert(mSize == 0);
  sCurrMemory -= mAlloc;
  while(mRoot) {
    Node *tmp = mRoot;
    mRoot = mRoot->mNext;
    delete tmp;
  }
}

void Allocator::swap(Allocator& a) {
  auto tmp1=mAlloc;
  mAlloc = a.mAlloc;
  a.mAlloc = tmp1;

  auto tmp2=mSize;
  mSize = a.mSize;
  a.mSize = tmp2;

  auto tmp3=mRoot;
  mRoot = a.mRoot;
  a.mRoot = tmp3;

  auto tmp4=mNodeAlloc;
  mNodeAlloc = a.mNodeAlloc;
  a.mNodeAlloc = tmp4;

  auto tmp5=mNodeSize;
  mNodeSize = a.mNodeSize;
  a.mNodeSize = tmp5;
}

void* Allocator::allocate(size_t n) {
  assert(n > 0);
  n = ((n >> 6) + 1) << 6;
  if (mNodeSize + n > mNodeAlloc) {
    mNodeAlloc = ((n + memoryPageSize) / memoryPageSize)*memoryPageSize;
    mAlloc += mNodeAlloc;
    mRoot = new Node(mNodeAlloc, mRoot);
    mNodeSize = 0;
    sCurrMemory += mNodeAlloc;
    sMaxMemory = (sMaxMemory >= sCurrMemory) ? sMaxMemory: sCurrMemory;
  }
  assert(mNodeSize + n <= mNodeAlloc);
  void *r=(char*)mRoot->mPointer + mNodeSize;
  mSize += n;
  mNodeSize += n;
  assert(mNodeSize <= mNodeAlloc);
  return r;
}

void Allocator::deallocate(const void*, size_t n) {
  assert(n > 0);
  n = ((n >> 6) + 1) << 6;
  assert(mSize >= n);
  mSize -= n;
}

bool Allocator::isGC() const {
  return mAlloc > memoryPageSize && mAlloc > mSize*2;
}

#else

Allocator::Allocator(const Allocator& a) {
}

Allocator::~Allocator() {
  assert(mAlloc == mSize);
  assert(mSize == 0);
  sCurrMemory -= mAlloc;
}

void Allocator::swap(Allocator& a) {
  auto tmp1=mAlloc;
  mAlloc = a.mAlloc;
  a.mAlloc = tmp1;

  auto tmp2=mSize;
  mSize = a.mSize;
  a.mSize = tmp2;
}

const int addSize=sizeof(size_t)*2;
void* Allocator::allocate(size_t n) {
  assert(n > 0);
  size_t full=n + addSize;
  mAlloc += full;
  mSize += full;
  sCurrMemory += full;
  sMaxMemory = (sMaxMemory > sCurrMemory) ? sMaxMemory: sCurrMemory;
  return malloc(n);
}

void Allocator::deallocate(const void* ptr, size_t n) {
  assert(n > 0);
  size_t full=n + addSize;
  mAlloc -= full;
  mSize -= full;
  sCurrMemory -= full;
  free((void*)ptr);
}

bool Allocator::isGC() const {
  return false;
}

#endif // GINV_UTIL_ALLOCATOR

size_t Allocator::sCurrMemory = 0;
size_t Allocator::sMaxMemory = 0;
Timer  Allocator::sTimer;

}
