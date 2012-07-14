/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Instance_Molten_Core
SD%Complete: 25
SDComment: Majordomos and Ragnaros Event missing
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

instance_molten_core::instance_molten_core(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
}

void instance_molten_core::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

bool instance_molten_core::IsEncounterInProgress() const
{
    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            return true;
    }

    return false;
}

void instance_molten_core::OnPlayerEnter(Player* pPlayer)
{
    // Summon Majordomo if can
    DoSpawnMajordomoIfCan(true);
}

void instance_molten_core::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        // Bosses
        case NPC_GARR:
        case NPC_SULFURON:
        case NPC_MAJORDOMO:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
    }
}

void instance_molten_core::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        // Runes
        case GO_RUNE_KRESS:
        case GO_RUNE_MOHN:
        case GO_RUNE_BLAZ:
        case GO_RUNE_MAZJ:
        case GO_RUNE_ZETH:
        case GO_RUNE_THERI:
        case GO_RUNE_KORO:

        // Majordomo event chest
        case GO_CACHE_OF_THE_FIRE_LORD:
        // Ragnaros GOs
        case GO_LAVA_STEAM:
        case GO_LAVA_SPLASH:
            m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            break;
    }
}

void instance_molten_core::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_LUCIFRON:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_MAGMADAR:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_GEHENNAS:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_GARR:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_SHAZZRAH:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_GEDDON:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_GOLEMAGG:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_SULFURON:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_MAJORDOMO:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoRespawnGameObject(GO_CACHE_OF_THE_FIRE_LORD, HOUR);
            break;
        case TYPE_RAGNAROS:
            m_auiEncounter[uiType] = uiData;
            break;
    }

    // Check if Majordomo can be summoned
    if (uiData == SPECIAL)
        DoSpawnMajordomoIfCan(false);

    if (uiData == DONE || uiData == SPECIAL)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
            << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
            << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " "
            << m_auiEncounter[9];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_molten_core::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

// Handle Majordomo summon here
void instance_molten_core::DoSpawnMajordomoIfCan(bool bByPlayerEnter)
{
    // If both Majordomo and Ragnaros events are finished, return
    if (m_auiEncounter[TYPE_MAJORDOMO] == DONE && m_auiEncounter[TYPE_RAGNAROS] == DONE)
        return;

    // If already spawned return
    if (GetSingleCreatureFromStorage(NPC_MAJORDOMO, true))
        return;

    // Check if all rune bosses are done
    for(uint8 i = TYPE_MAGMADAR; i < TYPE_MAJORDOMO; ++i)
    {
        if (m_auiEncounter[i] != SPECIAL)
            return;
    }

    Player* pPlayer = GetPlayerInMap();
    if (!pPlayer)
        return;

    // Summon Majordomo
    // If Majordomo encounter isn't done, summon at encounter place, else near Ragnaros
    uint8 uiSummonPos = m_auiEncounter[TYPE_MAJORDOMO] == DONE ? 1 : 0;
    if (Creature* pMajordomo = pPlayer->SummonCreature(m_aMajordomoLocations[uiSummonPos].m_uiEntry, m_aMajordomoLocations[uiSummonPos].m_fX, m_aMajordomoLocations[uiSummonPos].m_fY, m_aMajordomoLocations[uiSummonPos].m_fZ, m_aMajordomoLocations[uiSummonPos].m_fO, TEMPSUMMON_MANUAL_DESPAWN, 2*HOUR*IN_MILLISECONDS))
    {
        if (uiSummonPos)                                    // Majordomo encounter already done, set faction
        {
            pMajordomo->setFaction(FACTION_MAJORDOMO_FRIENDLY);
            pMajordomo->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
            pMajordomo->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
        else                                                // Else yell and summon adds
        {
            if (!bByPlayerEnter)
                DoScriptText(SAY_MAJORDOMO_SPAWN, pMajordomo);

            for (uint8 i = 0; i < MAX_MAJORDOMO_ADDS; ++i)
                pMajordomo->SummonCreature(m_aBosspawnLocs[i].m_uiEntry, m_aBosspawnLocs[i].m_fX, m_aBosspawnLocs[i].m_fY, m_aBosspawnLocs[i].m_fZ, m_aBosspawnLocs[i].m_fO, TEMPSUMMON_MANUAL_DESPAWN, DAY*IN_MILLISECONDS);
        }
    }
}

void instance_molten_core::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);

    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
    >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7]
    >> m_auiEncounter[8] >> m_auiEncounter[9];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstance_instance_molten_core(Map* pMap)
{
    return new instance_molten_core(pMap);
}

void AddSC_instance_molten_core()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_molten_core";
    pNewScript->GetInstanceData = &GetInstance_instance_molten_core;
    pNewScript->RegisterSelf();
}
