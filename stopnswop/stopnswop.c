#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h> 
#include "stopnswop.h"
#include "stopnswopdefs.h"

uint8_t snsPayload[SNS_PAYLOAD_LENGTH];
uint8_t snsKeyCount;
bool snsLoadedExtGamePayload;

static bool sns_empty_block(const uint8_t *p) {
    for (size_t i = 0; i < SNS_PAYLOAD_LENGTH; i++) {
		if (p[i] != 0x00) {
			return false;
		}
	}
	
    return true;
}

uint16_t sns_pairtokey(uint8_t game, uint8_t flag) {
	return (game << 8) | (flag & 0xFFu);
}

void sns_flush() {
	int placed = 0;
	
	uintptr_t p = SNS_DOMAIN_START;

    while (p + SNS_PAYLOAD_LENGTH <= SNS_DOMAIN_END && placed < SNS_COPIES)
    {
        uint8_t *slot = (uint8_t*)p;
		
		p += 0x8000;
		
        if (sns_read_u32(slot) != SNS_MAGIC && !sns_empty_block(slot)) {
            continue;
		}

        memcpy(slot, snsPayload, SNS_PAYLOAD_LENGTH);
		
        placed++;
    }
}

void write_checksum() {
	uint32_t checksum0;
	uint32_t checksum1;
	
	bk_crc_pair(snsPayload, SNS_PAYLOAD_LENGTH - 8, &checksum0, &checksum1);
	
	sns_write_u32(&snsPayload[SNS_PAYLOAD_LENGTH - 8], checksum0);    
    sns_write_u32(&snsPayload[SNS_PAYLOAD_LENGTH - 4], checksum1);  
}

bool sns_add(uint8_t game, uint8_t flag) {
	return sns_add_params(game, flag, 0);
}

bool sns_add_params(uint8_t game, uint8_t flag, uint16_t params) {
	if (snsKeyCount == SNS_MAX_KEYS) {
		return false;
	}
	
	uint16_t key = sns_pairtokey(game, flag);
	sns_write_u32(&snsPayload[4 + snsKeyCount * 4], key + (params << 16));
	snsKeyCount++;
	
	write_checksum();
	
	return true;
}

bool sns_remove(uint8_t game, uint8_t flag) {
    uint8_t *p = snsPayload + 4;
	
	uint16_t key = sns_pairtokey(game, flag);

    for (int i = 0; i < snsKeyCount; i++) {
        if (sns_read_u16(p + 2) == key) {
            for (int j = i; j < snsKeyCount - 1; j++) {
                uint8_t *dst = (snsPayload + 4) + j * 4;
                uint8_t *src = (snsPayload + 4) + (j + 1)*4;
                sns_write_u32(dst, sns_read_u32(src));
            }

            sns_write_u32((snsPayload + 4) + (snsKeyCount - 1)*4, 0);
            snsKeyCount--;

            write_checksum();
			
            return true;
        }
		
        p += 4;
    }
	
    return false;
}

uint16_t sns_get_params(uint8_t game, uint8_t flag) {
	const uint8_t *p = snsPayload + 4;
	
	uint16_t key = sns_pairtokey(game, flag);
	
    for (int i = 0; i < snsKeyCount; i++) {
        if (sns_read_u16(p + 2) == key) {
            return sns_read_u16(p);
		}
		
		p += 4;
    }
	
    return 0;
}

bool sns_contains(uint8_t game, uint8_t flag) {
	const uint8_t *p = snsPayload + 4;
	
	uint16_t key = sns_pairtokey(game, flag);
	
    for (int i = 0; i < snsKeyCount; i++) {
        if (sns_read_u16(p + 2) == key) {
            return true;
		}
		
		p += 4;
    }
	
    return false;
}

void sns_reset() {
	sns_write_u32(snsPayload, SNS_MAGIC);
	sns_add(0, SNS_GAME_ID);
	write_checksum();
}

void sns_payloadtest() { // Simulate owning a red egg from Banjo-Kazooie
	sns_write_u32(snsPayload, SNS_MAGIC);
	sns_add(0, SNS_GAMEID_BANJOKAZOOIE);
	sns_add(SNS_GAMEID_BANJOKAZOOIE, SNS_BANJOKAZOOIE_REDEGG);
	sns_flush();
	
	uint8_t *origin = (uint8_t *)SNS_ORIGIN;
	memcpy(origin, (uint8_t*)snsPayload, SNS_PAYLOAD_LENGTH);
}

void sns_init(void) {
	snsLoadedExtGamePayload = false;
	snsKeyCount = 0;
	
	const uint8_t *origin = (const uint8_t *)SNS_ORIGIN;
	
	if (sns_read_u32(origin) == SNS_MAGIC) {
		memcpy((uint8_t*)snsPayload, origin, SNS_PAYLOAD_LENGTH);
		snsLoadedExtGamePayload = true;
		
		const uint8_t *p = snsPayload + 4; 
		
		for (int i = 0; i < SNS_MAX_KEYS; i++) {
			if (sns_read_u32(p) == 0) {
				break; 
			}
				
			p += 4;
			snsKeyCount++;
		}
		
		memset((uint8_t *)SNS_ORIGIN, 0, SNS_PAYLOAD_LENGTH);
		
		return;
	}

	sns_reset();
}
