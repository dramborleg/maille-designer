#define _USE_MATH_DEFINES
#include <algorithm>
#include <climits>
#include <cmath>
#include <set>
#include <stdexcept>

#include <cpptoml.h>
#include <nanogui/checkbox.h>
#include <nanogui/layout.h>
#include <nanogui/messagedialog.h>
#include <nanogui/textbox.h>
#include <nanogui/window.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "common.h"
#include "european4in1.h"

const std::string European4in1::weaveID = "European 4 in 1";
const int European4in1::VERSION = 1;

European4in1::European4in1(nanogui::Window *weaveSettings,
                           std::shared_ptr<MailleInlay> inlay)
{
    using namespace nanogui;

    weaveSettings->setLayout(new GroupLayout());

    wrongWay = new CheckBox(weaveSettings, "Wrong Way");
    wrongWay->setChecked(true);
    wrongWay->setCallback([this, inlay](bool val) { swapWay(*inlay); });

    rotateInlay = new CheckBox(weaveSettings, "Rotate Inlay            ");
    rotateInlay->setTooltip("Whether or not to rotate inlay design when "
                            "switching between wrong way and right way");

    ar = new FloatBox<float>(weaveSettings, 3.6);
    ar->setEditable(true);
    ar->setSpinnable(true);
    ar->setMinMaxValues(3, 10);
    ar->setUnits("AR");
    ar->setCallback([this, inlay](float val) { resetARValues(*inlay); });

    resetARValues(*inlay);
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
    float xDiv = wrongWay->checked() ? xDist : yDist;
    float yDiv = wrongWay->checked() ? yDist : xDist;

    minIdxX = std::min(begin(0), end(0)) / xDiv;
    minIdxY = std::min(begin(1), end(1)) / yDiv;
    maxIdxX = std::max(begin(0), end(0)) / xDiv;
    maxIdxY = std::max(begin(1), end(1)) / yDiv;

    yBeginEven = (minIdxY % 2) ? (minIdxY + 1) : minIdxY;
    yBeginOdd = (minIdxY % 2) ? minIdxY : (minIdxY + 1);

    for (int x = minIdxX; x <= maxIdxX; x++)
        for (int y = (x % 2) ? yBeginOdd : yBeginEven; y <= maxIdxY; y += 2)
            addRingByIndex(std::make_pair(x, y), color, inlay);
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

    // Could get a better time complexity using unordered_map + vector, but I
    // spent a while figuring out how to use Color objects as keys so may as
    // well keep it around for future reference.
    //
    // There should never be a tie for the ordering of two Color objects since
    // the hash function returns unique values for each Color (unless
    // sizeof(size_t) < 4B), so we can use find and distance to determine their
    // indices in the save file array.
    std::set<Maille::Color, Maille::ColorCompare> colorIndices;

    for (const auto &r : rings)
        colorIndices.insert(r.second->get_color());

    for (const auto &r : rings)
    {
        auto idx = cpptoml::make_array();
        idx->push_back(r.first.first);
        idx->push_back(r.first.second);
        idx->push_back(std::distance(colorIndices.begin(),
                                     colorIndices.find(r.second->get_color())));
        indices->push_back(idx);
    }

    for (const auto &c : colorIndices)
    {
        auto color = cpptoml::make_array();
        color->push_back(c(0));
        color->push_back(c(1));
        color->push_back(c(2));
        colors->push_back(color);
    }

    root->insert("WeaveID", weaveID);
    root->insert("Version", VERSION);
    root->insert("WrongWay", wrongWay->checked());
    root->insert("AR", ar->value());
    root->insert("Indices", indices);
    root->insert("Colors", colors);

    return root;
}

void European4in1::importSaveFile(nanogui::Widget *parent,
                                  std::shared_ptr<cpptoml::table> design,
                                  MailleInlay &inlay)
{
    auto vx = design->get_as<int>("Version");

    if (!vx)
        new nanogui::MessageDialog(
            parent, nanogui::MessageDialog::Type::Information,
            "Import Information",
            "This does not appear to be a Maille Inlay Designer file. "
            "Import aborted.");
    else if (*vx == VERSION)
        importLatestFile(parent, design, inlay);
    else if (*vx == 0)
        importV0File(parent, design, inlay);
}

