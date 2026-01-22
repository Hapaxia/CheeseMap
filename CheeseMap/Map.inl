//////////////////////////////////////////////////////////////////////////////
//
// Cheese Map (https://github.com/Hapaxia/CheeseMap
// --
//
// Map
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

#include "Map.hpp"

#include <cmath>

namespace cheesemap
{

inline Map::Map()
	: grids{}
	, layers{}
	, textureAtlas{}
	, tileTemplates{}

	, m_texture{ nullptr }
	, m_view{}
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
	, m_vertices{}
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

inline void Map::setRangeZ(const std::size_t min, const std::size_t max)
{
	m_useRangeZ = true;
	m_rangeMinZ = min;
	m_rangeMaxZ = max;
	update();
}

inline void Map::setRangeDepth(const float min, const float max)
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

inline sf::Vector2f Map::getLocalCoord(const sf::Vector2f coord) const
{
	return getInverseTransform().transformPoint(coord);
}

inline Map::GridTileId Map::getGridTileIdAtLocalCoord(const sf::Vector2f localCoord) const
{
	GridTileId gridTileId{};
	const std::size_t numberOfGrids{ grids.size() };
	if (numberOfGrids == 0u)
		return gridTileId;

	bool tileFound{ false };

	std::size_t zOrder{ 0u };

	for (std::size_t g{ 0u }; g < numberOfGrids; ++g)
	{
		const Grid& grid{ grids[g] };
		if ((!grid.isActive) || (grid.depth < 0.f))
			continue;

		if (grid.zOrder < zOrder)
			continue;

		const std::size_t numberOfTileIds{ grid.tileIds.size() };
		for (std::size_t t{ 0u }; t < numberOfTileIds; ++t)
		{
			const sf::Vector2<std::size_t> location{ t % grid.rowWidth, t / grid.rowWidth };
			
			sf::Vector2f topLeft{ grid.position };

			topLeft.x += static_cast<float>(location.x * grid.tileSize.x);
			topLeft.y += static_cast<float>(location.y * grid.tileSize.y);

			const sf::FloatRect tileRect{ topLeft, grid.tileSize };

			if (tileRect.contains(localCoord))
			{
				zOrder = grids[g].zOrder;
				gridTileId.gridIndex = g;
				gridTileId.tileIndex = t;
				tileFound = true;
				break;
			}
		}
	}

	if (!tileFound)
		gridTileId.gridIndex = grids.size();

	return gridTileId;
}

inline std::vector<Map::GridTileId> Map::getGridTileIdsAtLocalCoord(sf::Vector2f localCoord) const
{
	std::vector<GridTileId> gridTileIds{};
	const std::size_t numberOfGrids{ grids.size() };
	if (numberOfGrids == 0u)
		return gridTileIds;

	for (std::size_t g{ 0u }; g < numberOfGrids; ++g)
	{
		const Grid& grid{ grids[g] };
		if ((!grid.isActive) || (grid.depth < 0.f))
			continue;

		const std::size_t numberOfTileIds{ grid.tileIds.size() };
		for (std::size_t t{ 0u }; t < numberOfTileIds; ++t)
		{
			const sf::Vector2<std::size_t> location{ t % grid.rowWidth, t / grid.rowWidth };

			sf::Vector2f topLeft{ grid.position };

			topLeft.x += static_cast<float>(location.x * grid.tileSize.x);
			topLeft.y += static_cast<float>(location.y * grid.tileSize.y);

			const sf::FloatRect tileRect{ topLeft, grid.tileSize };

			if (tileRect.contains(localCoord))
			{
				GridTileId gridTileId{};
				gridTileId.gridIndex = g;
				gridTileId.tileIndex = t;
				gridTileIds.push_back(gridTileId);
				break;
			}
		}
	}

	return gridTileIds;
}

inline Map::LayerTileId Map::getLayerTileIdAtLocalCoord(sf::Vector2f localCoord) const
{
	LayerTileId layerTileId{};
	const std::size_t numberOfLayers{ layers.size() };
	if (numberOfLayers == 0u)
		return layerTileId;

	bool tileFound{ false };

	std::size_t zOrder{ 0u };

	for (std::size_t l{ 0u }; l < numberOfLayers; ++l)
	{
		const Layer& layer{ layers[l] };
		if (!layer.isActive || (layer.depth < 0.f))
			continue;

		if (layer.zOrder < zOrder)
			continue;

		const std::size_t numberOfTiles{ layer.tiles.size() };
		for (std::size_t t{ 0u }; t < numberOfTiles; ++t)
		{
			const cm::Tile& tile{ layer.tiles[t] };
			if ((!layer.isActive) || (!(layer.depth < 0.f)))
				continue;

			const sf::Vector2f topLeft{ layer.offset + tile.position };

			sf::Vector2f tileSize{ tile.size };
			if (tile.isTemplate)
			{
				const sf::Vector2f tileTemplateSize{ tileTemplates[tile.id].size };
				tileSize.x *= tileTemplateSize.x;
				tileSize.y *= tileTemplateSize.y;
			}

			const sf::FloatRect tileRect{ topLeft, tileSize };

			if (tileRect.contains(localCoord))
			{
				zOrder = layers[l].zOrder;
				layerTileId.layerIndex = l;
				layerTileId.tileIndex = t;
				tileFound = true;
				break;
			}
		}
	}

	if (!tileFound)
		layerTileId.layerIndex = numberOfLayers;

	return layerTileId;
}

inline std::vector<Map::LayerTileId> Map::getLayerTileIdsAtLocalCoord(sf::Vector2f localCoord) const
{
	std::vector<LayerTileId> layerTileIds{};
	const std::size_t numberOfLayers{ layers.size() };
	if (numberOfLayers == 0u)
		return layerTileIds;

	for (std::size_t l{ 0u }; l < numberOfLayers; ++l)
	{
		const Layer& layer{ layers[l] };
		if ((!layer.isActive) || (layer.depth < 0.f))
			continue;

		const std::size_t numberOfTiles{ layer.tiles.size() };
		for (std::size_t t{ 0u }; t < numberOfTiles; ++t)
		{
			const cm::Tile& tile{ layer.tiles[t] };
			if (!tile.isActive)
				continue;

			const sf::Vector2f topLeft{ layer.offset + tile.position };

			sf::Vector2f tileSize{ tile.size };
			if (tile.isTemplate)
			{
				const sf::Vector2f tileTemplateSize{ tileTemplates[tile.id].size };
				tileSize.x *= tileTemplateSize.x;
				tileSize.y *= tileTemplateSize.y;
			}

			const sf::FloatRect tileRect{ topLeft, tileSize };

			if (tileRect.contains(localCoord))
			{
				LayerTileId layerTileId{};
				layerTileId.layerIndex = l;
				layerTileId.tileIndex = t;
				layerTileIds.push_back(layerTileId);
				break;
			}
		}
	}

	return layerTileIds;
}

inline std::size_t Map::getGridTileIndexAtGridLocation(const std::size_t gridIndex, const sf::Vector2<std::size_t> location) const
{
	const std::size_t numberOfGrids{ grids.size() };
	if ((numberOfGrids == 0u) || (gridIndex >= numberOfGrids))
		return{};

	return (location.y * grids[gridIndex].rowWidth) + location.x;
}

inline std::size_t Map::getGridHeight(const std::size_t gridIndex) const
{
	return grids[gridIndex].tileIds.size() / grids[gridIndex].rowWidth;
}

inline bool Map::doesGridBoundsContainCoord(const std::size_t gridIndex, const sf::Vector2f localCoord) const
{
	const std::size_t numberOfGrids{ grids.size() };
	if ((numberOfGrids == 0u) || (gridIndex >= numberOfGrids))
		return false;

	const Grid& grid{ grids[gridIndex] };

	const sf::Vector2f tileSize{ grid.tileSize };
	const sf::Vector2f topLeft{ grid.position };
	const sf::Vector2f bottomRight{ topLeft.x + (grid.rowWidth * tileSize.x), topLeft.y + (getGridHeight(gridIndex) * tileSize.y) };

	return !((localCoord.x < topLeft.x) || (localCoord.y < topLeft.y) || (localCoord.x >= bottomRight.x) || (localCoord.y >= bottomRight.y));
}



































// PRIVATE

inline void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (m_texture == nullptr)
		return;

