/***************************************************************************
 *   Copyright (C) 2021 by Blinkov Yu. A.                                  *
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

#ifndef GINV_UTIL_HILBERTPOLY_H
#define GINV_UTIL_HILBERTPOLY_H

#include <cassert>

#include <gmp.h>

#include "config.h"

namespace GInv {

class HilbertPoly {
  static size_t buffer_size;
  static char*  buffer;

  int              mDim;
  __mpq_struct*    mCoeff;

public:
  HilbertPoly(int dim):
      mDim(dim+1),
      mCoeff(new __mpq_struct[mDim]) {
    for(int k = 0; k < mDim; k++)
      mpq_init(mCoeff + k);
  }
  ~HilbertPoly() {
    for(int k = 0; k < mDim; k++)
      mpq_clear(mCoeff + k);
    delete[] mCoeff;
  }

  int dim() const { return mDim-1; }
  const __mpq_struct* operator[](int k) const {
    assert(0 <= k && k <= mDim);
    return mCoeff + k;
  }
  const char* numer(int k) const;
  const char* denom(int k) const;

  void add(const HilbertPoly& a);
  void sub(const HilbertPoly& a);
  void mult(int a);
  void binomial(long int a, unsigned long b);
};

}

#endif // GINV_UTIL_HILBERTPOLY_H
