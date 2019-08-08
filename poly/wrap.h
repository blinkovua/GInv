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

#ifndef GINV_POLY_WRAP_H
#define GINV_POLY_WRAP_H

#include "util/allocator.h"
#include "util/list.h"
#include "monom.h"

#include "config.h"

namespace GInv {

class Wrap {
  Allocator*  mAllocator;
  Monom       mLm;
  Monom       mAnsector;
  bool*       mNM;
  bool*       mNMd;
  bool*       mBuild;

public:
  Wrap()=delete;
  Wrap(const Wrap& a)=delete;
  Wrap(Allocator* allocator, const Monom& m):
      mAllocator(allocator),
      mLm(allocator, m),
      mAnsector(allocator, m),
      mNM(new(allocator) bool[m.size()]),
      mNMd(new(allocator) bool[m.size()]),
      mBuild(new(allocator) bool[m.size()]) {
    for(int i=0; i < mLm.size(); i++) {
      mNM[i] = false;
      mNMd[i] = false;
      mBuild[i] = false;
    }
  }
  Wrap(Allocator* allocator, Monom::Variable var, const Wrap* ansector):
      mAllocator(allocator),
      mLm(allocator, var, ansector->mLm),
      mAnsector(allocator, ansector->mAnsector),
      mNM(new(allocator) bool[ansector->mLm.size()]),
      mNMd(new(allocator) bool[ansector->mLm.size()]),
      mBuild(new(allocator) bool[ansector->mLm.size()]) {
    assert((ansector->mNM[var] || ansector->mNMd[var]) && !ansector->mBuild[var]);
    ansector->mBuild[var] = true;
    for(int i=0; i < mLm.size(); i++) {
      mNM[i] = false;
      mNMd[i] = false;
      mBuild[i] = false;
    }
  }
  ~Wrap() {
    mAllocator->dealloc(mNM, mLm.size());
    mAllocator->dealloc(mNMd, mLm.size());
    mAllocator->dealloc(mBuild, mLm.size());
  }

  const Monom& lm() const { return mLm; }
  const Monom& ansector() const { return mAnsector; }
  bool isGB() const { return mAnsector.degree() == mLm.degree(); }

  void setNM(int var) const {
    assert(0 <= var && var < mLm.size());
    mNM[var] = true;
  }
  void setNMd(int var) const {
    assert(0 <= var && var < mLm.size());
    mNMd[var] = true;
  }
  bool NM(int var) const {
    assert(0 <= var && var < mLm.size());
    return mNM[var];
  }
  bool NMd(int var) const {
    assert(0 <= var && var < mLm.size());
    return mNMd[var];
  }
  bool build(int var) const {
    assert(0 <= var && var < mLm.size());
    return mBuild[var];
  }

  friend std::ostream& operator<<(std::ostream& out, const Wrap &a);
};

typedef List<Wrap*> ListWrap;

}

#endif // GINV_POLY_WRAP_H
