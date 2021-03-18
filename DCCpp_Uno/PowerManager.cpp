#include "PowerManager.h"
#include "Arduino.h"
#include <avr/sleep.h>
#include <avr/wdt.h>

// the prescaler is set so that timer2 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER2_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer2 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER2_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER2_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile unsigned long timer2_overflow_count = 0;
volatile unsigned long timer2_millis = 0;
static unsigned char timer2_fract = 0;

ISR(TIMER2_OVF_vect)
{
    // copy these to local variables so they can be stored in registers
    // (volatile variables must be read from memory on every access)
    unsigned long m = timer2_millis;
    unsigned char f = timer2_fract;

    m += MILLIS_INC;
    f += FRACT_INC;
    if (f >= FRACT_MAX)
    {
        f -= FRACT_MAX;
        m += 1;
    }

    timer2_fract = f;
    timer2_millis = m;
    timer2_overflow_count++;
}

unsigned long millisTimer2()
{
    unsigned long m;
    uint8_t oldSREG = SREG;

    // disable interrupts while we read timer2_millis or we might get an
    // inconsistent value (e.g. in the middle of a write to timer2_millis)
    cli();
    m = timer2_millis;
    SREG = oldSREG;

    return m;
}

PowerManager::PowerManager(int notAlivePin,
                           int wakeupPin,
                           int ledPin,
                           int rebootPin,
                           unsigned long powerStartupMinDurationSeconds,
                           unsigned long powerShutdownMinDurationSeconds)
    : _notAlivePin(notAlivePin),
      _wakeupPin(wakeupPin),
      _ledPin(ledPin),
      _rebootPin(rebootPin),
      _powerStartupMinDurationMs(powerStartupMinDurationSeconds * 1000),
      _powerShutdownMinDurationMs(powerShutdownMinDurationSeconds * 1000)
{
    wdt_disable();

    pinMode(_notAlivePin, INPUT_PULLUP);
    pinMode(_wakeupPin, INPUT_PULLUP);

    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, HIGH);
}

void PowerManager::waitForInit() const
{
    // Init timer2 (must be done after Arduino setup() called)
    bitClear(TCCR2A, WGM20); // WGM20 = 0
    bitClear(TCCR2A, WGM21); // WGM21 = 0

    bitSet(TIMSK2, TOIE2); // Enable Timer2

    auto startTime = millisTimer2();
    while (digitalRead(_notAlivePin) == HIGH || (millisTimer2() - startTime < _powerStartupMinDurationMs))
        oneFlash(500, 300);

    bitClear(TIMSK2, TOIE2); // Disable Timer2
    digitalWrite(_ledPin, HIGH);
}

bool PowerManager::stopAsked() const
{
    return digitalRead(_notAlivePin) == HIGH;
}

void PowerManager::shutdown() const
{
    bitSet(TIMSK2, TOIE2); // Enable Timer2

    auto startTime = millisTimer2();
    while (digitalRead(_wakeupPin) == LOW || millisTimer2() - startTime < _powerShutdownMinDurationMs)
        oneFlash(200, 150);

    bitClear(TIMSK2, TOIE2); // Disable Timer2
    digitalWrite(_ledPin, LOW);
    digitalWrite(LED_BUILTIN, LOW);

    off();
}

void PowerManager::oneFlash(unsigned int onDurationMs, unsigned int offDurationMs) const
{
    digitalWrite(_ledPin, HIGH);
    delayMs(onDurationMs);

    digitalWrite(_ledPin, LOW);
    delayMs(offDurationMs);
}

void PowerManager::delayMs(unsigned int durationMs) const
{
    auto startTime = millisTimer2();
    while (millisTimer2() - startTime < durationMs)
    {
    }
}

void PowerManager::fullReboot() const
{
  pinMode(_rebootPin, OUTPUT);
  digitalWrite(_rebootPin, LOW);
}

void wakeupInterrupt()
{
}

void PowerManager::off() const
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    attachInterrupt(digitalPinToInterrupt(_wakeupPin), wakeupInterrupt, FALLING);
    sleep_mode();

    // Sleeping....

    sleep_disable();
    detachInterrupt(digitalPinToInterrupt(_wakeupPin));
    
    fullReboot();
}