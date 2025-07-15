#pragma once

#include "core/IDeck.hpp"
#include "core/Card.hpp"

#include <gmock/gmock.h>

class MockDeck : public IDeck {
public:
    MOCK_METHOD(void, Shuffle, (), (noexcept, override));
    MOCK_METHOD(Card, Draw, (), (noexcept, override));
    MOCK_METHOD(const  DeckCards_t&, GetCards, (), (const, noexcept, override));
};
