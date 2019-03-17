/// --------------------------------------------------------------------------
/// Copyright (C) 2015  Robin Grönberg
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include <stdexcept>
#include "common/thirdparty/catch.hpp"


#define ECS_ASSERT(Expr, Msg) if(!(Expr)) throw std::runtime_error(Msg);

#include "ecs_iterator.h"
#include "entity_admin.h"

using namespace ecs;

namespace
{
	int movement_component_count;
	class MovementComponent : public BaseComponent
	{
	public:
		MovementComponent() { ++movement_component_count; }
		~MovementComponent() { --movement_component_count; }

		void Reset(float velocity) { this->velocity = velocity; }

		void Print() { std::cout << "MovementComponent: " << velocity << std::endl; }
		float velocity;
	};
	int health_component_count;
	class HealthComponent : public BaseComponent
	{
	public:
		HealthComponent() { ++health_component_count; }
		~HealthComponent() { --health_component_count; }

		void Reset(float hp, float mana)
		{
			this->hp = hp;
			this->mana = mana;
		}

		void Print() { std::cout << "HealthComponent: " << hp << '\t' << mana << std::endl; }
		float hp;
		float mana;
	};
	int position_component_count;
	class PositionComponent : public BaseComponent
	{
	public:
		PositionComponent() { ++position_component_count; }
		~PositionComponent() { --position_component_count; }

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

