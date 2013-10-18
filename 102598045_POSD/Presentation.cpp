#include "Presentation.h"
#include <iostream>
#include "Entity.h"
#include "Connector.h"
#include <iomanip>
#include "ComponentUtil.h"
#include "ERModelUtil.h"
#include "StringUtil.h"
#include "CommandMenu.h"
#include "CommandManager.h"
#include "NullPointerException.h"

Presentation::Presentation(ERModel* erModel) : erModel(erModel){
}

Presentation::~Presentation(){
}

ERModel* Presentation::getERModel(){
	return this->erModel;
}

bool Presentation::isAlive(){
	return this->alive;
}
//if alive == false,program will close
void Presentation::close(){
	this->alive = false;
	cout<<"GooBye!"<<endl;
}

string Presentation::getInput(){
	string input;	
	while(input.length() == 0){
		cout<<">";
		getline(cin,input);		
	}
	return input;
}

void Presentation::displayMenu(){
	this->displayTitle("Commands Menu");
	cout<<" +------------------------------------------------------+"<<endl;
	
	CommandMenu commandMenu;
	for each(CommandData* cmdData in commandMenu.getCommandDataMap()){
		int len = COMMAND_KEY_WIDTH+cmdData->getInfo().size();
		len = MENU_WIDTH-len;
		cout<<"�x"<<setw(COMMAND_KEY_WIDTH)<<left<<cmdData->getKey()+".";
		cout<<right<<cmdData->getInfo()<<setw(len)<<"�x"<<endl;
	}
	cout<<" +------------------------------------------------------+"<<endl;	
}

void Presentation::displayTitle(string title){	
	cout<<"  +----------------------------+"<<endl;
	cout<<" �x    "<<title<<setw(TITLE_WIDTH-title.size())<<"�x"<<endl;
	cout<<"  +----------------------------+"<<endl;
}

void Presentation::displayTable(){
	HashMap<string,Table*> tableMap = erModel->getAllTables();

	cout<<" +------------------------------------------------------------------"<<endl;
	cout<<" |    Entity      |  Attributes"<<endl;
	cout<<" +----------------+--------------------------------------------------"<<endl;
	for each(Table* table in tableMap){
		string entityName = table->getEntityName();
		int len = TABLE_WIDTH-entityName.size();

		cout<<"   "<<entityName<<setw(len)<<"|";		
		this->displayStringWithComma(" PK(",table->getAllPrimaryKeyAttributesNameSet(),")");		
		this->displayStringWithComma(" ",table->getAllDefaultKeyAttributesNameSet(),"");		
		this->displayStringWithComma(" FK(",table->getAllForeignKeyAttributesNameSet(),")");	
		cout<<endl;
	}
	cout<<" +------------------------------------------------------------------"<<endl;
}

void Presentation::displayComponents(){	
	HashMap<string,Component*> componentSet = erModel->getAllComponents();
	
	this->displayTitle("Components");
	this->displayComponentSet(componentSet);
}

void Presentation::displayConnections(){	
	HashMap<string,Connector*> connectorSet = erModel->getAllConnectors();
	//convert to vector ordered by id	
	this->displayTitle("Connections");
	cout<<" +----------------------------------------------"<<endl;
	cout<<"    Connectors   |     Node1     |     Node2"<<endl;
	cout<<" +---------------+---------------+--------------"<<endl;
	for each (Connector* connector in connectorSet){	
		cout<<"  "<<setw(COLUMN_WIDTH)<<connector->getID()<<"       ";
		cout<<"|"<<setw(COLUMN_WIDTH)<<connector->getFirstConnectedNode()->getID()<<"       ";
		cout<<"|"<<setw(COLUMN_WIDTH)<<connector->getSecondConnectedNode()->getID()<<endl;		
	}
	cout<<" +----------------------------------------------"<<endl;
}

void Presentation::displayEntities(){
	HashMap<string,Entity*> entitySet = erModel->getAllEntities();

	this->displayTitle("Entities");
	this->displayComponentSet(ComponentUtil::toComponentHashMap<Entity>(entitySet));
}

void Presentation::displayEntityAttributes(Entity* entity){
	HashMap<string,Attribute*> attributeSet = entity->getConnectedAttributes();

	cout<<"Attributes of the entity '"<<entity->getID()<<"'"<<endl;
	this->displayComponentSet(ComponentUtil::toComponentHashMap<Attribute>(attributeSet));
}

void Presentation::displayStringWithComma(string strStart,set<string> stringSet,string strEnd){
	if(stringSet.empty())
		return;

	cout<<strStart;
	string stringWithComma = StringUtil::appendWithComma(stringSet);
	cout<<stringWithComma;
	cout<<strEnd;	
}

void Presentation::processCommand(string commandKey){
	Command* command = NULL;
	try {
		CommandMenu commandMenu;
		CommandData* commandData = commandMenu.getCommandDataByKey(commandKey);		
		//get new Command Function From Command Data
		NewCommandFunction newCommandFunction = commandData->getNewCommandFunction();
		//new one Command for Command Manager to Execute
		command = newCommandFunction(this);
		executeCommand(command);
	}
	catch(NullPointerException){
		cout<<"wrong command,please input correct command."<<endl;		
	}
	catch(Exception& exception){
		cout<<exception.getMessage()<<endl;		
		delete command;
	}	
}

void Presentation::executeCommand(Command* command){
	//display command information & get user input
	string commandInfo = command->getCommandInformation();
	if(commandInfo!= "")
		cout<<commandInfo<<endl;
	command->setupCommand();
	//execute
	if(command->isUnexecutable())
		this->commandManager->execute(static_cast<UnexecutableCommand*>(command));
	else {		
		command->execute();
	}
}

void Presentation::logMessage(string message,bool nextLine){
	cout<<message;
	if(nextLine)
		cout<<endl;
}

void Presentation::setCommandManager(CommandManager* commandManager){
	this->commandManager = commandManager;
}

CommandManager* Presentation::getCommandManager(){
	return this->commandManager;
}

void Presentation::displayComponentSet(HashMap<string,Component*> componentSet){	
	cout<<" +-----------------------------------------------------"<<endl;
	cout<<"        Type      |       ID       |      Name"<<endl;
	cout<<" +----------------+----------------+-------------------"<<endl;
	for each (Component* component in componentSet){
		cout<<setw(COLUMN_WIDTH)<<component->getType()<<"          ";	
		cout<<"|"<<setw(COLUMN_WIDTH)<<component->getID()<<"        ";	
		cout<<"|  "<<component->getName()<<endl;		
	}
	cout<<" +-----------------------------------------------------"<<endl;
}