//////////////////////////////////////////////////////////////////////////////
//
// Cheese Map (https://github.com/Hapaxia/CheeseMap
// --
//
// Layer
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
#include "Tile.hpp"

#include <SFML/Graphics/Color.hpp>

namespace cheesemap
{

struct Layer
{
	bool isActive{ true };
	std::size_t zOrder{ 0u };
	float depth{ 0.f };
	sf::Vector2f offset{ 0.f, 0.f };
	sf::Color color{ sf::Color::White };
	std::vector<Tile> tiles;
};

} // namespace cheesemap
