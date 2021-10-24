/***************************************************************************
 *   Copyright (C) 2012 by Blinkov Yu. A.                                  *
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

#ifndef GINV_UTIL_LIST_H
#define GINV_UTIL_LIST_H

#include <cstddef>

#include "config.h"
#include "allocator.h"

namespace GInv {

template <typename T>
class List {
  static_assert(std::is_integral<T>::value ||
                std::is_floating_point<T>::value ||
                std::is_pointer<T>::value,
    "typename T requires simple type: integral, floating and pointer");

  struct Node {
    T       mData;
    Node*   mNext;

    Node()=delete;
    Node(const Node&)=delete;
    Node(T data, Node* next):
        mData(data),
        mNext(next) {
    }
    ~Node() {}
  };

  Allocator*  mAllocator;
  Node*       mHead;
  size_t      mLength;

public:
  class Iterator;
  friend class Iterator;

  class ConstIterator {
    friend class Iterator;
    Node* mLink;

  public:
    inline ConstIterator(): mLink(nullptr) {}
    inline ConstIterator(const ConstIterator& a): mLink(a.mLink) {}
    inline ConstIterator(Node* node): mLink(node) {}
    inline ~ConstIterator() {}

    inline void operator=(const ConstIterator& a) { mLink = a.mLink; }

    inline operator bool() const { return mLink != nullptr; }
    inline bool operator!=(const ConstIterator& a) { return mLink != a.mLink; }
    inline bool operator==(const ConstIterator& a) { return mLink == a.mLink; }
    inline void operator++() {
      assert(mLink != nullptr);
      mLink = mLink->mNext;
    }

    inline T& data() const { assert(mLink); return mLink->mData; }
  };

  class Iterator {
    List*      mList;
    Node**     mLink;

  public:
    inline Iterator():
        mList(nullptr),
        mLink(nullptr) {
    }
    inline Iterator(const Iterator& a):
        mList(a.mList),
        mLink(a.mLink) {
    }
    inline Iterator(List* list):
        mList(list),
        mLink(&mList->mHead) {
    }
    inline ~Iterator() {}

    inline operator ConstIterator() {
      assert(mLink);
      return *mLink;
    }

    inline void operator=(const Iterator& a) {
      mList = a.mList;
      mLink = a.mLink;
    }

    inline operator bool() const { return mLink != nullptr && *mLink != nullptr; }
    inline bool operator!=(const Iterator& a) { return mLink != a.mLink; }
    inline bool operator==(const Iterator& a) { return mLink == a.mLink; }
    inline void operator++() {
      assert(mLink != nullptr);
      mLink = &(*mLink)->mNext;
    }

    inline T& data() const {
      assert(mLink);
      return (*mLink)->mData;
    }

    inline void insert(T data) {
      assert(mLink);
      *mLink = new(mList->mAllocator) Node(data, *mLink);
      ++mList->mLength;
    }
    inline void insert(ConstIterator j) {
      assert(mLink);
      j.mLink->mNext = *mLink;
      *mLink = j.mLink;
      ++mList->mLength;
    }
    inline ConstIterator get() {
      assert(mLink);
      ConstIterator j(*mLink);
      *mLink = (*mLink)->mNext;
      j.mLink->mNext = nullptr;
      --mList->mLength;
      return j;
    }
    inline void del() {
      assert(mLink);
      auto tmp=*mLink;
      *mLink = tmp->mNext;
      mList->mAllocator->destroy(tmp);
      --mList->mLength;
    }
    void clear() { //TODO
      assert(mLink);
      while (*mLink)
        del();
      assert(mList->mLength == 0);
    }
  };

  List()=delete;
  List(const List&)=delete;
  inline List(Allocator *allocator):
      mAllocator(allocator),
      mHead(nullptr),
      mLength(0) {
  }
  inline List(List&& a):
      mAllocator(a.mAllocator),
      mHead(a.mHead),
      mLength(a.mLength) {
    a.mHead = nullptr;
    a.mLength = 0;
  }
  List(Allocator* allocator, const List& a):
      mAllocator(allocator),
      mHead(nullptr),
      mLength(0) {
    Iterator i(this);
    ConstIterator ia(a.mHead);
    while(ia) {
      i.insert(ia.data());
      ++ia;
      ++i;
    }
  }
  inline ~List() { Iterator(this).clear(); }

  inline void clear() { Iterator(this).clear(); }

  inline void swap(List& a) {
    auto tmp1=mAllocator;
    mAllocator = a.mAllocator;
    a.mAllocator = tmp1;

    auto tmp2 = mHead;
    mHead = a.mHead;
    a.mHead = tmp2;

    auto tmp3 = mLength;
    mLength = a.mLength;
    a.mLength = tmp3;
  }

  inline void operator=(const List& a) {
    assert(this != &a);
    Iterator(this).clear();

    Iterator i(this);
    ConstIterator ia(a.mHead);
    while(ia) {
      i.insert(ia.data());
      ++ia;
      ++i;
    }
  }
  inline void operator=(const List&& a) {
    assert(this != &a);

    Iterator(this).clear();

    mAllocator = a.mAllocator;
    mHead = a.mHead;
    mLength = a.mLength;

    a.mHead = nullptr;
    a.mLength = 0;
  }

  inline operator bool() const { return mHead != nullptr; }
  inline size_t length() const { return mLength; }

  inline ConstIterator begin() const { return mHead; }
  inline Iterator begin() { return this; }

  inline T head() const { assert(mHead); return mHead->mData; }

  inline void push(T data) {
    mHead = new(mAllocator) Node(data, mHead);
    ++mLength;
  }
  inline T pop() {
    assert(mHead);
    auto tmp=mHead;
    mHead = mHead->mNext;
    T r=tmp->mData;
    mAllocator->destroy(tmp);
    --mLength;
    return r;
  }
};

}

#endif // GINV_UTIL_LIST_H
