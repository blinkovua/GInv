/***************************************************************************
 *   Copyright (C) 2013 by Blinkov Yu. A.                                  *
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

#ifndef GINV_UTIL_STACK_H
#define GINV_UTIL_STACK_H

#include "config.h"
#include "allocator.h"

namespace GInv {

template <typename T>
class Stack {
  static_assert(std::is_integral<T>::value ||
                std::is_floating_point<T>::value ||
                std::is_pointer<T>::value,
    "typename T requires simple type: integral, floating and pointer");

  Allocator*  mAllocator;
  size_t      mSize;
  size_t      mDeep;
  T*          mArray;

  void resize() {
    assert(mDeep == mSize);
    size_t size=(mSize+1)*12/10;
    T* array=new(mAllocator) T[size];
    for(size_t i=0; i <  mDeep; i++)
      array[i] = mArray[i];
    mAllocator->dealloc(mArray, mSize);
    mSize = size;
    mArray = array;
  }

public:
  Stack() = delete;
  Stack(const Stack &a) = delete;
  inline Stack(size_t size,  Allocator* a):
      mAllocator(a),
      mSize(size),
      mDeep(0),
      mArray(new(mAllocator) T[mSize]) {
  }
  Stack(const Stack &a,  Allocator* allocator):
      mAllocator(allocator),
      mSize(a.mSize),
      mDeep(a.mDeep),
      mArray(new(mAllocator) T[mSize]) {
    for(size_t i=0; i < mDeep; i++)
      mArray[i] = a.mArray[i];
  }
  inline ~Stack() { mAllocator->dealloc(mArray, mSize); }

  void swap(Stack& a) {
    auto tmp1=mAllocator;
    mAllocator = a.mAllocator;
    a.mAllocator = tmp1;

    auto tmp2=mSize;
    mSize = a.mSize;
    a.mSize = tmp2;

    auto tmp3=mDeep;
    mDeep = a.mDeep;
    a.mDeep = tmp3;

    auto tmp4=mArray;
    mArray = a.mArray;
    a.mArray = tmp4;
  }
  void operator =(const Stack& a) {
    assert(this != &a);
    if (mSize < a.mDeep) {
      mAllocator->dealloc(mArray, mSize);
      mSize = a.mDeep;
      mArray = new(mAllocator) T[mSize];
    }
    for(mDeep=0; mDeep <  a.mDeep; mDeep++)
      mArray[mDeep] = a.mArray[mDeep];
  }

  inline size_t size() const { return mSize; }
  inline operator bool() const { return mDeep > 0; }
  inline size_t deep() const { return mDeep; }
  inline T top() const {
    assert(mDeep > 0);
    return mArray[mDeep-1];
  }
  inline T operator[](size_t i) const {
    assert(0 <= i && i < mDeep);
    return mArray[i];
  }

  inline void reset() {
    mDeep = 0;
  }
  inline void push(T data) {
    if (mDeep == mSize)
      resize();
    mArray[mDeep++] = data;
  }
  inline void pop() {
    assert(mDeep > 0);
    --mDeep;
  }
};

}

#endif // GINV_UTIL_STACK_H
