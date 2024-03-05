cmake_minimum_required(VERSION 3.9)
project(TCLib_Cmake_Project)

#========== Global Configurations =============#
#----------------------------------------------#

set(CMAKE_CXX_STANDARD 17)     
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(UNIX)
  # Add file textension *.bin to all executables on Unix (Linux, OSX, BSD ...)
  set(CMAKE_EXECUTABLE_SUFFIX ".bin")
endif()

# ------------ Download CmakeRC Resource compiler ----------------#

file(DOWNLOAD "https://raw.githubusercontent.com/vector-of-bool/cmrc/master/CMakeRC.cmake"
                 "${CMAKE_BINARY_DIR}/CMakeRC.cmake")
include("${CMAKE_BINARY_DIR}/CMakeRC.cmake")

#========== Targets Configurations ============#

cmrc_add_resource_library(
  TCLib-resources

    ALIAS TCLib::rc
    NAMESPACE TCLib

    resources/dark-theme.ini
    resources/light-theme.ini
    resources/LTspice.jpg
    resources/winrar_music.mp3
    )

add_executable(TCLib TCLib.cpp setup-funcs.cpp)
target_link_libraries(TCLib TCLib::rc)