	int demo_system_movement_update_times;
	int demo_system_position_update_times;
	int demo_system_health_update_times;
	class DemoSystem : public BaseSystem
	{
	public:
		using BaseSystem::BaseSystem;
		void Update(float time_step) override
		{
			for (MovementComponent* m : ComponentItr<MovementComponent>(admin_)) {
				demo_system_movement_update_times++;
				HealthComponent* h = m->Sibling<HealthComponent>();
				if (h)
				{
					demo_system_health_update_times++;
				}
				PositionComponent* p = m->Sibling<PositionComponent>();
				if (p)
				{
					demo_system_position_update_times++;
				}
			}
		}
	};
}

SCENARIO("Testing ecs framework, unittests") {
	GIVEN("An EntityAdmin") {
		EntityAdmin admin;
		movement_component_count = 0;
		health_component_count = 0;
		position_component_count = 0;
		
		//testing entity function
		GIVEN("1 Entity without components") {
			Entity& entity = admin.CreateEntity<Entity>();

			WHEN("Adding Components to the entity") {
				entity.Add<PositionComponent>(3.f, 7.f, 10.f);
				entity.Add<HealthComponent>(50.f, 70.f);
				THEN("It has the components attached") {
					bool has_position = entity.Has<PositionComponent>();
					bool has_health = entity.Has<HealthComponent>();
					bool has_position_and_health = entity.Has<PositionComponent, HealthComponent>();
					REQUIRE(has_position);
					REQUIRE(has_health);
					REQUIRE(has_position_and_health);
				}
				THEN("It doesn't have the components unattached") {
					bool has_movement = entity.Has<MovementComponent>();
					bool has_position_and_movement = entity.Has<PositionComponent, MovementComponent>();
					REQUIRE(!has_movement);
					REQUIRE(!has_position_and_movement);
				}
				THEN("Accessing added components should work") {
					entity.Get<PositionComponent>();
					entity.Get<HealthComponent>();
					entity.Get<PositionComponent, HealthComponent>();
				}
				THEN("Accessing data from added components should work") {
					REQUIRE((entity.Get<PositionComponent>()->x == 3.f && entity.Get<PositionComponent>()->y == 7.f && entity.Get<PositionComponent>()->z == 10.f));
					REQUIRE((entity.Get<HealthComponent>()->hp == 50.f && entity.Get<HealthComponent>()->mana == 70.f));
					std::tuple<PositionComponent*, HealthComponent*> val = entity.Get<PositionComponent, HealthComponent>();
					REQUIRE((std::get<0>(val)->x == 3.f && std::get<0>(val)->y == 7.f && std::get<0>(val)->z == 10.f));
					REQUIRE((std::get<1>(val)->hp == 50.f && std::get<1>(val)->mana == 70.f));
				}
				THEN("Get other components should be nullptr") {
					REQUIRE(entity.Get<MovementComponent>() == nullptr);
					std::tuple<PositionComponent*, MovementComponent*> val = entity.Get<PositionComponent, MovementComponent>();
					REQUIRE(std::get<0>(val) == nullptr);
					REQUIRE(std::get<1>(val) == nullptr);
				}
				AND_WHEN("When replacing the PositionComponent which existed") {
					entity.Replace<PositionComponent>(10.f, 100.f, 1000.f);
					THEN("Only one PositionComponent should exist") {
						REQUIRE( position_component_count == 1);
					}
					THEN("PositionComponent was replaced") {
						REQUIRE((entity.Get<PositionComponent>()->x == 10.f && entity.Get<PositionComponent>()->y == 100.f && entity.Get<PositionComponent>()->z == 1000.f));
					}
				}
				AND_WHEN("When replacing the MovementComponent which not existed") {
					entity.Replace<MovementComponent>(111.f);
					THEN("MovementComponent should be added") {
						bool has_movement = entity.Has<MovementComponent>();
						REQUIRE(has_movement);
					}
					THEN("Only one MovementComponent should exist") {
						REQUIRE(movement_component_count == 1);
					}
					THEN("Accessing data from MovementComponent should work") {
						REQUIRE(entity.Get<MovementComponent>()->velocity == 111);
					}
				}
				AND_WHEN("When removing the HealthComponent") {
					entity.Remove<HealthComponent>();
					THEN("No HealthComponent should exist") {
						REQUIRE(health_component_count == 0);
					}
					THEN("Entity should not have that component attached") {
						REQUIRE(!entity.Has<HealthComponent>());
					}
				}
				AND_WHEN("When removing the HealthComponent twice") {
					entity.Remove<HealthComponent>();
					THEN("It should not work") {
						REQUIRE_THROWS(entity.Remove<HealthComponent>());
					}
				}
				AND_WHEN("When removing the HealthComponent and PositionComponent") {
					entity.Remove<HealthComponent, PositionComponent>();
					THEN("No HealthComponent and PositionComponent should exist") {
						REQUIRE((health_component_count == 0 && position_component_count == 0));
					}
					THEN("Entity should not have the components attached") {
						bool has_health = entity.Has<HealthComponent>();
						bool has_position = entity.Has<PositionComponent>();
						bool has_health_and_position = entity.Has<HealthComponent, PositionComponent>();
						REQUIRE(!has_health);
						REQUIRE(!has_position);
						REQUIRE(!has_health_and_position);
					}
				}
				AND_WHEN("Destroying the entity") {
					admin.DestroyEntity(entity.GetEntityID());
					THEN("No Components should exist") {
						REQUIRE((health_component_count == 0 && position_component_count == 0 && movement_component_count == 0));
					}
				}
			}

			WHEN("When adding the PositionComponent twice") {
				entity.Add<PositionComponent>(3.f, 7.f, 10.f);
				THEN("Should not work") {
					REQUIRE_THROWS(entity.Add<PositionComponent>(22.f, 23.f, 24.f));
				}
			}
		}

		GIVEN("4 Entities without components") {
			Entity& entity1 = admin.CreateEntity<Entity>();
			Entity& entity2 = admin.CreateEntity<Entity>();
			Entity& entity3 = admin.CreateEntity<Entity>();
			Entity& entity4 = admin.CreateEntity<Entity>();

			WHEN("Adding Components to all entities") {
				entity2.Add<MovementComponent>(22.f);
				entity3.Add<MovementComponent>(33.f);
				entity4.Add<MovementComponent>(44.f);

				entity3.Add<HealthComponent>(30.f, 60.f);
				entity4.Add<HealthComponent>(40.f, 80.f);

				entity4.Add<PositionComponent>(4.f, 5.f, 6.f);

				THEN("Component should exist") {
					REQUIRE((movement_component_count == 3 && health_component_count == 2 && position_component_count == 1));
				}
				THEN("Iterating MovementComponent") {
					int count = 0;
					for (MovementComponent* m : ComponentItr<MovementComponent>(&admin)) {
						REQUIRE(m != nullptr);
						count++;
					}
					REQUIRE(count == 3);
				}
				THEN("Iterating MovementComponent with velocity > 30") {
					int count = 0;
					for (MovementComponent* m : ComponentItr<MovementComponent>(&admin,
						[](const MovementComponent* m) -> bool { return m->velocity > 30.f; })) {
						REQUIRE(m != nullptr);
						count++;
					}
					REQUIRE(count == 2);
				}
				THEN("Iterating MovementComponent And HealthComponent") {
					int count = 0;
					for (std::tuple<MovementComponent*, HealthComponent*>&& t : ComponentItr<MovementComponent, HealthComponent>(&admin)) {
						REQUIRE(std::get<0>(t) != nullptr);
						REQUIRE(std::get<1>(t) != nullptr);
						count++;
					}
					REQUIRE(count == 2);
				}
				THEN("Iterating MovementComponent And HealthComponent with velocity > 30 and hp > 33") {
					int count = 0;
					for (std::tuple<MovementComponent*, HealthComponent*>&& t : ComponentItr<MovementComponent, HealthComponent>(&admin,
						[](const MovementComponent* m, const HealthComponent* h) -> bool { return (m->velocity > 30.f && h->hp > 33.f); })) {
						REQUIRE(std::get<0>(t) != nullptr);
						REQUIRE(std::get<1>(t) != nullptr);
						count++;
					}
					REQUIRE(count == 1);
				}
				THEN("Iterating MovementComponent, HealthComponent And PositionComponent with hp > 80") {
					int count = 0;
					for (auto&& t : ComponentItr<MovementComponent, HealthComponent, PositionComponent>(&admin,
						[](const MovementComponent* m, const HealthComponent* h, const PositionComponent* p) -> bool { return h->hp > 80.f; })) {
						REQUIRE(std::get<0>(t) != nullptr);
						REQUIRE(std::get<1>(t) != nullptr);
						count++;
					}
					REQUIRE(count == 0);
				}
			}
		}

		GIVEN("1 System") {
			DemoSystem& sys = admin.CreateSystem<DemoSystem>();
			demo_system_movement_update_times = 0;
			demo_system_health_update_times = 0;
			demo_system_position_update_times = 0;
			WHEN("Check System exists") {
				THEN("should have system") {
					REQUIRE(admin.HasSystem<DemoSystem>());
				}
			}
			WHEN("Adding same system") {
				THEN("System should exists") {
					REQUIRE_THROWS(admin.CreateSystem<DemoSystem>());
				}
			}
			WHEN("Removing system") {
				admin.RemoveSystem<DemoSystem>();
				THEN("System should not exist anymore") {
					REQUIRE(!admin.HasSystem<DemoSystem>());
				}
			}
			WHEN("Adding 3 entities, and calling update") {
				Entity& entity1 = admin.CreateEntity<Entity>();
				Entity& entity2 = admin.CreateEntity<Entity>();
				Entity& entity3 = admin.CreateEntity<Entity>();

				entity1.Add<MovementComponent>(22.f);
				entity2.Add<MovementComponent>(33.f);
				entity3.Add<MovementComponent>(44.f);

				entity1.Add<HealthComponent>(30.f, 60.f);
				entity2.Add<HealthComponent>(40.f, 80.f);

				entity1.Add<PositionComponent>(4.f, 5.f, 6.f);

				sys.Update(0.1f);
				THEN("Check component update times") {
					REQUIRE(demo_system_movement_update_times == 3);
					REQUIRE(demo_system_health_update_times == 2);
					REQUIRE(demo_system_position_update_times == 1);
				}
			}
		}
	}
}
