set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories(     ${CMAKE_SOURCE_DIR}/sdk/tests )

macro_add_unittest_definitions()

########### next target ###############
set(kis_ppm_test_SRCS kis_ppm_test.cpp )

kde4_add_broken_unit_test(kis_ppm_test TESTNAME krita-plugin-format-ppm_test ${kis_ppm_test_SRCS})

target_link_libraries(kis_ppm_test   kritaui Qt5::Test)
