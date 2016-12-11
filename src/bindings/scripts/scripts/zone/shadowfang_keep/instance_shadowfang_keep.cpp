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
SDName: Instance_Shadowfang_Keep
SD%Complete: 75
SDComment: TODO: check what other parts would require additional code (ex: make sure door are in open state if boss dead)
SDCategory: Shadowfang Keep
EndScriptData */

#include "precompiled.h"
#include "def_shadowfang_keep.h"

#define ENCOUNTERS              4

struct TRINITY_DLL_DECL instance_shadowfang_keep : public ScriptedInstance
{
    instance_shadowfang_keep(Map *map) : ScriptedInstance(map) {Initialize();};

    uint32 Encounter[ENCOUNTERS];
    std::string str_data;

    uint64 DoorCourtyardGUID;
    uint64 DoorSorcererGUID;
    uint64 DoorArugalGUID;

    void Initialize()
    {
        DoorCourtyardGUID = 0;
        DoorSorcererGUID = 0;
        DoorArugalGUID = 0;

         for(uint8 i=0; i < ENCOUNTERS; ++i)
             Encounter[i] = NOT_STARTED;
    }

    Player* GetPlayerInMap()
    {
        Map::PlayerList const& players = instance->GetPlayers();

        if (!players.isEmpty())
        {
            for(Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (Player* plr = itr->getSource())
                    return plr;
            }
        }
        debug_log("TSCR: Instance Shadowfang Keep: GetPlayerInMap, but PlayerList is empty!");
        return NULL;
    }

    void OnObjectCreate(GameObject *go)
    {
        switch(go->GetEntry())
        {
        case 18895: DoorCourtyardGUID = go->GetGUID(); break;
        case 18972: DoorSorcererGUID = go->GetGUID(); break;
        case 18971: DoorArugalGUID = go->GetGUID(); break;
        }
    }

    void HandleGameObject(uint64 guid, uint32 state)
    {
        Player *player = GetPlayerInMap();

        if (!player || !guid)
        {
            debug_log("TSCR: Instance Shadowfang Keep: HandleGameObject fail");
            return;
        }

        if (GameObject *go = GameObject::GetGameObject(*player,guid))
            go->SetGoState(state);
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
            case TYPE_FREE_NPC:
                if(data == DONE)
                    HandleGameObject(DoorCourtyardGUID,0);
                Encounter[0] = data;
                break;
            case TYPE_RETHILGORE:
                Encounter[1] = data;
                break;
            case TYPE_FENRUS:
                if(data == DONE)
                    HandleGameObject(DoorSorcererGUID,0);
                Encounter[2] = data;
                break;
            case TYPE_NANDOS:
                if(data == DONE)
                    HandleGameObject(DoorArugalGUID,0);
                Encounter[3] = data;
                break;
        }

        if (data == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << Encounter[0] << " " << Encounter[1] << " " << Encounter[2] << " " << Encounter[3];

            str_data = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 type)
    {
        switch(type)
        {
            case TYPE_FREE_NPC:
                return Encounter[0];
            case TYPE_RETHILGORE:
                return Encounter[1];
            case TYPE_FENRUS:
                return Encounter[2];
            case TYPE_NANDOS:
                return Encounter[3];
        }
        return 0;
    }

    const char* Save()
    {
        return str_data.c_str();
    }

    void Load(const char* in)
    {
        if (!in)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(in);

        std::istringstream loadStream(in);
        loadStream >> Encounter[0] >> Encounter[1] >> Encounter[2] >> Encounter[3];

        for(uint8 i = 0; i < ENCOUNTERS; ++i)
            if (Encounter[i] == IN_PROGRESS)
                Encounter[i] = NOT_STARTED;

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_shadowfang_keep(Map* map)
{
    return new instance_shadowfang_keep(map);
}

void AddSC_instance_shadowfang_keep()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_shadowfang_keep";
    newscript->GetInstanceData = &GetInstanceData_instance_shadowfang_keep;
    newscript->RegisterSelf();
}

