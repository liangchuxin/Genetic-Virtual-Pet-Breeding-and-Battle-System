#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <algorithm>

#include "../boss/Boss.h"
#include "../common.h"
#include "../pet/Pet.h"
#include "../skill/Skill.h"
#pragma once

using namespace std;

// forward declarations
class TestPetSkill : public PetSkill {
   public:
    string getName() const override { return "Test Skill"; }
    int getEnergyCost() const override { return 2; }
    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        return user->getEntity()->getAttack() -
               target->getEntity()->getDefense();
    }
    void applyEffects(BattleEntityState* user, BattleEntityState* target)
        override { /* Apply skill effects */ }
};

class TestPet : public Pet {
   private:
    vector<StatusEffect*> statusEffects;
    mutable vector<PetSkill*> cachedSkills;  // 写死的三个默认自带技能

   public:
    TestPet() : Pet(10, 5, 7, 100) {}

    ~TestPet() {
        // 清空buff 和技能
        for (StatusEffect* effect : statusEffects) {
            delete effect;
        }
        for (PetSkill* skill : cachedSkills) {
            delete skill;
        }
    }
    int getAttack() const override { return 10; }
    int getBaseAttack() const override { return 10; }
    int getBaseDefense() const override { return 5; }
    int getBaseSpeed() const override { return 7; }
    int getDefense() const override { return 5; }
    int getSpeed() const override { return 7; }
    int getMaxHP() const override { return 100; }
    void addStatusEffect(StatusEffect* effect) override {
        statusEffects.push_back(effect);
    }
    string getName() const override { return "Test Pet"; }

    // 显示自带技能（前面写死的）
    vector<PetSkill*> getAvailableSkills() const override {
        if (cachedSkills.empty()) {
            cachedSkills.push_back(new PowerStrike());
            cachedSkills.push_back(new LifeDrain());
            cachedSkills.push_back(new HeavyStrike());
        }
        return cachedSkills;
    }

    void removeStatusEffect(const string& effectName) override {
        statusEffects.erase(
            remove_if(statusEffects.begin(), statusEffects.end(),
                      [&effectName](StatusEffect* effect) {
                          if (effect->getName() == effectName) {
                              delete effect;
                              return true;
                          }
                          return false;
                      }),
            statusEffects.end());
    }
    void tickStatusEffects() override {
        for (auto it = statusEffects.begin(); it != statusEffects.end();) {
            StatusEffect* effect = *it;
            if (!effect->tick()) {  // Assuming tick() returns false when effect
                                    // expires
                delete effect;
                it = statusEffects.erase(it);
            } else {
                ++it;
            }
        }
    }

    vector<StatusEffect*> getStatusEffects() const override {
        return statusEffects;
    }
};

class TestBoss : public Boss {
   private:
    vector<BossSkill*> skills;
    vector<StatusEffect*> statusEffects;
    int currentSkillIndex;

   public:
    TestBoss() : currentSkillIndex(0) {
        // hardcoded skills for testing
        skills.push_back(new SimpleAttack());
        skills.push_back(new HeavyBlow());
        skills.push_back(new SimpleAttack());
        skills.push_back(new QuickStrike());
        skills.push_back(new DefensiveStance());  // 每5回合防御一次
        skills.push_back(new SimpleAttack());
        skills.push_back(new ShieldUp());    // 每7回合护盾一次
        skills.push_back(new Intimidate());  // 威吓
    }
    ~TestBoss() {
        for (BossSkill* skill : skills) {
            delete skill;
        }
        for (StatusEffect* effect : statusEffects) {
            delete effect;
        }
    }
    string getName() const override { return "Test Boss"; }
    int getAttack() const override { return 15; }
    int getBaseAttack() const override { return 15; }
    int getBaseDefense() const override { return 10; }
    int getBaseSpeed() const override { return 5; }
    int getDefense() const override { return 10; }
    int getSpeed() const override { return 5; }
    int getMaxHP() const override { return 150; }

    vector<BossSkill*> getAvailableSkills() const override { return skills; }
    BossSkill* getNextSkill() override {
        if (skills.empty()) return nullptr;

        BossSkill* nextSkill = skills[currentSkillIndex];
        currentSkillIndex = (currentSkillIndex + 1) % skills.size();
        return nextSkill;
    }
    void addStatusEffect(StatusEffect* effect) override {
        statusEffects.push_back(effect);
    }

