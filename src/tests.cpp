#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include "SDL.h"

#include "Player.hpp"
#include "TextureManager.hpp"
#include "common.hpp"
#include "Line.hpp"
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

/*
test template:

bool test___() {
    bool result = false;

    if (result)
    {
        std::cout << "\033[1;32mPASS:\033[0m " << __func__ << std::endl;
    }
    else
    {
        std::cout << "\033[1;31mFAIL:\033[0m " << __func__ << std::endl;
    }

    return result;
}
*/

bool isEqual(double a, double b)
{
    double epsilon = 0.0001;
    return std::abs(a - b) < epsilon;
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
        std::cout << "\033[1;32mPASS:\033[0m " << __func__ << std::endl;
    }
    else
    {
        std::cout << "\033[1;31mFAIL:\033[0m " << __func__ << std::endl;
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
        std::cout << "\033[1;32mPASS:\033[0m " << __func__ << std::endl;
    }
    else
    {
        std::cout << "\033[1;31mFAIL:\033[0m " << __func__ << std::endl;
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
        std::cout << "\033[1;32mPASS:\033[0m " << __func__ << std::endl;
    }
    else
    {
        std::cout << "\033[1;31mFAIL:\033[0m " << __func__ << std::endl;
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
        std::cout << "\033[1;32mPASS:\033[0m " << __func__ << std::endl;
    }
    else
    {
        std::cout << "\033[1;31mFAIL:\033[0m " << __func__ << std::endl;
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
        SDL_Texture *target = nullptr;
        TextureManager::retriveCachedTexture(target, "test_res/tex/tilemap_proto.png");
        result = target != nullptr;
        TextureManager::loadTexture("test_res/tex/tilemap_proto.png");
        result = result && TextureManager::textureCacheUsed;
    }
    if (result)
    {
        std::cout << "\033[1;32mPASS:\033[0m " << __func__ << std::endl;
    }
    else
    {
        std::cout << "\033[1;31mFAIL:\033[0m " << __func__ << std::endl;
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
        std::cout << "\033[1;32mPASS:\033[0m " << __func__ << std::endl;
    }
    else
    {
        std::cout << "\033[1;31mFAIL:\033[0m " << __func__ << std::endl;
    }

    return result;
}

bool testPolymorphicDeconstructorClean()
{
    // Test if calling ai->~Entity() deconstructs properly
    bool result = false;

    AI ai = AI();
    ai.init(0, 0, TextureManager::loadTexture("test_res/tex/tilemap_proto.png"));
    SDL_Texture *tex = ai.getTexture();
    Entity *ai_ptr = &ai;
    ai_ptr->~Entity();

    SDL_RenderCopy(Game::renderer, tex, nullptr, nullptr);
    const char *err = SDL_GetError();
    result = tex != nullptr && (std::string)(err) == (std::string)("Invalid texture");

    if (result)
    {
        std::cout << "\033[1;32mPASS:\033[0m " << __func__ << std::endl;
    }
    else
    {
        std::cout << "\033[1;31mFAIL:\033[0m " << __func__ << std::endl;
    }

    return result;
}

bool testLevelLoading()
{
    bool result = true;

    if (Level::entities.size() > 0)
    {
        if (Level::entities[0]->getStringAttribute("name") == "Anthony")
        {
            // background layer 0
            Tile tile = Level::getTileFromName(Level::getTileNameAtPos("background", 0, 31, 31));
            if (tile.texture != nullptr && tile.isNatural && tile.movementCost == 1)
            {
                result = result && true;
            }
            tile = Level::getTileFromName(Level::getTileNameAtPos("background", 0, 32, 32));
            if (tile.texture != nullptr && !tile.isNatural && tile.movementCost == -1)
            {
                result = result && true;
            }
            // foreground tests (different layers)
            tile = Level::getTileFromName(Level::getTileNameAtPos("foreground", 0, 45, 36));
            if (tile.texture != nullptr && !tile.isNatural && tile.movementCost == 1)
            {
                result = result && true;
            }
            tile = Level::getTileFromName(Level::getTileNameAtPos("foreground", 1, 32, 32));
            if (tile.texture != nullptr && !tile.isNatural && tile.movementCost == -1)
            {
                result = result && true;
            }
        }
        else
        {
            result = false;
        }
    }
    else
    {
        result = false;
    }

    if (result)
    {
        std::cout << "\033[1;32mPASS:\033[0m " << __func__ << std::endl;
    }
    else
    {
        std::cout << "\033[1;31mFAIL:\033[0m " << __func__ << std::endl;
    }

    return result;
}

