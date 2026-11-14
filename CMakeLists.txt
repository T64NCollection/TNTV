## This file is a quick tutorial on writing CMakeLists for targeting the Vita
cmake_minimum_required(VERSION 3.16)

## This includes the Vita toolchain, must go before project definition
# It is a convenience so you do not have to type
# -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake for cmake. It is
# highly recommended that you include this block for all projects.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

## Define project parameters here
# Name of the project
project(new_tetris_vita)
# This line adds Vita helper macros, must go after project definition in order
# to build Vita specific artifacts (self/vpk).
include("${VITASDK}/share/vita.cmake" REQUIRED)

## Configuration options for this app
# Display name (under bubble in LiveArea)
set(VITA_APP_NAME "TNTV")
# Unique ID must be exactly 9 characters. Recommended: XXXXYYYYY where X =
# unique string of developer and Y = a unique number for this app
set(VITA_TITLEID  "NTET00001")
# Optional version string to show in LiveArea's more info screen
set(VITA_VERSION  "01.01")

## Flags and includes for building
# Note that we make sure not to overwrite previous flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# Test203D: with -fdata-sections / --gc-sections, Vita module's main-thread
# stack metadata would otherwise be discarded despite marked-used C source.
string(APPEND CMAKE_EXE_LINKER_FLAGS " -Wl,--undefined=sceUserMainThreadStackSize")
# TEST91: large embedded N64 textures leave little room after text.
# VitaSDK vita-elf-create needs ~1.8 KiB for SCE module metadata at
# the end of executable PT_LOAD segment 0. Without this data-segment
# spacing, four-player code builds as ELF but velf conversion fails.
# Preserve the exact assets and allow explicit linker overrides.
if(NOT CMAKE_EXE_LINKER_FLAGS MATCHES "section-start=.init_array")
  string(APPEND CMAKE_EXE_LINKER_FLAGS
         " -Wl,--section-start=.init_array=0x81900000")
endif()
# Optional. You can specify more param.sfo flags this way.
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")

# Add any additional include paths here
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/romloader
)

# Add any additional library paths here
# ${CMAKE_CURRENT_BINARY_DIR} lets you use any library currently being built
link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

