#ifndef TNTV_ROM_BRIDGE_H
#define TNTV_ROM_BRIDGE_H
void TNTV_ROM_SetProgress(void (*callback)(const char *));
int TNTV_ROM_BridgeBoot(void);
/* Original source labels are read from the owner's verified N64 image
 * in TNTV_ROM_BridgeBoot and never stored as bytes in this program. */
const char *TNTV_ROM_OriginalText(unsigned id);
extern unsigned char g_tnt_font_c_map[64];
extern unsigned char g_tnt_font_c_char_widths[46];
extern char g_tnt_name_editor_chars[45];

#endif
