# Stop 'n' Swop Toolkit

**Stop 'n' Swop Toolkit** is a library for Nintendo 64 games, to exchange data with cart swapping. 

This feature was originally intended for Banjo-Kazooie as early versions of N64s cached the RDRAM for 20-30 seconds after the power turned off. Later models (starting with NUS-CPU-08 of late 1999) gave approximately 1-2 seconds before clearing, making reliable swapping impossible. 

## Running the Example

1. *Prerequisites*:
 - N64 (NUS-CPU-01 - NUS-CPU-07). You can find this information on the bottom of your console.
 - *Banjo-Kazooie* cartridge.
 - N64 Flash Cart.
 
2. *Download the example ROM under releases*
 - Download the [latest release](https://github.com/AdamRVierra/stop-n-swop-toolkit/releases).  
 - Add it to your Flashcart, either as direct-to-boot or from a menu.
 
3. *Send data to Banjo-Kazooie*
 - After opening *stopnswop.z64*, press A to select which *Stop 'n' Swop* items you want to make available in *Banjo-Kazooie*.
 - When you're done, turn off your cartridge, swap in your *Banjo-Kazooie* cartridge and turn your console back on within 20-30 seconds. 
 
4. *Retrieving the items*
 - In *Banjo-Kazooie*, access the item(s) you've made available.
  - If you need guidance finding items, [watch this guide](https://www.youtube.com/watch?v=KkUPuP6LZog).  
  - After you've retrieved the *Stop 'n' Swop* items, turn off your cartridge, swap in your flash cart and turn your console back on within 20-30 seconds. 
  - Load *stopnswop.z64*.
 
5. *Processing the items*
 - If all has gone right, the menu should now display checkmarks representing the items you've collected. You've just done *Stop 'n' Swop* with *Banjo-Kazooie*, the way *Rareware* intended!

## Building the Example
1. *Prerequisites*:
 - [Download Latest Source Code](https://github.com/AdamRVierra/stop-n-swop-toolkit/releases/) or run ```git clone https://github.com/AdamRVierra/stop-n-swop-toolkit.git ```
 - [Install libdragon environment](https://github.com/DragonMinded/libdragon/wiki/Installing-libdragon) 
 
2. *Building a ROM*
 - Take `ipl3.c` and the `stopnswop` folder from this repository, copy both into *libdragon/boot/*
  - This is needed to include the payload scan into the ROM's boot process.
 - Inside libdragon/boot/, run `make clean && make`
 - Retrieve the built *ipl3_dev.z64* inside *libdragon/boot/bin/* , copy it back into the source code directory.
 - Return to your source code's directory, run `make clean && make`
 - If all went right, you'll have your very own built `stopnswop.z64`!

## Reseting Banjo-Kazooie's SNS Data
Unfortnuately, there's no way to erase what items have been unlocked or acquired. The only way is with a Gameshark.

Reset All SNS Collectibles/Locations Codes:
| Version     | Code          |
|-------------|---------------|
| NTSC-U 1.0  | 81283400 0000 |
| PAL         | 81283220 0000 |
| NTSC-U 1.1  | 81282240 0000 |
| NTSC-J      | 81282240 0000 |

## Disclaimer
This project is for educational and preservation purposes.
  
## Credits
 - Adam V: Main programmer
 - wedarobi: Discovering the *Stop 'n' Swop* method.
 - [Banjo-Kazooie Disassembly](https://github.com/n64decomp/banjo-kazooie) for many notes
 - [libdragon team](https://github.com/DragonMinded/libdragon)
 - Swako and [his video](https://www.youtube.com/watch?v=sHQbswTkEjQ) for inspiration

## Contact
- [adam@adamvierra.com](mailto:adam@adamvierra.com)  
- [kbmadam.bsky.social](https://bsky.app/profile/kbmadam.bsky.social)  
- [@Koolboyman](https://x.com/Koolboyman)  
