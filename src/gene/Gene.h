#ifndef GENE_H
#define GENE_H

#include "../common.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
using namespace std;

// 前向声明
class PetSkill;
class GeneticSkill;

// 基因抽象基类
class Gene {
public:
    string name;
    Rarity rarity;
    GeneType type;
    
    Gene(const string& n, Rarity r, GeneType t) : name(n), rarity(r), type(t) {}
    virtual ~Gene() {}
    virtual string getDescription() const = 0;
    virtual Gene* clone() const = 0;
    
    double getInheritanceProbability() const {
        return static_cast<double>(rarity) / 15.0; // 稀有度越高，继承概率越高
    }
};

// 数值基因类 - 影响宠物基础属性
class StatGene : public Gene {
public:
    string attribute;  // "attack", "defense", "speed", "hp"
    int bonus;
    
    StatGene(const string& n, Rarity r, const string& attr, int b) 
        : Gene(n, r, GeneType::STAT_GENE), attribute(attr), bonus(b) {}
    
    string getDescription() const {
        return name + " (" + getRarityString(rarity) + ") - " + attribute + " +" + to_string(bonus);
    }
    
    Gene* clone() const {
        return new StatGene(name, rarity, attribute, bonus);
    }

private:
    string getRarityString(Rarity r) const {
        switch(r) {
            case Rarity::COMMON: return "Common";
            case Rarity::UNCOMMON: return "Uncommon";
            case Rarity::RARE: return "Rare";
            case Rarity::EPIC: return "Epic";
            case Rarity::LEGENDARY: return "Legendary";
            default: return "Unknown";
        }
    }
    
    string to_string(int value) const {
        char buffer[32];
        sprintf(buffer, "%d", value);
        return string(buffer);
    }
};

// 基因技能 - 由基因产生的宠物技能
class GeneticSkill : public PetSkill {
private:
    int basePower;

public:
    GeneticSkill(const string& skillName, int power, int cost, const string& desc)
        : PetSkill(skillName, "Genetic skill", desc, cost), basePower(power) {}
    
    int calculateDamage(BattleEntityState* user, BattleEntityState* target) {
        // 基因技能伤害 = 基础威力 + 宠物攻击力的一定比例
        int baseDamage = basePower + (user->getFinalAttack() * basePower / 100);
        int defense = target->getFinalDefense() / 3;
        return max(1, baseDamage - defense);
    }
    
    void applyEffects(BattleEntityState* user, BattleEntityState* target) {
        cout << "🧬 " << getName() << " activated through genetic inheritance!" << endl;
        
        // 根据技能名称应用不同效果
        if (name.find("Fire") != string::npos || name.find("Flame") != string::npos) {
            // 火焰类技能 - 有概率造成灼烧
            if (rand() % 100 < 20) {
                StatusEffect* burn = new StatusEffect("Burn", 2);
                burn->addModifier("hp_per_turn", -2);
                target->addStatusEffect(burn);
                cout << "🔥 Target is burned!" << endl;
            }
        } else if (name.find("Ice") != string::npos || name.find("Frost") != string::npos) {
            // 冰霜类技能 - 有概率减速
            if (rand() % 100 < 25) {
                StatusEffect* freeze = new StatusEffect("Slowed", 2);
                freeze->addModifier("speed", -3);
                target->addStatusEffect(freeze);
                cout << "❄️ Target is slowed!" << endl;
            }
        } else if (name.find("Lightning") != string::npos || name.find("Thunder") != string::npos) {
            // 雷电类技能 - 有概率眩晕
            if (rand() % 100 < 15) {
                StatusEffect* stun = new StatusEffect("Stunned", 1);
                stun->addModifier("speed", -5);
                stun->addModifier("defense", -2);
                target->addStatusEffect(stun);
                cout << " Target is stunned!" << endl;
            }
        }
    }
};

// 技能基因类 - 为宠物提供特殊技能
class SkillGene : public Gene {
public:
    string skillName;
    int skillPower;    // 技能威力修正
    int energyCost;
    
    SkillGene(const string& n, Rarity r, const string& skill, int power, int cost) 
        : Gene(n, r, GeneType::SKILL_GENE), skillName(skill), skillPower(power), energyCost(cost) {}
    
