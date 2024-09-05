cmake_minimum_required(VERSION 3.10)
project(CrossPlatformFileTransfer)

set(CMAKE_CXX_STANDARD 17)

# find Boost
find_package(Boost REQUIRED COMPONENTS system thread filesystem)

# if Boost is not found, hardcode the paths manually
if(NOT Boost_FOUND)
    message(WARNING "Boost libraries not found! Falling back to hardcoded paths.")
    
    # Hardcoded Boost paths (change these paths to match your installation)
    set(BOOST_ROOT "D:/boost")
    set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
    set(BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib")

    # Manually set Boost include and library directories
    include_directories(${BOOST_INCLUDEDIR})
    link_directories(${BOOST_LIBRARYDIR})
else()
    # if Boost is found dynamically, print a success message
    message(STATUS "Boost libraries found.")
endif()

# if Boost is found, configure the project
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})

    # server executable
    add_executable(server server/main.cpp)
    target_link_libraries(server ${Boost_LIBRARIES})

    # client executable
    add_executable(client client/main.cpp)
    target_link_libraries(client ${Boost_LIBRARIES})
else()
    message(FATAL_ERROR "Boost libraries not found!")
endif()
