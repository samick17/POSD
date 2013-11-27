#include "ERModel.h"
#include "ERModelUtil.h"
#include "TableUtil.h"
#include "ComponentFactory.h"
#include "CommandFactory.h"
#include "CommandManager.h"
#include "NoSuchNodeException.h"
#include "InvalidConnectException.h"
#include "NoConnectionException.h"
#include "InvalidNodeTypeException.h"
#include "InputFileParser.h"
#include "OutputFileParser.h"

int ERModel::attributeCount = 0;
int ERModel::entityCount = 0;
int ERModel::relationShipCount = 0;

ERModel::ERModel(){
    this->initialCountMap();
    this->initialCardinality();
}

ERModel::~ERModel(){
    this->resetERModel();
}

Node* ERModel::addNode(string componentType){
    if(componentType == ComponentType::TypeConnector)
        throw InvalidNodeTypeException();
    ComponentFactory componentFactory;
    Node* node = static_cast<Node*>(componentFactory.createComponent(componentType));
    CommandFactory commandFactory;
    Command* addNodeCommand = commandFactory.createAddNodeCommand(this,node);
    this->commandManager.execute(addNodeCommand);
    this->setNodePosition(componentType,node);
    return node;
}
//insert component in componentMap, if no such key
void ERModel::insertComponent(Component* component){
    string componentID = component->getID();
    if(!this->componentMap.containsKey(component->getID()))
        this->componentMap.put(component->getID(),component);
}
//insert component at index argument
void ERModel::insertComponentAt(Component* component,unsigned int index){
    string componentID = component->getID();
    if(!this->componentMap.containsKey(component->getID()))
        this->componentMap.insertAt(component->getID(),component,index);
}
//erase component in componentMap, if contains key
void ERModel::eraseComponent(Component* component){
    this->componentMap.remove(component->getID());
}

void ERModel::deleteComponent(string componentID){
    Component* componentToDelete = this->getComponentByID(componentID);
    CommandFactory commandFactory;
    Command* deleteComponentCommand = commandFactory.createDeleteComponentCommand(this,componentToDelete);
    this->commandManager.execute(deleteComponentCommand);
}
//return: NodeConnectionType
int ERModel::addConnection(Component* firstNode,Component* secondNode){
    if(!firstNode | !secondNode)
        throw InvalidConnectException(StringSymbol::Empty,StringSymbol::Empty);
    int result = firstNode->canConnectTo(secondNode);
    if(result == NodeConnectionType::ValidConnect || result == NodeConnectionType::ConnectEntityAndRelation){
        ComponentFactory componentFactory;
        Connector* connector = static_cast<Connector*>(componentFactory.createComponent(ComponentType::TypeConnector));
        CommandFactory commandFactory;
        Command* connectNodeCommand = commandFactory.createConnectNodeCommand(this,firstNode,secondNode,connector);
        this->commandManager.execute(connectNodeCommand);
        connector->updateRect();
    }
    return result;
}

void ERModel::undo(){
    this->commandManager.undo();
}

void ERModel::redo(){
    this->commandManager.redo();
}
//set cardinality without undo/redo
void ERModel::setCardinality(Connector* connector,string cardinality){
    if(this->cardinality.hasCardinality(cardinality))
        connector->setName(cardinality);
}
//set primarykey by its 'id',Exception Case : NoSuchNode,NoConnection
void ERModel::setPrimaryKey(string componentID){
    //make sure there has such component
    Attribute* attribute = dynamic_cast<Attribute*>(this->getComponentByID(componentID));
    //find entity & set its primary key
    HashMap<string,Entity*> entityMap = this->getAllEntities();
    Entity* connectedEntity = NULL;
    for each(Entity* entity in entityMap){
        if(entity->getConnectedAttributes().containsKey(componentID))
            connectedEntity = entity;
    }
    if(!connectedEntity)
        throw NoConnectionException(componentID);

    CommandFactory commandFactory;
    Command* setPrimaryKeyCommand = commandFactory.createSetPrimaryKeyCommand(attribute);
    this->commandManager.execute(setPrimaryKeyCommand);
}
//set component text & cardinality with undo/redo,Exception Case : NoSuchNode
void ERModel::setComponentText(string componentID,string text){
    Component* component = this->getComponentByID(componentID);
    //if is connector,determine this operation is valid
    Connector* connector = dynamic_cast<Connector*>(component);
    if(connector && (!connector->isCardinalityConnector() || !this->cardinality.hasCardinality(text)))
        return;

    CommandFactory commandFactory;
    Command* editTextOfComponentsCommand = commandFactory.createEditTextOfComponentsCommand(component,text);
    this->commandManager.execute(editTextOfComponentsCommand);
}

void ERModel::openFile(string filePath){
    InputFileParser inputFileParser;
    inputFileParser.parseFileToModel(filePath,this);
    //open file should reset undo/redo state
    this->commandManager.popAllStack();
}

