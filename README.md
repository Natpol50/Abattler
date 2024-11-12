# Abattler

Asha's (smol) battler's a text-based RPG combat game written in C++ featuring an interactive battle system with elements, status effects, and math-based blocking mechanics.

## 🎮 Game Features

### Combat System
- Turn-based battles against various monsters
- Combo point system for special moves
- Interactive blocking system with math challenges
- Element-based damage system
- Status effects that influence combat
- Inventory system with usable items

### Character Elements
- **5 Monster Types**: Normal, Fire, Ice, Poison, Undead
- **Element Interactions**:
  - Fire is super effective against Ice
  - Ice is strong against Poison
  - Fire deals bonus damage to Undead
  - Poison is weak against Undead

### Status Effects
- **Burn**: Reduces attack power
- **Frozen**: Reduces defense
- **Poison**: Reduces both attack and defense
- **Curse**: Severely reduces both stats

### Items and Equipment
- Health Potion: Instant healing
- Healing Salve: Healing over time
- Warrior's Elixir: Temporary attack boost
- Stone Skin Potion: Temporary defense boost
- Battle Flask: Temporary attack and defense boost

## 🎯 Gameplay Mechanics

### Combat Actions
1. **Basic Attack**: Builds combo points
2. **Special Moves**: Requires 3+ combo points
3. **Block Stance**: Solve math problems to reduce incoming damage
4. **Item Usage**: Use items from inventory
5. **Run**: Attempt to escape battle

### Progression System
- Gain experience points from defeating monsters
- Level up to increase stats
- Bonus XP for defeating elemental monsters
- Random item drops from victories


## 🚀 Getting Started

1. Clone the repository
2. Compile the source code using a C++ compiler
3. Run the executable
4. Enter your hero's name when prompted
5. Follow the tutorial to learn game mechanics
6. Battle monsters and level up!



## 🛠️ Technical Requirements (for dev)

### Prerequisites
- C++ compiler with C++11 support or higher
- Standard Template Library (STL)
- System capable of running console applications

### Required Libraries
```cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <limits>
#include <ctime>
#include <future>
#include <map>
#include <random>
#include <iomanip>
```

## 🎪 Game Structure (for dev)

### Core Classes
- `Creature`: Base class for all entities
- `Hero`: Player character class (inherits from Creature)
- `StatusEffect`: Manages status effects and their durations
- `Item`: Handles item properties and effects

### Key Data Structures
- `MonsterTemplate`: Template for monster creation
- `vector<Item>`: Inventory management
- `map<string, StatusEffect>`: Active effects tracking

## 📜 License

This project is available for free use and modification. Please credit the original source if you redistribute or modify it.
