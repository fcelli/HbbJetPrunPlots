add_executable(performance_graph)
target_sources(performance_graph PRIVATE performance_graph.cpp)
target_link_libraries(performance_graph HbbJetPrunPlots ${ROOT_LIBRARIES})

add_executable(fit_comp)
target_sources(fit_comp PRIVATE fit_comp.cpp)
target_link_libraries(fit_comp HbbJetPrunPlots ${ROOT_LIBRARIES})