    void removeStatusEffect(const string& effectName) override {
        statusEffects.erase(
            remove_if(statusEffects.begin(), statusEffects.end(),
                      [&effectName](StatusEffect* effect) {
                          if (effect->getName() == effectName) {
                              delete effect;
                              return true;
                          }
                          return false;
                      }),
            statusEffects.end());
    }

    void tickStatusEffects() override {
        for (auto it = statusEffects.begin(); it != statusEffects.end();) {
            StatusEffect* effect = *it;
            if (!effect->tick()) {
                delete effect;
                it = statusEffects.erase(it);
            } else {
                ++it;
            }
        }
    }

    vector<StatusEffect*> getStatusEffects() const override {
        return statusEffects;
    }
};

class PetEntityState : public BattleEntityState {
   private:
    Pet* pet;
    int energy;
    int defaultEnergy;

   public:
    PetEntityState(Pet* pet, int startEnergy = 3)
        : BattleEntityState(pet),
          pet(pet),
          energy(startEnergy),
          defaultEnergy(startEnergy) {}

    void spendEnergy(int amount) {
        if (amount <= energy) {
            energy -= amount;
        } else {
            cout << "Not enough energy!" << endl;
        }
    }
    void restoreEnergy(int amount) override {
        energy += amount;
        cout << " Restored " << amount << " energy! Current: " << energy
             << endl;
    }

    void refreshEnergyForNewTurn() {
        energy += defaultEnergy;  // 每回合加3点，累积
    }
    int getDefaultEnergy() const { return defaultEnergy; }

    void spendAllEnergy() { energy = 0; }
    int getEnergy() const { return energy; }
};

class BossEntityState : public BattleEntityState {
   private:
    Boss* boss;
    int energy;

   public:
    BossEntityState(Boss* boss, int maxEnergy = 3)
        : BattleEntityState(boss), boss(boss) {}
};

class Battle {
   private:
    int currentTurn;
    int maxTurns;  // -1 无限回合
    Pet* player;
    Boss* boss;
    PetEntityState* playerState;
    BossEntityState* bossState;
    RandomSkillManager* skillManager;
    bool ownsPlayer = false;
    bool ownsBoss = false;

    // 核心战斗部分
    void battleLoop() {
        while (playerState->isAlive() && bossState->isAlive()) {
            cout << player->getName() << " vs " << boss->getName() << endl;
            cout << "\n" << string(50, '=') << endl;
            cout << "TURN " << getCurrentTurn() << endl;
            cout << string(50, '=') << endl;

            startNewTurn();

            showBattleStatus();

            playerTurn();

            // Check if boss is defeated
            if (!bossState->isAlive()) {
                cout << "\n VICTORY! " << boss->getName()
                     << " has been defeated!" << endl;
                break;
            }

            bossTurn();

            if (!playerState->isAlive()) {
                cout << "\nDEFEAT! " << player->getName() << " has fallen..."
                     << endl;
                break;
            }
            cout << "\n" << string(50, '-') << endl;
            cout << "  Press ENTER to continue to next turn...";
            cin.ignore(1000, '\n');  // 清理输入缓冲区
            string dummy;
            getline(cin, dummy);  // 等待玩家按回车

            // 准备下一回合
            nextTurn();
        }
        endBattle();
    }
    void startNewTurn() {
        if (getCurrentTurn() > 1) {
            playerState->refreshEnergyForNewTurn();
        }  // 加上默认的 n 点能量
        skillManager->refreshSkills();
        cout << "New turn begins!" << endl;
    }

    void nextTurn() { currentTurn++; }

    int getCurrentTurn() { return currentTurn; }
    bool isMaxTurnsReached() const {
        return maxTurns > 0 && currentTurn >= maxTurns;
    }

