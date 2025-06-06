cmake_minimum_required(VERSION 3.17)
project(SimpleVRP LANGUAGES CXX)

# ========================
# Basic Settings
# ========================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ========================
# CUDA optional support (for future use)
# ========================
enable_language(CUDA OPTIONAL)

# ========================
# CPR settings
# Assumes CPR is cloned via git submodule into dependencies/cpr
# and has already been built
# ========================
add_subdirectory(dependencies/cpr)

# ========================
# JSON settings
# ========================
add_subdirectory(dependencies/json)

# ========================
# Source files list
# can add .cu files for CUDA later
# ========================
set(SOURCE_FILES
    test/main.cc
    data/data.cc
    solver/algorithm/solver.cc
    solver/algorithm/initial_solution/initial_solution.cc
    solver/algorithm/local_search/tools.cc
    solver/algorithm/local_search/local_search.cc
    solver/algorithm/parameter/parameter.cc
    solver/wrapper/wrapper.cc
)

# ========================
# Target executable
# ========================
add_executable(main ${SOURCE_FILES})

# ========================
# Create results directory
# ========================
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/../test/results")

add_custom_target(make_result_dir
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/../test/results"
)

add_dependencies(main make_result_dir)

# ========================
# Link CPR
# ========================
target_link_libraries(main PRIVATE cpr::cpr nlohmann_json)

# ========================
# CUDA settings (for future use)
# ========================
if(CMAKE_CUDA_COMPILER)
    message(STATUS "CUDA detected: ${CMAKE_CUDA_COMPILER}")
    # Example: list(APPEND SOURCE_FILES src/cuda/my_kernel.cu)
    # target_sources(main PRIVATE src/cuda/my_kernel.cu)
    # set_target_properties(main PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif()

# ========================
# Include paths (to locate custom headers)
# ========================
target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/cpr/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/cpr/build/_deps/curl-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/cpr/build/cpr_generated_includes
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/algorithm
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/algorithm/parameter
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/algorithm/local_search
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/dao
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/wrapper
    ${CMAKE_CURRENT_SOURCE_DIR}/test
)

# ========================
# Post-build: Copy required DLLs to output dir
# ========================
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/dependencies/cpr/cpr/Release/cpr.dll"
            $<TARGET_FILE_DIR:main>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/_deps/curl-build/lib/Release/libcurl.dll"
            $<TARGET_FILE_DIR:main>
)