//////////////////////////////////////////////////////////////////////////////
//
// Cheese Map (https://github.com/Hapaxia/CheeseMap
// --
//
// Tile
//
// Copyright(c) 2023-2025 M.J.Silk
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions :
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software.If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// M.J.Silk
// MJSilk2@gmail.com
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Common.hpp"
#include "TextureTransform.hpp"

#include <SFML/System/Vector2.hpp>

namespace cheesemap
{

struct Tile
{
	bool isActive{ true };
	bool isTemplate{ false };
	std::size_t id{ 0u }; // this is the id of the tile (or tile template) to use.
	sf::Vector2f position{ 0.f, 0.f };
	sf::Vector2f size{ 1.f, 1.f };
	TextureTransform textureTransform;
};

struct TileTemplate
{
	bool isActive{ true }; // this allows all tiles using a tile template to be de-activated as a group (when active, each tile's own active status is used)
	std::size_t id{ 0u }; // this is the id of the tile
	sf::Vector2f size{ 1.f, 1.f };
};

} // namespace cheesemap
