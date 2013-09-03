#pragma once

#include "Common.h"
#include <boost/unordered_map.hpp>
#include "UnitAction.hpp"

namespace SparCraft
{
class RaveNode;
typedef	boost::shared_ptr<RaveNode> RaveNodePtr; 

struct UnitActionComp {
	bool operator() (const UnitAction  & a,const  UnitAction & b) const{
		if (a._moveType != b._moveType) return false;
		if (a._moveType == UnitActionTypes::ATTACK || a._moveType == UnitActionTypes::MOVE)
			return (a._moveIndex == b._moveIndex);
		return true;
	}
};

struct UnitActionHasher {
	size_t operator() (const UnitAction  & a) const {
		return a._unit*1000 + a._moveType*100+ a._moveIndex;
	}
};

class RaveNode
{
	// Rave stat counting variables
    long						_numVisits;         // total visits to this node
    double                      _numWins;           // wins from this node
	double						_numGraded;         // total Grades given to this node
    double                      _RaveVal;            // previous computed Rave value
            
    // game specific variables
    size_t                      _player;            // the player who made a move to generate this node
    IDType                      _nodeType;
    std::vector<UnitAction>     _move;              // the ove that generated this node

    // holds children
    std::vector<RaveNodePtr>        _children;

    // nodes for traversing the tree
    RaveNode *                  _parent;
    
	// Die Proud Map
	boost::unordered_map<UnitAction,std::vector<RaveNodePtr>,UnitActionHasher,UnitActionComp> _childrenByUnitActionMap;

	RandomInt			_rand;
public:
    
	RaveNode ()
        : _numVisits            (0)
        , _numWins              (0)
		, _numGraded			(0)
        , _RaveVal              (0)
        , _player               (Players::Player_None)
        , _nodeType             (SearchNodeType::Default)
        , _parent               (NULL)
		, _childrenByUnitActionMap (UnitActionTypes::UnitActionTypesCount)
		, _rand					(0,0,0)
    {

    }

    RaveNode (RaveNode * parent, const IDType player, const IDType nodeType, const std::vector<UnitAction> & move, const size_t & maxChildren, std::vector<RaveNode> * fromPool = NULL)
        : _numVisits            (0)
        , _numWins              (0)
		, _numGraded			(0)
        , _RaveVal              (0)
        , _player               (player)
        , _nodeType             (nodeType)
        , _move                 (move)
        , _parent               (parent)
		, _childrenByUnitActionMap (UnitActionTypes::UnitActionTypesCount)
		, _rand					(0,0,0)
    {
        _children.reserve(maxChildren);
    }

	~RaveNode(){
		//_childrenByUnitActionMap.clear();
		//_children.clear();
	}

    const long		numVisits()                 const           { return _numVisits; }
	const double	numGraded()					const			{ return _numGraded; }
    const double    numWins()                   const           { return _numWins; }
    const size_t    numChildren()               const           { return _children.size(); }
    const double    getRaveVal()                 const           { return _RaveVal; }
    const bool      hasChildren()               const           { return numChildren() > 0; }
    const size_t    getNodeType()               const           { return _nodeType; }
    const IDType    getPlayer()                 const           { return _player; }
	boost::unordered_map<UnitAction,std::vector<RaveNodePtr>,UnitActionHasher,UnitActionComp>& getChildrenByActionMap() {return _childrenByUnitActionMap;}

    RaveNode*       getParent()                 const           { return _parent; }
    RaveNode &       getChild(const size_t & c)                  { return (*_children[c]); }

    void            setRaveVal(double val)                       { _RaveVal = val; }
    void            incVisits()                                 { _numVisits+= 1; }
	void            addGraded(double val)                         { _numGraded += val; }
    void            addWins(double val)                         { _numWins += val; }

    std::vector<RaveNodePtr> & getChildren()                        { return _children; }

    const std::vector<UnitAction> & getMove() const
    {		
        return _move;
    }

    void setMove(const std::vector<UnitAction> & move)
    {
        _move = move;
    }

    void addChild(RaveNode * parent, const IDType player, const IDType nodeType, std::vector<UnitAction> move, const size_t & maxChildren, std::vector<RaveNode> * fromPool = NULL)
    {
		int bbb = _children.size();
		_children.push_back(RaveNodePtr(new RaveNode(parent, player, nodeType, move, maxChildren)));
        
		std::vector<UnitAction>::iterator moveIter = move.begin();
		for(;moveIter != move.end(); moveIter++)
		{
			if (_childrenByUnitActionMap.count(*moveIter) == 0)
			{
				std::vector<RaveNodePtr> emptyVec;
				_childrenByUnitActionMap.insert(std::pair<UnitAction,std::vector<RaveNodePtr>>(*moveIter,emptyVec));
			}
			_childrenByUnitActionMap[*moveIter].push_back(_children[bbb]);
		}
		
    }

	RaveNode * getRandomChild(){
		_rand.reset(0,numChildren(),0);
		return &getChild(_rand.nextInt());
	}

    RaveNode & mostVisitedChild() 
    {
        RaveNode * mostVisitedChild = NULL;
        double mostVisits = 0;

       for (size_t c(0); c < numChildren(); ++c)
       {
           RaveNode & child = getChild(c);

           if (!mostVisitedChild || (child.numVisits() > mostVisits))
           {
               mostVisitedChild = &child;
               mostVisits = child.numVisits();
           }
       }
	   if (mostVisitedChild == NULL){
		   //std::cout<< "Rave - DIE PROUD " <<numChildren()<<std::endl;
		   mostVisitedChild = getRandomChild();
	   }
       return *mostVisitedChild;
    }

	// numWins/numGraded
    RaveNode & bestRaveValueChild() 
    {
        RaveNode * bestChild = NULL;
        double bestVal = -2;
		size_t nChild = numChildren();
        for (size_t c(0); c < nChild; ++c)
        {
            RaveNode & child = getChild(c);

			double currentVal  = child.numGraded()>0 ? child.numWins() / child.numGraded() : -1;
            
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

	RaveNode & bestComboValueChild()
	{
		RaveNode * bestChild = NULL;
        double bestVal = -2;
		size_t nChild = numChildren();
        for (size_t c(0); c < nChild; ++c)
        {
            RaveNode & child = getChild(c);
			double currentVal   =0;
			if (child.numGraded() > 0)
			{
				double winRate      = (double)child.numWins() / (double)child.numGraded();
				double uctVal       = sqrt( log( (double)numVisits() ) / ( child.numVisits() ) );
				currentVal = (winRate / uctVal);
			}

            if (currentVal > bestVal)
            {
                bestVal             = currentVal;
			    bestChild           = &child;
            }
            
        }
		if (bestChild == NULL) {
			//std::cout<< "DIE PROUD" <<std::endl;
			bestChild = getRandomChild();
		}
		return *bestChild;
	}

};

}