#pragma once

#include "core/Card.hpp"

#include <vector>

class IRandomProvider {
public:
    virtual ~IRandomProvider() = default;
    virtual void Shuffle(std::vector<Card>& cards) = 0;
};
