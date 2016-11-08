cmake_minimum_required(VERSION 2.8)

project(find_path)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions( -Wall -Wl,--no-as-needed -pthread -lpthread -std=gnu++11 -DGL_GLEXT_PROTOTYPES )
elseif (MSVC)
endif()

find_package(Qt4 REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(QT_USE_QTOPENGL TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include (${QT_USE_FILE})

set( GLRENDERER 
        ../glExt/glExtInit.cpp
        ../glExt/glExtInit.h )

set( GLRENDERER_MOC 
        ./src/gl_renderer/glmapview.cpp
        ./src/gl_renderer/glmapview.h )

set( GL
        ../glExt/GL/glext.h
        ../glExt/GL/glu.h
        ../glExt/GL/wglext.h )

set( KFP
        ./src/kfp_algo/Heightmap.cpp
        ./src/kfp_algo/Heightmap.h
        ./src/kfp_algo/MeshObject.cpp
        ./src/kfp_algo/MeshObject.h
        ./src/kfp_algo/Scene.cpp
        ./src/kfp_algo/Scene.h )
		
set( MAP_READERS
		./src/map_readers/fileMapReader.cpp
		./src/map_readers/fileMapReader.h )

set ( WIDGETS_RESOURCES
        ./src/widgets/findpath3d.qrc )

set ( WIDGETS_UI
        ./src/widgets/findpath3d.ui )

set ( WIDGETS_MOC
        ./src/widgets/findpath3d.cpp
        ./src/widgets/findpath3d.h )

#qt4_wrap_cpp( MOC_SOURCES 
#                ${WIDGETS_MOC} 
#                ${GLRENDERER_MOC} )
                
qt4_add_resources( QRC_SOURCES 
                    ${WIDGETS_RESOURCES} )

QT4_WRAP_UI( UI_SOURCES 
                ${WIDGETS_UI} )

#qt4_generate_moc(MOC_SOURCES)
                
include_directories( 
                ./src/gl_renderer/
                ${CMAKE_SOURCE_DIR}/glExt/
                ${CMAKE_SOURCE_DIR}/geoMath/
                ./src/kfp_algo/
                ./src/widgets/
				./src/map_readers
                   )             

add_executable( find3d
                ./src/main.cpp
                ${GLRENDERER}
                ${GL}
                ${KFP}
                ${MOC_SOURCES}
                ${QRC_SOURCES}
                ${UI_SOURCES}
                ${GLRENDERER_MOC}
                ${WIDGETS_MOC}
				${MAP_READERS}
              )

source_group( "gl_renderer" FILES ${GLRENDERER} ${GLRENDERER_MOC} )
source_group( "gl_renderer/GL/" FILES ${GL} )
source_group( "kfp_algo" FILES ${KFP} )
source_group( "widgets" FILES ${WIDGETS_MOC} ${WIDGETS_UI} ${WIDGETS_RESOURCES} )
source_group( "map_readers" FILES ${MAP_READERS} )

configure_file( ./src/gl_renderer/Land.frag ${CMAKE_CURRENT_BINARY_DIR}/Land.frag COPYONLY )
configure_file( ./src/gl_renderer/Land.vert ${CMAKE_CURRENT_BINARY_DIR}/Land.vert COPYONLY )
configure_file( ./src/gl_renderer/Water.frag ${CMAKE_CURRENT_BINARY_DIR}/Water.frag COPYONLY )
configure_file( ./src/gl_renderer/Water.vert ${CMAKE_CURRENT_BINARY_DIR}/Water.vert COPYONLY )

message (STATUS ${QT_DIR})

foreach (_variableName ${QT_LIBRARIES})
	message(STATUS ${_variableName} )
endforeach()

foreach (_variableName ${OPENGL_LIBRARIES})
	#message(STATUS ${_variableName} )
endforeach()

#target_link_libraries( find3d Qt4::Widgets Qt4:Core)
target_link_libraries( find3d
                       geomath
                       ${QT_LIBRARIES} 
                       ${OPENGL_LIBRARIES} )
