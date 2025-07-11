# === src/CMakeLists.txt ===

# Library for core game logic
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
list(REMOVE_ITEM SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_library(pokerlib ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(pokerlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Executable for main game (with Raylib)
add_executable(poker_main main.cpp)
target_link_libraries(pokerlib PUBLIC raylib)
target_link_libraries(poker_main PRIVATE pokerlib)

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(
        poker_main
        PRIVATE
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL")
endif()

# Emscripten/Web build handling
if ("${PLATFORM}" STREQUAL "Web")
    set_target_properties(poker_main PROPERTIES SUFFIX ".html")
    target_link_options(poker_main PUBLIC -sUSE_GLFW=3 PUBLIC --preload-file assets)
endif()

# Output directories
set_target_properties(poker_main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Copy assets folder after build (optional)
if ("${PLATFORM}" STREQUAL "Web")
    add_custom_command(
        TARGET poker_main PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:poker_main>/../assets
    )
else()
    add_custom_command(
        TARGET poker_main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:poker_main>/assets
    )
endif()
