project(rabbit)
cmake_minimum_required(VERSION 2.8)

set(SDIR ${PROJECT_SOURCE_DIR})
set(BDIR ${PROJECT_BINARY_DIR})

set(DX_DIR "C:/Program\ Files/Microsoft\ DirectX\ SDK\ \(June\ 2010\)")

include_directories(
    ${SDIR}
    ${SDIR}/rt_window
    ${SDIR}/rt_mesh
    ${SDIR}/rt_collada

    ${DX_DIR}/Include
    )
link_directories( ${DX_DIR}/Lib/x86 )

add_executable(${PROJECT_NAME}
    main.cpp
    rt_defs.h
    rt_math.h

    rabbit.h
    rabbit.cpp

    rt_window/rt_window.h
    rt_window/rt_window.cpp
    rt_window/rt_d3dmanager.h
    rt_window/rt_d3dmanager.cpp

    rt_mesh/rt_mesh.h

    rt_mesh/flower_cube.h
    rt_mesh/flower_cube.cpp
    rt_mesh/test_udo.h
    rt_mesh/test_udo.cpp
    rt_mesh/suz.h
    rt_mesh/suz.cpp


    rt_mesh/info_object.h
    rt_mesh/info_object.cpp

    rt_collada/tinyxml2.h
    rt_collada/tinyxml2.cpp
    rt_collada/rt_collada.h
    rt_collada/rt_collada.cpp

    )

target_link_libraries(${PROJECT_NAME} d3d9 d3dx9 )


add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/data/flower_cube.bmp  ${CMAKE_CURRENT_BINARY_DIR}/flower_cube.bmp)
add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/data/flower_cube.dae  ${CMAKE_CURRENT_BINARY_DIR}/flower_cube.dae)

add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/data/suz_uv.bmp  ${CMAKE_CURRENT_BINARY_DIR}/suz_uv.bmp)
add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/data/suz.dae  ${CMAKE_CURRENT_BINARY_DIR}/suz.dae)

add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/data/test-udo_uv.bmp  ${CMAKE_CURRENT_BINARY_DIR}/test-udo_uv.bmp)
add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/data/test-udo.dae  ${CMAKE_CURRENT_BINARY_DIR}/test-udo.dae)

add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/data/stand.dae  ${CMAKE_CURRENT_BINARY_DIR}/stand.dae)

add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/rabbit.hlsl  ${CMAKE_CURRENT_BINARY_DIR}/rabbit.hlsl)


