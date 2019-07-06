#pragma once

#include <map>

#include "common.h"
#include "weave.h"

class European4in1 : public Weave
{
public:
    European4in1(nanogui::Window *weaveSettings,
                 std::shared_ptr<MailleInlay> inlay);

    void addRing(const Eigen::Vector2f &worldClickLoc,
                 const Maille::Color &color, MailleInlay &inlay) override;
    void addRingsInArea(const Eigen::Vector2f &begin,
                        const Eigen::Vector2f &end, const Maille::Color &color,
                        MailleInlay &inlay) override;
    void deleteRing(const Eigen::Vector2f &worldClickLoc,
                    MailleInlay &inlay) override;
    std::shared_ptr<cpptoml::table>
        generateSaveFile(const MailleInlay &inlay) const override;
    void importSaveFile(nanogui::Widget *parent,
                        std::shared_ptr<cpptoml::table> design,
                        MailleInlay &inlay) override;
    void importImage(nanogui::Widget *parent, std::string fpath,
                     MailleInlay &inlay) override;
    std::string exportPOV(const MailleInlay &inlay) const override;
    static std::string getWeaveID() { return weaveID; }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
    void importV0File(nanogui::Widget *parent,
                      std::shared_ptr<cpptoml::table> design,
                      MailleInlay &inlay);
    void importLatestFile(nanogui::Widget *parent,
                          std::shared_ptr<cpptoml::table> design,
                          MailleInlay &inlay);
    std::pair<int, int> nearestRing(const Eigen::Vector2f &loc);
    bool addRingByIndex(const std::pair<int, int> &index,
                        const Maille::Color &color, MailleInlay &inlay);
    void swapWay(MailleInlay &inlay);
    void recalculateRingTransformations(MailleInlay &inlay);
    void resetARValues(MailleInlay &inlay);
    std::pair<float, float> idxToPos(const std::pair<int, int> &idx) const;
    const Eigen::Matrix4f &idxToRot(const std::pair<int, int> &idx) const;

    static const std::string weaveID;
    static const int VERSION;

    nanogui::CheckBox *wrongWay;
    nanogui::CheckBox *rotateInlay;
    nanogui::FloatBox<float> *ar;
    const float radius = 1.0;
    float thickness;
    float theta;
    float xDist;
    float yDist;
    Eigen::Matrix4f rot0WrongWay;
    Eigen::Matrix4f rot0RightWay;
    Eigen::Matrix4f rot1WrongWay;
    Eigen::Matrix4f rot1RightWay;

    // Rings are indexed by their (x, y) coordinate in the weave. The rotation
    // of a ring can be determined based on its x coordinate (even/odd) for
    // wrong way, or y coordinate for right way.
    std::map<std::pair<int, int>, std::shared_ptr<Torus>> rings;
};