    string getDescription() const {
        return name + " (" + getRarityString(rarity) + ") - Skill: " + skillName + 
               " (Power:" + to_string(skillPower) + ", Cost:" + to_string(energyCost) + ")";
    }
    
    Gene* clone() const {
        return new SkillGene(name, rarity, skillName, skillPower, energyCost);
    }
    
    // 创建对应的宠物技能
    PetSkill* createPetSkill() const {
        return new GeneticSkill(skillName, skillPower, energyCost, 
                               "Skill inherited from " + name + " gene");
    }

private:
    string getRarityString(Rarity r) const {
        switch(r) {
            case Rarity::COMMON: return "Common";
            case Rarity::UNCOMMON: return "Uncommon";
            case Rarity::RARE: return "Rare";
            case Rarity::EPIC: return "Epic";
            case Rarity::LEGENDARY: return "Legendary";
            default: return "Unknown";
        }
    }
    
    string to_string(int value) const {
        char buffer[32];
        sprintf(buffer, "%d", value);
        return string(buffer);
    }
};

// 基因工厂 - 生成随机基因
class GeneFactory {
public:
    static vector<Gene*> createRandomGenes(int count) {
        vector<Gene*> genes;
        
        // 基因模板数据
        struct GeneTemplate {
            string name;
            Rarity rarity;
            GeneType type;
            string attribute;  // for stat genes
            string skillName;  // for skill genes
            int value;
        };
        
        vector<GeneTemplate> templates;
        
        // 数值基因
        templates.push_back({"Strength Gene", Rarity::COMMON, GeneType::STAT_GENE, "attack", "", 5});
        templates.push_back({"Agility Gene", Rarity::COMMON, GeneType::STAT_GENE, "speed", "", 3});
        templates.push_back({"Toughness Gene", Rarity::COMMON, GeneType::STAT_GENE, "defense", "", 4});
        templates.push_back({"Vitality Gene", Rarity::COMMON, GeneType::STAT_GENE, "hp", "", 15});
        templates.push_back({"Mighty Gene", Rarity::RARE, GeneType::STAT_GENE, "attack", "", 12});
        templates.push_back({"Swift Gene", Rarity::RARE, GeneType::STAT_GENE, "speed", "", 10});
        templates.push_back({"Fortified Gene", Rarity::EPIC, GeneType::STAT_GENE, "defense", "", 15});
        templates.push_back({"Legendary Vigor", Rarity::LEGENDARY, GeneType::STAT_GENE, "hp", "", 50});
        
        // 技能基因
        templates.push_back({"Fire Gene", Rarity::UNCOMMON, GeneType::SKILL_GENE, "", "Flame Burst", 80});
        templates.push_back({"Ice Gene", Rarity::UNCOMMON, GeneType::SKILL_GENE, "", "Frost Bite", 70});
        templates.push_back({"Lightning Gene", Rarity::RARE, GeneType::SKILL_GENE, "", "Thunder Strike", 120});
        templates.push_back({"Earth Gene", Rarity::RARE, GeneType::SKILL_GENE, "", "Stone Crush", 110});
        templates.push_back({"Wind Gene", Rarity::EPIC, GeneType::SKILL_GENE, "", "Tornado Slash", 160});
        templates.push_back({"Sacred Gene", Rarity::LEGENDARY, GeneType::SKILL_GENE, "", "Divine Judgment", 250});
        
        for(int i = 0; i < count; i++) {
            int index = rand() % templates.size();
            const GeneTemplate& tmpl = templates[index];
            
            Gene* gene = NULL;
            if(tmpl.type == GeneType::STAT_GENE) {
                gene = new StatGene(tmpl.name, tmpl.rarity, tmpl.attribute, tmpl.value);
            } else {
                gene = new SkillGene(tmpl.name, tmpl.rarity, tmpl.skillName, tmpl.value, 2);
            }
            
            genes.push_back(gene);
        }
        
        return genes;
    }
    
