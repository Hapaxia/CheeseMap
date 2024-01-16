//////////////////////////////////////////////////////////////////////////////
//
// Cheese Map (https://github.com/Hapaxia/CheeseMap
// --
//
// Map
//
// Copyright(c) 2023-2024 M.J.Silk
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

#include "Map.hpp"

#include <cmath>

namespace
{

inline void setQuad(
	std::vector<sf::Vertex>& vertices,
	const std::size_t startVertex,
	const sf::Vector2f topLeft,
	const sf::Vector2f bottomRight,
	sf::Vector2f textureTopLeft,
	sf::Vector2f textureBottomRight,
	const sf::Color color,
	const bool flipX,
	const bool flipY,
	const bool turn
)
{
	if (flipX)
	{
		const float left{ textureTopLeft.x };
		textureTopLeft.x = textureBottomRight.x;
		textureBottomRight.x = left;
	}
	if (flipY)
	{
		const float top{ textureTopLeft.y };
		textureTopLeft.y = textureBottomRight.y;
		textureBottomRight.y = top;
	}

	vertices[startVertex + 0u].position = topLeft;
	vertices[startVertex + 1u].position = { topLeft.x, bottomRight.y };
	vertices[startVertex + 2u].position = { bottomRight.x, topLeft.y };
	vertices[startVertex + 3u].position = vertices[startVertex + 2u].position;
	vertices[startVertex + 4u].position = vertices[startVertex + 1u].position;
	vertices[startVertex + 5u].position = bottomRight;

	vertices[startVertex + 0u].texCoords = textureTopLeft;
	vertices[startVertex + 1u].texCoords = { textureTopLeft.x, textureBottomRight.y };
	vertices[startVertex + 2u].texCoords = { textureBottomRight.x, textureTopLeft.y };
	vertices[startVertex + 3u].texCoords = vertices[startVertex + 2u].texCoords;
	vertices[startVertex + 4u].texCoords = vertices[startVertex + 1u].texCoords;
	vertices[startVertex + 5u].texCoords = textureBottomRight;

	if (turn)
	{
		//  --------       --------
		// | 0  2,3 | --> | 2,3  5 |
		// |        |     |        |
		// | 1,4  5 | --> | 0  1,4 |
		//  --------       --------

		vertices[startVertex + 1u].texCoords = vertices[startVertex + 5u].texCoords;
		vertices[startVertex + 2u].texCoords = vertices[startVertex + 0u].texCoords;
		vertices[startVertex + 0u].texCoords = vertices[startVertex + 4u].texCoords;
		vertices[startVertex + 5u].texCoords = vertices[startVertex + 3u].texCoords;
		vertices[startVertex + 3u].texCoords = vertices[startVertex + 2u].texCoords;
		vertices[startVertex + 4u].texCoords = vertices[startVertex + 1u].texCoords;
	}

	vertices[startVertex + 0u].color = color;
	vertices[startVertex + 1u].color = color;
	vertices[startVertex + 2u].color = color;
	vertices[startVertex + 3u].color = color;
	vertices[startVertex + 4u].color = color;
	vertices[startVertex + 5u].color = color;
}

} // namespace

