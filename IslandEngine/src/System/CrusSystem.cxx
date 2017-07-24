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
#include <optional>
#include <memory>
#include <map>
#include <any>
#include <set>

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

using entity_id_t = size_t;

struct Component {
    static auto constexpr id = 0;

    entity_id_t entityID;

    Component(entity_id_t entityID) : entityID(entityID) { };
};

struct AudioComponent final : public Component {
    static auto constexpr id = 1;

    float volume;

    AudioComponent(entity_id_t entityID, float volume) : Component(entityID), volume(volume) { };
};

struct MovementComponent final : Component {
    static auto constexpr id = 2;

    float initial_speed;
    float max_speed;

    MovementComponent(entity_id_t entityID, float initial_speed, float max_speed) : Component(entityID), initial_speed(initial_speed), max_speed(max_speed) { };
};

struct PhysicsComponent final : Component {
    static auto constexpr id = 3;

    float mass;

    PhysicsComponent(entity_id_t entityID, float mass) : Component(entityID), mass(mass) { };
};

struct MeshComponent final : Component {
    static auto constexpr id = 4;

    int64 meshID;

    MeshComponent(entity_id_t entityID, int64 meshID) : Component(entityID), meshID(meshID) { };
};

using component_t = std::variant<PositionComponent *, VelocityComponent *, TransformComponent *>;

class Entity final {
public:
    entity_id_t id;

    constexpr Entity(entity_id_t id) noexcept : id(id) { };
        
    /*template<class Component>
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
    }*/

    /*explicit*/ constexpr operator entity_id_t() const
    {
        return id;
    }

private:

    //std::vector<component_t> components_;
};

class Engine;

class ISystem {
public:

    virtual ~ISystem() = default;

    virtual bool Start(gsl::not_null<Engine *> engine) = 0;

    virtual void Update(float time) = 0;
};

#if _CRUS_NOT_YET_IMPLEMENTED
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
#endif

class Engine final {
public:

    std::optional<entity_id_t> CreateEntity()
    {
        return entitites.emplace_back(entitites.size()).id;
    }

    /*template<class E, typename = std::enable_if_t<std::is_same_v<Entity, std::decay_t<E>>>>
    auto constexpr GetID(entity_id_t entity)
    {
        return entity.id;
    }*/

    template<class C, class ... Args, typename = std::enable_if_t<std::is_base_of_v<Component, std::decay_t<C>>>>
    std::optional<C> AddComponent(Entity entity, Args &&...args)
    {
        /*if constexpr (std::is_same_v<C, AudioComponent>)
            return audioComponents_.emplace_back(entity.id, std::forward<Args>(args)...);

        else if constexpr (std::is_same_v<C, MovementComponent>)
            return movementComponents_.emplace_back(entity.id, std::forward<Args>(args)...);

        else if constexpr (std::is_same_v<C, PhysicsComponent>)
            return physicsComponents_.emplace_back(entity.id, std::forward<Args>(args)...);

        else if constexpr (std::is_same_v<C, MeshComponent>)
            return meshComponents_.emplace_back(entity.id, std::forward<Args>(args)...);

        else
            static_assert(always_false<C>::value, "unsupported component type");*/

        auto it = components_.try_emplace(C::id, std::vector<C>()).first;

        auto &container = std::any_cast<std::vector<C>>(it->second);

        return container.emplace_back(entity.id, std::forward<Args>(args)...);
    }

    template<class C, typename = std::enable_if_t<std::is_base_of_v<Component, std::decay_t<C>>>>
    std::optional<std::vector<C> *> GetComponents()
    {
        if (auto it = components_.find(C::id); it != components_.end())
            return std::any_cast<std::vector<C>>(&(it->second));

        else return std::nullopt;
    }

    void AddSystem(uint16 priority, std::unique_ptr<ISystem> system)
    {
        systems_.emplace(priority, std::move(system));
    }

    bool Start()
    {
        for (auto &system : systems_)
            if(!system.second->Start(this))
                return false;

        return true;
    }

    void Update(float time)
    {
        for (auto &system : systems_)
            system.second->Update(time);
    }

private:
    std::vector<Entity> entitites;

    /*using node_t = std::variant<RenderSystem::Node, MoveSystem::Node>;

    std::vector<node_t> nodes;

    std::vector<PositionComponent> positionComponents;
    std::vector<VelocityComponent> velocityComponents;
    std::vector<TransformComponent> transformComponents;*/

    /*std::vector<AudioComponent> audioComponents_;
    std::vector<MovementComponent> movementComponents_;
    std::vector<PhysicsComponent> physicsComponents_;
    std::vector<MeshComponent> meshComponents_;*/

    std::map<uint16, std::unique_ptr<ISystem>> systems_;
    std::map<decltype(Component::id), std::any> components_;
};

class AudioSystem final : public ISystem {
public:

    bool Start(gsl::not_null<Engine *> engine) override
    {
        if (auto components = engine->GetComponents<AudioComponent>(); components) {
            audioComponents_ = components.value();
            return true;
        }

        else return false;
    }