    // 根据稀有度生成基因
    static Gene* createGeneByRarity(Rarity targetRarity) {
        vector<Gene*> allGenes = createRandomGenes(20); // 生成大量基因作为候选
        vector<Gene*> filteredGenes;
        
        // 筛选出指定稀有度的基因
        for(int i = 0; i < allGenes.size(); i++) {
            Gene* gene = allGenes[i];
            if(gene->rarity == targetRarity) {
                filteredGenes.push_back(gene);
            } else {
                delete gene; // 清理不需要的基因
            }
        }
        
        if(!filteredGenes.empty()) {
            int index = rand() % filteredGenes.size();
            Gene* selectedGene = filteredGenes[index];
            
            // 清理其他未选中的基因
            for(int i = 0; i < filteredGenes.size(); i++) {
                if(i != index) {
                    delete filteredGenes[i];
                }
            }
            
            return selectedGene;
        }
        
        // 如果没有找到指定稀有度的基因，返回一个通用基因
        return new StatGene("Basic Gene", Rarity::COMMON, "attack", 3);
    }
};

// 品种类
class Breed {
public:
    string name;
    Species species;
    int baseAttack;
    int baseDefense;
    int baseSpeed;
    int baseHP;
    vector<string> naturalSkills; // 品种天赋技能
    
    Breed() : baseAttack(0), baseDefense(0), baseSpeed(0), baseHP(0) {}
    
    Breed(const string& n, Species s, int att, int def, int spd, int hp)
        : name(n), species(s), baseAttack(att), baseDefense(def), baseSpeed(spd), baseHP(hp) {}
    
    void addNaturalSkill(const string& skillName) {
        naturalSkills.push_back(skillName);
    }
};

// 品种数据库
class BreedDatabase {
private:
    static vector<Breed> breeds;
    static bool initialized;
    
public:
    static void initializeBreeds() {
        if(initialized) return;
        
        breeds.clear();
        
        // 狗类品种
        Breed spottyDog("Spotted Dog", Species::DOG, 25, 15, 20, 120);
        spottyDog.addNaturalSkill("Bite");
        breeds.push_back(spottyDog);
        
        Breed poodle("Poodle", Species::DOG, 15, 20, 30, 110);
        poodle.addNaturalSkill("Quick Attack");
        breeds.push_back(poodle);
        
        Breed shepherd("Shepherd Dog", Species::DOG, 20, 25, 15, 140);
        shepherd.addNaturalSkill("Guard");
        breeds.push_back(shepherd);
        
        // 猫类品种
        Breed persian("Persian Cat", Species::CAT, 18, 22, 25, 100);
        persian.addNaturalSkill("Stealth Strike");
        breeds.push_back(persian);
        
        Breed siamese("Siamese Cat", Species::CAT, 22, 18, 28, 95);
        siamese.addNaturalSkill("Agile Attack");
        breeds.push_back(siamese);
        
        Breed british("British Shorthair", Species::CAT, 20, 20, 20, 115);
        british.addNaturalSkill("Balanced Strike");
        breeds.push_back(british);
        
        // 鸟类品种
        Breed canary("Canary", Species::BIRD, 15, 10, 40, 80);
        canary.addNaturalSkill("Swift Dive");
        breeds.push_back(canary);
        
        Breed parrot("Parrot", Species::BIRD, 20, 15, 35, 90);
        parrot.addNaturalSkill("Sonic Screech");
        breeds.push_back(parrot);
        
        Breed eagle("Eagle", Species::BIRD, 30, 20, 30, 100);
        eagle.addNaturalSkill("Predator Strike");
        breeds.push_back(eagle);
        
        initialized = true;
    }
    
    static Breed* getBreed(const string& name) {
        initializeBreeds();
        for(int i = 0; i < breeds.size(); i++) {
            if(breeds[i].name == name) {
                return &breeds[i];
            }
        }
        return NULL;
    }
    
    static vector<Breed*> getBreedsBySpecies(Species species) {
        initializeBreeds();
        vector<Breed*> result;
        for(int i = 0; i < breeds.size(); i++) {
            if(breeds[i].species == species) {
                result.push_back(&breeds[i]);
            }
        }
        return result;
    }
    
    static vector<Breed>& getAllBreeds() {
        initializeBreeds();
        return breeds;
    }
};

// 静态成员定义
vector<Breed> BreedDatabase::breeds;
bool BreedDatabase::initialized = false;

#endif // GENE_H