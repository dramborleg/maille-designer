#pragma once

#include "torus.h"

/*
 * Struct with all necessary variables for describing an inlay design.
 */
struct MailleInlay
{
    float ambientIntensity;
    std::vector<std::shared_ptr<Torus>> rings;
    bool ringsModified;
};
