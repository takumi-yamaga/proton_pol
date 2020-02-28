#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(proton_pol)

#----------------------------------------------------------------------------
# Setup compile options
#
set(CMAKE_CSS_FLAGS_DEBUG "-g3 -O0")
set(CMAKE_CSS_FLAGS_RELEASE "-O2")

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include 
  ${Geant4_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(execute-proton_pol proton_pol.cc ${sources} ${headers})
target_link_libraries(execute-proton_pol ${Geant4_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory.
#
set(proton_pol_SCRIPTS
  icons.mac
  gui.mac
  init.mac 
  init_vis.mac 
  vis.mac
  run.mac 
  proton_pol.root
  )

foreach(_script ${proton_pol_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Copy all macros to the build directory.
#
set(proton_pol_MACRO
  Analysis.c 
  DrawToos.h 
  )

foreach(_script ${proton_pol_MACRO})
  configure_file(
    ${PROJECT_SOURCE_DIR}/macro/${_script}
    ${PROJECT_BINARY_DIR}/macro/${_script}
    COPYONLY
    )
endforeach()
set(proton_pol_FIG
  analysis.pdf 
  )

foreach(_script ${proton_pol_FIG})
  configure_file(
    ${PROJECT_SOURCE_DIR}/fig/${_script}
    ${PROJECT_BINARY_DIR}/fig/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Add program to the project targets
# (this avoids the need of typing the program name after make)
#
add_custom_target(proton_pol DEPENDS execute-proton_pol)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS execute-proton_pol DESTINATION bin)

