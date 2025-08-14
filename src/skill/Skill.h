#ifndef SKILL_H
#define SKILL_H

#include <map>
#include <vector>

#include "../common.h"
#include <algorithm>  // for find, remove_if
#include <cstdlib>    // for rand
#include <cstdio>     // for sprintf

using namespace std;

class PetEntityState;
class BossEntityState;

// skill interface

class Skill {
   public:
    virtual ~Skill() {}
    virtual string getName() const = 0;
    virtual string getDescription() const = 0;
    virtual string getDetailedDescription() const = 0;
};

class PetSkill : public Skill {
   protected:
    string name;
    string description;
    string detailedDescription;
    int energyCost;

   public:
    PetSkill(const string& skillName, const string& skillDesc,
             const string& detailedDesc, int cost)
        : name(skillName),
          description(skillDesc),
          detailedDescription(detailedDesc),
          energyCost(cost) {}
    string getName() const override { return name; }
    string getDescription() const override { return description; }
    string getDetailedDescription() const override {
        return detailedDescription;
    }
    virtual int getEnergyCost() const { return energyCost; }

    virtual int calculateDamage(BattleEntityState* user,
                                BattleEntityState* target) = 0;
    virtual void applyEffects(BattleEntityState* user,
                              BattleEntityState* target) = 0;
};

// 默认宠物技能

// 技能1：强力攻击 + Debuff
class PowerStrike : public PetSkill {
   public:
    PowerStrike()
        : PetSkill("Power Strike",
                   "Massive damage attack with defense reduction effect",
                   "Deals 350% attack damage to the target and reduces enemy "
                   "defense by 50% of their base defense for 3 turns.",
                   2) {}
    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        int baseDamage = (int)(user->getFinalAttack() * 3.5f);  // 350%
        int defense = target->getFinalDefense() / 3;            // 减少防御效果
        return max(1, baseDamage - defense);
    }

    void applyEffects(BattleEntityState* user,
                      BattleEntityState* target) override {
        // 降低Boss防御50%
        int defenseReduction = target->getEntity()->getBaseDefense() * 0.5f;
        StatusEffect* defenseDown = new StatusEffect("Defense Break", 3);
        defenseDown->addModifier("defense", -defenseReduction);
        target->addStatusEffect(defenseDown);

        cout << " Defense reduced by " << defenseReduction << " for 3 turns!"
             << endl;
    }
};

// 技能2：生命吸取 + 护盾
class LifeDrain : public PetSkill {
   public:
    LifeDrain()
        : PetSkill(
              "Life Drain", "Moderate damage attack with shield generation",
              "Deals 130% attack damage to the target and grants a shield "
              "equal to 104% of attack power that lasts until the next turn.",
              1) {}

    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        int baseDamage = (int)(user->getFinalAttack() * 1.3f);  // 130%
        int defense = target->getFinalDefense() / 3;
        return max(1, baseDamage - defense);
    }

    void applyEffects(BattleEntityState* user,
                      BattleEntityState* target) override {
        // 获得护盾 = 104% 攻击力
        int shieldAmount = (int)(user->getFinalAttack() * 1.04f);
        user->addShield(shieldAmount);

        cout << " Gained " << shieldAmount << " shield!" << endl;
    }
};

// 技能3：基础强化攻击
class HeavyStrike : public PetSkill {
   public:
    HeavyStrike()
        : PetSkill("Heavy Strike", "Large damage attack",
                   "Deals 240% attack damage to the target with no additional "
                   "effects.",
                   1) {}
    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        int baseDamage = (int)(user->getFinalAttack() * 2.4f);  // 240%
        int defense = target->getFinalDefense() / 3;
        return max(1, baseDamage - defense);
    }

    void applyEffects(BattleEntityState* user,
                      BattleEntityState* target) override {
        // 纯伤害技能，无额外效果
        cout << "🔨 Heavy strike lands!" << endl;
    }
};

// 可以随机抽取的全局技能

