#define datain 21
#define shclk 23
#define stclk 22
#define ebene0 4
#define ebene1 5
#define ebene2 18
#define ebene3 19





void setup() {
pinMode(datain, OUTPUT);
pinMode(shclk, OUTPUT);
pinMode(stclk, OUTPUT);

pinMode(ebene0, OUTPUT);
pinMode(ebene1, OUTPUT);
pinMode(ebene2, OUTPUT);
pinMode(ebene3, OUTPUT);

digitalWrite(ebene0, LOW);
digitalWrite(ebene1, LOW);
digitalWrite(ebene2, LOW);
digitalWrite(ebene3, LOW);
}

void loop() {
  
  //turn all leds on
digitalWrite(stclk,HIGH); 
shwifty(255);
shwifty(255);
digitalWrite(stclk,LOW); 
// blink all layers:
for(int u = 0; u < 500; ++u) {
  digitalWrite(ebene0, HIGH);
  delay(1);
  digitalWrite(ebene0, LOW);
  digitalWrite(ebene1, HIGH);
  delay(1);
  digitalWrite(ebene1, LOW);
  digitalWrite(ebene2, HIGH);
  delay(1);
  digitalWrite(ebene2, LOW);
  digitalWrite(ebene3, HIGH);
  delay(1);
  digitalWrite(ebene3, LOW);
}

//turn all leds off
digitalWrite(stclk,HIGH); 
shwifty(0);
shwifty(0);
digitalWrite(stclk,LOW); 
//blink all layers
for(int u = 0; u < 500; ++u) {
  digitalWrite(ebene0, HIGH);
  delay(1);
  digitalWrite(ebene0, LOW);
  digitalWrite(ebene1, HIGH);
  delay(1);
  digitalWrite(ebene1, LOW);
  digitalWrite(ebene2, HIGH);
  delay(1);
  digitalWrite(ebene2, LOW);
  digitalWrite(ebene3, HIGH);
  delay(1);
  digitalWrite(ebene3, LOW);
}

}

void shwifty (byte input) {

uint8_t i;

//digitalWrite(stclk,HIGH); //register mode inverted

delayMicroseconds(1);

for (i = 0; i < 8; i++)  {
        //if (bitOrder == LSBFIRST)
            digitalWrite(datain, !(input & (1 << i)));
        //else    
            //digitalWrite(dataPin, !!(input & (1 << (7 - i))));

        digitalWrite(shclk, LOW);
        delayMicroseconds(1);
        digitalWrite(shclk, HIGH);
        delayMicroseconds(1);        
    }
    
//digitalWrite(stclk,LOW); //negative edge for enabling outputs
}
