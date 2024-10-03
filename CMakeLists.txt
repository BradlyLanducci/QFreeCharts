cmake_minimum_required(VERSION 3.16)

project(QFreeCharts VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(${PROJECT_NAME})

qt_add_qml_module(${PROJECT_NAME}
    URI ${PROJECT_NAME}
    VERSION 1.0

    SOURCES
        source/Series/Series.h
        source/Series/Series.cpp
        source/Shaders/RotationShader.h
        source/Shaders/RotationShader.cpp
        source/Shaders/RotationMaterial.h
        source/Shaders/RotationMaterial.cpp
        source/Shaders/RotationNode.h
        source/Shaders/RotationNode.cpp
        source/Resources.qrc
        source/main.cpp

    QML_FILES
        source/Qml/Main.qml
        source/Qml/LineChart.qml

    RESOURCES
        source/Shaders/Rotation.vert
        source/Shaders/Rotation.frag
        source/Shaders/Rotation.vert.qsb
        source/Shaders/Rotation.frag.qsb
)

add_library(FreeCharts STATIC
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Quick
)

set(CMAKE_INSTALL_PREFIX ../install/)

# Install the library
install(TARGETS ${PROJECT_NAME}
    EXPORT FreeChartsTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install the public header files
install(FILES
    source/Series/Series.h
    source/Shaders/RotationShader.h
    source/Shaders/RotationMaterial.h
    source/Shaders/RotationNode.h
    DESTINATION include/${PROJECT_NAME}
)

# Install QML files
install(FILES
    source/Qml/Main.qml
    source/Qml/LineChart.qml
    DESTINATION ${CMAKE_INSTALL_PREFIX}/qml/${PROJECT_NAME}
)

# Install shader files
install(FILES
    source/Shaders/Rotation.vert.qsb
    source/Shaders/Rotation.frag.qsb
    DESTINATION ${CMAKE_INSTALL_PREFIX}/shaders/${PROJECT_NAME}
)

# Export the library for other projects to use
install(EXPORT FreeChartsTargets
    FILE FreeChartsTargets.cmake
    NAMESPACE FreeCharts::
    DESTINATION lib/cmake/FreeCharts
)

# Create a package configuration file for CMake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/FreeChartsConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/FreeChartsConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/FreeChartsConfig.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/FreeChartsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/FreeChartsConfigVersion.cmake"
    DESTINATION lib/cmake/FreeCharts
)
