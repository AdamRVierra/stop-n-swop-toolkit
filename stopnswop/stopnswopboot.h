#ifndef STOPNSWOPBOOT_H
#define STOPNSWOPBOOT_H

#include <stdint.h>
#include "stopnswop.h"

static inline bool sns_scan(int memsize) {
    volatile uint8_t  *p   = (volatile uint8_t *)(0xA0000400u); 
    volatile uint8_t  *end = p + (memsize - SNS_PAYLOAD_LENGTH);
    volatile uint32_t *dst = (volatile uint32_t *)SNS_ORIGIN;

    for (; p < end; p += SNS_PAYLOAD_LENGTH) {
        if (*(volatile uint32_t *)p == SNS_MAGIC && sns_validchecksum(p)) {
            volatile const uint32_t *srcw = (volatile const uint32_t *)p;
            for (int i = 0; i < (SNS_PAYLOAD_LENGTH / 4); i++)
                dst[i] = srcw[i];

            return true;
        }
    }
	
	return false;
}

#endif 
