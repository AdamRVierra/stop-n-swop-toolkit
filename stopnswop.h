#ifndef STOPNSWOP_H
#define STOPNSWOP_H

#define SNS_PAYLOAD_LENGTH  128 
#define SNS_MAGIC           0xC908C52Fu // Header used to be detected by scan. Supplied by Banjo-Kazooie.
#define SNS_MAX_KEYS        29  // Capacity of the Payload.
#define SNS_GAME_ID			SNS_GAMEID_STOPNSWOP // Your game's SNS ID.
#define SNS_COPIES			30 // How many copies to colone with sns_clonedata.
#define SNS_ORIGIN			0x80000380u // Boot scans + writes to this address.

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
void sns_clonedata(); // Clone the manifest data around the RAM, so the next game has many chances to find it. 

#endif 