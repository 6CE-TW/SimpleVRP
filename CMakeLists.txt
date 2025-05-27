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
# Source files list
# can add .cu files for CUDA later
# ========================
set(SOURCE_FILES
    test/main.cc
    data/data.cc
    algorithm/solver.cc
    algorithm/initial_solution/initial_solution.cc
    algorithm/local_search/tools.cc
    algorithm/local_search/local_search.cc
    algorithm/parameter/parameter.cc
)

# ========================
# Target executable
# ========================
add_executable(main ${SOURCE_FILES})

# ========================
# Link CPR
# ========================
target_link_libraries(main PRIVATE cpr::cpr)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/algorithm
    ${CMAKE_CURRENT_SOURCE_DIR}/algorithm/parameter
    ${CMAKE_CURRENT_SOURCE_DIR}/algorithm/local_search
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