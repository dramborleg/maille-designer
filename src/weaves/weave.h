#pragma once

#include <cpptoml.h>

#include "common.h"
#include "mailleinlay.h"

class Torus;

class Weave
{
public:
    Weave() {}

    virtual void addRing(const Eigen::Vector2f &worldClickLoc,
                         const Maille::Color &color, MailleInlay &inlay) = 0;
    virtual void addRingsInArea(const Eigen::Vector2f &begin,
                                const Eigen::Vector2f &end,
                                const Maille::Color &color,
                                MailleInlay &inlay) = 0;
    virtual void deleteRing(const Eigen::Vector2f &worldClickLoc,
                            MailleInlay &inlay) = 0;
    virtual std::shared_ptr<cpptoml::table>
        generateSaveFile(const MailleInlay &inlay) const = 0;
    virtual std::string getWeaveID() const = 0;
};
