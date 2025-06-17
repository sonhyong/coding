#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <map>
#include <iomanip>

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printHeader(const std::string& title) {
    std::cout << "==============================\n";
    std::cout << "  " << title << "\n";
    std::cout << "==============================" << std::endl;
}

void drawBar(const std::string& label, int value, int max) {
    int width = 20;
    int filled = static_cast<int>((static_cast<double>(value) / max) * width);
    std::cout << label << " [";
    for (int i = 0; i < width; ++i) {
        if (i < filled)
            std::cout << '#';
        else
            std::cout << '-';
    }
    std::cout << "] " << value << "/" << max << std::endl;
}

struct Item {
    std::string name;
    int heal;
};

enum class Job { Warrior, Mage, Rogue };

class Character {
public:
    std::string name;
    int hp;
    int maxHp;
    int mp;
    int maxMp;
    int attackPower;

    Character(const std::string& n, int h, int m, int atk)
        : name(n), hp(h), maxHp(h), mp(m), maxMp(m), attackPower(atk) {}

    bool isAlive() const { return hp > 0; }

    virtual int attack() const {
        return attackPower;
    }

    void takeDamage(int dmg) {
        hp -= dmg;
        if (hp < 0) hp = 0;
    }
};

class Player : public Character {
public:
    int gold = 0;
    int exp = 0;
    std::vector<Item> inventory;
    Job job;
    bool defending = false;
    int skill1Cd = 0;
    int skill2Cd = 0;
    int buffTurns = 0;
    int shieldTurns = 0;
    bool dodgeNext = false;

    Player(const std::string& n, Job j)
        : Character(n, 60, 30, j == Job::Warrior ? 12 : j == Job::Mage ? 8 : 10),
          job(j) {
        std::srand((unsigned)std::time(nullptr));
    }

    int attack() const override {
        int dmg = attackPower + (std::rand() % 5 - 2);
        if (buffTurns > 0) dmg += 5;
        return dmg;
    }

    int useSkill(int num) {
        if (num == 1) {
            if (skill1Cd > 0) {
                std::cout << "\uc544\uc9c1 \uc0ac\uc6a9\ud560 \uc218 \uc5c6\uc2b5\ub2c8\ub2e4. " << skill1Cd << " \ud134 \ub0a8\uc558\uc2b5\ub2c8\ub2e4." << std::endl;
                return 0;
            }
            if (job == Job::Warrior && mp >= 5) {
                mp -= 5;
                skill1Cd = 2;
                std::cout << "\ud30c\uc6b0\uc5b4 \uc2a4\ud2b8\ub77c\uc774\ud06c!" << std::endl;
                return attackPower * 2;
            } else if (job == Job::Mage && mp >= 8) {
                mp -= 8;
                skill1Cd = 1;
                int dmg = 14 + std::rand() % 5;
                std::cout << "\ud30c\uc774\uc5b4\ubcfc!" << std::endl;
                return dmg;
            } else if (job == Job::Rogue && mp >= 4) {
                mp -= 4;
                skill1Cd = 1;
                std::cout << "\ubc31\uc2a4\ud0c0\ube0c!" << std::endl;
                return attackPower * 1.5 + (std::rand() % 5);
            }
        } else if (num == 2) {
            if (skill2Cd > 0) {
                std::cout << "\uc544\uc9c1 \uc0ac\uc6a9\ud560 \uc218 \uc5c6\uc2b5\ub2c8\ub2e4. " << skill2Cd << " \ud134 \ub0a8\uc558\uc2b5\ub2c8\ub2e4." << std::endl;
                return 0;
            }
            if (job == Job::Warrior && mp >= 8) {
                mp -= 8;
                buffTurns = 3;
                skill2Cd = 5;
                std::cout << "\ubd88\ubaa8\uae30! \uc804\ud22c\ubc95\uc744 \uac15\ud654\ud569\ub2c8\ub2e4." << std::endl;
            } else if (job == Job::Mage && mp >= 10) {
                mp -= 10;
                hp += 15;
                if (hp > maxHp) hp = maxHp;
                skill2Cd = 3;
                shieldTurns = 2;
                std::cout << "\ub9c8\ubc95\ubc29\uc0ac!" << std::endl;
            } else if (job == Job::Rogue && mp >= 6) {
                mp -= 6;
                dodgeNext = true;
                skill2Cd = 4;
                std::cout << "\uc5f4\ub9b9\ud0c0! \ub2e4\uc74c \ud134\uc758 \acf5\uaca9\uc744 \ud68c\ud53c\ud569\ub2c8\ub2e4." << std::endl;
            }
        }
        if (mp <= 0) std::cout << "MP\uac00 \ubd80\uc871\ud569\ub2c8\ub2e4." << std::endl;
        return 0;
    }

