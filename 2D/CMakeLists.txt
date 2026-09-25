add_executable(smoke-simulator-2D
        main.cpp
        utils.cpp
        utils.h
        renderSFML.cpp
        renderSFML.h
        color.hh
)

target_link_libraries(smoke-simulator-2D SFML::Graphics SFML::Window SFML::System)

# Default image used by "smoke-simulator-2D image"
target_compile_definitions(smoke-simulator-2D PRIVATE SMOKE_ASSETS_DIR="${PROJECT_SOURCE_DIR}/assets")
