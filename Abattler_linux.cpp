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

using namespace std;

// Status Effect structure
struct StatusEffect {
    string name;
    int duration;
    float damageMultiplier;
    float defenseMultiplier;
    
    string getDescription() const {
        string desc = name;
        if (damageMultiplier != 1.0f) {
            desc += " (ATK x" + to_string(damageMultiplier).substr(0, 4) + ")";
        }
        if (defenseMultiplier != 1.0f) {
            desc += " (DEF x" + to_string(defenseMultiplier).substr(0, 4) + ")";
        }
        return desc + " [" + to_string(duration) + " turns]";
    }
};

// Item structure
struct Item {
    string name;
    string description;
    int duration;  // 0 for instant effects, >0 for over-time effects
    float healAmount;
    float attackBuff;
    float defenseBuff;
    int quantity;
    
    string getDescription() const {
        string desc = name + ": " + description;
        if (quantity > 0) {
            desc += " (x" + to_string(quantity) + ")";
        }
        return desc;
    }
};

// Monster Types
enum class MonsterType {
    NORMAL,
    FIRE,
    ICE,
    POISON,
    UNDEAD
};

map<MonsterType, string> elementNames = {
    {MonsterType::NORMAL, "Normal"},
    {MonsterType::FIRE, "Fire"},
    {MonsterType::ICE, "Ice"},
    {MonsterType::POISON, "Poison"},
    {MonsterType::UNDEAD, "Undead"}
};

struct MonsterTemplate {
    string name;
    MonsterType type;
    float baseHP;
    float baseAttack;
    vector<pair<string, float>> specialMoves; // name and damage multiplier
};

vector<MonsterTemplate> monsterTemplates = {
    {"Goblin", MonsterType::NORMAL, 20, 4, {{"Sneaky Strike", 1.2f}, {"Rabid Attack", 1.4f}}},
    {"Fire Drake", MonsterType::FIRE, 25, 5, {{"Flame Breath", 1.5f}, {"Heat Wave", 1.3f}}},
    {"Frost Giant", MonsterType::ICE, 30, 3, {{"Ice Shard", 1.4f}, {"Freeze", 1.2f}}},
    {"Poison Spider", MonsterType::POISON, 15, 6, {{"Venom Strike", 1.3f}, {"Web Trap", 1.1f}}},
    {"Skeleton", MonsterType::UNDEAD, 18, 4, {{"Bone Throw", 1.2f}, {"Death Touch", 1.4f}}},
    {"Dragon", MonsterType::FIRE, 40, 7, {{"Inferno", 1.8f}, {"Wing Slash", 1.5f}}},
    {"Ice Witch", MonsterType::ICE, 22, 5, {{"Blizzard", 1.6f}, {"Frost Nova", 1.4f}}},
    {"Toxic Slime", MonsterType::POISON, 25, 3, {{"Acid Splash", 1.3f}, {"Dissolve", 1.5f}}},
    {"Lich", MonsterType::UNDEAD, 35, 6, {{"Soul Drain", 1.7f}, {"Curse", 1.4f}}},
    {"Babayaga", MonsterType::NORMAL, 50, 10, {{"Doggono", 3.0f}, {"Mad gun", 2.5f}}}
};

vector<Item> itemTemplates = {
    {"Health Potion", "Instantly restores 15 HP", 0, 15.0f, 0.0f, 0.0f, 1},
    {"Healing Salve", "Heals 5 HP per turn for 3 turns", 3, 5.0f, 0.0f, 0.0f, 1},
    {"Warrior's Elixir", "Increases attack by 50% for 3 turns", 3, 0.0f, 1.5f, 1.0f, 1},
    {"Stone Skin Potion", "Increases defense by 50% for 3 turns", 3, 0.0f, 1.0f, 1.5f, 1},
    {"Battle Flask", "Increases both attack and defense by 25% for 2 turns", 2, 0.0f, 1.25f, 1.25f, 1}
};

