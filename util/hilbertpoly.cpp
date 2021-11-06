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

#include "allocator.h"
#include "hilbertpoly.h"

namespace GInv {

size_t HilbertPoly::sBufferSize=4096*16;
char*  HilbertPoly::sBuffer=new char[HilbertPoly::sBufferSize];

const char* HilbertPoly::operator[](int k) const {
  assert(0 <= k && k <= mDim);
  int base=10;
  if (mpq_numref(mCoeff + k)->_mp_size == 0) {
    sBuffer[0] = '0';
    sBuffer[1] = '\0';
  }
  else {
    size_t size=mpz_sizeinbase(mpq_numref(mCoeff + k), base) +
        mpz_sizeinbase(mpq_denref(mCoeff + k), base) + 3;
    if (size > sBufferSize) {
      delete[] sBuffer;
      sBufferSize = (size*15/10/4096 + 1)*4096;
      sBuffer=new char[sBufferSize];
    }
    assert(size <= sBufferSize);
    char *res=mpq_get_str(sBuffer, base, mCoeff + k);
    assert(sBuffer == res);
  }
  return sBuffer;  
}
  
void HilbertPoly::add(const HilbertPoly& a) {
  for(int k = 0; k < mDim; k++)
    mpq_add(mCoeff + k, mCoeff + k, a.mCoeff + k);
}

void HilbertPoly::sub(const HilbertPoly& a) {
  for(int k = 0; k < mDim; k++)
    mpq_sub(mCoeff + k, mCoeff + k, a.mCoeff + k);
}

void HilbertPoly::mult(int a) {
  __mpq_struct b;
  mpq_init(&b);
  mpq_set_si(&b, (long)a, 1ul);
  for(int k = 0; k < mDim; k++) {
    mpq_mul(mCoeff + k, mCoeff + k, &b);
  }
  mpq_clear(&b);
}

void HilbertPoly::binomial(long int a, unsigned long b) {
  int k;
  for(k = 0; k < mDim; k++)
    mpq_set_ui(mCoeff + k, 0ul, 1ul);

  if (b == 0)
    mpq_set_ui(mCoeff + 0, 1ul, 1ul);
  else {
    __mpq_struct *tmp = new __mpq_struct[b];
    for(k = 0; k < b; k++)
      mpq_init(tmp + k);

    mpq_set_si(mCoeff + 0, a, 1ul);
    mpq_set_ui(mCoeff + 1, 1ul, 1ul);

    __mpq_struct q;
    mpq_init(&q);

    for(unsigned long p = 2; p <= b; p++) {
      int i;
      mpq_set_ui(&q, 1ul, p);
      for(i = 0; i < p; i++) {
        mpq_set_ui(&q, 1ul, p);
        mpq_set(tmp + i, mCoeff + i);
        mpq_mul(tmp + i, tmp + i, &q);
      }

      mpq_set_si(&q, a-(long int)(p-1), p);

      for(i = 0; i < p; i++)
        mpq_mul(mCoeff + i, mCoeff + i, &q);

      for(i = 1; i <= p; i++)
        mpq_add(mCoeff + i, mCoeff + i, tmp + i-1);
    }
    for(k = 0; k < mDim; k++)
      mpq_canonicalize(mCoeff + k);

    for(k = 0; k < b; k++)
      mpq_clear(tmp + k);

    delete[] tmp;
  }
}

std::ostream& operator<<(std::ostream& out, const HilbertPoly &a) {
  bool sep=false;
  for(int i=a.dim(); i >= 0; i--)
    if (!a.isZero(i)) {
      const char* s=a[i];
      if (!sep)
        out << s;
      else if (s[0] == '-')
        out << " - " << s+1;
      else
        out << " + " << s;
      if (i)
        out << "s^" << i;
      sep=true;
    }
  return out;
}

}
