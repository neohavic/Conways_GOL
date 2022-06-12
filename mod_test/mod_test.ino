void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.print(8 %8 );
Serial.print(" ");
Serial.print(-1 % 8);
Serial.print(" ");
Serial.print(((-1 % 8 ) + 8) % 8);
Serial.print("\n");
}
