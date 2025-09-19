#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>
#include "stopnswop.h"
#include "stopnswopdefs.h"

typedef struct { 
    const char  *name;
	uint16_t     collected_key;
    uint16_t     unlock_key;
    const char  *sprite_path;  
    sprite_t    *sprite; 
} SnsItemDef;

#define SNS_ITEM_COUNT 7

static SnsItemDef SNS_Items[SNS_ITEM_COUNT] = {
    { "Yellow Egg",  SNS_BANJOKAZOOIE_YELLOWEGG, SNS_BANJOKAZOOIE_YELLOWEGG_UNLOCKED,"Egg_Yellow.sprite", NULL },
    { "Red Egg",     SNS_BANJOKAZOOIE_REDEGG,    SNS_BANJOKAZOOIE_REDEGG_UNLOCKED,   "Egg_Red.sprite",    NULL },
    { "Green Egg",   SNS_BANJOKAZOOIE_GREENEGG,  SNS_BANJOKAZOOIE_GREENEGG_UNLOCKED, "Egg_Green.sprite",  NULL },
    { "Blue Egg",    SNS_BANJOKAZOOIE_BLUEEGG,   SNS_BANJOKAZOOIE_BLUEEGG_UNLOCKED,  "Egg_Blue.sprite",   NULL },
    { "Pink Egg",    SNS_BANJOKAZOOIE_PINKEGG,   SNS_BANJOKAZOOIE_PINKEGG_UNLOCKED,  "Egg_Pink.sprite",   NULL },
    { "Cyan Egg",    SNS_BANJOKAZOOIE_CYANEGG,   SNS_BANJOKAZOOIE_CYANEGG_UNLOCKED,  "Egg_Cyan.sprite",   NULL },
    { "Ice Key",     SNS_BANJOKAZOOIE_ICEKEY,    SNS_BANJOKAZOOIE_ICEKEY_UNLOCKED,   "Ice_Key.sprite",    NULL },
};

static int cursor_pos = 0;
static bool dirDown = false;
static bool btnDown = false;

static bool unlocked[SNS_ITEM_COUNT];
static bool collected[SNS_ITEM_COUNT];

sprite_t * load_sprite(const char *sprite_path) {
    char path[100];
	
    snprintf(path, sizeof(path), "rom:/%s", sprite_path);
	
    return sprite_load(path);
}

int main(void) {
	sns_init();
	
	display_init(RESOLUTION_640x480, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE);
	dfs_init(DFS_DEFAULT_LOCATION);
	joypad_init();
	joypad_inputs_t inputs;
	
	for (int i = 0; i < SNS_ITEM_COUNT; i++) {
		SNS_Items[i].sprite = load_sprite(SNS_Items[i].sprite_path);
		collected[i] = sns_contains(SNS_GAMEID_BANJOKAZOOIE, SNS_Items[i].collected_key);
	}
	
	sprite_t * checkmark = load_sprite("check-mark.sprite");
	sprite_t * arrow = load_sprite("arrow.sprite");
	
	while (1) {
		surface_t* disp = display_get();
		graphics_fill_screen(disp, 0);
		graphics_draw_text(disp, 272, 20, "STOP 'N' SWOP");
		
		for (int i = 0; i < SNS_ITEM_COUNT; i++) {
			graphics_draw_sprite_trans(disp, 84 + (i * 68), 40, SNS_Items[i].sprite);
			
			if (collected[i]) {
				graphics_draw_sprite_trans(disp, 102 + (i * 68), 120, checkmark);
			}
			
			if (unlocked[i]) {
				graphics_draw_sprite_trans(disp, 102 + (i * 68), 170, checkmark);
			} 
		}

		display_show(disp);
		
		graphics_draw_text(disp, 20, 130, "COLLECTED");
		graphics_draw_text(disp, 20, 180, "UNLOCK");
		
		if (snsLoadedExtGamePayload) {
			graphics_draw_text(disp, 272, 280, "DATA LOADED!");
		}
		
		graphics_draw_sprite_trans(disp, 102 + cursor_pos * 68, 225, arrow);

		joypad_poll();
		inputs = joypad_get_inputs(JOYPAD_PORT_1);

		if (inputs.btn.d_right || inputs.stick_x > 60) {
			if (!dirDown) {
				cursor_pos++; 
					
				if (cursor_pos >= SNS_ITEM_COUNT) {
					cursor_pos = 0;
				}
				
				dirDown = true;
			}
		}
			
		else if (inputs.btn.d_left || inputs.stick_x < -60) {
			if (!dirDown) {
				cursor_pos--; 
					
				if (cursor_pos < 0) {
					cursor_pos = SNS_ITEM_COUNT - 1;
				}
				
				dirDown = true;
			}
		}
		else if (inputs.btn.a) {
			if (!btnDown) {
				unlocked[cursor_pos] = !unlocked[cursor_pos];
				
				if (unlocked[cursor_pos]) {
					sns_add(SNS_GAMEID_BANJOKAZOOIE, SNS_Items[cursor_pos].unlock_key);
				} 
				else {
					sns_remove(SNS_GAMEID_BANJOKAZOOIE, SNS_Items[cursor_pos].unlock_key);
				}
				
				sns_flush();
				
				btnDown = true;
			}
		}
		else {
			btnDown = false;
			dirDown = false;
		}
	}
}
