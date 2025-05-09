#ifndef MANAGER_CLASS
#define MANAGER_CLASS

#include "ComponentManager.hpp"
#include "ECS/Component.hpp"
#include <optional>
#include <utility>
#include <vector>

class Manager;

class Entity
{
private:
	EntityID id;
	Manager* manager;

public:
	Entity(EntityID id, Manager* manager) : id(id), manager(manager) {}
	EntityID GetID() const { return id; }
	void Destroy();
	bool IsAlive() const;

	template <typename T, typename... Args> void AddComponent(Args&&... args);
	template <typename T> void RemoveComponent();
	template <typename T> T& GetComponent();
	template <typename T> bool HasComponent();
};

class Manager
{
private:
	EntityID nextEntityID{};
	Size availableIDs{};
	ComponentManager componentManager;
	std::array<std::bitset<MAX_COMPONENTS>, MAX_ENTITIES> entitySignatures{};
	std::array<std::optional<Entity>, MAX_ENTITIES> entities{};
	std::array<EntityID, MAX_ENTITIES> availableEntityID{};
	std::bitset<MAX_ENTITIES> aliveEntity{};

public:
	Manager()
	{
		for(EntityID id = 0; id < MAX_ENTITIES; id++)
			availableEntityID[id] = id;

		availableIDs = MAX_ENTITIES;
	}

	Entity& AddEntity()
	{
		if(availableIDs == 0)
			throw std::runtime_error("Max entities reached.");

		EntityID id = availableEntityID[--availableIDs];

		entities[id] = Entity(id, this);
		aliveEntity.set(id);
		return *entities[id];
	}

	template <typename T>
	void RegisterComponent()
	{
		componentManager.RegisterComponent<T>();
	}

	template <typename T>
	void AddComponent(EntityID entity, T&& component)
	{
		componentManager.AddComponent<T>(entity, std::forward<T>(component));
		entitySignatures[entity].set(componentManager.GetComponentType<T>(), true);
	}

	template <typename T>
	void RemoveComponent(EntityID entity)
	{
		componentManager.RemoveComponent<T>(entity);
		entitySignatures[entity].set(componentManager.GetComponentType<T>(), false);
	}

	template <typename T>
	T& GetComponent(EntityID entity)
	{
		return componentManager.GetComponent<T>(entity);
	}

	template <typename T>
	bool HasComponent(EntityID entity)
	{
		return componentManager.HasComponent<T>(entity);
	}

	void DestroyEntity(EntityID entity)
	{
		if (!IsAlive(entity)) return;

		componentManager.EntityDestroyed(entity);
		entitySignatures[entity].reset();
		entities[entity].reset();
		aliveEntity.reset(entity);

		availableEntityID[availableIDs++] = entity;
	}

	bool IsAlive(EntityID id) const
	{
		return id < MAX_ENTITIES && aliveEntity.test(id);
	}

	size_t GetAliveEntityCount() const
    {
        return aliveEntity.count();
    }

	template <typename... Components>
	std::bitset<MAX_COMPONENTS> CreateSignature() const
	{
		std::bitset<MAX_COMPONENTS> signature;
		(signature.set(componentManager.GetComponentType<Components>()), ...);
		return signature;
	}	

	std::vector<EntityID> GetEntitiesWithSignature(const std::bitset<MAX_COMPONENTS>& signature) const
	{
		std::vector<EntityID> result;

		for(EntityID id = 0; id < MAX_ENTITIES; id++)
		{
			if(!aliveEntity.test(id)) continue;
			if((entitySignatures[id] & signature) == signature) result.push_back(id);
		}

		return result;
	}

    friend class Entity;
};

template <typename T, typename... Args>
void Entity::AddComponent(Args&&... args)
{
	T component(std::forward<Args>(args)...);
	manager->AddComponent<T>(id, std::move(component));
}

template <typename T>
void Entity::RemoveComponent()
{
	manager->RemoveComponent<T>(id);
}

template <typename T>
T& Entity::GetComponent()
{
	return manager->GetComponent<T>(id);
}

template <typename T>
bool Entity::HasComponent()
{
	return manager->HasComponent<T>(id);
}

inline void Entity::Destroy()
{
	manager->DestroyEntity(id);
}

inline bool Entity::IsAlive() const
{
	return manager->IsAlive(id);
}

#endif