// 技能1: 狂暴打击 - 扣除当前 2% 血量，造成 690% 攻击力的伤害 (2 能量)
class BerserkerStrike : public PetSkill {
   public:
    BerserkerStrike()
        : PetSkill("Berserker Strike",
                   "Sacrifice health for devastating damage",
                   "Consumes 2% of current HP and deals 690% attack damage to "
                   "the target.",
                   2) {}

    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        // 先扣血作为代价
        int hpCost = max(1, (int)(user->getCurrentHP() * 0.02f));
        user->takeDamage(hpCost);
        cout << " Sacrificed " << hpCost << " HP for berserker power!"
             << endl;

        // 计算巨额伤害
        int baseDamage = (int)(user->getFinalAttack() * 6.9f);  // 690%
        int defense = target->getFinalDefense() / 3;
        return max(1, baseDamage - defense);
    }

    void applyEffects(BattleEntityState* user,
                      BattleEntityState* target) override {
        cout << " BERSERKER RAGE! Overwhelming force unleashed!" << endl;
    }
};

// 技能2: 守护姿态 - 在回合期间提升 20% 的防御 (1 能量)
class GuardianStance : public PetSkill {
   public:
    GuardianStance()
        : PetSkill("Guardian Stance", "Defensive posture with damage reduction",
                   "Activates defending status and increases defense by 20% of "
                   "base defense for 1 turn.",
                   1) {}

    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        return 0;  // 守护技能不造成伤害
    }

    void applyEffects(BattleEntityState* user,
                      BattleEntityState* target) override {
        // 设置守护状态（利用现有系统）
        user->setDefending(true);

        // 额外增加20%防御
        int defenseBonus = (int)(user->getEntity()->getBaseDefense() * 0.2f);
        StatusEffect* defenseBoost = new StatusEffect("Guardian Stance", 1);
        defenseBoost->addModifier("defense", defenseBonus);
        user->addStatusEffect(defenseBoost);

        cout << " Guardian stance activated! Defense increased by "
             << defenseBonus << "!" << endl;
    }
};

// 技能3: 临时护盾 - 获得 364% 防御力的护盾 (在当前回合内有效) (1 能量)
class TemporaryShield : public PetSkill {
   public:
    TemporaryShield()
        : PetSkill("Temporary Shield", "Generate large protective barrier",
                   "Creates a shield equal to 364% of current defense that "
                   "lasts until the next turn.",
                   1) {}

    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        return 0;  // 护盾技能不造成伤害
    }

    void applyEffects(BattleEntityState* user,
                      BattleEntityState* target) override {
        int shieldAmount =
            (int)(user->getFinalDefense() * 3.64f);  // 364%防御力
        user->addShield(shieldAmount);
        cout << " Temporary shield generated! " << shieldAmount
             << " shield points!" << endl;
    }
};

// 技能4: 能量攻击 - 造成 240% 伤害，下回合获得额外 2 点能量 (1 能量)
class EnergyStrike : public PetSkill {
   public:
    EnergyStrike()
        : PetSkill("Energy Strike", "Large damage attack with energy recovery",
                   "Deals 240% attack damage to the target and grants 2 "
                   "additional energy at the start of next turn.",
                   1) {}

    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        int baseDamage = (int)(user->getFinalAttack() * 2.4f);  // 240%
        int defense = target->getFinalDefense() / 3;
        return max(1, baseDamage - defense);
    }

    void applyEffects(BattleEntityState* user,
                      BattleEntityState* target) override {
        // 添加"能量充盈"状态，下回合开始时恢复能量
        StatusEffect* energyBoost = new StatusEffect("Energy Surge", 1);
        energyBoost->addModifier("energy_next_turn", 2);  // 自定义属性
        user->addStatusEffect(energyBoost);

        cout
            << " Energy strike connects! Next turn will start with +2 energy!"
            << endl;
    }
};

