#include <Arduino.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#ifndef __AVR_ATtiny85__
#error "This firmware is intended for ATtiny85."
#endif

// ATtiny85 (DIP-8) pin mapping used by this project:
// Arduino 0 = PB0 = physical pin 5 -> cold-water output pulse
// Arduino 1 = PB1 = physical pin 6 -> optical-sensor power
// Arduino 2 = PB2 = physical pin 7 -> hot-water output pulse
// Arduino 3 = PB3 = physical pin 2 -> cold-water sensor input
// Arduino 4 = PB4 = physical pin 3 -> hot-water sensor input
// PB5 / physical pin 1 remains RESET.
constexpr uint8_t PIN_LITR_SV = 0;
constexpr uint8_t PIN_NAPAJENI_OPTOSENZORU = 1;
constexpr uint8_t PIN_LITR_TV = 2;
constexpr uint8_t PIN_SV = 3;
constexpr uint8_t PIN_TV = 4;

constexpr uint8_t SENSOR_SETTLE_MS = 2;
constexpr uint8_t OUTPUT_PULSE_MS = 100;

bool posledniStavSV = HIGH;
bool posledniStavTV = HIGH;

// The watchdog interrupt is used only as a wake-up source.
ISR(WDT_vect)
{
}

static void nastavWatchdog()
{
  const uint8_t oldSreg = SREG;

  cli();
  wdt_reset();

  // Clear a possible watchdog-reset flag first.
  MCUSR &= ~_BV(WDRF);

  // Timed sequence required by the ATtiny85 watchdog.
  // Interrupt-only mode, nominal timeout about 1 s.
  WDTCR = _BV(WDCE) | _BV(WDE);
  WDTCR = _BV(WDIE) | _BV(WDP2) | _BV(WDP1);

  SREG = oldSreg;
}

static void uspatDoWatchdogu()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Avoid the race where the watchdog fires between enabling sleep
  // and executing the SLEEP instruction.
  cli();
  sleep_enable();
  sei();
  sleep_cpu();
  sleep_disable();
}

static void zapnoutCidla()
{
  digitalWrite(PIN_NAPAJENI_OPTOSENZORU, HIGH);
  delay(SENSOR_SETTLE_MS);
}

static void vypnoutCidla()
{
  digitalWrite(PIN_NAPAJENI_OPTOSENZORU, LOW);
}

static void inicializovatStavyCidel()
{
  zapnoutCidla();
  posledniStavSV = (digitalRead(PIN_SV) == HIGH);
  posledniStavTV = (digitalRead(PIN_TV) == HIGH);
  vypnoutCidla();
}

static void zpracovatCidla()
{
  zapnoutCidla();

  // Read both sensors before producing either output pulse so one
  // 100 ms report pulse cannot delay the second sensor measurement.
  const bool stavSV = (digitalRead(PIN_SV) == HIGH);
  const bool stavTV = (digitalRead(PIN_TV) == HIGH);

  vypnoutCidla();

  // Preserve the original behaviour: report only a HIGH -> LOW edge.
  const bool impulsSV = posledniStavSV && !stavSV;
  const bool impulsTV = posledniStavTV && !stavTV;

  posledniStavSV = stavSV;
  posledniStavTV = stavTV;

  if (!impulsSV && !impulsTV)
  {
    return;
  }

  if (impulsSV)
  {
    digitalWrite(PIN_LITR_SV, HIGH);
  }

  if (impulsTV)
  {
    digitalWrite(PIN_LITR_TV, HIGH);
  }

  delay(OUTPUT_PULSE_MS);

  if (impulsSV)
  {
    digitalWrite(PIN_LITR_SV, LOW);
  }

  if (impulsTV)
  {
    digitalWrite(PIN_LITR_TV, LOW);
  }
}

void setup()
{
  pinMode(PIN_NAPAJENI_OPTOSENZORU, OUTPUT);
  digitalWrite(PIN_NAPAJENI_OPTOSENZORU, LOW);

  // Keep the original electrical semantics. The external sensor circuit
  // must provide a defined HIGH/LOW level; internal pull-ups are not enabled.
  pinMode(PIN_SV, INPUT);
  pinMode(PIN_TV, INPUT);

  pinMode(PIN_LITR_SV, OUTPUT);
  pinMode(PIN_LITR_TV, OUTPUT);
  digitalWrite(PIN_LITR_SV, LOW);
  digitalWrite(PIN_LITR_TV, LOW);

  // ADC and analog comparator are not used.
  ADCSRA &= ~_BV(ADEN);
  ACSR |= _BV(ACD);

  inicializovatStavyCidel();
  nastavWatchdog();
}

void loop()
{
  // Sensors stay unpowered while the MCU sleeps. The watchdog wakes the
  // device roughly once per second for one measurement cycle.
  uspatDoWatchdogu();
  zpracovatCidla();
}
