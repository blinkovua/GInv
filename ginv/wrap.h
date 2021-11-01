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

#ifndef GINV_WRAP_H
#define GINV_WRAP_H

#include "util/allocator.h"
#include "util/list.h"
#include "monom.h"

#include "config.h"

namespace GInv {

class Wrap {
  Allocator   mAllocator;
  Monom       mLm;
  Monom       mAnsector;
  bool*       mNM;
  bool*       mNMd;
  bool*       mBuild;

public:
  Wrap()=delete;
  Wrap(const Wrap& a):
      mAllocator(),
      mLm(&mAllocator, a.mLm),
      mAnsector(&mAllocator, a.mAnsector),
      mNM(new(&mAllocator) bool[a.mLm.size()]),
      mNMd(new(&mAllocator) bool[a.mLm.size()]),
      mBuild(new(&mAllocator) bool[a.mLm.size()]) {
    for(int i=0; i < mLm.size(); i++) {
      mNM[i] = a.mNM[i];
      mNMd[i] = a.mNMd[i];
      mBuild[i] = a.mBuild[i];
    }
  }
  Wrap(const Monom& m):
      mAllocator(),
      mLm(&mAllocator, m),
      mAnsector(&mAllocator, m),
      mNM(new(&mAllocator) bool[m.size()]),
      mNMd(new(&mAllocator) bool[m.size()]),
      mBuild(new(&mAllocator) bool[m.size()]) {
    for(int i=0; i < mLm.size(); i++) {
      mNM[i] = false;
      mNMd[i] = false;
      mBuild[i] = false;
    }
  }
  Wrap(Monom::Variable var, const Wrap* ansector):
      mAllocator(),
      mLm(&mAllocator, var, ansector->mLm),
      mAnsector(&mAllocator, ansector->mAnsector),
      mNM(new(&mAllocator) bool[ansector->mLm.size()]),
      mNMd(new(&mAllocator) bool[ansector->mLm.size()]),
      mBuild(new(&mAllocator) bool[ansector->mLm.size()]) {
    assert((ansector->mNM[var] || ansector->mNMd[var]) && !ansector->mBuild[var]);
    ansector->mBuild[var] = true;
    for(int i=0; i < mLm.size(); i++) {
      mNM[i] = false;
      mNMd[i] = false;
      mBuild[i] = false;
    }
  }
  ~Wrap() {
    mAllocator.dealloc(mNM, mLm.size());
    mAllocator.dealloc(mNMd, mLm.size());
    mAllocator.dealloc(mBuild, mLm.size());
  }

  void update(const Monom& m);
  void clearNM() const {
    for(int i=0; i < mLm.size(); i++) {
      mNM[i] = false;
    }
  }

  const Monom& lm() const { return mLm; }
  const Monom& ansector() const { return mAnsector; }
  bool isGB() const { return mAnsector.degree() == mLm.degree(); }
  int multi() const {
    int sum=0;
    for(int i=0; i < mLm.size(); i++)
      if (mNM[i])
        ++sum;
    return sum;
  }

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
  void setBuild(int var) const {
    assert(0 <= var && var < mLm.size());
    mBuild[var] = true;
  }

  friend std::ostream& operator<<(std::ostream& out, const Wrap &a);
};

typedef List<Wrap*> ListWrap;

}

#endif // GINV_WRAP_H