    void defend() { defending = true; }

    void takeDamage(int dmg) {
        if (dodgeNext) {
            std::cout << "\uacf5\uaca9\uc744 \ud68c\ud53c\ud588\uc2b5\ub2c8\ub2e4!" << std::endl;
            dodgeNext = false;
            return;
        }
        if (defending) {
            dmg /= 2;
            defending = false;
        }
        if (shieldTurns > 0) {
            dmg /= 2;
        }
        Character::takeDamage(dmg);
    }

    void newTurn() {
        if (skill1Cd > 0) --skill1Cd;
        if (skill2Cd > 0) --skill2Cd;
        if (buffTurns > 0) --buffTurns;
        if (shieldTurns > 0) --shieldTurns;
    }

    void gainExp(int amount) {
        exp += amount;
    }

    void gainGold(int amount) {
        gold += amount;
    }

    void addItem(const Item& item) {
        inventory.push_back(item);
    }

    void useItem(int index) {
        if (index < 0 || index >= static_cast<int>(inventory.size())) {
            std::cout << "\uc798\ubabb\ub41c \uc120\ud0dd\uc785\ub2c8\ub2e4." << std::endl;
            return;
        }
        const Item& item = inventory[index];
        if (item.heal > 0) {
            hp += item.heal;
            if (hp > maxHp) hp = maxHp;
            std::cout << item.name << "\uc744 \uc0ac\uc6a9\ud558\uc5ec " << item.heal << " \ud68c\ubcf5\ud588\uc2b5\ub2c8\ub2e4." << std::endl;
        }
        inventory.erase(inventory.begin() + index);
    }
};

class Enemy : public Character {
public:
    std::string type;
    bool charging = false;
    Enemy(const std::string& t, const std::string& n, int h, int atk)
        : Character(n, h, 0, atk), type(t) {
        std::srand((unsigned)std::time(nullptr));
    }

    int attack(Player& player) {
        int dmg = attackPower;
        int roll = std::rand() % 100;
        if (type == "Goblin") {
            if (roll < 50) {
                std::cout << "\uace0\ube14\ub9b0\uc774 \ud754\ub4e4\uc5b4 \ub450 \ubc88 \uacf5\uaca9\ud569\ub2c8\ub2e4!" << std::endl;
                player.takeDamage(attackPower);
                dmg += attackPower;
            }
        } else if (type == "Orc") {
            if (charging) {
                std::cout << "\uc624\ud06c\uac00 \ucc28\uc9c0 \uacf5\uaca9!" << std::endl;
                charging = false;
                dmg *= 2;
            } else if (roll < 30) {
                std::cout << "\uc624\ud06c\uac00 \uacf5\uaca9\uc744 \uc900\ube44\ud558\ub294\uad6c\ub098!" << std::endl;
                charging = true;
            }
        } else if (type == "Troll") {
            if (roll < 20) {
                std::cout << "\ud2b8\ub864\uc774 \uc2a4\uc18c\ub97c \uce58\uc720\ud569\ub2c8\ub2e4." << std::endl;
                hp += 5;
                if (hp > maxHp) hp = maxHp;
            }
        } else if (type == "Dragon") {
            if (roll < 30) {
                std::cout << "\ub4dc\ub798\uace4\uc758 \ud654\uc5fc\uc0ac\uc720!" << std::endl;
                dmg = 15 + std::rand() % 6;
            }
        }
        return dmg;
    }
};

struct Area {
    std::string name;
    std::vector<std::string> enemies;
};

struct Quest {
    std::string description;
    int target;
    int progress = 0;
    bool completed = false;
};

void displayMap(const std::vector<std::vector<Area>>& map, int x, int y) {
    for (size_t j = 0; j < map.size(); ++j) {
        for (size_t i = 0; i < map[j].size(); ++i) {
            if (static_cast<int>(i) == x && static_cast<int>(j) == y)
                std::cout << "[P]";
            else
                std::cout << "[ ]";
        }
        std::cout << std::endl;
    }
    std::cout << "\ud604\uc7ac\uc704\uce58: " << map[y][x].name << std::endl;
}

Enemy createEnemy(const std::string& type) {
    if (type == "Goblin") return Enemy("Goblin", "\uace0\ube14\ub9b0", 25, 7);
    if (type == "Orc") return Enemy("Orc", "\uc624\ud06c", 35, 10);
    if (type == "Troll") return Enemy("Troll", "\ud2b8\ub864", 40, 8);
    if (type == "Dragon") return Enemy("Dragon", "\ub4dc\ub798\uace4", 60, 12);
    return Enemy("Goblin", "\uace0\ube14\ub9b0", 25, 7);
}

