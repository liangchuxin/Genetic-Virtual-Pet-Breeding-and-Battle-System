#ifndef BREEDING_SYSTEM_H
#define BREEDING_SYSTEM_H

#include "../gene/Gene.h"
#include "GeneticPet.h"
#include <algorithm>
#include <random>

class BreedingSystem {
public:
    // 繁殖两只宠物
    static GeneticPet* breedPets(const GeneticPet& parent1, const GeneticPet& parent2, const string& childName) {
        cout << "\n" << string(50, '=') << endl;
        cout << "🍼 BREEDING PROCESS" << endl;
        cout << string(50, '=') << endl;
        
        // 1. 检查物种兼容性
        if(!areCompatibleSpecies(parent1, parent2)) {
            cout << " Breeding failed: Incompatible species!" << endl;
            cout << "   " << parent1.getName() << " (" << getSpeciesString(parent1.getSpecies()) << ")" << endl;
            cout << "   " << parent2.getName() << " (" << getSpeciesString(parent2.getSpecies()) << ")" << endl;
            return nullptr;
        }
        
        cout << " Species compatibility confirmed: " << getSpeciesString(parent1.getSpecies()) << endl;
        cout << " Parents: " << parent1.getName() << " x " << parent2.getName() << endl;
        
        // 2. 确定子代品种
        Breed* childBreed = determineChildBreed(parent1.getBreed(), parent2.getBreed());
        cout << " Child breed determined: " << childBreed->name << endl;
        
        // 3. 基因继承
        vector<Gene*> inheritedGenes = inheritGenes(parent1.getGenesCopy(), parent2.getGenesCopy());
        cout << "Inherited " << inheritedGenes.size() << " genes" << endl;
        
        // 4. 检查基因突变
        performMutations(inheritedGenes);
        
        // 5. 创建子代宠物
        GeneticPet* child = new GeneticPet(childName, childBreed, inheritedGenes);
        
        cout << "\n Breeding successful!" << endl;
        cout << "   New pet: " << childName << " (" << childBreed->name << ")" << endl;
        cout << "   Final gene count: " << inheritedGenes.size() << endl;
        cout << string(50, '=') << endl;
        
        return child;
    }
    
    // 预测繁殖结果(不实际执行)
    static void predictBreedingOutcome(const GeneticPet& parent1, const GeneticPet& parent2) {
        cout << "\n" << string(50, '=') << endl;
        cout << " BREEDING PREDICTION" << endl;
        cout << string(50, '=') << endl;
        
        if(!areCompatibleSpecies(parent1, parent2)) {
            cout << " These pets cannot breed (incompatible species)" << endl;
            return;
        }
        
        cout << "Parents: " << parent1.getName() << " x " << parent2.getName() << endl;
        cout << "Species: " << getSpeciesString(parent1.getSpecies()) << endl;
        
        // 预测品种可能性
        cout << "\n Possible breeds:" << endl;
        if(parent1.getBreed()->name == parent2.getBreed()->name) {
            cout << "  100% - " << parent1.getBreed()->name << " (same breed)" << endl;
        } else {
            cout << "  50% - " << parent1.getBreed()->name << endl;
            cout << "  50% - " << parent2.getBreed()->name << endl;
        }
        
        // 分析基因继承概率
        cout << "\n Gene inheritance probabilities:" << endl;
        analyzeGeneInheritance(parent1.getGenesCopy(), "Parent 1");
        analyzeGeneInheritance(parent2.getGenesCopy(), "Parent 2");
        
        cout << "\n Mutation chance: 10%" << endl;
        cout << string(50, '=') << endl;
    }

private:
    // 检查物种兼容性
    static bool areCompatibleSpecies(const GeneticPet& pet1, const GeneticPet& pet2) {
        return pet1.getSpecies() == pet2.getSpecies();
    }
    
    // 确定子代品种
    static Breed* determineChildBreed(Breed* breed1, Breed* breed2) {
        // 如果父母是同一品种，子代100%继承该品种
        if(breed1->name == breed2->name) {
            return breed1;
        }
        
        // 不同品种时，随机选择一个父母的品种
        // 5% 概率发生品种突变（生成同物种的稀有品种）
        if(rand() % 100 < 5) {
            cout << " Rare breed mutation detected!" << endl;
            vector<Breed*> availableBreeds = BreedDatabase::getBreedsBySpecies(breed1->species);
            
            // 尝试找到一个稀有品种
            for(Breed* breed : availableBreeds) {
                if(breed->name != breed1->name && breed->name != breed2->name) {
                    cout << "   Mutated to: " << breed->name << endl;
                    return breed;
                }
            }
        }
        
        // 正常情况：50%概率继承任一父母品种
        return (rand() % 2 == 0) ? breed1 : breed2;
    }
    
