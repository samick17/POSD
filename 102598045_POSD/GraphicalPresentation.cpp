#include "GraphicalPresentation.h"
#include "Connector.h"
#include "Node.h"
#include "ControllerEvent.h"
#include "ERModelUtil.h"
#include "StateID.h"
#include "StateSubject.h"

GraphicalPresentation::GraphicalPresentation(Presentation* presentation) : presentation(presentation){
    this->isCtrlPressed = false;
    this->stateSubject = new StateSubject();
}

GraphicalPresentation::~GraphicalPresentation(){
    delete this->stateSubject;
}

StateSubject* GraphicalPresentation::getStateSubject(){
    return this->stateSubject;
}

HashMap<string,Component*> GraphicalPresentation::getAllComponents(){
    HashMap<string,Component*> componentMap = this->presentation->getAllComponents();
    //extract connector & insert to front
    HashMap<string,Connector*> connectorMap = ERModelUtil::convertComponentHashMapToTypeHashMap<Connector>(componentMap);
    for each(Connector* connector in connectorMap)
        componentMap.remove(connector->getID());
    for each(Connector* connector in connectorMap)
        componentMap.insertAt(connector->getID(),connector,0);
    return componentMap;
}

Node* GraphicalPresentation::getLastAddedNode(){
    return this->lastAddedNode;
}

Component* GraphicalPresentation::getLastPressedComponent(){
    return this->lastPressedComponent;
}

Component* GraphicalPresentation::getLastReleasedComponent(){
    return this->lastReleasedComponent;
}

void GraphicalPresentation::addNode(string nodeType,string nodeName,Point position){
    Node* node = this->presentation->addNode(nodeType);
    this->lastAddedNode = node;
    node->setName(nodeName);
    node->setSize(Size());
    node->setCenterPosition(position);
    this->presentation->sync(ControllerEvent::DisplayDiagram);
}

void GraphicalPresentation::addConnection(Component* sourceComponent,Component* targetComponent){
    try{
        this->presentation->addConnection(sourceComponent,targetComponent);
        this->presentation->sync(ControllerEvent::ConnectTwoNodes);
    }
    catch(Exception&){
    }
}

void GraphicalPresentation::openFile(string filePath){
    this->presentation->openFile(filePath);
    this->presentation->sync(ControllerEvent::DisplayDiagram);
}

void GraphicalPresentation::saveFile(string filePath){
    this->presentation->saveFile(filePath);
}
//close & exit application
void GraphicalPresentation::close(){
    this->presentation->close();
}
//is widget being selected?
bool GraphicalPresentation::isWidgetSelected(string componentID){
    if(this->selectedWidgetSet.find(componentID) != this->selectedWidgetSet.end())
        return true;
    return false;
}
//select widget : highlight/unhilight selected,notify all
void GraphicalPresentation::selectWidget(){
    if(!this->lastPressedComponent){
        this->unSelectAll();
        return;
    }
    string lastPressedComponentID = this->lastPressedComponent->getID();
    bool isSelected = this->isWidgetSelected(lastPressedComponentID);
    if(!this->isCtrlPressed)
        this->selectedWidgetSet.clear();
    this->revertSelectWidget(isSelected,lastPressedComponentID);
}

void GraphicalPresentation::selectLastPressedWidget(){
    if(!this->lastPressedComponent)
        return;
    string lastPressedComponentID = this->lastPressedComponent->getID();
    if(this->selectedWidgetSet.find(lastPressedComponentID) == this->selectedWidgetSet.end())
        this->selectedWidgetSet.insert(lastPressedComponentID);
}

void GraphicalPresentation::revertSelectWidget(bool isSelected,string componentID){
    if(isSelected)
        this->selectedWidgetSet.erase(componentID);
    else 
        this->selectedWidgetSet.insert(componentID);
}

void GraphicalPresentation::moveSelectedWidget(Point deltaPosition){
    HashMap<string,Component*> componentMap = this->presentation->getAllComponents();
    for each(string componentID in this->selectedWidgetSet){
        Component* component = componentMap.get(componentID);
        Point position = component->getRect().getPosition();
        component->setPosition(position+deltaPosition);
    }
}

void GraphicalPresentation::unSelectAll(){
    this->selectedWidgetSet.clear();
}

void GraphicalPresentation::switchState(int stateID){
    this->stateSubject->switchState(stateID,this);
}
//key pressed
void GraphicalPresentation::keyCtrlPressed(){
    this->isCtrlPressed = true;
}

void GraphicalPresentation::keyCtrlReleased(){
    this->isCtrlPressed = false;
}
//synchronized view
void GraphicalPresentation::registerSynchronizer(ISynchronizer* synchronizer){
    this->presentation->registerSynchronizer(synchronizer);
}

void GraphicalPresentation::unregisterSynchronizer(ISynchronizer* synchronizer){
    this->presentation->unregisterSynchronizer(synchronizer);
}

void GraphicalPresentation::sync(string syncEventType){
    this->presentation->sync(syncEventType);
}

void GraphicalPresentation::doRegisterObserver(IObserver* observer){
    this->presentation->registerObserver(observer);
}

void GraphicalPresentation::doUngisterObserver(IObserver* observer){
    this->presentation->unregisterObserver(observer);
}

void GraphicalPresentation::mousePressEvent(Point position,Component* component){
    this->lastPressedComponent = component;
    this->stateSubject->getState()->mousePressEvent(position);
    //must use notify to avoid some bug
    this->notify();
}

void GraphicalPresentation::mouseMoveEvent(Point position,Component* component){
    this->stateSubject->getState()->mouseMoveEvent(position);
}

void GraphicalPresentation::mouseReleaseEvent(Point position,Component* component){
    this->lastReleasedComponent = component;
    this->stateSubject->getState()->mouseReleaseEvent(position);
    this->notify();
}