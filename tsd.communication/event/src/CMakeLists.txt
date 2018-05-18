file(GLOB_RECURSE SOURCES *.[ch]pp)

build_artifact(lib-static ${SOURCES})
module_include_directories(PUBLIC ${CMAKE_CURRENT_LIST_DIR})
module_dependencies(PUBLIC tsd.common)
