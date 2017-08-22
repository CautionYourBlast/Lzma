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


project(liblzma_wrp)


set(PACKAGE "liblzma_wrp")
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
option(LZMA_WRP_OPT_SHARED "Build shared lib" ON)
option(LZMA_WRP_OPT_STATIC "Build static lib" ON)
option(LZMA_WRP_OPT_TESTS "Build lzma_wrp tests" OFF)

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
	set(LZMA_WRP_COMPILER_MINGW 1)
endif(MINGW)


add_definitions(-DCMAKE_BUILD)

check_include_file("pthread.h" LZMA_WRP_HAVE_PTHREAD_H)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
	set(CMAKE_INSTALL_LIBDIR lib)
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)

# Now make sure that you the the build directory on your "Include" path when compiling
include_directories(${PROJECT_BINARY_DIR})

set(LIBLZMA_WRP_SOURCES 
		Sources/lzma_wrp/C/7zAlloc.c
		Sources/lzma_wrp/C/7zArcIn.c
		Sources/lzma_wrp/C/7zBuf.c
		Sources/lzma_wrp/C/7zBuf2.c
		Sources/lzma_wrp/C/7zCrc.c
		Sources/lzma_wrp/C/7zCrcOpt.c
		Sources/lzma_wrp/C/7zDec.c
		Sources/lzma_wrp/C/7zFile.c
		Sources/lzma_wrp/C/7zStream.c
		Sources/lzma_wrp/C/Aes.c
		Sources/lzma_wrp/C/AesOpt.c
		Sources/lzma_wrp/C/Alloc.c
		Sources/lzma_wrp/C/Bcj2.c
		Sources/lzma_wrp/C/Bcj2Enc.c
		Sources/lzma_wrp/C/Bra.c
		Sources/lzma_wrp/C/Bra86.c
		Sources/lzma_wrp/C/BraIA64.c
		Sources/lzma_wrp/C/CpuArch.c
		Sources/lzma_wrp/C/Delta.c
		Sources/lzma_wrp/C/LzFind.c
		Sources/lzma_wrp/C/LzFindMt.c
		Sources/lzma_wrp/C/Lzma2Dec.c
		Sources/lzma_wrp/C/Lzma2Enc.c
		Sources/lzma_wrp/C/Lzma86Dec.c
		Sources/lzma_wrp/C/Lzma86Enc.c
		Sources/lzma_wrp/C/LzmaDec.c
		Sources/lzma_wrp/C/LzmaEnc.c
		Sources/lzma_wrp/C/LzmaLib.c
		Sources/lzma_wrp/C/MtCoder.c
		Sources/lzma_wrp/C/Ppmd7.c
		Sources/lzma_wrp/C/Ppmd7Dec.c
		Sources/lzma_wrp/C/Ppmd7Enc.c
		Sources/lzma_wrp/C/Sha256.c
		Sources/lzma_wrp/C/Sort.c
		Sources/lzma_wrp/C/Threads.c
		Sources/lzma_wrp/C/Xz.c
		Sources/lzma_wrp/C/XzCrc64.c
		Sources/lzma_wrp/C/XzCrc64Opt.c
		Sources/lzma_wrp/C/XzDec.c
		Sources/lzma_wrp/C/XzEnc.c
		Sources/lzma_wrp/C/XzIn.c
		Sources/lzma_wrp/CPP/7zip/Archive/7z/7zCompressionMode.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/7z/7zDecode.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/7z/7zEncode.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/7z/7zExtract.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/7z/7zFolderInStream.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/7z/7zHandler.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/7z/7zHandlerOut.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/7z/7zHeader.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/7z/7zIn.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/7z/7zOut.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/7z/7zProperties.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/7z/7zRegister.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/7z/7zSpecStream.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/7z/7zUpdate.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/ArchiveExports.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/Common/CoderMixer2.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/Common/DummyOutStream.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/Common/HandlerOut.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/Common/InStreamWithCRC.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/Common/ItemNameUtils.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/Common/MultiStream.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/Common/OutStreamWithCRC.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/Common/ParseProperties.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/DllExports2.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/LzmaHandler.cpp
		Sources/lzma_wrp/CPP/7zip/Archive/SplitHandler.cpp
		Sources/lzma_wrp/CPP/7zip/Common/CreateCoder.cpp
		Sources/lzma_wrp/CPP/7zip/Common/CWrappers.cpp
		Sources/lzma_wrp/CPP/7zip/Common/FileStreams.cpp
		Sources/lzma_wrp/CPP/7zip/Common/FilterCoder.cpp
		Sources/lzma_wrp/CPP/7zip/Common/InBuffer.cpp
		Sources/lzma_wrp/CPP/7zip/Common/InOutTempBuffer.cpp
		Sources/lzma_wrp/CPP/7zip/Common/LimitedStreams.cpp
		Sources/lzma_wrp/CPP/7zip/Common/LockedStream.cpp
		Sources/lzma_wrp/CPP/7zip/Common/MethodId.cpp
		Sources/lzma_wrp/CPP/7zip/Common/MethodProps.cpp
		Sources/lzma_wrp/CPP/7zip/Common/OffsetStream.cpp
		Sources/lzma_wrp/CPP/7zip/Common/OutBuffer.cpp
		Sources/lzma_wrp/CPP/7zip/Common/ProgressUtils.cpp
		Sources/lzma_wrp/CPP/7zip/Common/PropId.cpp
		Sources/lzma_wrp/CPP/7zip/Common/StreamBinder.cpp
		Sources/lzma_wrp/CPP/7zip/Common/StreamObjects.cpp
		Sources/lzma_wrp/CPP/7zip/Common/StreamUtils.cpp
		Sources/lzma_wrp/CPP/7zip/Common/UniqBlocks.cpp
		Sources/lzma_wrp/CPP/7zip/Common/VirtThread.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/Bcj2Coder.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/Bcj2Register.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/BcjCoder.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/BcjRegister.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/BranchMisc.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/BranchRegister.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/ByteSwap.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/CodecExports.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/CopyCoder.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/CopyRegister.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/DeltaFilter.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/Lzma2Decoder.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/Lzma2Encoder.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/Lzma2Register.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/LzmaDecoder.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/LzmaEncoder.cpp
		Sources/lzma_wrp/CPP/7zip/Compress/LzmaRegister.cpp
		Sources/lzma_wrp/CPP/7zip/Crypto/7zAes.cpp
		Sources/lzma_wrp/CPP/7zip/Crypto/7zAesRegister.cpp
		Sources/lzma_wrp/CPP/7zip/Crypto/MyAes.cpp
		Sources/lzma_wrp/CPP/7zip/Crypto/MyAesReg.cpp
		Sources/lzma_wrp/CPP/7zip/Crypto/RandGen.cpp
		Sources/lzma_wrp/CPP/Common/C_FileIO.cpp
		Sources/lzma_wrp/CPP/Common/CommandLineParser.cpp
		Sources/lzma_wrp/CPP/Common/CRC.cpp
		Sources/lzma_wrp/CPP/Common/CrcReg.cpp
		Sources/lzma_wrp/CPP/Common/IntToString.cpp
		Sources/lzma_wrp/CPP/Common/MyString.cpp
		Sources/lzma_wrp/CPP/Common/MyVector.cpp
		Sources/lzma_wrp/CPP/Common/MyWindows.cpp
		Sources/lzma_wrp/CPP/Common/NewHandler.cpp
		Sources/lzma_wrp/CPP/Common/Sha256Reg.cpp
		Sources/lzma_wrp/CPP/Common/StringConvert.cpp
		Sources/lzma_wrp/CPP/Common/StringToInt.cpp
		Sources/lzma_wrp/CPP/Common/TextConfig.cpp
		Sources/lzma_wrp/CPP/Common/UTFConvert.cpp
		Sources/lzma_wrp/CPP/Common/Wildcard.cpp
		Sources/lzma_wrp/CPP/Common/XzCrc64Reg.cpp
		Sources/lzma_wrp/CPP/Windows/PropVariant.cpp
		Sources/lzma_wrp/CPP/Windows/PropVariantConv.cpp
		Sources/lzma_wrp/lzma_wrp_base_coder.cpp
		Sources/lzma_wrp/lzma_wrp_common.cpp
		Sources/lzma_wrp/lzma_wrp_error.cpp
		Sources/lzma_wrp/lzma_wrp_extern.cpp
		Sources/lzma_wrp/lzma_wrp_extract_callback.cpp
		Sources/lzma_wrp/lzma_wrp_file_decoder.cpp
		Sources/lzma_wrp/lzma_wrp_file_encoder.cpp
		Sources/lzma_wrp/lzma_wrp_in_file.cpp
		Sources/lzma_wrp/lzma_wrp_item.c
		Sources/lzma_wrp/lzma_wrp_memory.c
		Sources/lzma_wrp/lzma_wrp_mutable_item.c
		Sources/lzma_wrp/lzma_wrp_open_callback.cpp
		Sources/lzma_wrp/lzma_wrp_out_file.cpp
		Sources/lzma_wrp/lzma_wrp_path.cpp
		Sources/lzma_wrp/lzma_wrp_reader.cpp
		Sources/lzma_wrp/lzma_wrp_update_callback.cpp
		Sources/lzma_wrp/lzma_wrp_writer.cpp
		)
				

