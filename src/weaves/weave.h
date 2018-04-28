#pragma once

#include "mailleinlay.h"

class Torus;

class Weave
{
public:
    Weave() {}

    virtual void addRing(const Eigen::Vector2f &worldClickLoc,
                         const Eigen::Vector3f &color, MailleInlay &inlay) = 0;
    virtual void deleteRing(const Eigen::Vector2f &worldClickLoc,
                            MailleInlay &inlay) = 0;
};
