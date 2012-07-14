/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_TEMPLE_OF_AHNQIRAJ_H
#define DEF_TEMPLE_OF_AHNQIRAJ_H

enum
{
    MAX_ENCOUNTER               = 4,

    TYPE_SKERAM                 = 0,
    TYPE_BUG_TRIO               = 1,
    TYPE_TWINS                  = 2,
    TYPE_CTHUN_PHASE            = 3,

    NPC_SKERAM                  = 15263,
    //NPC_KRI                   = 15511,
    //NPC_VEM                   = 15544,
    //NPC_YAUJ                  = 15543,
    NPC_VEKLOR                  = 15276,
    NPC_VEKNILASH               = 15275,
    NPC_MASTERS_EYE             = 15963,
    NPC_CTHUN                   = 15727,

    GO_SKERAM_GATE              = 180636,
    GO_TWINS_ENTER_DOOR         = 180634,
    GO_TWINS_EXIT_DOOR          = 180635,

    EMOTE_EYE_INTRO             = -1531012,
    SAY_EMPERORS_INTRO_1        = -1531013,
    SAY_EMPERORS_INTRO_2        = -1531014,
    SAY_EMPERORS_INTRO_3        = -1531015,
    SAY_EMPERORS_INTRO_4        = -1531016,
    SAY_EMPERORS_INTRO_5        = -1531017,
    SAY_EMPERORS_INTRO_6        = -1531018,

    AREATRIGGER_TWIN_EMPERORS   = 4047,

    SPELL_SUMMON_PLAYER         = 20477,
};

enum CThunPhase
{
    PHASE_NOT_STARTED           = 0,
    PHASE_EYE_NORMAL            = 1,
    PHASE_EYE_DARK_GLARE        = 2,
    PHASE_TRANSITION            = 3,
    PHASE_CTHUN                 = 4,
    PHASE_CTHUN_WEAKENED        = 5,
    PHASE_FINISH                = 6,
};

class MANGOS_DLL_DECL instance_temple_of_ahnqiraj : public ScriptedInstance
{
    public:
        instance_temple_of_ahnqiraj(Map* pMap);

        void Initialize();

        bool IsEncounterInProgress() const;

        void OnCreatureCreate(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        void DoHandleTempleAreaTrigger(uint32 uiTriggerId);

        const char* Save() { return m_strInstData.c_str(); }
        void Load(const char* chrIn);

        void Update(uint32 uiDiff) { m_dialogueHelper.DialogueUpdate(uiDiff); }

    private:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        uint8 m_uiBugTrioDeathCount;

        bool m_bIsEmperorsIntroDone;

        DialogueHelper m_dialogueHelper;
};

#endif
