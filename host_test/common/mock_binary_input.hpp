#pragma once

#include "gmock/gmock.h"
#include "interfaces/i_binary_input.hpp"

class MockBinaryInput : public IBinaryInput
{
public:
    MOCK_METHOD(bool, is_active, (), (override));
};
