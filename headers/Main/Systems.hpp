#pragma once

#include "ECS/Manager.hpp"
#include "Graphics/SpriteRenderer.hpp"

void RenderSystem(Manager &manager, SpriteRenderer *renderer);
void PlayerMovementSystem(Manager &manager, bool keys[4], float dt);
void DeleteAllSystem(Manager &manager);
void DestroySystem(Entity entity, Manager &manager, float wall);
