#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>
#include "stopnswop.h"
#include "stopnswopdefs.h"

uint8_t snsPayload[SNS_PAYLOAD_LENGTH];
uint8_t snsKeyCount;
bool snsLoadedExtGamePayload;

static inline void write_u32(uint8_t *p, uint32_t v){
    p[0] = (uint8_t)(v>>24); 
	p[1] = (uint8_t)(v>>16); 
	p[2] = (uint8_t)(v>>8); 
	p[3] = (uint8_t)v;
}

static inline uint32_t read_u32(const uint8_t *p){
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
}

static inline void write_u16(uint8_t *p, uint32_t v){
    p[0] = (uint8_t)(v >> 8); 
	p[1] = (uint8_t)v;
}

static inline uint32_t read_u16(const uint8_t *p){
    return ((uint32_t)p[0] << 8) | p[1];
}

static inline uint64_t crc_shift(uint64_t s) {
    s = (((s >> 1) | (s << 32)) ^ ((s << 44) >> 32)) & 0x1FFFFFFFF;
    s ^= (s >> 20) & 0xFFF;
    return s;
}

void bk_crc_pair(const uint8_t *data, size_t len, uint32_t *cs0, uint32_t *cs1) {
    uint64_t sum = 0x13108B3C1;
	uint64_t acc0 = 0;
	uint64_t acc1 = 0;
    uint32_t sd = 0;

    for (size_t i = 0; i < len; i++) {
        sum += (uint64_t)data[i] << (sd & 0xF);
        sum  = crc_shift(sum);
        sd  += 7;
        acc0 ^= sum;
    }
	
    for (size_t i = len; i-- > 0; ) {
        sum += (uint64_t)data[i] << (sd & 0xF);
        sum  = crc_shift(sum);
        sd  += 3;
        acc1 ^= sum;
    }

    *cs0 = (uint32_t)acc0;
    *cs1 = (uint32_t)acc1;
}

static bool sns_empty_block(const uint8_t *p) {
    for (size_t i = 0; i < SNS_PAYLOAD_LENGTH; i++) {
		if (p[i] != 0x00) {
			return false;
		}
	}
	
    return true;
}

uint16_t sns_pairtokey(uint8_t game, uint8_t flag)  {
	return (game << 8) | (flag & 0xFFu);
}

void sns_clonedata() {
    const uintptr_t ramEnd = is_memory_expanded() ? 0x80800000 : 0x80400000;
	
	int placed = 0;
	
	uintptr_t p = 0x80010000;

    while (p + SNS_PAYLOAD_LENGTH <= ramEnd && placed < SNS_COPIES)
    {
        uint8_t *slot = (uint8_t*)p;
		
		p += 0x10000;
		
        if (read_u32(slot) != SNS_MAGIC && !sns_empty_block(slot)) {
            continue;
		}

        memcpy(slot, snsPayload, SNS_PAYLOAD_LENGTH);
		data_cache_hit_writeback(slot, SNS_PAYLOAD_LENGTH);
		
        placed++;
    }
}

void write_checksum() {
	uint32_t checksum0;
	uint32_t checksum1;
	
	bk_crc_pair(snsPayload, SNS_PAYLOAD_LENGTH - 8, &checksum0, &checksum1);
	
	write_u32(&snsPayload[SNS_PAYLOAD_LENGTH - 8], checksum0);    
    write_u32(&snsPayload[SNS_PAYLOAD_LENGTH - 4], checksum1);  
}

bool check_checksum(const uint8_t *p) {
	uint32_t checksum0, checksum1;
    bk_crc_pair(p, SNS_PAYLOAD_LENGTH - 8, &checksum0, &checksum1); 
	
    uint32_t exchecksum0 = read_u32(p + SNS_PAYLOAD_LENGTH - 8);
    uint32_t exchecksum1 = read_u32(p + SNS_PAYLOAD_LENGTH - 4);       
	
    return (checksum0 == exchecksum0) && (checksum1 == exchecksum1);
}

bool sns_add(uint8_t game, uint8_t flag) {
	return sns_add_params(game, flag, 0);
}

bool sns_add_params(uint8_t game, uint8_t flag, uint16_t params) {
	if (snsKeyCount == SNS_MAX_KEYS) {
		debugf("error: Payload stack is full!");
		return false;
	}
	
	uint16_t key = sns_pairtokey(game, flag);
	write_u32(&snsPayload[4 + snsKeyCount * 4], key + (params << 16));
	snsKeyCount++;
	
	write_checksum();
	sns_clonedata();
	
	return true;
}

bool sns_remove(uint8_t game, uint8_t flag) {
    uint8_t *p = snsPayload + 4;
	
	uint16_t key = sns_pairtokey(game, flag);

    for (int i = 0; i < snsKeyCount; i++) {
        if (read_u16(p + 2) == key) {
            for (int j = i; j < snsKeyCount - 1; j++) {
                uint8_t *dst = (snsPayload + 4) + j*4;
                uint8_t *src = (snsPayload + 4) + (j+1)*4;
                write_u32(dst, read_u32(src));
            }

            write_u32((snsPayload + 4) + (snsKeyCount - 1)*4, 0);
            snsKeyCount--;

            write_checksum();
			sns_clonedata();
			
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
        if (read_u16(p + 2) == key) {
            return read_u16(p);
		}
		
		p += 4;
    }
	
    return 0;
}

bool sns_contains(uint8_t game, uint8_t flag) {
	const uint8_t *p = snsPayload + 4;
	
	uint16_t key = sns_pairtokey(game, flag);
	
    for (int i = 0; i < snsKeyCount; i++) {
        if (read_u16(p + 2) == key) {
            return true;
		}
		
		p += 4;
    }
	
    return false;
}

void sns_init(bool clone) {
	write_u32(snsPayload, SNS_MAGIC);
	sns_add(0, SNS_GAME_ID);

	if (clone) {
		sns_clonedata();
	}
}

void sns_payloadtest() { // Simulate owning a red egg from Banjo-Kazooie
	write_u32(snsPayload, SNS_MAGIC);
	sns_add(0, SNS_GAMEID_STOPNSWOP);
	sns_add(SNS_GAMEID_BANJOKAZOOIE, SNS_BANJOKAZOOIE_REDEGG);
	sns_clonedata();
}

void sns_scan(void) {
	//sns_payloadtest();
	
	const uint8_t *ram  = (const uint8_t*)0x80000000;
    const uint8_t *end  = (const uint8_t*)(is_memory_expanded() ? 0x80800000 : 0x80400000);
	
	snsLoadedExtGamePayload = false;
	snsKeyCount = 0;
	
	for (const uint8_t *p = ram; p + SNS_PAYLOAD_LENGTH <= end; p += 0x80) {
		if (read_u32(p) == SNS_MAGIC && check_checksum(p)) {
			memcpy((uint8_t*)snsPayload, p, SNS_PAYLOAD_LENGTH);
			snsLoadedExtGamePayload = true;

			p = snsPayload + 4;
			
			snsKeyCount = 0;
			
			for (int i = 0; i < SNS_MAX_KEYS; i++) {
				if (read_u32(p) == 0) {
					break; 
				}
				
				p += 4;
				snsKeyCount++;
			}
			
			return;
        }
    }
	
	sns_init(false);
}
