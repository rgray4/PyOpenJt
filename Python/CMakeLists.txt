if(MSVC)
    #add_definitions(-D_3DLIFE_BUILD_)
else(MSVC)
    add_definitions(-Wno-write-strings)
endif(MSVC)

find_package(Python3 COMPONENTS Development REQUIRED)

   
include_directories(
    ${CMAKE_SOURCE_DIR}/TKJT/src/
    ${CMAKE_SOURCE_DIR}/ToolsLib/src/
) 




link_directories(${PYTHON_INCLUDE_DIR}/../libs)


add_library(PyOpenJt SHARED 
	PyOpenJt.cpp
    JtFileObject.cpp
)

target_link_libraries(PyOpenJt PRIVATE Python3::Python TKJT ToolsLib)

if(MSVC)
	set_target_properties(PyOpenJt PROPERTIES SUFFIX ".pyd")
endif(MSVC)

set_target_properties(PyOpenJt PROPERTIES RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR})
set_target_properties(PyOpenJt PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set_target_properties(PyOpenJt PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo)
set_target_properties(PyOpenJt PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug)
set_target_properties(PyOpenJt PROPERTIES LIBRARY_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR}/lib)
set_target_properties(PyOpenJt PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/lib)
set_target_properties(PyOpenJt PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo/lib)
set_target_properties(PyOpenJt PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug/lib)
set_target_properties(PyOpenJt PROPERTIES OUTPUT_NAME bca)
set_target_properties(PyOpenJt PROPERTIES DEBUG_OUTPUT_NAME bca_d)
set_target_properties(PyOpenJt PROPERTIES PREFIX "")



if(WIN32)
    INSTALL(TARGETS PyOpenJt
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
else(WIN32)
    INSTALL(TARGETS PyOpenJt
        RUNTIME DESTINATION ~/bin
        LIBRARY DESTINATION ~/lib
        ARCHIVE DESTINATION ~/lib
   )
endif(WIN32)