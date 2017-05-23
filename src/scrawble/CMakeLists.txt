
set(SOURCE_FILES
	main.cpp
	game.cpp
	game_config.cpp
	io.cpp
)

set(BINARY_NAME ${PROJECT_NAME}-bin)

add_executable(${BINARY_NAME} ${SOURCE_FILES})

target_include_directories(${BINARY_NAME} SYSTEM PUBLIC ${CMAKE_SOURCE_DIR}/lib/src)

target_link_libraries(${BINARY_NAME} ${CURSES_LIBRARIES} ${PROJECT_NAME})

set_target_properties(${BINARY_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