    // 基因继承算法
    static vector<Gene*> inheritGenes(vector<Gene*> genes1, vector<Gene*> genes2) {
        vector<Gene*> inheritedGenes;
        
        cout << "\n Gene inheritance process:" << endl;
        
        // 处理第一个父母的基因
        cout << "    1:" << endl;
        for(Gene* gene : genes1) {
            double inheritanceChance = gene->getInheritanceProbability();
            double roll = static_cast<double>(rand()) / RAND_MAX;
            
            if(roll < inheritanceChance) {
                inheritedGenes.push_back(gene->clone());
                cout << "      Inherited: " << gene->name << " (chance: " << int(inheritanceChance * 100) << "%)" << endl;
            } else {
                cout << "      Lost: " << gene->name << " (chance: " << int(inheritanceChance * 100) << "%)" << endl;
            }
        }
        
        // 处理第二个父母的基因
        cout << "   From parent 2:" << endl;
        for(Gene* gene : genes2) {
            double inheritanceChance = gene->getInheritanceProbability();
            double roll = static_cast<double>(rand()) / RAND_MAX;
            
            if(roll < inheritanceChance) {
                inheritedGenes.push_back(gene->clone());
                cout << "      Inherited: " << gene->name << " (chance: " << int(inheritanceChance * 100) << "%)" << endl;
            } else {
                cout << "      Lost: " << gene->name << " (chance: " << int(inheritanceChance * 100) << "%)" << endl;
            }
        }
        
        // 清理父母基因副本
        for(Gene* gene : genes1) delete gene;
        for(Gene* gene : genes2) delete gene;
        
        return inheritedGenes;
    }
    
    // 基因突变处理
    static void performMutations(vector<Gene*>& genes) {
        cout << "\n Checking for mutations..." << endl;
        
        // 10%概率发生突变
        if(rand() % 100 < 10) {
            int mutationType = rand() % 3;
            
            switch(mutationType) {
                case 0: // 新基因生成
                    performNewGeneMutation(genes);
                    break;
                case 1: // 基因强化
                    performGeneEnhancement(genes);
                    break;
                case 2: // 基因融合
                    performGeneFusion(genes);
                    break;
            }
        } else {
            cout << "   No mutations occurred." << endl;
        }
    }
    
    // 新基因突变
    static void performNewGeneMutation(vector<Gene*>& genes) {
        cout << "    New gene mutation!" << endl;
        
        // 根据现有基因的稀有度决定新基因的稀有度
        Rarity newGeneRarity = Rarity::COMMON;
        for(Gene* gene : genes) {
            if(static_cast<int>(gene->rarity) > static_cast<int>(newGeneRarity)) {
                newGeneRarity = gene->rarity;
            }
        }
        
        // 有小概率提升稀有度
        if(rand() % 100 < 20 && static_cast<int>(newGeneRarity) < 5) {
            newGeneRarity = static_cast<Rarity>(static_cast<int>(newGeneRarity) + 1);
        }
        
        Gene* newGene = GeneFactory::createGeneByRarity(newGeneRarity);
        genes.push_back(newGene);
        
        cout << "     Generated: " << newGene->name << " (" << getRarityString(newGene->rarity) << ")" << endl;
    }
    
    // 基因强化突变
    static void performGeneEnhancement(vector<Gene*>& genes) {
        if(genes.empty()) return;
        
        cout << "    Gene enhancement mutation!" << endl;
        
        int targetIndex = rand() % genes.size();
        Gene* targetGene = genes[targetIndex];
        
        if(targetGene->type == GeneType::STAT_GENE) {
            StatGene* statGene = static_cast<StatGene*>(targetGene);
            int enhancement = 2 + rand() % 4; // +2 to +5
            statGene->bonus += enhancement;
            
            cout << "     Enhanced: " << statGene->name << " +" << enhancement << " (now +" << statGene->bonus << ")" << endl;
        } else if(targetGene->type == GeneType::SKILL_GENE) {
            SkillGene* skillGene = static_cast<SkillGene*>(targetGene);
            int enhancement = 10 + rand() % 21; // +10 to +30
            skillGene->skillPower += enhancement;
            
            cout << "     Enhanced: " << skillGene->name << " +" << enhancement << " power (now " << skillGene->skillPower << ")" << endl;
        }
    }
    