// 技能5: 坚固护盾 - 获得 349% 防御力的护盾 (1 能量)
class SolidShield : public PetSkill {
   public:
    SolidShield()
        : PetSkill("Solid Shield", "Generate durable protective barrier",
                   "Creates a shield equal to 349% of current defense that "
                   "persists until destroyed by enemy attacks.",
                   1) {}

    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        return 0;  // 护盾技能不造成伤害
    }

    void applyEffects(BattleEntityState* user,
                      BattleEntityState* target) override {
        int shieldAmount =
            (int)(user->getFinalDefense() * 3.49f);  // 349%防御力
        user->addShield(shieldAmount);
        cout << " Solid shield created! " << shieldAmount << " shield points!"
             << endl;
    }
};

// 技能6: 连击抽卡 - 造成 220% 攻击力的伤害，并再抽取一个技能 (1 能量)
class ComboStrike : public PetSkill {
   public:
    ComboStrike()
        : PetSkill("Combo Strike", "Large damage attack with skill draw",
                   "Deals 220% attack damage to the target and draws one "
                   "additional random skill for the current turn.",
                   1) {}
    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        int baseDamage = (int)(user->getFinalAttack() * 2.2f);
        return max(1, baseDamage - target->getFinalDefense() / 3);
    }

    void applyEffects(BattleEntityState* user,
                      BattleEntityState* target) override {
        cout << "🎯 Combo strike hits! Preparing to draw extra skill..."
             << endl;

        // 直接给一个标记状态效果
        StatusEffect* drawFlag =
            new StatusEffect("Draw Extra Skill", 0);  // 立即处理
        user->addStatusEffect(drawFlag);
    }
};

// 技能7: 生命转能 - 扣除当前 3% 血量，并立刻回复一点能量 (0 能量)
class LifeToEnergy : public PetSkill {
   public:
    LifeToEnergy()
        : PetSkill("Life to Energy", "Convert health into combat energy",
                   "Consumes 3% of current HP and immediately grants 1 energy "
                   "point.",
                   0) {}
    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        return 0;  // 不造成伤害
    }

    void applyEffects(BattleEntityState* user,
                      BattleEntityState* target) override {
        // 扣除3%当前血量
        int hpCost = max(1, (int)(user->getCurrentHP() * 0.03f));
        user->takeDamage(hpCost);

        user->restoreEnergy(1);

        cout << " Lost " << hpCost << " HP" << endl;
        cout << " Gained 1 energy immediately!" << endl;
    }
};

// 技能8: 破甲攻击 - 降低 boss 44% 的防御，持续两回合 (1 能量)
class ArmorBreak : public PetSkill {
   public:
    ArmorBreak()
        : PetSkill("Armor Break", "Small damage attack with armor penetration",
                   "Deals 50% attack damage to the target and reduces enemy "
                   "defense by 44% of their base defense for 2 turns.",
                   1) {}
    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        // 小额伤害，主要是debuff效果
        int baseDamage = (int)(user->getFinalAttack() * 0.5f);  // 50%
        int defense = target->getFinalDefense() / 4;  // 破甲时防御效果减弱
        return max(1, baseDamage - defense);
    }

    void applyEffects(BattleEntityState* user,
                      BattleEntityState* target) override {
        // 降低Boss 44%的基础防御
        int defenseReduction =
            (int)(target->getEntity()->getBaseDefense() * 0.44f);
        StatusEffect* armorBreak =
            new StatusEffect("Armor Break", 2);  // 持续2回合
        armorBreak->addModifier("defense", -defenseReduction);
        target->addStatusEffect(armorBreak);

        cout << "🔨 Armor shattered! Boss defense reduced by "
             << defenseReduction << " for 2 turns!" << endl;
    }
};

class RandomSkillManager {
   private:
    vector<PetSkill*> skillTemplates;  // 技能模板
    vector<PetSkill*> currentSkills;
    int skillsPerTurn;
    PetSkill* createSkillCopy(PetSkill* original) {
        string name = original->getName();

        if (name == "Berserker Strike") return new BerserkerStrike();
        if (name == "Guardian Stance") return new GuardianStance();
        if (name == "Temporary Shield") return new TemporaryShield();
        if (name == "Energy Strike") return new EnergyStrike();
        if (name == "Solid Shield") return new SolidShield();
        if (name == "Combo Strike") return new ComboStrike();
        if (name == "Life to Energy") return new LifeToEnergy();
        if (name == "Armor Break") return new ArmorBreak();

        return nullptr;
    }

