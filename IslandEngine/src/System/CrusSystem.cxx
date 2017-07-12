/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: implementation of system's class file.
****
********************************************************************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <variant>
#include <memory>
#include <map>
#include <any>

#include "Renderer\CrusPosition.h"

#if _DEBUG
#define _CRUS_MEMORY_CONTROL        1   // Use this to check for memory leaks!
#endif

#if _CRUS_MEMORY_CONTROL
#   define _CRTDBG_MAP_ALLOC
#   include <crtdbg.h>
#endif // _CRUS_MEMORY_CONTROL

#include "System\CrusSystem.h"
#include "System\CrusWindow.h"

#include "System\CrusInput.h"

#include "Renderer\CrusRender.h"
#include "Camera\CrusCamera.h"

/*std::ostream &operator<< (std::ostream &stream, isle::math::Vector const &vector)
{
    return stream << vector.x() << "; " << vector.y() << "; " << vector.z();
}*/

/*template<typename... Ts>
struct overloader : Ts... {
    using Ts::operator()...;
};

template<typename... Ts>
overloader(Ts...)->overloader<Ts...>;*/

struct PositionComponent {
    isle::math::Vector position;

    template<class T, typename = std::enable_if_t<std::is_same_v<isle::math::Vector, std::decay_t<T>>>>
    PositionComponent(T &&pos) : position(std::forward<T>(pos)) { };
};

struct VelocityComponent {
    isle::math::Vector linear_velocity;

    template<class T, typename = std::enable_if_t<std::is_same_v<isle::math::Vector, std::decay_t<T>>>>
    VelocityComponent(T &&vel) : linear_velocity(std::forward<T>(vel)) { };
};

struct TransformComponent {
    isle::math::Matrix transform;
};

struct Component {
    static auto constexpr id = 0;
};

struct AudioComponent : Component {
    static auto constexpr id = 1;

    float volume;
};

struct MovementComponent : Component {
    static auto constexpr id = 2;

    float initial_speed;
    float max_speed;
};

struct PhysicsComponent : Component {
    static auto constexpr id = 3;

    float mass;
};

struct MeshComponent : Component {
    static auto constexpr id = 4;

    int64 meshID;
};

using component_t = std::variant<PositionComponent *, VelocityComponent *, TransformComponent *>;

class Entity final {
public:
    

    template<class Component>
    void AddComponent1(Component &&component)
    {
        components_.push_back(std::forward<Component>(component));
    }

    template<class T, class ... Args, typename = std::enable_if_t<std::is_base_of_v<Component, std::decay_t<T>>>>
    auto AddComponent(Args &&...args)
    {
        if constexpr (std::is_same_v<T, AudioComponent>)
            return &audioComponents_.emplace_back(std::forward<Args>(args)...);

        else if constexpr (std::is_same_v<T, MovementComponent>)
            return &movementComponents_.emplace_back(std::forward<Args>(args)...);

        else if constexpr (std::is_same_v<T, PhysicsComponent>)
            return &physicsComponents_.emplace_back(std::forward<Args>(args)...);

        else if constexpr (std::is_same_v<T, MeshComponent>)
            return &meshComponents_.emplace_back(std::forward<Args>(args)...);

        else
            static_assert(always_false<T>::value, "unsupported component type");
    }

private:

    std::vector<component_t> components_;
};

class ISystem {
public:

    virtual ~ISystem() = default;

    virtual void Update(float time) = 0;

    void AddNode()
    {
        isle::log::Debug() << __FUNCTION__;
    }
};

class RenderSystem final : public ISystem {
public:

    struct Node {
        PositionComponent *positionComp;
    };

    void AddNode()
    {
        isle::log::Debug() << __FUNCTION__;
    }

    void Update(float time) final
    {
        isle::log::Debug() << __FUNCTION__;

        /*for (auto const &node : nodes_)
            if (auto locked = node.positionComp.lock(); locked)
                isle::log::Debug() << "position: " << locked;*/

        /*for (auto const &node : nodes_)
            isle::log::Debug() << "position: " << node.positionComp->position;*/
    }

private:

    std::vector<Node> nodes_;
};

class MoveSystem final : public ISystem {
public:

    struct Node {
        PositionComponent *positionComp;
        VelocityComponent *velocityComp;
    };

    void AddNode()
    {
        isle::log::Debug() << __FUNCTION__;
    }

    void Update(float time) final
    {
        isle::log::Debug() << __FUNCTION__;

        /*for (auto const &node : nodes_)
            if (auto position = node.positionComp.lock(); position)
                if (auto velocity = node.velocityComp.lock(); velocity)
                    position->position += velocity->linear_velocity * time;*/

        for (auto const &node : nodes_)
            node.positionComp->position += node.velocityComp->linear_velocity * time;
    }

private:

    std::vector<Node> nodes_;
};

class Engine {
public:

    template<class T, class ... Args, typename = std::enable_if_t<std::is_base_of_v<Component, std::decay_t<T>>>>
    auto CreateComponent(Args &&...args)
    {
        if constexpr (std::is_same_v<T, AudioComponent>)
            return &audioComponents_.emplace_back(std::forward<Args>(args)...);

        else if constexpr (std::is_same_v<T, MovementComponent>)
            return &movementComponents_.emplace_back(std::forward<Args>(args)...);

        else if constexpr (std::is_same_v<T, PhysicsComponent>)
            return &physicsComponents_.emplace_back(std::forward<Args>(args)...);

        else if constexpr (std::is_same_v<T, MeshComponent>)
            return &meshComponents_.emplace_back(std::forward<Args>(args)...);

        else
            static_assert(always_false<T>::value, "unsupported component type");
    }

    void AddEntity(Entity &&entity)
    {
        /*auto components = entity->GetComponents();

        for (auto &component : components)
            std::visit([] (auto &&comp)
        {
            using T = std::decay_t<decltype(comp)>;

            if constexpr (std::is_same_v<T, PositionComponent *>)
                isle::log::Debug() << "PositionComponent";

            else if constexpr (std::is_same_v<T, VelocityComponent *>)
                isle::log::Debug() << "VelocityComponent";

            else if constexpr (std::is_same_v<T, TransformComponent *>)
                isle::log::Debug() << "TransformComponent";

            else
                static_assert(always_false<T>::value, "non-exhaustive visitor!");

        }, component);*/

        entitites.push_back(std::move(entity));

        //systems[0]->AddNode();
    }

    void AddSystem(uint16 priority, std::unique_ptr<ISystem> &&system)
    {
        systems.emplace(priority, std::move(system));
    }

    void Update(float time)
    {
        for (auto &system : systems)
            system.second->Update(time);
    }

private:
    std::vector<Entity> entitites;
    std::map<uint16, std::unique_ptr<ISystem>> systems;

    /*using node_t = std::variant<RenderSystem::Node, MoveSystem::Node>;

    std::vector<node_t> nodes;

    std::vector<PositionComponent> positionComponents;
    std::vector<VelocityComponent> velocityComponents;
    std::vector<TransformComponent> transformComponents;*/

    std::vector<AudioComponent> audioComponents_;
    std::vector<MovementComponent> movementComponents_;
    std::vector<PhysicsComponent> physicsComponents_;
    std::vector<MeshComponent> meshComponents_;
};

namespace isle {
Time System::time;

/*static*/ void System::Setup()
{
    auto entity = std::make_unique<Entity>();

    Engine engine;

    auto audioComp = engine.CreateComponent<AudioComponent>(64.f);

    entity->AddComponent(audioComp);

    engine.AddEntity(std::move(entity));

    engine.AddSystem(0, std::make_unique<MoveSystem>());
    engine.AddSystem(1, std::make_unique<RenderSystem>());

    engine.Update(1.f);

    log::Debug() << "component_t " << sizeof(component_t);
    log::Debug() << "PositionComponent " << sizeof(PositionComponent);
    log::Debug() << "VelocityComponent " << sizeof(VelocityComponent);
    log::Debug() << "TransformComponent " << sizeof(TransformComponent);
    log::Debug() << "PhysicsComponent " << sizeof(PhysicsComponent);

    log::Debug() << "MoveSystem " << sizeof(MoveSystem);

    // This method for only start of system and used before splash screen creation.
    HWND const hExistWnd = FindWindowW(crus::names::kMAIN_WINDOW_CLASS, nullptr);

    if (hExistWnd != nullptr) {
        SetForegroundWindow(hExistWnd);
        ShowWindow(hExistWnd, SW_SHOWNORMAL);

        ::_exit(EXIT_SUCCESS);
    }

    //::atexit(System::Destroy);

#if _CRUS_MEMORY_CONTROL
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    //_CrtSetBreakAlloc(84);
#endif
}

/*static*/ void System::Destroy()
{
    log::Info() << "application destroyed.";
}

/*static*/ int32 System::Loop()
{
    MSG msg;
    time.Restart();

    // :COMPILER: just to shut up the compiler.
#pragma warning(push, 3)
#pragma warning(disable: 4127)
    while (true) {
#pragma warning(pop)
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE /*| PM_NOYIELD*/) != 0) {
            if (msg.message == WM_QUIT)
                return static_cast<int32>(msg.wParam);

            //TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (Window::main().InFocus()) {
            System::Update();
            Render::inst().DrawFrame();
        }

        else WaitMessage();
    }

    return 0;
}

/*static*/ void System::Update()
{
    time.Update();

    app::Update();
}
};