	states.transform *= getTransform();
	states.texture = m_texture;

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

	if (m_view.getRotation().asDegrees() != 0.f)
	{
		auto rotatePoint = [](sf::Vector2f& p, const float cos, const float sin) { p = { p.x * cos - p.y * sin, p.y * cos + p.x * sin }; };

		sf::Vector2f topLeft{ -viewHalfSize };
		sf::Vector2f topRight{ -topLeft.x, topLeft.y };

		const float angle{ m_view.getRotation().asRadians() };
		const float sine{ std::sin(angle) };
		const float cosine{ std::cos(angle) };

		rotatePoint(topLeft, cosine, sine);
		rotatePoint(topRight, cosine, sine);

		const sf::Vector2f bottomLeft{ -topRight };
		const sf::Vector2f bottomRight{ -topLeft };

		sf::Vector2f min{ viewCenter }, max{ viewCenter };
		min.x += std::min(std::min(std::min(topLeft.x, topRight.x), bottomLeft.x), bottomRight.x);
		min.y += std::min(std::min(std::min(topLeft.y, topRight.y), bottomLeft.y), bottomRight.y);
		max.x += std::max(std::max(std::max(topLeft.x, topRight.x), bottomLeft.x), bottomRight.x);
		max.y += std::max(std::max(std::max(topLeft.y, topRight.y), bottomLeft.y), bottomRight.y);

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
	std::vector<TileId> activeTiles{};

	auto pointWithDepth = [&](const sf::Vector2f& p, const float dr) { return ((p - (viewCenter + m_vanishingPointOffsetFromCenter)) * dr) + (viewCenter + m_vanishingPointOffsetFromCenter); };
	auto pointDepthScale = [&](const sf::Vector2f p, const float dr) { return p * dr; };

	sf::FloatRect tileBounds{};



	// test layers' tiles
	for (std::size_t l{ 0u }, numberOfLayers{ layers.size() }; l < numberOfLayers; ++l)
	{
		const float depth{ layers[l].depth - m_depthOffset };

		if ((!layers[l].isActive) || (depth <= 0.f))
			continue;

		if (m_useRangeZ && ((layers[l].zOrder < m_rangeMinZ) || (layers[l].zOrder > m_rangeMaxZ)))
			continue;
		if (m_useRangeDepth && ((depth < m_rangeMinDepth) || (depth > m_rangeMaxDepth)))
			continue;

		float depthRatio{ 1.f };
		const float adjustedDepth{ m_depthMultiplier * depth };
		if ((depth > 0.f) && (adjustedDepth != 0.f))
			depthRatio = 1.f / adjustedDepth;

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

			tileBounds = { pointWithDepth(tileBounds.position, depthRatio), pointDepthScale(tileBounds.size, depthRatio) };

			if (!effectiveViewRectangle.findIntersection(tileBounds))
				isAnActiveTile = false;

			if (isAnActiveTile)
				activeTiles.push_back({ TileId::GroupType::Layer, l, t });
		}
	}

