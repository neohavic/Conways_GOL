## ***Conways Game Of Life***

Conway's GOL implemented two ways: an Adafruit Trinket 5V and an 8x8 LED matrix, and an Adafruit Matrix portal clone wired to a HUB75

64x64 LED panel



## -FEATURES (8x8)

* 8x8 Toroidal playfield (essentially a globe; edge edge of matrix affects the opposite edge
* Extremely compact
  --> Utilizes Adafruit Trinket to fit entire project inside of small Radioshack plastic enclosure
* Custom written MOD routine
  --> Built-in MOD routine doesn't properly calculate a "true" mod; see MOD test program
* Auto detection of both "dead" world and a world that has settled into a steady state; if so, program reinitializes and runs again
* Gratuitous use of FOR loops 0\_o

### NEED TO DO:

* convert MOD routine into proper function to clean up code



--------------------------------------------------------------------------------------------------------------------------------------



## -FEATURES (64x64)

* Same toroidal engine as the first implementation, but now 64/64 cells
* RGB COLORS!!!
* Wrote a proper modulo function to clean up the code and have a proper function for portability
* **RGB COLORS!!!!!!**
