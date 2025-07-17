#pragma once

#include "core/IDeck.hpp"
#include "core/Card.hpp"

#include <gmock/gmock.h>

#include <optional>

class MockDeck : public IDeck {
public:
    MOCK_METHOD(void, Shuffle, (), (noexcept, override));
    MOCK_METHOD(std::optional<Card>, Draw, (), (noexcept, override));
    MOCK_METHOD(const IDeck::DeckCards_t&, GetCards, (), (const, noexcept, override));
};
