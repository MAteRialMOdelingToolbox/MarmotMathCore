# add_executable("TestMarmotNumericalIntegration_executable" "${CMAKE_CURRENT_LIST_DIR}/test/TestMarmotNumericalIntegration.cpp")
# include_directories("${CMAKE_CURRENT_BINARY_DIR}/../include/" "${INSTALLED_MODULE_PATHS}/include/")
# target_link_libraries("TestMarmotNumericalIntegration_executable" "${CMAKE_CURRENT_BINARY_DIR}/lib/libMarmot.so")
# add_dependencies("TestMarmotNumericalIntegration_executable" "${PROJECT_NAME}")
# add_test(NAME "TestMarmotNumericalIntegration" COMMAND "TestMarmotNumericalIntegration_executable" WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
# add_executable("MarmotMathCore_executable" "${CMAKE_CURRENT_LIST_DIR}/test/test.cpp")
# target_link_libraries("MarmotMathCore_executable" Marmot)
# add_test(NAME "MarmotMathCore" COMMAND "MarmotMathCore_executable")