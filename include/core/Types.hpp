#pragma once

// We may change it later.
using Coins_t = double;

enum class EPlayerAction {
    FOLD,
    CHECK,
    CALL,
    BET,
    RAISE,
    ALL_IN
};

enum class ECardSuit {
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES
};

enum class ECardRank {
    TWO   = 2,
    THREE = 3, 
    FOUR  = 4,
    FIVE  = 5,
    SIX   = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE  = 9,
    TEN   = 10,
    JACK  = 11,
    QUEEN = 12,
    KING  = 13,
    ACE   = 14
};

enum class ELogicState {
    NONE,
    PREFLOP,
    FLOP,
    TURN,
    RIVER,
    SHOWDOWN,
    HAND_FINISHED
};