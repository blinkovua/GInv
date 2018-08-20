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

#ifndef GINV_UTIL_ALLOCATOR_H
#define GINV_UTIL_ALLOCATOR_H

#include <cstddef>
#include <cstdlib>
#include <cassert>

#include "config.h"
#include "timer.h"

namespace GInv {

class Allocator {
  static size_t  sCurrMemory;
  static size_t  sMaxMemory;
  static Timer   sTimer;

  size_t         mAlloc=0;
  size_t         mSize=0;

#ifdef GINV_UTIL_ALLOCATOR
  struct Node {
    void* mPointer;
    Node* mNext;

    Node() = delete;
    Node(size_t size, Node* next=nullptr):
        mPointer(malloc(size)),
        mNext(next) {
    }
    ~Node() { free(mPointer); }
  };

  Node*  mRoot=nullptr;
  size_t mNodeAlloc=0;
  size_t mNodeSize=0;
#endif // GINV_UTIL_ALLOCATOR

public:
  static size_t maxMemory() { return sMaxMemory; }
  static size_t currMemory() { return sCurrMemory; }

  static const Timer& timer() { return sTimer; }
  static void timerCont() { sTimer.cont(); }
  static void timerStop() { sTimer.stop(); }

  Allocator() {}
  Allocator(const Allocator& a);
  ~Allocator();

  void swap(Allocator& a);

  void* allocate(size_t n);
  void deallocate(const void* ptr, size_t n);

  template <typename T>
  inline void dealloc(const T *ptr) {
    deallocate(ptr, sizeof(T));
  }
  template <typename T>
  inline void dealloc(const T *ptr, size_t n) {
    assert(n > 0);
    deallocate(ptr, sizeof(T)*n);
  }
  template <typename T>
  inline void destroy(const T *ptr) {
    ptr->~T();
    deallocate(ptr, sizeof(T));
  }
  template <typename T>
  inline void destroy(const T *ptr, size_t n) {
    assert(n > 0);
    for(int i=0; i < n; i++)
      ptr[i].~T();
    deallocate(ptr, sizeof(size_t) + sizeof(T)*n);
  }

  size_t alloc() const { return mAlloc; }
  size_t size() const { return mSize; }
  bool isGC() const;
};

class AllocatorPtr {
  Allocator* mAllocator;

public:
  AllocatorPtr():
      mAllocator(new Allocator) {
  }
  AllocatorPtr(const AllocatorPtr& a):
      mAllocator(new Allocator(*a.mAllocator)) {
  }
  ~AllocatorPtr() { delete mAllocator; }

  const Allocator* allocator() const { return mAllocator; }
  Allocator* allocator() { return mAllocator; }

  void swap(AllocatorPtr& a) {
    Allocator *tmp=mAllocator;
    mAllocator = a.mAllocator;
    a.mAllocator = tmp;
  }
};

template <typename T>
class GC: protected AllocatorPtr, public T {
public:
  GC():
      AllocatorPtr(),
      T(allocator()) {
  }
  GC(const GC &a):
      AllocatorPtr(a),
      T(a, allocator()) {
  }
  GC(const T &a):
      AllocatorPtr(),
      T(a, allocator()) {
  }
  ~GC() {}

  void operator=(const GC &a) {
    assert(this != &a);
    T::operator=(a);
  }

  void swap(GC &a) {
    AllocatorPtr::swap(a);
    T::swap(a);
  }

  void reallocate() {
#ifdef GINV_UTIL_ALLOCATOR
    if (allocator()->isGC()) {
      Allocator::timerCont();
      GC a(*this);
      swap(a);
      Allocator::timerStop();
    }
#endif // GINV_UTIL_ALLOCATOR
  }
  size_t alloc() const { return allocator()->alloc(); }
  size_t size() const { return allocator()->size(); }
  bool isGC() const { return allocator()->isGC(); }
};

}

inline void* operator new(std::size_t size, GInv::Allocator* allocator)  {
  return allocator->allocate(size);
}

inline void* operator new[](std::size_t size, GInv::Allocator* allocator) {
  return allocator->allocate(size);
}

#endif // GINV_UTIL_ALLOCATOR_H
