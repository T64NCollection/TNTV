#ifndef TNT_AUDIO_VITA_H
#define TNT_AUDIO_VITA_H

#define SFX_MENU_0 0
#define SFX_MENU_1 1
#define SFX_MENU_2 2
#define SFX_MENU_3 3
#define SFX_MENU_4 4
#define SFX_MENU_5 5

#define SFX_INTRO_0 0
#define SFX_INTRO_1 1
#define SFX_INTRO_2 2
#define SFX_INTRO_3 3
#define SFX_INTRO_4 4
#define SFX_INTRO_5 5
#define SFX_INTRO_6 6
#define SFX_INTRO_7 7
#define SFX_INTRO_8 8
#define SFX_INTRO_9 9
#define SFX_INTRO_10 10
#define SFX_INTRO_11 11

extern SfxBank g_menuSfxBank;
extern SfxBank g_introSfxBank;

int TNT_Vita_AudioInit(void);
void TNT_Vita_AudioTerm(void);
int TNT_Vita_MusicPlaySong(u8 song);
u8 TNT_Vita_MusicCurrentSong(void);
void TNT_Vita_MusicPlayTitle(void);
void TNT_Vita_MusicStop(void);
void TNT_Vita_SfxStopAll(void);

#endif
