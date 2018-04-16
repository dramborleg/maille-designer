#pragma once

#include "mailleinlay.h"

class Torus;

class Weave
{
public:
    Weave() {}

    virtual void addRing(const Eigen::Vector2f &worldClickLoc,
                         std::shared_ptr<MailleInlay> inlay) = 0;
};
