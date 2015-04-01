cmake_minimum_required(VERSION 2.6)
project(fakefeed C)

# Create the binary directory for makefile output and copy over
# a test q script.
file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
file(COPY "src/run.sh" DESTINATION "${CMAKE_SOURCE_DIR}/bin")
file(COPY "src/run.q" DESTINATION "${CMAKE_SOURCE_DIR}/bin")

set(FEED_SOURCES "src/fakefeed.c")
set(PROJ_SOURCES "src/main.c")

# Set some operating system dependent flags to be passed to the
# compilers.
if(UNIX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -fpic -std=gnu99")
elseif(WIN32)
    set(LINK_LIBS "${CMAKE_SOURCE_DIR}/src/w32/q.lib" "ws2_32.lib")
    set(PROJ_SOURCES ${PROJ_SOURCES} "src/w32/exports.def")
    set(FEED_SOURCES ${FEED_SOURCES} "src/w32/fakefeed.def")
endif()

LINK_DIRECTORIES(${CMAKE_BINARY_DIR})

# Define how the library should be linked to the other objects.
add_library(fakefeed SHARED ${FEED_SOURCES})
set_target_properties(fakefeed PROPERTIES PREFIX "")

add_library(feedhandler SHARED ${PROJ_SOURCES})
target_link_libraries(feedhandler fakefeed ${LINK_LIBS})
set_target_properties(feedhandler PROPERTIES PREFIX "")

# Move the finished binaries into the binary folder if installing.
INSTALL(TARGETS feedhandler fakefeed DESTINATION "${CMAKE_SOURCE_DIR}/bin")
