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
SDName: boss_kazrogal
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
	SPELL_CLEAVE			= 31436,
	SPELL_WAR_STOMP			= 31480,
	SPELL_CRIPPLE			= 31477,
	SPELL_MARK_OF_KAZROGAL	= 31447,

	//Texts
	YELL_AGGRO				= -1999951,
	YELL_MARK_1				= -1999952,
	YELL_MARK_2				= -1999953,
	YELL_KILL_1				= -1999954,
	YELL_KILL_2				= -1999955,
	YELL_KILL_3				= -1999956,
	YELL_DEATH				= -1999957
};

struct MANGOS_DLL_DECL boss_kazrogalAI : public ScriptedAI
{
    boss_kazrogalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
	ScriptedInstance* m_pInstance;

	uint32 m_uiCleaveTimer;
    uint32 m_uiWarStompTimer;
    uint32 m_uiCrippleTimer;
	uint32 m_uiMarkOfKazrogalTimer;
	uint32 m_uiMarkOfKazrogalCountTimer;

	void Reset()
    {
		m_uiCleaveTimer = urand(7000,9000);
		m_uiWarStompTimer = urand(14000,17000);
		m_uiCrippleTimer = urand(6000,8000);
		m_uiMarkOfKazrogalTimer = 45000;
		m_uiMarkOfKazrogalCountTimer = 45000;
	}

	void JustReachedHome()
	{
		if(m_pInstance)
		{
			if(m_pInstance->GetData(TYPE_KAZROGAL) != FAIL)
				m_pInstance->SetData(TYPE_KAZROGAL, FAIL);
		}
	}

	void Aggro(Unit* pWho)
    {
		DoScriptText(YELL_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KAZROGAL, IN_PROGRESS);
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
            m_pInstance->SetData(TYPE_KAZROGAL, DONE);
    }

	void UpdateAI(const uint32 uiDiff)
    {
		if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

		if(m_uiCleaveTimer < uiDiff)
		{
			Unit * pTarget = m_creature->getVictim();
			m_creature->CastSpell(pTarget, SPELL_CLEAVE, false);
			m_uiCleaveTimer = urand(7000,9000);
		}
		else m_uiCleaveTimer -= uiDiff;

		if(m_uiWarStompTimer < uiDiff)
		{
			DoCast(m_creature, SPELL_WAR_STOMP);
			m_uiWarStompTimer = urand(16000,18000);
		}
		else m_uiWarStompTimer -= uiDiff;

		if(m_uiCrippleTimer < uiDiff)
		{
            if(!m_creature->IsWithinDistInMap(m_creature->getVictim(), 15.0f))
               m_creature->CastSpell(m_creature->getVictim(), SPELL_CRIPPLE, false);
            else
                if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
		            m_creature->CastSpell(pTarget, SPELL_CRIPPLE, false);
   	            m_uiCrippleTimer = urand(6000,8000);
        }
		else m_uiCrippleTimer -= uiDiff;

		if(m_uiMarkOfKazrogalTimer < uiDiff)
		{
			
			switch(urand(0, 1))
			{
				case 0: DoScriptText(YELL_MARK_1, m_creature); break;
				case 1: DoScriptText(YELL_MARK_2, m_creature); break;
			}

			DoCast(m_creature, SPELL_MARK_OF_KAZROGAL);

			if (m_uiMarkOfKazrogalCountTimer >= 15000)
				m_uiMarkOfKazrogalCountTimer -= 5000;
			else
				m_uiMarkOfKazrogalCountTimer = 10000;
			m_uiMarkOfKazrogalTimer = m_uiMarkOfKazrogalCountTimer;
		}
		else m_uiMarkOfKazrogalTimer -= uiDiff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_kazrogal(Creature* pCreature)
{
    return new boss_kazrogalAI(pCreature);
}

void AddSC_boss_kazrogal()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_kazrogal";
    pNewScript->GetAI = &GetAI_boss_kazrogal;
    pNewScript->RegisterSelf();
}