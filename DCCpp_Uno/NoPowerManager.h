#pragma once

class NoPowerManager
{
public:
    NoPowerManager();

    void waitForInit() const;
    bool stopAsked() const;
    void shutdown() const;
};