    void showInitialStats() {
        cout << "\nBattle Participants:" << endl;
        cout << "Player:  " << player->getName()
             << " - ATK:" << player->getAttack()
             << " DEF:" << player->getDefense()
             << " HP:" << playerState->getCurrentHP() << "/"
             << player->getMaxHP() << endl;

        cout << "Boss: " << boss->getName() << " - ATK:" << boss->getAttack()
             << " DEF:" << boss->getDefense()
             << " HP:" << bossState->getCurrentHP() << "/" << boss->getMaxHP()
             << endl;
    }
    void showBattleStatus() {
        cout << "\n" << string(60, '=') << endl;
        cout << " BATTLE STATUS" << endl;
        cout << string(60, '=') << endl;

        // 玩家状态 - 突出显示
        cout << " " << player->getName() << endl;
        cout << "   HP: " << playerState->getCurrentHP() << "/"
             << player->getMaxHP();

        // 显示HP条（可选的视觉效果）
        int hpPercent =
            (playerState->getCurrentHP() * 100) / player->getMaxHP();
        cout << " [";
        for (int i = 0; i < 20; i++) {
            if (i < hpPercent / 5) {
                cout << "";
            } else {
                cout << "";
            }
        }
        cout << "] " << hpPercent << "%" << endl;

        cout << "   Energy: " << playerState->getEnergy() << "/"
             << playerState->getDefaultEnergy() << endl;

        // 玩家状态栏
        showEntityStatus(playerState, player->getName());

        cout << endl;

        // Boss状态
        cout << " " << boss->getName() << endl;
        cout << "   HP: " << bossState->getCurrentHP() << "/"
             << boss->getMaxHP();

        // Boss HP条
        int bossHpPercent =
            (bossState->getCurrentHP() * 100) / boss->getMaxHP();
        cout << " [";
        for (int i = 0; i < 20; i++) {
            if (i < bossHpPercent / 5) {
                cout << "";
            } else {
                cout << "";
            }
        }
        cout << "] " << bossHpPercent << "%" << endl;

        // Boss状态栏
        showEntityStatus(bossState, boss->getName());

        cout << string(60, '=') << endl;
    }

    void showEntityStatus(BattleEntityState* entityState,
                          const string& entityName) {
        vector<string> statusLines;

        // 护盾状态
        int shieldAmount = entityState->getShieldAmount();
        if (shieldAmount > 0) {
            statusLines.push_back(" Shield: " + to_string(shieldAmount));
        }

        // 获取状态效果并分类
        vector<StatusEffect*> effects = entityState->getStatusEffects();
        vector<string> buffs, debuffs, neutrals;

        for (StatusEffect* effect : effects) {
            string effectInfo = effect->getName();
            if (effect->getDuration() > 0) {
                effectInfo +=
                    " (" + to_string(effect->getDuration()) + " turns)";
            }

            // 根据效果类型分类（基于名称和修饰符）
            if (isBuffEffect(effect)) {
                buffs.push_back(" " + effectInfo);
            } else if (isDebuffEffect(effect)) {
                debuffs.push_back(" " + effectInfo);
            } else {
                neutrals.push_back(" " + effectInfo);
            }
        }

        // 添加分类后的效果到状态行
        for (const string& buff : buffs) {
            statusLines.push_back(buff);
        }
        for (const string& debuff : debuffs) {
            statusLines.push_back(debuff);
        }
        for (const string& neutral : neutrals) {
            statusLines.push_back(neutral);
        }

        // 显示状态（如果有的话）
        if (!statusLines.empty()) {
            cout << "   Status: ";
            for (size_t i = 0; i < statusLines.size(); i++) {
                if (i == 0) {
                    cout << statusLines[i];
                } else {
                    cout << "\n             " << statusLines[i];  // 对齐缩进
                }
            }
            cout << endl;
        }
    }
    // 判断是否为增益效果
    bool isBuffEffect(StatusEffect* effect) {
        string name = effect->getName();

        // 根据名称判断
        vector<string> buffNames = {"Defense Boost",     "Guardian Stance",
                                    "Protective Stance", "Energy Surge",
                                    "Adrenaline Rush",   "Menacing Aura"};

        for (const string& buffName : buffNames) {
            if (name == buffName) return true;
        }

        // 根据修饰符判断（如果有正面的攻击/防御/速度加成）
        if (effect->getModifier("attack") > 0 ||
            effect->getModifier("defense") > 0 ||
            effect->getModifier("speed") > 0) {
            return true;
        }

        return false;
    }

