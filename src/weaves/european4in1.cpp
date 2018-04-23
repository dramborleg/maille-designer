#include "european4in1.h"

void European4in1::addRing(const Eigen::Vector2f &worldClickLoc,
                           std::shared_ptr<MailleInlay> inlay)
{
    std::tuple<int, int, int> idx;
    auto t = std::shared_ptr<Torus>(new Torus());
    std::tuple<int, int, int> nearIdx = nearestRing(worldClickLoc);

    auto near = rings.find(nearIdx);
    if (near != rings.end())
    {
        Eigen::Vector2f nearCenter = near->second->get_center();
        int newLayer = std::get<0>(nearIdx) ? 0 : 1;
        int curX = std::get<1>(nearIdx);
        int curY = std::get<2>(nearIdx);

        if (worldClickLoc(0) < nearCenter(0) &&
            worldClickLoc(1) < nearCenter(1))
        {
            idx = std::make_tuple(newLayer, curX, curY);
            t->set_center(nearCenter(0) - 1.5 * radius,
                          nearCenter(1) - 1.5 * radius);
        }
        else if (worldClickLoc(0) < nearCenter(0))
        {
            idx = std::make_tuple(newLayer, curX - 1, curY + 1);
            t->set_center(nearCenter(0) - 1.5 * radius,
                          nearCenter(1) + 1.5 * radius);
        }
        else if (worldClickLoc(0) > nearCenter(0) &&
                 worldClickLoc(1) < nearCenter(1))
        {
            idx = std::make_tuple(newLayer, curX + 1, curY);
            t->set_center(nearCenter(0) + 1.5 * radius,
                          nearCenter(1) - 1.5 * radius);
        }
        else
        {
            idx = std::make_tuple(newLayer, curX + 1, curY + 1);
            t->set_center(nearCenter(0) + 1.5 * radius,
                          nearCenter(1) + 1.5 * radius);
        }
    }
    else
    {
        idx = std::make_tuple(0, 0, 0);
        t->set_center(0.0, 0.0);
    }

    auto idxIt = rings.find(idx);
    if (idxIt == rings.end())
    {
        rings[idx] = t;
        inlay->rings.push_back(t);
        inlay->ringsModified = true;
    }

    return;
}

std::tuple<int, int, int> European4in1::nearestRing(const Eigen::Vector2f &loc)
{
    // If near ring doesn't exist, return an invalid layer
    std::tuple<int, int, int> nearIdx(-1, 0, 0);
    float nearDist = std::numeric_limits<float>::max();

    for (auto r : rings)
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
