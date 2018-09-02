#include <unordered_map>

#include "ringglcanvas.h"
#include "tools/tool.h"

RingGLCanvas::RingGLCanvas(Widget *parent, std::shared_ptr<MailleInlay> inlay,
                           std::shared_ptr<Tool> tool)
    : nanogui::GLCanvas(parent)
    , zoom(8.0)
    , inlay(std::move(inlay))
{
    mShader.init(
        /* An identifying name */
        "a_simple_shader",

        /* Vertex shader */
        "#version 330\n"
        "uniform mat4 modelViewProj;\n"
        "uniform vec3 viewDirection;\n"
        "uniform vec3 lDirection;\n"
        "uniform vec3 ambientIntensity;\n"
        "uniform vec3 diffuseIntensity;\n"
        "uniform vec3 specularIntensity;\n"
        "uniform vec3 ringColor;\n"
        "uniform float shininess;\n"
        "uniform bool selected;\n"
        "in vec4 position;\n"
        "in vec3 normal;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "    vec3 n = normalize((modelViewProj * vec4(normal, 0.0)).xyz);\n"
        "    float diffusePrefix = max(dot(lDirection, n), 0);\n"
        "    vec3 reflection = reflect(lDirection, n);\n"
        "    float specularPrefix = dot(viewDirection, reflection);\n"
        "    specularPrefix = pow(max(specularPrefix, 0), shininess);\n"
        "    vec3 aLight = ringColor * ambientIntensity;\n"
        "    vec3 dLight = ringColor * diffuseIntensity;\n"
        "    dLight = diffusePrefix * dLight;\n"
        "    vec3 sLight = ringColor * specularIntensity;\n"
        "    sLight = specularPrefix * sLight;\n"
        "    if (!selected)\n"
        "        frag_color = vec4(0.4 * (aLight + sLight), 1.0);\n"
        "    else\n"
        "        frag_color = vec4(aLight + dLight + sLight, 1.0);\n"
        "    gl_Position = modelViewProj * position;\n"
        "}",

        /* Fragment shader */
        "#version 330\n"
        "out vec4 color;\n"
        "in vec4 frag_color;\n"
        "void main() {\n"
        "    color = frag_color;\n"
        "}");

    aIntensity = 0.6;
    dIntensity = 0.4;
    sIntensity = 0.8;
    lDirection << -0.4, 0.4, -0.2;
    lDirection.normalize();

    this->inlay->mvp.setIdentity();
    this->inlay->transformation.setIdentity();
    setZoom();
    setTool(std::move(tool));
}

void RingGLCanvas::uploadRingData()
{
    nanogui::MatrixXf positions, cur_positions;
    nanogui::MatrixXf normals, cur_normals;
    nanogui::MatrixXu indices, cur_indices;
    for (size_t i = 0; i < inlay->rings.size(); i++)
    {
        // append indices
        size_t num_cols = indices.cols();
        cur_indices = inlay->rings[i]->get_indices();
        indices.conservativeResize(3, indices.cols() + cur_indices.cols());
        indices.topRightCorner(3, cur_indices.cols()) = cur_indices;
        for (size_t x = 0; x < indices.rows(); x++)
        {
            for (size_t y = num_cols; y < indices.cols(); y++)
            {
                indices(x, y) += positions.cols();
            }
        }

        // append positions
        cur_positions = inlay->rings[i]->get_positions();
        positions.conservativeResize(4,
                                     positions.cols() + cur_positions.cols());
        positions.topRightCorner(4, cur_positions.cols()) = cur_positions;

        // append normals
        cur_normals = inlay->rings[i]->get_normals();
        normals.conservativeResize(3, normals.cols() + cur_normals.cols());
        normals.topRightCorner(3, cur_normals.cols()) = cur_normals;
    }

    mShader.bind();
    mShader.uploadIndices(indices);

    mShader.uploadAttrib("position", positions);
    mShader.uploadAttrib("normal", normals);
}

bool RingGLCanvas::mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                    bool down, int modifiers)
{
    requestFocus();

    if (tool)
    {
        Eigen::Vector2f pos = canvasToWorld(p);
        return tool->mouseButtonEvent(p, button, down, modifiers, pos, *inlay);
    }

    return true;
}

