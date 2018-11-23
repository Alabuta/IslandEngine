#include "Camera/CameraController.hxx"
#include "Camera/MouseHandler.hxx"

template<class U, class V>
glm::quat from_two_vec3(U &&u, V &&v)
{
    auto norm_uv = std::sqrt(glm::dot(u, u) * glm::dot(v, v));
    auto real_part = norm_uv + glm::dot(u, v);

    glm::vec3 w{0};

    if (real_part < 1.e-6f * norm_uv) {
        real_part = 0.f;

        w = std::abs(u.x) > std::abs(u.z) ? glm::vec3{-u.y, u.x, 0} : glm::vec3{0, -u.z, u.y};
    }

    else w = glm::cross(u, v);

    return glm::normalize(glm::quat{real_part, w});
}


namespace isle
{
OrbitController::OrbitController(std::shared_ptr<Camera> camera, InputManager &inputManager) : camera_{camera}
{
    mouseHandler_ = std::make_shared<MouseHandler>(*this);
    inputManager.mouse().connect(mouseHandler_);
}

void OrbitController::rotate(float x, float y)
{
    log::Debug() << __FUNCTION__ << ' ' << x << '\t' << y;
}

void OrbitController::pan(float x, float y)
{
    log::Debug() << __FUNCTION__ << ' ' << x << '\t' << y;
}

void OrbitController::dolly(float delta)
{
    log::Debug() << __FUNCTION__ << ' ' << delta;
}

void OrbitController::update()
{
    auto &&world = camera_->world;

    auto const translation = glm::vec3{world[3]};

    offset_ = translation - target_;

    auto orientation = from_two_vec3(camera_->up, glm::vec3{0, 1, 0});

    auto xAxis = glm::vec3{world[0]};
    auto yAxis = glm::vec3{world[1]};
    auto zAxis = glm::vec3{world[2]};

    auto distance = glm::length(offset_);
    distance *= 2.f * std::tan(camera_->yFOV * .5f);

    xAxis *= panDelta_.x * distance;
    yAxis *= panDelta_.y * distance;

    panOffset_ = xAxis + yAxis;

    //offset_ = glm::quat::rotate(, )
    
}
