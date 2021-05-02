#include "NoPowerManager.h"
NoPowerManager::NoPowerManager()
{
}

void NoPowerManager::waitForInit() const
{
}

bool NoPowerManager::stopAsked() const
{
    return false;
}

void NoPowerManager::shutdown() const
{
}
