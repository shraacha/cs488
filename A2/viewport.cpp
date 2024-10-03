#include "viewport.hpp"
#include "constants.hpp"

// returns viewport transformation matrix from V to W
glm::mat4 makeViewportTransformationMatrix(const Viewport & V, const Viewport & W)
{
    glm::mat4 transformation = glm::mat4(); // start with identity matrix

    // avoiding div by 0
    //
    // This is kinda wrong, but we'll roll with it, just don't transform from a
    // 0 width/height viewport.

    float LRatio = (V.L == 0.0f) ? 0.0f : W.L / V.L;
    float HRatio = (V.H == 0.0f) ? 0.0f : W.H / V.H;

    transformation[0][0] = LRatio;
    transformation[1][1] = HRatio;

    transformation[3][0] =  -LRatio * V.x + W.x;
    transformation[3][1] =  -HRatio * V.y + W.y;

    return transformation;
}

std::vector<line4> getViewportExplicitEdges(const Viewport & V)
{
    std::vector<line4> edges;

    glm::vec4 tl = {V.x, V.y, 0.0f, 1.0f};
    glm::vec4 tr = {V.x + V.L, V.y, 0.0f, 1.0f};
    glm::vec4 bl = {V.x, V.y + V.H, 0.0f, 1.0f};
    glm::vec4 br = {V.x + V.L, V.y + V.H, 0.0f, 1.0f};

    edges.emplace_back(std::make_pair(tl, tr));  //top
    edges.emplace_back(std::make_pair(tl, bl));  //left
    edges.emplace_back(std::make_pair(bl, br));  //bottom
    edges.emplace_back(std::make_pair(tr, br));  //right

    return edges;
}

std::vector<pointAndNormal> getViewportImplicitEdges(const Viewport & V)
{
    std::vector<line4> edges;

    glm::vec4 tl = {V.x, V.y, 0.0f, 1.0f};
    glm::vec4 tr = {V.x + V.L, V.y, 0.0f, 1.0f};
    glm::vec4 bl = {V.x, V.y + V.H, 0.0f, 1.0f};
    glm::vec4 br = {V.x + V.L, V.y + V.H, 0.0f, 1.0f};

    edges.emplace_back(std::make_pair(tl, bl - tl)); //top
    edges.emplace_back(std::make_pair(tl, tr - tl)); //left
    edges.emplace_back(std::make_pair(br, tr - br)); //bottom
    edges.emplace_back(std::make_pair(br, bl - br)); //right

    return edges;
}

line4 makeDeviceViewportCornersFromPortion(const float & width, const float & height, const float & portion)
{
    float L = width * portion;
    float H = height * portion;
    float x = (width - L) / 2;
    float y = (height - H) / 2;

    return {{x, y, 0.0f, 1.0f}, {x + L, y + H, 0.0f, 1.0f}};
}

Viewport makeDeviceViewport(const float & width, const float & height)
{
    return {0.0f, 0.0f, width, height};
}

// two diagonally separated corners of the rectangular region
Viewport makeNDCViewportFromCorners(const line4 & corners)
{
    float x, y, L, H;

    float x1 = corners.first[0];
    float x2 = corners.second[0];
    float y1 = corners.first[1];
    float y2 = corners.second[1];

    if (x1 <= x2) {
        x = x1;
        L = x2 - x1;
    } else {
        x = x2;
        L = x1 - x2;
    }

    if (y1 >= y2) {
        y = y1;
        H = y2 - y1;
    } else {
        y = y2;
        H = y1 - y2;
    }

    return {x, y, L, H};
}