namespace cheesemap
{

inline Map::Map()
	: m_texture{ nullptr }
	, m_view()
	, m_depthMultiplier{ 1.f }
	, m_vanishingPointOffsetFromCenter{ 0.f, 0.f }
	, m_useRangeZ{ false }
	, m_useRangeDepth{ false }
	, m_rangeMinZ{ 0u }
	, m_rangeMaxZ{ 0u }
	, m_rangeMinDepth{ 0.f }
	, m_rangeMaxDepth{ 0.f }
	, m_depthOffset{ 0.f }
	, m_isUpdateRequired{ false }
	, m_vertices()
{

}

inline void Map::update()
{
	m_isUpdateRequired = true;
}
inline void Map::update(const sf::View& view)
{
	m_view = view;
	update();
}

inline void Map::setRangeZ(std::size_t min, std::size_t max)
{
	m_useRangeZ = true;
	m_rangeMinZ = min;
	m_rangeMaxZ = max;
	update();
}

inline void Map::setRangeDepth(float min, float max)
{
	m_useRangeDepth = true;
	m_rangeMinDepth = min;
	m_rangeMaxDepth = max;
	update();
}

inline void Map::setRangeZ()
{
	m_useRangeDepth = false;
	update();
}

inline void Map::setRangeDepth()
{
	m_useRangeZ = false;
	update();
}

inline void Map::setTexture(const sf::Texture& texture)
{
	m_texture = &texture;
	update();
}

inline void Map::setTexture()
{
	m_texture = nullptr;
	update();
}

inline void Map::setDepthScale(const float depthScale)
{
	m_depthMultiplier = depthScale > 0.f ? 1.f / depthScale : 0.f;
	update();
}

inline void Map::setVanishingPointOffsetFromCenter(const sf::Vector2f vanishingPointOffsetFromCenter)
{
	m_vanishingPointOffsetFromCenter = vanishingPointOffsetFromCenter;
	update();
}

inline sf::Vector2f Map::getVanishingPointOffsetCenter() const
{
	return m_vanishingPointOffsetFromCenter;
}

inline void Map::setDepthOffset(const float depthOffset)
{
	m_depthOffset = depthOffset;
	update();
}

inline float Map::getDepthOffset() const
{
	return m_depthOffset;
}



































// PRIVATE

inline void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (m_texture == nullptr)
		return;

	states.transform *= getTransform();
	states.texture = m_texture;
	states.shader = nullptr;

	if (m_isUpdateRequired)
		priv_update();

	target.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::Triangles, states);
}

