#pragma once

#include <cpptoml.h>

#include "common.h"
#include "mailleinlay.h"

class Torus;

class Weave
{
public:
    virtual ~Weave() = default;

    virtual void addRing(const Eigen::Vector2f &worldClickLoc,
                         const Maille::Color &color, MailleInlay &inlay) = 0;
    virtual void addRingsInArea(const Eigen::Vector2f &begin,
                                const Eigen::Vector2f &end,
                                const Maille::Color &color,
                                MailleInlay &inlay) = 0;
    virtual void deleteRing(const Eigen::Vector2f &worldClickLoc,
                            MailleInlay &inlay) = 0;
    virtual void selectAdjacentColors(const Eigen::Vector2f &worldClickLoc,
                                      float threshold, MailleInlay &inlay) = 0;
    virtual std::shared_ptr<cpptoml::table>
        generateSaveFile(const MailleInlay &inlay) const = 0;
    virtual void importSaveFile(nanogui::Widget *parent,
                                std::shared_ptr<cpptoml::table> design,
                                MailleInlay &inlay) = 0;
    virtual void importImage(nanogui::Widget *parent, std::string fpath,
                             MailleInlay &inlay)
    {
    }
    virtual std::string exportPOV(const MailleInlay &inlay) const { return ""; }
};
