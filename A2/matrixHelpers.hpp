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
//  - creates a rotation matrix around the x axis relative to the current frame. When right
inline glm::mat4 makeRotationMatrixX(const float & theta)
{
    // glm::mat4 is column major - i.e. columns are indexed first
    glm::mat4 xAxisRotation = glm::mat4();

    /*
     *     1      0       0      0
     *     0    cos(t) -sin(t)   0
     *     0    sin(t)  cos(t)   0
     *     0      0       0      1
     */
    xAxisRotation[1][1] = cosf(theta);
    xAxisRotation[1][2] = sinf(theta);
    xAxisRotation[2][1] = -sinf(theta);
    xAxisRotation[2][2] = cosf(theta);

    return xAxisRotation;
}

// Description:
//  - creates a rotation matrix around the y axis relative to the current frame. When right
inline glm::mat4 makeRotationMatrixY(const float & theta)
{
    // glm::mat4 is column major - i.e. columns are indexed first
    glm::mat4 yAxisRotation = glm::mat4();

    /*
     * with y towards us, z faces right, x faces up.
     * This means that we need to flip t to perform a ccw rotation, when compared to a rotation on the xy plane.
     *
     * sin is an odd f'n, cos is an even f'n, i.e. sin(-t) = -sin(t), cos(-t) = cos(t);
     *
     *   cos(t)   0   sin(t)  0
     *     0      1     0     0
     *  -sin(t)   0   cos(t)  0
     *     0      0     0     1
     */
    yAxisRotation[0][0] = cosf(theta);
    yAxisRotation[0][2] = -sinf(theta);
    yAxisRotation[2][0] = sinf(theta);
    yAxisRotation[2][2] = cosf(theta);

    return yAxisRotation;
}

// Description:
//  - creates a rotation matrix around the z axis relative to the current frame. When right
inline glm::mat4 makeRotationMatrixZ(const float & theta)
{
    // glm::mat4 is column major - i.e. columns are indexed first
    glm::mat4 zAxisRotation = glm::mat4();

    /*
     * cos(t)   -sin(t)   0   0
     * sin(t)   cos(t)    0   0
     *   0         0      1   0
     *   0         0      0   1
     */
    zAxisRotation[0][0] = cosf(theta);
    zAxisRotation[0][1] = sinf(theta);
    zAxisRotation[1][0] = -sinf(theta);
    zAxisRotation[1][1] = cosf(theta);

    return zAxisRotation;
}

// returns Rx * Ry * Rz
inline glm::mat4 makeRotationMatrixXYZ(const glm::vec3 &inputVec){
    return  makeRotationMatrixX(inputVec[0]) * makeRotationMatrixY(inputVec[1]) * makeRotationMatrixZ(inputVec[2]);
}
// returns Rz * Ry * Rx
inline glm::mat4 makeRotationMatrixZYX(const glm::vec3 &inputVec){
    return makeRotationMatrixZ(inputVec[2]) * makeRotationMatrixY(inputVec[1]) * makeRotationMatrixX(inputVec[0]) ;
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
    glm::mat4 perspectiveMatrix = glm::mat4(0.0f);

    // scale x and y based on the FOV. This controls what gets mapped to 1.
    perspectiveMatrix[0][0] = cosf(fov / 2)/sin(fov / 2);
    perspectiveMatrix[1][1] = cosf(fov / 2)/sin(fov / 2);

    // currently not mapping z
    // TODO add z mapping
    perspectiveMatrix[2][2] = 1;

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