bool battle(Player& player, Enemy enemy, Quest& quest) {
    while (player.isAlive() && enemy.isAlive()) {
        player.newTurn();
        clearScreen();
        printHeader(enemy.name + " \ubc29\uc120");
        drawBar(player.name + " HP", player.hp, player.maxHp);
        drawBar("MP", player.mp, player.maxMp);
        drawBar(enemy.name + " HP", enemy.hp, enemy.maxHp);
        std::cout << "------------------------------" << std::endl;
        std::cout << "1) \uacf5\uaca9  2) \uc2a4\ud0ac1  3) \uc2a4\ud0ac2  4) \ubc29\uc5b4  5) \uc544\uc774\ud15c" << std::endl;
        std::cout << "> ";
        int choice;
        std::cin >> choice;
        if (choice == 1) {
            int dmg = player.attack();
            enemy.takeDamage(dmg);
            std::cout << "\uc801\uc5d0\uac8c " << dmg << "\uc758 \ud53c\ud574\ub97c \uc8fc\uc5c8\uc2b5\ub2c8\ub2e4." << std::endl;
        } else if (choice == 2) {
            int dmg = player.useSkill(1);
            if (dmg > 0) {
                enemy.takeDamage(dmg);
                std::cout << "\uc801\uc5d0\uac8c " << dmg << " \ud53c\ud574!" << std::endl;
            }
        } else if (choice == 3) {
            player.useSkill(2);
        } else if (choice == 4) {
            player.defend();
            std::cout << "\ubc29\uc5b4 \ud0dc\uc138\ub97c \ucde8\ud569\ub2c8\ub2e4." << std::endl;
        } else if (choice == 5) {
            if (player.inventory.empty()) {
                std::cout << "\uc0ac\uc6a9\ud560 \uc544\uc774\ud15c\uc774 \uc5c6\uc2b5\ub2c8\ub2e4." << std::endl;
            } else {
                std::cout << "\uc544\uc774\ud15c \ubaa9\ub85d:" << std::endl;
                for (size_t i = 0; i < player.inventory.size(); ++i) {
                    std::cout << i << ": " << player.inventory[i].name << std::endl;
                }
                std::cout << "\uc0ac\uc6a9\ud560 \uc544\uc774\ud15c \ubc88\ud638\ub97c \uc785\ub825\ud558\uc138\uc694: ";
                int idx;
                std::cin >> idx;
                player.useItem(idx);
            }
        } else {
            std::cout << "\uc798\ubabb\ub41c \uc785\ub825\uc785\ub2c8\ub2e4. \ud130\ub110\ub97c \uc18c\ubaa8\ud588\uc2b5\ub2c8\ub2e4." << std::endl;
        }

        if (enemy.isAlive()) {
            int dmg = enemy.attack(player);
            player.takeDamage(dmg);
            std::cout << "\uc801\uc774 " << dmg << "\uc758 \ud53c\ud574\ub97c \uc8fc\uc5c8\uc2b5\ub2c8\ub2e4." << std::endl;
        }
    }

    if (player.isAlive()) {
        clearScreen();
        printHeader("\uc2b9\ub9ac!");
        player.gainExp(10);
        player.gainGold(5);
        player.addItem({"\uce58\uc720 \ubb3c\uc57d", 10});
        if (!quest.completed && enemy.type != "Dragon") {
            quest.progress++;
            if (quest.progress >= quest.target) {
                quest.completed = true;
                std::cout << "\ud034\uc2a4\ud2b8 \uc644\ub8cc! \ucd94\uac00 \ubc18\uc751\uc73c\ub85c \uace8\ub4dc 20\uc744 \ud68d\ub4dd\ud588\uc2b5\ub2c8\ub2e4." << std::endl;
                player.gainGold(20);
            }
        }
    } else {
        clearScreen();
        printHeader("\ud30c\ubd88...");
    }
    return player.isAlive();
}

