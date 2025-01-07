#include "geometry.h"

bool geometry::WorldToScreen(Vector3 worldCoords, Vector2& screenCoords, float matrix[16], int windowWidth, int windowHeight) {
    // matrix-vector product: multiplying world(eye) coordinates by projection matrix = clipCoords
    Vector4 clipCoords;
    clipCoords.x = worldCoords.x * matrix[0] + worldCoords.y * matrix[4] + worldCoords.z * matrix[8] + matrix[12];
    clipCoords.y = worldCoords.x * matrix[1] + worldCoords.y * matrix[5] + worldCoords.z * matrix[9] + matrix[13];
    clipCoords.z = worldCoords.x * matrix[2] + worldCoords.y * matrix[6] + worldCoords.z * matrix[10] + matrix[14];
    clipCoords.w = worldCoords.x * matrix[3] + worldCoords.y * matrix[7] + worldCoords.z * matrix[11] + matrix[15];

    if (clipCoords.w < 0.1f)
        return false;

    // perspective division, dividing by clip.W = Normalized Device Coordinates
    Vector3 NDC;
    NDC.x = clipCoords.x / clipCoords.w;
    NDC.y = clipCoords.y / clipCoords.w;
    NDC.z = clipCoords.z / clipCoords.w;

    screenCoords.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
    screenCoords.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);

    return true;
}