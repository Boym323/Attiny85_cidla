#include <Arduino.h>

#define PIN_napajeni_optosenzoru 1
#define PIN_SV 3
#define PIN_TV 4
#define LitrSV 5
#define LitrTV 6

boolean StavTV;
boolean StavSV;
boolean posledniStavSV;
boolean posledniStavTV;

void setup()
{
  pinMode(PIN_napajeni_optosenzoru, OUTPUT);
  pinMode(PIN_SV, INPUT);
  pinMode(PIN_TV, INPUT);
  pinMode(LitrSV, OUTPUT);
  pinMode(LitrTV, OUTPUT);
}
void loop()
{
  digitalWrite(PIN_napajeni_optosenzoru, HIGH);
  StavSV = digitalRead(PIN_SV);
  if (StavSV != posledniStavSV)
  {
    if (StavSV == LOW)
    {
      digitalWrite(LitrSV, HIGH);
   
    }
  }
  posledniStavSV = StavSV;

  StavTV = digitalRead(PIN_TV);
  if (StavTV != posledniStavSV)
  {
    if (StavTV == LOW)
    {
      digitalWrite(LitrTV, HIGH);
    }
  }
  posledniStavTV = StavTV;
  digitalWrite(PIN_napajeni_optosenzoru, LOW);
  delay(1000);
}