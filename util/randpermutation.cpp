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

#include "randpermutation.h"

namespace GInv {

RandPermutation::RandPermutation(int n):
    mSize(n),
    mPerm(new int[mSize]),
    mDis(new std::uniform_int_distribution<int>*[mSize-1]) {
  assert(mSize > 0);
  for(int i=0; i < mSize; i++)
    mPerm[i] = i;
  for(int i=mSize-1; i > 0; i--)
    mDis[i-1] = new std::uniform_int_distribution<int>(0, i);
}

RandPermutation::~RandPermutation() {
  for(int i=mSize-1; i > 0; i--)
    delete mDis[i-1];
  delete[] mDis;
  delete[] mPerm;
}


std::ostream& operator<<(std::ostream& out, const RandPermutation &a) {
  out << '(' << a[0];
  for(int i=1; i < a.size(); i++)
    out << ' ' << a[i];
  return out << ')';
}

}
