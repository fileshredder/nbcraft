#include "DyePowderItem.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Player.hpp"
#include "world/entity/Sheep.hpp"
#include "world/tile/Tile.hpp"
#include "world/tile/Sapling.hpp"
#include "world/tile/CropsTile.hpp"
#include "world/tile/ClothTile.hpp"
#include "world/entity/EntityType.hpp"
#include "DyeColor.hpp"
#include "compat/GameVersion.h"

DyePowderItem::DyePowderItem(int itemID) : Item(itemID)
{
	m_bStackedByData = true;
	m_maxDamage = 0;
}

int DyePowderItem::getIcon(const ItemStack* item) const
{
	int i = item->getAuxValue();
	return m_icon + i % 8 * 16 + i / 8;
}

std::string DyePowderItem::getDescriptionId() const
{
	return Item::getDescriptionId() + "." + DyeColor::IDS[0];
}

std::string DyePowderItem::getDescriptionId(ItemStack* item) const
{
	return Item::getDescriptionId() + "." + DyeColor::IDS[item->getAuxValue()];
}

bool DyePowderItem::useOn(ItemStack* item, Player* player, Level* level, const TilePos& pos, Facing::Name face) const
{
    // Aux value 15 is bonemeal
    if (item->getAuxValue() == 15)
	{
		TileID tile = level->getTile(pos);
		
		if (tile == Tile::sapling->m_ID)
		{
			(static_cast<Sapling*>(Tile::sapling))->growTree(level, pos, &level->m_random);
			item->m_count--;
			return true;
		}
		
		if (tile == Tile::crops->m_ID)
		{
			static_cast<CropsTile*>(Tile::crops)->growCropsToMax(level, pos);
			item->m_count--;
			return true;
		}

#if MC_VERSION >= MC_VER_BETA(1, 6, 6)
		if (tile == Tile::grass->m_ID)
		{
			item->m_count--;

			for (int i = 0; i < 128; i++)
			{
				int k = pos.x;
				int l = pos.y + 1;
				int m = pos.z;

				bool isValid = true;

				for (int n = 0; n < i / 16; n++) {
					k += level->m_random.nextInt(3) - 1;
					l += (level->m_random.nextInt(3) - 1) * level->m_random.nextInt(3) / 2;
					m += level->m_random.nextInt(3) - 1;

					TilePos checkPos(k, l, m);

					if (level->getTile(checkPos.below()) != Tile::grass->m_ID || level->isSolidTile(checkPos)) {
						isValid = false;
						break;
					}
				}

				TilePos targetPos(k, l, m);

				if (isValid)
				{
					if (level->getTile(targetPos) == 0) {
						if (level->m_random.nextInt(10) != 0) {
							level->setTileAndData(targetPos, Tile::tallGrass->m_ID, 1);
						}
						else if (level->m_random.nextInt(3) != 0) {
							level->setTile(targetPos, Tile::flower->m_ID);
						}
						else {
							level->setTile(targetPos, Tile::rose->m_ID);
						}
					}
				}
			}

			return true;
		}
#endif
	}
	
	return false;
}

void DyePowderItem::interactEnemy(ItemStack* item, Mob* mob) const
{
	if (!mob->getDescriptor().isType(EntityType::SHEEP))
        return;

    Sheep* sheep = static_cast<Sheep*>(mob);
    if (sheep->isSheared())
        return;

    int color = ClothTile::getColorFromData(item->getAuxValue());
    
    if (sheep->getColor() != color)
    {
        sheep->setColor(color);
        item->m_count--;
    }
}
