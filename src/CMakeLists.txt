cmake_minimum_required( VERSION 3.10)

set(PATCH_VERSION "2" CACHE INTERNAL "Patch version")

project( homework2 VERSION 0.1.${PATCH_VERSION})
configure_file( version_config.in ${CMAKE_BINARY_DIR}/generated/version_config.h )
include_directories ( ${CMAKE_BINARY_DIR}/generated/ )
# copy input to build directory
file( COPY ip_filter.tsv DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
file( COPY etalon.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
add_library( homework2_lib do_homework2.cpp )

add_executable( homework2 ip_filter.cpp homework2_version.cpp )
add_executable( homework2_test homework2_test.cpp )

target_link_libraries(homework2 PRIVATE homework2_lib )
target_link_libraries(homework2_test PRIVATE homework2_lib )

# package part which enables
# run: cmake --build . --target package
install(TARGETS homework2 RUNTIME DESTINATION bin)
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)

#testing part which enables
# cmake --build . --target test
enable_testing()
add_test(test_md5 homework2_test) 