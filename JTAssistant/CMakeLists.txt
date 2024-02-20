# This script configures project for JT Assistant application.
#

# =============================================================================
# Look for required dependencies
# =============================================================================

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(OpenCASCADE 	CONFIG 	REQUIRED)
find_package(TBB 			CONFIG 	REQUIRED)
find_package(ZLIB 					REQUIRED)
find_package(OpenGL 				REQUIRED)


mark_as_advanced(CLEAR ZLIB_LIBRARY)

if(WIN32)
  if(ZLIB_FOUND)
    get_filename_component(ZLIB_LIBRARY_DIR "${ZLIB_LIBRARY}" PATH)
    get_filename_component(ZLIB_LIBRARY_NAME "${ZLIB_LIBRARY}" NAME_WE)
    get_filename_component(ZLIB_ROOT_DIR "${ZLIB_LIBRARY_DIR}" PATH)
    find_program(ZLIB_DLL ${ZLIB_LIBRARY_NAME}.dll NAMES zlib1.dll PATHS "${ZLIB_ROOT_DIR}" "${ZLIB_ROOT_DIR}/bin" NO_DEFAULT_PATH)
  endif(ZLIB_FOUND)
endif(WIN32)

set (OPENGL_LIBRARIES ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})

get_target_property (TKJT_INCLUDE_DIRS TKJT INTERFACE_INCLUDE_DIRECTORIES)

# =============================================================================
# Define production steps
# =============================================================================

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/BVH
  ${CMAKE_CURRENT_SOURCE_DIR}/src/JTData
  ${CMAKE_CURRENT_SOURCE_DIR}/src/JTGui
  ${CMAKE_CURRENT_SOURCE_DIR}/src/JTVis
  ${OCE_INCLUDE_DIRS}
  ${ZLIB_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIR}
  ${TKJT_INCLUDE_DIRS}
)

file (GLOB_RECURSE ProjectSources src/*.cxx)
file (GLOB_RECURSE ProjectHeaders src/*.hxx)
file (GLOB_RECURSE ProjectQRC *.qrc)
file (GLOB_RECURSE ProjectUI src/*.ui)


if(WIN32)
  add_definitions (-DWNT -DWINVER=0x0500)
else()
  add_definitions (-fexceptions -fPIC -DOCC_CONVERT_SIGNALS)
endif()

# Suppression of 3rdparty warnings in MSVC with #pragma warnings (disable)
if(MSVC)
  add_definitions (/wd4718)
else()
  add_definitions (-Wno-unknown-pragmas)
endif()


if (WIN32)
  enable_language (RC)
  set (EXE_ICON res/icons/desktop/icon.rc)
endif()

source_group ("Header Files" FILES
  ${ProjectHeaders}
)

source_group ("Resources" FILES
  ${ProjectUI}
  ${ProjectQRC}
  ${EXE_ICON}
)



add_executable (JTAssistant WIN32 ${ProjectSources} ${ProjectHeaders} ${ProjectUI} ${ProjectQRC} ${EXE_ICON})

add_dependencies (JTAssistant TKJT)
target_include_directories (JTAssistant PUBLIC ${OpenCASCADE_INCLUDE_DIR} )

target_link_libraries(JTAssistant PRIVATE TKernel TKMath TKXSBase)
target_link_libraries(JTAssistant PRIVATE ZLIB::ZLIB)
target_link_libraries(JTAssistant PRIVATE TBB::tbb)
target_link_libraries(JTAssistant PRIVATE Eigen3::Eigen)
target_link_libraries(JTAssistant PRIVATE OpenGL::GL)



target_link_libraries (JTAssistant PRIVATE TKJT)
target_link_libraries (JTAssistant PRIVATE Qt5::Widgets)



#if (WIN32)
#  if (MSVC)
#    set_target_properties (JTAssistant PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#  endif (MSVC)
#endif (WIN32)

