#include "ringglcanvas.h"
#include "tools/tool.h"

RingGLCanvas::RingGLCanvas(Widget *parent, std::shared_ptr<MailleInlay> inlay,
                           std::shared_ptr<Tool> tool)
    : nanogui::GLCanvas(parent)
    , inlay(inlay)
    , tool(tool)
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
        "    frag_color = vec4(aLight + dLight + sLight, 1.0);\n"
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

    mvp.setIdentity();
    mvp(3, 3) = 8.0;
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
    std::cout << "point: " << p << "button: " << button << "down: " << down
              << "modifiers" << modifiers << std::endl;
    std::cout << "absolute position: " << absolutePosition() << std::endl;

    if (tool)
    {
        Eigen::Vector2f pos = canvasToWorld(p);
        return tool->mouseButtonEvent(p, button, down, modifiers, pos, inlay);
    }

    return false;
}

bool RingGLCanvas::mouseDragEvent(const Eigen::Vector2i &p,
                                  const Eigen::Vector2i &rel, int button,
                                  int modifiers)
{
    std::cout << "point: " << p << "rel: " << rel << "button: " << button
              << "modifiers" << modifiers << std::endl;

    if (tool)
    {
        Eigen::Vector2f pos = canvasToWorld(p);
        return tool->mouseDragEvent(p, rel, button, modifiers, pos, inlay);
    }

    return false;
}

bool RingGLCanvas::keyboardEvent(int key, int scancode, int action,
                                 int modifiers)
{
    std::cout << "key: " << key << "scancode: " << scancode
              << "action: " << action << "modifiers: " << modifiers
              << std::endl;
    return false;
}

bool RingGLCanvas::scrollEvent(const Eigen::Vector2i &p,
                               const Eigen::Vector2f &rel)
{
    std::cout << "p: " << p << "rel: " << rel << std::endl;
    if (rel(1) > 0)
    {
        mvp(3, 3) -= 0.4;
    }
    else if (rel(1) < 0)
    {
        mvp(3, 3) += 0.4;
    }
    return false;
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
        mShader.setUniform("ringColor", inlay->rings[i]->get_color());
        mShader.setUniform("shininess", inlay->rings[i]->get_shininess());

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

Eigen::Vector2f RingGLCanvas::canvasToWorld(const Eigen::Vector2i &p)
{
    Eigen::Vector4f pos(0, 0, 0, 1);
    pos(0) = 2 * (p(0) - absolutePosition()(0)) / (float)size()(0) - 1.0;
    pos(1) = 1.0 - 2 * (p(1) - absolutePosition()(1)) / (float)size()(1);
    pos = mvp.inverse() * pos;
    pos = pos / pos(3);

    return Eigen::Vector2f(pos(0), pos(1));
}
