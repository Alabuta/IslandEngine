#include "System\CrusSystem.h"
#include "Game\CrusEntity.h"


namespace isle {
Entity::Entity()
{
    ;
}

template<typename T>
inline void Entity::AddComponent()
{
    ;
}

template<typename T>
inline T *Entity::GetComponent()
{
    ;
}
}

void f()
{
    Entity1 e;
    Component c;
    Entity1::entitySystem->addComponent<Component>(&e, &c);

    auto comp = Entity1::entitySystem->getComponent<Component>(&e);
}