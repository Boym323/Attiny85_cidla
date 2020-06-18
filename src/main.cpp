#include <Arduino.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#define PIN_napajeni_optosenzoru 1
#define PIN_SV 8
#define PIN_TV 9
#define LitrSV 13
#define LitrTV 14

volatile int impuls_z_wdt = 1;

ISR(WDT_vect)
{
  //když je proměnná impuls_z_wdt na 0
  if (impuls_z_wdt == 0)
  {
    // zapiš do proměnné 1
    impuls_z_wdt = 1;
  }
}

volatile boolean StavSV;
volatile boolean posledniStavSV;
volatile boolean StavTV;
volatile boolean posledniStavTV;

void setup()
{

  pinMode(PIN_napajeni_optosenzoru, OUTPUT);
  pinMode(PIN_SV, INPUT);
  pinMode(LitrSV, OUTPUT);
  pinMode(PIN_TV, INPUT);
  pinMode(LitrTV, OUTPUT);

  // nastavení WATCHDOG TIMERU
  MCUSR &= ~(1 << WDRF);              // neřešte
  WDTCSR |= (1 << WDCE) | (1 << WDE); // neřešte

  // nastavení času impulsu
  /**
	 *	Setting the watchdog pre-scaler value with VCC = 5.0V and 16mHZ
	 *	WDP3 WDP2 WDP1 WDP0 | Number of WDT | Typical Time-out at Oscillator Cycles
	 *	0    0    0    0    |   2K cycles   | 16 ms
	 *	0    0    0    1    |   4K cycles   | 32 ms
	 *	0    0    1    0    |   8K cycles   | 64 ms
	 *	0    0    1    1    |  16K cycles   | 0.125 s
	 *	0    1    0    0    |  32K cycles   | 0.25 s
	 *	0    1    0    1    |  64K cycles   | 0.5 s
	 *	0    1    1    0    |  128K cycles  | 1.0 s
	 *	0    1    1    1    |  256K cycles  | 2.0 s
	 *	1    0    0    0    |  512K cycles  | 4.0 s
	 *	1    0    0    1    | 1024K cycles  | 8.0 s
	*/
  WDTCSR = (0 << WDP3) | (1 << WDP2) | (1 << WDP1) | (0 << WDP0);

  WDTCSR |= _BV(WDIE); //neřešte
}

void enterSleep(void)
{
  //nastavení nejúspornějšího módu
  set_sleep_mode(SLEEP_MODE_STANDBY);
  // spánkový režim je povolený
  sleep_enable();
  // spuštění režimu spánku
  sleep_mode();

  // tady bude program pokračovat když se probudí

  // spánek zakázán
  sleep_disable();
  //znovu zapojení všech funkcí
  power_all_enable();
}

void TeplaVoda()
{
  StavTV = digitalRead(PIN_TV);

  // porovnejte buttonState (stav tlačítka) s předchozím stavem
  if (StavTV != posledniStavTV)
  {
    // jestliže se stav změnil, navyšte hodnotu počítadla
    if (StavTV == LOW)
    {
      // jestliže je současný stav HIGH, tlačítko přešlo
      //z off na on:
      digitalWrite(LitrTV, HIGH);
      delay(100);
      digitalWrite(LitrTV, LOW);
    }
    else
    {
      // jestliže je současný stav LOW, tlačítko přešlo
      // z on na off:
    }
  }
  // uložte současný stav jako „poslední stav“,
  //abyste ho mohli v příští smyčce použít
  posledniStavTV = StavTV;
}

void StudenaVoda()
{
  StavSV = digitalRead(PIN_SV);

  // porovnejte buttonState (stav tlačítka) s předchozím stavem
  if (StavSV != posledniStavSV)
  {
    // jestliže se stav změnil, navyšte hodnotu počítadla
    if (StavSV == LOW)
    {
      // jestliže je současný stav HIGH, tlačítko přešlo
      //z off na on:
      digitalWrite(LitrSV, HIGH);
      delay(100);
      digitalWrite(LitrSV, LOW);
    }
    else
    {
      // jestliže je současný stav LOW, tlačítko přešlo
      // z on na off:
    }
  }
  // uložte současný stav jako „poslední stav“,
  //abyste ho mohli v příští smyčce použít
  posledniStavSV = StavSV;
}

void loop()
{
  digitalWrite(PIN_napajeni_optosenzoru, HIGH);
  delay(1);
  StudenaVoda();
  TeplaVoda();
  delay(1);
  digitalWrite(PIN_napajeni_optosenzoru, LOW);
  enterSleep();
}