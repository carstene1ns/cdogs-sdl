# CMake Toolchain file for Wii homebrew ports based on SDL libraries
# written by carstene1ns 2014

# helper variables
set(DKPRO "$ENV{DEVKITPRO}")
set(DKPPC "$ENV{DEVKITPPC}")
set(WII 1)

# check environment
if(DKPRO STREQUAL "" OR DKPPC STREQUAL "")
  message(FATAL_ERROR
    "You need to setup DevkitPPC properly by exporting the required environment "
    "variables DEVKITPRO and DEVKITPPC! Then try again."
  )
endif()

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER ${DKPPC}/bin/powerpc-eabi-gcc)
set(CMAKE_CXX_COMPILER ${DKPPC}/bin/powerpc-eabi-g++)

# help FindXXX.cmake modules
set(CMAKE_FIND_ROOT_PATH ${DKPRO}/portlibs/ppc)
# this finds sdl, but not sdl_mixer:
#set(CMAKE_FIND_ROOT_PATH ${DKPRO}/portlibs/ppc ${DKPRO}/libogc)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# this seems to have no effect at all:
#set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${DKPRO}/libogc/include ${DKPRO}/portlibs/ppc/include)
#set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${DKPRO}/libogc/lib/wii ${DKPRO}/portlibs/ppc/lib)

# help pkg-config
set($ENV{PKG_CONFIG_LIBDIR} $DEVKITPRO/portlibs/ppc/lib/pkgconfig)

# sdl, sdl_mixer, sdl_image and  can not be found otherwise
set(SDL_INCLUDE_DIR ${DKPRO}/libogc/include/SDL)
set(SDL_LIBRARY ${DKPRO}/libogc/lib/wii/libSDL.a)
set(SDL_MIXER_INCLUDE_DIR ${DKPRO}/libogc/include/SDL)
set(SDL_MIXER_LIBRARY ${DKPRO}/libogc/lib/wii/libSDL_mixer.a)
set(SDL_IMAGE_INCLUDE_DIR ${DKPRO}/libogc/include/SDL)
set(SDL_IMAGE_LIBRARY ${DKPRO}/libogc/lib/wii/libSDL_image.a)

# this is a quirk, because cmake's FindFreetype module is broken
set(ENV{FREETYPE_DIR} ${DKPRO}/portlibs/ppc)

# additional flags (MACHDEP)
set(WII_COMMON_FLAGS "-mrvl -mcpu=750 -meabi -mhard-float")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe -O2 ${WII_COMMON_FLAGS} " CACHE STRING "DevkitPPC CFLAGS" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -O2 ${WII_COMMON_FLAGS} " CACHE STRING "DevkitPPC CXXFLAGS" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${WII_COMMON_FLAGS} -Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/wii-boot-elf.map" FORCE)

# additional defines
add_definitions(-DWORDS_BIGENDIAN=1 -DGEKKO)

# additional include directories
include_directories("${DKPRO}/libogc/include")

# additional libraries (this is specific to sdl based ports and
# needs to be adapted and included in the main CMakeLists.txt)
set(WII_TOOLCHAIN_LIBRARIES
	${DKPRO}/libogc/lib/wii/libmad.a
	${DKPRO}/portlibs/ppc/lib/libvorbisidec.a
	${DKPRO}/libogc/lib/wii/libfat.a
	${DKPRO}/libogc/lib/wii/libwiikeyboard.a
	${DKPRO}/libogc/lib/wii/libwiiuse.a
	${DKPRO}/libogc/lib/wii/libbte.a
	${DKPRO}/libogc/lib/wii/libogc.a
)
