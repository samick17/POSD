#pragma once

#include "TextInstruction.h"

class Node;

class AddNodeInstruction : public TextInstruction{
public:
    AddNodeInstruction();
    ~AddNodeInstruction();
    virtual void execute(TextPresentation* textPresentation,TextUIPresenter* textUIPresenter);
private:
	Node* addNode(TextPresentation* textPresentation);
};