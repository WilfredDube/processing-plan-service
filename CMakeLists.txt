cmake_minimum_required(VERSION 3.0)

project(processing-plan-service_1.0)

link_libraries(stdc++fs)

add_subdirectory(logging)
add_subdirectory(libseqgen)
add_subdirectory(libfxtract)
add_subdirectory(msgqueue)
#add_subdirectory(tests)

set(main_source_files
 main.cpp
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

IF((CMAKE_CXX_STANDARD EQUAL 17) OR (CMAKE_CXX_STANDARD GREATER 17))
SET(WT_CPP17_ANY_DEFAULT_IMPLEMENTATION "std")
ENDIF()

set(THREADS_PREFER_PTHREAD_FLAG ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs -Wall -lm -Wl,-R/usr/local/lib")

find_package(amqpcpp REQUIRED)
find_package(Threads REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

# <------------ add hiredis dependency --------------->
find_path(HIREDIS_HEADER hiredis)
# target_include_directories(target PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
# target_link_libraries(target ${HIREDIS_LIB})

# <------------ add redis-plus-plus dependency -------------->
# NOTE: this should be *sw* NOT *redis++*
find_path(REDIS_PLUS_PLUS_HEADER sw)
# target_include_directories(target PUBLIC ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
# target_link_libraries(target ${REDIS_PLUS_PLUS_LIB})

add_executable (processing-plan-service ${main_source_files})
target_include_directories(processing-plan-service PRIVATE ${amqpcpp_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${REDIS_PLUS_PLUS_HEADER} ${HIREDIS_HEADER})
target_link_libraries(processing-plan-service  logger frelib seqgen msgqueue -g ${Boost_LIBRARIES} ${HIREDIS_LIB} ${REDIS_PLUS_PLUS_LIB} Threads::Threads -lboost_serialization ev boost_system dl Threads::Threads ${amqpcpp_LIBRARIES} amqpcpp nlohmann_json::nlohmann_json)

install(
  TARGETS processing-plan-service 
  DESTINATION bin
)


#find_package(Boost COMPONENTS system filesystem REQUIRED)
#find_package(Threads REQUIRED)

#add_executable (fxtract ${main_source_files})
#target_include_directories(fxtract PRIVATE ${Boost_INCLUDE_DIRS})
#target_link_libraries(fxtract logger frelib seqgen -g ${Boost_LIBRARIES} Threads::Threads -lboost_serialization)
