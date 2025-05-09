#ifndef COMPONENT_MANAGER_CLASS
#define COMPONENT_MANAGER_CLASS

#include "Component.hpp"

using CompDeleter = void(*)(void*);
using EntDeleter = void(*)(void*, EntityID);

class ComponentManager
{
private:
	inline static ComponentID nextID = 0;
	std::array<void*, MAX_COMPONENTS> componentArrays{};
	std::array<CompDeleter, MAX_COMPONENTS> deleters{};
	std::array<EntDeleter, MAX_COMPONENTS> destroyCallbacks{};

	template <typename T>
	static ComponentID GetUniqueComponentID()
	{
		static ComponentID id = nextID++;
		return id;
	}

	template <typename T>
	Component<T>& GetComponentStorage()
	{
		std::size_t componentID = GetComponentType<T>();
		Component<T>* componentStorage = static_cast<Component<T>*>(componentArrays[componentID]);

		if(!componentStorage)
			throw std::runtime_error("Component not registered.");

		return *componentStorage;
	}

public:
	template <typename T>
	ComponentID GetComponentType() const
	{
		return GetUniqueComponentID<T>();
	}

	template <typename T>
	void RegisterComponent()
	{
		const ComponentID id = GetComponentType<T>();
		componentArrays[id] = new Component<T>();

		deleters[id] = [](void* ptr)
		{
			delete static_cast<Component<T>*>(ptr);	
		};

		destroyCallbacks[id] = [](void* storage, EntityID entity)
		{
			static_cast<Component<T>*>(storage)->EntityDestroyed(entity);
		};
	}

	template <typename T>
	void AddComponent(EntityID entity, T&& component)
	{
		GetComponentStorage<T>().Add(std::forward<T>(component), entity);
	}

	template <typename T> 
	void RemoveComponent(EntityID entity)
	{
		GetComponentStorage<T>().Remove(entity);
	}

	template <typename T> 
	T& GetComponent(EntityID entity)	
	{
		return GetComponentStorage<T>().Get(entity);
	}

	template <typename T> 
	bool HasComponent(EntityID entity)
	{
		return GetComponentStorage<T>().Has(entity);
	}

	void EntityDestroyed(EntityID entity)
	{
		for (ComponentID id = 0; id < nextID; id++)
		{
			if (componentArrays[id] && destroyCallbacks[id])
			{
				destroyCallbacks[id](componentArrays[id], entity);
			}
		}
	}

	~ComponentManager()
	{
		for (ComponentID i = 0; i < MAX_COMPONENTS; i++)
		{
			if (componentArrays[i])
			{
				deleters[i](componentArrays[i]);
				componentArrays[i] = nullptr;
			}
		}
	}
};

#endif
