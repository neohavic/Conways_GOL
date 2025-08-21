/*
Code to see if the encapsulated modulo function worked correctly...spoiler, it does.
Verified using the ESP-S3 emulator at: https://wokwi.com/projects/new/esp32-s2
*/

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
uint8_t in = 85;
String test = String(modFixed(in));
  // put your main code here, to run repeatedly:
Serial.print("Old way " + String(((in % 64 ) + 64) % 64) + " " + "modFixed " + test + " \r\n");
delay(250);
}

uint8_t modFixed(uint8_t in)
{
  return (((in % 64) + 64) % 64);
}