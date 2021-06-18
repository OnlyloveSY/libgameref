#pragma once
#include "../../table_ref.h"


TABLE_STRUCT_DEF_4(CharacterQualityFragment,"./test/CharacterQualityFragment.tab",'\t',
	int, Quality,		"Quality,key",
	int, DecomposeCount,"DecomposeCount",
	int, ComposeCount,	"ComposeCount",
	std::vector<int>, StarUseCount, "StarUseCount\\[[0-9]\\],pkg"
	)


void regex_test() {
	auto* p = CharacterQualityFragment::GetTablePtr();

	std::vector<CharacterQualityFragment>ret1;
	if (p && p->Unmarshal(ret1, [](CharacterQualityFragment& r) {
		
		for (auto Iter : r.StarUseCount) {
			std::cout << Iter;
		}
		std::cout << std::endl;
		return true;
		})) {
		std::cout << "ok" << std::endl;
	}
}