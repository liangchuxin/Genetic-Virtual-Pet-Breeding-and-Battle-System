#ifndef GENETIC_PET_H
#define GENETIC_PET_H

#include "../gene/Gene.h"
#include "../common.h"
#include <algorithm>
#include <cstdio>

class Pet;
class PetSkill;

// 基于基因系统的宠物类
class GeneticPet : public Pet
{
private:
    string petName;
    Breed *breed;
    vector<Gene *> genes;
    vector<StatusEffect *> statusEffects;
    mutable vector<PetSkill *> cachedSkills;

    // 最终属性 (基础属性 + 基因加成)
    int finalAttack;
    int finalDefense;
    int finalSpeed;
    int finalMaxHP;

public:
    GeneticPet(const string &name, Breed *petBreed, const vector<Gene *> &inheritedGenes)
        : Pet(0, 0, 0, 0), petName(name), breed(petBreed)
    {

        // 复制基因
        for (int i = 0; i < inheritedGenes.size(); i++)
        {
            genes.push_back(inheritedGenes[i]->clone());
        }

        // 计算最终属性
        calculateFinalAttributes();

        // 生成技能
        generateSkills();
    }

    // 随机基因构造函数
    GeneticPet(const string &name, Breed *petBreed)
        : Pet(0, 0, 0, 0), petName(name), breed(petBreed)
    {

        // 生成随机基因 (3-6个)
        int geneCount = 3 + (rand() % 4);
        genes = GeneFactory::createRandomGenes(geneCount);

        calculateFinalAttributes();
        generateSkills();
    }

    virtual ~GeneticPet()
    {
        // 清理基因
        for (int i = 0; i < genes.size(); i++)
        {
            delete genes[i];
        }

        // 清理状态效果
        for (int i = 0; i < statusEffects.size(); i++)
        {
            delete statusEffects[i];
        }

        // 清理技能缓存
        for (int i = 0; i < cachedSkills.size(); i++)
        {
            delete cachedSkills[i];
        }
    }

    // 计算最终属性
    void calculateFinalAttributes()
    {
        // 从品种获取基础属性
        finalAttack = breed->baseAttack;
        finalDefense = breed->baseDefense;
        finalSpeed = breed->baseSpeed;
        finalMaxHP = breed->baseHP;

        // 应用基因加成
        for (int i = 0; i < genes.size(); i++)
        {
            Gene *gene = genes[i];
            if (gene->type == GeneType::STAT_GENE)
            {
                StatGene *statGene = static_cast<StatGene *>(gene);

                if (statGene->attribute == "attack")
                {
                    finalAttack += statGene->bonus;
                }
                else if (statGene->attribute == "defense")
                {
                    finalDefense += statGene->bonus;
                }
                else if (statGene->attribute == "speed")
                {
                    finalSpeed += statGene->bonus;
                }
                else if (statGene->attribute == "hp")
                {
                    finalMaxHP += statGene->bonus;
                }
            }
        }

        // 确保属性不为负数
        finalAttack = max(1, finalAttack);
        finalDefense = max(1, finalDefense);
        finalSpeed = max(1, finalSpeed);
        finalMaxHP = max(10, finalMaxHP);
    }

    // 生成技能列表
    void generateSkills()
    {
        // 清理现有技能
        for (int i = 0; i < cachedSkills.size(); i++)
        {
            delete cachedSkills[i];
        }
        cachedSkills.clear();

        // 添加基础技能
        cachedSkills.push_back(new PowerStrike());
        cachedSkills.push_back(new LifeDrain());
        cachedSkills.push_back(new HeavyStrike());

        // 从技能基因生成特殊技能
        for (int i = 0; i < genes.size(); i++)
        {
            Gene *gene = genes[i];
            if (gene->type == GeneType::SKILL_GENE)
            {
                SkillGene *skillGene = static_cast<SkillGene *>(gene);
                cachedSkills.push_back(skillGene->createPetSkill());
            }
        }
    }

    // 重写Entity类的虚函数
    string getName() const
    {
        return petName;
    }

    int getBaseAttack() const
    {
        return breed->baseAttack;
    }

    int getBaseDefense() const
    {
        return breed->baseDefense;
    }

    int getBaseSpeed() const
    {
        return breed->baseSpeed;
    }

    int getAttack() const
    {
        return finalAttack;
    }

    int getDefense() const
    {
        return finalDefense;
    }

    int getSpeed() const
    {
        return finalSpeed;
    }

    int getMaxHP() const
    {
        return finalMaxHP;
    }

    vector<PetSkill *> getAvailableSkills() const
    {
        return cachedSkills;
    }

    void addStatusEffect(StatusEffect *effect)
    {
        statusEffects.push_back(effect);
    }

    void removeStatusEffect(const string &effectName)
    {
        for (int i = 0; i < statusEffects.size(); i++)
        {
            if (statusEffects[i]->getName() == effectName)
            {
                delete statusEffects[i];
                statusEffects.erase(statusEffects.begin() + i);
                break;
            }
        }
    }

    void tickStatusEffects()
    {
        for (int i = 0; i < statusEffects.size();)
        {
            StatusEffect *effect = statusEffects[i];
            if (!effect->tick())
            {
                delete effect;
                statusEffects.erase(statusEffects.begin() + i);
            }
            else
            {
                i++;
            }
        }
    }

    vector<StatusEffect *> getStatusEffects() const
    {
        return statusEffects;
    }

    void displayDetailedInfo() const
    {
        string line = "============================================================";
        cout << "\n"
             << line << endl;
        cout << "  " << petName << " (" << breed->name << ")" << endl;
        cout << line << endl;

        cout << "Species: " << getSpeciesString(breed->species) << endl;
        cout << "Breed: " << breed->name << endl;

        cout << "\n Final Attributes:" << endl;
        cout << "  Attack:  " << finalAttack << " (Base: " << breed->baseAttack << ")" << endl;
        cout << "  Defense: " << finalDefense << " (Base: " << breed->baseDefense << ")" << endl;
        cout << "  Speed:   " << finalSpeed << " (Base: " << breed->baseSpeed << ")" << endl;
        cout << "  Max HP:  " << finalMaxHP << " (Base: " << breed->baseHP << ")" << endl;

        cout << "\n Genetic Information:" << endl;
        cout << "  Total Genes: " << genes.size() << endl;
        for (int i = 0; i < genes.size(); i++)
        {
            cout << "  " << (i + 1) << ". " << genes[i]->getDescription() << endl;
        }

        cout << "\n Available Skills:" << endl;
        for (int i = 0; i < cachedSkills.size(); i++)
        {
            cout << "  " << (i + 1) << ". " << cachedSkills[i]->getName()
                 << " (Cost: " << cachedSkills[i]->getEnergyCost() << ")" << endl;
        }

        cout << line << endl;
    }

    // 获取基因副本(用于繁殖)
    vector<Gene *> getGenesCopy() const
    {
        vector<Gene *> genesCopy;
        for (int i = 0; i < genes.size(); i++)
        {
            genesCopy.push_back(genes[i]->clone());
        }
        return genesCopy;
    }

    Breed *getBreed() const
    {
        return breed;
    }

    Species getSpecies() const
    {
        return breed->species;
    }

private:
    string getSpeciesString(Species species) const
    {
        switch (species)
        {
        case Species::DOG:
            return "Dog";
        case Species::CAT:
            return "Cat";
        case Species::BIRD:
            return "Bird";
        default:
            return "Unknown";
        }
    }
};

#endif // GENETIC_PET_H