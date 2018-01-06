#include <cassert>
#include <iostream>
#include "base_component.h"
#include "base_system.h"
#include "ecs_define.h"
#include "ecs_iterator.h"
#include "entity_admin.h"
using namespace ecs;

class MovementComponent : public BaseComponent
{
public:
    ~MovementComponent() { std::cout << __FUNCTION__ << std::endl; }

    void Reset(float velocity) { this->velocity = velocity; }

    void Print() { std::cout << "MovementComponent: " << velocity << std::endl; }
    float velocity;
};

class HealthComponent : public BaseComponent
{
public:
    ~HealthComponent() { std::cout << __FUNCTION__ << std::endl; }

    void Reset(float hp, float mana)
    {
        this->hp = hp;
        this->mana = mana;
    }

    void Print() { std::cout << "HealthComponent: " << hp << '\t' << mana << std::endl; }
    float hp;
    float mana;
};

class PositionComponent : public BaseComponent
{
public:
    ~PositionComponent() { std::cout << __FUNCTION__ << std::endl; }

    void Reset(float px, float py, float pz)
    {
        x = px;
        y = py;
        z = pz;
    }

    void Print() { std::cout << "PositionComponent: " << x << '\t' << y << '\t' << z << std::endl; }
    float x;
    float y;
    float z;
};

class DemoSystem : public BaseSystem
{
public:
    using BaseSystem::BaseSystem;
    void Update(float time_step) override
    {
        for (PositionComponent* p : ComponentItr<PositionComponent>(admin_)) {
			p->Print();
			HealthComponent* h = p->Sibling<HealthComponent>();
			if (h)
			{
				h->Print();
			}
			MovementComponent* m = p->Sibling<MovementComponent>();
			if (m)
			{
				m->Print();
			}
        }
    }
};

int main()
{
    EntityAdmin admin;
    Entity& entity = admin.CreateEntity<Entity>();
	float fZ = 10.f;
    entity.Add<PositionComponent>(3.f, 7.f, fZ);
    // entity.Add<PositionComponent>(22.f, 23.f, 24.f); //throw exception
	std::cout << "entity replace PositionComponent: " << std::endl;
    entity.Replace<PositionComponent>(10.f, 100.f, 100.f);
    entity.Add<HealthComponent>(50.f, 70.f);

	std::cout << "entity contains PositionComponent and HealthComponent: "
		<< entity.Has<PositionComponent, HealthComponent>() << std::endl;
	std::cout << "entity contains PositionComponent and HealthComponent and MovementComponent: "
		<< entity.Has<PositionComponent, HealthComponent, MovementComponent>() << std::endl;

	std::cout << "entity remove PositionComponent and HealthComponent: " << std::endl;
	entity.Remove<PositionComponent, HealthComponent>();

	entity.Add<PositionComponent>(10.f, 20.f, 30.f);
	std::cout << "entity contains HealthComponent: "
		<< entity.Has<HealthComponent>() << std::endl;
	entity.Add<HealthComponent>(50.f, 70.f);

	std::cout << std::endl;
	Entity& entity2 = admin.CreateEntity<Entity>();
	entity2.Add<PositionComponent>(3.f, 4.f, 5.f);
	entity2.Add<HealthComponent>(20.f, 70.f);

	std::cout << "entity2 Get PositionComponent:" << std::endl;
	auto val1 = entity2.Get<PositionComponent>();
	val1->Print();

	std::cout << "entity2 Get PositionComponent and HealthComponent:" << std::endl;
	std::tuple<PositionComponent*, HealthComponent*> va12 = entity2.Get<PositionComponent, HealthComponent>();
	std::get<0>(va12)->Print();
	std::get<1>(va12)->Print();

	std::cout << std::endl;
	auto va13 = entity2.Get<PositionComponent, HealthComponent, MovementComponent>();
	assert((std::get<0>(va13) == nullptr) && (std::get<0>(va13) == nullptr) &&
		(std::get<0>(va13) == nullptr));
	
	std::cout << "ComponentItr HealthComponent: with Condition(hp > 30)" << std::endl;
    for (HealthComponent* h : ComponentItr<HealthComponent>(
             &admin, [](const HealthComponent* h) -> bool { return h->hp > 30.f; })) {
        h->Print();
    }

	Entity& entity3 = admin.CreateEntity<Entity>();
	entity3.Add<PositionComponent>(3.f, 4.f, 5.f);
	entity3.Add<HealthComponent>(80.f, 70.f);
	entity3.Add<MovementComponent>(100.f);

    std::cout << "TupleItr PositionComponent, HealthComponent with Condition(hp > 60): " << std::endl;
    for (std::tuple<PositionComponent*, HealthComponent*>&& t : TupleItr<PositionComponent, HealthComponent>(
             &admin, [](const PositionComponent*p, const HealthComponent* h) -> bool { return h->hp > 60; })) {
        std::get<0>(t)->Print();
        std::get<1>(t)->Print();
    }

	std::cout << std::endl;
	std::cout << "Update DemoSystem: " << std::endl;
	DemoSystem& sys = admin.CreateSystem<DemoSystem>();
	sys.Update(0.1f);
	
    std::cin.get();
    return 0;
}