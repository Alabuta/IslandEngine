#pragma once

#include <memory>
#include <execution>


#define GLM_FORCE_CXX17
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#pragma warning(push, 3)
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#pragma warning(pop)

#include "System\CrusSystem.h"

#include "Camera/MouseHandler.h"

struct Camera {

    float yFOV{glm::radians(75.f)};
    float near{.01f}, far{100.f};
    float aspect{1.f};

    glm::mat4 view{1.f};
    glm::mat4 projection{1.f};
    glm::mat4 projectionView{1.f};

    glm::mat4 world{1.f};
};

class CameraSystem {
public:

    std::shared_ptr<Camera> CreateCamera()
    {
        return cameras_.emplace_back();
    }

    void Update()
    {
        std::for_each(std::execution::par_unseq, std::begin(cameras_), std::end(cameras_), [] (auto &&camera)
        {
            camera->projectionView = camera->projection * camera->view;
        });
    }

private:

    std::vector<std::shared_ptr<Camera>> cameras_;
};


class CameraController {
public:
    CameraController(std::shared_ptr<Camera> camera) : camera_{camera} { }

private:
    std::shared_ptr<Camera> camera_;
};