bool RingGLCanvas::mouseMotionEvent(const Eigen::Vector2i &p,
                                    const Eigen::Vector2i &rel, int button,
                                    int modifiers)
{
    requestFocus();

    return true;
}

bool RingGLCanvas::mouseDragEvent(const Eigen::Vector2i &p,
                                  const Eigen::Vector2i &rel, int button,
                                  int modifiers)
{
    if (tool)
    {
        Eigen::Vector2f pos = canvasToWorld(p);
        return tool->mouseDragEvent(p, rel, button, modifiers, pos, *inlay);
    }

    return true;
}

bool RingGLCanvas::keyboardEvent(int key, int scancode, int action,
                                 int modifiers)
{
    if (tool)
    {
        if (tool->keyboardEvent(key, scancode, action, modifiers, *inlay))
            return true;
    }

    return false;
}

bool RingGLCanvas::scrollEvent(const Eigen::Vector2i &p,
                               const Eigen::Vector2f &rel)
{
    zoom += (rel(1) > 0) ? -0.4 : 0.4;
    setZoom();
    return true;
}

void RingGLCanvas::resize(const Eigen::Vector2i &size)
{
    setSize(size);
    setZoom();
    inlay->mvp(1, 1) = (float)width() / height();
}

void RingGLCanvas::drawGL()
{
    using namespace nanogui;

    if (inlay->ringsModified)
    {
        uploadRingData();
        inlay->ringsModified = false;
    }

    mShader.bind();

    Eigen::Matrix4f mvp = inlay->mvp * inlay->transformation;
    mShader.setUniform("modelViewProj", mvp);

    // set up view direction and light color (white light, not fully on)
    Eigen::Vector3f viewDirection(0, 0, 1);
    viewDirection.normalize();
    float ai = aIntensity * inlay->ambientIntensity;
    Eigen::Vector3f ambientIntensity(ai, ai, ai);
    Eigen::Vector3f diffuseIntensity(dIntensity, dIntensity, dIntensity);
    Eigen::Vector3f specularIntensity(sIntensity, sIntensity, sIntensity);
    mShader.setUniform("ambientIntensity", ambientIntensity);
    mShader.setUniform("diffuseIntensity", diffuseIntensity);
    mShader.setUniform("specularIntensity", specularIntensity);
    mShader.setUniform("viewDirection", viewDirection);
    mShader.setUniform("lDirection", lDirection);

    glEnable(GL_DEPTH_TEST);

    uint32_t offset = 0;
    for (size_t i = 0; i < inlay->rings.size(); i++)
    {
        // set ring color and shininess
        auto c = inlay->rings[i]->get_color();
        Eigen::Vector3f color(c(0) / 255.0, c(1) / 255.0, c(2) / 255.0);
        mShader.setUniform("ringColor", color);
        mShader.setUniform("shininess", inlay->rings[i]->get_shininess());
        mShader.setUniform("selected", inlay->rings[i]->get_selected());

        unsigned count = 2 * inlay->rings[i]->get_num_samples_radius() *
                         inlay->rings[i]->get_num_samples_cross_section();
        mShader.drawIndexed(GL_TRIANGLES, offset, count);
        offset += count;
    }

    glDisable(GL_DEPTH_TEST);
}

void RingGLCanvas::setAmbientIntensityFactor(float iFactor)
{
    inlay->ambientIntensity = iFactor;
}

void RingGLCanvas::applyViewTransformation(Eigen::Matrix4f transform)
{
    inlay->mvp *= transform;
}

void RingGLCanvas::setTool(std::shared_ptr<Tool> t)
{
    if (tool)
        tool->unload(*inlay);

    tool = t;
    tool->load(*inlay);
}

Eigen::Vector2f RingGLCanvas::canvasToWorld(const Eigen::Vector2i &p)
{
    Eigen::Vector4f pos(0, 0, 0, 1);
    pos(0) = 2 * (p(0) - absolutePosition()(0)) / (float)size()(0) - 1.0;
    pos(1) = 1.0 - 2 * (p(1) - absolutePosition()(1)) / (float)size()(1);
    pos = inlay->mvp.inverse() * pos;
    pos = pos / pos(3);

    return Eigen::Vector2f(pos(0), pos(1));
}

void RingGLCanvas::setZoom() { inlay->mvp(3, 3) = zoom * width() / 600.0; }
