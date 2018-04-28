#pragma once

#include <map>

#include "weave.h"

class European4in1 : public Weave
{
public:
    European4in1(float radius = 1.0, float thickness = 0.278);

    void addRing(const Eigen::Vector2f &worldClickLoc,
                 const Eigen::Vector3f &color, MailleInlay &inlay);
    void deleteRing(const Eigen::Vector2f &worldClickLoc, MailleInlay &inlay);

private:
    std::pair<int, int> nearestRing(const Eigen::Vector2f &loc);
    Eigen::Matrix4f rot0, rot1;
    float radius;
    float theta;
    float thickness;

    // Rings are indexed by their (x, y) coordinate in the weave. The rotation
    // of a ring can be determined based on its x coordinate (even/odd).
    std::map<std::pair<int, int>, std::shared_ptr<Torus>> rings;
};