	// test grids' tiles
	for (std::size_t g{ 0u }, numberOfGrids{ grids.size() }; g < numberOfGrids; ++g)
	{
		const float depth{ grids[g].depth - m_depthOffset };

		if ((!grids[g].isActive) || (depth <= 0.f))
			continue;

		if (m_useRangeZ && (grids[g].zOrder < m_rangeMinZ || grids[g].zOrder > m_rangeMaxZ))
			continue;
		if (m_useRangeDepth && (depth < m_rangeMinDepth || depth > m_rangeMaxDepth))
			continue;

		float depthRatio{ 1.f };
		const float adjustedDepth{ m_depthMultiplier * depth };
		if ((depth > 0.f) && (adjustedDepth != 0.f))
			depthRatio = 1.f / adjustedDepth;

		for (std::size_t t{ 0u }, numberOfTiles{ grids[g].tileIds.size() }; t < numberOfTiles; ++t)
		{
			sf::Vector2<std::size_t> tileLocation{ t % grids[g].rowWidth, t / grids[g].rowWidth };
			tileBounds = { { grids[g].position.x + (tileLocation.x * grids[g].tileSize.x), grids[g].position.y + (tileLocation.y * grids[g].tileSize.y) }, grids[g].tileSize };
			tileBounds = { pointWithDepth(tileBounds.position, depthRatio), pointDepthScale(tileBounds.size, depthRatio) };
			//const bool isTileWithinRange{  };

			if ((grids[g].tileIds[t] != grids[g].invisibleId) && effectiveViewRectangle.findIntersection(tileBounds) && (grids[g].tileIds[t] < numberOfTextureAtlasRectangle))
				activeTiles.push_back({ TileId::GroupType::Grid, g, t });
		}
	}

