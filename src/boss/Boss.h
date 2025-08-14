// Boss class header
#include "../common.h"
#include "../skill/Skill.h"

class Boss : public Entity {
   private:
    vector<BossSkill*> skills;  // 自带既定技能顺序
   public:
    Boss() {}

    virtual ~Boss() {
        for (BossSkill* skill : skills) {
            delete skill;
        }
    }
    virtual int getBaseAttack() const override { return 15; }
    virtual int getBaseDefense() const override { return 10; }
    virtual int getBaseSpeed() const override { return 5; }
    virtual int getAttack() const override { return 15; }
    virtual int getDefense() const override { return 5; }
    virtual int getSpeed() const override { return 7; }
    virtual int getMaxHP() const override { return 30; }
    virtual vector<BossSkill*> getAvailableSkills() const { return skills; }
    virtual BossSkill* getNextSkill() = 0;  // 获取下一个技能
    virtual string getName() const override = 0;
};
