DELETE FROM spell_proc_event where entry IN (8494,8495,10191,10192,10193,27131);
INSERT INTO spell_proc_event (entry, SchoolMask, Category, SkillID, SpellFamilyName, SpellFamilyMask, procFlags, ppmRate, cooldown) VALUES
(8494, 0,0,0,0,0x0000000000000000,0x00100402,0,0),
(8495, 0,0,0,0,0x0000000000000000,0x00100402,0,0),
(10191,0,0,0,0,0x0000000000000000,0x00100402,0,0),
(10192,0,0,0,0,0x0000000000000000,0x00100402,0,0),
(10193,0,0,0,0,0x0000000000000000,0x00100402,0,0),
(27131,0,0,0,0,0x0000000000000000,0x00100402,0,0);
