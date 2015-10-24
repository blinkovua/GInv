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

#ifdef GINV_UTIL_WIN32
  #ifndef _WIN32_WINNT            // Allow use of features specific to Windows 2000 or later.
  #define _WIN32_WINNT 0x0500
  #endif
  #include <windows.h>
#else
  #include <unistd.h>
  #include <sys/times.h>
#endif // GINV_UTIL_WIN32

#include <iomanip>

#include "timer.h"

namespace GInv {

#ifdef GINV_UTIL_WIN32

double Timer::sHZ = 10000000.0;

void Timer::start() {
  mUserElapsed = 0.0;
  mSysElapsed = 0.0;
  mRealElapsed = 0.0;

  ULARGE_INTEGER cTime, eTime, kTime, uTime, rTime;

  ::GetProcessTimes(::GetCurrentProcess(), (LPFILETIME)&cTime, (LPFILETIME)&eTime, (LPFILETIME)&kTime, (LPFILETIME)&uTime);
  ::GetSystemTimeAsFileTime((LPFILETIME)&rTime);

  mUserTime = uTime.QuadPart/sHZ;
  mSysTime = kTime.QuadPart/sHZ;
  mRealTime = rTime.QuadPart/sHZ;
}

void Timer::cont() {
  ULARGE_INTEGER cTime, eTime, kTime, uTime, rTime;

  ::GetProcessTimes(::GetCurrentProcess(), (LPFILETIME)&cTime, (LPFILETIME)&eTime, (LPFILETIME)&kTime, (LPFILETIME)&uTime);
  ::GetSystemTimeAsFileTime((LPFILETIME)&rTime);

  mUserTime = uTime.QuadPart/sHZ;
  mSysTime = kTime.QuadPart/sHZ;
  mRealTime = rTime.QuadPart/sHZ;
}

void Timer::stop() {
  ULARGE_INTEGER cTime, eTime, kTime, uTime, rTime;

  ::GetProcessTimes(::GetCurrentProcess(), (LPFILETIME)&cTime, (LPFILETIME)&eTime, (LPFILETIME)&kTime, (LPFILETIME)&uTime);
  ::GetSystemTimeAsFileTime((LPFILETIME)&rTime);

  mUserElapsed += uTime.QuadPart/sHZ - mUserTime;
  mSysElapsed += kTime.QuadPart/sHZ - mSysTime;
  mRealElapsed += rTime.QuadPart/sHZ - mRealTime;

  mUserTime = uTime.QuadPart/sHZ;
  mSysTime = kTime.QuadPart/sHZ;
  mRealTime = rTime.QuadPart/sHZ;
}

#else

double Timer::sHZ = double(sysconf(_SC_CLK_TCK));

void Timer::start() {
  mUserElapsed = 0.0;
  mSysElapsed = 0.0;
  mRealElapsed = 0.0;

  tms buffer;
  mRealTime = times(&buffer)/sHZ;
  mUserTime = buffer.tms_utime/sHZ;
  mSysTime = buffer.tms_stime/sHZ;
}

void Timer::cont() {
  tms buffer;
  mRealTime = times(&buffer)/sHZ;
  mUserTime = buffer.tms_utime/sHZ;
  mSysTime = buffer.tms_stime/sHZ;
}

void Timer::stop() {
  tms buffer;
  double t = times(&buffer)/sHZ;

  mUserElapsed += buffer.tms_utime/sHZ - mUserTime;
  mSysElapsed += buffer.tms_stime/sHZ - mSysTime;
  mRealElapsed += t - mRealTime;

  mUserTime = buffer.tms_utime/sHZ;
  mSysTime = buffer.tms_stime/sHZ;
  mRealTime = t;
}

#endif // GINV_UTIL_WIN32

std::ostream& operator<<(std::ostream& out, const Timer &a) {
  std::ios::fmtflags flags = out.flags();
  out.flags(flags | std::ios::fixed);
  out << "  real time: " << std::setprecision(2) << a.realTime() << " sec" << std::endl;
  out << "  user time: " << std::setprecision(2) << a.userTime() << " sec" << std::endl;
  out << "system time: " << std::setprecision(2) << a.sysTime()  << " sec" << std::endl;
  out.flags(flags);
  return out;
}

}