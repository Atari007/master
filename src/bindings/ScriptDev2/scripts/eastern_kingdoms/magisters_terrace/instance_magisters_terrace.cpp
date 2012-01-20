/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Instance_Magisters_Terrace
SD%Complete: 60
SDComment:  Designed only for Selin Fireheart
SDCategory: Magister's Terrace
EndScriptData */

#include "precompiled.h"
#include "magisters_terrace.h"

/*
0  - Selin Fireheart
1  - Vexallus
2  - Priestess Delrissa
3  - Kael'thas Sunstrider
*/

instance_magisters_terrace::instance_magisters_terrace(Map* pMap) : ScriptedInstance(pMap),
    m_uiDelrissaDeathCount(0)
{
    Initialize();
}

void instance_magisters_terrace::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_magisters_terrace::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_SELIN_FIREHEART:
        case NPC_DELRISSA:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_FEL_CRYSTAL:
            m_lFelCrystalGuid.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_magisters_terrace::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_VEXALLUS_DOOR:
            if (m_auiEncounter[TYPE_VEXALLUS] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SELIN_DOOR:
            if (m_auiEncounter[TYPE_SELIN] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_DELRISSA_DOOR:
            if (m_auiEncounter[TYPE_DELRISSA] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SELIN_ENCOUNTER_DOOR:
        case GO_KAEL_DOOR:
        case GO_KAEL_STATUE_LEFT:
        case GO_KAEL_STATUE_RIGHT:
            break;

        default:
            return;
    }
    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_magisters_terrace::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_SELIN:
            if (uiData == DONE)
                DoUseDoorOrButton(GO_SELIN_DOOR);
            DoUseDoorOrButton(GO_SELIN_ENCOUNTER_DOOR);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_VEXALLUS:
            if (uiData == DONE)
                DoUseDoorOrButton(GO_VEXALLUS_DOOR);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_DELRISSA:
            if (uiData == DONE)
                DoUseDoorOrButton(GO_DELRISSA_DOOR);
            if (uiData == IN_PROGRESS)
                m_uiDelrissaDeathCount = 0;
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_KAELTHAS:
            DoUseDoorOrButton(GO_KAEL_DOOR);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_DELRISSA_DEATH_COUNT:
            if (uiData == SPECIAL)
                ++m_uiDelrissaDeathCount;
            else
                m_uiDelrissaDeathCount = 0;
            return;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_magisters_terrace::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_magisters_terrace::GetData(uint32 uiType)
{
    switch (uiType)
    {
        case TYPE_SELIN:                return m_auiEncounter[0];
        case TYPE_VEXALLUS:             return m_auiEncounter[1];
        case TYPE_DELRISSA:             return m_auiEncounter[2];
        case TYPE_KAELTHAS:             return m_auiEncounter[3];
        case TYPE_DELRISSA_DEATH_COUNT: return m_uiDelrissaDeathCount;

        default:
            return 0;
    }
}

void instance_magisters_terrace::GetFelCrystalList(GUIDList& lList)
{
    if (m_lFelCrystalGuid.empty())
        error_log("SD2: Magisters Terrace: No Fel Crystals loaded in Inst Data");

    lList = m_lFelCrystalGuid;
}

InstanceData* GetInstanceData_instance_magisters_terrace(Map* pMap)
{
    return new instance_magisters_terrace(pMap);
}

void AddSC_instance_magisters_terrace()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_magisters_terrace";
    pNewScript->GetInstanceData = &GetInstanceData_instance_magisters_terrace;
    pNewScript->RegisterSelf();
}
