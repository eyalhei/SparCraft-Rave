#include "Player_Rave.h"

using namespace SparCraft;

Player_Rave::Player_Rave (const IDType & playerID, const RaveSearchParameters & params) 
{
	_playerID = playerID;
    _params = params;
}

void Player_Rave::getMoves(GameState & state, const MoveArray & moves, std::vector<UnitAction> & moveVec)
{
    moveVec.clear();
    
    RaveSearch rave(_params);

    rave.doSearch(state, moveVec, _playerID);
    _prevResults = rave.getResults();
}

RaveSearchParameters & Player_Rave::getParams()
{
    return _params;
}

RaveSearchResults & Player_Rave::getResults()
{
    return _prevResults;
}