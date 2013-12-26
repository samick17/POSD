#include "ClipBoardStateFactory.h"
#include "ClipBoardStateID.h"
#include "NullClipBoardState.h"
#include "CutState.h"
#include "CopyState.h"

ClipBoardStateFactory::ClipBoardStateFactory(){
    this->newStateMap.put(ClipBoardStateID::NullClipBoardState,newState<NullClipBoardState>);
    this->newStateMap.put(ClipBoardStateID::CutState,newState<CutState>);
    this->newStateMap.put(ClipBoardStateID::CopyState,newState<CopyState>);
}

ClipBoardStateFactory::~ClipBoardStateFactory(){
}

ClipBoardState* ClipBoardStateFactory::createState(int clipBoardStateID,ERModel* erModel){
    NewClipBoardStateFunction newStateFunction = this->findNewStateFunction(clipBoardStateID);
    return newStateFunction(clipBoardStateID,erModel);
}

NewClipBoardStateFunction ClipBoardStateFactory::findNewStateFunction(int clipBoardStateID){
    return this->newStateMap.get(clipBoardStateID);
}