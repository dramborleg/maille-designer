#pragma once

#include "mailleinlay.h"

class Tool
{
public:
    Tool(int icon) { this->icon = icon; }
    virtual ~Tool() = default;

    virtual void load(MailleInlay &inlay) {}
    virtual void unload(MailleInlay &inlay) {}
    virtual bool mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                  bool down, int modifiers,
                                  const Eigen::Vector2f &worldPos,
                                  MailleInlay &inlay)
    {
        return false;
    }
    virtual bool mouseDragEvent(const Eigen::Vector2i &p,
                                const Eigen::Vector2i &rel, int button,
                                int modifiers, const Eigen::Vector2f &worldPos,
                                MailleInlay &inlay)
    {
        return false;
    }
    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers,
                               MailleInlay &inlay)
    {
        return false;
    }
    int getIcon() { return icon; }

protected:
    std::pair<std::shared_ptr<Torus>, float>
        findNearestRing(const MailleInlay &inlay,
                        const Eigen::Vector2f &worldPos) const
    {
        if (inlay.rings.empty())
            throw std::domain_error("Nearest ring undefined\n");

        float nearDist = std::numeric_limits<float>::max();
        std::shared_ptr<Torus> nearest;

        for (const auto &r : inlay.rings)
        {
            Eigen::Vector2f center = r->get_center();
            float dist = (worldPos - center).norm();

            if (dist < nearDist)
            {
                nearest = r;
                nearDist = dist;
            }
        }

        return std::make_pair(nearest, nearDist);
    }

private:
    int icon;
};
