INCLUDE_DIRECTORIES(../../)

ADD_EXECUTABLE(unittest
  unittest.cpp
  integer.cpp
)

TARGET_LINK_LIBRARIES(unittest
  ginvinteger
  ginvutil
  gmp
  cppunit
)
