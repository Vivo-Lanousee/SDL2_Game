cmake_minimum_required(VERSION 3.16)

# プロジェクト名
project(MeltedDefense)

# C++規格の設定
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# パスの定義
set(LIBS_DIR "${CMAKE_SOURCE_DIR}/libs")
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/MyGame")
set(ARCH "x64")

message(STATUS "Build Mode: Manual library configuration")
message(STATUS "Root Directory: ${CMAKE_SOURCE_DIR}")

#SDL2 手動設定 
set(SDL2_INCLUDE_DIR "${LIBS_DIR}/SDL2/include")
set(SDL2_LIBRARY     "${LIBS_DIR}/SDL2/lib/${ARCH}/SDL2.lib")
set(SDL2MAIN_LIBRARY "${LIBS_DIR}/SDL2/lib/${ARCH}/SDL2main.lib")

add_library(SDL2::SDL2 UNKNOWN IMPORTED)
set_target_properties(SDL2::SDL2 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}"
    IMPORTED_LOCATION "${SDL2_LIBRARY}"
)

add_library(SDL2::SDL2main UNKNOWN IMPORTED)
set_target_properties(SDL2::SDL2main PROPERTIES
    IMPORTED_LOCATION "${SDL2MAIN_LIBRARY}"
)

#  SDL2_image 手動設定 ---
set(SDL2_IMAGE_INCLUDE_DIR "${LIBS_DIR}/SDL2_image/include")
set(SDL2_IMAGE_LIBRARY     "${LIBS_DIR}/SDL2_image/lib/${ARCH}/SDL2_image.lib")

add_library(SDL2_image::SDL2_image UNKNOWN IMPORTED)
set_target_properties(SDL2_image::SDL2_image PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_IMAGE_INCLUDE_DIR}"
    IMPORTED_LOCATION "${SDL2_IMAGE_LIBRARY}"
    INTERFACE_LINK_LIBRARIES SDL2::SDL2
)

#  SDL2_ttf 手動設定 ---
set(SDL2_TTF_INCLUDE_DIR "${LIBS_DIR}/SDL2_ttf/include")
set(SDL2_TTF_LIBRARY     "${LIBS_DIR}/SDL2_ttf/lib/${ARCH}/SDL2_ttf.lib")

add_library(SDL2_ttf::SDL2_ttf UNKNOWN IMPORTED)
set_target_properties(SDL2_ttf::SDL2_ttf PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_TTF_INCLUDE_DIR}"
    IMPORTED_LOCATION "${SDL2_TTF_LIBRARY}"
    INTERFACE_LINK_LIBRARIES SDL2::SDL2
)

# --- ImGui のソースファイルをリストアップ ---
set(IMGUI_DIR "${LIBS_DIR}/imgui")
set(IMGUI_SOURCES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imgui_impl_sdl2.cpp"
    "${IMGUI_DIR}/imgui_impl_sdlrenderer2.cpp"
)

# --- インクルードパスの設定 ---
include_directories(
    "${PROJECT_DIR}/src"
    "${PROJECT_DIR}/include" # nlohmann json用
    "${IMGUI_DIR}"
)

# --- プロジェクトソースファイルの収集 ---
file(GLOB_RECURSE SOURCES 
    "${PROJECT_DIR}/src/*.cpp" 
    "${PROJECT_DIR}/src/*.h"
)

# --- 実行ファイルの生成 ---
# Windowsでコンソールを出さない場合は WIN32 を追加
add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES})

# --- ライブラリのリンク ---
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
)

# --- ビルド後処理: アセットのコピー ---
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Copying assets to output directory..."
)

# --- ビルド後処理: 必要な全DLLのコピー ---
# libsから主要DLLを収集
set(MAIN_DLLS
    "${LIBS_DIR}/SDL2/lib/${ARCH}/SDL2.dll"
    "${LIBS_DIR}/SDL2_image/lib/${ARCH}/SDL2_image.dll"
    "${LIBS_DIR}/SDL2_ttf/lib/${ARCH}/SDL2_ttf.dll"
)

# MyGame直下の追加DLLを取得 (画像サポート等)
file(GLOB EXTRA_DLLS "${PROJECT_DIR}/*.dll")

foreach(dll_path IN LISTS MAIN_DLLS EXTRA_DLLS)
    if(EXISTS "${dll_path}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${dll_path}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMENT "Copying DLL: ${dll_path}"
        )
    endif()
endforeach()