inline void Map::priv_update() const
{
	m_isUpdateRequired = false;



	const std::size_t numberOfTextureAtlasRectangle{ textureAtlas.size() };

	const sf::Vector2f viewCenter{ m_view.getCenter() };
	const sf::Vector2f viewSize{ m_view.getSize() };
	const sf::Vector2f viewHalfSize{ viewSize / 2.f };

	// an axis-aligned rectangle that encompasses view rectangle even if rotated
	sf::FloatRect effectiveViewRectangle{ viewCenter - viewHalfSize, viewSize };

	if (m_view.getRotation() != 0.f)
	{
		auto rotatePoint = [](sf::Vector2f& p, const float cos, const float sin) { p = { p.x * cos - p.y * sin, p.y * cos + p.x * sin }; };

		sf::Vector2f topLeft{ -viewHalfSize };
		sf::Vector2f topRight{ -topLeft.x, topLeft.y };

		const float angle{ m_view.getRotation() / 180.f * 3.14159265f };
		const float sin{ std::sin(angle) };
		const float cos{ std::cos(angle) };

		rotatePoint(topLeft, cos, sin);
		rotatePoint(topRight, cos, sin);

		const sf::Vector2f bottomLeft{ -topRight };
		const sf::Vector2f bottomRight{ -topLeft };

		sf::Vector2f min{ topLeft }, max{ topLeft };
		min.x = std::min(std::min(std::min(topLeft.x, topRight.x), bottomLeft.x), bottomRight.x);
		min.y = std::min(std::min(std::min(topLeft.y, topRight.y), bottomLeft.y), bottomRight.y);
		max.x = std::max(std::max(std::max(topLeft.x, topRight.x), bottomLeft.x), bottomRight.x);
		max.y = std::max(std::max(std::max(topLeft.y, topRight.y), bottomLeft.y), bottomRight.y);

		min += viewCenter;
		max += viewCenter;

		effectiveViewRectangle = { min, max - min };
	}



	struct TileId
	{
		enum class GroupType
		{
			Layer,
			Grid,
		} groupType;
		std::size_t groupIndex; // index of layer or grid
		std::size_t tileIndex; // index of tile within specific layer or grid
	};
	std::vector<TileId> activeTiles;

	auto pointWithDepth = [&](const sf::Vector2f& p, const float dr) { return ((p - (viewCenter + m_vanishingPointOffsetFromCenter)) * dr) + (viewCenter + m_vanishingPointOffsetFromCenter); };
	auto pointDepthScale = [&](const sf::Vector2f p, const float dr) { return p * dr; };

	sf::FloatRect tileBounds;



	// test layers' tiles
	for (std::size_t l{ 0u }, numberOfLayers{ layers.size() }; l < numberOfLayers; ++l)
	{
		const float depth{ layers[l].depth - m_depthOffset };

		if (!layers[l].isActive || (depth < 0.f))
			continue;

		if (m_useRangeZ && (layers[l].zOrder < m_rangeMinZ || layers[l].zOrder > m_rangeMaxZ))
			continue;
		if (m_useRangeDepth && (layers[l].depth < m_rangeMinDepth || layers[l].depth > m_rangeMaxDepth))
			continue;

		float depthRatio{ 1.f };
		if (depth > 0.f)
			depthRatio = 1.f / (m_depthMultiplier * depth + 1.f);

		for (std::size_t t{ 0u }, numberOfTiles{ layers[l].tiles.size() }; t < numberOfTiles; ++t)
		{
			if (!layers[l].tiles[t].isActive)
				continue;

			bool isAnActiveTile{ false };
			if (!layers[l].tiles[t].isTemplate)
			{
				tileBounds = { layers[l].tiles[t].position + layers[l].offset, layers[l].tiles[t].size };
				if (layers[l].tiles[t].id < numberOfTextureAtlasRectangle)
					isAnActiveTile = true;
			}
			else if (tileTemplates[layers[l].tiles[t].id].isActive)
			{
				tileBounds = { layers[l].tiles[t].position + layers[l].offset, { layers[l].tiles[t].size.x * tileTemplates[layers[l].tiles[t].id].size.x, layers[l].tiles[t].size.y * tileTemplates[layers[l].tiles[t].id].size.y } };
				if (tileTemplates[layers[l].tiles[t].id].id < numberOfTextureAtlasRectangle)
					isAnActiveTile = true;
			}

			tileBounds = { pointWithDepth(tileBounds.getPosition(), depthRatio), pointDepthScale(tileBounds.getSize(), depthRatio) };

			if (!effectiveViewRectangle.intersects(tileBounds))
				isAnActiveTile = false;

			if (isAnActiveTile)
				activeTiles.push_back({ TileId::GroupType::Layer, l, t });
		}
	}

	// test grids' tiles
	for (std::size_t g{ 0u }, numberOfGrids{ grids.size() }; g < numberOfGrids; ++g)
	{
		const float depth{ grids[g].depth - m_depthOffset };

		if (!grids[g].isActive || depth < 0.f)
			continue;

		if (m_useRangeZ && (grids[g].zOrder < m_rangeMinZ || grids[g].zOrder > m_rangeMaxZ))
			continue;
		if (m_useRangeDepth && (grids[g].depth < m_rangeMinDepth || grids[g].depth > m_rangeMaxDepth))
			continue;

		float depthRatio{ 1.f };
		if (depth > 0.f)
			depthRatio = 1.f / (m_depthMultiplier * depth + 1.f);

		for (std::size_t t{ 0u }, numberOfTiles{ grids[g].tileIds.size() }; t < numberOfTiles; ++t)
		{
			sf::Vector2u tileLocation(t % grids[g].rowWidth, t / grids[g].rowWidth);
			tileBounds = { { grids[g].position.x + tileLocation.x * grids[g].tileSize.x, grids[g].position.y + tileLocation.y * grids[g].tileSize.y }, grids[g].tileSize };
			tileBounds = { pointWithDepth(tileBounds.getPosition(), depthRatio), pointDepthScale(tileBounds.getSize(), depthRatio) };
			const bool isTileWithinRange{  };

			if ((grids[g].tileIds[t] != grids[g].invisibleId) && effectiveViewRectangle.intersects(tileBounds) && (grids[g].tileIds[t] < numberOfTextureAtlasRectangle))
				activeTiles.push_back({ TileId::GroupType::Grid, g, t });
		}
	}

	// sort active tiles by z, regardless of if it's a layer or grid
	std::sort(activeTiles.begin(), activeTiles.end(), [&](TileId lhs, TileId rhs)
		{

			std::size_t left{ 0u }, right{ 0u };
			switch (lhs.groupType)
			{
			case TileId::GroupType::Grid:
				left = grids[lhs.groupIndex].zOrder;
				break;
			default:
			case TileId::GroupType::Layer:
				left = layers[lhs.groupIndex].zOrder;
				break;
			}
			switch (rhs.groupType)
			{
			case TileId::GroupType::Grid:
				right = grids[rhs.groupIndex].zOrder;
				break;
			default:
			case TileId::GroupType::Layer:
				right = layers[rhs.groupIndex].zOrder;
				break;
			}
			return left < right;
		});

	// build vertex array
	m_vertices.resize(activeTiles.size() * 6u);
	std::size_t startVertex{ 0u };
	for (auto& activeTile : activeTiles)
	{
		float tileDepth{ 0.f };
		Tile tile;
		TextureTransform textureTransform;
		sf::Color color;
		switch (activeTile.groupType)
		{
		case TileId::GroupType::Grid:
		{
			tile.id = grids[activeTile.groupIndex].tileIds[activeTile.tileIndex];
			tile.size = grids[activeTile.groupIndex].tileSize;
			const std::size_t rowWidth{ grids[activeTile.groupIndex].rowWidth };
			const sf::Vector2f position{ grids[activeTile.groupIndex].position };
			tile.position = { position.x + tile.size.x * (activeTile.tileIndex % rowWidth), position.y + tile.size.y * (activeTile.tileIndex / rowWidth) };
			tileDepth = grids[activeTile.groupIndex].depth;
			auto tt{ std::find_if(grids[activeTile.groupIndex].tileTextureTransforms.begin(), grids[activeTile.groupIndex].tileTextureTransforms.end(),
				[&](const Grid::TileTextureTransform& ttt) { return ttt.tileIndex == activeTile.tileIndex; }
				) };
			if (tt != grids[activeTile.groupIndex].tileTextureTransforms.end())
				textureTransform = tt->textureTransform;
			color = grids[activeTile.groupIndex].color;
		}
		break;
		default:
		case TileId::GroupType::Layer:
		{
			const Tile& tileControl{ layers[activeTile.groupIndex].tiles[activeTile.tileIndex] };
			tile = tileControl;
			textureTransform = tile.textureTransform;
			if (tileControl.isTemplate)
			{
				const TileTemplate& templateTile{ tileTemplates[tileControl.id] };
				tile.id = templateTile.id;
				tile.size.x *= templateTile.size.x;
				tile.size.y *= templateTile.size.y;
			}
			tile.position += layers[activeTile.groupIndex].offset;
			tileDepth = layers[activeTile.groupIndex].depth;
			color = layers[activeTile.groupIndex].color;
		}
		break;
		}

		float depthRatio{ 1.f };
		tileDepth -= m_depthOffset;
		if (tileDepth > 0.f)
			depthRatio = 1.f / (m_depthMultiplier * tileDepth + 1.f);

		setQuad(
			m_vertices,
			startVertex,
			pointWithDepth(tile.position, depthRatio),
			pointWithDepth(tile.position + tile.size, depthRatio),
			textureAtlas[tile.id].getPosition(),
			textureAtlas[tile.id].getPosition() + textureAtlas[tile.id].getSize(),
			color,
			textureTransform.flipX,
			textureTransform.flipY,
			textureTransform.turn);
		startVertex += 6u;
	}
}

} // namespace cheesemap