	// sort active tiles by z, regardless of if it's a layer or grid
	std::sort(activeTiles.begin(), activeTiles.end(), [&](const TileId lhs, const TileId rhs)
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
			return (left < right);
		});

	// build vertex array
	constexpr std::size_t numOfVerticesPerQuad{ 6u };
	m_vertices.resize(activeTiles.size() * numOfVerticesPerQuad);
	std::size_t startVertex{ 0u };
	for (auto& activeTile : activeTiles)
	{
		float tileDepth{ 0.f };
		Tile tile{};
		TextureTransform textureTransform{};
		sf::Color color{};
		sf::Vector2f texInset{};
		switch (activeTile.groupType)
		{
		case TileId::GroupType::Grid:
		{
			texInset = grids[activeTile.groupIndex].texInset;
			tile.id = grids[activeTile.groupIndex].tileIds[activeTile.tileIndex];
			tile.size = grids[activeTile.groupIndex].tileSize;
			const std::size_t rowWidth{ grids[activeTile.groupIndex].rowWidth };
			const sf::Vector2f position{ grids[activeTile.groupIndex].position };
			tile.position = { position.x + tile.size.x * (activeTile.tileIndex % rowWidth), position.y + tile.size.y * (activeTile.tileIndex / rowWidth) };
			tileDepth = grids[activeTile.groupIndex].depth;
			auto tt{ std::find_if(grids[activeTile.groupIndex].tileTextureTransforms.begin(), grids[activeTile.groupIndex].tileTextureTransforms.end(),
				[&](const Grid::TileTextureTransform& ttt) { return ttt.tileIndex == activeTile.tileIndex; }
				) };
			tile.expand = grids[activeTile.groupIndex].tileExpand;
			if (tt != grids[activeTile.groupIndex].tileTextureTransforms.end())
			{
				textureTransform = tt->textureTransform;
				tile.expand += tt->tileExpand;
			}
			color = grids[activeTile.groupIndex].color;
		}
			break;
		default:
		case TileId::GroupType::Layer:
		{
			texInset = layers[activeTile.groupIndex].texInset;
			const Tile& tileControl{ layers[activeTile.groupIndex].tiles[activeTile.tileIndex] };
			tile = tileControl;
			textureTransform = tile.textureTransform;
			if (tileControl.isTemplate)
			{
				const TileTemplate& templateTile{ tileTemplates[tileControl.id] };
				tile.id = templateTile.id;
				tile.size.x *= templateTile.size.x;
				tile.size.y *= templateTile.size.y;
				tile.expand += templateTile.expand;
			}
			tile.position += layers[activeTile.groupIndex].offset;
			tileDepth = layers[activeTile.groupIndex].depth;
			color = layers[activeTile.groupIndex].color;
		}
			break;
		}
		texInset += textureTransform.texInset;

		float depthRatio{ 1.f };
		tileDepth -= m_depthOffset;
		const float adjustedDepth{ m_depthMultiplier * tileDepth };
		if ((tileDepth > 0.f) && (adjustedDepth != 0.f))
			depthRatio = 1.f / adjustedDepth;

		priv_setQuad(
			startVertex,
			pointWithDepth(tile.position - tile.expand, depthRatio),
			pointWithDepth(tile.position + tile.size + tile.expand, depthRatio),
			textureAtlas[tile.id].position + texInset,
			textureAtlas[tile.id].position + textureAtlas[tile.id].size - texInset,
			color,
			textureTransform.flipX,
			textureTransform.flipY,
			textureTransform.turn);
		startVertex += numOfVerticesPerQuad;
	}
}

inline void Map::priv_setQuad
(
	const std::size_t startVertex,
	const sf::Vector2f topLeft,
	const sf::Vector2f bottomRight,
	sf::Vector2f textureTopLeft,
	sf::Vector2f textureBottomRight,
	const sf::Color color,
	const bool flipX,
	const bool flipY,
	const bool turn
) const
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

	m_vertices[startVertex + 0u].position = topLeft;
	m_vertices[startVertex + 1u].position = { topLeft.x, bottomRight.y };
	m_vertices[startVertex + 2u].position = { bottomRight.x, topLeft.y };
	m_vertices[startVertex + 3u].position = m_vertices[startVertex + 2u].position;
	m_vertices[startVertex + 4u].position = m_vertices[startVertex + 1u].position;
	m_vertices[startVertex + 5u].position = bottomRight;

	m_vertices[startVertex + 0u].texCoords = textureTopLeft;
	m_vertices[startVertex + 1u].texCoords = { textureTopLeft.x, textureBottomRight.y };
	m_vertices[startVertex + 2u].texCoords = { textureBottomRight.x, textureTopLeft.y };
	m_vertices[startVertex + 3u].texCoords = m_vertices[startVertex + 2u].texCoords;
	m_vertices[startVertex + 4u].texCoords = m_vertices[startVertex + 1u].texCoords;
	m_vertices[startVertex + 5u].texCoords = textureBottomRight;

	if (turn)
	{
		//  --------       --------
		// | 0  2,3 | --> | 2,3  5 |
		// |        |     |        |
		// | 1,4  5 | --> | 0  1,4 |
		//  --------       --------

		m_vertices[startVertex + 1u].texCoords = m_vertices[startVertex + 5u].texCoords;
		m_vertices[startVertex + 2u].texCoords = m_vertices[startVertex + 0u].texCoords;
		m_vertices[startVertex + 0u].texCoords = m_vertices[startVertex + 4u].texCoords;
		m_vertices[startVertex + 5u].texCoords = m_vertices[startVertex + 3u].texCoords;
		m_vertices[startVertex + 3u].texCoords = m_vertices[startVertex + 2u].texCoords;
		m_vertices[startVertex + 4u].texCoords = m_vertices[startVertex + 1u].texCoords;
	}

	m_vertices[startVertex + 0u].color = color;
	m_vertices[startVertex + 1u].color = color;
	m_vertices[startVertex + 2u].color = color;
	m_vertices[startVertex + 3u].color = color;
	m_vertices[startVertex + 4u].color = color;
	m_vertices[startVertex + 5u].color = color;
}

} // namespace cheesemap