    // 判断是否为减益效果
    bool isDebuffEffect(StatusEffect* effect) {
        string name = effect->getName();

        // 根据名称判断
        vector<string> debuffNames = {
            "Defense Break", "Armor Break", "Armor Crack",    "Stunned",
            "Intimidated",   "Fear",        "Minor Bleeding", "Disoriented"};

        for (const string& debuffName : debuffNames) {
            if (name == debuffName) return true;
        }

        // 根据修饰符判断（如果有负面的攻击/防御/速度减少）
        if (effect->getModifier("attack") < 0 ||
            effect->getModifier("defense") < 0 ||
            effect->getModifier("speed") < 0 ||
            effect->getModifier("hp_per_turn") < 0) {
            return true;
        }

        return false;
    }

    void playerTurn() {
        cout << "\nYour turn! Choose your action:" << endl;

        while (playerState->getEnergy() > 0) {
            showPlayerOptions();

            int choice = getPlayerChoice();
            bool actionTaken = executePlayerAction(choice);

            if (!actionTaken) {
                continue;  // 重新选择
            }
            skillManager->checkForExtraSkillDraw(playerState);

            // 检查是否还有能量
            if (playerState->getEnergy() > 0) {
                showBattleStatus();
                cout << "\nYou have " << playerState->getEnergy()
                     << " energy left. Continue? (y/n): ";
                char continueChoice;
                cin >> continueChoice;
                if (continueChoice != 'y' && continueChoice != 'Y') {
                    break;
                }
            }
        }

        cout << "\nPlayer turn ended." << endl;
    }

    void showPlayerOptions() {
        cout << "\nAvailable Actions:" << endl;

        // 显示三个宠物自带技能
        vector<PetSkill*> petSkills = player->getAvailableSkills();
        if (!petSkills.empty()) {
            cout << "\nPet Skills:" << endl;
            for (size_t i = 0; i < petSkills.size(); i++) {
                cout << "[" << (i + 3) << "] " << petSkills[i]->getName()
                     << " - " << petSkills[i]->getDescription() << " ("
                     << petSkills[i]->getEnergyCost() << " energy)" << endl;
            }
        }

        // 然后显示随机技能库技能
        vector<PetSkill*> randomSkills = skillManager->getCurrentSkills();
        if (!randomSkills.empty()) {
            cout << "\nRandom Skills (this turn only):" << endl;
            int startIndex = 3 + petSkills.size();
            for (size_t i = 0; i < randomSkills.size(); i++) {
                cout << "[" << (startIndex + i) << "] "
                     << randomSkills[i]->getName() << " - "
                     << petSkills[i]->getDescription() << " ("
                     << randomSkills[i]->getEnergyCost() << " energy)" << endl;
            }
        }

        cout << "\n0. Skip remaining actions" << endl;
        cout << "Choice: ";
    }

    int getPlayerChoice() {
        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input! Please enter a number: ";
            return getPlayerChoice();
        }