void European4in1::importLatestFile(nanogui::Widget *parent,
                                    std::shared_ptr<cpptoml::table> design,
                                    MailleInlay &inlay)
{
    bool success = true;
    auto indices = design->get_array_of<cpptoml::array>("Indices");
    auto colors = design->get_array_of<cpptoml::array>("Colors");
    auto wrongWay = design->get_as<bool>("WrongWay");
    auto AR = design->get_as<double>("AR");
    std::vector<Maille::Color> importedColors;

    if (!indices || !colors)
    {
        success = false;
        goto out;
    }

    if (!wrongWay)
        success = false;
    else
        this->wrongWay->setChecked(*wrongWay);

    if (!AR)
        success = false;
    else
        ar->callback()(std::to_string(*AR));

    for (size_t i = 0; i < colors->size(); i++)
    {
        auto c = (*colors)[i]->get_array_of<int64_t>();
        if (!c || (*c).size() != 3)
        {
            success = false;
            continue;
        }
        importedColors.push_back(Maille::Color((*c)[0], (*c)[1], (*c)[2]));
    }

    for (size_t i = 0; i < indices->size(); i++)
    {
        auto idx = (*indices)[i]->get_array_of<int64_t>();

        if (!idx || idx->size() != 3 || (*idx)[2] >= importedColors.size())
        {
            success = false;
            continue;
        }

        if (!addRingByIndex({(*idx)[0], (*idx)[1]}, importedColors[(*idx)[2]],
                            inlay))
            success = false;
    }

out:
    if (!success)
        new nanogui::MessageDialog(
            parent, nanogui::MessageDialog::Type::Warning, "Import Error",
            "Error while importing design file. Possible file corruption.");
}

void European4in1::importV0File(nanogui::Widget *parent,
                                std::shared_ptr<cpptoml::table> design,
                                MailleInlay &inlay)
{
    size_t max;
    bool success = true;
    auto indices = design->get_array_of<cpptoml::array>("Indices");
    auto colors = design->get_array_of<cpptoml::array>("Colors");

    new nanogui::MessageDialog(
        parent, nanogui::MessageDialog::Type::Information, "Import Information",
        "It seems this save file is from an older version of Maille Inlay "
        "Designer. If you designed this file while using a rotated view so "
        "that it looked like the Right Way, please check the \"Rotate "
        "Inlay\" checkbox and afterwards uncheck the Wrong Way checkbox to "
        "get your old configuration, then resave the file so in the future "
        "it will load the Right Way by default.");

    if (!indices || !colors)
    {
        success = false;
        goto out;
    }

    if (indices->size() != colors->size())
        success = false;

    wrongWay->setChecked(true);
    ar->callback()("3.6");

    max = std::min(indices->size(), colors->size());
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

out:
    if (!success)
        new nanogui::MessageDialog(
            parent, nanogui::MessageDialog::Type::Warning, "Import Error",
            "Error while importing design file. Possible file corruption.");
}

void European4in1::importImage(nanogui::Widget *parent, std::string fpath,
                               MailleInlay &inlay)
{
    int xdim, ydim, nchan, yOffset;
    unsigned char *img = stbi_load(fpath.c_str(), &xdim, &ydim, &nchan, 3);
    nchan = 3;
    if (img == nullptr)
    {
        std::string error_msg = "failed to allocate memory for image";
        new nanogui::MessageDialog(parent,
                                   nanogui::MessageDialog::Type::Warning,
                                   "Import from Image Error", error_msg);
        return;
    }

    inlay.rings.clear();
    rings.clear();

    // If ydim is odd, we need to add one to the y coordinate so we stay on the
    // expected grid values and don't end up with addRingsInArea adding rings in
    // between other rings where they don't belong, eg adding a ring at index
    // (0, 1), which is supposed to be empty.
    yOffset = ydim % 2 ? 0 : 1;
    for (int y = 0; y < ydim; y++)
    {
        for (int x = 0; x < xdim; x++)
        {
            int idx = nchan * y * xdim + nchan * x;
            Maille::Color c(img[idx], img[idx + 1], img[idx + 2]);
            int xIdx = y % 2 ? 2 * x : 2 * x - 1;
            int yIdx = ydim - y + yOffset;
            addRingByIndex({xIdx, yIdx}, c, inlay);
        }
    }
}

