
# install public header
file(GLOB_RECURSE PUBLIC_HEADERS
        *.h*
        )

# General source files 支持 * 通配符引入文件
file(GLOB SOURCE_FILES
        ${SOURCE_FILES}
        *.cpp
        )

# 头文件会在以下目录进行搜索
include_directories(
        ${PROJECT_SOURCE_DIR}/include
)

set(LIBRARY_NAME "Multithread")

add_library(${LIBRARY_NAME} STATIC ${SOURCE_FILES}) # 配置静态库
#add_library(${LIBRARY_NAME} SHARED ${SOURCE_FILES}) # 配置动态库

# windows 编译debug版本后，在库名字后面追加d
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_target_properties(${LIBRARY_NAME} PROPERTIES
            DEBUG_POSTFIX "d"
            OUTPUT_NAME "${LIBRARY_NAME}"
            )
endif()

# windows 编译release库生成pdb符号文件
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_options(${LIBRARY_NAME} PRIVATE /Zi)

    # Tell linker to include symbol data
    set_target_properties(${LIBRARY_NAME} PROPERTIES
            LINK_FLAGS "/INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF"
            )

    # Set file name & location
    set_target_properties(${LIBRARY_NAME} PROPERTIES
            COMPILE_PDB_NAME ${LIBRARY_NAME}
            COMPILE_PDB_OUTPUT_DIR ${CMAKE_BINARY_DIR}
            )
endif()

# install lib and headers
if(APPLE)
    install(
            TARGETS ${LIBRARY_NAME}
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            FRAMEWORK DESTINATION /
    )
else()
    install(TARGETS ${LIBRARY_NAME}
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
    )
endif ()

foreach(file ${PUBLIC_HEADERS})
    get_filename_component( dir ${file} DIRECTORY )
    if (NOT ${dir} STREQUAL "")
        string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} "" dir ${dir})
    endif()
    if (NOT ${dir} STREQUAL "")
        string(REPLACE ${CMAKE_CURRENT_BINARY_DIR} "" dir ${dir})
    endif()
    install(FILES ${file} DESTINATION include/${dir})
endforeach()