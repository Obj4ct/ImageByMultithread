set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti")
# 配置头文件search目录
include_directories(
        ${PROJECT_SOURCE_DIR}/include
)




file(GLOB files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
foreach(file ${files})
    string(REGEX REPLACE "(.+).cpp$" "\\1" exe ${file})
    add_executable(${exe} ${file})
    target_link_libraries(${exe} Multithread ${dependency_LIBS})
    install(TARGETS ${exe}
        RUNTIME
        DESTINATION "bin"
        BUNDLE DESTINATION "bin"
        )
endforeach()
