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

#include <cstdlib>
#include <graphviz/gvc.h>

#include "util/rbtree.h"

inline int compare(int a, int b) {
  if (a > b)
    return 1;
  else if (a < b)
    return -1;
  else
    return 0;
}

using namespace GInv;

int main(int argc, char *argv[]) {
  Allocator allocator[1];
  RBTree<int, compare> a(allocator);

//   int data[]={1, 6, 8, 11, 13, 15, 17, 22, 25, 27};
  int data[]={13, 8, 17, 1, 11, 15, 25, 6, 22, 27};
//   int data[]={27, 25, 22, 17, 15, 13, 11, 8, 6, 1};
  for(int i=0; i < 10; i++)
    a.insert(data[i]);

//   for(int i=0; i < 100; i++)
//     a.insert((3*i+7)%101);

  a.draw("pdf", "rbtree.pdf");

  return EXIT_SUCCESS;
}
