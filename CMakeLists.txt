cmake_minimum_required(VERSION 3.22)

# override for faster local builds: -DCMAKE_OSX_ARCHITECTURES=arm64
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "minimum supported macOS")
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "build architectures")

project(Silver68 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # for clangd / IDE tooling

# JUCE (git submodule at libs/JUCE)
add_subdirectory(libs/JUCE)

juce_add_plugin(Silver68
    COMPANY_NAME            "nickpanaccione"
    PLUGIN_MANUFACTURER_CODE Npan
    PLUGIN_CODE              Sv68
    FORMATS                  VST3 AU Standalone   # AU is ignored off-macOS
    PRODUCT_NAME             "Silver 68"
    IS_SYNTH                 FALSE
    NEEDS_MIDI_INPUT         FALSE
    NEEDS_MIDI_OUTPUT        FALSE
    COPY_PLUGIN_STEP_ENABLED TRUE
    COPY_PLUGIN_AFTER_BUILD  TRUE
    VST3_CATEGORIES          Fx Distortion
    AU_MAIN_TYPE             kAudioUnitType_Effect
)

juce_generate_juce_header(Silver68)

target_sources(Silver68 PRIVATE
    source/PluginProcessor.cpp
    source/PluginEditor.cpp
    source/dsp/ToneStack.cpp
)

target_include_directories(Silver68 PRIVATE source)

target_compile_definitions(Silver68 PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Silver68
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# tests (enabled by default when building top-level)
option(SILVER68_BUILD_TESTS "Build unit tests" ON)
if(SILVER68_BUILD_TESTS AND PROJECT_IS_TOP_LEVEL)
    enable_testing()
    add_subdirectory(tests)
endif()
