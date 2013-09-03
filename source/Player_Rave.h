#pragma once

#include "Common.h"
#include "Player.h"
#include "AllPlayers.h"
#include "RaveSearch.h"
#include "RaveMemoryPool.hpp"

namespace SparCraft
{
class Player_Rave : public Player
{
    RaveSearchParameters     _params;
    RaveSearchResults        _prevResults;
public:
    Player_Rave (const IDType & playerID, const RaveSearchParameters & params);
	void getMoves(GameState & state, const MoveArray & moves, std::vector<UnitAction> & moveVec);
    IDType getType() { return PlayerModels::Rave; }
    RaveSearchParameters & getParams();
    RaveSearchResults & getResults();
};
}