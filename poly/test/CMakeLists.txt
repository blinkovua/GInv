INCLUDE_DIRECTORIES(../../)

ADD_EXECUTABLE(unittest
  unittest.cpp
  integer.cpp
  monom.cpp
)

TARGET_LINK_LIBRARIES(unittest
  ginvpoly
  ginvutil
  gmp
  cppunit
)