// Math challenge functions
pair<string, int> generateMathProblem() {
    int num1 = rand() % 10 + 1;
    int num2 = rand() % 10 + 1;
    int operation = rand() % 3;
    string problem;
    int answer;
    
    switch(operation) {
        case 0:
            problem = to_string(num1) + " + " + to_string(num2);
            answer = num1 + num2;
            break;
        case 1:
            if (num1 < num2) swap(num1, num2);
            problem = to_string(num1) + " - " + to_string(num2);
            answer = num1 - num2;
            break;
        case 2:
            problem = to_string(num1) + " × " + to_string(num2);
            answer = num1 * num2;
            break;
    }
    
    return {problem, answer};
}

bool getAnswerWithTimeout(int& answer, int timeoutSeconds) {
    auto start = chrono::steady_clock::now();
    string input;
    
    cout << "Time remaining: " << timeoutSeconds << "s\n";
    
    while (true) {
        if (cin.rdbuf()->in_avail()) {
            cin >> input;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            try {
                answer = stoi(input);
                return true;
            } catch (...) {
                return false;
            }
        }
        
        auto now = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::seconds>(now - start).count();
        if (elapsed >= timeoutSeconds) {
            return false;
        }
        
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

class Creature {
protected:
    string name;
    float pv;
    float pa;
    int niveau;
    MonsterType type;
    vector<pair<string, float>> specialMoves;
    map<string, StatusEffect> activeEffects;
    int comboPoints;
    float pv_max;

public:
    Creature(const MonsterTemplate& templ, int level = 1) {
        name = templ.name;
        type = templ.type;
        specialMoves = templ.specialMoves;
        niveau = level;
        pv = templ.baseHP * (1 + (niveau * 0.5f));
        pa = templ.baseAttack * (1 + (niveau * 0.3f));
        pv_max = pv;
        comboPoints = 0;
    }

    string getName() const { return name; }
    float getPV() const { return pv; }
    float getPVMax() const { return pv_max; }
    float getPA() const { return pa; }
    int getNiveau() const { return niveau; }
    MonsterType getType() const { return type; }
    int getComboPoints() const { return comboPoints; }
    
    vector<string> getActiveEffects() const {
        vector<string> effects;
        for (const auto& [_, effect] : activeEffects) {
            effects.push_back(effect.getDescription());
        }
        return effects;
    }

    virtual float calculateDamage(float baseDamage, MonsterType targetType) {
        float multiplier = 1.0f;
        
        // Type effectiveness
        if (type == MonsterType::FIRE && targetType == MonsterType::ICE) multiplier = 1.5f;
        if (type == MonsterType::ICE && targetType == MonsterType::FIRE) multiplier = 0.5f;
        if (type == MonsterType::POISON && targetType == MonsterType::UNDEAD) multiplier = 0.5f;
        if (type == MonsterType::FIRE && targetType == MonsterType::UNDEAD) multiplier = 1.25f;
        if (type == MonsterType::ICE && targetType == MonsterType::POISON) multiplier = 1.25f;
        
        // Apply status effects
        for (const auto& [_, effect] : activeEffects) {
            multiplier *= effect.damageMultiplier;
        }
        
        if (multiplier != 1.0f) {
            if (multiplier > 1.0f) cout << "It's super effective! (x" << multiplier << ")\n";
            else cout << "It's not very effective... (x" << multiplier << ")\n";
        }
        
        return baseDamage * multiplier;
    }

    float attaque(Creature& target) {
        float baseDamage = pa * (1.0f + float(rand() % 10)/10.0f);
        float finalDamage = calculateDamage(baseDamage, target.getType());
        comboPoints++;
        return finalDamage;
    }

    string performSpecialMove(Creature& target) {
        if (specialMoves.empty()) return "No special moves available!";
        
        int moveIndex = rand() % specialMoves.size();
        auto [moveName, multiplier] = specialMoves[moveIndex];
        float damage = pa * multiplier;
        
        // Add status effects based on type
        switch(type) {
            case MonsterType::FIRE:
                target.addStatusEffect("Burn", 3, 0.9f, 1.0f);
                break;
            case MonsterType::ICE:
                target.addStatusEffect("Frozen", 2, 1.0f, 0.8f);
                break;
            case MonsterType::POISON:
                target.addStatusEffect("Poisoned", 4, 0.8f, 0.9f);
                break;
            case MonsterType::UNDEAD:
                target.addStatusEffect("Cursed", 3, 0.7f, 0.7f);
                break;
            default:
                break;
        }
        
        target.subitDegat(calculateDamage(damage, target.getType()));
        return moveName;
    }

    void addStatusEffect(string name, int duration, float dmgMult, float defMult) {
        activeEffects[name] = {name, duration, dmgMult, defMult};
        cout << name << " status effect applied!\n";
    }

    void updateStatusEffects() {
        vector<string> expiredEffects;
        for (auto& [name, effect] : activeEffects) {
            effect.duration--;
            if (effect.duration <= 0) {
                expiredEffects.push_back(name);
            }
        }
        for (const auto& name : expiredEffects) {
            activeEffects.erase(name);
            cout << name << " effect has worn off!\n";
        }
    }

    virtual void subitDegat(float degat) {
        float finalDamage = degat;
        for (const auto& [_, effect] : activeEffects) {
            finalDamage *= effect.defenseMultiplier;
        }
        
        int esquive = rand() % 4;
        if (esquive > 1) {
            pv = pv - finalDamage;
            cout << name << " took " << finalDamage << " damage!\n";
        } else {
            cout << name << " dodged the attack!\n";
        }
    }

    bool estVivant() const {
        return pv > 0;
    }

    virtual void resetCombo() {
        comboPoints = 0;
    }
};

class Hero : public Creature {
private:
    bool isBlocking;
    float xp;
    int successful_blocks;
    vector<pair<string, float>> heroSpecialMoves;
    vector<Item> inventory;
    vector<Item> activeItems;

public:
    Hero(string name) 
        : Creature({"Hero", MonsterType::NORMAL, 30, 5, {}}, 1) {
        this->name = name;
        isBlocking = false;
        xp = 0;
        successful_blocks = 0;
        
        heroSpecialMoves = {
            {"Triple Strike", 1.8f},
            {"Whirlwind Slash", 2.0f},
            {"Power Attack", 2.2f},
            {"Ultimate Combo", 2.5f}
        };
    }

    void setBlocking(bool blocking) { isBlocking = blocking; }
    bool getIsBlocking() const { return isBlocking; }
    int getSuccessfulBlocks() const { return successful_blocks; }

    void addItem(const Item& item) {
        for (auto& invItem : inventory) {
            if (invItem.name == item.name) {
                invItem.quantity += item.quantity;
                cout << "Added " << item.name << " to inventory.\n";
                return;
            }
        }
        inventory.push_back(item);
        cout << "Added " << item.name << " to inventory.\n";
    }
    
    void useItem(int index) {
        if (index >= 0 && index < inventory.size()) {
            Item& item = inventory[index];
            
            if (item.quantity <= 0) {
                cout << "No more " << item.name << " remaining!\n";
                return;
            }
            
            cout << "Used " << item.name << "!\n";
            
            // Apply instant healing
            if (item.healAmount > 0 && item.duration == 0) {
                float oldHP = pv;
                pv = min(pv + item.healAmount, pv_max);
                cout << "Healed for " << (pv - oldHP) << " HP!\n";
            }
            
            // Add to active items if it has duration
            if (item.duration > 0) {
                activeItems.push_back(item);
                cout << "Effect will last for " << item.duration << " turns.\n";
            }
            
            item.quantity--;
        }
    }
    
    void updateActiveItems() {
        vector<int> expiredIndexes;
        
        // Update and track expired items
        for (int i = 0; i < activeItems.size(); i++) {
            Item& item = activeItems[i];
            
            // Apply over-time effects
            if (item.healAmount > 0) {
                float oldHP = pv;
                pv = min(pv + item.healAmount, pv_max);
                cout << item.name << " healed for " << (pv - oldHP) << " HP!\n";
            }
            
            item.duration--;
            if (item.duration <= 0) {
                expiredIndexes.push_back(i);
            }
        }
        
        // Remove expired items (in reverse order to maintain indexes)
        for (int i = expiredIndexes.size() - 1; i >= 0; i--) {
            cout << activeItems[expiredIndexes[i]].name << " effect has worn off!\n";
            activeItems.erase(activeItems.begin() + expiredIndexes[i]);
        }
    }
    
    float calculateDamage(float baseDamage, MonsterType targetType) override {
        float multiplier = Creature::calculateDamage(baseDamage, targetType);
        
        // Apply item buffs
        for (const auto& item : activeItems) {
            multiplier *= item.attackBuff;
        }
        
        return multiplier;
    }

    void subitDegat(float degat) override {
        if (isBlocking) {
            cout << "\nQuick! Solve this problem to block effectively!\n";
            auto [problem, correct_answer] = generateMathProblem();
            cout << problem << " = ? (5 seconds to answer!)\n";
            
            int user_answer;
            bool answered = getAnswerWithTimeout(user_answer, 5);
            
            if (answered && user_answer == correct_answer) {
                cout << "Correct! Perfect block!\n";
                successful_blocks++;
                float reducedDamage = degat * 0.3f;
                
                // Apply item defense buffs to blocked damage
                for (const auto& item : activeItems) {
                    reducedDamage *= (2.0f - item.defenseBuff);
                }
                
                pv = pv - reducedDamage;
                cout << name << " blocked most of the damage! Only took " << reducedDamage << " damage!\n";
            } else {
                if (!answered) cout << "Time's up! Block failed!\n";
                else cout << "Wrong answer! Block failed!\n";
                cout << "The correct answer was: " << correct_answer << "\n";
                
                float finalDamage = degat;
                // Apply item defense buffs to failed block
                for (const auto& item : activeItems) {
                    finalDamage *= (2.0f - item.defenseBuff);
                }
                pv = pv - finalDamage;
                cout << name << " took " << finalDamage << " damage!\n";
            }
        } else {
            float finalDamage = degat;
            // Apply item defense buffs
            for (const auto& item : activeItems) {
                finalDamage *= (2.0f - item.defenseBuff);
            }
            pv = pv - finalDamage;
            cout << name << " took " << finalDamage << " damage!\n";
        }
    }

    string performHeroSpecialMove(Creature& target) {
        if (comboPoints < 3) {
            return "Not enough combo points! (Need 3, have " + to_string(comboPoints) + ")";
        }
        
        int moveIndex = min((comboPoints - 3), (int)heroSpecialMoves.size() - 1);
        auto [moveName, multiplier] = heroSpecialMoves[moveIndex];
        float damage = pa * multiplier * (1.0f + float(successful_blocks) / 10.0f);
        
        target.subitDegat(calculateDamage(damage, target.getType()));
        resetCombo();
        return moveName;
    }

    void addXP(float gained_xp) {
        xp += gained_xp;
        cout << "\nGained " << gained_xp << " XP!\n";
        
        while (xp >= 100.0) {
            niveau++;
            pv_max += 10;
            pa += 3;
            xp -= 100.0;
            cout << "\nLEVEL UP! You are now level " << niveau << "!\n";
            cout << "Max HP increased by 5!\n";
            cout << "Attack increased by 2!\n";
            
            pv = pv_max;
            cout << "You've been fully healed!\n";
        }
        
        cout << "XP Progress: " << xp << "/100\n";
    }
    
    vector<string> getInventoryList() const {
        vector<string> list;
        for (const auto& item : inventory) {
            if (item.quantity > 0) {
                list.push_back(item.getDescription());
            }
        }
        return list;
    }
    
    vector<string> getActiveItemsList() const {
        vector<string> list;
        for (const auto& item : activeItems) {
            list.push_back(item.name + " (" + to_string(item.duration) + " turns remaining)");
        }
        return list;
    }
};

void displayBattle(const Hero& player, const Creature& monster) {
    cout << string(50, '=') << "\n\n";
    
    // Display player status
    cout << "=== " << player.getName() << " ===\n"
         << "Level: " << player.getNiveau() << "\n"
         << "HP: " << player.getPV() << "/" << player.getPVMax() << "\n"
         << "Attack: " << player.getPA() << "\n"
         << "Combo Points: " << player.getComboPoints() << "\n"
         << "Stance: " << (player.getIsBlocking() ? "Blocking" : "Normal") << "\n";
    
    auto playerEffects = player.getActiveEffects();
    if (!playerEffects.empty()) {
        cout << "Status Effects:\n";
        for (const auto& effect : playerEffects) {
            cout << "  - " << effect << "\n";
        }
    }
    
    auto activeItems = player.getActiveItemsList();
    if (!activeItems.empty()) {
        cout << "Active Items:\n";
        for (const auto& item : activeItems) {
            cout << "  - " << item << "\n";
        }
    }
    
    auto inventory = player.getInventoryList();
    if (!inventory.empty()) {
        cout << "Inventory:\n";
        for (int i = 0; i < inventory.size(); i++) {
            cout << "  " << (i+1) << ". " << inventory[i] << "\n";
        }
    }
    
    cout << "\n" << string(25, '-') << "\n\n";
    
    // Display monster status
    cout << "=== " << monster.getName() << " ===\n"
         << "Type: " << elementNames[monster.getType()] << "\n"
         << "Level: " << monster.getNiveau() << "\n"
         << "HP: " << monster.getPV() << "/" << monster.getPVMax() << "\n"
         << "Attack: " << monster.getPA() << "\n";
    
    auto monsterEffects = monster.getActiveEffects();
    if (!monsterEffects.empty()) {
        cout << "Status Effects:\n";
        for (const auto& effect : monsterEffects) {
            cout << "  - " << effect << "\n";
        }
    }
    
    cout << "\n" << string(50, '=') << "\n";
}

void clearScreen() {
    cout << string(100, '\n');
}

void pause(int milliseconds) {
    this_thread::sleep_for(chrono::milliseconds(milliseconds));
}

void displayGameTitle() {
    cout << R"(
 █████        ██████   █████  ████████ ████████ ██      ███████ 
██   ██       ██   ██ ██   ██    ██       ██    ██      ██      
███████ █████ ██████  ███████    ██  🦊   ██    ██      █████   
██   ██       ██   ██ ██   ██    ██       ██    ██      ██      
██   ██       ██████  ██   ██    ██       ██    ███████ ███████ 
    )" << "\n";
}

void displayTutorial() {
    cout << "\n=== GAME TUTORIAL ===\n"
         << "1. COMBAT BASICS:\n"
         << "   - Attack to build combo points\n"
         << "   - Use special moves when you have 3+ combo points\n"
         << "   - Block with correct math answers to reduce damage\n"
         << "   - Use items to heal or gain temporary buffs\n\n"
         << "2. ELEMENT SYSTEM:\n"
         << "   - Fire beats Ice\n"
         << "   2 Ice > Poison\n"
         << "   - Fire > Undead\n"
         << "   D Poison is weak vs Undead\n\n"
         << "3. STATUS EFFECTS:\n"
         << "   - Burn: Reduces attack power\n"
         << "   - Frozen: Reduces defense\n"
         << "   - Poison: Reduces both attack and defense\n"
         << "   - Curse: Severely reduces both stats\n\n"
         << "4. ITEMS:\n"
         << "   - Health Potion: Instant healing\n"
         << "   - Healing Salve: Healing over time\n"
         << "   - Warrior's Elixir: Temporary attack boost\n"
         << "   - Stone Skin Potion: Temporary defense boost\n"
         << "   - Battle Flask: Temporary attack and defense boost\n\n"
         << "Press Enter to continue...";
    cin.get();
}

int main() {
    srand(time(nullptr));
    clearScreen();
    displayGameTitle();
    
    string playerName;
    cout << "\nEnter your hero's name: ";
    getline(cin, playerName);
    
    Hero player(playerName);
    
    // Give starting items
    player.addItem({"Health Potion", "Instantly restores 15 HP", 0, 15.0f, 0.0f, 0.0f, 8});
    player.addItem({"Healing Salve", "Heals 6 HP per turn for 4 turns", 4, 6.0f, 0.0f, 0.0f, 8});
    player.addXP(300);
    
    int monstersDefeated = 0;
    
    displayTutorial();
    
    while (player.estVivant()) {
        clearScreen();
        
        // Select and scale monster based on progress
        int monsterIndex = rand() % monsterTemplates.size();
        int monsterLevel = 1 + (monstersDefeated / 3);
        Creature monster(monsterTemplates[monsterIndex], monsterLevel);
        
        cout << "A level " << monster.getNiveau() << " " 
             << elementNames[monster.getType()] << " " 
             << monster.getName() << " appears!\n\n";
        
        // Battle loop
        bool playerTurn = true;
        bool battleContinues = true;
        
        while (battleContinues && monster.estVivant() && player.estVivant()) {
            displayBattle(player, monster);
            
            if (playerTurn) {
                cout << "\nYour turn! Choose action:\n"
                     << "1. Attack (Build combo)\n"
                     << "2. Special Move (Requires 3+ combo points)\n"
                     << "3. Block Stance\n"
                     << "4. Use Item\n"
                     << "5. Try to Run\n"
                     << "Choice: ";
                
                int choice;
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                clearScreen();
                
                switch(choice) {
                    case 1: {
                        float damage = player.attaque(monster);
                        cout << "You attack!\n";
                        monster.subitDegat(damage);
                        player.setBlocking(false);
                        break;
                    }
                    case 2: {
                        string result = player.performHeroSpecialMove(monster);
                        cout << "Special Move: " << result << "!\n";
                        player.setBlocking(false);
                        break;
                    }
                    case 3: {
                        cout << "You take a defensive stance!\n";
                        player.setBlocking(true);
                        break;
                    }
                    case 4: {
                        auto inventory = player.getInventoryList();
                        if (inventory.empty()) {
                            cout << "No items in inventory!\n";
                        } else {
                            cout << "Choose item to use (1-" << inventory.size() << "): ";
                            int itemChoice;
                            cin >> itemChoice;
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            if (itemChoice > 0 && itemChoice <= inventory.size()) {
                                player.useItem(itemChoice - 1);
                            } else {
                                cout << "Invalid item choice!\n";
                            }
                        }
                        player.setBlocking(false);
                        break;
                    }
                    case 5: {
                        if (rand() % 4 == 0) {
                            cout << "You successfully ran away!\n";
                            battleContinues = false;
                        } else {
                            cout << "Couldn't escape!\n";
                            float damage = monster.attaque(player);
                            cout << "\n" << monster.getName() << " attacks from behind!\n";
                            player.subitDegat(damage*1.3);
                        }
                        player.setBlocking(false);
                        break;
                    }
                    default:
                        cout << "Invalid choice! Turn skipped.\n";
                        player.setBlocking(false);
                }
                
                pause(1000);
            } else {
                // Monster's turn
                if (rand() % 4 == 0) { // 25% chance for special move
                    string moveName = monster.performSpecialMove(player);
                    cout << "\n" << monster.getName() << " uses " << moveName << "!\n";
                } else {
                    float damage = monster.attaque(player);
                    cout << "\n" << monster.getName() << " attacks!\n";
                    player.subitDegat(damage);
                }
                
                // Update status effects and active items
                player.updateStatusEffects();
                player.updateActiveItems();
                monster.updateStatusEffects();
                
                pause(1000);
            }
            
            playerTurn = !playerTurn;
        }
        
        if (!monster.estVivant() && battleContinues) {
            cout << "\nVictory! You defeated the " << monster.getName() << "!\n";
            
            // Calculate XP with bonus for elemental monsters
            float xpGained = 30 + (monster.getNiveau() * 5);
            if (monster.getType() != MonsterType::NORMAL) {
                xpGained *= 1.2f;
            }
            player.addXP(xpGained);
            monstersDefeated++;
            
            // Random item drop (50% chance)
            if (rand() % 2 == 0) {
                Item droppedItem = itemTemplates[rand() % itemTemplates.size()];
                droppedItem.quantity = 1;
                player.addItem(droppedItem);
            }
            
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    }
    
    clearScreen();
    cout << R"(
 ██████   █████  ███    ███ ███████      ██████  ██    ██ ███████ ██████                     ██████  
██       ██   ██ ████  ████ ██          ██    ██ ██    ██ ██      ██   ██                 ██      ██ 
██   ███ ███████ ██ ████ ██ █████       ██    ██ ██    ██ █████   ██████                      █████  
██    ██ ██   ██ ██  ██  ██ ██          ██    ██  ██  ██  ██      ██   ██                 ██      ██ 
 ██████  ██   ██ ██      ██ ███████      ██████    ████   ███████ ██   ██                    ██████  
    )" << "\n\n";
    
    cout << "Final Statistics:\n"
         << "Monsters Defeated: " << monstersDefeated << "\n"
         << "Final Level: " << player.getNiveau() << "\n"
         << "Successful Blocks: " << player.getSuccessfulBlocks() << "\n\n";
    
    return 0;
}