        return choice;
    }
    bool confirmSkillUsage(PetSkill* skill, int energyCost, int currentEnergy) {
        cout << "\n" << string(60, '-') << endl;
        cout << " Skill Details:" << endl;
        cout << "Name: " << skill->getName() << endl;
        cout << "Cost: " << energyCost << " energy (Current: " << currentEnergy
             << ")" << endl;
        cout << "Description: " << skill->getDetailedDescription() << endl;
        cout << string(60, '-') << endl;

        cout << "Use this skill? (y/n): ";
        char confirm;
        cin >> confirm;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            return false;
        }

        return (confirm == 'y' || confirm == 'Y');
    }

    bool executePlayerAction(int choice) {
        if (choice == 0) {
            playerState->spendAllEnergy();
            return true;
        }

        // 获取技能列表
        vector<PetSkill*> petSkills = player->getAvailableSkills();
        vector<PetSkill*> randomSkills = skillManager->getCurrentSkills();

        PetSkill* selectedSkill = nullptr;

        // 根据选择找到技能
        int petSkillsCount = static_cast<int>(petSkills.size());
        int randomSkillsCount = static_cast<int>(randomSkills.size());

        // 根据选择找到技能
        if (choice >= 3 && choice < 3 + petSkillsCount) {
            selectedSkill = petSkills[choice - 3];
        } else if (choice >= 3 + petSkillsCount &&
                   choice < 3 + petSkillsCount + randomSkillsCount) {
            selectedSkill = randomSkills[choice - 3 - petSkillsCount];
        }
        if (selectedSkill) {
            int cost = selectedSkill->getEnergyCost();
            bool confirmed = confirmSkillUsage(selectedSkill, cost,
                                               playerState->getEnergy());

            if (!confirmed) {
                cout << " Skill usage cancelled." << endl;
                return false;
            }
            if (playerState->getEnergy() >= cost) {
                cout << "Using skill: " << selectedSkill->getName() << endl;

                // 计算伤害
                int damage =
                    selectedSkill->calculateDamage(playerState, bossState);
                bossState->takeDamage(damage);
                cout << "Dealt " << damage << " damage!" << endl;

                // 应用效果
                selectedSkill->applyEffects(playerState, bossState);

                // 消耗能量
                playerState->spendEnergy(cost);
                return true;
            } else {
                cout << "Not enough energy!" << endl;
                return false;
            }
        }

        cout << "Invalid choice!" << endl;
        return false;
    }
    void bossTurn() {
        cout << "\n " << boss->getName() << "'s turn:" << endl;

        // 每回合开始时更新Boss的状态效果
        bossState->tickStatusEffects();

        // 获取Boss的下一个技能
        BossSkill* bossSkill = boss->getNextSkill();

        if (bossSkill != nullptr) {
            // Boss使用技能
            cout << " " << boss->getName() << " uses " << bossSkill->getName()
                 << "!" << endl;
            cout << "   (" << bossSkill->getDescription() << ")" << endl;

            // 计算技能伤害（使用BossEntityState的最终属性）
            int damage = bossSkill->calculateDamage(bossState, playerState);

            // 对玩家造成伤害（通过playerState）
            if (damage > 0) {
                playerState->takeDamage(damage);
                cout << " " << player->getName() << " takes " << damage
                     << " damage!" << endl;
            }

            // 应用技能的特殊效果（现在可以正确处理状态效果）
            bossSkill->applyEffects(bossState, playerState);
        }

        // 显示当前状态
        cout << " " << player->getName()
             << " HP: " << playerState->getCurrentHP() << "/"
             << player->getMaxHP() << endl;
        cout << " " << boss->getName() << " HP: " << bossState->getCurrentHP()
             << "/" << boss->getMaxHP() << endl;

        // 重置防御状态
        if (playerState->isDefending()) {
            playerState->setDefending(false);
            cout << " Player's defensive stance ends." << endl;
        }

        cout << "Boss turn ended." << endl;
    }

    void endBattle() {
        cout << "\n" << string(50, '=') << endl;
        cout << "BATTLE END!" << endl;
        cout << string(50, '=') << endl;

        cout << "Battle lasted " << getCurrentTurn() << " turns." << endl;

        if (playerState->isAlive()) {
            cout << player->getName() << " emerges victorious!" << endl;
            // 可以添加奖励逻辑
        } else {
            cout << player->getName() << " has been defeated..." << endl;
        }

        cout << "\nFinal Status:" << endl;
        cout << " " << player->getName()
             << " - HP: " << playerState->getCurrentHP() << "/"
             << player->getMaxHP() << endl;
        cout << " " << boss->getName()
             << " - HP: " << bossState->getCurrentHP() << "/"
             << boss->getMaxHP() << endl;
    }

   public:
    // Battle initialisation
    Battle() : currentTurn(1), maxTurns(-1) {
        player = nullptr;
        boss = nullptr;
        playerState = nullptr;
        bossState = nullptr;
        skillManager = new RandomSkillManager();
    }
    ~Battle() {  //
        if (ownsPlayer) delete player;
        if (ownsBoss) delete boss;
        delete playerState;
        delete bossState;
        delete skillManager;
    }
    void start(Pet* selectedPet = nullptr, Boss* selectedBoss = nullptr) {
        cout << "Battle starts!" << endl;

        if (selectedPet) {
            player = selectedPet;
            ownsPlayer = false;
        } else {
            player = new TestPet();
            ownsPlayer = true;
        }

        if (selectedBoss) {
            boss = selectedBoss;
            ownsBoss = false;
        } else {
            boss = new TestBoss();
            ownsBoss = true;
        }

        // 创建状态对象
        playerState = new PetEntityState(player);
        bossState = new BossEntityState(boss);

        battleLoop();
    }
};