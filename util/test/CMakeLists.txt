INCLUDE_DIRECTORIES(../../)

ADD_EXECUTABLE(unittest
  unittest.cpp
  allocator.cpp
  list.cpp
  rbtree.cpp
  randpermutation.cpp
  integer.cpp
)

TARGET_LINK_LIBRARIES(unittest
  ginvutil
  cppunit
)

IF(BUILD_GRAPHVIZ)
  ADD_EXECUTABLE(graphtest
    graph.cpp
  )
  TARGET_LINK_LIBRARIES(graphtest
    ginvutil
  )
ENDIF(BUILD_GRAPHVIZ)
