#ifndef COMPONENT_CLASS
#define COMPONENT_CLASS

#include <cstdint>
#include <array>
#include <bitset>
#include <stdexcept>

using EntityID = std::uint32_t;
using ComponentID = std::uint8_t;
using Size = std::size_t;

constexpr EntityID MAX_ENTITIES = 5000;
constexpr ComponentID MAX_COMPONENTS = 32;

template <typename T>
class Component
{
private:
	std::array<T, MAX_ENTITIES> entityComponent{};
	std::array<Size, MAX_ENTITIES> entityToIndex{};
	std::array<EntityID, MAX_ENTITIES> indexToEntity{};
	std::bitset<MAX_ENTITIES> hasComponent{};
	Size size{};

public:
	void Add(T&& component, EntityID entity)
	{
		if (hasComponent[entity])
			throw std::runtime_error("Attempted to add a component to an entity that already has it.");

		entityToIndex[entity] = size;
		indexToEntity[size] = entity;
		entityComponent[size] = std::move(component);
		hasComponent[entity] = true;

		size++;
	}

	void Remove(EntityID entity)
	{
		if (!hasComponent[entity])
			throw std::runtime_error("Attempted to remove a component that does not exist on this entity.");

		size_t removeIndex = entityToIndex[entity];
		size_t lastIndex = size - 1;

		entityComponent[removeIndex] = entityComponent[lastIndex];

		EntityID lastEntity = indexToEntity[lastIndex];
		entityToIndex[lastEntity] = removeIndex;
		indexToEntity[removeIndex] = lastEntity;
		indexToEntity[lastIndex] = EntityID{};
		entityToIndex[entity] = 0;

		hasComponent[entity] = false;

		size--;
	}

	T& Get(EntityID entity)
	{
		if (!hasComponent[entity])
			throw std::runtime_error("Attempted to get a component that does not exist on this entity.");

		return entityComponent[entityToIndex[entity]];
	}

	bool Has(EntityID entity) const
	{
		return hasComponent[entity];
	}

	void EntityDestroyed(EntityID entity)
	{
		if(hasComponent[entity]) Remove(entity);
	}
};


#endif
