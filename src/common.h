#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <map>
#include <string>

#include "skill/StatusEffect.h"

// forward declarations
class StatusEffect;
class PetSkill;
class BossSkill;

enum class Rarity {
    COMMON = 1,
    UNCOMMON = 2,
    RARE = 3,
    EPIC = 4,
    LEGENDARY = 5
};

enum class Species { DOG, CAT, BIRD };

enum class GeneType {
    STAT_GENE,  // numerical gene
    SKILL_GENE  // skill gene
};

class Entity {
   public:
    virtual ~Entity() {}
    virtual int getBaseAttack() const = 0;
    virtual int getBaseDefense() const = 0;
    virtual int getBaseSpeed() const = 0;
    virtual int getAttack() const = 0;
    virtual int getDefense() const = 0;
    virtual int getSpeed() const = 0;
    virtual int getMaxHP() const = 0;
    virtual string getName() const = 0;
    virtual void addStatusEffect(StatusEffect* effect) = 0;
    virtual void removeStatusEffect(const string& effectName) = 0;
    virtual void tickStatusEffects() = 0;  // update effects every turn
    virtual vector<StatusEffect*> getStatusEffects() const = 0;
};

class BattleEntityState {
   private:
    Entity* entity;
    int currentHP;
    int shieldAmount;

    map<string, int> tempModifiers;
    vector<StatusEffect*> activeEffects;

    // turn management
    bool defending;  // 反伤
    int totalActions;
    int skillsUsed;

    bool isPlayer;

   public:
    // initialisation
    BattleEntityState(Entity* ent)
        : entity(ent),
          currentHP(entity->getMaxHP()),  // 满血开始
          shieldAmount(0),
          defending(false),  // 先初始化defending
          totalActions(0),   // 再初始化totalActions
          skillsUsed(0),
          isPlayer(false) {
        // 初始化tempModifiers，避免at()方法出错
        tempModifiers["attack"] = 0;
        tempModifiers["defense"] = 0;
        tempModifiers["speed"] = 0;
    }
    ~BattleEntityState() {
        for (StatusEffect* effect : activeEffects) {
            delete effect;
        }
        activeEffects.clear();
    }
    Entity* getEntity() const { return entity; }
    virtual void restoreEnergy(int amount) {
        cout << " This entity cannot restore energy." << endl;
    }

    void setDefending(bool defending) { this->defending = defending; }
    bool isDefending() const { return defending; }

    vector<StatusEffect*> getStatusEffects() const { return activeEffects; }
    void removeStatusEffect(const string& effectName) {
        for (auto it = activeEffects.begin(); it != activeEffects.end(); ++it) {
            if ((*it)->getName() == effectName) {
                delete *it;
                activeEffects.erase(it);
                cout << "🗑️ Removed effect: " << effectName << endl;
                break;
            }
        }
    }

    void tickStatusEffects() {
        for (auto it = activeEffects.begin(); it != activeEffects.end();) {
            StatusEffect* effect = *it;
            if (!effect
                     ->tick()) {  // 假设StatusEffect有tick()方法返回是否还有效
                delete effect;
                it = activeEffects.erase(it);
            } else {
                ++it;
            }
        }
    }

    void reset() {
        totalActions = 0;
        skillsUsed = 0;
    }

    // skill effects
    void takeDamage(int damage) {
        if (shieldAmount > 0) {
            int blocked = min(shieldAmount, damage);
            shieldAmount -= blocked;
            damage -= blocked;
            cout << " Shield absorbs " << blocked << " damage!" << endl;
        }

        currentHP -= damage;
        if (currentHP < 0) currentHP = 0;
    }
    void heal(int amount) {
        int oldHP = currentHP;
        currentHP = min(entity->getMaxHP(), currentHP + amount);
        int actualHeal = currentHP - oldHP;
        if (actualHeal > 0) {
            cout << "🩹 Healed " << actualHeal << " HP!" << endl;
        }
    }
    void addShield(int amount) {
        shieldAmount += amount;
        cout << " Shield increased by " << amount
             << " (Total: " << shieldAmount << ")" << endl;
    }
    void addStatusEffect(StatusEffect* effect) {
        activeEffects.push_back(effect);
        cout << " Applied " << effect->getName() << " for "
             << effect->getDuration() << " turns!" << endl;
    }

    void addTempModifier(const string& attribute, int value) {
        tempModifiers[attribute] += value;
        cout << "📈 " << attribute << " modified by " << value << endl;
    }
    int getCurrentHP() const { return currentHP; }
    int getShieldAmount() const { return shieldAmount; }
    int getMaxHP() const { return entity->getMaxHP(); }
    bool isAlive() const { return currentHP > 0; }
    int getFinalAttack() const {
        int total = entity->getAttack() + tempModifiers.at("attack");
        for (StatusEffect* effect : activeEffects) {
            total += effect->getModifier("attack");
        }
        return total;
    }

    int getFinalDefense() const {
        int total = entity->getDefense() + tempModifiers.at("defense");
        for (StatusEffect* effect : activeEffects) {
            total += effect->getModifier("defense");
        }

        // 防御状态额外加成
        if (isDefending()) total += entity->getDefense() / 2;

        return total;
    }
    int getFinalSpeed() const {
        int total = entity->getSpeed();
        total += tempModifiers.at("speed");
        for (StatusEffect* effect : activeEffects) {
            total += effect->getModifier("speed");
        }
        return max(0, total);
    }
};


class Gene;
class StatGene;
class SkillGene;
class GeneticPet;
class BreedingSystem;
#endif