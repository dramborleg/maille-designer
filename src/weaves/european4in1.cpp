#include <cmath>
#include <stdexcept>

#include "european4in1.h"

European4in1::European4in1(float radius, float thickness)
    : radius(radius)
    , thickness(thickness)
{
    theta = atan(1.5 * thickness / radius);
    // clang-format off
    rot0 << 1.0, 0.0, 0.0, 0.0,
            0.0, cos(theta), -sin(theta), 0.0,
            0.0, sin(theta), cos(theta), 0.0,
            0.0, 0.0, 0.0, 1.0;
    rot1 << 1.0, 0.0, 0.0, 0.0,
            0.0, cos(-theta), -sin(-theta), 0.0,
            0.0, sin(-theta), cos(-theta), 0.0,
            0.0, 0.0, 0.0, 1.0;
    // clang-format on
}

void European4in1::addRing(const Eigen::Vector2f &worldClickLoc,
                           MailleInlay &inlay)
{
    std::pair<int, int> idx;
    auto t = std::shared_ptr<Torus>(new Torus());
    std::pair<int, int> nearIdx;
    int curX = 0;
    int curY = 0;

    try
    {
        nearIdx = nearestRing(worldClickLoc);
        auto near = rings.find(nearIdx);
        if (near == rings.end())
            throw std::domain_error("Ring with given index not found\n");

        Eigen::Vector2f nearCenter = near->second->get_center();
        float xDist = 1.85 * radius - 2 * thickness;
        float yDist = radius - thickness;
        curX = nearIdx.first;
        curY = nearIdx.second;

        if (worldClickLoc(0) < nearCenter(0) &&
            worldClickLoc(1) < nearCenter(1))
        {
            idx = std::make_pair(--curX, --curY);
            t->set_center(nearCenter(0) - xDist, nearCenter(1) - yDist);
        }
        else if (worldClickLoc(0) < nearCenter(0))
        {
            idx = std::make_pair(--curX, ++curY);
            t->set_center(nearCenter(0) - xDist, nearCenter(1) + yDist);
        }
        else if (worldClickLoc(0) > nearCenter(0) &&
                 worldClickLoc(1) < nearCenter(1))
        {
            idx = std::make_pair(++curX, --curY);
            t->set_center(nearCenter(0) + xDist, nearCenter(1) - yDist);
        }
        else
        {
            idx = std::make_pair(++curX, ++curY);
            t->set_center(nearCenter(0) + xDist, nearCenter(1) + yDist);
        }
    }
    catch (std::domain_error &e)
    {
        idx = std::make_pair(curX, curY);
        t->set_center(0.0, 0.0);
    }

    t->set_rotation(curX % 2 ? rot1 : rot0);

    auto idxIt = rings.find(idx);
    if (idxIt == rings.end())
    {
        rings[idx] = t;
        inlay.rings.push_back(t);
        inlay.ringsModified = true;
    }
}

void European4in1::deleteRing(const Eigen::Vector2f &worldClickLoc,
                              MailleInlay &inlay)
{
    try
    {
        std::pair<int, int> nearIdx = nearestRing(worldClickLoc);
        auto near = rings.find(nearIdx);
        if (near == rings.end())
            throw std::domain_error("Ring with given index not found\n");

        for (auto r = inlay.rings.begin(); r != inlay.rings.end(); r++)
        {
            if ((*r)->hasSameCenter(*near->second))
            {
                inlay.rings.erase(r);
                inlay.ringsModified = true;
                break;
            }
        }

        for (auto r = inlay.selectedRings.begin();
             r != inlay.selectedRings.end(); r++)
        {
            if ((*r)->hasSameCenter(*near->second))
            {
                inlay.selectedRings.erase(r);
                break;
            }
        }

        rings.erase(near);
    }
    catch (std::domain_error &e)
    {
    }
}

std::pair<int, int> European4in1::nearestRing(const Eigen::Vector2f &loc)
{
    if (rings.empty())
        throw std::domain_error("Nearest ring undefined\n");

    std::pair<int, int> nearIdx;
    float nearDist = std::numeric_limits<float>::max();

    for (const auto &r : rings)
    {
        Eigen::Vector2f center = r.second->get_center();
        float dist = (loc - center).norm();
        if (dist < nearDist)
        {
            nearDist = dist;
            nearIdx = r.first;
        }
    }

    return nearIdx;
}
