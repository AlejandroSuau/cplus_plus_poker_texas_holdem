# C++ Poker Texas Hold'em Engine
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![License](https://img.shields.io/badge/license-MIT-blue.svg)]()

A modern, extensible and robust Texas Hold'em Poker engine written in **Modern C++**.  
Engineered for clarity, reliability, testability, and professional-grade code quality.

---

## ✨ Features

- **Strict Texas Hold'em Rules:** Full support for game state, betting rounds, community cards, and seat management.
- **Clean Domain Separation:** Clear division of core data, per-hand session state, and player seating.
- **Fully Tested:** 100% unit tests with [Google Test](https://github.com/google/googletest) and mocks for all key components.
- **Modern C++:** Uses `std::array`, `std::optional`, strong enums, and C++20 idioms.
- **Best-in-class hand evaluation:** Uses the [phevaluator](https://github.com/HenryRLee/PokerHandEvaluator) C++ library for fast, accurate poker hand ranking.
- **Extensible:** Easily add bots, game variants (Omaha, short deck, tournaments), or network support.
- **Deterministic and Fast:** Supports reproducible games and deterministic testing.
- **Recruiter Friendly:** Readable, idiomatic C++ codebase, professionally documented.

---

## 🚀 Quick Start - Running on MacOS

```bash
git clone --recurse-submodules https://github.com/AlejandroSuau/cplus_plus_poker_texas_holdem
cd cplus_plus_poker_texas_holdem
cmake -B build
cd build
cmake --build .
./bin/poker_main
```

To run all tests:
```bash
ctest --output-on-failure
```

---

## 📁 Directory Structure

```
include/      # Public headers: core, table, game_logic, utils
src/          # Source files
tests/        # GoogleTest-based unit and integration tests
externals/    # External dependencies (phevaluator)
CMakeLists.txt
README.md
```

---

## 🏗️ Architecture Highlights

- **Domain-Driven Design:**  
  - `Player` for persistent player data  
  - `PlayerSession` for per-hand (ephemeral) state  
  - `PlayerList` for seat management and table logic  
- **Pure Engine:** All randomness and state transitions are fully injectable for deterministic tests.
- **Single Responsibility Principle:** Each class encapsulates a clear and unique responsibility.
- **Full Mockability:** Deck, Table, and RandomProvider can all be mocked or injected.
- **Clear separation of core, game logic, and infrastructure.**

---

## 🧪 Testing

- Built with **Google Test** and **Google Mock** for all major modules.
- 100% coverage of Deck, PlayerList, Table, PlayerSession, and GameLogic.
- To run tests:
  ```bash
  ctest --output-on-failure
  ```

---

## 🛣️ Roadmap

- [x] Complete hand management and seat rotation
- [x] Full unit and integration testing
- [ ] Side pot and split-pot support
- [ ] Graphics

   ... Maybe aswell ...

- [ ] Tournament and statistics modules
- [ ] Multiplayer/network functionality
- [ ] Bot players (AI)
- [ ] Command-line or graphical UI

---

## 📄 License

This project is licensed under the MIT License.

---

## 👤 About the Author

Built with passion by **[Alejandro]**  
*Senior C++ Engineer & Poker Enthusiast*  

---

> **Feel free to fork, star, or contact me for collaboration or professional opportunities!**