set(LIBLZMA_WRP_HEADERS Sources/lzma_wrp/lzma_wrp.h)


add_definitions(-DLZMA_WRP_BUILD)


if(LZMA_WRP_OPT_SHARED)
	add_library(lzma_wrp SHARED ${LIBLZMA_WRP_SOURCES} ${LIBLZMA_WRP_HEADERS})
	if(MSVC)
    	# msvc does not append 'lib' - do it here to have consistent name
	    set_property(TARGET lzma_wrp PROPERTY PREFIX "lib")
		set_property(TARGET lzma_wrp PROPERTY IMPORT_PREFIX "lib")
	endif(MSVC)
endif(LZMA_WRP_OPT_SHARED)

if(LZMA_WRP_OPT_STATIC)
	add_library(lzma_wrp_static STATIC ${LIBLZMA_WRP_SOURCES} ${LIBLZMA_WRP_HEADERS})
	set_property(TARGET lzma_wrp_static APPEND PROPERTY COMPILE_FLAGS -DLIBLZMA_WRP_STATIC)
	if(MSVC)
    	# msvc does not append 'lib' - do it here to have consistent name
	    set_target_properties(lzma_wrp_static PROPERTIES PREFIX "lib")
	endif(MSVC)
endif(LZMA_WRP_OPT_STATIC)


if(LZMA_WRP_HAVE_PTHREAD_H)
	target_link_libraries(lzma_wrp pthread)
endif(LZMA_WRP_HAVE_PTHREAD_H)

install(TARGETS lzma_wrp
		DESTINATION lib)

install(TARGETS lzma_wrp_static
		DESTINATION lib)

install(FILES Sources/lzma_wrp/lzma_wrp.h
		DESTINATION include)


if(LZMA_WRP_OPT_TESTS)
	enable_testing()
	add_subdirectory(Tests/lzma_wrp)

	# This must always be last!
	include(CPack)
endif(LZMA_WRP_OPT_TESTS)

