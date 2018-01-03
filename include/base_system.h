#pragma once

namespace ecs
{
	class EntityAdmin;
	class BaseSystem
	{
	protected:
		EntityAdmin* admin_{ nullptr };
	public:
		BaseSystem(EntityAdmin* admin);
		virtual ~BaseSystem() = default;

		virtual void Update(float time_step) {}
	};
}