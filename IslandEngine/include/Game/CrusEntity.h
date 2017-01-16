#pragma once

#ifndef CRUS_ENTITY_H               // Include guard "CrusEntity.h"
#define CRUS_ENTITY_H

#include <memory>
#include <vector>
#include <map>
#include "Game\CrusObject.h"
#include "Game\CrusComponent.h"
#include "Game\CrusTransform.h"

namespace isle {
class Entity final : CrusObject {
public:

    explicit Entity();
    ~Entity() = default;

    template<typename T>
    void AddComponent();

    template<typename T>
    T *GetComponent();

    Transform const &transform() const;

    std::string ToString() const override
    {
        return{};
    }

private:

    Transform transform_;

    std::vector<uint32> componentsIDs_;
};
};

typedef int FamilyId;
struct EntitySystem;

struct Component {
    static const FamilyId familyId{1};
};

struct Entity1 {
    static EntitySystem *entitySystem;

    Entity1() {};

    template<typename Type> Type *getAs();

    std::map<FamilyId, Component*> mComponents;
};

EntitySystem *Entity1::entitySystem = 0;

template<typename Type> Type *Entity1::getAs()
{
    return entitySystem->getComponent<Type>(this);
}

struct EntitySystem {
    EntitySystem()
    {
        Entity1::entitySystem = this;
    }

    template<typename T> T *getComponent(Entity1 *e)
    {
        return (T*)e->mComponents[T::familyId];
    }

    template<typename T> void addComponent(Entity1 *e, T* comp)
    {
        mComponentStore.insert(std::pair<FamilyId, Entity1*>(T::familyId, e));
        e->mComponents.insert(std::pair<FamilyId, Component*>(T::familyId, comp));
    }

    template<typename T> void getEntities(std::vector<Entity1*> &result)
    {
        auto iterPair = mComponentStore.equal_range(T::familyId);
        for (auto iter = iterPair.first; iter != iterPair.second; ++iter) {
            result.push_back(iter->second);
        }
    }
protected:
    std::multimap<FamilyId, Entity1*> mComponentStore;
};

#endif // CRUS_ENTITY_H