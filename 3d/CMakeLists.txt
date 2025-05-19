cmake_minimum_required(VERSION 3.24)
project(3d)

set(CMAKE_CXX_STANDARD 17)
find_package(PkgConfig)

find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt5 COMPONENTS Sql REQUIRED)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
qt_add_resources(RESOURCES icons.qrc)

#
    pkg_check_modules(GTKMM gtkmm-3.0)

    include_directories(${GTKMM_INCLUDE_DIRS})
    link_directories(${GTKMM_LIBRARY_DIRS})
    add_executable(3d main_auth.cpp Obj3DModel.cpp Obj3DModel.h Primitives.h myarea.cpp myarea.h stlParser.cpp Polygons.cpp Polygons.h loginWindow.cpp loginWindow.h SliceWindow.cpp SliceWindow.h authmanager.h authmanager.cpp clipper.cpp clipper.hpp ${RESOURCES})
    target_link_libraries(${CMAKE_PROJECT_NAME} Qt5::Widgets Qt5::Sql)

#[[
    pkg_check_modules(GTKMM gtkmm-3.0)

    include_directories(${GTKMM_INCLUDE_DIRS})
    link_directories(${GTKMM_LIBRARY_DIRS})
    add_executable(3d main_gui.cpp Obj3DModel.cpp Obj3DModel.h Primitives.h myarea.cpp myarea.h stlParser.cpp Polygons.cpp Polygons.h SliceWindow.cpp SliceWindow.h clipper.cpp clipper.hpp ${RESOURCES} )
target_link_libraries(${CMAKE_PROJECT_NAME} Qt5::Widgets Qt5::Sql)

#[[
    add_executable(3d main_slice.cpp Obj3DModel.cpp Obj3DModel.h Primitives.h stlParser.cpp Polygons.cpp Polygons.h)
    target_link_libraries(${CMAKE_PROJECT_NAME})
]]
#[[
    add_executable(3d main.cpp Primitives.h Polygons.cpp Polygons.h)
    target_link_libraries(${CMAKE_PROJECT_NAME})
]]