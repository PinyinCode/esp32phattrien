# Define source files for default configuration (wk-esp32s3-dev)
set(SOURCES 
    "audio/audio_codec.cc"
    "audio/audio_debugger.cc"
    "audio/audio_service.cc"
    "audio/demuxer/ogg_demuxer.cc"
    "audio/codecs/no_audio_codec.cc"
    "audio/codecs/box_audio_codec.cc"
    "audio/codecs/es8311_audio_codec.cc"
    "audio/codecs/es8374_audio_codec.cc"
    "audio/codecs/es8388_audio_codec.cc"
    "audio/codecs/es8389_audio_codec.cc"
    "audio/codecs/dummy_audio_codec.cc"
    "led/single_led.cc"
    "led/circular_strip.cc"
    "led/gpio_led.cc"
    "display/display.cc"
    "display/text_glyph.cc"
    "display/lcd_display.cc"
    "display/oled_display.cc"
    "display/lvgl_display/lvgl_display.cc"
    "display/emote_display.cc"
    "display/lvgl_display/emoji_collection.cc"
    "display/lvgl_display/lvgl_theme.cc"
    "display/lvgl_display/lvgl_font.cc"
    "display/lvgl_display/dynamic_glyph_cache.cc"
    "display/lvgl_display/lvgl_image.cc"
    "display/lvgl_display/gif/lvgl_gif.cc"
    "display/lvgl_display/gif/gifdec.c"
    "display/lvgl_display/jpg/image_to_jpeg.cpp"
    "display/lvgl_display/jpg/jpeg_to_image.c"
    "protocols/protocol.cc"
    "protocols/text_glyph_payload.cc"
    "protocols/mqtt_protocol.cc"
    "protocols/websocket_protocol.cc"
    "mcp_server.cc"
    "system_info.cc"
    "application.cc"
    "ota.cc"
    "settings.cc"
    "device_state_machine.cc"
    "assets.cc"
    "main.cc"
)

set(INCLUDE_DIRS 
    "." 
    "display" 
    "display/lvgl_display" 
    "display/lvgl_display/jpg" 
    "audio" 
    "audio/demuxer" 
    "protocols"
)

# Add board common files
list(APPEND SOURCES
    "boards/common/board.cc"
    "boards/common/wifi_board.cc"
    "boards/common/adc_battery_monitor.cc"
    "boards/common/axp2101.cc"
    "boards/common/backlight.cc"
    "boards/common/button.cc"
    "boards/common/i2c_device.cc"
    "boards/common/knob.cc"
    "boards/common/power_save_timer.cc"
    "boards/common/press_to_talk_mcp_tool.cc"
    "boards/common/sleep_timer.cc"
    "boards/common/sy6970.cc"
    "boards/common/system_reset.cc"
    "boards/common/esp_video.cc"
    "boards/common/esp32_camera.cc"
)
list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/boards/common)

idf_build_get_property(build_components BUILD_COMPONENTS)
function(find_component_by_pattern PATTERN COMPONENT_VAR PATH_VAR)
    foreach(COMPONENT ${build_components})
        if(COMPONENT MATCHES "${PATTERN}")
            set(${COMPONENT_VAR} ${COMPONENT} PARENT_SCOPE)
            idf_component_get_property(COMPONENT_PATH ${COMPONENT} COMPONENT_DIR)
            set(${PATH_VAR} "${COMPONENT_PATH}" PARENT_SCOPE)
            break()
        endif()
    endforeach()
endfunction()

# Cố định cấu hình cho board wk-esp32s3-dev
set(BUILTIN_TEXT_FONT font_noto_sans_basic_20_4)
set(BUILTIN_ICON_FONT font_material_symbols_20_4)
set(DEFAULT_EMOJI_COLLECTION noto-color-emoji_128)
set(EMOTE_RESOLUTION "320_240")

