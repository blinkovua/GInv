INCLUDE_DIRECTORIES(../../)

ADD_EXECUTABLE(unittest
  unittest.cpp
  monom.cpp
  poly_int.cpp
  janet.cpp
  gb.cpp
  mcompletion.cpp
)

TARGET_LINK_LIBRARIES(unittest
  ginv
  ginvutil
  gmp
  cppunit
)

IF(BUILD_GRAPHVIZ)
  ADD_EXECUTABLE(graphtest
    graph.cpp
  )
  TARGET_LINK_LIBRARIES(graphtest
    ginv
    ginvutil
  )
ENDIF(BUILD_GRAPHVIZ)

ADD_EXECUTABLE(stattest
  stattest.cpp
)

TARGET_LINK_LIBRARIES(stattest
  ginv
  ginvutil
)
