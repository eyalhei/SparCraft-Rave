#include "EnumData.h"
#include "RaveSearchParameters.hpp"
#include "UCTSearchParameters.hpp"

using namespace SparCraft;

void SparCraft::EnumDataInit()
{
    Players::init();
    SearchMethods::init();
    PlayerModels::init();
    EvaluationMethods::init();
    SearchNodeType::init();
    MoveOrderMethod::init();
    PlayerToMove::init();
	UCTMoveSelect::init();
}