#pragma once
#include "../../table_ref.h"


TABLE_STRUCT_DEF_2(TeamType,"./test/TeamType.tab",'\t',
	int, TeamId,"TeamId",
	int, TypeId,"TypeId,key"
	)

void lambda_test() {
	auto* pdata = TeamType::GetTablePtr();

	std::vector<TeamType> ret1;
	if (pdata && pdata->Unmarshal(ret1, [](TeamType& result) {

		std::cout <<" result.TeamId: "<< result.TeamId << std::endl;
		std::cout <<" result.TypeId: "<< result.TypeId << std::endl;
		return true;
	}))
	{
		std::cout << "ok" << std::endl;
	}

	std::map<int,TeamType> ret_map;
	if (pdata && pdata->Unmarshal(ret_map, [](TeamType& result) {

		std::cout << " result.TeamId: " << result.TeamId << std::endl;
		std::cout << " result.TypeId: " << result.TypeId << std::endl;
		//key not 0
		assert(result.TypeId);
		return true;
		}))
	{
		std::cout << "ok" << std::endl;
	}
}