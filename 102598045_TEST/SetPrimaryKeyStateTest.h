#pragma once

#include "SetPrimaryKeyState.h"
#include "ERModel.h"
#include "Presentation.h"
#include "GraphicalPresentation.h"
#include <gtest/gtest.h>

class SetPrimaryKeyStateTest : public testing::Test{
protected:
    SetPrimaryKeyState* setPrimaryKeyState;
    ERModel erModel;
    Presentation* presentation;
    GraphicalPresentation* graphicalPresentation;
    virtual void SetUp();
    virtual void TearDown();
};