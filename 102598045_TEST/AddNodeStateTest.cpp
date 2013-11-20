#include "AddNodeStateTest.h"

void AddNodeStateTest::SetUp(){
    this->presentation = new Presentation(&this->erModel);
    this->graphicalPresentation = new GraphicalPresentation(this->presentation);
}

void AddNodeStateTest::TearDown(){
    delete this->presentation;
    delete this->graphicalPresentation;
}