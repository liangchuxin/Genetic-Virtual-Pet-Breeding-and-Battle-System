// Game main class header
#ifndef GAME_H
#define GAME_H

#include "../battle/BattleEngine.h"
#include "../breeding/BreedingSystem.h"
#include "../breeding/GeneticPet.h"
#include <iomanip>

class Game
{
private:
    vector<GeneticPet *> playerPets;
    vector<TestBoss *> availableBosses;
    bool gameRunning;

public:
    Game() : gameRunning(true)
    {
        // 初始化随机数种子
        srand(static_cast<unsigned int>(time(0)));

        // 初始化品种数据库
        BreedDatabase::initializeBreeds();

        // 创建可用的Boss
        initializeBosses();

        // 给玩家两只初始宠物
        createInitialPets();
    }

    ~Game()
    {
        // 清理宠物
        for (GeneticPet *pet : playerPets)
        {
            delete pet;
        }

        // 清理Boss
        for (TestBoss *boss : availableBosses)
        {
            delete boss;
        }
    }

    void run()
    {
        showWelcomeMessage();

        while (gameRunning)
        {
            showMainMenu();
            int choice = getPlayerChoice();

            switch (choice)
            {
            case 1:
                viewPets();
                break;
            case 2:
                createNewPet();
                break;
            case 3:
                breedPets();
                break;
            case 4:
                battleMenu();
                break;
            case 5:
                showGameStats();
                break;
            case 6:
                showHelp();
                break;
            case 0:
                exitGame();
                break;
            default:
                cout << " Invalid choice! Please select 0-6." << endl;
                break;
            }

            if (gameRunning)
            {
                cout << "\nPress ENTER to continue...";
                cin.ignore();
                string dummy;
                getline(cin, dummy);
            }
        }
    }

private:
    void showWelcomeMessage()
    {
        cout << "\n"
             << string(70, '=') << endl;
        cout << " WELCOME TO THE GENETIC VIRTUAL PET SYSTEM! " << endl;
        cout << string(70, '=') << endl;
        cout << " Breed, Battle, and Evolve your genetic pets!" << endl;
        cout << " Defeat powerful bosses with strategic combat!" << endl;
        cout << " Discover rare genes and create legendary pets!" << endl;
        cout << string(70, '=') << endl;
    }

    void showMainMenu()
    {
        cout << "\n"
             << string(50, '=') << endl;
        cout << " MAIN MENU" << endl;
        cout << string(50, '=') << endl;
        cout << "1.  View My Pets (" << playerPets.size() << " pets)" << endl;
        cout << "2.  Create New Pet" << endl;
        cout << "3.  Breed Pets" << endl;
        cout << "4.  Battle Arena" << endl;
        cout << "5.  Game Statistics" << endl;
        cout << "6.  Help & Instructions" << endl;
        cout << "0.  Exit Game" << endl;
        cout << string(50, '=') << endl;
        cout << "Choose an option: ";
    }

    int getPlayerChoice()
    {
        int choice;
        while (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << " Please enter a valid number: ";
        }
        cin.ignore(); // 清理输入缓冲
        return choice;
    }

    void initializeBosses()
    {
        // 创建不同难度的Boss
        TestBoss *easyBoss = new TestBoss();
        availableBosses.push_back(easyBoss);

        // 可以在这里添加更多Boss
    }

    void createInitialPets()
    {
        cout << "\n Creating your starter pets..." << endl;

        // 创建第一只宠物 - 狗
        Breed *dogBreed = BreedDatabase::getBreed("Spotted Dog");
        if (dogBreed)
        {
            GeneticPet *starterDog = new GeneticPet("Buddy", dogBreed);
            playerPets.push_back(starterDog);
            cout << " Created starter pet: " << starterDog->getName() << " (Spotted Dog)" << endl;
        }

        // 创建第二只宠物 - 猫
        Breed *catBreed = BreedDatabase::getBreed("Persian Cat");
        if (catBreed)
        {
            GeneticPet *starterCat = new GeneticPet("Whiskers", catBreed);
            playerPets.push_back(starterCat);
            cout << " Created starter pet: " << starterCat->getName() << " (Persian Cat)" << endl;
        }

        cout << " Welcome to the world of genetic pets!" << endl;
    }

    void viewPets()
    {
        if (playerPets.empty())
        {
            cout << "\n You don't have any pets yet!" << endl;
            cout << "💡 Try creating a new pet or breeding existing ones." << endl;
            return;
        }

        cout << "\n"
             << string(60, '=') << endl;
        cout << " YOUR PET COLLECTION" << endl;
        cout << string(60, '=') << endl;

        for (int i = 0; i < playerPets.size(); i++)
        {
            cout << "\n[" << (i + 1) << "] ";
            showPetSummary(playerPets[i]);
        }

        cout << "\n"
             << string(60, '=') << endl;
        cout << "Enter pet number for detailed view (0 to return): ";

        int choice = getPlayerChoice();
        if (choice > 0 && choice <= playerPets.size())
        {
            playerPets[choice - 1]->displayDetailedInfo();
        }
    }