   public:
    RandomSkillManager(int skillCount = 3) : skillsPerTurn(skillCount) {
        // 初始化全局技能库
        skillTemplates.push_back(new BerserkerStrike());
        skillTemplates.push_back(new GuardianStance());
        skillTemplates.push_back(new TemporaryShield());
        skillTemplates.push_back(new EnergyStrike());
        skillTemplates.push_back(new SolidShield());
        skillTemplates.push_back(new ComboStrike());
        skillTemplates.push_back(new LifeToEnergy());
        skillTemplates.push_back(new ArmorBreak());
    }
    ~RandomSkillManager() {
        for (PetSkill* skill : skillTemplates) {
            delete skill;
        }
        for (PetSkill* skill : currentSkills) {
            delete skill;
        }
    }
    void refreshSkills() {
        // 清理上回合的技能
        for (PetSkill* skill : currentSkills) {
            delete skill;
        }
        currentSkills.clear();

        // 随机选择新技能
        vector<int> usedIndices;
        for (int i = 0;
             i < skillsPerTurn && usedIndices.size() < skillTemplates.size();
             i++) {
            int index;
            do {
                index = rand() % skillTemplates.size();
            } while (find(usedIndices.begin(), usedIndices.end(), index) !=
                     usedIndices.end());

            usedIndices.push_back(index);
            currentSkills.push_back(createSkillCopy(skillTemplates[index]));
        }
    }
    vector<PetSkill*> getCurrentSkills() const { return currentSkills; }
    // 抽取一个额外技能
    void drawExtraSkill() {
        if (!skillTemplates.empty()) {
            int index = rand() % skillTemplates.size();
            currentSkills.push_back(createSkillCopy(skillTemplates[index]));
            cout << "🎁 Drew extra skill: " << currentSkills.back()->getName()
                 << endl;
        }
    }
    // 检查是否需要抽取额外技能（给Battle调用）
    void checkForExtraSkillDraw(BattleEntityState* playerState) {
        vector<StatusEffect*> effects = playerState->getStatusEffects();
        for (StatusEffect* effect : effects) {
            if (effect->getName() == "Draw Extra Skill") {
                drawExtraSkill();
                playerState->removeStatusEffect(
                    "Draw Extra Skill");  // 使用后移除
                break;
            }
        }
    }
};

class BossSkill : public Skill {
   public:
    virtual string getName() const { return "Boss Skill"; }
    virtual string getDescription() const { return "Basic boss skill"; }
    virtual string getDetailedDescription() const {
        return "This is a basic skill for boss.";
    }
    virtual int calculateDamage(BattleEntityState* user,
                                BattleEntityState* target) = 0;

    virtual void applyEffects(BattleEntityState* boss,
                              BattleEntityState* target) = 0;
};

// a set of hardcoded boss skills - could be replaced with skills generated by a
// "skill factory" from a more efficient database

class SimpleAttack : public BossSkill {
   public:
    string getName() const override { return "Simple Attack"; }

    string getDescription() const override { return "Basic damage attack"; }

    string getDetailedDescription() const override {
        return "Boss performs a basic attack dealing normal damage.";
    }

    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        int damage = user->getFinalAttack() - target->getFinalDefense() / 3;
        return max(1, damage + (rand() % 5) - 2);  // -2到+2随机
    }

    void applyEffects(BattleEntityState* bossState,
                      BattleEntityState* playerState) override {
        cout << " " << bossState->getEntity()->getName()
             << " strikes with basic force!" << endl;

        // 基础攻击偶尔可能造成轻微流血效果
        if (rand() % 100 < 15) {  // 15%概率
            StatusEffect* bleeding = new StatusEffect("Minor Bleeding", 2);
            bleeding->addModifier("hp_per_turn", -2);  // 每回合损失2HP
            playerState->addStatusEffect(bleeding);
            cout << "🩸 The attack causes minor bleeding!" << endl;
        }
    }
};

