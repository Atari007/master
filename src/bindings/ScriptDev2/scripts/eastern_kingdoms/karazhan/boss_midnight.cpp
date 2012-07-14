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
SDName: Boss_Midnight
SD%Complete: 90
SDComment: Use SPELL_SUMMON_ATTUMEN and SPELL_SUMMON_ATTUMEN_MOUNTED instead of SummonCreature.
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_MIDNIGHT_KILL           = -1532000,
    SAY_APPEAR1                 = -1532001,
    SAY_APPEAR2                 = -1532002,
    SAY_APPEAR3                 = -1532003,
    SAY_MOUNT                   = -1532004,
    SAY_KILL1                   = -1532005,
    SAY_KILL2                   = -1532006,
    SAY_DISARMED                = -1532007,
    SAY_DEATH                   = -1532008,
    SAY_RANDOM1                 = -1532009,
    SAY_RANDOM2                 = -1532010,

    SPELL_SHADOWCLEAVE          = 29832,
    SPELL_INTANGIBLE_PRESENCE   = 29833,
    SPELL_BERSERKER_CHARGE      = 26561,                    //Only when mounted
    SPELL_SUMMON_ATTUMEN        = 29714,
    SPELL_SUMMON_ATTUMEN_MOUNTED= 29799,

    MOUNTED_DISPLAYID           = 16040,                    // should use creature 16152 instead of changing displayid

    //Attumen (TODO: Use the summoning spell instead of creature id. It works , but is not convenient for us)
    SUMMON_ATTUMEN              = 15550
};

struct MANGOS_DLL_DECL boss_midnightAI : public ScriptedAI
{
    boss_midnightAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    ObjectGuid m_attumenGuid;
    uint8  m_uiPhase;
    uint32 m_uiMount_Timer;

    void Reset()
    {
        m_uiPhase = 1;
        m_uiMount_Timer = 0;

        m_creature->SetVisibility(VISIBILITY_ON);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (m_uiPhase == 2)
        {
            if (Creature* pAttumen = m_creature->GetMap()->GetCreature(m_attumenGuid))
                DoScriptText(SAY_MIDNIGHT_KILL, pAttumen);
        }
    }

    void Mount(Creature* pAttumen)
    {
        DoScriptText(SAY_MOUNT, pAttumen);
        m_uiPhase = 3;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pAttumen->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        float fAngle = m_creature->GetAngle(pAttumen);
        float fDistance = m_creature->GetDistance2d(pAttumen);

        float fNewX = m_creature->GetPositionX() + cos(fAngle)*(fDistance/2) ;
        float fNewY = m_creature->GetPositionY() + sin(fAngle)*(fDistance/2) ;
        float fNewZ = 50.0f;

        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MovePoint(0, fNewX, fNewY, fNewZ);

        fDistance += 10.0f;
        fNewX = m_creature->GetPositionX() + cos(fAngle)*(fDistance/2);
        fNewY = m_creature->GetPositionY() + sin(fAngle)*(fDistance/2);

        pAttumen->GetMotionMaster()->Clear();
        pAttumen->GetMotionMaster()->MovePoint(0, fNewX, fNewY, fNewZ);

        m_uiMount_Timer = 1000;
    }

    void SetMidnight(Creature*, ObjectGuid);                // Below ..

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch(m_uiPhase)
        {
            case 1:
                if (m_creature->GetHealthPercent() < 95.0f)
                {
                    m_uiPhase = 2;

                    if (Creature* pAttumen = m_creature->SummonCreature(SUMMON_ATTUMEN, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30000))
                    {
                        m_attumenGuid = pAttumen->GetObjectGuid();
                        pAttumen->AI()->AttackStart(m_creature->getVictim());
                        SetMidnight(pAttumen, m_creature->GetObjectGuid());

                        switch(urand(0, 2))
                        {
                            case 0: DoScriptText(SAY_APPEAR1, pAttumen); break;
                            case 1: DoScriptText(SAY_APPEAR2, pAttumen); break;
                            case 2: DoScriptText(SAY_APPEAR3, pAttumen); break;
                        }
                    }
                }
                break;
            case 2:
                if (m_creature->GetHealthPercent() < 25.0f)
                {
                    if (Creature* pAttumen = m_creature->GetMap()->GetCreature(m_attumenGuid))
                        Mount(pAttumen);
                }
                break;
            case 3:
                if (m_uiMount_Timer)
                {
                    if (m_uiMount_Timer <= uiDiff)
                    {
                        m_uiMount_Timer = 0;
                        m_creature->SetVisibility(VISIBILITY_OFF);
                        m_creature->GetMotionMaster()->MoveIdle();

                        if (Creature *pAttumen = m_creature->GetMap()->GetCreature(m_attumenGuid))
                        {
                            pAttumen->SetDisplayId(MOUNTED_DISPLAYID);
                            pAttumen->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                            if (pAttumen->getVictim())
                            {
                                pAttumen->GetMotionMaster()->MoveChase(pAttumen->getVictim());
                                pAttumen->SetTargetGuid(pAttumen->getVictim()->GetObjectGuid());
                            }
                            pAttumen->SetFloatValue(OBJECT_FIELD_SCALE_X,1);
                        }
                    }
                    else
                        m_uiMount_Timer -= uiDiff;
                }
                break;
        }

