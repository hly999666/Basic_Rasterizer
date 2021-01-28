cmake_minimum_required (VERSION  3.18)
project (basic_rasterizer)

include(CheckCXXCompilerFlag)

function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
        unset(flag_supported CACHE)
    endif()
endfunction()

#enable_cxx_compiler_flag_if_supported("-Wall")
#enable_cxx_compiler_flag_if_supported("-Wextra")
#enable_cxx_compiler_flag_if_supported("-pedantic")
enable_cxx_compiler_flag_if_supported("-std=c++14")
enable_cxx_compiler_flag_if_supported("-O3")
enable_cxx_compiler_flag_if_supported("-fopenmp")

#file(GLOB SOURCES *.h *.hpp *.cpp)
find_package(OpenCV REQUIRED)


file(GLOB SOURCES *.h *.cpp) 

message( ${SOURCES})
add_executable(basic_rasterizer_1  ${SOURCES})


target_link_libraries(basic_rasterizer_1 ${OpenCV_LIBRARIES})

