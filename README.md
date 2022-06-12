## Conways_GOL
Conway's GOL implemented as an 8x8 toroidal playfield and deployed on an Adafruit Trinket and a 7219 SPI 8x8 LED matrix.

## FEATURES
- 8x8 Toroidal playfield (essentially a globe; edge edge of matrix affects the opposite edge
- Extremely compact
--> Utilizes Adafruit Trinket to fit entire project inside of small Radioshack plastic enclosure
- Custom written MOD routine
--> Built-in MOD routine doesn't properly calculate a "true" mod; see MOD test program
- Auto detection of both "dead" world and a world that has settled into a steady state; if so, program reinitializes and runs again
- Gratuitous use of FOR loops 0_o

### NEED TO DO:
- convert MOD routine into proper function to clean up code
