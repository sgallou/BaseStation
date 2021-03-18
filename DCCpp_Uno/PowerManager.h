#pragma once

class PowerManager
{
public:
    PowerManager(int notAlivePin,
                 int wakeupPin, // Must be an interrupt pin
                 int ledPin,
                 int rebootPin,
                 unsigned long powerStartupMinDurationSeconds,
                 unsigned long powerShutdownMinDurationSeconds);

    void waitForInit() const;
    bool stopAsked() const;
    void shutdown() const;

private:
    void oneFlash(unsigned int onDurationMs, unsigned int offDurationMs) const;
    void delayMs(unsigned int durationMs) const;
    void off() const;
    void fullReboot() const;

    const int _notAlivePin;
    const int _wakeupPin;
    const int _ledPin;
    const int _rebootPin;
    const unsigned long _powerStartupMinDurationMs;
    const unsigned long _powerShutdownMinDurationMs;
};