class HeavyBlow : public BossSkill {
   public:
    string getName() const override { return "Heavy Blow"; }

    string getDescription() const override {
        return "Strong attack dealing 150% damage";
    }

    string getDetailedDescription() const override {
        return "Boss winds up for a powerful blow dealing 150% normal damage.";
    }

    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        int baseDamage = (int)(user->getFinalAttack() * 1.5f);  // 150%攻击力
        int damage = baseDamage - target->getFinalDefense() / 3;
        return max(1, damage + (rand() % 6) - 1);  // -1到+4随机
    }

    void applyEffects(BattleEntityState* bossState,
                      BattleEntityState* playerState) override {
        cout << "🔨 " << bossState->getEntity()->getName()
             << " delivers a crushing blow!" << endl;
        cout << "💢 The ground shakes from the massive impact!" << endl;

        // 重击有较高概率造成眩晕效果
        if (rand() % 100 < 30) {  // 30%概率眩晕
            StatusEffect* stunned = new StatusEffect("Stunned", 1);
            stunned->addModifier("speed", -5);
            stunned->addModifier("defense", -3);  // 眩晕时防御也降低
            playerState->addStatusEffect(stunned);
            cout << "💫 " << playerState->getEntity()->getName()
                 << " is stunned by the powerful blow!" << endl;
        }

        // 额外还有小概率造成装甲破损
        if (rand() % 100 < 20) {  // 20%概率
            StatusEffect* armorBreak = new StatusEffect("Armor Crack", 3);
            armorBreak->addModifier("defense", -2);
            playerState->addStatusEffect(armorBreak);
            cout << " The blow cracks the target's defenses!" << endl;
        }
    }
};

class DefensiveStance : public BossSkill {
   public:
    string getName() const override { return "Defensive Stance"; }

    string getDescription() const override {
        return "Increases defense for 3 turns";
    }

    string getDetailedDescription() const override {
        return "Boss takes a defensive position, increasing defense by 5 for 3 "
               "turns.";
    }

    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        return 0;  // 防御技能不造成直接伤害
    }

    void applyEffects(BattleEntityState* bossState,
                      BattleEntityState* playerState) override {
        cout << " " << bossState->getEntity()->getName()
             << " takes a defensive stance!" << endl;

        // 创建防御增强状态效果
        StatusEffect* defenseBoost = new StatusEffect("Defense Boost", 3);
        defenseBoost->addModifier("defense", 5);

        // 应用到Boss身上
        bossState->addStatusEffect(defenseBoost);

        cout << "📈 Defense increased by 5 for 3 turns!" << endl;
    }
};

class ShieldUp : public BossSkill {
   public:
    string getName() const override { return "Shield Up"; }

    string getDescription() const override {
        return "Creates a shield absorbing 20 damage for 2 turns";
    }

    string getDetailedDescription() const override {
        return "Boss conjures a protective barrier that absorbs 20 damage.";
    }

    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        return 0;  // 护盾技能不造成伤害
    }

    void applyEffects(BattleEntityState* bossState,
                      BattleEntityState* playerState) override {
        cout << " " << bossState->getEntity()->getName()
             << " conjures a protective shield!" << endl;

        // 直接给Boss添加护盾
        bossState->addShield(20);
    }
};

class QuickStrike : public BossSkill {
   public:
    string getName() const override { return "Quick Strike"; }

    string getDescription() const override {
        return "Fast attack based on speed";
    }

    string getDetailedDescription() const override {
        return "Boss performs a lightning-fast strike, damage based on speed.";
    }

    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        // 基于速度的攻击：攻击力 + 速度/2
        int speedBonus = user->getFinalSpeed() / 2;
        int baseDamage = user->getFinalAttack() + speedBonus;

        // 快速攻击更难防御，防御效果减半
        int defense = target->getFinalDefense() / 2;

        int damage = baseDamage - defense;

        // 添加随机性，快速攻击比较稳定但有小幅波动
        damage += (rand() % 4);  // 0到+3随机加成