void ERModel::saveFile(string filePath){
    OutputFileParser outputFileParser = OutputFileParser(this->componentMap);
    outputFileParser.parseModelToFile(filePath);
}
//if doesn't contains such component, throw exception
Component* ERModel::getComponentByID(string id){
    if(this->componentMap.containsKey(id))
        return this->componentMap.get(id);
    throw NoSuchNodeException(id);
}
//return: firstNode & secondNode's connector
Connector* ERModel::getNodesConnector(Component* firstNode,Component* secondNode){
    Connector* connection = NULL;
    if(this->componentMap.containsKey(firstNode->getID()) && 
        this->componentMap.containsKey(secondNode->getID())){
        HashMap<string,Connector*> connections = this->getAllConnectors();
        for each(Connector* connector in connections){
            if(connector->isNodesConnection(firstNode,secondNode)){
                connection = connector;
                break;
            }
        }
    }
    return connection;
}

vector<string> ERModel::getCardinalityVector(){
    return this->cardinality.getCardinalityVector();
}

HashMap<string,Component*> ERModel::getAllComponents(){
    return this->componentMap;
}

HashMap<string,Attribute*> ERModel::getAllAttributes(){
    HashMap<string,Attribute*> attributeMap = ERModelUtil::convertComponentHashMapToTypeHashMap<Attribute>(this->componentMap);
    return attributeMap;
}

HashMap<string,Connector*> ERModel::getAllConnectors(){
    HashMap<string,Connector*> connectorMap = ERModelUtil::convertComponentHashMapToTypeHashMap<Connector>(this->componentMap);
    return connectorMap;
}

HashMap<string,Entity*> ERModel::getAllEntities(){
    HashMap<string,Entity*> entityMap = ERModelUtil::convertComponentHashMapToTypeHashMap<Entity>(this->getAllComponents());
    return entityMap;
}

HashMap<string,RelationShip*> ERModel::getAllRelationShips(){
    HashMap<string,RelationShip*> relationShipMap = ERModelUtil::convertComponentHashMapToTypeHashMap<RelationShip>(this->getAllComponents());
    return relationShipMap;
}
//get All Tables
HashMap<string,Table*> ERModel::getAllTables(){
    return TableUtil::convertToTableMap(this->tableManager,this->getAllEntities(),this->getAllRelationShips());
}
//clear all components & delete it
void ERModel::resetERModel(){
    ComponentFactory componentFactory;
    componentFactory.resetFactory();
    this->commandManager.popAllStack();
    this->resetCounting();

    for each(Component* component in this->componentMap)
        delete component;

    this->componentMap.clear();
}

void ERModel::registerSynchronizer(ISynchronizer* synchronizer){
    synchronizerVector.push_back(synchronizer);
}

void ERModel::unregisterSynchronizer(ISynchronizer* synchronizer){
    auto synchronizerIterator = find(synchronizerVector.begin(),synchronizerVector.end(),synchronizer);
    synchronizerVector.erase(synchronizerIterator);
}

void ERModel::sync(string syncEventType){
    for each(ISynchronizer* synchronizer in this->synchronizerVector)
        synchronizer->sync(syncEventType);
}

void ERModel::initialCountMap(){
    this->componentTypeMapOffsetX.insert(pair<string,double>(ComponentType::TypeAttribute,WidgetDefaultSetting::AttributeOffsetX));
    this->componentTypeMapOffsetX.insert(pair<string,double>(ComponentType::TypeEntity,WidgetDefaultSetting::EntityOffsetX));
    this->componentTypeMapOffsetX.insert(pair<string,double>(ComponentType::TypeRelationShip,WidgetDefaultSetting::RelationShipOffsetX));
    this->componentTypeCountMap.put(ComponentType::TypeAttribute,&ERModel::attributeCount);
    this->componentTypeCountMap.put(ComponentType::TypeEntity,&ERModel::entityCount);
    this->componentTypeCountMap.put(ComponentType::TypeRelationShip,&ERModel::relationShipCount);
}

void ERModel::initialCardinality(){
    this->cardinality.insertCardinality(RelationType::One);
    this->cardinality.insertCardinality(RelationType::Many);
}

void ERModel::resetCounting(){
    ERModel::attributeCount = 0;
    ERModel::entityCount = 0;
    ERModel::relationShipCount = 0;
}

void ERModel::setNodePosition(string componentType,Node* node){
    auto countIterator = this->componentTypeCountMap.get(componentType);
    double positionX = componentTypeMapOffsetX.find(componentType)->second;
    double positionY = WidgetDefaultSetting::WidgetStartY+(*countIterator)*WidgetDefaultSetting::WidgetOffsetY;
    node->setPosition(Point(positionX,positionY));
    (*countIterator)++;
}