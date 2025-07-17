#pragma once

#include "table/ITable.hpp"
#include "core/Types.hpp"
#include "core/Card.hpp"

#include <gmock/gmock.h>

#include <vector>

class MockTable : public ITable {
public:
    MOCK_METHOD(void, SetBlindSmall, (Coins_t cost), (noexcept, override));
    MOCK_METHOD(void, SetBlindBig, (Coins_t cost), (noexcept, override));
    MOCK_METHOD(Coins_t, GetBlindSmall, (), (const, noexcept, override));
    MOCK_METHOD(Coins_t, GetBlindBig, (), (const, noexcept, override));

    MOCK_METHOD(void, IncreasePot, (Coins_t amount), (noexcept, override));
    MOCK_METHOD(Coins_t, CollectPot, (), (noexcept, override));
    MOCK_METHOD(Coins_t, GetPot, (), (const, noexcept, override));

    MOCK_METHOD(void, AddCommunityCard, (Card card), (noexcept, override));
    MOCK_METHOD(const ITable::CommunityCards_t&, GetCommunityCards, (), (const, noexcept, override));
};