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

IF(BUILD_GRAPHVIZ)
  ADD_EXECUTABLE(graphtest
    graph.cpp
  )
  TARGET_LINK_LIBRARIES(graphtest
    ginvpoly
    ginvutil
  )
ENDIF(BUILD_GRAPHVIZ)
