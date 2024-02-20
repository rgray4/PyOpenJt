

cmake_minimum_required(VERSION 3.14.0)

project (PyOpenJt)

# =============================================================================
# Configure CMake
# =============================================================================

set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
  ${CMAKE_MODULE_PATH}
)

#include (DefineMacro)
include (DefineCxxFlags)



# =============================================================================
# Include projects or dependencies into solution
# =============================================================================


add_subdirectory (TKJT)
add_subdirectory (JTAssistant)

