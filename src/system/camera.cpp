#include "camera.hpp"

Camera::Camera(
        float p_aspect_ratio, float p_fov, float p_near_clipping_dist, float p_far_clipping_dist
) : aspect_ratio(p_aspect_ratio), fov(p_fov), near_clipping_dist(p_near_clipping_dist),
    far_clipping_dist(p_far_clipping_dist)
{
    target = {0, 0, 0};
    angles = {0, 0, 0};
    angles[0] = -0.25f * M_PI;
    zoom_level = 2.0f;
}

int Camera::add_zoom(float zoom_increment)
{
    zoom_level = fmaxf(MIN_ZOOM, fminf(MAX_ZOOM, zoom_level - zoom_increment));

    return EXIT_SUCCESS;
}

glm::mat4 Camera::get_view_matrix()
{
    const glm::vec3 UP = {0, 1, 0};
    return glm::lookAt(get_camera_position(), target, UP);
}

glm::mat4 Camera::get_proj_matrix()
{
    return glm::perspective(fov, aspect_ratio, near_clipping_dist, far_clipping_dist);
}

glm::vec3 Camera::get_camera_position()
{
    // distance to focus point is computed as (1.2^zoomConstant)
    float dist = powf(1.2f, zoom_level);

    // pitch, roll and yaw rotation to find world position
    // calculate the rotation matrix
    glm::mat4 rotation = glm::eulerAngleYXZ(angles[1], angles[0], angles[2]);

    // apply rotation matrix
    glm::vec4 above = {0, 0, dist, 0};
    glm::vec4 pos4 = rotation * above;

    glm::vec3 position = {pos4[0], pos4[1], pos4[2]};

    return position + target;
}

void Camera::set_aspect(float p_aspect_ratio)
{
    aspect_ratio = p_aspect_ratio;
}

void Camera::translate(double offset_xpos, double offset_ypos)
{
    float PANNING_SENSITIVITY = 0.005f * zoom_level;

    // new offset based on sensitivity
    glm::vec4 offset = {
            (float) -offset_xpos * PANNING_SENSITIVITY, 0,
            (float) -offset_ypos * PANNING_SENSITIVITY, 0
    };

    // rotate the offset with the camera yaw
    glm::mat4 rotation = glm::identity<glm::mat4>();
    rotation = glm::rotate(rotation, angles[1], {0, 1, 0});
    glm::vec4 rotated_offset = rotation * offset;

    // add offset to current lookat
    target[0] += rotated_offset[0];
    target[1] += rotated_offset[1];
    target[2] += rotated_offset[2];
}

void Camera::rotate(double offset_xpos, double offset_ypos)
{
    const float ROTATION_SENSITIVITY = 0.03f;

    angles[0] += (float) -offset_ypos * ROTATION_SENSITIVITY;      // pitch
    angles[0] = fmaxf(MIN_PITCH, fminf(MAX_PITCH, angles[0])); // enforce max and min pitch
    angles[1] += (float) -offset_xpos * ROTATION_SENSITIVITY;      // yaw
}
