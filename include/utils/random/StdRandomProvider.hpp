#pragma once

#include "utils/random/IRandomProvider.hpp"

#include <random>

class StdRandomProvider : public IRandomProvider {
public:
    explicit StdRandomProvider(uint64_t seed = std::random_device{}());
    void Shuffle(std::vector<Card>& cards) override;

private:
    std::mt19937 rng_;
};
