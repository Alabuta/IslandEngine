#pragma once

auto constexpr kPI = 3.14159265358979323846f;


#include <memory>
#include <execution>


#define GLM_FORCE_CXX17
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_SWIZZLE
#define GLM_GTX_polar_coordinates
#define GLM_GTX_quaternion
#define GLM_GTX_transform

//#pragma warning(push, 3)
//#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/polar_coordinates.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
//#pragma warning(pop)

#include "System/CrusSystem.h"

#include "System/InputManager.hxx"
//#include "Camera/MouseHandler.hxx"


namespace isle
{
struct Camera {
    float yFOV{glm::radians(75.f)};
    float znear{.01f}, zfar{100.f};
    float aspect{1.f};

    glm::vec3 up{0, 1, 0};

    struct data_t {
        glm::mat4 view{1.f};
        glm::mat4 projection{1.f};

        glm::mat4 projectionView{1.f};

        glm::mat4 invertedView{1.f};
        glm::mat4 invertedProjection{1.f};
    } data;

    glm::mat4 world{1.f};
};

class CameraSystem {
public:

    std::shared_ptr<Camera> createCamera()
    {
        auto camera = std::make_shared<Camera>();

        cameras_.push_back(std::move(camera));

        return cameras_.back();
    }

    void update();

private:

    std::vector<std::shared_ptr<Camera>> cameras_;
};

class MouseHandler;

class OrbitController final {
public:

    OrbitController(std::shared_ptr<Camera> camera, InputManager &inputManager);

    void lookAt(glm::vec3 &&eye, glm::vec3 &&target);

    void rotate(float x, float y);
    void pan(float x, float y);
    void dolly(float delta);

    void update();

private:
    std::shared_ptr<Camera> camera_;

    std::shared_ptr<MouseHandler> mouseHandler_;

    glm::vec3 offset_{4};
    glm::vec2 polar_{0, 0}, polarDelta_{0, 0};
    glm::vec3 panOffset_{0}, panDelta_{0};
    glm::vec3 direction_{0}, directionLerped_{0};

    glm::vec3 target_{0};

    float damping_{.5f};

    glm::quat orientation_;

    float scale_{1.f};

    // latitude ands longitude
    glm::vec2 minPolar{-kPI * .49f, -kPI};
    glm::vec2 maxPolar{+kPI * .49f, +kPI};

    float minZ{.01f}, maxZ{1000.f};

    glm::vec3 const up_{0, 1, 0};

    void applyDamping();
};
}
