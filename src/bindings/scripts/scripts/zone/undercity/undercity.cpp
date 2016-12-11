/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Undercity
SD%Complete: 95
SDComment: Quest support: 6628.
SDCategory: Undercity
EndScriptData */

/* ContentData
npc_parqual_fintallas
EndContentData */

#include "precompiled.h"

/*######
## npc_parqual_fintallas
######*/

#define SPELL_MARK_OF_SHAME 6767

#define GOSSIP_HPF1 "Gul'dan"
#define GOSSIP_HPF2 "Kel'Thuzad"
#define GOSSIP_HPF3 "Ner'zhul"

bool GossipHello_npc_parqual_fintallas(Player *player, Creature *_Creature)
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );

    if (player->GetQuestStatus(6628) == QUEST_STATUS_INCOMPLETE && !player->HasAura(SPELL_MARK_OF_SHAME,0) )
    {
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_HPF1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_HPF2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_HPF3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        player->SEND_GOSSIP_MENU(5822, _Creature->GetGUID());
    }
    else
        player->SEND_GOSSIP_MENU(5821, _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_parqual_fintallas(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
        player->CLOSE_GOSSIP_MENU();
        _Creature->CastSpell(player,SPELL_MARK_OF_SHAME,false);
    }
    if (action == GOSSIP_ACTION_INFO_DEF+2)
    {
        player->CLOSE_GOSSIP_MENU();
        player->AreaExploredOrEventHappens(6628);
    }
    return true;
}

/*######
## AddSC
######*/

void AddSC_undercity()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="npc_parqual_fintallas";
    newscript->pGossipHello = &GossipHello_npc_parqual_fintallas;
    newscript->pGossipSelect = &GossipSelect_npc_parqual_fintallas;
    newscript->RegisterSelf();
}

