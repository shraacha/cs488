#pragma once

#include <glm/glm.hpp>

// Params:
//  - translatioVector[0] - translation in the x dir, relative to to current frame
//  - translatioVector[1] - ... y dir ...
//  - translatioVector[2] - ... z dir ...
inline glm::mat4 makeTranslationMatrix(const glm::vec3 & inputVec)
{
    glm::mat4 translationMatrix = glm::mat4();
    for(int i = 0; i < 3; i ++) {
        // glm::mat4 is column major - i.e. columns are indexed first
        translationMatrix[3][i] = inputVec[i];
    }

    return translationMatrix;
}

// Description:
//  - creates a rotation matrix relative to the current frame. When right
//  multiplied by a matrix, the z-axis rotation is applied first, then y, then x.
// Params:
//  - inputVec[0] - radians to rotate counterclockwise around the x-axis, relative to to
//  current frame
//  - inputVec[1] - ... y-axis ...
//  - inputVec[2] - ... z-axis ...
inline glm::mat4 makeRotationMatrix(const glm::vec3 &inputVec)
{
    // glm::mat4 is column major - i.e. columns are indexed first
    glm::mat4 xAxisRotation = glm::mat4();
    glm::mat4 yAxisRotation = glm::mat4();
    glm::mat4 zAxisRotation = glm::mat4();

    // x
    /*
     *     1      0       0      0
     *     0    cos(t) -sin(t)   0
     *     0    sin(t)  cos(t)   0
     *     0      0       0      1
     */
    xAxisRotation[1][1] = cosf(inputVec[0]);
    xAxisRotation[1][2] = sinf(inputVec[0]);
    xAxisRotation[2][1] = -sinf(inputVec[0]);
    xAxisRotation[2][2] = cosf(inputVec[0]);

    // y
    /*
     * with z up, x to the right, y faces forward, not towards us.
     * This means that we need to flip t when compared to a rotation wrt x & y.
     * sin is an odd f'n, cos is an even f'n
     *
     *   cos(t)   0   sin(t)  0
     *     0      1     0     0
     *  -sin(t)   0   cos(t)  0
     *     0      0     0     1
     */
    yAxisRotation[0][0] = cosf(inputVec[1]);
    yAxisRotation[0][2] = -sinf(inputVec[1]);
    yAxisRotation[2][0] = sinf(inputVec[1]);
    yAxisRotation[2][2] = cosf(inputVec[1]);

    // z
    /*
     * cos(t)   -sin(t)   0   0
     * sin(t)   cos(t)    0   0
     *   0         0      1   0
     *   0         0      0   1
     */
    zAxisRotation[0][0] = cosf(inputVec[2]);
    zAxisRotation[0][1] = sinf(inputVec[2]);
    zAxisRotation[1][0] = -sinf(inputVec[2]);
    zAxisRotation[1][1] = cosf(inputVec[2]);

    return xAxisRotation * yAxisRotation * zAxisRotation;
}

//  Description:
//  - creates a scaling matrix relative to the current frame.
// Params:
//  - inputVec[0] - factor to scale in the x-axis
//  - inputVec[1] - ... y-axis ...
//  - inputVec[2] - ... z-axis ...
inline glm::mat4 makeScaleMatrix(const glm::vec3 &inputVec)
{
    glm::mat4 scaleMatrix = glm::mat4();

    // glm::mat4 is column major - i.e. columns are indexed first
    scaleMatrix[0][0] = inputVec[0];
    scaleMatrix[1][1] = inputVec[1];
    scaleMatrix[2][2] = inputVec[2];

    return scaleMatrix;
}

inline glm::mat4 makePerspectiveMatrix(const float & fov)
{
    glm::mat4 perspectiveMatrix = glm::mat4();

    // scale x and y based on the FOV. This controls what gets mapped to 1.
    perspectiveMatrix[0][0] = cosf(fov / 2)/sin(fov / 2);
    perspectiveMatrix[1][1] = cosf(fov / 2)/sin(fov / 2);

    // currently not mapping z
    // TODO add z mapping

    // replace w with z
    perspectiveMatrix[2][3] = -1; // camera is looking down -z, so we flip it

    perspectiveMatrix[3][3] = 0;
    return perspectiveMatrix;
}

inline glm::mat4 makeOrthographicMatrix()
{
    glm::mat4 orthographicMatrix = glm::mat4();
    orthographicMatrix[2][3] = -1;
    orthographicMatrix[3][3] = 0;

    return orthographicMatrix;
}