## Build and link
add_executable(${PROJECT_NAME}
  src/main.c
  src/tntv_rom_bridge.c
  romloader/tntv_rom_reader.c
  romloader/tntv_rom_locator.c
  romloader/tntv_music_builder.c
  romloader/tntv_fx_reader.c
  romloader/tntv_img_reader.c
  romloader/tntv_ai_reader.c
  romloader/tntv_sky_synth.c
  romloader/tntv_cache_reader.c
  romloader/tntv_cache_builder.c
  romloader/third_party/crypto-algorithms/sha256.c
  romloader/third_party/lzokay/lzokay.cpp
  romloader/third_party/lzokay/lzokay-c/lzokay-c.cpp
  src/vita_input.c
  src/tnt_cont.c
  src/tnt_contq.c
  src/tnt_playervars.c
  src/tnt_026C80.c
  src/tnt_tetris.c
  src/tnt_currentpiece.c
  src/tnt_026900.c
  src/tnt_033310.c
  src/tnt_piecehold.c
  src/tnt_piecedefs.c
  src/tnt_color.c
  src/tnt_mobilepiece.c
  src/tnt_ghostpiece.c
  src/tnt_003E40.c
  src/tnt_ids.c
  src/tnt_square_analysis.c
  src/tnt_minos_gameplay.c
  src/tnt_pfgfx_layout.c
  src/tnt_cube.c
  src/tnt_cubetiles.c
  src/tnt_spotlight.c
  src/tnt_spotlight_data.c
  src/tnt_mobilecubes.c
  src/tnt_fallingcubes.c
  src/tnt_boardpieces_min.c
  src/tnt_keyspin.c
  src/tnt_frameact.c
  src/tnt_gamefinish.c
  src/tnt_gamefinish_data.c
  src/tnt_bag63.c
  src/tnt_nextpieces.c
  src/tnt_gamestats.c
  src/tnt_linescan.c
  src/tnt_lineeffect_vita.c
  src/tnt_multisquare_min.c
  src/tnt_multisquareglow.c
  src/tnt_square_glow_data.c
  src/tnt_font_c_data.c
  src/tnt_font_a_data.c
  src/tnt_font_b_data.c
  src/tnt_pause_vita.c
  src/tnt_mayan_static_data.c
  src/tnt_mayan_skybox_data.c
  src/tnt_mayan_fire_data.c
  src/tnt_mayan_large_fire.c
  src/tnt_title_data.c
  src/tnt_developer_data.c
  src/tnt_intro_data.c
  src/tnt_intro_anim.c
  src/tnt_main_menu_data.c
  src/tnt_one_player_menu_data.c
  src/tnt_options_data.c
  src/tnt_data_data.c
  src/tnt_scores_data.c
  src/tnt_credits_data.c
  src/tnt_tally_data.c
  src/tnt_tally_trophy_data.c
  src/tnt_haluci_vita.c
  src/tnt_wonder_vita.c
  src/tnt_wonder_data.c
  src/tnt_wonder_milestone_vita.c
  src/tnt_wonder_milestone_gxm.c
  src/tnt_wonder_milestone_data.c
  src/tnt_wonder_greek_data.c
  src/tnt_wonder_egyptian_data.c
  src/tnt_wonder_celtic_data.c
  src/tnt_wonder_african_data.c
  src/tnt_wonder_russian_camera_data.c
  src/tnt_greek_gameplay_data.c
  src/tnt_egyptian_gameplay_data.c
  src/tnt_celtic_gameplay_data.c
  src/tnt_celtic_gameplay_anim_data.c
  src/tnt_african_gameplay_data.c
  src/tnt_african_gameplay_anim_data.c
  src/tnt_japanese_gameplay_data.c
  src/tnt_japanese_gameplay_anim_data.c
  src/tnt_russian_gameplay_data.c
  src/tnt_russian_gameplay_anim_data.c
  src/tnt_finale_gameplay_data.c
  src/tnt_opponent_2p_data.c
  src/tnt_multiplayer_menu_data.c
  src/tnt_multiplayer_4p_data.c
  src/tnt_multiplayer_4p_cultures.c
  src/tnt_greek_gameplay_anim.c
  src/tnt_celtic_gameplay_anim.c
  src/tnt_african_gameplay_anim.c
  src/tnt_japanese_gameplay_anim.c
  src/tnt_russian_gameplay_anim.c
  src/tnt_wonder_complete_data.c
  src/tnt_wonder_complete_anim_data.c
  src/tnt_wonder_complete_anim.c
  src/tnt_wonder_complete_greek_data.c
  src/tnt_wonder_complete_greek_anim_data.c
  src/tnt_wonder_complete_greek_anim.c
  src/tnt_wonder_complete_egyptian_data.c
  src/tnt_wonder_complete_egyptian_anim_data.c
  src/tnt_wonder_complete_egyptian_anim.c
  src/tnt_wonder_complete_celtic_data.c
  src/tnt_wonder_complete_celtic_anim_data.c
  src/tnt_wonder_complete_celtic_anim.c
  src/tnt_wonder_complete_african_data.c
  src/tnt_wonder_complete_african_anim_data.c
  src/tnt_wonder_complete_african_anim.c
  src/tnt_wonder_complete_japanese_data.c
  src/tnt_wonder_complete_japanese_anim_data.c
  src/tnt_wonder_complete_japanese_anim.c
  src/tnt_wonder_complete_russian_data.c
  src/tnt_wonder_complete_russian_anim_data.c
  src/tnt_wonder_complete_russian_anim.c
  src/tnt_wonder_hall_vita.c
  src/tnt_wonder_hall_data.c
  src/tnt_sram_vita.c
  src/tnt_audio_vita.c
  src/tnt_menu_sfx_data.c
  src/tnt_game_sfx_data.c
  src/tnt_intro_sfx_data.c
  # The title track is derived from the user ROM, not embedded.
  src/tnt_rand.c
  src/tnt_ai_004220.c
  src/tnt_ai_005580.c
  src/tnt_ai_006780.c
  src/tnt_ai_006930.c
  src/tnt_ai_006BB0.c
  src/tnt_ai_008050.c
  src/tnt_ai_aisquarelist.c
  src/tnt_ai_0074E0_extra.c
  src/tnt_ai_aiplayer.c
  src/tnt_stubs.c
  src/tnt_garbage.c
  src/tnt_aimedat_state.c
  src/tnt_landfill.c
  third_party/libvita2d_shaders/texture_v_gxp.o
  third_party/libvita2d_shaders/texture_f_gxp.o
  third_party/libvita2d_shaders/color_v_gxp.o
  third_party/libvita2d_shaders/color_f_gxp.o
  third_party/tntv_shaders/texedge_f_gxp.o
)

set_source_files_properties(
  third_party/libvita2d_shaders/texture_v_gxp.o
  third_party/libvita2d_shaders/texture_f_gxp.o
  third_party/libvita2d_shaders/color_v_gxp.o
  third_party/libvita2d_shaders/color_f_gxp.o
  third_party/tntv_shaders/texedge_f_gxp.o
  PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE
)

# The Wonder construction renderers preserve source-derived fixed-point-style
# scene math; fast-math reduces hot reciprocal/transform cost without changing
# the authored geometry, timing, texture data, or state-machine behavior.
set_property(SOURCE src/tnt_wonder_milestone_vita.c src/tnt_wonder_milestone_gxm.c APPEND PROPERTY COMPILE_OPTIONS "-ffast-math")

