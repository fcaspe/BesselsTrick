# Example Audio Plugin CMakeLists.txt

# To get started on a new plugin, copy this entire folder (containing this file and C++ sources) to
# a convenient location, and then start making modifications.

# The first line of any CMake project should be a call to `cmake_minimum_required`, which checks
# that the installed CMake will be able to understand the following CMakeLists, and ensures that
# CMake's behaviour is compatible with the named version. This is a standard CMake command, so more
# information can be found in the CMake docs.

cmake_minimum_required(VERSION 3.14)
# The top-level CMakeLists.txt file for a project must contain a literal, direct call to the
# `project()` command. `project()` sets up some helpful variables that describe source/binary
# directories, and the current project version. This is a standard CMake command.

project(BesselsTrick VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0.1")  
  set (formats AU VST3)
else()
  set (formats VST3)
endif()

option(JUCE_BUILD_EXTRAS "Build JUCE Extras" OFF)

# If you've installed JUCE somehow (via a package manager, or directly using the CMake install
# target), you'll need to tell this project that it depends on the installed copy of JUCE. If you've
# included JUCE directly in your source tree (perhaps as a submodule), you'll need to tell CMake to
# include that subdirectory as part of the build.

# find_package(JUCE CONFIG REQUIRED)        # If you've installed JUCE to your system
# or
# add_subdirectory(JUCE)                    # If you've put JUCE in a subdirectory called JUCE

include(FetchContent)
FetchContent_Declare(
        juce
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 69795dc)

FetchContent_MakeAvailable(juce)

FetchContent_Declare(
        foleys_gui_magic
        GIT_REPOSITORY https://github.com/ffAudio/foleys_gui_magic
        GIT_TAG 795f879)

# Make the content available for the plugin, don't build it.
# We just add the module directory.
FetchContent_Populate(foleys_gui_magic)
add_subdirectory(${foleys_gui_magic_SOURCE_DIR}/modules)

##### Torch related
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(TorchUtils)
# build.sh leaves libtorch in ./libtorch/ - Fetch from there.
list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/libtorch")
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")


# Test RPATH 
set(CMAKE_MACOSX_RPATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE) # This is to use the use the local libraries in the rpath as well
# set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
     # This is the path to the folder containing the executable
    set(CMAKE_INSTALL_RPATH "@loader_path")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()


# Experimental: Compile with -fPIC for linking VST in Linux
if (UNIX AND NOT APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

# `juce_add_plugin` adds a static library target with the name passed as the first argument
# (BesselsTrick here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. As well as this shared code static library, this function adds targets for each of
# the formats specified by the FORMATS arguments. This function accepts many optional arguments.
# Check the readme at `docs/CMake API.md` in the JUCE repo for the full list.

juce_add_plugin(BesselsTrick
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    # IS_SYNTH TRUE/FALSE                       # Is this a synth or an effect?
    # NEEDS_MIDI_INPUT TRUE/FALSE               # Does the plugin need midi input?
    # NEEDS_MIDI_OUTPUT TRUE/FALSE              # Does the plugin need midi output?
    # IS_MIDI_EFFECT TRUE/FALSE                 # Is this plugin a MIDI effect?
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    # COPY_PLUGIN_AFTER_BUILD TRUE/FALSE        # Should the plugin be installed to a default location after building?
    COMPANY_NAME AIL                            # Specify the name of the plugin's author
    PLUGIN_MANUFACTURER_CODE Fran               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE FMTT                            # A unique four-character plugin id with at least one upper-case character
    FORMATS ${formats}                          # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "BesselsTrick")              # The name of the final executable, which can differ from the target name

# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.

juce_generate_juce_header(BesselsTrick)

# `target_sources` adds source files to a target. We pass the target that needs the sources as the
# first argument, then a visibility parameter for the sources (PRIVATE is normally best practice,
# although it doesn't really affect executable targets). Finally, we supply a list of source files
# that will be built into the target. This is a standard CMake command.

target_sources(BesselsTrick PRIVATE
    src/PluginProcessor.cpp
    src/PluginProcessorGUI.cpp
    src/PluginProcessorModelRoutines.cpp
    src/TorchInference.cpp
    src/EnvModels.cpp
    src/FMSynth.cpp
    src/RMSProcessor.cpp
    src/Yin.cpp
    src/GuiItems/DrawableLabel.cpp
    src/GuiItems/ModelComboBox.cpp
    src/GuiItems/RatiosBar.cpp
    src/GuiItems/StatusBar.cpp
    )

# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.

target_compile_definitions(BesselsTrick
    PRIVATE
    # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
    FOLEYS_ENABLE_BINARY_DATA=1
    JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
    JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
    JUCE_VST3_CAN_REPLACE_VST2=0)

# If your target needs extra binary assets, you can add them here. The first argument is the name of
# a new static library target that will include all the binary resources. There is an optional
# `NAMESPACE` argument that can specify the namespace of the generated binary data class. Finally,
# the SOURCES argument should be followed by a list of source files that should be built into the
# static library. These source files can be of any kind (wav data, images, fonts, icons etc.).
# Conversion to binary-data will happen when your target is built.

juce_add_binary_data(${PROJECT_NAME}_data
                    SOURCES
                    resources/magic_bake.xml
                    resources/img/algo_1.png  resources/img/algo_2.png  resources/img/algo_3.png  resources/img/algo_4.png
                    resources/img/algo_5.png  resources/img/algo_6.png  resources/img/algo_7.png  resources/img/algo_8.png
                    resources/img/algo_9.png  resources/img/algo_10.png resources/img/algo_11.png resources/img/algo_12.png
                    resources/img/algo_13.png resources/img/algo_14.png resources/img/algo_15.png resources/img/algo_16.png
                    resources/img/algo_17.png resources/img/algo_18.png resources/img/algo_19.png resources/img/algo_20.png
                    resources/img/algo_21.png resources/img/algo_22.png resources/img/algo_23.png resources/img/algo_24.png
                    resources/img/algo_25.png resources/img/algo_26.png resources/img/algo_27.png resources/img/algo_28.png
                    resources/img/algo_29.png resources/img/algo_30.png resources/img/algo_31.png resources/img/algo_32.png)

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_audio_utils` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.

target_link_libraries(BesselsTrick PUBLIC
    ${PROJECT_NAME}_data #Binary data
    foleys_gui_magic
    juce_audio_processors
    juce::juce_osc
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags    
    ${TORCH_LIBRARIES})


get_target_property(active_targets BesselsTrick JUCE_ACTIVE_PLUGIN_TARGETS)
foreach( sub_target IN LISTS active_targets )
    message(STATUS "Adding libraries to ${sub_target}")

    get_target_property(
        BUNDLE_PATH
        ${sub_target}
        JUCE_PLUGIN_ARTEFACT_FILE
    )
    copy_torch_libs(${sub_target})

endforeach()

juce_enable_copy_plugin_step(BesselsTrick)