std::string European4in1::exportPOV(const MailleInlay &inlay) const
{
    if (!inlay.rings.size())
        return "";

    std::string rot;
    std::ostringstream out(Maille::POV_TEMPLATE, std::ios_base::ate);
    std::pair<int, int> minIdx = {INT_MAX, INT_MAX};
    std::pair<int, int> maxIdx = {INT_MIN, INT_MIN};
    std::pair<float, float> minBound;
    std::pair<float, float> maxBound;
    std::set<Maille::Color, Maille::ColorCompare> colors;
    float camX, camZ, camRadius;
    float thetaDeg = theta * 180 / M_PI;

    for (const auto &r : rings)
    {
        minIdx.first = std::min(minIdx.first, r.first.first);
        maxIdx.first = std::max(maxIdx.first, r.first.first);
        minIdx.second = std::min(minIdx.second, r.first.second);
        maxIdx.second = std::max(maxIdx.second, r.first.second);
        colors.insert(r.second->get_color());
    }

    minBound = idxToPos({minIdx.first, minIdx.second});
    maxBound = idxToPos({maxIdx.first, maxIdx.second});
    camX = (minBound.first + maxBound.first) / 2;
    camZ = (minBound.second + maxBound.second) / 2;
    camRadius = std::max(maxBound.first - camX, maxBound.second - camZ);
    camRadius = 2 * (camRadius + radius + thickness) + radius;

    // camera parameters
    out << "camera {\n"
        << "  orthographic\n"
        << "  location  <" << camX << ", 1, " << camZ << ">\n"
        << "  right     <" << camRadius << ", 0, 0>\n"
        << "  up        <0, 0, " << camRadius << ">\n"
        << "  direction <0, -1, 0>\n"
        << "  look_at   <" << camX << ", 0, " << camZ << ">\n}\n\n";

    // textures
    for (const auto &c : colors)
    {
        out << "#declare T_" << Maille::ColorHash()(c) << " = texture { "
            << "pigment { color rgb <" << c(0) / 255.0 << ", " << c(1) / 255.0
            << ", " << c(2) / 255.0 << "> } finish { F_Metal } }\n";
    }

    for (const auto &r : rings)
    {
        if (wrongWay->checked())
        {
            rot = r.first.first % 2 ? "<-" : "<";
            rot += std::to_string(thetaDeg) + ", 0, 0>\n";
        }
        else
        {
            rot = r.first.second % 2 ? "<0, 0, -" : "<0, 0, ";
            rot += std::to_string(thetaDeg) + ">\n";
        }
        Eigen::Vector2f ctr = r.second->get_center();
        // clang-format off
        out << "\ntorus {\n  " << radius << ", " << radius / ar->value() << "\n"
            << "  rotate " << rot
            << "  translate<" << ctr(0) << ", 0, " << ctr(1) << ">\n"
            << "  texture { T_" << Maille::ColorHash()(r.second->get_color())
            << " }\n}";
        // clang-format on
    }

    return out.str();
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

    auto t = std::make_shared<Torus>(color, ar->value());
    std::pair<float, float> center = idxToPos(index);
    t->set_center(center.first, center.second);
    t->set_rotation(idxToRot(index));
    rings[index] = t;
    inlay.rings.push_back(t);
    inlay.ringsModified = true;

    return true;
}

const Eigen::Matrix4f &
    European4in1::idxToRot(const std::pair<int, int> &idx) const
{
    if (wrongWay->checked())
        return idx.first % 2 ? rot1WrongWay : rot0WrongWay;

    return idx.second % 2 ? rot1RightWay : rot0RightWay;
}

std::pair<float, float>
    European4in1::idxToPos(const std::pair<int, int> &idx) const
{
    if (wrongWay->checked())
        return std::make_pair(idx.first * xDist, idx.second * yDist);

    return std::make_pair(idx.first * yDist, idx.second * xDist);
}

void European4in1::swapWay(MailleInlay &inlay)
{
    if (rings.empty())
        return;

    if (rotateInlay->checked())
    {
        std::map<std::pair<int, int>, std::shared_ptr<Torus>> rotRings;
        for (const auto &r : rings)
        {
            std::pair<int, int> idx = r.first;
            std::swap(idx.first, idx.second);
            idx.second = -idx.second;
            rotRings[idx] = r.second;
        }

        rotRings.swap(rings);
    }

    recalculateRingTransformations(inlay);
}

void European4in1::recalculateRingTransformations(MailleInlay &inlay)
{
    for (auto &r : rings)
    {
        std::pair<float, float> center = idxToPos(r.first);
        r.second->set_center(center.first, center.second);
        r.second->set_rotation(idxToRot(r.first));
    }

    inlay.ringsModified = true;
}

void European4in1::resetARValues(MailleInlay &inlay)
{
    float AR = ar->value();
    theta = atan(1.5 / AR);
    thickness = radius / AR;
    xDist = 1.85 * radius - 2 * thickness;
    yDist = radius - thickness;
    // clang-format off
    rot0WrongWay << 1.0, 0.0,        0.0,         0.0,
                    0.0, cos(theta), -sin(theta), 0.0,
                    0.0, sin(theta), cos(theta),  0.0,
                    0.0, 0.0,        0.0,         1.0;
    rot1WrongWay << 1.0, 0.0,         0.0,          0.0,
                    0.0, cos(-theta), -sin(-theta), 0.0,
                    0.0, sin(-theta), cos(-theta),  0.0,
                    0.0, 0.0,         0.0,          1.0;
    rot0RightWay << cos(theta), 0.0, -sin(theta), 0.0,
                    0.0,        1.0, 0.0,         0.0,
                    sin(theta), 0.0, cos(theta),  0.0,
                    0.0,        0.0, 0.0,         1.0;
    rot1RightWay << cos(-theta), 0.0, -sin(-theta), 0.0,
                    0.0,         1.0, 0.0,          0.0,
                    sin(-theta), 0.0, cos(-theta),  0.0,
                    0.0,         0.0, 0.0,          1.0;
    // clang-format on

    for (auto &r : rings)
        r.second->set_AR(AR);

    recalculateRingTransformations(inlay);
}
