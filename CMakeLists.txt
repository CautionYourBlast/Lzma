#
#   Copyright (c) 2015 - 2017 Kulykov Oleh <info@resident.name>
#
#   Permission is hereby granted, free of charge, to any person obtaining a copy
#   of this software and associated documentation files (the "Software"), to deal
#   in the Software without restriction, including without limitation the rights
#   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#   copies of the Software, and to permit persons to whom the Software is
#   furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included in
#   all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#   THE SOFTWARE.
#

cmake_minimum_required(VERSION 2.8)


project(cliblzma)


set(PACKAGE "cliblzma")
set(CPACK_PACKAGE_NAME "${PACKAGE}")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
set(CPACK_PACKAGE_VENDOR "info@resident.name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PACKAGE} ${PACKAGE_VERSION}")
set(SOVERSION "0.0.1")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(VERSION "${CPACK_PACKAGE_VERSION}")


include(CheckLibraryExists)
include(CheckFunctionExists)

include(CheckIncludeFile)
include(CheckIncludeFileCXX)

include(CheckTypeSize)
include(CheckSymbolExists)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

# COMMAND LINE OPTIONS
option(CLZMA_OPT_SHARED "Build shared lib" ON)
option(CLZMA_OPT_STATIC "Build static lib" ON)
option(CLZMA_OPT_TESTS "Build clzma tests" OFF)

# C with -fPIC
check_c_compiler_flag("-fPIC" WITH_FPIC_C)
if(WITH_FPIC_C)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif(WITH_FPIC_C)


