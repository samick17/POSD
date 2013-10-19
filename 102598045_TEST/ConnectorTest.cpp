#include "ConnectorTest.h"
#include "ComponentType.h"
#include "InvalidConnectException.h"

void ConnectorTest::SetUp(){
	this->attribute = this->erModel.addNode(ComponentType::TypeAttribute);
	this->relationShip = this->erModel.addNode(ComponentType::TypeRelationShip);
	this->entity = this->erModel.addNode(ComponentType::TypeEntity);

	this->erModel.addConnection(this->attribute,this->entity);
	this->connector = this->erModel.getNodesConnector(this->attribute,this->entity);
}

void ConnectorTest::TearDown(){
}

TEST_F(ConnectorTest,testToString){
	ASSERT_EQ("3 0,2",this->connector->toString());
}

TEST_F(ConnectorTest,testGetType){
	ASSERT_EQ(ComponentType::TypeConnector,this->connector->getType());
}

TEST_F(ConnectorTest,testBreakAllConnections){
	//assert connection size
	ASSERT_EQ(2,this->connector->connectionMap.size());
	Component* firstConnectedNode = this->connector->getFirstConnectedNode();
	Component* secondConnectedNode = this->connector->getSecondConnectedNode();
	//assert have connection
	ASSERT_EQ(this->connector,firstConnectedNode->connectionMap.get(this->connector->getID()));
	ASSERT_EQ(this->connector,secondConnectedNode->connectionMap.get(this->connector->getID()));
	//assert break connection
	this->connector->breakAllConnections();
	
	ASSERT_EQ(0,this->connector->connectionMap.size());
}

TEST_F(ConnectorTest,testCanConnectTo){
	ASSERT_THROW(this->connector->canConnectTo(this->attribute),InvalidConnectException);
	ASSERT_THROW(this->connector->canConnectTo(this->relationShip),InvalidConnectException);
	ASSERT_THROW(this->connector->canConnectTo(this->entity),InvalidConnectException);
	ASSERT_THROW(this->connector->canConnectTo(this->connector),InvalidConnectException);
}

TEST_F(ConnectorTest,testHasSizeToConnect){
	//will always false, because connector will always construct when need to connect two node.
	ASSERT_EQ(false,this->connector->hasSizeToConnect());
}

TEST_F(ConnectorTest,testGetFirstConnectedNode){
	this->erModel.addConnection(this->entity,this->relationShip);
	Connector* connectorEntityAndRelation = this->erModel.getNodesConnector(this->relationShip,this->entity);
	
	ASSERT_EQ(this->attribute,this->connector->getFirstConnectedNode());
	ASSERT_EQ(this->relationShip,connectorEntityAndRelation->getFirstConnectedNode());

}

TEST_F(ConnectorTest,testGetSecondConnectedNode){
	this->erModel.addConnection(this->entity,this->relationShip);
	Connector* connectorEntityAndRelation = this->erModel.getNodesConnector(this->relationShip,this->entity);

	ASSERT_EQ(this->entity,this->connector->getSecondConnectedNode());
	ASSERT_EQ(this->entity,connectorEntityAndRelation->getSecondConnectedNode());
}

TEST_F(ConnectorTest,testIsNodesConnection){
	ASSERT_EQ(false,this->connector->isNodesConnection(this->attribute,this->relationShip));
	ASSERT_EQ(false,this->connector->isNodesConnection(this->relationShip,this->attribute));
	ASSERT_EQ(false,this->connector->isNodesConnection(this->entity,this->relationShip));
	ASSERT_EQ(false,this->connector->isNodesConnection(this->relationShip,this->entity));

	ASSERT_EQ(true,this->connector->isNodesConnection(this->attribute,this->entity));
	ASSERT_EQ(true,this->connector->isNodesConnection(this->entity,this->attribute));
}

TEST_F(ConnectorTest,testClone){
	Connector* connectorCloned = static_cast<Connector*>(this->connector->clone());

	ASSERT_EQ(this->connector->getID(),connectorCloned->getID());
	ASSERT_EQ(this->connector->getName(),connectorCloned->getName());
	ASSERT_EQ(this->connector->getType(),connectorCloned->getType());
	//assert map
	ASSERT_EQ(this->connector->componentData,connectorCloned->componentData);

	delete connectorCloned;
}