#include "Main/Systems.hpp"
#include "Main/EntityComponents.hpp"

void RenderSystem(Manager &manager, SpriteRenderer *renderer)
{
	auto signature = manager.CreateSignature<Transform, Sprite>();
    auto entities = manager.GetEntitiesWithSignature(signature);

    for(EntityID id : entities)
	{
        auto& transform = manager.GetComponent<Transform>(id);
		auto& sprite = manager.GetComponent<Sprite>(id);
		renderer->DrawSprite(sprite.texture, transform.pos, transform.size, 0.0f);
	}
}

void PlayerMovementSystem(Manager &manager, bool keys[4], float dt)
{
	auto signature = manager.CreateSignature<Transform, Physic, PlayerTag>();
	auto entities = manager.GetEntitiesWithSignature(signature);

	for(EntityID id : entities)
	{
		auto& transform = manager.GetComponent<Transform>(id);
		auto& physic = manager.GetComponent<Physic>(id);

		if (keys[0]) transform.pos.x -= physic.vel.x * dt;
		if (keys[1]) transform.pos.x += physic.vel.x * dt;
		if (keys[2]) transform.pos.y += physic.vel.y * dt;
		if (keys[3]) transform.pos.y -= physic.vel.y * dt;
	}
}

// void DeleteAllSystem(Manager &manager)
// {
// 	auto signature = manager.CreateSignature<Transform, Sprite>();
//     auto entities = manager.GetEntitiesWithSignature(signature);
//
//     for (EntityID id : entities)
// 	{
// 		manager.DestroyEntity(id);
// 	} 
// }
//
// void DestroySystem(Entity entity, Manager &manager, float wall)
// {
// 	auto transform = entity.GetComponent<Transform>();
// 	auto sprite = entity.GetComponent<Sprite>();
//
// 	if(wall < transform.pos.x + sprite.size.x)
// 	{
// 		float newSize = sprite.size.x * 0.5f;
// 		glm::vec2 offset[] = {
// 			{-newSize, -newSize},
// 			{ newSize, -newSize},
// 			{-newSize,  newSize},
// 			{ newSize,  newSize}
// 		};
//
// 		for (int i = 0; i < 4; i++)
// 		{
// 			auto& piece = manager.AddEntity();
// 			glm::vec3 newPos = transform.pos + glm::vec3(offset[i], 0.0f);
//
// 			piece.AddComponent<Transform>(newPos, 0.0f, newSize);
// 			piece.AddComponent<Sprite>(
// 				entity.GetComponent<Sprite>().texture,
// 				entity.GetComponent<Sprite>().size / 2.0f
// 			);
// 			piece.AddComponent<Velocity>(offset[i] * 2.0f);
// 		}
//
// 		entity.Destroy();
// 	}
// }
