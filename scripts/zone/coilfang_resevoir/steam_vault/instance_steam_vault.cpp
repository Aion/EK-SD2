/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Instance_Steam_Vault
SD%Complete: 80
SDComment: Instance script and access panel GO
SDCategory: Coilfang Resevoir, The Steamvault
EndScriptData */

#include "precompiled.h"
#include "def_steam_vault.h"

#define ENCOUNTERS 4

#define MAIN_CHAMBERS_DOOR      183049
#define ACCESS_PANEL_HYDRO      184125
#define ACCESS_PANEL_MEK        184126

/* Steam Vaults encounters:
1 - Hydromancer Thespia Event
2 - Mekgineer Steamrigger Event
3 - Warlord Kalithresh Event
*/

bool GOHello_go_main_chambers_access_panel(Player* pPlayer, GameObject* _GO)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)_GO->GetInstanceData();

    if (!pInstance)
        return false;

    if (_GO->GetEntry() == ACCESS_PANEL_HYDRO && pInstance->GetData(TYPE_HYDROMANCER_THESPIA) == DONE)
        pInstance->SetData(TYPE_HYDROMANCER_THESPIA,SPECIAL);

    if (_GO->GetEntry() == ACCESS_PANEL_MEK && pInstance->GetData(TYPE_MEKGINEER_STEAMRIGGER) == DONE)
        pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER,SPECIAL);

    return true;
}

struct MANGOS_DLL_DECL instance_steam_vault : public ScriptedInstance
{
    instance_steam_vault(Map *map) : ScriptedInstance(map) {Initialize();};

    uint32 Encounter[ENCOUNTERS];

    uint64 ThespiaGUID;
    uint64 MekgineerGUID;
    uint64 KalithreshGUID;

    uint64 MainChambersDoor;
    uint64 AccessPanelHydro;
    uint64 AccessPanelMek;

    void Initialize()
    {
        ThespiaGUID = 0;
        MekgineerGUID = 0;
        KalithreshGUID = 0;
        MainChambersDoor = 0;
        AccessPanelHydro = 0;
        AccessPanelMek = 0;

        for(uint8 i = 0; i < ENCOUNTERS; i++)
            Encounter[i] = NOT_STARTED;
    }

    bool IsEncounterInProgress() const
    {
        for(uint8 i = 0; i < ENCOUNTERS; i++)
            if (Encounter[i] == IN_PROGRESS)
                return true;

        return false;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case 17797: ThespiaGUID = pCreature->GetGUID(); break;
            case 17796: MekgineerGUID = pCreature->GetGUID(); break;
            case 17798: KalithreshGUID = pCreature->GetGUID(); break;
        }
    }

    void OnObjectCreate(GameObject *go)
    {
        switch(go->GetEntry())
        {
            case MAIN_CHAMBERS_DOOR: MainChambersDoor = go->GetGUID(); break;
            case ACCESS_PANEL_HYDRO: AccessPanelHydro = go->GetGUID(); break;
            case ACCESS_PANEL_MEK:   AccessPanelMek = go->GetGUID(); break;
        }
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
            case TYPE_HYDROMANCER_THESPIA:
                if (data == SPECIAL)
                {
                    if (GameObject* pGo = instance->GetGameObject(AccessPanelHydro))
                        pGo->SetGoState(GO_STATE_ACTIVE);

                    if (GetData(TYPE_MEKGINEER_STEAMRIGGER) == SPECIAL)
                    {
                        if (GameObject* pGo = instance->GetGameObject(MainChambersDoor))
                            pGo->SetGoState(GO_STATE_ACTIVE);
                    }

                    debug_log("SD2: Instance Steamvault: Access panel used.");
                }
                Encounter[0] = data;
                break;
            case TYPE_MEKGINEER_STEAMRIGGER:
                if (data == SPECIAL)
                {
                    if (GameObject* pGo = instance->GetGameObject(AccessPanelMek))
                        pGo->SetGoState(GO_STATE_ACTIVE);

                    if (GetData(TYPE_HYDROMANCER_THESPIA) == SPECIAL)
                    {
                        if (GameObject* pGo = instance->GetGameObject(MainChambersDoor))
                            pGo->SetGoState(GO_STATE_ACTIVE);
                    }

                    debug_log("SD2: Instance Steamvault: Access panel used.");
                }
                Encounter[1] = data;
                break;
            case TYPE_WARLORD_KALITHRESH:
                Encounter[2] = data;
                break;
            case TYPE_DISTILLER:
                Encounter[3] = data;
                break;
        }
    }

    uint32 GetData(uint32 type)
    {
        switch(type)
        {
            case TYPE_HYDROMANCER_THESPIA:
                return Encounter[0];
            case TYPE_MEKGINEER_STEAMRIGGER:
                return Encounter[1];
            case TYPE_WARLORD_KALITHRESH:
                return Encounter[2];
            case TYPE_DISTILLER:
                return Encounter[3];
        }
        return 0;
    }

    uint64 GetData64(uint32 data)
    {
        switch(data)
        {
            case DATA_THESPIA:
                return ThespiaGUID;
            case DATA_MEKGINEERSTEAMRIGGER:
                return MekgineerGUID;
            case DATA_KALITRESH:
                return KalithreshGUID;
        }
        return 0;
    }
};

InstanceData* GetInstanceData_instance_steam_vault(Map* map)
{
    return new instance_steam_vault(map);
}

void AddSC_instance_steam_vault()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "go_main_chambers_access_panel";
    newscript->pGOHello = &GOHello_go_main_chambers_access_panel;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "instance_steam_vault";
    newscript->GetInstanceData = &GetInstanceData_instance_steam_vault;
    newscript->RegisterSelf();
}
