//This file will continued to be updated to gain support of N64 games that wish to use this method.
#ifndef STOPNSWOPDEFS_H
#define STOPNSWOPDEFS_H

enum SNS_GameID {  // Add your own game here and collaborate with others using this to not conflict IDs.  Supports up to 256 games.
	SNS_GAMEID_CHECKGAME = 0,
	SNS_GAMEID_BANJOKAZOOIE = 1,
	SNS_GAMEID_STOPNSWOP    = 2
};

enum SNS_BANJOKAZOOIE { // Each origin game has their own index table of items/flags. Supports up to 256 per game.
	SNS_BANJOKAZOOIE_YELLOWEGG = 0, 
	SNS_BANJOKAZOOIE_REDEGG = 1, 
	SNS_BANJOKAZOOIE_GREENEGG = 2, 
	SNS_BANJOKAZOOIE_BLUEEGG = 3, 
	SNS_BANJOKAZOOIE_PINKEGG = 4, 
	SNS_BANJOKAZOOIE_CYANEGG = 5, 
	SNS_BANJOKAZOOIE_ICEKEY = 6, 
	
	SNS_BANJOKAZOOIE_YELLOWEGG_UNLOCKED = 8, 
	SNS_BANJOKAZOOIE_REDEGG_UNLOCKED = 9, 
	SNS_BANJOKAZOOIE_GREENEGG_UNLOCKED = 10, 
	SNS_BANJOKAZOOIE_BLUEEGG_UNLOCKED = 11, 
	SNS_BANJOKAZOOIE_PINKEGG_UNLOCKED = 12, 
	SNS_BANJOKAZOOIE_CYANEGG_UNLOCKED = 13, 
	SNS_BANJOKAZOOIE_ICEKEY_UNLOCKED = 14
};

#endif 