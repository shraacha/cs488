#pragma once

#include <optional>

#include "Intersection.hpp"
#include "Material.hpp"
#include "SceneManager.hpp"

std::optional<std::pair<Intersection, Material *>>
intersect(const SceneManager & sceneManager, const Ray & ray);
