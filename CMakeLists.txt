cmake_minimum_required(VERSION 3.17)

project(HbbJetPrunPlots
    DESCRIPTION "Hbb+Jet Pruning Plots"
)

add_library(${PROJECT_NAME} SHARED
    src/Record.cpp
    src/Helpers.cpp
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER inc/${PROJECT_NAME}/Record.h
    PUBLIC_HEADER inc/${PROJECT_NAME}/Helpers.h
)
target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/${PROJECT_NAME}>
    $<INSTALL_INTERFACE:inc/${PROJECT_NAME}>
)
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:inc>
)

find_package(ROOT REQUIRED COMPONENTS RooFit RooFitCore RooStats)
include(${ROOT_USE_FILE})

add_subdirectory(apps)