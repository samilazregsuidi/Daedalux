file(GLOB INTEG_SOURCES *.cpp)
foreach(src ${INTEG_SOURCES})
  get_filename_component(name ${src} NAME_WE)
  add_executable(${name} ${src} ${CMAKE_SOURCE_DIR}/tests/TestFilesUtils.hpp)
  target_link_libraries(${name} PRIVATE daedalux::daedalux_lib GTest::gtest_main)
  gtest_discover_tests(${name}
    PROPERTIES LABELS "integration;${name}"
               TIMEOUT 120
               # You can pass extra args here if needed:
               # EXTRA_ARGS "--model-dir=${CMAKE_SOURCE_DIR}/examples/models"
  )
endforeach()

