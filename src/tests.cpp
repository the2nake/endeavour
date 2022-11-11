#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include "SDL.h"

#include "Player.hpp"
#include "TextureManager.hpp"
#include "common.hpp"
#include "pathfinding.hpp"
#include "Game.hpp"
#include "AI.hpp"
#include "Entity.hpp"
#include "Level.hpp"

#if __STDC_VERSION__ < 199901L
#if __GNUC__ >= 2
#define __func__ __FUNCTION__
#else
#define __func__ "<unknown>"
#endif
#endif

#define TEST_TEXTURE_CACHE
// Tells TextureManager to report when the texture cache has been accessed

int isEqual(double a, double b)
{
    double epsilon = 0.001;
    return std::abs(a - b) < epsilon;
}

int testTrue(bool a)
{
    if (a)
    {
        return 1;
        std::cout << "Test succeeded." << std::endl;
    }
    else
    {
        return 0;
        std::cout << "Test failed; " << a << " is not true." << std::endl;
    }
}

typedef bool (*testFunction)();

bool testKeyBindMapExecution()
{
    bool result = true;
    Player player1 = Player();
    Player player2 = Player();

    player1.initAliasMap();
    player1.initAliasMap();

    player1.keybinds.insert_or_assign(SDLK_d, "callbackMoveRight");
    player1.keybinds.insert_or_assign(SDLK_a, "callbackMoveLeft");
    player1.keybinds.insert_or_assign(SDLK_w, "callbackMoveUp");
    player1.keybinds.insert_or_assign(SDLK_s, "callbackMoveDown");
    std::unordered_map<int, std::string>::iterator iter;

    iter = player1.keybinds.find(SDLK_d);
    (player1.*player1.getFunctionOf(iter->second))();
    player2.callbackMoveRight();

    if (!(isEqual(player1.getX(), player2.getX()) && isEqual(player1.getY(), player2.getY())))
    {
        result = false;
    }

    iter = player1.keybinds.find(SDLK_s);
    (player1.*player1.getFunctionOf(iter->second))();
    (player1.*player1.getFunctionOf(iter->second))();
    player2.callbackMoveDown();
    player2.callbackMoveDown();

    if (!(isEqual(player1.getX(), player2.getX()) && isEqual(player1.getY(), player2.getY())))
    {
        result = false;
    }

    iter = player1.keybinds.find(SDLK_w);
    (player1.*player1.getFunctionOf(iter->second))();
    player2.callbackMoveUp();

    if (!(isEqual(player1.getX(), player2.getX()) && isEqual(player1.getY(), player2.getY())))
    {
        result = false;
    }

    iter = player1.keybinds.find(SDLK_a);
    (player1.*player1.getFunctionOf(iter->second))();
    player2.callbackMoveLeft();

    if (!(isEqual(player1.getX(), player2.getX()) && isEqual(player1.getY(), player2.getY())))
    {
        result = false;
    }

    if (result)
    {
        std::cout << "PASS: " << __func__ << std::endl;
    }
    else
    {
        std::cout << "FAIL: " << __func__ << std::endl;
    }

    return result;
}

bool testKeyBindReset()
{
    Player player1 = Player();
    Player player2 = Player();

    player1.keybinds.clear();
    player2.keybinds.clear();
    player1.keybinds.insert_or_assign(SDLK_a, "callbackMoveUp");
    player1.resetDefaultKeybind();
    player2.resetDefaultKeybind();

    bool result = player1.keybinds.at(SDLK_a) == player2.keybinds.at(SDLK_a);

    if (result)
    {
        std::cout << "PASS: " << __func__ << std::endl;
    }
    else
    {
        std::cout << "FAIL: " << __func__ << std::endl;
    }

    return result;
}

bool testStringSplit()
{
    std::vector<std::string> container;
    splitString(container, "a, b, c, d, e", ", ");
    std::vector<std::string> expected;
    expected.push_back("a");
    expected.push_back("b");
    expected.push_back("c");
    expected.push_back("d");
    expected.push_back("e");
    bool result = container == expected;

    splitString(container, "a>=bdad>=casf>=agd>=ae", ">=");
    expected.clear();
    expected.push_back("a");
    expected.push_back("bdad");
    expected.push_back("casf");
    expected.push_back("agd");
    expected.push_back("ae");
    result = (container == expected) && result;

    if (result)
    {
        std::cout << "PASS: " << __func__ << std::endl;
    }
    else
    {
        std::cout << "FAIL: " << __func__ << std::endl;
    }

    return result;
}

bool testStringToRect()
{

    SDL_Rect rect = stringToSDLRect("0 0", " ");

    bool result = rect.x == 0 && rect.y == 0;

    rect = stringToSDLRect("0 0", " ");
    result = rect.x == 0 && rect.y == 0;

    rect = stringToSDLRect("01 -11 31 21", " ");
    result = (rect.x == 1 && rect.y == -11 && rect.w == 31 && rect.h == 21) && result;

    if (result)
    {
        std::cout << "PASS: " << __func__ << std::endl;
    }
    else
    {
        std::cout << "FAIL: " << __func__ << std::endl;
    }

    return result;
}

