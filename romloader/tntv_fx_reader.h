/* User-owned original N64 The New Tetris FX archive extraction.

 * The source-only parser contains no original game imagery/audio. It
 * converts data only from the user's verified NTSC USA 1.0 ROM.
 */
#ifndef TNTV_FX_READER_H
#define TNTV_FX_READER_H
#include "tntv_rom_reader.h"
/* FX type6 raw original animation events (no second-layer decompression). */
int tntv_fx_extract_animation(TNTVRom *rom,uint32_t resource_id,
                              uint8_t **out,uint32_t *outlen);
/* From FX type5 completed-Wonder scene: concatenate every referenced type1
 * 16bit RGBA texture's big-endian pixel values in canonical texture order.
 * The game cache's existing 16bit-endian bridge then restores native u16.
 */
int tntv_fx_extract_wonder_texels(TNTVRom *rom,uint32_t scene_id,
                                  uint8_t **out,uint32_t *outlen);
int tntv_fx_extract_scene_table(TNTVRom *rom,uint32_t scene_id,uint32_t kind,
                               uint8_t **out,uint32_t *outlen);
int tntv_fx_extract_scene_meshes(TNTVRom *rom,uint32_t scene_id,uint32_t colors,
                                uint8_t **out,uint32_t *outlen);
/* Bounded original type6 animation sampler, FX type5 base scene poses. */
int tntv_fx_sample_scene_animation(TNTVRom *rom,uint32_t scene_id,
                                    uint32_t animation_id,uint32_t first_tick,
                                    uint32_t sample_count,uint32_t first_node_only,
                                    uint8_t **out,uint32_t *outlen);
/* Strict original ROM static pointer-list/struct reader. No block data embedded. */
int tntv_rom_restore_african_blocks(TNTVRom *rom,
                                     uint8_t counts[7],void *blocks,
                                     uint32_t block_stride,uint32_t block_bytes);
/* Original Greek Wonder falling-block pointer metadata, no data literals. */
int tntv_rom_restore_greek_blocks(TNTVRom *rom,uint8_t counts[7],
                                   void *blocks,uint32_t stride,uint32_t capacity);
/* Original Celtic Wonder 7 stage static pointer lists, source ROM only. */
int tntv_rom_restore_celtic_blocks(TNTVRom *rom,uint8_t counts[7],
                                    void *blocks,uint32_t stride,uint32_t bytes);
/* Strict original Egyptian seven-stage falling block pointer parser. */
int tntv_rom_restore_egyptian_blocks(TNTVRom *rom,uint8_t counts[7],
                                      void *blocks,uint32_t stride,uint32_t bytes);
/* Original Mayan Wonder scene1046, 20 textures of 8x8 plus 4x4 mips
 * (80 16bit texels each), reconstructed from player's verified ROM. */
int tntv_fx_extract_mayan_wonder_mips(TNTVRom *rom,uint8_t **out,
                                       uint32_t *outlen);
/* Original Mayan construction N64 48-byte static block pointers, no payload bytes. */
int tntv_rom_restore_mayan_milestone_blocks(TNTVRom *rom,uint8_t counts[7],
                                             void *blocks,uint32_t stride,uint32_t bytes);
/* Original N64 intro FX985 scene hierarchy/camera/fog/lights, no source asset payload. */
int tntv_fx_extract_intro_basics(TNTVRom *rom,uint32_t kind,
                                  uint8_t **out,uint32_t *outlen);
/* Original 37 intro material RGBA16 mip source images and tile0 sampler descriptors. */
int tntv_fx_extract_intro_textures(TNTVRom *rom,uint32_t kind,
                                   uint8_t **out,uint32_t *outlen);
/* User-ROM original intro RSP geometry/draw stream, no embedded vertex bytes. */
int tntv_fx_extract_intro_geometry(TNTVRom *rom,uint32_t kind,
                                   uint8_t **out,uint32_t *outlen);
/* Original intro type6 FX animation986, source-only key/track/target records. */
int tntv_fx_extract_intro_animation(TNTVRom *rom,uint32_t kind,
                                    uint8_t **out,uint32_t *outlen);
#endif
