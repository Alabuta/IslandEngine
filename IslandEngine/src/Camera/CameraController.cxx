#include <execution>
#include <iomanip>

#include "Camera/CameraController.hxx"
#include "Camera/MouseHandler.hxx"


template<class U, class V>
glm::quat fromTwoVec3(U &&u, V &&v)
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
void CameraSystem::update()
{
    std::for_each(std::execution::par_unseq, std::begin(cameras_), std::end(cameras_), [] (auto &&camera)
    {
        camera->data.projection = glm::infinitePerspective(camera->yFOV, camera->aspect, camera->znear);
        camera->data.invertedProjection = glm::inverse(camera->data.projection);

        camera->data.view = glm::inverse(camera->world);
        camera->data.invertedView = glm::inverse(camera->data.view);

        camera->data.projectionView = camera->data.projection * camera->data.view;
    });
}

OrbitController::OrbitController(std::shared_ptr<Camera> camera, InputManager &inputManager) : camera_{camera}
{
    mouseHandler_ = std::make_shared<MouseHandler>(*this);
    inputManager.mouse().connect(mouseHandler_);
}

void OrbitController::lookAt(glm::vec3 &&eye, glm::vec3 &&target)
{
    target_ = target;
    offset_ = eye;

    auto &&world = camera_->world;

    world = glm::inverse(glm::lookAt(offset_, target_, {0, 1, 0}));
}

void OrbitController::rotate(float longitude, float latitude)
{
    auto speed = (1.f - damping_) * .008f;

    polarDelta_.x += latitude * speed;
    polarDelta_.y -= longitude * speed;
}

void OrbitController::pan(float x, float y)
{
    auto speed = (1.f - damping_) * .001f;

    panDelta_.x -= x * speed;
    panDelta_.y += y * speed;
}

void OrbitController::dolly(float delta)
{
    auto speed = (1.f - damping_) * 2.f;

    auto dollying = std::pow(.95f, std::abs(delta) * speed);

    scale_ = std::signbit(delta) ? (std::signbit(delta) ? 1.f / dollying : 1.f) : dollying;
}

void OrbitController::update()
{
    auto &&world = camera_->world;

    auto xAxis = glm::vec3{world[0]};
    auto yAxis = glm::vec3{world[1]};
    auto zAxis = glm::vec3{world[2]};

    auto position = glm::vec3{world[3]};

    offset_ = position - target_;

    auto radius = glm::length(offset_);
    auto distance = radius * 2.f * std::tan(camera_->yFOV * .5f);

    radius = std::clamp(radius * scale_, minZ, maxZ);

    xAxis *= panDelta_.x * distance;
    yAxis *= panDelta_.y * distance;

    panOffset_ = xAxis + yAxis;

    polar_ = glm::polar(offset_);
    polar_ = glm::clamp(polar_ + polarDelta_, minPolar, maxPolar);

    offset_ = glm::euclidean(polar_) * radius;

    /*auto yaw = std::atan2(zAxis.x, zAxis.z);

    orientation_ = glm::angleAxis(yaw, up_);

    zAxis = orientation_ * (directionLerped_ * glm::vec3{1, 0, -1});

    panOffset_ += zAxis * std::max(.1f, std::log(std::abs(distance) + 1.f));*/

    target_ += panOffset_;

    position = target_ + offset_;

    world = glm::inverse(glm::lookAt(position, target_, up_));


#if 0
    auto orientation = from_two_vec3(camera_->up, glm::vec3{0, 1, 0});
#endif

    applyDamping();
}

void OrbitController::applyDamping()
{
    polarDelta_ *= damping_;

    panDelta_ *= damping_;

    scale_ += (1 - scale_) * (1 - damping_);

    directionLerped_ = glm::mix(directionLerped_, direction_, 1.f - damping_);
}
}
