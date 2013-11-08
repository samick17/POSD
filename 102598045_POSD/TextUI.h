#pragma once

#include "TextPresentation.h"
#include "ISynchronizer.h"

class Table;
class Connector;

class TextUI : public ISynchronizer{
public:    
    TextUI(TextPresentation* textPresentation);
    ~TextUI();

    void displayMenu();
    void processCommand();

    void sync(int syncEventType);
private:
    TextPresentation* textPresentation;
};