    void Update(float time) override
    {
        isle::log::Debug() << __FUNCTION__;

        for (auto &component : *audioComponents_)
            component.volume += 2.f;

        for (auto &component : *audioComponents_)
            isle::log::Debug() << "volume " << component.volume;
    }

private:

    std::vector<AudioComponent> *audioComponents_;
};

class PhysicsSystem final : public ISystem {
public:

    bool Start(gsl::not_null<Engine *> engine) override
    {
        if (auto components = engine->GetComponents<PhysicsComponent>(); components) {
            physicsComponents_ = components.value();
            return true;
        }

        else return false;
    }

    void Update(float time) override
    {
        isle::log::Debug() << __FUNCTION__;

        for (auto &component : *physicsComponents_)
            component.mass += 2.f;

        for (auto &component : *physicsComponents_)
            isle::log::Debug() << "volume " << component.mass;
    }

private:

    std::vector<PhysicsComponent> *physicsComponents_;
};

namespace isle {
Time System::time;

std::map<decltype(Component::id), std::any> components_;


template<class C, class ... Args, typename = std::enable_if_t<std::is_base_of_v<Component, std::decay_t<C>>>>
std::optional<C *const> AddComponent(Entity entity, Args &&...args)
{
    auto it = components_.try_emplace(C::id, std::make_any<std::vector<C>>(16)).first;

    auto container = std::any_cast<std::vector<C>>(&(it->second));

    return &container->emplace_back(entity.id, std::forward<Args>(args)...);
}

template<class C, typename = std::enable_if_t<std::is_base_of_v<Component, std::decay_t<C>>>>
std::optional<std::vector<C> *> GetComponents()
{
    if (auto it = components_.find(C::id); it != components_.end())
        return std::any_cast<std::vector<C>>(&(it->second));

    else return std::nullopt;
}

/*static*/ void System::Setup()
{
    Engine engine;

    auto entity = engine.CreateEntity();

    auto audioComp = engine.AddComponent<AudioComponent>(entity.value(), 64.f);
    auto physicsComp = engine.AddComponent<PhysicsComponent>(entity.value(), 2.f);

    engine.AddSystem(2, std::make_unique<AudioSystem>());
    engine.AddSystem(1, std::make_unique<PhysicsSystem>());

    /*log::Debug() << "volume " << audioComp->volume;
    log::Debug() << "mass " << physicsComp->mass;

    //auto bStartSucceeded = engine.Start();

    //log::Debug() << "bStartSucceeded: " << std::boolalpha << bStartSucceeded;

    //engine.Update(1.f);

    log::Debug() << "volume " << audioComp->volume;
    log::Debug() << "mass " << physicsComp->mass;*/

    //components_.emplace(AudioComponent::id, std::make_any<std::vector<AudioComponent>>());

    auto x = AddComponent<AudioComponent>(entity.value(), 64.f);

    if (x)
        x.value()->volume = 2.f;

    std::vector<AudioComponent> *audioComponent = GetComponents<AudioComponent>().value_or(nullptr);

    /*if (audioComponent != nullptr)
        audioComponent->emplace_back(entity.value(), 64.f);*/

    if (audioComponent != nullptr)
        for (auto const &ac : *audioComponent)
            log::Debug() << "volume " << ac.volume;

    AddComponent<AudioComponent>(entity.value(), 4.f);

    if (x)
        x.value()->volume = 8.f;

    if (audioComponent != nullptr)
        for (auto const &ac : *audioComponent)
            log::Debug() << "volume " << ac.volume;

    /*auto vec = std::any_cast<std::vector<AudioComponent>>(&components_.at(AudioComponent::id));

    vec->back().volume += 20.f;

    log::Debug() << "volume " << vec->back().volume;

    if (auto it = components_.find(AudioComponent::id); it != components_.end()) {
        optional = std::any_cast<std::vector<AudioComponent>>(&(it->second));
    }

    if (optional)
        log::Debug() << "volume " << optional.value()->back().volume;*/

    /*auto any = std::make_any<std::vector<AudioComponent>>();

    auto vec = std::any_cast<std::vector<AudioComponent>>(&any);

    vec->emplace_back(entity.value(), 64.f);

    log::Debug() << "volume " << vec->back().volume;
    log::Debug() << "volume " << std::any_cast<std::vector<AudioComponent>>(any).back().volume;*/


    /*entity->AddComponent(audioComp);

    engine.AddEntity(std::move(entity));*/

    /*log::Debug() << "component_t " << sizeof(component_t);
    log::Debug() << "PositionComponent " << sizeof(PositionComponent);
    log::Debug() << "VelocityComponent " << sizeof(VelocityComponent);
    log::Debug() << "TransformComponent " << sizeof(TransformComponent);
    log::Debug() << "PhysicsComponent " << sizeof(PhysicsComponent);*/

    //log::Debug() << "MoveSystem " << sizeof(MoveSystem);


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