    void showPetSummary(const GeneticPet *pet)
    {
        cout << " " << pet->getName() << " (" << pet->getBreed()->name << ")" << endl;
        cout << "    ATK:" << setw(3) << pet->getAttack()
             << " DEF:" << setw(3) << pet->getDefense()
             << " SPD:" << setw(3) << pet->getSpeed()
             << " HP:" << setw(3) << pet->getMaxHP() << endl;
    }

    void createNewPet()
    {
        cout << "\n"
             << string(50, '=') << endl;
        cout << " CREATE NEW PET" << endl;
        cout << string(50, '=') << endl;

        // 选择物种
        cout << "Choose species:" << endl;
        cout << "1.  Dog" << endl;
        cout << "2.  Cat" << endl;
        cout << "3.  Bird" << endl;
        cout << "Choice: ";

        int speciesChoice = getPlayerChoice();
        Species selectedSpecies;

        switch (speciesChoice)
        {
        case 1:
            selectedSpecies = Species::DOG;
            break;
        case 2:
            selectedSpecies = Species::CAT;
            break;
        case 3:
            selectedSpecies = Species::BIRD;
            break;
        default:
            cout << " Invalid species choice!" << endl;
            return;
        }

        // 选择品种
        vector<Breed *> availableBreeds = BreedDatabase::getBreedsBySpecies(selectedSpecies);
        cout << "\nChoose breed:" << endl;

        for (int i = 0; i < availableBreeds.size(); i++)
        {
            Breed *breed = availableBreeds[i];
            cout << (i + 1) << ". " << breed->name
                 << " (ATK:" << breed->baseAttack
                 << " DEF:" << breed->baseDefense
                 << " SPD:" << breed->baseSpeed
                 << " HP:" << breed->baseHP << ")" << endl;
        }

        cout << "Choice: ";
        int breedChoice = getPlayerChoice();

        if (breedChoice < 1 || breedChoice > availableBreeds.size())
        {
            cout << " Invalid breed choice!" << endl;
            return;
        }

        Breed *selectedBreed = availableBreeds[breedChoice - 1];

        // 获取宠物名称
        cout << "\nEnter pet name: ";
        string petName;
        getline(cin, petName);

        if (petName.empty())
        {
            petName = "Unnamed Pet";
        }

        // 创建宠物
        GeneticPet *newPet = new GeneticPet(petName, selectedBreed);
        playerPets.push_back(newPet);

        cout << "\n Successfully created: " << petName << "!" << endl;
        newPet->displayDetailedInfo();
    }

    void breedPets()
    {
        if (playerPets.size() < 2)
        {
            cout << "\n You need at least 2 pets to breed!" << endl;
            cout << " Create more pets first." << endl;
            return;
        }

        cout << "\n"
             << string(50, '=') << endl;
        cout << " BREEDING SYSTEM" << endl;
        cout << string(50, '=') << endl;

        // 显示可用宠物
        cout << "Available pets:" << endl;
        for (int i = 0; i < playerPets.size(); i++)
        {
            cout << (i + 1) << ". ";
            showPetSummary(playerPets[i]);
        }

        // 选择第一只宠物
        cout << "\nSelect first parent (1-" << playerPets.size() << "): ";
        int parent1Index = getPlayerChoice() - 1;

        if (parent1Index < 0 || parent1Index >= playerPets.size())
        {
            cout << " Invalid choice!" << endl;
            return;
        }

        // 选择第二只宠物
        cout << "Select second parent (1-" << playerPets.size() << "): ";
        int parent2Index = getPlayerChoice() - 1;

        if (parent2Index < 0 || parent2Index >= playerPets.size() || parent2Index == parent1Index)
        {
            cout << " Invalid choice or same pet selected!" << endl;
            return;
        }

        GeneticPet *parent1 = playerPets[parent1Index];
        GeneticPet *parent2 = playerPets[parent2Index];

        // 显示预测结果
        BreedingSystem::predictBreedingOutcome(*parent1, *parent2);

        cout << "\nProceed with breeding? (y/n): ";
        string confirm;
        getline(cin, confirm);

        if (confirm != "y" && confirm != "Y")
        {
            cout << " Breeding cancelled." << endl;
            return;
        }

        // 获取子代名称
        cout << "Enter name for the offspring: ";
        string childName;
        getline(cin, childName);

        if (childName.empty())
        {
            childName = parent1->getName() + "Jr";
        }

        // 执行繁殖
        GeneticPet *child = BreedingSystem::breedPets(*parent1, *parent2, childName);

        if (child)
        {
            playerPets.push_back(child);
            cout << "\n Breeding successful!" << endl;
            child->displayDetailedInfo();
        }
    }

