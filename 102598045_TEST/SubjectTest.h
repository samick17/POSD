#pragma once

#include "Subject.h"
#include <gtest/gtest.h>

class SubjectTest : public testing::Test{
protected:
    virtual void SetUp();
    virtual void TearDown();
};