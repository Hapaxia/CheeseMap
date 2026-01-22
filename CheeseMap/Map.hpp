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

#include "Common.hpp"
#include "Grid.hpp"
#include "Layer.hpp"
#include "Tile.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Vertex.hpp>

namespace cheesemap
{

// Cheese Map v1.2.1
class Map : public sf::Drawable, public sf::Transformable
{
public:
	std::vector<Grid> grids;
	std::vector<Layer> layers;
	std::vector<sf::FloatRect> textureAtlas;
	std::vector<TileTemplate> tileTemplates;

	struct GridTileId
	{
		std::size_t gridIndex{};
		std::size_t tileIndex{};
	};

	struct LayerTileId
	{
		std::size_t layerIndex{};
		std::size_t tileIndex{};
	};

	Map();

	void update();
	void update(const sf::View& view);

	void setRangeZ(std::size_t min, std::size_t max);
	void setRangeDepth(float min, float max);
	void setRangeZ(); // resets range to all
	void setRangeDepth(); // resets range to all
	
	void setTexture(const sf::Texture& texture);
	void setTexture();

	void setDepthScale(float depthScale);

	void setVanishingPointOffsetFromCenter(sf::Vector2f vanishingPointOffsetFromCenter);
	sf::Vector2f getVanishingPointOffsetCenter() const;

	void setDepthOffset(float depthOffset);
	float getDepthOffset() const;

	sf::Vector2f getLocalCoord(sf::Vector2f coord) const;

	GridTileId getGridTileIdAtLocalCoord(sf::Vector2f localCoord) const;
	std::vector<GridTileId> getGridTileIdsAtLocalCoord(sf::Vector2f localCoord) const;

	LayerTileId getLayerTileIdAtLocalCoord(sf::Vector2f localCoord) const;
	std::vector<LayerTileId> getLayerTileIdsAtLocalCoord(sf::Vector2f localCoord) const;

	std::size_t getGridTileIndexAtGridLocation(std::size_t gridIndex, sf::Vector2<std::size_t> location) const;
	std::size_t getGridHeight(std::size_t gridIndex) const;

	bool doesGridBoundsContainCoord(std::size_t gridIndex, sf::Vector2f localCoord) const;















private:
	const sf::Texture* m_texture;
	sf::View m_view;
	float m_depthMultiplier;
	sf::Vector2f m_vanishingPointOffsetFromCenter;
	bool m_useRangeZ;
	bool m_useRangeDepth;
	std::size_t m_rangeMinZ;
	std::size_t m_rangeMaxZ;
	float m_rangeMinDepth;
	float m_rangeMaxDepth;
	float m_depthOffset;

	mutable bool m_isUpdateRequired;
	mutable std::vector<sf::Vertex> m_vertices;

	void draw(sf::RenderTarget&, sf::RenderStates) const override;

	void priv_update() const;
};

} // namespace cheesemap

#include "Map.inl"
