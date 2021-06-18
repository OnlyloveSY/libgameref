#pragma once
#include "../../table_ref.h"
#include "../common.h"


TABLE_STRUCT_DEF_39(Monster,"./test/NPC.txt",'\t',
	int, id,					"id,key",
	int, Type,					"Type",
	int, position_type,			"position_type",
	int, Hero_Professional,		"Hero_Professional",
	int, fight_index,			"fight_index",
	int, hero_camp,				"hero_camp",
	int, hero_gender,			"hero_gender",
	int, AtkSpeed,				"AtkSpeed",
	int, Level,					"Level",
	int, Fighting,				"Fighting",
	int, AtkSkillID,			"AtkSkillID",
	std::string, SubSkillID1,	"SubSkillID1",
	int, Mercenary_id,			"Mercenary_id",
	int, DamageType,			"DamageType",
	int, AtkValue,				"AtkValue",
	int, Defend,				"Defend",
	int, SpeDefend,				"SpeDefend",
	int, MaxHP,					"MaxHP",
	int, Luck,					"Luck",
	int, LuckAnt,				"LuckAnt",
	int, RateHit,				"RateHit",
	int, RateDodge,				"RateDodge",
	int, LuckHurt,				"LuckHurt",
	int, LuckSub,				"LuckSub",
	int, DamageAdd,				"DamageAdd",
	int, DamageSub,				"DamageSub",
	int, RageRequired,			"RageRequired",
	int, RageStart,				"RageStart",
	int, AtkRage,				"AtkRage",
	int, DefRage,				"DefRage",
	int, TimeRage,				"TimeRage",
	int, MoveSpeed,				"MoveSpeed",
	int, heroid,				"heroid",
	int, LifeState,				"LifeState",
	int, LifeAI,				"LifeAI",
	int, Radius,				"Radius",
	int, WaitFrame,				"WaitFrame",
	int, Surround,				"Surround",
	int, buff_id,				"buff_id"
)


//16605 * 39
void efficiency_test() {
	int64_t t1 = getTime();

	auto* p = Monster::GetTablePtr();
	std::vector<Monster> ret1;
	if (p != nullptr) {
		p->Unmarshal(ret1);
	}
	int64_t t2 = getTime();
	std::cout << "cost time: " << t2 - t1 << " get data size: " << ret1.size() << std::endl;
}