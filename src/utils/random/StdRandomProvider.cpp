#include "utils/random/StdRandomProvider.hpp"

StdRandomProvider::StdRandomProvider(uint64_t seed)
    : rng_(seed) {}

void StdRandomProvider::Shuffle(std::vector<Card>& cards) {
    std::shuffle(cards.begin(), cards.end(), rng_);
}
