#include "TextUI.h"

TextUI::TextUI(TextPresentation* textPresentation) : textPresentation(textPresentation){
    this->textPresentation->registerSynchronizer(this);
}

TextUI::~TextUI(){
    this->textPresentation->unregisterSynchronizer(this);
}

void TextUI::displayMenu(){
    this->textPresentation->displayMenu();
}

void TextUI::processCommand(){
    this->textPresentation->processCommand();
}

void TextUI::sync(string syncEventType){
    this->textPresentation->executeSync(syncEventType);
}