    void battleMenu()
    {
        if (playerPets.empty())
        {
            cout << "\n You need at least one pet to battle!" << endl;
            return;
        }

        cout << "\n"
             << string(50, '=') << endl;
        cout << " BATTLE ARENA" << endl;
        cout << string(50, '=') << endl;

        // 选择宠物
        cout << "Choose your fighter:" << endl;
        for (int i = 0; i < playerPets.size(); i++)
        {
            cout << (i + 1) << ". ";
            showPetSummary(playerPets[i]);
        }

        cout << "Choice: ";
        int petChoice = getPlayerChoice() - 1;

        if (petChoice < 0 || petChoice >= playerPets.size())
        {
            cout << " Invalid pet choice!" << endl;
            return;
        }

        // 选择Boss
        cout << "\nChoose your opponent:" << endl;
        for (int i = 0; i < availableBosses.size(); i++)
        {
            TestBoss *boss = availableBosses[i];
            cout << (i + 1) << ". " << boss->getName()
                 << " (ATK:" << boss->getAttack()
                 << " DEF:" << boss->getDefense()
                 << " HP:" << boss->getMaxHP() << ")" << endl;
        }

        cout << "Choice: ";
        int bossChoice = getPlayerChoice() - 1;

        if (bossChoice < 0 || bossChoice >= availableBosses.size())
        {
            cout << " Invalid boss choice!" << endl;
            return;
        }

        // 开始战斗
        GeneticPet *selectedPet = playerPets[petChoice];
        TestBoss *selectedBoss = availableBosses[bossChoice];

        cout << "\n " << selectedPet->getName() << " vs " << selectedBoss->getName() << "!" << endl;
        cout << "Press ENTER to begin battle...";
        string dummy;
        getline(cin, dummy);

        Battle battle;
        battle.start(selectedPet, selectedBoss);
    }

    void showGameStats()
    {
        cout << "\n"
             << string(50, '=') << endl;
        cout << " GAME STATISTICS" << endl;
        cout << string(50, '=') << endl;

        cout << " Total Pets: " << playerPets.size() << endl;

        // 按物种统计
        int dogs = 0, cats = 0, birds = 0;
        for (GeneticPet *pet : playerPets)
        {
            switch (pet->getSpecies())
            {
            case Species::DOG:
                dogs++;
                break;
            case Species::CAT:
                cats++;
                break;
            case Species::BIRD:
                birds++;
                break;
            }
        }

        cout << "   Dogs: " << dogs << endl;
        cout << "   Cats: " << cats << endl;
        cout << "   Birds: " << birds << endl;

        // 找出最强的宠物
        if (!playerPets.empty())
        {
            GeneticPet *strongest = playerPets[0];
            int maxPower = strongest->getAttack() + strongest->getDefense() + strongest->getSpeed();

            for (GeneticPet *pet : playerPets)
            {
                int power = pet->getAttack() + pet->getDefense() + pet->getSpeed();
                if (power > maxPower)
                {
                    strongest = pet;
                    maxPower = power;
                }
            }

            cout << "\n Strongest Pet: " << strongest->getName()
                 << " (Total Stats: " << maxPower << ")" << endl;
        }

        cout << string(50, '=') << endl;
    }

    void showHelp()
    {
        cout << "\n"
             << string(60, '=') << endl;
        cout << " HELP & INSTRUCTIONS" << endl;
        cout << string(60, '=') << endl;

        cout << " GAME BASICS:" << endl;
        cout << "  • Create pets with random genetic traits" << endl;
        cout << "  • Breed pets to combine and improve genes" << endl;
        cout << "  • Battle bosses to test your pets' strength" << endl;
        cout << "  • Discover rare genes through breeding and mutations" << endl;

        cout << "\n GENETIC SYSTEM:" << endl;
        cout << "  • Stat Genes: Improve attack, defense, speed, or HP" << endl;
        cout << "  • Skill Genes: Grant special combat abilities" << endl;
        cout << "  • Rarity affects inheritance probability" << endl;
        cout << "  • Mutations can create powerful new genes" << endl;

        cout << "\n BREEDING:" << endl;
        cout << "  • Only same species can breed" << endl;
        cout << "  • Child inherits genes based on rarity" << endl;
        cout << "  • 10% chance of beneficial mutations" << endl;
        cout << "  • Rare breed mutations possible" << endl;

        cout << "\n COMBAT:" << endl;
        cout << "  • Turn-based battle system" << endl;
        cout << "  • Manage energy to use powerful skills" << endl;
        cout << "  • Use pet skills and random global skills" << endl;
        cout << "  • Status effects add strategic depth" << endl;

        cout << string(60, '=') << endl;
    }

    void exitGame()
    {
        cout << "\n Thanks for playing the Genetic Virtual Pet System!" << endl;
        cout << " Your pets will be waiting for your return!" << endl;
        gameRunning = false;
    }
};

#endif // GAME_H