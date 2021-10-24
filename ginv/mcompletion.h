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

#ifndef GINV_MCOMPLETION_H
#define GINV_MCOMPLETION_H

#include "util/allocator.h"
#include "util/list.h"
#include "wrap.h"
#include "janet.h"
#include "gb.h"

#include "config.h"

namespace GInv {

class MCompletion {
  Allocator*  mAllocator;
  ListWrap    mQ;
  ListWrap    mT;
  int (*mCmp)(const Wrap&, const Wrap&);

  void insert(Wrap *w);
  void prolong();

public:
  MCompletion()=delete;
  MCompletion(const MCompletion& a)=delete;
  MCompletion(Allocator* allocator, int (*cmp)(const Wrap&, const Wrap&)):
      mAllocator(allocator),
      mQ(allocator),
      mT(allocator),
      mCmp(cmp) {
  }
  ~MCompletion();

  void push(const Monom& m) {
    insert(new(mAllocator) Wrap(m));
  }
  void build();

  Wrap* find(const Monom& m) const;
  const ListWrap& T() const { return mT; }
  int length() const { return mT.length(); }

  static int lex(const Wrap& a, const Wrap& b) {
    return a.lm().lex(b.lm());
  }
  static int deglex(const Wrap& a, const Wrap& b) {
    return a.lm().deglex(b.lm());
  }
  static int alex(const Wrap& a, const Wrap& b) {
    return a.lm().alex(b.lm());
  }
  static int lexGB(const Wrap& a, const Wrap& b) {
    int r=a.ansector().lex(b.ansector());
    if (r == 0)
      r=a.lm().lex(b.lm());
    return r;
  }
  static int deglexGB(const Wrap& a, const Wrap& b) {
    int r=a.ansector().deglex(b.ansector());
    if (r == 0)
      r=a.lm().deglex(b.lm());
    return r;
  }
  static int alexGB(const Wrap& a, const Wrap& b) {
    int r=a.ansector().alex(b.ansector());
    if (r == 0)
      r=a.lm().alex(b.lm());
    return r;
  }
  static int GB(const Wrap& a, const Wrap& b) {
    int r;
    if (a.ansector().degree() > b.ansector().degree())
      r = -1;
    else if (a.ansector().degree() < b.ansector().degree())
      r = 1;
    else
      r = 0;
    if (r == 0)
      r=a.lm().lex(b.lm());
    return r;
  }
};

class MJanet {
  Allocator*  mAllocator;
  ListWrap    mQ;
  Janet       mJanet;

  static void clear(Allocator* allocator, Janet::ConstIterator j);
  void prolong(Janet::ConstIterator j);

public:
  MJanet()=delete;
  MJanet(const MJanet& a)=delete;
  MJanet(Allocator* allocator):
      mAllocator(allocator),
      mQ(allocator),
      mJanet(allocator) {
  }
  ~MJanet();

  void push(const Monom& m) {
//     mQ.push(new(mAllocator) Wrap(mAllocator, m));
    mJanet.insert(new(mAllocator) Wrap(m));
  }
  void build();

  const Janet& tree() const { return mJanet; }
};

}

#endif // GINV_MCOMPLETION_H
