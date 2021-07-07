set(Headers
  include/BendSequenceGenerator.h
  include/ProductionTime.h
  include/GA.h
)

set(Sources
  src/BendSequenceGenerator.cpp
  src/ProductionTime.cpp
  src/GA.cpp
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

IF((CMAKE_CXX_STANDARD EQUAL 17) OR (CMAKE_CXX_STANDARD GREATER 17))
SET(WT_CPP17_ANY_DEFAULT_IMPLEMENTATION "std")
ENDIF()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs")
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(wt COMPONENTS wt wtdbo wtdbosqlite3 wthttp REQUIRED)

add_library(seqgen ${Sources} ${Headers})
target_include_directories(seqgen PRIVATE ${Boost_INCLUDE_DIRS} ${wt_INCLUDE_DIRS} ${OpenCASCADE_INCLUDE_DIR})
target_link_libraries(seqgen frelib ${Boost_LIBRARIES} Threads::Threads ${wt_LIBRARIES} stdc++fs wthttp wt wtdbo wtdbosqlite3)
