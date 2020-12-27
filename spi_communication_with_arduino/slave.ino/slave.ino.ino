#include <SPI.h>
#define LEDpin 7
volatile boolean received;
char SlaveReceived;

void setup() {
  Serial.begin(9600);
  pinMode(LEDpin, OUTPUT);
  SPCR |= (1<<SPE);                   // turn on SPI in slave mode
  SPCR |= (1<<SPIE);                  // turn on interrupts
  received = false;
  SPI.attachInterrupt();
  sei();
}

ISR(SPI_STC_vect)
{
  SlaveReceived = SPDR;
  received = true;
}

void loop() {
  if(received)
  {
    if(SlaveReceived == 'A')
    {
      digitalWrite(LEDpin,HIGH);
      Serial.println("Slave LED on");
      Serial.println(SlaveReceived);
    }
    else
    {
      digitalWrite(LEDpin, LOW);
      Serial.println("Slave LED off");
      Serial.println(SlaveReceived);
    }
  }
  delay(1000);
}