        return max(1, damage);
    }

    void applyEffects(BattleEntityState* bossState,
                      BattleEntityState* playerState) override {
        cout << " " << bossState->getEntity()->getName()
             << " strikes with lightning speed!" << endl;
        cout << "💨 The attack is too fast to properly defend against!" << endl;

        // 快速攻击有20%概率给Boss增加速度buff
        if (rand() % 100 < 20) {
            StatusEffect* speedBoost = new StatusEffect("Adrenaline Rush", 2);
            speedBoost->addModifier("speed", 3);
            speedBoost->addModifier("attack", 1);  // 轻微攻击加成

            bossState->addStatusEffect(speedBoost);
            cout << "🏃 " << bossState->getEntity()->getName()
                 << " feels energized by the swift movement!" << endl;
        }

        // 10%概率让玩家下回合速度降低（被突袭打懵）
        if (rand() % 100 < 10) {
            StatusEffect* disoriented = new StatusEffect("Disoriented", 1);
            disoriented->addModifier("speed", -2);

            playerState->addStatusEffect(disoriented);
            cout << "😵 " << playerState->getEntity()->getName()
                 << " is caught off guard by the sudden attack!" << endl;
        }
    }
};

class Intimidate : public BossSkill {
   public:
    string getName() const override { return "Intimidate"; }

    string getDescription() const override {
        return "Reduces target attack by 3 for 2 turns";
    }

    string getDetailedDescription() const override {
        return "Boss intimidates the target, reducing their attack power by 3 "
               "for 2 turns.";
    }

    int calculateDamage(BattleEntityState* user,
                        BattleEntityState* target) override {
        // 威吓主要是心理攻击，物理伤害较低
        int baseDamage = user->getFinalAttack() / 3;  // 只有1/3攻击力的伤害
        int defense = target->getFinalDefense() / 4;  // 防御效果也有限

        int damage = baseDamage - defense;

        // 威吓伤害比较固定，主要靠debuff效果
        damage += (rand() % 3);  // 0到+2的小幅随机

        return max(1, damage);  // 至少造成1点伤害
    }

    void applyEffects(BattleEntityState* bossState,
                      BattleEntityState* playerState) override {
        cout << "😨 " << bossState->getEntity()->getName()
             << " lets out a terrifying roar!" << endl;
        cout << "🌊 Waves of intimidating presence wash over the battlefield!"
             << endl;

        // 主要效果：攻击力降低
        StatusEffect* intimidated =
            new StatusEffect("Intimidated", 3);  // 持续3回合
        intimidated->addModifier("attack", -4);  // 攻击力-4
        intimidated->addModifier("speed", -2);   // 速度-2（被吓得动作迟缓）

        playerState->addStatusEffect(intimidated);

        cout << "📉 " << playerState->getEntity()->getName()
             << " is intimidated! Attack reduced by 4 and speed reduced by 2 "
                "for 3 turns!"
             << endl;

        // 额外效果：如果玩家正在防御，威吓会打断防御状态
        if (playerState->isDefending()) {
            playerState->setDefending(false);
            cout << " The intimidation breaks "
                 << playerState->getEntity()->getName()
                 << "'s defensive concentration!" << endl;
        }

        // 25%概率造成额外的"恐惧"效果
        if (rand() % 100 < 25) {
            StatusEffect* fear = new StatusEffect("Fear", 2);
            fear->addModifier("defense", -2);  // 防御力也降低

            playerState->addStatusEffect(fear);
            cout << "😰 " << playerState->getEntity()->getName()
                 << " is overcome with fear! Defense also reduced by 2!"
                 << endl;
        }

        // Boss自己获得"威吓气势"buff（小幅增强）
        StatusEffect* menacingAura = new StatusEffect("Menacing Aura", 2);
        menacingAura->addModifier("attack", 2);  // 攻击力+2

        bossState->addStatusEffect(menacingAura);
        cout << " " << bossState->getEntity()->getName()
             << " is empowered by the successful intimidation!" << endl;
    }
};

#endif  // SKILL_H