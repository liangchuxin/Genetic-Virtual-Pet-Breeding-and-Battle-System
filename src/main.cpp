// #include <iostream>
// using namespace std;

// #include "battle/BattleEngine.h"

// int main() {
//     srand(static_cast<unsigned int>(time(0)));
//     cout << "Genetic Virtual Pet System" << endl;
//     Battle battle;
//     battle.start();
//     return 0;
// }

#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;

#include "ui/Game.h"

int main()
{
    try
    {
        cout << " Initializing Genetic Virtual Pet System..." << endl;

        // 初始化随机数种子
        srand(static_cast<unsigned int>(time(0)));

        // 创建并运行游戏
        Game game;
        game.run();

        cout << "\n Game ended successfully!" << endl;
    }
    catch (const exception &e)
    {
        cout << "\n Fatal error occurred: " << e.what() << endl;
        cout << " Please restart the game." << endl;
        return 1;
    }
    catch (...)
    {
        cout << "\n Unknown error occurred!" << endl;
        cout << " Please restart the game." << endl;
        return 1;
    }

    return 0;
}