int main() {
    std::cout << "\ud604\uc7ac \ub9c8\uc9c0\ub9c9 \ub4dc\ub808\uace4\uc758 \ubd88\uc5b4\ub2e4 \ub0a8\uc740 \uc138\uacc4\ub97c \uad6c\ud558\uae30 \uc704\ud574 \ubc29\ud5a5\uc744 \ucd95\ubcf5\ud558\uba70 \uc2dc\uc791\ud569\ub2c8\ub2e4." << std::endl;
    std::cout << "\uc601\uc6c5\uc758 \uc774\ub984\uc744 \uc785\ub825\ud558\uc138\uc694: ";
    std::string name;
    std::getline(std::cin, name);
    clearScreen();
    printHeader("\uc9c1\uc5c5 \uc120\ud0dd");
    std::cout << "1) \uc804\uc0ac\n2) \ub9c8\ubc95\uc0ac\n3) \ub3c4\uc801" << std::endl;
    std::cout << "> ";
    int jobSel;
    std::cin >> jobSel;
    Job job = Job::Warrior;
    if (jobSel == 2) job = Job::Mage;
    else if (jobSel == 3) job = Job::Rogue;
    Player player(name, job);

    Quest quest{"\uc232\uc5d0\uc11c \ubaa8\ub378\ud130 3\ub9c8\ub9ac\ub97c \ucc98\uce58\ud574\ub77c", 3};

    std::vector<std::vector<Area>> map = {
        {{"\ub9c8\uc744", {}} , {"\ud3c9\uc6d0", {"Goblin"}}, {"\uc232 \uc785\uad6c", {"Goblin"}}},
        {{"\uc232", {"Goblin", "Orc"}}, {"\ub3d9\uad74", {"Orc"}}, {"\ud638\uc218", {"Troll"}}},
        {{"\uc0b0\uae38", {"Troll"}}, {"\ud654\uc0b0 \uc785\uad6c", {"Troll"}}, {"\ud654\uc0b0", {"Dragon"}}}
    };

    int posX = 0;
    int posY = 0;

    bool running = true;
    while (running && player.isAlive()) {
        clearScreen();
        Area& area = map[posY][posX];
        printHeader(area.name);
        std::cout << "\uace8\ub4dc: " << player.gold << "  \uacbd\ud5d8\uce58: " << player.exp << std::endl;
        if (!quest.completed)
            std::cout << "\ud034\uc2a4\ud2b8: " << quest.description << " (" << quest.progress
                      << "/" << quest.target << ")" << std::endl;
        std::cout << "------------------------------" << std::endl;
        std::cout << "1. \uc774\ub3d9  2. \uc804\ud22c  3. \uc778\ubca4\ud1a0\ub9ac \ud655\uc778  4. \ud734\uc2dd  5. \uc885\ub8cc" << std::endl;
        std::cout << "> ";
        int menu;
        std::cin >> menu;
        if (menu == 1) {
            clearScreen();
            printHeader("\uc774\ub3d9");
            displayMap(map, posX, posY);
            std::cout << "W/A/S/D? ";
            char dir; std::cin >> dir;
            if (dir == 'w' || dir == 'W') { if (posY > 0) posY--; }
            else if (dir == 's' || dir == 'S') { if (posY + 1 < static_cast<int>(map.size())) posY++; }
            else if (dir == 'a' || dir == 'A') { if (posX > 0) posX--; }
            else if (dir == 'd' || dir == 'D') { if (posX + 1 < static_cast<int>(map[0].size())) posX++; }
        } else if (menu == 2) {
            if (area.enemies.empty()) {
                std::cout << "\uc774\uacf3\uc5d0\ub294 \uc804\ud22c\ud560 \uc801\uc774 \uc5c6\uc2b5\ub2c8\ub2e4." << std::endl;
            } else {
                Enemy enemy = createEnemy(area.enemies[std::rand() % area.enemies.size()]);
                bool alive = battle(player, enemy, quest);
                if (!alive) {
                    running = false;
                } else if (enemy.type == "Dragon") {
                    std::cout << "\ub4dc\ub798\uace4\uc744 \ubb3c\ub824 \uc138\uacc4\uc744 \ad6c\ud588\uc2b5\ub2c8\ub2e4!" << std::endl;
                    running = false;
                }
            }
        } else if (menu == 3) {
            if (player.inventory.empty()) {
                std::cout << "\uc778\ubca4\ud1a0\ub9ac\uac00 \ube44\uc5b4 \uc788\uc2b5\ub2c8\ub2e4." << std::endl;
            } else {
                std::cout << "\uc778\ubca4\ud1a0\ub9ac:" << std::endl;
                for (size_t i = 0; i < player.inventory.size(); ++i) {
                    std::cout << i << ": " << player.inventory[i].name << std::endl;
                }
            }
        } else if (menu == 4) {
            player.hp = player.maxHp;
            std::cout << "\uc644\uc804\ud788 \ud68c\ubcf5\ub418\uc5c8\uc2b5\ub2c8\ub2e4." << std::endl;
        } else if (menu == 5) {
            running = false;
        } else {
            std::cout << "\uc798\ubabb\ub41c \uc785\ub825\uc785\ub2c8\ub2e4." << std::endl;
        }
    }

    printHeader("\uac8c\uc784\uc744 \uc885\ub8cc\ud569\ub2c8\ub2e4.");
    return 0;
}

