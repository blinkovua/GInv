/***************************************************************************
 *   Copyright (C) 2008 by Blinkov Yu. A.                                  *
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

#ifndef GINV_UTIL_TIMER_H
#define GINV_UTIL_TIMER_H

#include <iostream>

#include "config.h"

namespace GInv {

class Timer {
  static double sHZ;

  double mUserTime;
  double mSysTime;
  double mRealTime;

  double mUserElapsed;
  double mSysElapsed;
  double mRealElapsed;

public:
  Timer():
      mUserTime(0.0),
      mSysTime(0.0),
      mRealTime(0.0),
      mUserElapsed(0.0),
      mSysElapsed(0.0),
      mRealElapsed(0.0) {
  }

  void start();
  void cont();
  void stop();

  double userTime() const { return mUserElapsed; }
  double sysTime() const { return mSysElapsed; }
  double realTime() const { return mRealElapsed; }

  friend std::ostream& operator<<(std::ostream& out, const Timer &a);
};

}

#endif // GINV_UTIL_TIMER_H
