#pragma once

#include <phevaluator/phevaluator.h>

#include "core/Card.hpp"
#include "table/PlayerSession.hpp"

#include <string>
#include <cassert>
#include <array>
#include <vector>

// This file will help us to translate our code to what
// PokerHandEvaluator module expects.

namespace Translator
{
inline std::string FromCustomCardToPhevaluatorCard(const Card& card) {
    std::string card_name{};
    switch(card.GetRank()) {
        case ECardRank::TWO:    card_name += "2"; break;
        case ECardRank::THREE:  card_name += "3"; break;
        case ECardRank::FOUR:   card_name += "4"; break;
        case ECardRank::FIVE:   card_name += "5"; break;
        case ECardRank::SIX:    card_name += "6"; break;
        case ECardRank::SEVEN:  card_name += "7"; break;
        case ECardRank::EIGHT:  card_name += "8"; break;
        case ECardRank::NINE:   card_name += "9"; break;
        case ECardRank::TEN:    card_name += "T"; break;
        case ECardRank::JACK:   card_name += "J"; break;
        case ECardRank::QUEEN:  card_name += "Q"; break;
        case ECardRank::KING:   card_name += "K"; break;
        case ECardRank::ACE:    card_name += "A"; break;
    }

    switch(card.GetSuit()) {
        case ECardSuit::CLUBS:      card_name += "C"; break;
        case ECardSuit::DIAMONDS:   card_name += "D"; break;
        case ECardSuit::HEARTS:     card_name += "H"; break;
        case ECardSuit::SPADES:     card_name += "S"; break;
    }
    
    return card_name;
}

inline phevaluator::Rank RankFromPlayerTableCards(
    const PlayerSession::Hand_t& player_cards, const std::vector<Card>& community_cards) {
    switch(community_cards.size()) {
        case 3: 
            return phevaluator::EvaluateCards(
                FromCustomCardToPhevaluatorCard(player_cards[0]),
                FromCustomCardToPhevaluatorCard(player_cards[1]),
                FromCustomCardToPhevaluatorCard(community_cards[0]),
                FromCustomCardToPhevaluatorCard(community_cards[1]),
                FromCustomCardToPhevaluatorCard(community_cards[2]));
        case 4:
            return phevaluator::EvaluateCards(
                FromCustomCardToPhevaluatorCard(player_cards[0]),
                FromCustomCardToPhevaluatorCard(player_cards[1]),
                FromCustomCardToPhevaluatorCard(community_cards[0]),
                FromCustomCardToPhevaluatorCard(community_cards[1]),
                FromCustomCardToPhevaluatorCard(community_cards[2]),
                FromCustomCardToPhevaluatorCard(community_cards[3]));
        case 5:
            return phevaluator::EvaluateCards(
                FromCustomCardToPhevaluatorCard(player_cards[0]),
                FromCustomCardToPhevaluatorCard(player_cards[1]),
                FromCustomCardToPhevaluatorCard(community_cards[0]),
                FromCustomCardToPhevaluatorCard(community_cards[1]),
                FromCustomCardToPhevaluatorCard(community_cards[2]),
                FromCustomCardToPhevaluatorCard(community_cards[3]),
                FromCustomCardToPhevaluatorCard(community_cards[4]));
        default:
            assert(false && "More community cards than expected");
            return {};
    }
}
}