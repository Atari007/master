/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_OLD_HILLSBRAD_H
#define DEF_OLD_HILLSBRAD_H

enum
{
    MAX_ENCOUNTER                   = 6,

    TYPE_BARREL_DIVERSION           = 0,
    TYPE_THRALL_EVENT               = 1,
    TYPE_THRALL_PART1               = 2,                    // prison to keep
    TYPE_THRALL_PART2               = 3,                    // keep to barn
    TYPE_THRALL_PART3               = 4,                    // barn to inn
    TYPE_THRALL_PART4               = 5,                    // inn to boss

    NPC_THRALL                      = 17876,
    NPC_TARETHA                     = 18887,
    NPC_DRAKE                       = 17848,
    NPC_LODGE_QUEST_TRIGGER         = 20155,
    NPC_EPOCH                       = 18096,

    QUEST_ENTRY_HILLSBRAD           = 10282,
    QUEST_ENTRY_DIVERSION           = 10283,
    QUEST_ENTRY_ESCAPE              = 10284,
    QUEST_ENTRY_RETURN              = 10285,

    WORLD_STATE_OH                  = 2436,
};

class MANGOS_DLL_DECL instance_old_hillsbrad : public ScriptedInstance
{
    public:
        instance_old_hillsbrad(Map* pMap);
        ~instance_old_hillsbrad() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
        void OnCreatureDeath(Creature* pCreature);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        void HandleThrallRelocation();

        void UpdateLodgeQuestCredit();

        Creature* GetThrall() { return GetSingleCreatureFromStorage(NPC_THRALL); }
        Creature* GetTaretha() { return GetSingleCreatureFromStorage(NPC_TARETHA); }
        Creature* GetEpoch() { return GetSingleCreatureFromStorage(NPC_EPOCH); }

    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];

        uint32 m_uiBarrelCount;
        uint32 m_uiThrallEventCount;
};

#endif
