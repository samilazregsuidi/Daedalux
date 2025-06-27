file(GLOB_RECURSE UNIT_SOURCES *.cpp)
foreach(src ${UNIT_SOURCES})
  get_filename_component(name ${src} NAME_WE)
  add_executable(${name} ${src} ${CMAKE_SOURCE_DIR}/tests/TestFilesUtils.hpp)
  target_link_libraries(${name} PRIVATE daedalux::daedalux_lib GTest::gtest_main)
  gtest_discover_tests(${name}
    PROPERTIES LABELS "unit;${name}"
  )
endforeach()

