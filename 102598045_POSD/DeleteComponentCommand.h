#pragma once

#include "Command.h"
#include "ConnectionData.h"
#include "HashMap.h"
#include <gtest/gtest_prod.h> 

class Component;
class Attribute;
class Connector;

class DeleteComponentCommand : public Command{
    friend class DeleteComponentCommandTest;
    FRIEND_TEST(DeleteComponentCommandTest,testUnexecute);
    FRIEND_TEST(DeleteComponentCommandTest,testDeleteComponentCommand);
    FRIEND_TEST(DeleteComponentCommandTest,testSaveConnectionData);
    FRIEND_TEST(DeleteComponentCommandTest,testClearConnectionDataMap);
    FRIEND_TEST(DeleteComponentCommandTest,testRemoveAndDisconnectComponents);
    FRIEND_TEST(DeleteComponentCommandTest,testReConnectComponents);
public:
    DeleteComponentCommand(HashMap<string,Component*>& componentMap,Component* component);
    ~DeleteComponentCommand();
protected:
    void doExecute();
    void doUnExecute();
private:
    inline void saveConnectionData(Connector* connector);
    inline void saveAttributeKeyAndSetAsDefaultKey(HashMap<string,Attribute*> connectedAttributeMap);
    inline void clearConnectionDataMap();
    inline void removeAndDisconnectComponents();
    inline void reConnectComponents(ConnectionData* connectionData,Connector* connector);
    HashMap<string,Component*>& componentMap;
    Component* component;
    HashMap<string,Connector*> connectionMap;
    HashMap<string,Attribute*> attributePrimaryKeyMap;
    HashMap<string,ConnectionData*> connectionDataMap;
    HashMap<string,unsigned int> componentIndexMap;
};