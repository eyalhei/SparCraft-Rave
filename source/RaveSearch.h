#pragma once

#include <limits>

#include "Timer.h"
#include "GameState.h"
#include "UnitAction.hpp"
#include "GraphViz.hpp"
#include "Array.hpp"
#include "MoveArray.h"
#include "RaveSearchParameters.hpp"
#include "RaveSearchResults.hpp"
#include "Player.h"
#include "AllPlayers.h"
#include "RaveNode.h"
#include "GraphViz.hpp"
#include "RaveMemoryPool.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/multi_array.hpp>

namespace SparCraft
{

class Game;
class Player;

class RaveSearch
{
	RaveSearchParameters 	_params;
    RaveSearchResults        _results;
	Timer		            _searchTimer;
    RaveNode *                _rootNode;
    RaveMemoryPool *         _memoryPool;
	bool					_maxPlayer;

    GameState               _currentState;

	// we will use these as variables to save stack allocation every time
    std::vector<UnitAction>                 _actionVec;
	MoveArray                               _moveArray;
	Array<std::vector<UnitAction>,
		 Constants::Max_Ordered_Moves>      _orderedMoves;

    std::vector<PlayerPtr>					_allScripts[Constants::Num_Players];
    PlayerPtr                               _playerModels[Constants::Num_Players];

	double PlayoutToWins(StateEvalScore& playout);

	void DoRaveUpdate(double playout, RaveNode& node, std::vector<std::vector<UnitAction>>& followedMoves);
public:

	RaveSearch(const RaveSearchParameters & params);

    // Rave-specific functions
    RaveNode &       RaveNodeSelect(RaveNode & parent);
    StateEvalScore  traverse(RaveNode & node, GameState & currentState,std::vector<std::vector<UnitAction>>& followedMoves);
	void            Rave(GameState & state, size_t depth, const IDType lastPlayerToMove, std::vector<UnitAction> * firstSimMove);

	void            doSearch(GameState & initialState, std::vector<UnitAction> & move, IDType player);
    
    // Move and Child generation functions
    void            generateChildren(RaveNode & node, GameState & state);
	void            generateOrderedMoves(GameState & state, MoveArray & moves, const IDType & playerToMove);
    void            makeMove(RaveNode & node, GameState & state);
	const bool      getNextMove(IDType playerToMove, MoveArray & moves, const size_t & moveNumber, std::vector<UnitAction> & actionVec);

    // Utility functions
	const IDType    getPlayerToMove(RaveNode & node, const GameState & state) const;
    const size_t    getChildNodeType(RaveNode & parent, const GameState & prevState) const;
	const bool      searchTimeOut();
	const bool      isRoot(RaveNode & node) const;
	const bool      terminalState(GameState & state, const size_t & depth) const;
    const bool      isFirstSimMove(RaveNode & node, GameState & state);
    const bool      isSecondSimMove(RaveNode & node, GameState & state);
    StateEvalScore  performPlayout(GameState & state);
    void            updateState(RaveNode & node, GameState & state, bool isLeaf);
    void            setMemoryPool(RaveMemoryPool * pool);
    RaveSearchResults & getResults();

    // graph printing functions
    void            printSubTree(RaveNode & node, GameState state, std::string filename);
    void            printSubTreeGraphViz(RaveNode & node, GraphViz::Graph & g, GameState state);
    std::string     getNodeIDString(RaveNode & node);
};
}