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
SDName: boss_anetheron
SD%Complete: 
SDComment: 
SDCategory: Caverns of Time, Mount Hyjal
SDAuthor: Dexter, <http://excalibur-wow.com>
EndScriptData */

#include "precompiled.h"
#include "hyjal.h"

enum
{
	//Spells
	SPELL_CARRION_SWARM			= 31306,
	SPELL_SLEEP					= 31298,
	SPELL_VAMPIRIC_AURA			= 38196,
	SPELL_INFERNO				= 31299, 
	SPELL_IMMOLATION_AURA		= 31303,
	SPELL_INFERNO_EFFECT		= 31302, 
	SPELL_STUN					= 39568, 
	SPELL_IMMOLATION_VISUAL		= 42726,
	SPELL_BERSERK				= 26662,

	//Texts
	YELL_AGGRO					= -1999940,
	YELL_CARRION_SWARM_1		= -1999941,
	YELL_CARRION_SWARM_2		= -1999942,
	YELL_SLEEP_1				= -1999943,
	YELL_SLEEP_2				= -1999944,
	YELL_KILL_1					= -1999945,
	YELL_KILL_2					= -1999946,
	YELL_KILL_3					= -1999947,
	YELL_SUMMON_INFERNO_1		= -1999948,
	YELL_SUMMON_INFERNO_2		= -1999949,
	YELL_DEATH					= -1999950,
};

/*######
## boss_anetheron
######*/

struct MANGOS_DLL_DECL boss_anetheronAI : public ScriptedAI
{
    boss_anetheronAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
	ScriptedInstance* m_pInstance;

	uint32 m_uiCarrionSwarmTimer;
    uint32 m_uiSleepTimer;
    uint32 m_uiVampiricAuraTimer;
	uint32 m_uiInfernoTimer;
	uint32 m_uiImmolationTimer;
    uint32 m_uiBerserkTimer;

	void Reset()
    {
		m_uiCarrionSwarmTimer = 14000;
		m_uiSleepTimer = 16000;
		m_uiInfernoTimer = 60000;
		m_uiVampiricAuraTimer = 5000;
		m_uiBerserkTimer = 600000;
	}

	void JustReachedHome()
	{
		if(m_pInstance)
		{
			if(m_pInstance->GetData(TYPE_ANETHERON) != FAIL)
				m_pInstance->SetData(TYPE_ANETHERON, FAIL);
		}
	}

	void Aggro(Unit* pWho)
    {
        if(pWho->GetTypeId() != TYPEID_PLAYER)
			return;

		DoScriptText(YELL_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANETHERON, IN_PROGRESS);
    }

	void KilledUnit(Unit* pVictim)
    {
		if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        switch(urand(0, 2))
        {
            case 0: DoScriptText(YELL_KILL_1, m_creature); break;
            case 1: DoScriptText(YELL_KILL_2, m_creature); break;
            case 2: DoScriptText(YELL_KILL_3, m_creature); break;
        }
    }

	void JustDied(Unit *victim)
    {
		DoScriptText(YELL_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANETHERON, DONE);
    }

	void UpdateAI(const uint32 uiDiff)
    {
		if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

		if(m_uiBerserkTimer < uiDiff)
		{
			DoCast(m_creature, SPELL_BERSERK);
			m_uiBerserkTimer = 60000;
		}
		else m_uiBerserkTimer -= uiDiff;

		if(m_uiVampiricAuraTimer < uiDiff)
		{
			DoCast(m_creature, SPELL_VAMPIRIC_AURA);
			m_uiVampiricAuraTimer = 5000;
		}
		else m_uiVampiricAuraTimer -= uiDiff;

		if(m_uiCarrionSwarmTimer < uiDiff)
		{
			switch(urand(0, 1))
			{
            case 0: DoScriptText(YELL_CARRION_SWARM_1, m_creature); break;
            case 1: DoScriptText(YELL_CARRION_SWARM_2, m_creature); break;
			}
			if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				m_creature->CastSpell(pTarget, SPELL_CARRION_SWARM, false);
			m_uiCarrionSwarmTimer = urand(14000,15000);
		}
		else m_uiCarrionSwarmTimer -= uiDiff;

		if(m_uiSleepTimer < uiDiff)
		{
			switch(urand(0, 1))
			{
            case 0: DoScriptText(YELL_SLEEP_1, m_creature); break;
            case 1: DoScriptText(YELL_SLEEP_1, m_creature); break;
			}
			for (int i = 0;i<3;i++)
			{
				if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
					m_creature->CastSpell(pTarget, SPELL_SLEEP, false);
			}
			m_uiSleepTimer = urand(18000,19000);

		}
		else m_uiSleepTimer -= uiDiff;

		if(m_uiInfernoTimer < uiDiff)
		{
			switch(urand(0, 1))
			{
            case 0: DoScriptText(YELL_SUMMON_INFERNO_1, m_creature); break;
            case 1: DoScriptText(YELL_SUMMON_INFERNO_2, m_creature); break;
			}

			if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
			{
				m_creature->CastSpell(pTarget, SPELL_INFERNO, false);
				pTarget->CastSpell(pTarget, SPELL_STUN, false);
			}
			m_uiInfernoTimer = 60000;
		}
		else m_uiInfernoTimer -= uiDiff;

		DoMeleeAttackIfReady();
	}
};

/*######
## npc_towering_infernal
######*/
struct MANGOS_DLL_DECL npc_towering_infernalAI : public ScriptedAI
{
	npc_towering_infernalAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
	}

	ScriptedInstance* pInstance; 
	
	uint32 m_uiImmolationTimer;
	
   void Reset()
 	{	
		m_uiImmolationTimer = 1000;
	}
	
	void UpdateAI(const uint32 diff)
    {       	
	    if(m_uiImmolationTimer < diff)
        {
			DoCast(m_creature, SPELL_IMMOLATION_AURA);
            m_uiImmolationTimer = 2000;
        }
		else m_uiImmolationTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_towering_infernal(Creature* pCreature)
{
    return new npc_towering_infernalAI (pCreature);
}

CreatureAI* GetAI_boss_anetheron(Creature* pCreature)
{
    return new boss_anetheronAI(pCreature);
}

void AddSC_boss_anetheron()
{
    Script *pNewScript;

	pNewScript = new Script;
    pNewScript->Name = "npc_towering_infernal";
    pNewScript->GetAI = &GetAI_npc_towering_infernal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_anetheron";
    pNewScript->GetAI = &GetAI_boss_anetheron;
    pNewScript->RegisterSelf();
}