/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_WAILING_CAVERNS_H
#define DEF_WAILING_CAVERNS_H

enum
{
    MAX_ENCOUNTER   = 6,

    TYPE_ANACONDRA  = 0,
    TYPE_COBRAHN    = 1,
    TYPE_PYTHAS     = 2,
    TYPE_SERPENTIS  = 3,
    TYPE_DISCIPLE   = 4,
    TYPE_MUTANUS    = 5,

    NPC_NARALEX     = 3679,
    NPC_DISCIPLE    = 3678,

    SAY_INTRO       = -1043000,                             // Say when the first 4 encounter are DONE

    GO_MYSTERIOUS_CHEST     = 180055,                       // used for quest 7944; spawns in the instance only if one of the players has the quest

    QUEST_FORTUNE_AWAITS    = 7944,
};

class MANGOS_DLL_DECL instance_wailing_caverns : public ScriptedInstance
{
    public:
        instance_wailing_caverns(Map* pMap);
        ~instance_wailing_caverns() {}

        void Initialize();

        void OnPlayerEnter(Player* pPlayer);
        void OnCreatureCreate(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return m_strInstData.c_str(); }
        void Load(const char* chrIn);

    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;
};
#endif
