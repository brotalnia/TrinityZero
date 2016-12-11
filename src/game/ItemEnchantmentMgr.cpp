/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2009-2010 TrinityZero <http://www.trinityzero.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <functional>
#include "ItemEnchantmentMgr.h"
#include "Database/DatabaseEnv.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "ProgressBar.h"
#include <list>
#include <vector>
#include "Util.h"

struct EnchStoreItem
{
    uint32  ench;
    float   chance;

    EnchStoreItem()
        : ench(0), chance(0) {}

    EnchStoreItem(uint32 _ench, float _chance)
        : ench(_ench), chance(_chance) {}
};

typedef std::vector<EnchStoreItem> EnchStoreList;
typedef UNORDERED_MAP<uint32, EnchStoreList> EnchantmentStore;

static EnchantmentStore RandomItemEnch;

void LoadRandomEnchantmentsTable()
{
    RandomItemEnch.clear();                                 // for reload case

    EnchantmentStore::iterator tab;
    uint32 entry, ench;
    float chance;
    uint32 count = 0;

    QueryResult *result = WorldDatabase.Query("SELECT entry, ench, chance FROM item_enchantment_template");

    if (result)
    {
        barGoLink bar(result->GetRowCount());

        do
        {
            Field *fields = result->Fetch();
            bar.step();

            entry = fields[0].GetUInt32();
            ench = fields[1].GetUInt32();
            chance = fields[2].GetFloat();

            if (chance > 0.000001f && chance <= 100.0f)
                RandomItemEnch[entry].push_back( EnchStoreItem(ench, chance) );

            ++count;
        } while (result->NextRow());

        delete result;

        sLog.outString();
        sLog.outString( ">> Loaded %u Item Enchantment definitions", count );
    }
    else
    {
        sLog.outString();
        sLog.outErrorDb( ">> Loaded 0 Item Enchantment definitions. DB table `item_enchantment_template` is empty.");
    }
}

uint32 GetItemEnchantMod(uint32 entry)
{
    if (!entry) return 0;

    EnchantmentStore::iterator tab = RandomItemEnch.find(entry);

    if (tab == RandomItemEnch.end())
    {
        sLog.outErrorDb("Item RandomProperty id #%u used in `item_template` but it doesn't have records in `item_enchantment_template` table.",entry);
        return 0;
    }

    double dRoll = rand_chance();
    float fCount = 0;

    for(EnchStoreList::iterator ench_iter = tab->second.begin(); ench_iter != tab->second.end(); ++ench_iter)
    {
        fCount += ench_iter->chance;

        if (fCount > dRoll) return ench_iter->ench;
    }

    //we could get here only if sum of all enchantment chances is lower than 100%
    dRoll =  (irand(0, (int)floor(fCount * 100) + 1)) / 100;
    fCount = 0;

    for(EnchStoreList::iterator ench_iter = tab->second.begin(); ench_iter != tab->second.end(); ++ench_iter)
    {
        fCount += ench_iter->chance;

        if (fCount > dRoll) return ench_iter->ench;
    }

    return 0;
}