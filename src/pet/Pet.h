// Pet class header

#include <vector>

#include "../common.h"
#include "../skill/Skill.h"
using namespace std;

class Pet : public Entity {
   private:
    vector<PetSkill*> skills;  // 自带技能组
    int attack;
    int defense;
    int speed;
    int maxHP;

   public:
    Pet(int atk, int def, int spd, int hp)
        : attack(atk), defense(def), speed(spd), maxHP(hp) {}

    virtual ~Pet() {
        for (PetSkill* skill : skills) {
            delete skill;
        }
    }
    virtual int getBaseAttack() const override { return attack; }
    virtual int getBaseDefense() const override { return defense; }
    virtual int getBaseSpeed() const override { return speed; }
    virtual int getAttack() const override { return attack; }
    virtual int getDefense() const override { return defense; }
    virtual int getSpeed() const override { return speed; }
    virtual int getMaxHP() const override { return maxHP; }
    virtual vector<PetSkill*> getAvailableSkills() const { return skills; }
    virtual string getName() const override = 0;
    virtual void removeStatusEffect(const string& effectName) override = 0;
    virtual void tickStatusEffects() override = 0;
    virtual vector<StatusEffect*> getStatusEffects() const override = 0;
};