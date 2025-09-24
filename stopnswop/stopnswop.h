#ifndef STOPNSWOP_H
#define STOPNSWOP_H

#define SNS_PAYLOAD_LENGTH  128 
#define SNS_MAGIC           0xC908C52Fu // Header used to be detected by scan. Supplied by Banjo-Kazooie.
#define SNS_MAX_KEYS        29  // Capacity of the Payload.
#define SNS_GAME_ID			SNS_GAMEID_STOPNSWOP // Your game's SNS ID.
#define SNS_COPIES			100 // How many copies to colone with sns_flush.
#define SNS_ORIGIN			0xA0000380u // Boot scans + writes to this address. Should be a safe area unaffected by the game until sns_init() is called.

#define SNS_DOMAIN_START    0xA0300000u // Start of RDRAM area to write SNS Clone Payloads
#define SNS_DOMAIN_END		0xA0400000u // End of RDRAM area to write SNS Clone Payloads

extern uint8_t  snsPayload[SNS_PAYLOAD_LENGTH]; // Payload represented as bytes. Contains magic key, current keys, and two checksums.
extern uint8_t  snsKeyCount; // Total amount of keys currently occupying the payload stack.
extern bool     snsLoadedExtGamePayload; // True if was able to successfully load a payload from another game.

void sns_init(); // Retrieves the scanned Stop 'n Swop Data
void sns_reset(); // Creates an new empty payload
bool sns_add(uint8_t game, uint8_t flag); // Add a key to the payload. Returns false if the stack is full.
bool sns_add_params(uint8_t game, uint8_t flag, uint16_t params); // Add a key to the payload with parameters.
bool sns_remove(uint8_t game, uint8_t flag); // Removes a key from the payload. Returns false if key isn't found.
bool sns_contains(uint8_t game, uint8_t flag); // Returns true if the payload contains a key.
uint16_t sns_get_params(uint8_t game, uint8_t flag); // Get the parameters associated with the key. Returns 0 if not found.
void sns_flush(); // Clone the manifest data around the RAM, so the next game has many chances to find it. 

static inline uint64_t crc_shift(uint64_t s) {
    s = (((s >> 1) | (s << 32)) ^ ((s << 44) >> 32)) & 0x1FFFFFFFF;
    s ^= (s >> 20) & 0xFFF;
    return s;
}

static inline void bk_crc_pair(const uint8_t *data, size_t len, uint32_t *cs0, uint32_t *cs1) {
    uint64_t sum = 0x13108B3C1; 
    uint32_t acc0 = 0;
	uint32_t acc1 = 0;
    uint32_t sd = 0;

    for (size_t i = 0; i < len; i++) {
        unsigned k = sd & 0xF; 
        uint32_t add32 = ((uint32_t)data[i]) << k;
        sum += (uint64_t)add32;

        sum  = crc_shift(sum);                
        sd  += 7;
        acc0 ^= (uint32_t)sum;
    }

    for (size_t i = len; i-- > 0; ) {
        unsigned k = sd & 0xF;
        uint32_t add32 = ((uint32_t)data[i]) << k;
        sum += (uint64_t)add32;

        sum  = crc_shift(sum);
        sd  += 3;
        acc1 ^= (uint32_t)sum;
    }

    *cs0 = acc0;
    *cs1 = acc1;
}

static inline void sns_write_u32(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v>>24); 
	p[1] = (uint8_t)(v>>16); 
	p[2] = (uint8_t)(v>>8); 
	p[3] = (uint8_t)v;
}

static inline uint32_t sns_read_u32(const volatile uint8_t *p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
}

static inline void sns_write_u16(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v >> 8); 
	p[1] = (uint8_t)v;
}

static inline uint32_t sns_read_u16(const uint8_t *p) {
    return ((uint32_t)p[0] << 8) | p[1];
}

static inline bool sns_validchecksum(const volatile uint8_t *p) {
    uint32_t checksum0, checksum1;
    const uint8_t *cp = (const uint8_t *)p;
    bk_crc_pair(cp, SNS_PAYLOAD_LENGTH - 8, &checksum0, &checksum1);

    uint32_t exchecksum0 = sns_read_u32(p + SNS_PAYLOAD_LENGTH - 8);
    uint32_t exchecksum1 = sns_read_u32(p + SNS_PAYLOAD_LENGTH - 4);

    return (checksum0 == exchecksum0) && (checksum1 == exchecksum1);
}

#endif 