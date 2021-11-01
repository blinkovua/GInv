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

size_t HilbertPoly::buffer_size=4096*16;
char*  HilbertPoly::buffer=new char[HilbertPoly::buffer_size];

// const char* Integer::get_str(int base) const {
//   assert(2 <= base && base <= 62);
//   if (mMpz._mp_size == 0) {
//     buffer[0] = '0';
//     buffer[1] = '\0';
//   }
//   else {
//     size_t size= mpz_sizeinbase(&mMpz, base) + 2;
//     if (size > buffer_size) {
//       delete[] buffer;
//       buffer_size = (size*15/10/4096 + 1)*4096;
//       buffer=new char[buffer_size];
//     }
//     assert(size <= buffer_size);
//     char *res=mpz_get_str(buffer, base, &mMpz);
//     assert(buffer == res);
//   }
//   return buffer;
// }

}
