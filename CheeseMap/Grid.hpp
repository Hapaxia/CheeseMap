//////////////////////////////////////////////////////////////////////////////
//
// Cheese Map (https://github.com/Hapaxia/CheeseMap
// --
//
// Grid
//
// Copyright(c) 2023-2026 M.J.Silk
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
#include <SFML/Graphics/Color.hpp>

namespace cheesemap
{

struct Grid
{
	bool isActive{ true };
	std::size_t zOrder{ 0u };
	float depth{ 1.f };
	sf::Vector2f tileSize{ 1.f, 1.f };
	sf::Vector2f position{ 0.f, 0.f };
	std::size_t rowWidth{ 1u };
	sf::Vector2f texInset{ 0.f, 0.f };
	sf::Vector2f tileExpand{ 0.f, 0.f };
	std::size_t invisibleId{ 0u };
	sf::Color color{ sf::Color::White };
	std::vector<std::size_t> tileIds{};
	struct TileTextureTransform
	{
		std::size_t tileIndex{ 0u };
		sf::Vector2f tileExpand{ 0.f, 0.f };
		TextureTransform textureTransform{};
	};
	std::vector<TileTextureTransform> tileTextureTransforms{};
};

} // namespace cheesemap
