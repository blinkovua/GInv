/***************************************************************************
 *   Copyright (C) 2015 by Blinkov Yu. A.                                  *
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

#ifndef GINV_UTIL_RANDPERMUTATION_H
#define GINV_UTIL_RANDPERMUTATION_H

#include "config.h"

#include <cassert>
#include <random>
#include <iostream>

namespace GInv {
  
class RandPermutation {
  const int                            mSize; 
  int                                 *mPerm;
  std::uniform_int_distribution<int> **mDis;
  
public:
  RandPermutation()=delete;
  explicit RandPermutation(int n);
  RandPermutation(const RandPermutation&)=delete;
  ~RandPermutation();
  
  inline int size() const { return mSize; }
  inline int operator[](int i) const {
    assert(0 <= i && i < mSize);
    return mPerm[i];
  }

  template<class Generator> void operator()(Generator& g) {
    for(int i=mSize-1; i > 1; i--) {
      auto j=(*mDis[i-2])(g);
      auto swap=mPerm[i];
      mPerm[i] = mPerm[j];
      mPerm[j] = swap;
    }
  }
  
  friend std::ostream& operator<<(std::ostream& out, const RandPermutation &a);
};

}

#endif // GINV_UTIL_RANDPERMUTATION_H