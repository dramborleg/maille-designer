#pragma once

#include "torus.h"

/*
 * Struct with all necessary variables for describing an inlay design.
 */
struct MailleInlay
{
    float ambientIntensity;
    std::vector<Torus> rings;
    bool ringsModified;
};