# The completed-Wonder scene is substantially larger than the construction
# scenes (3320 triangles).  Its per-frame Borg pose evaluation, transform, and
# GXM vertex staging must be optimized or the Vita spends most of a frame in
# scalar C before submitting the scene.  Keep this local to the renderer and
# compact animation evaluator rather than changing whole-project codegen.
set_property(SOURCE src/tnt_wonder_milestone_gxm.c src/tnt_wonder_complete_anim.c src/tnt_wonder_complete_greek_anim.c src/tnt_wonder_complete_egyptian_anim.c src/tnt_wonder_complete_celtic_anim.c src/tnt_wonder_complete_african_anim.c src/tnt_wonder_complete_japanese_anim.c src/tnt_wonder_complete_russian_anim.c src/tnt_greek_gameplay_anim.c src/tnt_celtic_gameplay_anim.c src/tnt_african_gameplay_anim.c src/tnt_japanese_gameplay_anim.c src/tnt_russian_gameplay_anim.c src/tnt_intro_anim.c APPEND PROPERTY COMPILE_OPTIONS "-O2")

# The main translation unit owns the hot software presentation paths used by
# frontend menus and normal gameplay, including per-pixel moving-mino texture
# mapping and alpha blending. Keep strict FP semantics but compile these loops
# with normal optimization; the previous unoptimized build was CPU-bound.
set_property(SOURCE src/main.c APPEND PROPERTY COMPILE_OPTIONS "-O2")

# Frontend/pause text is software-rasterized with per-glyph alpha blending and
# coordinate scaling; multisquare glow is a 66x66 software alpha effect. Both
# are hot presentation paths and were previously left at -O0. Optimize their
# translation units without changing math semantics or authored timing.
set_property(SOURCE src/tnt_pause_vita.c src/tnt_multisquareglow.c APPEND PROPERTY COMPILE_OPTIONS "-O2")

# Every moving/metallic mino regenerates its 8x8 spotlight texture here.
# Keep exact arithmetic, but do not run this sampler unoptimized.
set_property(SOURCE src/tnt_spotlight.c APPEND PROPERTY COMPILE_OPTIONS "-O2")
# CubeTile surroundings morphs touch 128 signed gradient bytes per active cube.
# Keep the exact NEON compatibility loop optimized for 2P square formation.
set_property(SOURCE src/tnt_minos_gameplay.c APPEND PROPERTY COMPILE_OPTIONS "-O2")

# Mayan's large fire performs particle diffusion plus two software-scaled alpha
# composites every gameplay frame. It was still compiling at the project O0
# default; optimize this isolated renderer without changing its exact math.
set_property(SOURCE src/tnt_mayan_large_fire.c APPEND PROPERTY COMPILE_OPTIONS "-O2")

# The Hall trophy and locked-Wonder models rasterize on the CPU. Optimize
# their per-pixel loops with strict floating-point semantics; preserve the
# original animation rate, geometry, sampling, and lighting.
set_property(SOURCE src/tnt_wonder_hall_vita.c APPEND PROPERTY COMPILE_OPTIONS "-O2")

# Audio mixing is real-time and can exceed 20 simultaneous voices during line clears.
# Optimize only the mixer translation unit; keep exact math semantics (no fast-math).
set_property(SOURCE src/tnt_audio_vita.c APPEND PROPERTY COMPILE_OPTIONS "-O2")

# Library to link to (drop the -l prefix). This will mostly be stubs.
target_link_options(${PROJECT_NAME} PRIVATE -Wl,--gc-sections)

target_link_libraries(${PROJECT_NAME}
  m
  SceDisplay_stub
  SceCtrl_stub
  SceSysmem_stub
  SceGxm_stub
  SceIofilemgr_stub
  SceAudio_stub
  SceKernelThreadMgr_stub
  SceProcessmgr_stub
)

## Create Vita files
vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
# Local ROM loader uses MIT lzokay C++14.
set_property(SOURCE romloader/third_party/lzokay/lzokay.cpp
    APPEND PROPERTY COMPILE_OPTIONS "-std=c++14" "-Wno-maybe-uninitialized")
set_property(SOURCE romloader/third_party/lzokay/lzokay-c/lzokay-c.cpp
    APPEND PROPERTY COMPILE_OPTIONS "-std=c++14")

# The FILE directive lets you add additional files to the VPK, the syntax is
# FILE src_path dst_path_in_vpk. In this case, we add the LiveArea paths.
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE sce_sys/icon0.png sce_sys/icon0.png
  FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
  # User-ROM music is generated locally; NEVER package original audio.
)

# Compressed tokens are byte-aligned; prohibit synthesized unaligned ARM loads.
set_property(SOURCE romloader/third_party/lzokay/lzokay.cpp APPEND PROPERTY COMPILE_OPTIONS "-mno-unaligned-access")

# Startup-only optimization; no fast-math, game/render flags unchanged.
set_property(SOURCE
  src/tntv_rom_bridge.c
  romloader/tntv_img_reader.c
  romloader/tntv_music_builder.c
  romloader/tntv_cache_builder.c
  romloader/tntv_sky_synth.c
  romloader/tntv_ai_reader.c
  romloader/tntv_cache_reader.c
  romloader/tntv_rom_reader.c
  romloader/tntv_fx_reader.c
  romloader/third_party/crypto-algorithms/sha256.c
  romloader/third_party/lzokay/lzokay.cpp
  romloader/third_party/lzokay/lzokay-c/lzokay-c.cpp
 APPEND PROPERTY COMPILE_OPTIONS "-O2")
