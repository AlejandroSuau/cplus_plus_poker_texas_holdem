#pragma once

enum class ESuit {
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES
};

enum class ERank {
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

// TODO: Struct better?
//       inline operator? or in class method?

class Card {
public:
    Card() noexcept;

    // constexpr Card(const Card& card);
    // constexpr Card(Card&& card);
    // constexpr Card& operator=(const Card& card);
    // constexpr Card& operator=(Card&& card);

    Card(ESuit suit, ERank rank) noexcept;
    ESuit GetSuit() const noexcept;
    ERank GetRank() const noexcept;

    bool operator==(const Card& other) const noexcept;

private:
    ESuit suit_;
    ERank rank_;
};
