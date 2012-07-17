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
SDName: boss_azgalor
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
	SPELL_CLEAVE			= 31345,
	SPELL_RAIN_OF_FIRE		= 31340,
	SPELL_HOWL_OF_AZGALOR	= 31344,
	SPELL_DOOM				= 31347,
	SPELL_BERSERK			= 26662,
	SPELL_CRIPPLE			= 31406,
	SPELL_WAR_STOMP			= 31408,

	//Texts
	YELL_AGGRO				= -1999960,
	YELL_KILL_1				= -1999961,
	YELL_KILL_2				= -1999962,
	YELL_KILL_3				= -1999963,
	YELL_DOOM_1				= -1999964,
	YELL_DOOM_2				= -1999965,
	YELL_DEATH				= -1999966
};

/*######
## boss_azgalor
######*/

struct MANGOS_DLL_DECL boss_azgalorAI : public ScriptedAI
{
    boss_azgalorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
	ScriptedInstance* m_pInstance;

	uint32 m_uiCleaveTimer;
    uint32 m_uiRainOfFireTimer;
    uint32 m_uiHowlOfAzgalorTimer;
	uint32 m_uiDoomTimer;
    uint32 m_uiBerserkTimer;

	void Reset()
    {
		m_uiCleaveTimer = urand(6000,7000);
		m_uiRainOfFireTimer = urand(32000,33000);
		m_uiHowlOfAzgalorTimer = urand (19000,20000);
		m_uiDoomTimer = urand(44000,45000);
		m_uiBerserkTimer = 600000;
	}

	void JustReachedHome()
	{
		if(m_pInstance)
		{
			if(m_pInstance->GetData(TYPE_AZGALOR) != FAIL)
				m_pInstance->SetData(TYPE_AZGALOR, FAIL);
		}
	}

	void Aggro(Unit* pWho)
    {
		DoScriptText(YELL_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_AZGALOR, IN_PROGRESS);
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
            m_pInstance->SetData(TYPE_AZGALOR, DONE);
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

		if(m_uiHowlOfAzgalorTimer < uiDiff)
		{
			DoCast(m_creature, SPELL_HOWL_OF_AZGALOR);
			m_uiHowlOfAzgalorTimer = urand (19000,20000);
		}
		else m_uiHowlOfAzgalorTimer -= uiDiff;

		if(m_uiRainOfFireTimer < uiDiff)
		{
			if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				m_creature->CastSpell(pTarget, SPELL_RAIN_OF_FIRE, false);
			m_uiRainOfFireTimer = urand(32000,33000);
		}
		else m_uiRainOfFireTimer -= uiDiff;

		if(m_uiDoomTimer < uiDiff)
		{
			switch(urand(0, 1))
			{
            case 0: DoScriptText(YELL_DOOM_1, m_creature); break;
            case 1: DoScriptText(YELL_DOOM_2, m_creature); break;
			}
			if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				m_creature->CastSpell(pTarget, SPELL_DOOM, false);
			m_uiDoomTimer = urand(44000,45000);
		}
		else m_uiDoomTimer -= uiDiff;

		if(m_uiCleaveTimer < uiDiff)
		{
			Unit * pTarget = m_creature->getVictim();
			m_creature->CastSpell(pTarget, SPELL_CLEAVE, false);
			m_uiCleaveTimer = urand(6000,7000);
		}
		else m_uiCleaveTimer -= uiDiff;

		DoMeleeAttackIfReady();
	}
};

/*######
## npc_lesser_doomguard
######*/
struct MANGOS_DLL_DECL npc_lesser_doomguardAI : public ScriptedAI
{
	npc_lesser_doomguardAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		Reset();
	}

	ScriptedInstance* m_pInstance; 
	
	uint32 m_uiCrippleTimer;
	uint32 m_uiWarStompTimer;
	
   void Reset()
 	{	
		m_uiCrippleTimer	= 3000;
		m_uiWarStompTimer	= 7000;
	}
	
	void UpdateAI(const uint32 uiDiff)
    {       	
	    if(m_uiWarStompTimer < uiDiff)
        {
			DoCast(m_creature, SPELL_WAR_STOMP);
            m_uiWarStompTimer = urand(7000,8000);
        }
		else m_uiWarStompTimer -= uiDiff;

		if(m_uiCrippleTimer < uiDiff)
		{
			if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				m_creature->CastSpell(pTarget, SPELL_CRIPPLE, false);
			m_uiCrippleTimer = urand(3000,4000);
		}
		else m_uiCrippleTimer -= uiDiff;

		if (m_pInstance && (m_pInstance->GetData(TYPE_AZGALOR) == FAIL || m_pInstance->GetData(TYPE_AZGALOR) == DONE))
        {
            m_creature->ForcedDespawn();
            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_lesser_doomguard(Creature* pCreature)
{
    return new npc_lesser_doomguardAI (pCreature);
}

CreatureAI* GetAI_boss_azgalor(Creature* pCreature)
{
    return new boss_azgalorAI(pCreature);
}

void AddSC_boss_azgalor()
{
    Script *pNewScript;

	pNewScript = new Script;
    pNewScript->Name = "npc_lesser_doomguard";
    pNewScript->GetAI = &GetAI_npc_lesser_doomguard;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_azgalor";
    pNewScript->GetAI = &GetAI_boss_azgalor;
    pNewScript->RegisterSelf();
}