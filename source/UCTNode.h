#pragma once

#include "Common.h"
#include "UnitAction.hpp"

namespace SparCraft
{


class UCTNode
{
    // uct stat counting variables
    size_t                      _numVisits;         // total visits to this node
    double                      _numWins;           // wins from this node
    double                      _uctVal;            // previous computed UCT value
            
    // game specific variables
    size_t                      _player;            // the player who made a move to generate this node
    IDType                      _nodeType;
    std::vector<UnitAction>     _move;              // the ove that generated this node

    // holds children
    std::vector<UCTNode>        _children;

    // nodes for traversing the tree
    UCTNode *                   _parent;
    
	RandomInt			_rand;

public:

    UCTNode ()
        : _numVisits            (0)
        , _numWins              (0)
        , _uctVal               (0)
        , _player               (Players::Player_None)
        , _nodeType             (SearchNodeType::Default)
        , _parent               (NULL)
		, _rand					(0,0,0)
    {

    }

    UCTNode (UCTNode * parent, const IDType player, const IDType nodeType, const std::vector<UnitAction> & move, const size_t & maxChildren, std::vector<UCTNode> * fromPool = NULL)
        : _numVisits            (0)
        , _numWins              (0)
        , _uctVal               (0)
        , _player               (player)
        , _nodeType             (nodeType)
        , _move                 (move)
        , _parent               (parent)
		, _rand					(0,0,0)
    {
        _children.reserve(maxChildren);
    }

    const size_t    numVisits()                 const           { return _numVisits; }
    const double    numWins()                   const           { return _numWins; }
    const size_t    numChildren()               const           { return _children.size(); }
    const double    getUCTVal()                 const           { return _uctVal; }
    const bool      hasChildren()               const           { return numChildren() > 0; }
    const size_t    getNodeType()               const           { return _nodeType; }
    const IDType    getPlayer()                 const           { return _player; }

    UCTNode *       getParent()                 const           { return _parent; }
    UCTNode &       getChild(const size_t & c)                  { return _children[c]; }

    void            setUCTVal(double val)                       { _uctVal = val; }
    void            incVisits()                                 { _numVisits++; }
    void            addWins(double val)                         { _numWins += val; }

    std::vector<UCTNode> & getChildren()                        { return _children; }

    const std::vector<UnitAction> & getMove() const
    {
        return _move;
    }

    void setMove(const std::vector<UnitAction> & move)
    {
        _move = move;
    }

    void addChild(UCTNode * parent, const IDType player, const IDType nodeType, const std::vector<UnitAction> & move, const size_t & maxChildren, std::vector<UCTNode> * fromPool = NULL)
    {
        _children.push_back(UCTNode(parent, player, nodeType, move, maxChildren));
    }

	UCTNode * getRandomChild(){
		_rand.reset(0,numChildren(),0);
		return &getChild(_rand.nextInt());
	}

    UCTNode & mostVisitedChild() 
    {
        UCTNode * mostVisitedChild = NULL;
        size_t mostVisits = 0;

       for (size_t c(0); c < numChildren(); ++c)
       {
           UCTNode & child = getChild(c);

           if (!mostVisitedChild || (child.numVisits() > mostVisits))
           {
               mostVisitedChild = &child;
               mostVisits = child.numVisits();
           }
       }
	   if (mostVisitedChild == NULL){
		   //std::cout<< "UCT - DIE PROUD" <<std::endl;
		   mostVisitedChild = getRandomChild();
	   }
       return *mostVisitedChild;
    }

	// numWins/numGraded
	UCTNode & bestAvgValueChild(const bool maxPlayer) 
    {
        UCTNode * bestChild = NULL;
		const double valueOfUnkown = maxPlayer ? -1 : 2; // if score unknown give really bad value (normal value from 0 to 1)
        double bestVal = valueOfUnkown; 
		size_t nChild = numChildren();
        for (size_t c(0); c < nChild; ++c)
        {
            UCTNode & child = getChild(c);

			double currentVal  = child.numVisits()>0 ? child.numWins() / child.numVisits() : valueOfUnkown;
            
            if (maxPlayer)
            {
                if (currentVal > bestVal)
                {
                    bestVal             = currentVal;
			        bestChild           = &child;
                }
            }
            else if (currentVal < bestVal)
            {
                bestVal             = currentVal;
		        bestChild           = &child;
            }
            
        }
		if (bestChild == NULL){
			//std::cout<< "DIE PROUD" <<std::endl;
			bestChild = getRandomChild();
		}
        return *bestChild;
    }

	UCTNode & bestComboValueChild(const bool maxPlayer)
	{
		UCTNode * bestChild = NULL;
        double bestVal = -1; 
		size_t nChild = numChildren();
        for (size_t c(0); c < nChild; ++c)
        {
            UCTNode & child = getChild(c);
			double currentVal   =-1;
			if (child.numVisits() > 0)
			{
				double winRate      = (double)child.numWins() / (double)child.numVisits();
				if (!maxPlayer) winRate = 1 - winRate;
				double uctVal       = sqrt( log( (double)numVisits() ) / ( child.numVisits() ) );
				currentVal = (winRate / uctVal);
			}

            if (currentVal > bestVal)
            {
                bestVal             = currentVal;
			    bestChild           = &child;
            }
        }
		if (bestChild == NULL){
			//std::cout<< "DIE PROUD" <<std::endl;
			bestChild = getRandomChild();
		}
        return *bestChild;
	}

	/*
    UCTNode & bestUCTValueChild(const bool maxPlayer, const UCTSearchParameters & params) 
    {
        UCTNode * bestChild = NULL;
        double bestVal = maxPlayer ? std::numeric_limits<double>::min() : std::numeric_limits<double>::max();

        for (size_t c(0); c < numChildren(); ++c)
        {
            UCTNode & child = getChild(c);
       
            double winRate      = (double)child.numWins() / (double)child.numVisits();
            double uctVal       = params.cValue() * sqrt( log( (double)numVisits() ) / ( child.numVisits() ) );
			double currentVal   = maxPlayer ? (winRate + uctVal) : (winRate - uctVal);

            if (maxPlayer)
            {
                if (currentVal > bestVal)
                {
                    bestVal             = currentVal;
			        bestChild           = &child;
                }
            }
            else if (currentVal < bestVal)
            {
                bestVal             = currentVal;
		        bestChild           = &child;
            }
        }

        return *bestChild;
    }
	*/
};
}