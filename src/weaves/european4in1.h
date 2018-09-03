#pragma once

#include <map>

#include "common.h"
#include "weave.h"

class European4in1 : public Weave
{
public:
    European4in1(float radius = 1.0, float thickness = 0.278);

    void addRing(const Eigen::Vector2f &worldClickLoc,
                 const Maille::Color &color, MailleInlay &inlay);
    void addRingsInArea(const Eigen::Vector2f &begin,
                        const Eigen::Vector2f &end, const Maille::Color &color,
                        MailleInlay &inlay);
    void deleteRing(const Eigen::Vector2f &worldClickLoc, MailleInlay &inlay);
    std::shared_ptr<cpptoml::table>
        generateSaveFile(const MailleInlay &inlay) const;
    bool importSaveFile(std::shared_ptr<cpptoml::table> design,
                        MailleInlay &inlay);
    static std::string getWeaveID() { return weaveID; }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
    std::pair<int, int> nearestRing(const Eigen::Vector2f &loc);
    bool addRingByIndex(const std::pair<int, int> &index,
                        const Maille::Color &color, MailleInlay &inlay);
    static const std::string weaveID;
    static const int VERSION;
    Eigen::Matrix4f rot0, rot1;
    float theta;
    float xDist;
    float yDist;

    // Rings are indexed by their (x, y) coordinate in the weave. The rotation
    // of a ring can be determined based on its x coordinate (even/odd).
    std::map<std::pair<int, int>, std::shared_ptr<Torus>> rings;
};
