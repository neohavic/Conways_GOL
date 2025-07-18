/*
   CONWAY'S GAME OF LIFE V1.0
   Austin Everman

   A simple 8x8 toroidal implementation of GOL on an Arduino Uno (or Adafruit Pro Trinkit 5V), using an absolutely GRATUITIS number of FOR loops LOL
   (The reason for this is to understand the logic of the GAME, not for code elegance). The Arduino is running an 8x8 LED matrix driven by a serial SPI run MAX7219 LED driver
   using the LedControl library, which gives much easier control of both individual pixels with less serial coding involved (like, a LOT less!). The library still allows for
   the daisy-chaining of multiple MAX7219's and individually addressing them while retaining single pixel control.

   Version History:
    1.0, 01/13/2017: First stable state with all features originally intended.

   To-Do's:
   -Make modulo a function, rather than hardcoded
   -Expand to 16x16 with 4 MAX7219 modules
*/

#include "LedControl.h"
LedControl lc = LedControl(12, 11, 10, 1); // 12-DIN, 11-CLK, 10 -CS

/* Glider test pattern
m.
  unsigned char masterCell[8][8] =
  {
  {0, 0, 1, 0, 0, 0, 0, 0},
  {1, 0, 1, 0, 0, 0, 0, 0},
  {0, 1, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
  };

*/

// Set up global variables