        if (m_uiPhase != 3)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_midnight(Creature* pCreature)
{
    return new boss_midnightAI(pCreature);
}

struct MANGOS_DLL_DECL boss_attumenAI : public ScriptedAI
{
    boss_attumenAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_uiPhase = 1;

        m_uiCleaveTimer = urand(10000, 16000);
        m_uiCurseTimer = 30000;
        m_uiRandomYellTimer = urand(30000, 60000);          //Occasionally yell
        m_uiChargeTimer = 20000;
        m_uiResetTimer = 0;
    }

    ObjectGuid m_midnightGuid;
    uint8  m_uiPhase;
    uint32 m_uiCleaveTimer;
    uint32 m_uiCurseTimer;
    uint32 m_uiRandomYellTimer;
    uint32 m_uiChargeTimer;                                 //only when mounted
    uint32 m_uiResetTimer;

    void Reset()
    {
        m_uiResetTimer = 2000;
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void SpellHit(Unit* pSource, const SpellEntry* pSpell)
    {
        if (pSpell->Mechanic == MECHANIC_DISARM)
            DoScriptText(SAY_DISARMED, m_creature);
    }

    void JustDied(Unit* pVictim)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (Creature* pMidnight = m_creature->GetMap()->GetCreature(m_midnightGuid))
            pMidnight->DealDamage(pMidnight, pMidnight->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiResetTimer)
        {
            if (m_uiResetTimer <= uiDiff)
            {
                m_uiResetTimer = 0;

                if (Creature *pMidnight = m_creature->GetMap()->GetCreature(m_midnightGuid))
                {
                    pMidnight->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pMidnight->SetVisibility(VISIBILITY_ON);
                }
                m_midnightGuid.Clear();

                m_creature->SetVisibility(VISIBILITY_OFF);
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
            else
                m_uiResetTimer -= uiDiff;
        }

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE))
            return;

        if (m_uiCleaveTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOWCLEAVE);
            m_uiCleaveTimer = urand(10000, 16000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        if (m_uiCurseTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_INTANGIBLE_PRESENCE);
            m_uiCurseTimer = 30000;
        }
        else
            m_uiCurseTimer -= uiDiff;

        if (m_uiRandomYellTimer < uiDiff)
        {
            DoScriptText(urand(0, 1) ? SAY_RANDOM1 : SAY_RANDOM2, m_creature);
            m_uiRandomYellTimer = urand(30000, 60000);
        }
        else
            m_uiRandomYellTimer -= uiDiff;

        if (m_creature->GetDisplayId() == MOUNTED_DISPLAYID)
        {
            if (m_uiChargeTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_BERSERKER_CHARGE, SELECT_FLAG_NOT_IN_MELEE_RANGE | SELECT_FLAG_IN_LOS))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_BERSERKER_CHARGE) == CAST_OK)
                        m_uiChargeTimer = 20000;
                }
                else
                    m_uiChargeTimer = 2000;
            }
            else
                m_uiChargeTimer -= uiDiff;
        }
        else
        {
            if (m_creature->GetHealthPercent() < 25.0f)
            {
                if (Creature *pMidnight = m_creature->GetMap()->GetCreature(m_midnightGuid))
                {
                    if (boss_midnightAI* pMidnightAI = dynamic_cast<boss_midnightAI*>(pMidnight->AI()))
                        pMidnightAI->Mount(m_creature);

                    m_creature->SetHealth(pMidnight->GetHealth());
                    DoResetThreat();
                }
            }
        }

        DoMeleeAttackIfReady();
    }
};

void boss_midnightAI::SetMidnight(Creature* pAttumen, ObjectGuid midnightGuid)
{
    if (boss_attumenAI* pAttumenAI = dynamic_cast<boss_attumenAI*>(pAttumen->AI()))
        pAttumenAI->m_midnightGuid = midnightGuid;
}

CreatureAI* GetAI_boss_attumen(Creature* pCreature)
{
    return new boss_attumenAI(pCreature);
}

void AddSC_boss_attumen()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_attumen";
    pNewScript->GetAI = &GetAI_boss_attumen;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_midnight";
    pNewScript->GetAI = &GetAI_boss_midnight;
    pNewScript->RegisterSelf();
}
