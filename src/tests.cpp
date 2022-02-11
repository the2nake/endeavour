#include <iostream>
#include <vector>
#include "SDL.h"

#include "Player.hpp"

#if __STDC_VERSION__ < 199901L
  #if __GNUC__ >= 2
    #define __func__ __FUNCTION__
  #else
    #define __func__ "<unknown>"
  #endif
#endif

int isEquals(double a, double b)
{
    double epsilon = 0.001;
    return std::abs(a - b) < epsilon;
}

int testTrue(bool a)
{
    if (a) {
        return 1;
        std::cout << "Test succeeded." << std::endl;
    } else {
        return 0;
        std::cout << "Test failed; " << a << " is not true." << std::endl;
    }
}

typedef bool (*testFunction)();

bool testKeyBindMapExecution ()
{
    bool result = true;
    Player player1 = Player();
    Player player2 = Player();

    player1.keybinds.insert_or_assign(SDLK_d, &Player::moveRight);
    player1.keybinds.insert_or_assign(SDLK_a, &Player::moveLeft);
    player1.keybinds.insert_or_assign(SDLK_w, &Player::moveUp);
    player1.keybinds.insert_or_assign(SDLK_s, &Player::moveDown);
    std::unordered_map<int, void (Player::*)()>::iterator iter;

    iter = player1.keybinds.find(SDLK_d);
    (player1.*(iter->second))();
    player2.moveRight();

    if (!(isEquals(player1.getX(), player2.getX()) && isEquals(player1.getY(), player2.getY()))) {
        result = false;
    }

    iter = player1.keybinds.find(SDLK_s);
    (player1.*(iter->second))();
    (player1.*(iter->second))();
    player2.moveDown();
    player2.moveDown();


    if (!(isEquals(player1.getX(), player2.getX()) && isEquals(player1.getY(), player2.getY()))) {
        result = false;
    }

    iter = player1.keybinds.find(SDLK_w);
    (player1.*(iter->second))();
    player2.moveUp();


    if (!(isEquals(player1.getX(), player2.getX()) && isEquals(player1.getY(), player2.getY()))) {
        result = false;
    }

    iter = player1.keybinds.find(SDLK_a);
    (player1.*(iter->second))();
    player2.moveLeft();


    if (!(isEquals(player1.getX(), player2.getX()) && isEquals(player1.getY(), player2.getY()))) {
        result = false;
    }

    if (result) {
        std::cout << "PASS: " << __func__ << std::endl;
    } else {
        std::cout << "FAIL: " << __func__ << std::endl;
    }

    return result;
}

int main()
{
    int passedTotal = 0;
    int testTotal = 0;

    std::vector<testFunction> tests;

    tests.push_back(testKeyBindMapExecution);

    for (int i = 0; i < tests.size(); i++) {
        if ((*tests[i])()) {
            passedTotal += 1;
        }
    }
    testTotal = tests.size();

    

    //passedTotal += testEquals(player1.getX(), player2.getX());
    //testTotal += 1;

    //player1.keybinds.clear();
    //player2.keybinds.clear();
    //player1.keybinds.insert_or_assign(SDLK_a, &Player::moveUp);
    //player1.resetDefaultKeybind();
    //player2.resetDefaultKeybind();

    //passedTotal += testTrue(player1.keybinds.at(SDLK_a) == player2.keybinds.at(SDLK_a));
    //testTotal += 1;

    std::cout << passedTotal << " out of " << testTotal << " tests passed. (" << (passedTotal/(double)(testTotal)) * 100.0 << "%)" << std::endl;
}
