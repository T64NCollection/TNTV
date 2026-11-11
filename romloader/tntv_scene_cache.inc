/* Local native scene cache. NEVER distribute this generated cache.
 * Stores byte arrays only, never runtime pointers. Recipe ID must change
 * whenever scene reconstruction, sky synthesis or native ABI changes. */
typedef struct { const char *name; void *data; size_t size; } SceneCacheBinding;
static const SceneCacheBinding s_scene_cache[] = {
    {"g_tnt_wonder_complete_nodes",g_tnt_wonder_complete_nodes,sizeof(g_tnt_wonder_complete_nodes)},
    {"g_tnt_wonder_complete_node_type",g_tnt_wonder_complete_node_type,sizeof(g_tnt_wonder_complete_node_type)},
    {"g_tnt_wonder_complete_parents",g_tnt_wonder_complete_parents,sizeof(g_tnt_wonder_complete_parents)},
    {"g_tnt_wonder_complete_node_mesh",g_tnt_wonder_complete_node_mesh,sizeof(g_tnt_wonder_complete_node_mesh)},
    {"g_tnt_wonder_complete_mesh_scale",g_tnt_wonder_complete_mesh_scale,sizeof(g_tnt_wonder_complete_mesh_scale)},
    {"g_tnt_wonder_complete_textures",g_tnt_wonder_complete_textures,sizeof(g_tnt_wonder_complete_textures)},
    {"g_tnt_greek_complete_nodes",g_tnt_greek_complete_nodes,sizeof(g_tnt_greek_complete_nodes)},
    {"g_tnt_greek_complete_node_type",g_tnt_greek_complete_node_type,sizeof(g_tnt_greek_complete_node_type)},
    {"g_tnt_greek_complete_parents",g_tnt_greek_complete_parents,sizeof(g_tnt_greek_complete_parents)},
    {"g_tnt_greek_complete_node_mesh",g_tnt_greek_complete_node_mesh,sizeof(g_tnt_greek_complete_node_mesh)},
    {"g_tnt_greek_complete_mesh_scale",g_tnt_greek_complete_mesh_scale,sizeof(g_tnt_greek_complete_mesh_scale)},
    {"g_tnt_greek_complete_textures",g_tnt_greek_complete_textures,sizeof(g_tnt_greek_complete_textures)},
    {"g_tnt_egyptian_complete_nodes",g_tnt_egyptian_complete_nodes,sizeof(g_tnt_egyptian_complete_nodes)},
    {"g_tnt_egyptian_complete_node_type",g_tnt_egyptian_complete_node_type,sizeof(g_tnt_egyptian_complete_node_type)},
    {"g_tnt_egyptian_complete_parents",g_tnt_egyptian_complete_parents,sizeof(g_tnt_egyptian_complete_parents)},
    {"g_tnt_egyptian_complete_node_mesh",g_tnt_egyptian_complete_node_mesh,sizeof(g_tnt_egyptian_complete_node_mesh)},
    {"g_tnt_egyptian_complete_mesh_scale",g_tnt_egyptian_complete_mesh_scale,sizeof(g_tnt_egyptian_complete_mesh_scale)},
    {"g_tnt_egyptian_complete_textures",g_tnt_egyptian_complete_textures,sizeof(g_tnt_egyptian_complete_textures)},
    {"g_tnt_celtic_complete_nodes",g_tnt_celtic_complete_nodes,sizeof(g_tnt_celtic_complete_nodes)},
    {"g_tnt_celtic_complete_node_type",g_tnt_celtic_complete_node_type,sizeof(g_tnt_celtic_complete_node_type)},
    {"g_tnt_celtic_complete_parents",g_tnt_celtic_complete_parents,sizeof(g_tnt_celtic_complete_parents)},
    {"g_tnt_celtic_complete_node_mesh",g_tnt_celtic_complete_node_mesh,sizeof(g_tnt_celtic_complete_node_mesh)},
    {"g_tnt_celtic_complete_mesh_scale",g_tnt_celtic_complete_mesh_scale,sizeof(g_tnt_celtic_complete_mesh_scale)},
    {"g_tnt_celtic_complete_textures",g_tnt_celtic_complete_textures,sizeof(g_tnt_celtic_complete_textures)},
    {"g_tnt_african_complete_nodes",g_tnt_african_complete_nodes,sizeof(g_tnt_african_complete_nodes)},
    {"g_tnt_african_complete_node_type",g_tnt_african_complete_node_type,sizeof(g_tnt_african_complete_node_type)},
    {"g_tnt_african_complete_parents",g_tnt_african_complete_parents,sizeof(g_tnt_african_complete_parents)},
    {"g_tnt_african_complete_node_mesh",g_tnt_african_complete_node_mesh,sizeof(g_tnt_african_complete_node_mesh)},
    {"g_tnt_african_complete_mesh_scale",g_tnt_african_complete_mesh_scale,sizeof(g_tnt_african_complete_mesh_scale)},
    {"g_tnt_african_complete_textures",g_tnt_african_complete_textures,sizeof(g_tnt_african_complete_textures)},
    {"g_tnt_japanese_complete_nodes",g_tnt_japanese_complete_nodes,sizeof(g_tnt_japanese_complete_nodes)},
    {"g_tnt_japanese_complete_node_type",g_tnt_japanese_complete_node_type,sizeof(g_tnt_japanese_complete_node_type)},
    {"g_tnt_japanese_complete_parents",g_tnt_japanese_complete_parents,sizeof(g_tnt_japanese_complete_parents)},
    {"g_tnt_japanese_complete_node_mesh",g_tnt_japanese_complete_node_mesh,sizeof(g_tnt_japanese_complete_node_mesh)},
    {"g_tnt_japanese_complete_mesh_scale",g_tnt_japanese_complete_mesh_scale,sizeof(g_tnt_japanese_complete_mesh_scale)},
    {"g_tnt_japanese_complete_textures",g_tnt_japanese_complete_textures,sizeof(g_tnt_japanese_complete_textures)},
    {"g_tnt_russian_complete_nodes",g_tnt_russian_complete_nodes,sizeof(g_tnt_russian_complete_nodes)},
    {"g_tnt_russian_complete_node_type",g_tnt_russian_complete_node_type,sizeof(g_tnt_russian_complete_node_type)},
    {"g_tnt_russian_complete_parents",g_tnt_russian_complete_parents,sizeof(g_tnt_russian_complete_parents)},
    {"g_tnt_russian_complete_node_mesh",g_tnt_russian_complete_node_mesh,sizeof(g_tnt_russian_complete_node_mesh)},
    {"g_tnt_russian_complete_mesh_scale",g_tnt_russian_complete_mesh_scale,sizeof(g_tnt_russian_complete_mesh_scale)},
    {"g_tnt_russian_complete_textures",g_tnt_russian_complete_textures,sizeof(g_tnt_russian_complete_textures)},
    {"g_tnt_greek_complete_triangles",g_tnt_greek_complete_triangles,sizeof(g_tnt_greek_complete_triangles)},
    {"g_tnt_egyptian_complete_triangles",g_tnt_egyptian_complete_triangles,sizeof(g_tnt_egyptian_complete_triangles)},
    {"g_tnt_egyptian_complete_untextured_colors",g_tnt_egyptian_complete_untextured_colors,sizeof(g_tnt_egyptian_complete_untextured_colors)},
    {"g_tnt_celtic_complete_triangles",g_tnt_celtic_complete_triangles,sizeof(g_tnt_celtic_complete_triangles)},
    {"g_tnt_celtic_complete_untextured_colors",g_tnt_celtic_complete_untextured_colors,sizeof(g_tnt_celtic_complete_untextured_colors)},
    {"g_tnt_african_complete_triangles",g_tnt_african_complete_triangles,sizeof(g_tnt_african_complete_triangles)},
    {"g_tnt_african_complete_untextured_colors",g_tnt_african_complete_untextured_colors,sizeof(g_tnt_african_complete_untextured_colors)},
    {"g_tnt_japanese_complete_triangles",g_tnt_japanese_complete_triangles,sizeof(g_tnt_japanese_complete_triangles)},
    {"g_tnt_japanese_complete_untextured_colors",g_tnt_japanese_complete_untextured_colors,sizeof(g_tnt_japanese_complete_untextured_colors)},
    {"g_tnt_russian_complete_triangles",g_tnt_russian_complete_triangles,sizeof(g_tnt_russian_complete_triangles)},
    {"g_tnt_russian_complete_untextured_colors",g_tnt_russian_complete_untextured_colors,sizeof(g_tnt_russian_complete_untextured_colors)},
    {"g_tnt_wonder_complete_triangles",g_tnt_wonder_complete_triangles,sizeof(g_tnt_wonder_complete_triangles)},
    {"g_tnt_african_sky_nodes",g_tnt_african_sky_nodes,sizeof(g_tnt_african_sky_nodes)},
    {"g_tnt_african_sky_node_type",g_tnt_african_sky_node_type,sizeof(g_tnt_african_sky_node_type)},
    {"g_tnt_african_sky_parents",g_tnt_african_sky_parents,sizeof(g_tnt_african_sky_parents)},
    {"g_tnt_african_sky_node_mesh",g_tnt_african_sky_node_mesh,sizeof(g_tnt_african_sky_node_mesh)},
    {"g_tnt_african_sky_mesh_scale",g_tnt_african_sky_mesh_scale,sizeof(g_tnt_african_sky_mesh_scale)},
    {"g_tnt_african_sky_textures",g_tnt_african_sky_textures,sizeof(g_tnt_african_sky_textures)},
    {"g_tnt_african_sky_triangles",g_tnt_african_sky_triangles,sizeof(g_tnt_african_sky_triangles)},
    {"g_tnt_african_sky_triangle_alpha",g_tnt_african_sky_triangle_alpha,sizeof(g_tnt_african_sky_triangle_alpha)},
    {"g_tnt_celtic_sky_nodes",g_tnt_celtic_sky_nodes,sizeof(g_tnt_celtic_sky_nodes)},
    {"g_tnt_celtic_sky_node_type",g_tnt_celtic_sky_node_type,sizeof(g_tnt_celtic_sky_node_type)},
    {"g_tnt_celtic_sky_parents",g_tnt_celtic_sky_parents,sizeof(g_tnt_celtic_sky_parents)},
    {"g_tnt_celtic_sky_node_mesh",g_tnt_celtic_sky_node_mesh,sizeof(g_tnt_celtic_sky_node_mesh)},
    {"g_tnt_celtic_sky_mesh_scale",g_tnt_celtic_sky_mesh_scale,sizeof(g_tnt_celtic_sky_mesh_scale)},
    {"g_tnt_celtic_sky_textures",g_tnt_celtic_sky_textures,sizeof(g_tnt_celtic_sky_textures)},
    {"g_tnt_celtic_sky_triangles",g_tnt_celtic_sky_triangles,sizeof(g_tnt_celtic_sky_triangles)},
    {"g_tnt_celtic_sky_alpha_class",g_tnt_celtic_sky_alpha_class,sizeof(g_tnt_celtic_sky_alpha_class)},
    {"g_tnt_celtic_sky_untextured",g_tnt_celtic_sky_untextured,sizeof(g_tnt_celtic_sky_untextured)},
    {"g_tnt_celtic_leaf_nodes",g_tnt_celtic_leaf_nodes,sizeof(g_tnt_celtic_leaf_nodes)},
    {"g_tnt_celtic_leaf_node_type",g_tnt_celtic_leaf_node_type,sizeof(g_tnt_celtic_leaf_node_type)},
    {"g_tnt_celtic_leaf_parents",g_tnt_celtic_leaf_parents,sizeof(g_tnt_celtic_leaf_parents)},
    {"g_tnt_celtic_leaf_node_mesh",g_tnt_celtic_leaf_node_mesh,sizeof(g_tnt_celtic_leaf_node_mesh)},
    {"g_tnt_celtic_leaf_mesh_scale",g_tnt_celtic_leaf_mesh_scale,sizeof(g_tnt_celtic_leaf_mesh_scale)},
    {"g_tnt_celtic_leaf_triangles",g_tnt_celtic_leaf_triangles,sizeof(g_tnt_celtic_leaf_triangles)},
    {"g_tnt_japanese_sky_nodes",g_tnt_japanese_sky_nodes,sizeof(g_tnt_japanese_sky_nodes)},
    {"g_tnt_japanese_sky_node_type",g_tnt_japanese_sky_node_type,sizeof(g_tnt_japanese_sky_node_type)},
    {"g_tnt_japanese_sky_parents",g_tnt_japanese_sky_parents,sizeof(g_tnt_japanese_sky_parents)},
    {"g_tnt_japanese_sky_node_mesh",g_tnt_japanese_sky_node_mesh,sizeof(g_tnt_japanese_sky_node_mesh)},
    {"g_tnt_japanese_sky_mesh_scale",g_tnt_japanese_sky_mesh_scale,sizeof(g_tnt_japanese_sky_mesh_scale)},
    {"g_tnt_japanese_sky_textures",g_tnt_japanese_sky_textures,sizeof(g_tnt_japanese_sky_textures)},
    {"g_tnt_japanese_sky_triangles",g_tnt_japanese_sky_triangles,sizeof(g_tnt_japanese_sky_triangles)},
    {"g_tnt_japanese_butterfly_nodes",g_tnt_japanese_butterfly_nodes,sizeof(g_tnt_japanese_butterfly_nodes)},
    {"g_tnt_japanese_butterfly_node_type",g_tnt_japanese_butterfly_node_type,sizeof(g_tnt_japanese_butterfly_node_type)},
    {"g_tnt_japanese_butterfly_parents",g_tnt_japanese_butterfly_parents,sizeof(g_tnt_japanese_butterfly_parents)},
    {"g_tnt_japanese_butterfly_node_mesh",g_tnt_japanese_butterfly_node_mesh,sizeof(g_tnt_japanese_butterfly_node_mesh)},
    {"g_tnt_japanese_butterfly_mesh_scale",g_tnt_japanese_butterfly_mesh_scale,sizeof(g_tnt_japanese_butterfly_mesh_scale)},
    {"g_tnt_japanese_butterfly_triangles",g_tnt_japanese_butterfly_triangles,sizeof(g_tnt_japanese_butterfly_triangles)},
    {"g_tnt_russian_sky_nodes",g_tnt_russian_sky_nodes,sizeof(g_tnt_russian_sky_nodes)},
    {"g_tnt_russian_sky_node_type",g_tnt_russian_sky_node_type,sizeof(g_tnt_russian_sky_node_type)},
    {"g_tnt_russian_sky_parents",g_tnt_russian_sky_parents,sizeof(g_tnt_russian_sky_parents)},
    {"g_tnt_russian_sky_node_mesh",g_tnt_russian_sky_node_mesh,sizeof(g_tnt_russian_sky_node_mesh)},
    {"g_tnt_russian_sky_mesh_scale",g_tnt_russian_sky_mesh_scale,sizeof(g_tnt_russian_sky_mesh_scale)},
    {"g_tnt_russian_sky_textures",g_tnt_russian_sky_textures,sizeof(g_tnt_russian_sky_textures)},
    {"g_tnt_russian_sky_texture_texels",g_tnt_russian_sky_texture_texels,sizeof(g_tnt_russian_sky_texture_texels)},
    {"g_tnt_russian_sky_triangles",g_tnt_russian_sky_triangles,sizeof(g_tnt_russian_sky_triangles)},
    {"g_tnt_russian_sky_triangle_alpha",g_tnt_russian_sky_triangle_alpha,sizeof(g_tnt_russian_sky_triangle_alpha)},
    {"g_tnt_russian_snow_nodes",g_tnt_russian_snow_nodes,sizeof(g_tnt_russian_snow_nodes)},
    {"g_tnt_russian_snow_node_type",g_tnt_russian_snow_node_type,sizeof(g_tnt_russian_snow_node_type)},
    {"g_tnt_russian_snow_parents",g_tnt_russian_snow_parents,sizeof(g_tnt_russian_snow_parents)},
    {"g_tnt_russian_snow_node_mesh",g_tnt_russian_snow_node_mesh,sizeof(g_tnt_russian_snow_node_mesh)},
    {"g_tnt_russian_snow_mesh_scale",g_tnt_russian_snow_mesh_scale,sizeof(g_tnt_russian_snow_mesh_scale)},
    {"g_tnt_russian_snow_triangles",g_tnt_russian_snow_triangles,sizeof(g_tnt_russian_snow_triangles)},
    {"g_tnt_russian_snow_lit",g_tnt_russian_snow_lit,sizeof(g_tnt_russian_snow_lit)},
    {"g_tnt_mayan_4p_background_packbits",g_tnt_mayan_4p_background_packbits,sizeof(g_tnt_mayan_4p_background_packbits)},
    {"g_tnt_greek_rain_nodes",g_tnt_greek_rain_nodes,sizeof(g_tnt_greek_rain_nodes)},
    {"g_tnt_greek_rain_texture",g_tnt_greek_rain_texture,sizeof(g_tnt_greek_rain_texture)},
    {"g_tnt_greek_rain_triangles",g_tnt_greek_rain_triangles,sizeof(g_tnt_greek_rain_triangles)},
    {"g_tnt_greek_fire_ci8",g_tnt_greek_fire_ci8,sizeof(g_tnt_greek_fire_ci8)},
    {"g_tnt_greek_fire_rgba16",g_tnt_greek_fire_rgba16,sizeof(g_tnt_greek_fire_rgba16)},
    {"g_tnt_egyptian_coal_ci8",g_tnt_egyptian_coal_ci8,sizeof(g_tnt_egyptian_coal_ci8)},
    {"g_tnt_egyptian_coal_rgba16",g_tnt_egyptian_coal_rgba16,sizeof(g_tnt_egyptian_coal_rgba16)},
    {"g_tnt_egyptian_candle_a_ci8",g_tnt_egyptian_candle_a_ci8,sizeof(g_tnt_egyptian_candle_a_ci8)},
    {"g_tnt_egyptian_candle_a_rgba16",g_tnt_egyptian_candle_a_rgba16,sizeof(g_tnt_egyptian_candle_a_rgba16)},
    {"g_tnt_egyptian_candle_b_ci8",g_tnt_egyptian_candle_b_ci8,sizeof(g_tnt_egyptian_candle_b_ci8)},
    {"g_tnt_egyptian_candle_b_rgba16",g_tnt_egyptian_candle_b_rgba16,sizeof(g_tnt_egyptian_candle_b_rgba16)},
    {"g_tnt_mayan_fire_ci8",g_tnt_mayan_fire_ci8,sizeof(g_tnt_mayan_fire_ci8)},
    {"g_tnt_mayan_fire_rgba16",g_tnt_mayan_fire_rgba16,sizeof(g_tnt_mayan_fire_rgba16)},
    {"g_tnt_greek_skybox_nodes",g_tnt_greek_skybox_nodes,sizeof(g_tnt_greek_skybox_nodes)},
    {"g_tnt_greek_skybox_textures",g_tnt_greek_skybox_textures,sizeof(g_tnt_greek_skybox_textures)},
    {"g_tnt_greek_skybox_triangles",g_tnt_greek_skybox_triangles,sizeof(g_tnt_greek_skybox_triangles)},
    {"g_tnt_mayan_skybox_textures",g_tnt_mayan_skybox_textures,sizeof(g_tnt_mayan_skybox_textures)},
    {"g_tnt_mayan_skybox_triangles",g_tnt_mayan_skybox_triangles,sizeof(g_tnt_mayan_skybox_triangles)},
    {"g_tnt_wonder_finale_hall_nodes",g_tnt_wonder_finale_hall_nodes,sizeof(g_tnt_wonder_finale_hall_nodes)},
    {"g_tnt_wonder_finale_hall_parents",g_tnt_wonder_finale_hall_parents,sizeof(g_tnt_wonder_finale_hall_parents)},
    {"g_tnt_wonder_finale_hall_mesh_scale",g_tnt_wonder_finale_hall_mesh_scale,sizeof(g_tnt_wonder_finale_hall_mesh_scale)},
    {"g_tnt_wonder_finale_hall_texture",g_tnt_wonder_finale_hall_texture,sizeof(g_tnt_wonder_finale_hall_texture)},
    {"g_tnt_wonder_finale_hall_triangles",g_tnt_wonder_finale_hall_triangles,sizeof(g_tnt_wonder_finale_hall_triangles)},
    {"g_tnt_wonder_locked_nodes",g_tnt_wonder_locked_nodes,sizeof(g_tnt_wonder_locked_nodes)},
    {"g_tnt_wonder_locked_texture",g_tnt_wonder_locked_texture,sizeof(g_tnt_wonder_locked_texture)},
    {"g_tnt_wonder_locked_triangles",g_tnt_wonder_locked_triangles,sizeof(g_tnt_wonder_locked_triangles)},
    {"g_tnt_greek_scene1125_nodes",g_tnt_greek_scene1125_nodes,sizeof(g_tnt_greek_scene1125_nodes)},
    {"g_tnt_greek_scene1125_triangles",g_tnt_greek_scene1125_triangles,sizeof(g_tnt_greek_scene1125_triangles)},
    {"g_tnt_african_scene1292_nodes",g_tnt_african_scene1292_nodes,sizeof(g_tnt_african_scene1292_nodes)},
    {"g_tnt_african_milestone_textures",g_tnt_african_milestone_textures,sizeof(g_tnt_african_milestone_textures)},
    {"g_tnt_african_scene1292_triangles",g_tnt_african_scene1292_triangles,sizeof(g_tnt_african_scene1292_triangles)},
    {"g_tnt_african_scene1292_stage_nodes",g_tnt_african_scene1292_stage_nodes,sizeof(g_tnt_african_scene1292_stage_nodes)},
    {"g_tnt_african_milestone_camera",g_tnt_african_milestone_camera,sizeof(g_tnt_african_milestone_camera)},
    {"g_tnt_african_milestone_texture_wrap",g_tnt_african_milestone_texture_wrap,sizeof(g_tnt_african_milestone_texture_wrap)},
    {"g_tnt_african_milestone_block_count",g_tnt_african_milestone_block_count,sizeof(g_tnt_african_milestone_block_count)},
    {"g_tnt_african_milestone_blocks",g_tnt_african_milestone_blocks,sizeof(g_tnt_african_milestone_blocks)},
    {"g_tnt_russian_milestone_stage_camera",g_tnt_russian_milestone_stage_camera,sizeof(g_tnt_russian_milestone_stage_camera)},
    {"g_tnt_greek_camera_xz",g_tnt_greek_camera_xz,sizeof(g_tnt_greek_camera_xz)},
    {"g_tnt_greek_milestone_block_count",g_tnt_greek_milestone_block_count,sizeof(g_tnt_greek_milestone_block_count)},
    {"g_tnt_greek_milestone_blocks",g_tnt_greek_milestone_blocks,sizeof(g_tnt_greek_milestone_blocks)},
    {"g_tnt_celtic_scene1244_nodes",g_tnt_celtic_scene1244_nodes,sizeof(g_tnt_celtic_scene1244_nodes)},
    {"g_tnt_celtic_milestone_textures",g_tnt_celtic_milestone_textures,sizeof(g_tnt_celtic_milestone_textures)},
    {"g_tnt_celtic_scene1244_triangles",g_tnt_celtic_scene1244_triangles,sizeof(g_tnt_celtic_scene1244_triangles)},
    {"g_tnt_celtic_scene1244_stage_nodes",g_tnt_celtic_scene1244_stage_nodes,sizeof(g_tnt_celtic_scene1244_stage_nodes)},
    {"g_tnt_celtic_milestone_camera",g_tnt_celtic_milestone_camera,sizeof(g_tnt_celtic_milestone_camera)},
    {"g_tnt_celtic_milestone_texture_wrap",g_tnt_celtic_milestone_texture_wrap,sizeof(g_tnt_celtic_milestone_texture_wrap)},
    {"g_tnt_celtic_milestone_block_count",g_tnt_celtic_milestone_block_count,sizeof(g_tnt_celtic_milestone_block_count)},
    {"g_tnt_celtic_milestone_blocks",g_tnt_celtic_milestone_blocks,sizeof(g_tnt_celtic_milestone_blocks)},
    {"g_tnt_egyptian_scene1148_nodes",g_tnt_egyptian_scene1148_nodes,sizeof(g_tnt_egyptian_scene1148_nodes)},
    {"g_tnt_egyptian_milestone_textures",g_tnt_egyptian_milestone_textures,sizeof(g_tnt_egyptian_milestone_textures)},
    {"g_tnt_egyptian_scene1148_triangles",g_tnt_egyptian_scene1148_triangles,sizeof(g_tnt_egyptian_scene1148_triangles)},
    {"g_tnt_egyptian_milestone_texture_wrap",g_tnt_egyptian_milestone_texture_wrap,sizeof(g_tnt_egyptian_milestone_texture_wrap)},
    {"g_tnt_egyptian_milestone_block_count",g_tnt_egyptian_milestone_block_count,sizeof(g_tnt_egyptian_milestone_block_count)},
    {"g_tnt_egyptian_milestone_blocks",g_tnt_egyptian_milestone_blocks,sizeof(g_tnt_egyptian_milestone_blocks)},
    {"g_tnt_wonder_mayan_nodes",g_tnt_wonder_mayan_nodes,sizeof(g_tnt_wonder_mayan_nodes)},
    {"g_tnt_wonder_mayan_textures",g_tnt_wonder_mayan_textures,sizeof(g_tnt_wonder_mayan_textures)},
    {"g_tnt_wonder_mayan_triangles",g_tnt_wonder_mayan_triangles,sizeof(g_tnt_wonder_mayan_triangles)},
    {"g_tnt_milestone_scene1006_nodes",g_tnt_milestone_scene1006_nodes,sizeof(g_tnt_milestone_scene1006_nodes)},
    {"g_tnt_milestone_scene1088_nodes",g_tnt_milestone_scene1088_nodes,sizeof(g_tnt_milestone_scene1088_nodes)},
    {"g_tnt_milestone_scene1104_nodes",g_tnt_milestone_scene1104_nodes,sizeof(g_tnt_milestone_scene1104_nodes)},
    {"g_tnt_milestone_scene1057_nodes",g_tnt_milestone_scene1057_nodes,sizeof(g_tnt_milestone_scene1057_nodes)},
    {"g_tnt_milestone_large_textures",g_tnt_milestone_large_textures,sizeof(g_tnt_milestone_large_textures)},
    {"g_tnt_milestone_large_texture_wrap",g_tnt_milestone_large_texture_wrap,sizeof(g_tnt_milestone_large_texture_wrap)},
    {"g_tnt_milestone_scene1006_triangles",g_tnt_milestone_scene1006_triangles,sizeof(g_tnt_milestone_scene1006_triangles)},
    {"g_tnt_milestone_scene1088_triangles",g_tnt_milestone_scene1088_triangles,sizeof(g_tnt_milestone_scene1088_triangles)},
    {"g_tnt_milestone_scene1104_triangles",g_tnt_milestone_scene1104_triangles,sizeof(g_tnt_milestone_scene1104_triangles)},
    {"g_tnt_milestone_scene1057_triangles",g_tnt_milestone_scene1057_triangles,sizeof(g_tnt_milestone_scene1057_triangles)},
    {"g_tnt_mayan_milestone_block_count",g_tnt_mayan_milestone_block_count,sizeof(g_tnt_mayan_milestone_block_count)},
    {"g_tnt_mayan_milestone_blocks",g_tnt_mayan_milestone_blocks,sizeof(g_tnt_mayan_milestone_blocks)},
    {"g_tnt_milestone_block_y",g_tnt_milestone_block_y,sizeof(g_tnt_milestone_block_y)},
    {"g_tnt_milestone_camera_xz",g_tnt_milestone_camera_xz,sizeof(g_tnt_milestone_camera_xz)},
    {"g_tnt_intro_nodes",g_tnt_intro_nodes,sizeof(g_tnt_intro_nodes)},
    {"g_tnt_intro_fog_initial",g_tnt_intro_fog_initial,sizeof(g_tnt_intro_fog_initial)},
    {"g_tnt_intro_camera_initial",g_tnt_intro_camera_initial,sizeof(g_tnt_intro_camera_initial)},
    {"g_tnt_intro_lights_initial",g_tnt_intro_lights_initial,sizeof(g_tnt_intro_lights_initial)},
    {"g_tnt_intro_textures",g_tnt_intro_textures,sizeof(g_tnt_intro_textures)},
    {"g_tnt_intro_texture_rgba16",g_tnt_intro_texture_rgba16,sizeof(g_tnt_intro_texture_rgba16)},
    {"g_tnt_intro_vertices",g_tnt_intro_vertices,sizeof(g_tnt_intro_vertices)},
    {"g_tnt_intro_draws",g_tnt_intro_draws,sizeof(g_tnt_intro_draws)},
    {"g_tnt_intro_keys",g_tnt_intro_keys,sizeof(g_tnt_intro_keys)},
    {"g_tnt_intro_tracks",g_tnt_intro_tracks,sizeof(g_tnt_intro_tracks)},
    {"g_tnt_intro_targets",g_tnt_intro_targets,sizeof(g_tnt_intro_targets)},
    {"g_tnt_credits_sintable",g_tnt_credits_sintable,sizeof(g_tnt_credits_sintable)},
    {"g_tnt_credits_font_is_b",g_tnt_credits_font_is_b,sizeof(g_tnt_credits_font_is_b)},
    {"g_tnt_font_a_char_widths",g_tnt_font_a_char_widths,sizeof(g_tnt_font_a_char_widths)},
    {"g_tnt_font_b_char_widths",g_tnt_font_b_char_widths,sizeof(g_tnt_font_b_char_widths)},
    {"g_tnt_game_dcm_sample_map",g_tnt_game_dcm_sample_map,sizeof(g_tnt_game_dcm_sample_map)},
    {"g_tnt_game_sample_flags",g_tnt_game_sample_flags,sizeof(g_tnt_game_sample_flags)},
    {"g_tnt_game_sample_lengths",g_tnt_game_sample_lengths,sizeof(g_tnt_game_sample_lengths)},
    {"g_tnt_game_sample_loop_begin",g_tnt_game_sample_loop_begin,sizeof(g_tnt_game_sample_loop_begin)},
    {"g_tnt_game_sample_loop_end",g_tnt_game_sample_loop_end,sizeof(g_tnt_game_sample_loop_end)},
    {"g_tnt_game_sfx_stream_lengths",g_tnt_game_sfx_stream_lengths,sizeof(g_tnt_game_sfx_stream_lengths)},
    {"g_tnt_intro_dcm_sample_map",g_tnt_intro_dcm_sample_map,sizeof(g_tnt_intro_dcm_sample_map)},
    {"g_tnt_intro_sample_flags",g_tnt_intro_sample_flags,sizeof(g_tnt_intro_sample_flags)},
    {"g_tnt_intro_sample_lengths",g_tnt_intro_sample_lengths,sizeof(g_tnt_intro_sample_lengths)},
    {"g_tnt_intro_sample_loop_begin",g_tnt_intro_sample_loop_begin,sizeof(g_tnt_intro_sample_loop_begin)},
    {"g_tnt_intro_sample_loop_end",g_tnt_intro_sample_loop_end,sizeof(g_tnt_intro_sample_loop_end)},
    {"g_tnt_intro_sfx_stream_lengths",g_tnt_intro_sfx_stream_lengths,sizeof(g_tnt_intro_sfx_stream_lengths)},
    {"g_tnt_menu_sample_lengths",g_tnt_menu_sample_lengths,sizeof(g_tnt_menu_sample_lengths)},
    {"g_tnt_menu_sfx_stream_lengths",g_tnt_menu_sfx_stream_lengths,sizeof(g_tnt_menu_sfx_stream_lengths)},
    {"g_tnt_egyptian_4p_packbits",g_tnt_egyptian_4p_packbits,sizeof(g_tnt_egyptian_4p_packbits)},
    {"g_tnt_egyptian_4p_rgba16",g_tnt_egyptian_4p_rgba16,sizeof(g_tnt_egyptian_4p_rgba16)},
    {"g_tnt_japanese_4p_packbits",g_tnt_japanese_4p_packbits,sizeof(g_tnt_japanese_4p_packbits)},
    {"g_tnt_japanese_4p_rgba16",g_tnt_japanese_4p_rgba16,sizeof(g_tnt_japanese_4p_rgba16)},
    {"g_tnt_celtic_4p_packbits",g_tnt_celtic_4p_packbits,sizeof(g_tnt_celtic_4p_packbits)},
    {"g_tnt_celtic_4p_rgba16",g_tnt_celtic_4p_rgba16,sizeof(g_tnt_celtic_4p_rgba16)},
    {"g_tnt_african_4p_packbits",g_tnt_african_4p_packbits,sizeof(g_tnt_african_4p_packbits)},
    {"g_tnt_african_4p_rgba16",g_tnt_african_4p_rgba16,sizeof(g_tnt_african_4p_rgba16)},
    {"g_tnt_greek_4p_packbits",g_tnt_greek_4p_packbits,sizeof(g_tnt_greek_4p_packbits)},
    {"g_tnt_greek_4p_rgba16",g_tnt_greek_4p_rgba16,sizeof(g_tnt_greek_4p_rgba16)},
    {"g_tnt_russian_4p_packbits",g_tnt_russian_4p_packbits,sizeof(g_tnt_russian_4p_packbits)},
    {"g_tnt_russian_4p_rgba16",g_tnt_russian_4p_rgba16,sizeof(g_tnt_russian_4p_rgba16)},
    {"g_tnt_russian_sky_baseline_rgba",g_tnt_russian_sky_baseline_rgba,sizeof(g_tnt_russian_sky_baseline_rgba)},
};
#define SCENE_CACHE_PATH TNTV_DATA_DIR "/tntv_scene_cache_v1.bin"
#define SCENE_CACHE_TEMP SCENE_CACHE_PATH ".tmp"
#define SCENE_HEADER_SIZE 108U
static size_t scene_cache_layout(uint8_t digest[32]) {
    SHA256_CTX ctx; size_t total=0,i; uint8_t size[4];
    const char recipe[]="TNTV native scenes recipe205D sky202Q LE IEEE754";
    sha256_init(&ctx);sha256_update(&ctx,(const uint8_t*)recipe,sizeof(recipe));
    for(i=0;i<sizeof(s_scene_cache)/sizeof(s_scene_cache[0]);i++) {
        uint32_t n=(uint32_t)s_scene_cache[i].size;
        size[0]=n;size[1]=n>>8;size[2]=n>>16;size[3]=n>>24;
        sha256_update(&ctx,(const uint8_t*)s_scene_cache[i].name,strlen(s_scene_cache[i].name)+1);
        sha256_update(&ctx,size,4);total+=n;
    }
    sha256_final(&ctx,digest);return total;
}
static int scene_cache_load(void) {
    uint8_t header[SCENE_HEADER_SIZE],layout[32],hash[32],*payload=NULL;
    size_t total=scene_cache_layout(layout),i,offset=0;SHA256_CTX ctx;
    FILE *f=fopen(SCENE_CACHE_PATH,"rb");int rc=-1;
    if(!f)return -1;
    if(fread(header,1,sizeof(header),f)!=sizeof(header) ||
       memcmp(header,"TNTVSC01",8) || memcmp(header+8,layout,32) ||
       memcmp(header+40,tntv_rom_expected_sha256(),32) ||
       ((uint32_t)header[104]|(uint32_t)header[105]<<8|(uint32_t)header[106]<<16|(uint32_t)header[107]<<24)!=total)goto done;
    payload=(uint8_t*)malloc(total);if(!payload)goto done;
    if(fread(payload,1,total,f)!=total || fgetc(f)!=EOF || ferror(f))goto done;
    sha256_init(&ctx);sha256_update(&ctx,payload,total);sha256_final(&ctx,hash);
    if(memcmp(hash,header+72,32))goto done;
    /* Commit only after entire cache passes length/layout/ROM/digest checks. */
    for(i=0;i<sizeof(s_scene_cache)/sizeof(s_scene_cache[0]);i++) {
        memcpy(s_scene_cache[i].data,payload+offset,s_scene_cache[i].size);
        offset+=s_scene_cache[i].size;
    }
    rc=0;
done:
    free(payload);fclose(f);return rc;
}
static int scene_cache_save(void) {
    uint8_t header[SCENE_HEADER_SIZE]={0};size_t total,i;SHA256_CTX ctx;
    FILE *f;int ok=1;
    memcpy(header,"TNTVSC01",8);total=scene_cache_layout(header+8);
    memcpy(header+40,tntv_rom_expected_sha256(),32);
    sha256_init(&ctx);
    for(i=0;i<sizeof(s_scene_cache)/sizeof(s_scene_cache[0]);i++)
        sha256_update(&ctx,s_scene_cache[i].data,s_scene_cache[i].size);
    sha256_final(&ctx,header+72);
    header[104]=total;header[105]=total>>8;header[106]=total>>16;header[107]=total>>24;
    f=fopen(SCENE_CACHE_TEMP,"wb");if(!f)return -1;
    if(fwrite(header,1,sizeof(header),f)!=sizeof(header))ok=0;
    for(i=0;ok && i<sizeof(s_scene_cache)/sizeof(s_scene_cache[0]);i++)
        if(fwrite(s_scene_cache[i].data,1,s_scene_cache[i].size,f)!=s_scene_cache[i].size)ok=0;
    if(fclose(f)!=0)ok=0;
    if(!ok) {remove(SCENE_CACHE_TEMP);return -1;}
    /* Only our own invalid/old derived scene cache is replaced. */
    remove(SCENE_CACHE_PATH);
    if(rename(SCENE_CACHE_TEMP,SCENE_CACHE_PATH)!=0){remove(SCENE_CACHE_TEMP);return -1;}
    return 0;
}
static int load_cached_scene_tables(TNTVRom *rom) {
    if(scene_cache_load()==0) {
        write_status("205D SCENE CACHE HIT: restoring text and AI");
        /* Pointer tables are rebuilt in the current process, never serialized. */
        if(tntv_ai_restore_piece_patterns(rom,D_800C55B0,D_800C55B8,D_800C55CC,D_800C55F4,D_800C5644)!=0)return -135;
        if(tntv_img_restore_ui_strings(rom,g_tnt_story_storage,g_tnt_complete_stories,g_tnt_guest_name_storage,g_tnt_guest_names)!=0)return -134;
        if(tntv_img_restore_credits_text(rom,g_tnt_credits_rom_text,sizeof(g_tnt_credits_rom_text),g_tnt_credits_lines,181U)!=0)return -133;
        return 0;
    }
    /* A cache miss/rebuild must NEVER proceed from only a sampled ROM check. */
    if(!rom->fully_verified) {
        write_status("208D FULL ROM HASH REQUIRED: rebuilding native scenes");
        if(tntv_rom_verify_full(rom)!=TNTV_ROM_OK)return -136;
    }
    write_status("205D SCENE CACHE MISS: preparing scenes once");
    { int rc=load_wonder_scene_tables(rom);if(rc)return rc; }
    write_status("205D SAVING SCENES: next launch will reuse them");
    /* A full/unwritable card must not prevent the already-decoded game boot. */
    (void)scene_cache_save();
    return 0;
}