unsigned char masterCell[8][8] =    // Create the 8x8 playing field
{
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

unsigned char newCell[8][8] =   // Create a second 8x8 field in which changes are made based on the conditions of the main field
{
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

int cellCount, steadyCheck1, steadyCheck2, steadyGenAlive, steadyGenDead = 0;
int x, y;

#define HEIGHT  32 // Matrix height (pixels) - SET TO 64 FOR 64x64 MATRIX!
#define WIDTH   64 // Matrix width (pixels)
#define MAX_FPS 45 // Maximum redraw rate, frames/second

#if defined(_VARIANT_MATRIXPORTAL_M4_) // MatrixPortal M4
uint8_t rgbPins[]  = {7, 8, 9, 10, 11, 12};
uint8_t addrPins[] = {17, 18, 19, 20, 21};
uint8_t clockPin   = 14;
uint8_t latchPin   = 15;
uint8_t oePin      = 16;
#else // MatrixPortal ESP32-S3
uint8_t rgbPins[]  = {42, 41, 40, 38, 39, 37};
uint8_t addrPins[] = {45, 36, 48, 35, 21};
uint8_t clockPin   = 2;
uint8_t latchPin   = 47;
uint8_t oePin      = 14;
#endif

#if HEIGHT == 16
#define NUM_ADDR_PINS 3
#elif HEIGHT == 32
#define NUM_ADDR_PINS 4
#elif HEIGHT == 64
#define NUM_ADDR_PINS 5
#endif

Adafruit_Protomatter matrix(
  WIDTH, 4, 1, rgbPins, NUM_ADDR_PINS, addrPins,
  clockPin, latchPin, oePin, true);

Adafruit_LIS3DH accel = Adafruit_LIS3DH();

#define N_COLORS   8
#define BOX_HEIGHT 8
#define N_GRAINS (BOX_HEIGHT*N_COLORS*8)
uint16_t colors[N_COLORS];

Adafruit_PixelDust sand(WIDTH, HEIGHT, N_GRAINS, 1, 128, false);

uint32_t prevTime = 0; // Used for frames-per-second throttle

// SETUP - RUNS ONCE AT PROGRAM START --------------------------------------

void err(int x) {
  uint8_t i;
  pinMode(LED_BUILTIN, OUTPUT);       // Using onboard LED
  for(i=1;;i++) {                     // Loop forever...
    digitalWrite(LED_BUILTIN, i & 1); // LED on/off blink to alert user
    delay(x);
  }
}

void setup()
{

  // Setup some system stuff for the LED
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);

  // Creates a random number seed based on electrical noise from 3 onboard ADC channels for hopefully better
  // random number generation
  randomSeed(analogRead(0) * (analogRead(1) + analogRead(2)));

  Serial.begin(9600); // Open a serial port for debugging purposes

  colors[0] = matrix.color565(64, 64, 64);  // Dark Gray
  colors[1] = matrix.color565(120, 79, 23); // Brown
  colors[2] = matrix.color565(228,  3,  3); // Red
  colors[3] = matrix.color565(255,140,  0); // Orange
  colors[4] = matrix.color565(255,237,  0); // Yellow
  colors[5] = matrix.color565(  0,128, 38); // Green
  colors[6] = matrix.color565(  0, 77,255); // Blue
  colors[7] = matrix.color565(117,  7,135); // Purple
}

void loop()
{
  // Fill the playing field with random pattern
  for (int x = 0; x < 8; x++)
  {
    for (int y = 0; y < 8; y++)
    {
      masterCell[x][y] = random(0, 2);
    }
  }

  // Clear the LED playfield
  for (x = 0; x < 8; x++)
  {
    for (y = 0; y < 8; y++)
    {
      lc.setLed(0, x, y, false);
    }
  }

  // Draw the playfield to LED slowly for aestethic effect
  for (x = 0; x < 8; x++)
  {
    for (y = 0; y < 8; y++)
    {
      if (masterCell[x][y] == 1)
      {
        lc.setLed(0, x, y, true);
      }

      delay(75);
      
    }
  }

  // Reset the generation number for steady state checks
  steadyGenDead, steadyGenAlive = 0;

  for (int gens = 0; gens < 300; gens++) // Run for 300 generations, tops
  {
    int x, y;

    // Draw the playfield to LED
    for (x = 0; x < 8; x++)
    {
      for (y = 0; y < 8; y++)
      {
        if (masterCell[x][y] == 1)
        {
          lc.setLed(0, x, y, true);
        }
        else
        {
          lc.setLed(0, x, y, false);
        }
      }
    }


    for (x = 0; x < 8; x++)
    {
      for (y = 0; y < 8; y++)
      {
        cellCount = 0;

        /*
           NOTE: The playfield is built as an 8x8 toroid, IE a globe flattened out. This is accomplished using modulo math. C++'s built-in mod function (%)
           only returns the REMAINDER, not the true mod. This can be proven by taking (-1 % 8), which will return -1, instead of 7 which is what is wanted for an object moving left on a
           torroidal array. The same can be used for the Y movements This was solved by using the formula (((X % 8) + 8) % 8). I should have written a function for this instead of
           hardcoding it into the coordinate calls, but I got lazy.
        */

        // Check left
        if (masterCell[((((x - 1) % 8) + 8) % 8)][(((y % 8) + 8) % 8)] == 1)
        {
          cellCount += 1;
        }

        // Check right
        if (masterCell[((((x + 1) % 8) + 8) % 8)][(((y % 8) + 8) % 8)] == 1)
        {
          cellCount += 1;
        }

        // Check up
        if (masterCell[(((x % 8) + 8) % 8)][((((y - 1) % 8) + 8) % 8)] == 1)
        {
          cellCount += 1;
        }

        // Check down
        if (masterCell[(((x % 8) + 8) % 8)][((((y + 1) % 8) + 8) % 8)] == 1)
        {
          cellCount += 1;
        }

        // Check up/left
        if (masterCell[((((x - 1) % 8) + 8) % 8)][((((y - 1) % 8) + 8) % 8)] == 1)
        {
          cellCount += 1;
        }

        // Check up/right

        if (masterCell[((((x + 1) % 8) + 8) % 8)][((((y - 1) % 8) + 8) % 8)] == 1)
        {
          cellCount += 1;
        }

        // Check down/left
        if (masterCell[((((x - 1) % 8) + 8) % 8)][((((y + 1) % 8) + 8) % 8)] == 1)
        {
          cellCount += 1;
        }

        // Check down/right
        if (masterCell[((((x + 1) % 8) + 8) % 8)][((((y + 1) % 8) + 8) % 8)] == 1)
        {
          cellCount += 1;
        }


        // Check conditions for cell life or death
        if ((masterCell[x][y] == 1) && (cellCount < 2))
        {
          newCell[x][y] = 0;
        }

        if ((masterCell[x][y] == 1) && (cellCount > 3))
        {
          newCell[x][y] = 0;
        }

        if ((masterCell[x][y] == 1) && ((cellCount == 2) || (cellCount == 3)))
        {
          newCell[x][y] = 1;
        }

        if ((masterCell[x][y] == 0) && (cellCount == 3))
        {
          newCell[x][y] = 1;
        }
      }
    }

    // Count the number of alive cells in masterCell and newCell for a steady state check.
    // If they have the same number alive, probably a steady state (or its an interesting pattern!), or if its empty
    for (int x = 0; x < 8; x++)
    {
      for (int y = 0; y < 8; y++)
      {
        if (masterCell[x][y] == 1)
        {
          steadyCheck1++;
        }

        if (newCell[x][y] == 1)
        {
          steadyCheck2++;
        }
      }
    }

    if ((steadyCheck1 == steadyCheck2) && (steadyCheck1 == 0)) // Check if totally dead
    {
      steadyGenDead++;
    }
    else
    {
      steadyGenDead = 0;
    }

    if ((steadyCheck1 == steadyCheck2) && (steadyCheck1 != 0)) // Check if steady, but alive
    {
      steadyGenAlive++;
    }
    else
    {
      steadyGenAlive = 0;
    }

    if (steadyGenDead == 30) // If dead, only loop for 30 more generations, regardless of where in the overall simulation we are at
    {
      loop();
    }

    if (steadyGenAlive == 75) // If alive but steady, only loop for 75 more generations, regardless of where in the overall simulation we are at
    {
      loop();
    }

    // Reset the steady set checks
    steadyCheck1 = 0;
    steadyCheck2 = 0;

    // Copy the edited play field to become the new master play field
    for (int x = 0; x < 8; x++)
    {
      for (int y = 0; y < 8; y++)
      {
        masterCell[x][y] = newCell[x][y];
      }
    }

    delay(250);

  }
}
