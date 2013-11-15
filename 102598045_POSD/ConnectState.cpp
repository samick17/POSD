#include "ConnectState.h"
#include <iostream>

ConnectState::ConnectState(int stateID,GraphicalPresentation* graphicalPresentation) : State(stateID,graphicalPresentation){
}

ConnectState::~ConnectState(){
}

void ConnectState::onCreate(){
}

void ConnectState::doMousePressEvent(Point position){
    Component* component = this->graphicalPresentation->getLastPressedComponent();
    this->graphicalPresentation->setPreviewState(component);
    this->graphicalPresentation->setPreviewSourcePoint(position);
    this->graphicalPresentation->setPreviewTargetPoint(position);
}

void ConnectState::doMouseMoveEvent(Point position){
    this->graphicalPresentation->setPreviewTargetPoint(position);
}

void ConnectState::doMouseReleaseEvent(Point position){
	this->graphicalPresentation->setPreviewState(false);
	Component* sourceComponent = this->graphicalPresentation->getLastPressedComponent();
	Component* targetComponent = this->graphicalPresentation->getLastReleasedComponent();
	if(sourceComponent && targetComponent)
		this->graphicalPresentation->addConnection(sourceComponent,targetComponent);	
}