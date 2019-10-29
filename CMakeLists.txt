cmake_minimum_required(VERSION 2.8.4)
project(libcsd)
set(CMAKE_COMPILER_IS_GNUCXX)

# Set output folder
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(./)
include_directories(./utils/)
include_directories(./utils/Coder/)
include_directories(./RePair/)
include_directories(./RePair/Coder/)


set(SOURCES
        utils/Coder/StatCoder.cpp
        utils/Coder/DecodingTable.cpp
        utils/Coder/DecodingTree.cpp
        utils/Coder/DecodingTree.cpp
        utils/Coder/BinaryNode.cpp
        utils/VByte.cpp
        utils/LogSequence.cpp
        utils/DAC_VLS.cpp
        utils/DAC_BVLS.cpp
        RePair/Coder/arrayg.cpp
        RePair/Coder/basics.cpp
        RePair/Coder/hash.cpp
        RePair/Coder/heap.cpp
        RePair/Coder/records.cpp
        RePair/Coder/dictionary.cpp
        RePair/Coder/IRePair.cpp
        RePair/Coder/CRePair.cpp
        RePair/RePair.cpp
        StringDictionary.cpp
        StringDictionaryMMPFC_plain.cpp
        StringDictionaryMMPFC_plain_llcp.cpp
        StringDictionaryMMPFC_plain_rlcp.cpp
        StringDictionaryMMPFC_libcds_bSD.cpp
        StringDictionaryMMPFC_libcds_bSD_llcp.cpp
        StringDictionaryMMPFC_libcds_bSD_rlcp.cpp
        StringDictionaryMMRPFC_libcds_bSD.cpp
        StringDictionaryMMRPFC_libcds_bSD_llcp.cpp
        StringDictionaryMMRPFC_libcds_bSD_rlcp.cpp
        StringDictionaryMMRPFC_libcds_bSD_DAC.cpp
        StringDictionaryMMRPFC_libcds_bSD_DAC_llcp.cpp
        StringDictionaryMMRPFC_libcds_bSD_DAC_rlcp.cpp
        StringDictionaryMMRPFC_libcds_bSD_DAC2.cpp
        StringDictionaryMMRPFC_libcds_bSD_DAC2_llcp.cpp
        StringDictionaryMMRPFC_libcds_bSD_DAC2_rlcp.cpp
        StringDictionaryMMRPFC_libcds_DAC_VLS.cpp
        StringDictionaryMMRPFC_libcds_DAC_VLS_llcp.cpp
        StringDictionaryMMRPFC_libcds_DAC_VLS_rlcp.cpp
        StringDictionaryMMRPFC_libcds_DAC2_VLS.cpp
        StringDictionaryMMRPFC_libcds_DAC2_VLS_llcp.cpp
        StringDictionaryMMRPFC_libcds_DAC2_VLS_rlcp.cpp
        )

# Flags
set(CMAKE_CXX_FLAGS " -Wall -m64")
set(CMAKE_CXX_FLAGS_RELEASE " -std=c++0x -Wall -O9 -g3")
set(CMAKE_CXX_FLAGS_DEBUG " -std=c++0x -O9 -g3 ")
#set(CMAKE_CXX_FLAGS_DEBUG " -std=c++0x -g3 -DDEBUG")
#set(CMAKE_CXX_FLAGS_DEBUG " -std=c++0x -pg ")

# Flags
set(CMAKE_CXX_FLAGS " -std=c++11 -Wall -O9 -g3 -m64")
set(CMAKE_CXX_FLAGS_RELEASE " -std=c++11 -Wall -O9 -g3")
set(CMAKE_CXX_FLAGS_DEBUG " -std=c++11 -O9 -g3 ")
#set(CMAKE_CXX_FLAGS_DEBUG " -std=c++11 -g3 -DDEBUG")
#set(CMAKE_CXX_FLAGS_DEBUG " -std=c++11 -pg ")

# Create the library
add_library(CSD STATIC ${SOURCES})
install(TARGETS CSD DESTINATION lib)
install(DIRECTORY "includes" DESTINATION includes/libCSD)

#Add libraries
# Add libcds
set(LIBCDS_DIR "./libs/libcds2/")
include_directories(${LIBCDS_DIR})
include_directories(${LIBCDS_DIR}includes/)
include_directories(/usr/local/include/)
add_subdirectory(${LIBCDS_DIR})

# Executables
add_executable(TEST ./Test.cpp)
add_executable(BUILD ./Build.cpp)


# Executables -- tools
add_executable(FileToDict ./tools/fileToDict.cpp)

# Target libraries

#find_library(libsdsl sdsl)
#find_library(libdivsufsort divsufsort)
#find_library(libdivsufsort64 divsufsort64)

link_directories( /usr/local/lib )

target_link_libraries(TEST LINK_PUBLIC CSD cds)
target_link_libraries(BUILD LINK_PUBLIC CSD cds)