bool testTextureCache()
{
    // TextureManager::loadTexture();
    // load, check if cached
    // load again, check if cache was used by setting a test flag (#ifdef PRINT_IF_USED_CACHE)
    bool result = false;
    if (!std::filesystem::exists("test_res/tex/tilemap_proto.png"))
    {
        std::cout << "" << std::endl;
    }
    else
    {
        TextureManager::loadTexture("test_res/tex/tilemap_proto.png");
        result = TextureManager::retriveCachedTexture("test_res/tex/tilemap_proto.png");
        TextureManager::loadTexture("test_res/tex/tilemap_proto.png");
        result = result && TextureManager::textureCacheUsed;
    }
    if (result)
    {
        std::cout << "PASS: " << __func__ << std::endl;
    }
    else
    {
        std::cout << "FAIL: " << __func__ << std::endl;
    }

    return result;
}

bool testEntityClean()
{
    bool result = false;

    Entity ent = Entity();
    ent.init(0, 0, TextureManager::loadTexture("test_res/tex/tilemap_proto.png"));
    ent.clean();
    SDL_RenderCopy(Game::renderer, ent.getTexture(), nullptr, nullptr);
    const char *err = SDL_GetError();
    result = (std::string)(err) == (std::string)("Invalid texture"); // should have an error because the texture should be deleted

    if (result)
    {
        std::cout << "PASS: " << __func__ << std::endl;
    }
    else
    {
        std::cout << "FAIL: " << __func__ << std::endl;
    }

    return result;
}

bool testPolymorphicDeconstructorClean()
{
    // Test if calling ai->~Entity() deconstructs properly
    bool result = false;

    AI ai = AI();
    ai.init(0, 0, TextureManager::loadTexture("test_res/tex/tilemap_proto.png"));
    SDL_Texture* tex = ai.getTexture();
    Entity* ai_ptr = &ai;
    ai_ptr->~Entity();
    
    SDL_RenderCopy(Game::renderer, tex, nullptr, nullptr);
    const char *err = SDL_GetError();
    result = tex != nullptr && (std::string)(err) == (std::string)("Invalid texture");

    if (result)
    {
        std::cout << "PASS: " << __func__ << std::endl;
    }
    else
    {
        std::cout << "FAIL: " << __func__ << std::endl;
    }

    return result;
}

bool testLevelLoading() {
    bool result = false;

    Level::loadPlayerData("../test_res/saves/TestPlayer", "save_1");
    Level::loadLevel("../test_res/saves/TestPlayer", "save_1", "level1");

    if (Level::entities.size()) {
        if (Level::entities[0]->getStringAttribute("name") == "Anthony") {
            // background layer 0
            Tile tile = Level::getTileFromName(Level::getTileNameAtPosition("background", 0, 31, 31));
            if (tile.texture != nullptr && tile.isNatural && tile.movementCost == 1) {
                result = true;
            }
            tile = Level::getTileFromName(Level::getTileNameAtPosition("background", 0, 32, 32));
            if (tile.texture != nullptr && !tile.isNatural && tile.movementCost == -1) {
                result = result && true;
            }
            // foreground tests (different layers)
            tile = Level::getTileFromName(Level::getTileNameAtPosition("foreground", 0, 45, 36));
            if (tile.texture != nullptr && !tile.isNatural && tile.movementCost == 1) {
                result = result && true;
            }
            tile = Level::getTileFromName(Level::getTileNameAtPosition("foreground", 1, 32, 32));
            if (tile.texture != nullptr && !tile.isNatural && tile.movementCost == -1) {
                result = result && true;
            }
        } else {
            result = false;
        }
    } else {
        result = false;
    }

    if (result)
    {
        std::cout << "PASS: " << __func__ << std::endl;
    }
    else
    {
        std::cout << "FAIL: " << __func__ << std::endl;
    }

    return result;
}

int main()
{
    auto path = std::filesystem::current_path();
    bool workingDirIsCorrect = std::filesystem::exists(path.string() + "/res") &&
                               std::filesystem::exists(path.string() + "/test_res/tex") &&
                               std::filesystem::exists(path.string() + "/test_res/saves") &&
                               std::filesystem::exists(path.string() + "/saves");
    if (!workingDirIsCorrect)
    {
        std::cout << "The game has started in the wrong working directory. Attempting to correct working directory." << std::endl;
        std::filesystem::current_path(path.string() + "/..");
    }

    int passedTotal = 0;
    int testTotal = 0;

    std::vector<testFunction> tests;

    // necessary to test texture loading
    Game game = Game("Test Window", 0, 0, false, false);

    // --- add more tests in this section ---
    tests = {&testKeyBindMapExecution,
             &testKeyBindReset,
             &testStringSplit,
             &testStringToRect,
             &testTextureCache,
             &testEntityClean,
             &testPolymorphicDeconstructorClean,
             &testLevelLoading};
    // ---
    for (int i = 0; i < tests.size(); i++)
    {
        if ((*tests[i])())
        {
            passedTotal += 1;
        }
    }
    testTotal = tests.size();

    std::cout << passedTotal << " out of " << testTotal << " tests passed. (" << (passedTotal / (double)(testTotal)) * 100.0 << "%)" << std::endl;
    game.clean();
}