set(BOARD_DIR "wk-esp32s3-dev")
set(BOARD_TYPE "wk-esp32s3-dev")
set(BOARD_NAME "wk-esp32s3-dev")
set(BOARD_MANUFACTURER "")

set(BOARD_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_DIR}/config.json")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${BOARD_CONFIG_FILE}")

# Thêm source của riêng board wk
file(GLOB BOARD_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_DIR}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_DIR}/*.c
)
list(APPEND SOURCES ${BOARD_SOURCES})

# Audio engine cho ESP32S3
list(APPEND SOURCES 
    "audio/engines/afe_audio_engine.cc"
    "audio/wake_words/custom_wake_word.cc"
    "audio/wake_words/wake_word_audio_cache.cc"
)

# Language config (mặc định lấy en-US hoặc zh-CN tùy bạn, ở đây dùng en-US làm ví dụ hoặc giữ cơ chế quét ngôn ngữ gọn nhẹ)
set(LANG_DIR "en-US")
set(LANG_JSON "${CMAKE_CURRENT_SOURCE_DIR}/assets/locales/${LANG_DIR}/language.json")
set(LANG_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/assets/lang_config.h")

file(GLOB LANG_SOUNDS ${CMAKE_CURRENT_SOURCE_DIR}/assets/locales/${LANG_DIR}/*.ogg)
file(GLOB COMMON_SOUNDS ${CMAKE_CURRENT_SOURCE_DIR}/assets/common/*.ogg)

idf_component_register(SRCS ${SOURCES}
                    EMBED_FILES ${LANG_SOUNDS} ${COMMON_SOUNDS}
                    INCLUDE_DIRS ${INCLUDE_DIRS}
                    WHOLE_ARCHIVE
                    PRIV_REQUIRES
                        esp_pm
                        esp_psram
                        esp_netif
                        esp_eth
                        esp_driver_gpio
                        esp_driver_uart
                        esp_driver_spi
                        esp_driver_i2c
                        esp_driver_i2s
                        esp_driver_ledc
                        esp_driver_jpeg
                        esp_driver_ppa
                        esp_driver_tsens
                        esp_app_format
                        app_update
                        spi_flash
                        mbedtls
                        console
                        efuse
                        bt
                        fatfs
                        xiaozhi-fonts
                        esp_http_client
                        esp_https_ota
                    )

string(REGEX MATCH "^font_noto_sans_basic_([0-9]+)_([0-9]+)$" _font_match "${BUILTIN_TEXT_FONT}")
set(TEXT_FONT_SIZE "${CMAKE_MATCH_1}")
set(TEXT_FONT_BPP "${CMAKE_MATCH_2}")

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    target_compile_definitions(${COMPONENT_LIB}
                        PRIVATE BOARD_TYPE=\"${BOARD_TYPE}\" BOARD_NAME=\"${BOARD_NAME}\"
                        PRIVATE BOARD_MANUFACTURER=\"${BOARD_MANUFACTURER}\"
                        PRIVATE BUILTIN_TEXT_FONT=${BUILTIN_TEXT_FONT} BUILTIN_ICON_FONT=${BUILTIN_ICON_FONT}
                        PRIVATE TEXT_FONT_SIZE=${TEXT_FONT_SIZE} TEXT_FONT_BPP=${TEXT_FONT_BPP}
                        )
endif()

# Ngôn ngữ generation rules
add_custom_command(
    OUTPUT ${LANG_HEADER}
    COMMAND python ${PROJECT_DIR}/scripts/gen_lang.py
            --language "${LANG_DIR}"
            --output "${LANG_HEADER}"
    DEPENDS
        ${LANG_JSON}
        ${PROJECT_DIR}/scripts/gen_lang.py
    COMMENT "Generating ${LANG_DIR} language config"
)

add_custom_target(lang_header ALL
    DEPENDS ${LANG_HEADER}
)

find_component_by_pattern("espressif__esp-sr" ESP_SR_COMPONENT ESP_SR_COMPONENT_PATH)
if(ESP_SR_COMPONENT_PATH)
    set(ESP_SR_MODEL_PATH "${ESP_SR_COMPONENT_PATH}/model")
endif()

find_component_by_pattern("xiaozhi-fonts" NOTO_FONTS_COMPONENT NOTO_FONTS_COMPONENT_PATH)
if(NOTO_FONTS_COMPONENT_PATH)
    set(NOTO_FONTS_PATH "${NOTO_FONTS_COMPONENT_PATH}")
endif()

function(build_default_assets_bin)
    set(GENERATED_ASSETS_BIN "${CMAKE_BINARY_DIR}/generated_assets.bin")
    set(DEFAULT_ASSETS_DEPENDENCIES
        "${SDKCONFIG}"
        "${PROJECT_DIR}/scripts/build_default_assets.py"
    )

    set(BUILD_ARGS
        "--sdkconfig" "${SDKCONFIG}"
        "--output" "${GENERATED_ASSETS_BIN}"
    )

    if(BUILTIN_TEXT_FONT)
        list(APPEND BUILD_ARGS "--builtin_text_font" "${BUILTIN_TEXT_FONT}")
        string(REPLACE "_basic_" "_common_" COMMON_TEXT_FONT "${BUILTIN_TEXT_FONT}")
        set(COMMON_TEXT_FONT_FILE "${NOTO_FONTS_PATH}/cbin/${COMMON_TEXT_FONT}.bin")
        if(EXISTS "${COMMON_TEXT_FONT_FILE}")
            list(APPEND DEFAULT_ASSETS_DEPENDENCIES "${COMMON_TEXT_FONT_FILE}")
        endif()
        if(EXISTS "${NOTO_FONTS_PATH}/manifest.json")
            list(APPEND DEFAULT_ASSETS_DEPENDENCIES "${NOTO_FONTS_PATH}/manifest.json")
        endif()
    endif()

    if(DEFAULT_EMOJI_COLLECTION)
        list(APPEND BUILD_ARGS "--emoji_collection" "${DEFAULT_EMOJI_COLLECTION}")
        file(GLOB DEFAULT_EMOJI_FILES CONFIGURE_DEPENDS
            "${NOTO_FONTS_PATH}/png/${DEFAULT_EMOJI_COLLECTION}/*"
        )
        list(APPEND DEFAULT_ASSETS_DEPENDENCIES ${DEFAULT_EMOJI_FILES})
    endif()

    list(APPEND BUILD_ARGS "--esp_sr_model_path" "${ESP_SR_MODEL_PATH}")
    list(APPEND BUILD_ARGS "--noto_fonts_path" "${NOTO_FONTS_PATH}")

    add_custom_command(
        OUTPUT ${GENERATED_ASSETS_BIN}
        COMMAND python ${PROJECT_DIR}/scripts/build_default_assets.py ${BUILD_ARGS}
        DEPENDS ${DEFAULT_ASSETS_DEPENDENCIES}
        COMMENT "Building default assets.bin based on configuration"
        VERBATIM
    )

    add_custom_target(generated_default_assets ALL
        DEPENDS ${GENERATED_ASSETS_BIN}
    )

    set(GENERATED_ASSETS_LOCAL_FILE ${GENERATED_ASSETS_BIN} PARENT_SCOPE)
endfunction()

partition_table_get_partition_info(size "--partition-name assets" "size")
partition_table_get_partition_info(offset "--partition-name assets" "offset")
if ("${size}" AND "${offset}")
    build_default_assets_bin()
    esptool_py_flash_to_partition(flash "assets" "${GENERATED_ASSETS_LOCAL_FILE}")
    message(STATUS "Generated default assets flash configured: ${GENERATED_ASSETS_LOCAL_FILE} -> assets partition")
else()
    message(STATUS "Assets partition not found, using v1 partition table")
endif()