# CXX with -fPIC
check_cxx_compiler_flag("-fPIC" WITH_FPIC_CXX)
if(WITH_FPIC_CXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif(WITH_FPIC_CXX)


if(WIN32)
	add_definitions(-DWIN32)
	add_definitions(-D_WIN32)
	set(RWS_OS_WINDOWS 1)
endif(WIN32)


if(MINGW)
	set(CLZMA_COMPILER_MINGW 1)
endif(MINGW)


add_definitions(-DCMAKE_BUILD)

check_include_file("pthread.h" CLZMA_HAVE_PTHREAD_H)
#check_include_file("iconv.h" CLZMA_HAVE_ICONV_H)

IF (ICONV_INCLUDE_DIR AND ICONV_LIBRARIES)
  # Already in cache, be silent
  SET(ICONV_FIND_QUIETLY TRUE)
ENDIF (ICONV_INCLUDE_DIR AND ICONV_LIBRARIES)

FIND_PATH(ICONV_INCLUDE_DIR iconv.h) 
 
FIND_LIBRARY(ICONV_LIBRARIES NAMES iconv libiconv libiconv-2 c)
 
IF(ICONV_INCLUDE_DIR AND ICONV_LIBRARIES) 
   SET(ICONV_FOUND TRUE) 
ENDIF(ICONV_INCLUDE_DIR AND ICONV_LIBRARIES) 

set(CMAKE_REQUIRED_INCLUDES ${ICONV_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${ICONV_LIBRARIES})
IF(ICONV_FOUND)
  check_cxx_source_compiles("
  #include <iconv.h>
  int main(){
    iconv_t conv = 0;
    const char* in = 0;
    size_t ilen = 0;
    char* out = 0;
    size_t olen = 0;
    iconv(conv, &in, &ilen, &out, &olen);
    return 0;
  }
" ICONV_SECOND_ARGUMENT_IS_CONST )
ENDIF(ICONV_FOUND)
set(CMAKE_REQUIRED_INCLUDES)
set(CMAKE_REQUIRED_LIBRARIES)

IF(ICONV_FOUND) 
  IF(NOT ICONV_FIND_QUIETLY)
    MESSAGE(STATUS "Found Iconv include dir: ${ICONV_INCLUDE_DIR}") 
    MESSAGE(STATUS "Found Iconv libraries: ${ICONV_LIBRARIES}") 
  ENDIF(NOT ICONV_FIND_QUIETLY) 
ELSE(ICONV_FOUND) 
  IF(Iconv_FIND_REQUIRED) 
    MESSAGE(FATAL_ERROR "Could not find Iconv") 
  ENDIF(Iconv_FIND_REQUIRED) 
ENDIF(ICONV_FOUND)

MARK_AS_ADVANCED(
  ICONV_INCLUDE_DIR
  ICONV_LIBRARIES
  ICONV_SECOND_ARGUMENT_IS_CONST
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
	set(CMAKE_INSTALL_LIBDIR lib)
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)

# Now make sure that you the the build directory on your "Include" path when compiling
include_directories(${PROJECT_BINARY_DIR})

set(LIBCLZMA_SOURCES 
		Sources/CLzma/C/7zAlloc.c
		Sources/CLzma/C/7zArcIn.c
		Sources/CLzma/C/7zBuf.c
		Sources/CLzma/C/7zBuf2.c
		Sources/CLzma/C/7zCrc.c
		Sources/CLzma/C/7zCrcOpt.c
		Sources/CLzma/C/7zDec.c
		Sources/CLzma/C/7zFile.c
		Sources/CLzma/C/7zStream.c
		Sources/CLzma/C/Aes.c
		Sources/CLzma/C/AesOpt.c
		Sources/CLzma/C/Alloc.c
		Sources/CLzma/C/Bcj2.c
		Sources/CLzma/C/Bcj2Enc.c
		Sources/CLzma/C/Bra.c
		Sources/CLzma/C/Bra86.c
		Sources/CLzma/C/BraIA64.c
		Sources/CLzma/C/CpuArch.c
		Sources/CLzma/C/Delta.c
		Sources/CLzma/C/LzFind.c
		Sources/CLzma/C/LzFindMt.c
		Sources/CLzma/C/Lzma2Dec.c
		Sources/CLzma/C/Lzma2Enc.c
		Sources/CLzma/C/Lzma86Dec.c
		Sources/CLzma/C/Lzma86Enc.c
		Sources/CLzma/C/LzmaDec.c
		Sources/CLzma/C/LzmaEnc.c
		Sources/CLzma/C/LzmaLib.c
		Sources/CLzma/C/MtCoder.c
		Sources/CLzma/C/Ppmd7.c
		Sources/CLzma/C/Ppmd7Dec.c
		Sources/CLzma/C/Ppmd7Enc.c
		Sources/CLzma/C/Sha256.c
		Sources/CLzma/C/Sort.c
		Sources/CLzma/C/Threads.c
		Sources/CLzma/C/Xz.c
		Sources/CLzma/C/XzCrc64.c
		Sources/CLzma/C/XzCrc64Opt.c
		Sources/CLzma/C/XzDec.c
		Sources/CLzma/C/XzEnc.c
		Sources/CLzma/C/XzIn.c
		Sources/CLzma/CPP/7zip/Archive/7z/7zCompressionMode.cpp
		Sources/CLzma/CPP/7zip/Archive/7z/7zDecode.cpp
		Sources/CLzma/CPP/7zip/Archive/7z/7zEncode.cpp
		Sources/CLzma/CPP/7zip/Archive/7z/7zExtract.cpp
		Sources/CLzma/CPP/7zip/Archive/7z/7zFolderInStream.cpp
		Sources/CLzma/CPP/7zip/Archive/7z/7zHandler.cpp
		Sources/CLzma/CPP/7zip/Archive/7z/7zHandlerOut.cpp
		Sources/CLzma/CPP/7zip/Archive/7z/7zHeader.cpp
		Sources/CLzma/CPP/7zip/Archive/7z/7zIn.cpp
		Sources/CLzma/CPP/7zip/Archive/7z/7zOut.cpp
		Sources/CLzma/CPP/7zip/Archive/7z/7zProperties.cpp
		Sources/CLzma/CPP/7zip/Archive/7z/7zRegister.cpp
		Sources/CLzma/CPP/7zip/Archive/7z/7zSpecStream.cpp
		Sources/CLzma/CPP/7zip/Archive/7z/7zUpdate.cpp
		Sources/CLzma/CPP/7zip/Archive/ArchiveExports.cpp
		Sources/CLzma/CPP/7zip/Archive/Common/CoderMixer2.cpp
		Sources/CLzma/CPP/7zip/Archive/Common/DummyOutStream.cpp
		Sources/CLzma/CPP/7zip/Archive/Common/HandlerOut.cpp
		Sources/CLzma/CPP/7zip/Archive/Common/InStreamWithCRC.cpp
		Sources/CLzma/CPP/7zip/Archive/Common/ItemNameUtils.cpp
		Sources/CLzma/CPP/7zip/Archive/Common/MultiStream.cpp
		Sources/CLzma/CPP/7zip/Archive/Common/OutStreamWithCRC.cpp
		Sources/CLzma/CPP/7zip/Archive/Common/ParseProperties.cpp
		Sources/CLzma/CPP/7zip/Archive/DllExports2.cpp
		Sources/CLzma/CPP/7zip/Archive/LzmaHandler.cpp
		Sources/CLzma/CPP/7zip/Archive/SplitHandler.cpp
		Sources/CLzma/CPP/7zip/Common/CreateCoder.cpp
		Sources/CLzma/CPP/7zip/Common/CWrappers.cpp
		Sources/CLzma/CPP/7zip/Common/FileStreams.cpp
		Sources/CLzma/CPP/7zip/Common/FilterCoder.cpp
		Sources/CLzma/CPP/7zip/Common/InBuffer.cpp
		Sources/CLzma/CPP/7zip/Common/InOutTempBuffer.cpp
		Sources/CLzma/CPP/7zip/Common/LimitedStreams.cpp
		Sources/CLzma/CPP/7zip/Common/LockedStream.cpp
		Sources/CLzma/CPP/7zip/Common/MethodId.cpp
		Sources/CLzma/CPP/7zip/Common/MethodProps.cpp
		Sources/CLzma/CPP/7zip/Common/OffsetStream.cpp
		Sources/CLzma/CPP/7zip/Common/OutBuffer.cpp
		Sources/CLzma/CPP/7zip/Common/ProgressUtils.cpp
		Sources/CLzma/CPP/7zip/Common/PropId.cpp
		Sources/CLzma/CPP/7zip/Common/StreamBinder.cpp
		Sources/CLzma/CPP/7zip/Common/StreamObjects.cpp
		Sources/CLzma/CPP/7zip/Common/StreamUtils.cpp
		Sources/CLzma/CPP/7zip/Common/UniqBlocks.cpp
		Sources/CLzma/CPP/7zip/Common/VirtThread.cpp
		Sources/CLzma/CPP/7zip/Compress/Bcj2Coder.cpp
		Sources/CLzma/CPP/7zip/Compress/Bcj2Register.cpp
		Sources/CLzma/CPP/7zip/Compress/BcjCoder.cpp
		Sources/CLzma/CPP/7zip/Compress/BcjRegister.cpp
		Sources/CLzma/CPP/7zip/Compress/BranchMisc.cpp
		Sources/CLzma/CPP/7zip/Compress/BranchRegister.cpp
		Sources/CLzma/CPP/7zip/Compress/ByteSwap.cpp
		Sources/CLzma/CPP/7zip/Compress/CodecExports.cpp
		Sources/CLzma/CPP/7zip/Compress/CopyCoder.cpp
		Sources/CLzma/CPP/7zip/Compress/CopyRegister.cpp
		Sources/CLzma/CPP/7zip/Compress/DeltaFilter.cpp
		Sources/CLzma/CPP/7zip/Compress/Lzma2Decoder.cpp
		Sources/CLzma/CPP/7zip/Compress/Lzma2Encoder.cpp
		Sources/CLzma/CPP/7zip/Compress/Lzma2Register.cpp
		Sources/CLzma/CPP/7zip/Compress/LzmaDecoder.cpp
		Sources/CLzma/CPP/7zip/Compress/LzmaEncoder.cpp
		Sources/CLzma/CPP/7zip/Compress/LzmaRegister.cpp
		Sources/CLzma/CPP/7zip/Crypto/7zAes.cpp
		Sources/CLzma/CPP/7zip/Crypto/7zAesRegister.cpp
		Sources/CLzma/CPP/7zip/Crypto/MyAes.cpp
		Sources/CLzma/CPP/7zip/Crypto/MyAesReg.cpp
		Sources/CLzma/CPP/7zip/Crypto/RandGen.cpp
		Sources/CLzma/CPP/Common/C_FileIO.cpp
		Sources/CLzma/CPP/Common/CommandLineParser.cpp
		Sources/CLzma/CPP/Common/CRC.cpp
		Sources/CLzma/CPP/Common/CrcReg.cpp
		Sources/CLzma/CPP/Common/IntToString.cpp
		Sources/CLzma/CPP/Common/MyString.cpp
		Sources/CLzma/CPP/Common/MyVector.cpp
		Sources/CLzma/CPP/Common/MyWindows.cpp
		Sources/CLzma/CPP/Common/NewHandler.cpp
		Sources/CLzma/CPP/Common/Sha256Reg.cpp
		Sources/CLzma/CPP/Common/StringConvert.cpp
		Sources/CLzma/CPP/Common/StringToInt.cpp
		Sources/CLzma/CPP/Common/TextConfig.cpp
		Sources/CLzma/CPP/Common/UTFConvert.cpp
		Sources/CLzma/CPP/Common/Wildcard.cpp
		Sources/CLzma/CPP/Common/XzCrc64Reg.cpp
		Sources/CLzma/CPP/Windows/PropVariant.cpp
		Sources/CLzma/CPP/Windows/PropVariantConv.cpp
		Sources/CLzma/clzma_base_coder.cpp
		Sources/CLzma/clzma_common.cpp
		Sources/CLzma/clzma_error.cpp
		Sources/CLzma/clzma_extern.cpp
		Sources/CLzma/clzma_extract_callback.cpp
		Sources/CLzma/clzma_file_decoder.cpp
		Sources/CLzma/clzma_file_encoder.cpp
		Sources/CLzma/clzma_in_file.cpp
		Sources/CLzma/clzma_item.c
		Sources/CLzma/clzma_memory.c
		Sources/CLzma/clzma_mutable_item.c
		Sources/CLzma/clzma_open_callback.cpp
		Sources/CLzma/clzma_out_file.cpp
		Sources/CLzma/clzma_path.cpp
		Sources/CLzma/clzma_reader.cpp
		Sources/CLzma/clzma_string.cpp
		Sources/CLzma/clzma_update_callback.cpp
		Sources/CLzma/clzma_writer.cpp
		)
				

set(LIBCLZMA_HEADERS Sources/CLzma/libclzma.h)


add_definitions(-DCLZMA_BUILD)


if(CLZMA_OPT_SHARED)
	add_library(clzma SHARED ${LIBCLZMA_SOURCES} ${LIBCLZMA_HEADERS})
	if(MSVC)
    	# msvc does not append 'lib' - do it here to have consistent name
	    set_property(TARGET clzma PROPERTY PREFIX "lib")
		set_property(TARGET clzma PROPERTY IMPORT_PREFIX "lib")
	endif(MSVC)
endif(CLZMA_OPT_SHARED)

if(CLZMA_OPT_STATIC)
	add_library(clzma_static STATIC ${LIBCLZMA_SOURCES} ${LIBCLZMA_HEADERS})
	set_property(TARGET clzma_static APPEND PROPERTY COMPILE_FLAGS -DLIBCLZMA_STATIC)
	if(MSVC)
    	# msvc does not append 'lib' - do it here to have consistent name
	    set_target_properties(clzma_static PROPERTIES PREFIX "lib")
	endif(MSVC)
endif(CLZMA_OPT_STATIC)


if(CLZMA_HAVE_PTHREAD_H)
	target_link_libraries(clzma pthread)
endif(CLZMA_HAVE_PTHREAD_H)

if(CLZMA_HAVE_ICONV_H)
	target_link_libraries(clzma ${ICONV_LIBRARIES})
endif(CLZMA_HAVE_ICONV_H)

install(TARGETS clzma
		DESTINATION lib)

install(TARGETS clzma_static
		DESTINATION lib)

install(FILES Sources/CLzma/libclzma.h
		DESTINATION include)


if(CLZMA_OPT_TESTS)
	enable_testing()
	add_subdirectory(Tests/clzma)

	# This must always be last!
	include(CPack)
endif(CLZMA_OPT_TESTS)

