#include <algorithm>
#include <cmath>
#include <cpptoml.h>
#include <stdexcept>

#include "common.h"
#include "european4in1.h"

const std::string European4in1::weaveID = "European 4 in 1";
const int European4in1::VERSION = 0;

European4in1::European4in1()
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
                           const Maille::Color &color, MailleInlay &inlay)
{
    std::pair<int, int> idx;
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
    }
    catch (std::domain_error &e)
    {
        idx = std::make_pair(curX, curY);
    }

    addRingByIndex(idx, color, inlay);
}

void European4in1::addRingsInArea(const Eigen::Vector2f &begin,
                                  const Eigen::Vector2f &end,
                                  const Maille::Color &color,
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

        rings.erase(near);
    }
    catch (std::domain_error &e)
    {
    }
}

std::shared_ptr<cpptoml::table>
    European4in1::generateSaveFile(const MailleInlay &inlay) const
{
    auto root = cpptoml::make_table();
    auto indices = cpptoml::make_array();
    auto colors = cpptoml::make_array();

    for (const auto &r : rings)
    {
        auto idx = cpptoml::make_array();
        auto color = cpptoml::make_array();
        Maille::Color c = r.second->get_color();
        idx->push_back(r.first.first);
        idx->push_back(r.first.second);
        color->push_back(c(0));
        color->push_back(c(1));
        color->push_back(c(2));

        indices->push_back(idx);
        colors->push_back(color);
    }

    root->insert("WeaveID", weaveID);
    root->insert("Version", VERSION);
    root->insert("Indices", indices);
    root->insert("Colors", colors);

    return root;
}

bool European4in1::importSaveFile(std::shared_ptr<cpptoml::table> design,
                                  MailleInlay &inlay)
{
    bool success = true;
    auto indices = design->get_array_of<cpptoml::array>("Indices");
    auto colors = design->get_array_of<cpptoml::array>("Colors");

    if (!indices || !colors)
        return false;

    if (indices->size() != colors->size())
        success = false;

    size_t max = std::min(indices->size(), colors->size());
    for (size_t i = 0; i < max; i++)
    {
        auto idx = (*indices)[i]->get_array_of<int64_t>();
        auto color = (*colors)[i]->get_array_of<int64_t>();

        if (idx->size() != 2 || color->size() != 3)
        {
            success = false;
            continue;
        }

        if (!addRingByIndex(
                {(*idx)[0], (*idx)[1]},
                Maille::Color((*color)[0], (*color)[1], (*color)[2]), inlay))
            success = false;
    }

    return success;
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

bool European4in1::addRingByIndex(const std::pair<int, int> &index,
                                  const Maille::Color &color,
                                  MailleInlay &inlay)
{
    auto idxIt = rings.find(index);
    if (idxIt != rings.end())
        return false;

    auto t = std::make_shared<Torus>(color);
    t->set_center(index.first * xDist, index.second * yDist);
    t->set_rotation(index.first % 2 ? rot1 : rot0);
    rings[index] = t;
    inlay.rings.push_back(t);
    inlay.ringsModified = true;

    return true;
}
