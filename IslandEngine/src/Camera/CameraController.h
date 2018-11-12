#pragma once

#include "Math\CrusMath.h"
#include "Math\CrusVector.h"
#include "Math\CrusMatrix.h"
#include "Math\CrusQuaternion.h"

#include "System\CrusSystem.h"

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

#include "MouseHandler.h"

class CameraSystem {
public:


};

class Camera {
public:
    Camera()
    {
        ;
    }

//private:
    float yFOV{glm::radians(75.f)};
    float near{.01f}, far{100.f};
    float aspect{1.f};

    glm::mat4 view{1.f};
    glm::mat4 projection{1.f};
    glm::mat4 projectionView{1.f};

    glm::mat4 world{1.f};
};


class CameraController {
public:
    CameraController();

    ~CameraController();
};

