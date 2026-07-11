/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TntTile.hpp"
#include "world/level/Level.hpp"
#include "world/entity/PrimedTnt.hpp"
#include "compat/GameVersion.h"

#define C_EXPLODE_BIT 1

TntTile::TntTile(int id, int texture) : Tile(id, texture, Material::explosive)
{

}

int TntTile::getResourceCount(Random* random) const
{
#if MC_VERSION >= MC_VER_BETA(1, 7, 0)
	return 1;
#else
	return 0;
#endif
}

void TntTile::onPlace(Level* level, const TilePos& pos)
{
	Tile::onPlace(level, pos);
#if MC_VERSION >= MC_VER_BETA(1, 7, 0)
	if (level->hasNeighborSignal(pos))
	{
		destroy(level, pos, C_EXPLODE_BIT);
		level->setTile(pos, TILE_AIR);
	}
#endif
}

int TntTile::getTexture(Facing::Name face) const
{
	switch (face)
	{
	case Facing::DOWN:
		return TEXTURE_TNT_BOTTOM;
	case Facing::UP:
		return TEXTURE_TNT_TOP;
	default:
		return TEXTURE_TNT_SIDE;
	}
}

void TntTile::destroy(Level* level, const TilePos& pos, TileData data)
{
	// prevent players from using this in multiplayer, to prevent a desync of player IDs
	if (level->m_bIsClientSide) return;

#if MC_VERSION >= MC_VER_BETA(1, 7, 0)
	if ((data & C_EXPLODE_BIT) == 1)
	{
		level->addEntity(new PrimedTnt(level, Vec3(pos) + 0.5f));
		level->playSound(pos + 0.5f, "random.fuse", 1.0f, 1.0f);
	}
#else
	level->addEntity(new PrimedTnt(level, Vec3(pos) + 0.5f));
#endif
}

void TntTile::wasExploded(Level* level, const TilePos& pos)
{
	PrimedTnt* tnt = new PrimedTnt(level, Vec3(pos) + 0.5f);
	tnt->m_fuseTimer = level->m_random.nextInt(tnt->m_fuseTimer / 4) + tnt->m_fuseTimer / 8;
	level->addEntity(tnt);
}

void TntTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	// @NOTE: Unused redstone
	if (tile > 0 && Tile::tiles[tile]->isSignalSource() && level->hasNeighborSignal(pos))
	{
		destroy(level, pos, C_EXPLODE_BIT);
		level->setTile(pos, TILE_AIR);
	}
}

bool TntTile::use(Level* level, const TilePos& pos, Player* player)
{
#if MC_VERSION >= MC_VER_BETA(1, 7, 0)
	if (player->getSelectedItem().getId() == Item::flintAndSteel->m_itemID)
	{
		destroy(level, pos, C_EXPLODE_BIT);
		level->setTile(pos, TILE_AIR);
		return true;
	}
#endif
	return Tile::use(level, pos, player);
}

