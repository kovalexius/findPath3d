cmake_minimum_required(VERSION 2.8)

project(find_path)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions( -Wall -Wl,--no-as-needed -pthread -lpthread -std=gnu++11 -DGL_GLEXT_PROTOTYPES )
elseif (MSVC)
        ## add appropriate keys
endif()

find_package (Qt4 REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(QT_USE_QTOPENGL TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include (${QT_USE_FILE})

add_executable( find3d
                ./src/main.cpp
		./src//gl_renderer/
		./src//gl_renderer/glExtInit.cpp
		./src//gl_renderer/glExtInit.h
		./src//gl_renderer/glmapview.cpp
		./src//gl_renderer/glmapview.h
		./src/gl_renderer/GL/glaux.h
		./src/gl_renderer/GL/glcorearb.h
		./src/gl_renderer/GL/glext.h
		./src/gl_renderer/GL/glu.h
		./src/gl_renderer/GL/glxext.h
		./src/gl_renderer/GL/wglext.h
		./src/kfp_algo/Geometry.cpp
		./src/kfp_algo/Geometry.h
		./src/kfp_algo/Heightmap.cpp
		./src/kfp_algo/Heightmap.h
		./src/kfp_algo/MeshObject.cpp
		./src/kfp_algo/MeshObject.h
		./src/kfp_algo/Scene.cpp
		./src/kfp_algo/Scene.h
		./src/widgets/findpath3d.qrc
		./src/widgets/findpath3d.ui
		./src/widgets/findpath3d.cpp
		./src/widgets/findpath3d.h
              )

include_directories( 
		     ./src/gl_renderer/
                     ./src/gl_renderer/GL/
		     ./src/kfp_algo/
	             ./src/widgets/
                   )

source_group(src)

configure_file( ./src/gl_renderer/Land.frag ${CMAKE_CURRENT_BINARY_DIR}/Land.frag COPYONLY )
configure_file( ./src/gl_renderer/Land.vert ${CMAKE_CURRENT_BINARY_DIR}/Land.vert COPYONLY )
configure_file( ./src/gl_renderer/Water.frag ${CMAKE_CURRENT_BINARY_DIR}/Water.frag COPYONLY )
configure_file( ./src/gl_renderer/Water.vert ${CMAKE_CURRENT_BINARY_DIR}/Water.vert COPYONLY )

foreach (_variableName ${QT_LIBRARIES})
	message(STATUS ${_variableName} )
endforeach()

foreach (_variableName ${OPENGL_LIBRARIES})
	message(STATUS ${_variableName} )
endforeach()

#target_link_libraries( find3d Qt4::Widgets Qt4:Core)
target_link_libraries( find3d ${QT_LIBRARIES} ${OPENGL_LIBRARIES} )
