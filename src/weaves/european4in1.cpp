#include <cmath>
#include <stdexcept>

#include "european4in1.h"

European4in1::European4in1(float radius, float thickness)
{
    theta = atan(1.5 * thickness / radius);
    xDist = 1.85 * radius - 2 * thickness;
    yDist = radius - thickness;
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
                           const Eigen::Vector3f &color, MailleInlay &inlay)
{
    std::pair<int, int> idx;
    std::pair<int, int> nearIdx;
    auto t = std::make_shared<Torus>(color);
    int curX = 0;
    int curY = 0;

    try
    {
        nearIdx = nearestRing(worldClickLoc);
        auto near = rings.find(nearIdx);
        if (near == rings.end())
            throw std::domain_error("Ring with given index not found\n");

        Eigen::Vector2f nearCenter = near->second->get_center();
        curX = nearIdx.first;
        curY = nearIdx.second;

        if (worldClickLoc(0) < nearCenter(0) &&
            worldClickLoc(1) < nearCenter(1))
        {
            idx = std::make_pair(--curX, --curY);
        }
        else if (worldClickLoc(0) < nearCenter(0))
        {
            idx = std::make_pair(--curX, ++curY);
        }
        else if (worldClickLoc(0) > nearCenter(0) &&
                 worldClickLoc(1) < nearCenter(1))
        {
            idx = std::make_pair(++curX, --curY);
        }
        else
        {
            idx = std::make_pair(++curX, ++curY);
        }

        t->set_center(idx.first * xDist, idx.second * yDist);
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

void European4in1::addRingsInArea(const Eigen::Vector2f &begin,
                                  const Eigen::Vector2f &end,
                                  const Eigen::Vector3f &color,
                                  MailleInlay &inlay)
{
    int minIdxX, minIdxY, maxIdxX, maxIdxY;
    int yBeginEven, yBeginOdd;

    minIdxX = std::min(begin(0), end(0)) / xDist;
    minIdxY = std::min(begin(1), end(1)) / yDist;
    maxIdxX = std::max(begin(0), end(0)) / xDist;
    maxIdxY = std::max(begin(1), end(1)) / yDist;

    yBeginEven = (minIdxY % 2) ? (minIdxY + 1) : minIdxY;
    yBeginOdd = (minIdxY % 2) ? minIdxY : (minIdxY + 1);

    for (int x = minIdxX; x <= maxIdxX; x++)
    {
        for (int y = (x % 2) ? yBeginOdd : yBeginEven; y <= maxIdxY; y += 2)
        {
            auto idx = std::make_pair(x, y);
            if (rings.find(idx) == rings.end())
            {
                auto t = std::make_shared<Torus>(color);
                t->set_center(idx.first * xDist, idx.second * yDist);
                t->set_rotation(x % 2 ? rot1 : rot0);

                rings[idx] = t;
                inlay.rings.push_back(t);
                inlay.ringsModified = true;
            }
        }
    }

    return;
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