bool testFloatingPointModulo()
{
    bool result = true;

    if (!isEqual(floatingPointModulo(1.0, 180.0), 1.0))
    {
        result = false;
    }

    if (!isEqual(floatingPointModulo(-192.0, 20.0), 8.0))
    {
        result = false;
    }

    if (!isEqual(floatingPointModulo(10.34, 1.53), 1.16))
    {
        result = false;
    }

    if (!isEqual(floatingPointModulo(2.0, 2.0), 0.0))
    {
        result = false;
    }

    if (!isEqual(floatingPointModulo(4.5, 2.25), 0.0))
    {
        result = false;
    }

    if (result)
    {
        std::cout << "\033[1;32mPASS:\033[0m " << __func__ << std::endl;
    }
    else
    {
        std::cout << "\033[1;31mFAIL:\033[0m " << __func__ << std::endl;
    }

    return result;
}

bool testLineSlope()
{
    bool result = true;

    Line line = Line(0, 0, 1, 1);

    if (!isEqual(line.getSlope(), 1.0))
    {
        result = false;
    }

    line = Line(0, 0, 0, 1);

    if (line.getSlope() < 10000)
    {
        result = false;
    }

    line = Line(-1.0f, 1.0f, 2.0f, 3.5f);

    if (!isEqual(line.getSlope(), 5.0f / 6.0f))
    {
        result = false;
    }

    line = Line(2.3f, 10.3f, -12.0f, 3.42f);

    if (!isEqual(line.getSlope(), 0.48111888))
    {
        result = false;
    }

    line = Line(-2.55f, -0.37f, 2.6f, -1.69f);

    if (!isEqual(line.getSlope(), -0.25631068))
    {
        result = false;
    }

    if (result)
    {
        std::cout << "\033[1;32mPASS:\033[0m " << __func__ << std::endl;
    }
    else
    {
        std::cout << "\033[1;31mFAIL:\033[0m " << __func__ << std::endl;
    }

    return result;
}

bool testLineAngle()
{
    bool result = true;

    Line line = Line(-1, 1, 1, 1);

    if (!isEqual(line.getAngleFromPosXAxis(false), 0))
    {
        result = false;
    }

    if (!isEqual(line.getAngleFromPosXAxis(true), 0))
    {
        result = false;
    }

    line = Line(0, 0, 0, 1);

    if (!isEqual(line.getAngleFromPosXAxis(false), 90))
    {
        result = false;
    }

    line = Line(-0.02f, 1.1f, 2.3f, -3.9f);

    if (!isEqual(line.getAngleFromPosXAxis(false), 114.891300125))
    {
        result = false;
    }

    if (!isEqual(line.getAngleFromPosXAxis(true), 2.00523146908))
    {
        result = false;
    }

    line = Line(-8.1f, 1.1f, 3.7f, -3.9f);

    if (!isEqual(line.getAngleFromPosXAxis(false), 157.03622694))
    {
        result = false;
    }

    if (!isEqual(line.getAngleFromPosXAxis(true), 2.740799205))
    {
        result = false;
    }

    if (result)
    {
        std::cout << "\033[1;32mPASS:\033[0m " << __func__ << std::endl;
    }
    else
    {
        std::cout << "\033[1;31mFAIL:\033[0m " << __func__ << std::endl;
    }

    return result;
}

bool testLineIntersection()
{
    bool result = true;

    Line line1 = Line(1, 1, 10, 1);
    Line line2 = Line(1, 2, 10, 2);

    if (line1.intersectsLine(&line2) || line2.intersectsLine(&line1))
    {
        result = false;
    }

    line1 = Line(10, 0, 0, 10);
    line2 = Line(0, 0, 10, 10);

    if ((!line1.intersectsLine(&line2)) || (!line2.intersectsLine(&line1)))
    {
        result = false;
    }

    line1 = Line(-5, -5, 0, 0);
    line2 = Line(1, 1, 10, 10);

    if (line1.intersectsLine(&line2) || line2.intersectsLine(&line1))
    {
        result = false;
    }

    line1 = Line(-5.0f, 0.1f, 5.0f, 0.1f);
    line2 = Line(3.0f, 0.1f, 3.0f, 10.0f);

    if ((!line1.intersectsLine(&line2)) || (!line2.intersectsLine(&line1)))
    {
        result = false;
    }

    line1 = Line(-5.0f, 0.1f, 5.0f, 0.1f);
    line2 = Line(3.0f, 0.2f, 3.0f, 10.0f);

    if (line1.intersectsLine(&line2) || line2.intersectsLine(&line1))
    {
        result = false;
    }

    if (result)
    {
        std::cout << "\033[1;32mPASS:\033[0m " << __func__ << std::endl;
    }
    else
    {
        std::cout << "\033[1;31mFAIL:\033[0m " << __func__ << std::endl;
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
             &testLevelLoading,
             &testFloatingPointModulo,
             &testLineSlope,
             &testLineAngle,
             &testLineIntersection};
    // ---
    for (uint i = 0; i < tests.size(); i++)
    {
        if ((*tests[i])())
        {
            passedTotal += 1;
        }
    }
    testTotal = tests.size();

    std::cout << passedTotal << " out of " << testTotal << " tests passed." << std::endl;
    game.clean();
}
