#include <Wire.h>                 // For I2C communication
#include <Adafruit_Protomatter.h> // For RGB matrix

uint32_t cellCount, steadyCheck1, steadyCheck2, steadyGenAlive, steadyGenDead = 0;
uint32_t x, y;
#define HEIGHT  64 // Matrix height (pixels) - SET TO 64 FOR 64x64 MATRIX!
#define WIDTH   64 // Matrix width (pixels)
#define MAX_FPS 45 // Maximum redraw rate, frames/second

unsigned char masterCell[WIDTH][HEIGHT] =
{
  {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

unsigned char newCell[WIDTH][HEIGHT] =
{
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

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

#define N_COLORS   8
uint16_t colors[N_COLORS];

uint32_t prevTime = 0; // Used for frames-per-second throttle

void setup()
{
  // Creates a random number seed based on electrical noise from 3 onboard ADC channels for hopefully better
  // random number generation
  randomSeed(analogRead(0) * (analogRead(1) + analogRead(2)));

  Serial.begin(9600); // Open a serial port for debugging purposes

  Serial.begin(115200);
  //while (!Serial) delay(10);

  ProtomatterStatus status = matrix.begin();
  Serial.printf("Protomatter begin() status: %d\n", status);

  cellCount = 0;
}

void loop()
{
  uint32_t t;
  while(((t = micros()) - prevTime) < (1000000L / MAX_FPS));
  prevTime = t;
  
  for (int x = 0; x < WIDTH; x++)
  {
    for (int y = 0; y < HEIGHT; y++)
    {
      cellCount = 0;

      /*
           NOTE: The playfield is built as an 8x8 toroid, IE a globe flattened out. This is accomplished using modulo math. C++'s built-in mod function (%)
           only returns the REMAINDER, not the true mod. This can be proven by taking (-1 % 8), which will return -1, instead of 7 which is what is wanted for an object moving left on a
           torroidal array. The same can be used for the Y movements This was solved by using the formula (((X % 8) + 8) % 8). I should have written a function for this instead of
           hardcoding it into the coordinate calls, but I got lazy.
        */

      // Check left
      if (masterCell[(x - 1) % WIDTH][(y % HEIGHT)] == 1)
      {
        cellCount += 1;
      }

      // Check right
      if (masterCell[(x + 1) % WIDTH][(y % HEIGHT)] == 1)
      {
        cellCount += 1;
      }

      // Check up
      if (masterCell[(x % WIDTH)][(y - 1) % HEIGHT] == 1)
      {
        cellCount += 1;
      }

      // Check down
      if (masterCell[(x % WIDTH)][(y + 1) % HEIGHT] == 1)
      {
        cellCount += 1;
      }

      // Check up/left
      if (masterCell[(x - 1) % WIDTH][(y - 1) % HEIGHT] == 1)
      {
        cellCount += 1;
      }

      // Check up/right
      if (masterCell[(x + 1) % WIDTH][(y - 1) % HEIGHT] == 1)
      {
        cellCount += 1;
      }

      // Check down/left
      if (masterCell[(x - 1) % WIDTH][(y + 1) % HEIGHT] == 1)
      {
        cellCount += 1;
      }

      // Check down/right
      if (masterCell[(x + 1) % WIDTH][(y + 1) % HEIGHT] == 1)
      {
        cellCount += 1;
      }
      
      // Check conditions for cell life or death
      if((masterCell[x][y] == 1) && (cellCount < 2))
      {
          newCell[x][y] = 0;
      }
         
      if((masterCell[x][y] == 1) && (cellCount > 3))
      {
          newCell[x][y] = 0;
      }
      
      if((masterCell[x][y] == 1) && ((cellCount == 2) || (cellCount == 3)))
      {
          newCell[x][y] = 1;
      }
         
      if((masterCell[x][y] == 0) && (cellCount == 3))
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

}