    // 基因融合突变
    static void performGeneFusion(vector<Gene*>& genes) {
        if(genes.size() < 2) return;
        
        cout << "    Gene fusion mutation!" << endl;
        
        // 随机选择两个基因进行融合
        int index1 = rand() % genes.size();
        int index2;
        do {
            index2 = rand() % genes.size();
        } while(index2 == index1);
        
        Gene* gene1 = genes[index1];
        Gene* gene2 = genes[index2];
        
        // 创建融合基因
        Gene* fusedGene = createFusedGene(gene1, gene2);
        
        cout << "     Fused: " << gene1->name << " + " << gene2->name << " = " << fusedGene->name << endl;
        
        // 移除原基因，添加融合基因
        delete gene1;
        delete gene2;
        genes.erase(genes.begin() + max(index1, index2)); // 先删除较大索引
        genes.erase(genes.begin() + min(index1, index2)); // 再删除较小索引
        genes.push_back(fusedGene);
    }
    
    // 创建融合基因
    static Gene* createFusedGene(Gene* gene1, Gene* gene2) {
        // 选择较高的稀有度
        Rarity fusedRarity = max(gene1->rarity, gene2->rarity);
        
        // 如果两个基因都是数值基因且属性相同，融合为更强的基因
        if(gene1->type == GeneType::STAT_GENE && gene2->type == GeneType::STAT_GENE) {
            StatGene* stat1 = static_cast<StatGene*>(gene1);
            StatGene* stat2 = static_cast<StatGene*>(gene2);
            
            if(stat1->attribute == stat2->attribute) {
                int fusedBonus = stat1->bonus + stat2->bonus;
                return new StatGene("Fused " + stat1->attribute + " Gene", fusedRarity, stat1->attribute, fusedBonus);
            }
        }
        
        // 如果是技能基因融合，创建混合技能
        if(gene1->type == GeneType::SKILL_GENE && gene2->type == GeneType::SKILL_GENE) {
            SkillGene* skill1 = static_cast<SkillGene*>(gene1);
            SkillGene* skill2 = static_cast<SkillGene*>(gene2);
            
            int fusedPower = (skill1->skillPower + skill2->skillPower) * 0.8; // 稍微降低以平衡
            int fusedCost = max(skill1->energyCost, skill2->energyCost);
            
            return new SkillGene("Hybrid Gene", fusedRarity, "Fusion Strike", fusedPower, fusedCost);
        }
        
        // 默认情况：随机选择一个基因作为基础，提升其稀有度
        Gene* baseGene = (rand() % 2 == 0) ? gene1 : gene2;
        Gene* result = baseGene->clone();
        
        if(static_cast<int>(result->rarity) < 5) {
            result->rarity = static_cast<Rarity>(static_cast<int>(result->rarity) + 1);
        }
        
        return result;
    }
    
    // 分析基因继承概率
    static void analyzeGeneInheritance(const vector<Gene*>& genes, const string& parentName) {
        cout << "   " << parentName << " genes:" << endl;
        
        for(Gene* gene : genes) {
            double chance = gene->getInheritanceProbability() * 100;
            cout << "     - " << gene->name << ": " << int(chance) << "% chance" << endl;
        }
        
        // 清理基因副本
        for(Gene* gene : genes) {
            delete gene;
        }
    }
    
    // 工具函数
    static string getSpeciesString(Species species) {
        switch(species) {
            case Species::DOG: return "Dog";
            case Species::CAT: return "Cat";
            case Species::BIRD: return "Bird";
            default: return "Unknown";
        }
    }
    
    static string getRarityString(Rarity rarity) {
        switch(rarity) {
            case Rarity::COMMON: return "Common";
            case Rarity::UNCOMMON: return "Uncommon";
            case Rarity::RARE: return "Rare";
            case Rarity::EPIC: return "Epic";
            case Rarity::LEGENDARY: return "Legendary";
            default: return "Unknown";
        }
    }
};

#endif // BREEDING_SYSTEM_H