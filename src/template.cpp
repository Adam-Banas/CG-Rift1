#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using std::begin;
using std::cerr;
using std::cout;
using std::end;
using std::endl;
using std::istream;
using std::ostream;
using std::string;
using std::vector;

//#define TESTS
#define DEBUG
#pragma GCC optimize "-O3"

//-------------------------------------------------------------------------------
//--------------------------------- Configuration -------------------------------
//-------------------------------------------------------------------------------

constexpr int TIMEOUT = 98;

//-------------------------------------------------------------------------------
//----------------------------------- Constants ---------------------------------
//-------------------------------------------------------------------------------

//constexpr int ASH_SPEED    = 1000;
//constexpr int SHOT_RANGE   = 2000;
//constexpr int SHOT_RANGE2  = SHOT_RANGE * SHOT_RANGE;
//constexpr int ZOMBIE_SPEED = 400;
//constexpr double PI        = std::acos(-1);

//-------------------------------------------------------------------------------
//----------------------------------- Utilities ---------------------------------
//-------------------------------------------------------------------------------

inline auto& debugLog() noexcept
{
#ifdef DEBUG
    return std::cerr;
#else
    static std::stringstream sink;
    return sink;
#endif
}

int fibonacci[20];

void initializeFibonacci() noexcept
{
    fibonacci[0] = 1;
    fibonacci[1] = 2;
    for (int i = 2; i < 20; ++i)
    {
        fibonacci[i] = fibonacci[i - 1] + fibonacci[i - 2];
    }
}

// Returns a random number in [min, max]
inline int randomNumber(int min, int max)
{
    static std::random_device dev;
    static std::mt19937 gen{dev()};

    std::uniform_int_distribution<> dis(min, max);

    return dis(gen);
}

template <typename T>
inline const T& sample(const std::vector<T>& obj)
{
    return obj[randomNumber(0, obj.size() - 1)];
}

class Point
{
public:
    constexpr explicit Point(int x = 0, int y = 0) noexcept : x(x), y(y) {}

public:
    int x;
    int y;
};

bool operator==(const Point& lhs, const Point& rhs) noexcept
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=(const Point& lhs, const Point& rhs) noexcept
{
    return !(lhs == rhs);
}

auto operator+(const Point& lhs, const Point& rhs) noexcept
{
    return Point(lhs.x + rhs.x, lhs.y + rhs.y);
}

// NOTICE: Needed for sorting algorithms
bool operator<(const Point& lhs, const Point& rhs)
{
    if (lhs.x < rhs.x)
    {
        return true;
    }
    else if (lhs.x > rhs.x)
    {
        return false;
    }
    else
    {
        return lhs.y < rhs.y;
    }
}

std::ostream& operator<<(ostream& out, const Point& obj)
{
    out << obj.x << ' ' << obj.y;
    return out;
}

std::istream& operator>>(istream& in, Point& obj)
{
    in >> obj.x >> obj.y;
    in.ignore();
    return in;
}

// Distance squared, for faster calculations
inline uint dist2(const Point& p1, const Point& p2) noexcept
{
    float dX = abs(p1.x - p2.x);
    float dY = abs(p1.y - p2.y);
    return dX * dX + dY * dY;
}

inline double dist(const Point& p1, const Point& p2) noexcept
{
    return sqrt(dist2(p1, p2));
}

auto toString(const Point& rhs)
{
    string result;

    result = std::to_string(rhs.x) + " " + std::to_string(rhs.y);

    return result;
}

bool flipCoin() noexcept
{
    return randomNumber(0, 1) == 1;
}

std::chrono::high_resolution_clock::time_point turnStart;

inline double msPassed() noexcept
{
    return std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
               std::chrono::high_resolution_clock::now() - turnStart)
        .count();
}

inline void startTurn() noexcept
{
    turnStart = std::chrono::high_resolution_clock::now();
}

inline bool isTimeout() noexcept
{
    return msPassed() >= TIMEOUT;
}

//-------------------------------------------------------------------------------
//------------------------------------- Model -----------------------------------
//-------------------------------------------------------------------------------

namespace model {

class Action;

class State
{
public:
    void simulateTurn(const Action& action) noexcept
    {
    }

public:
    //TODO: State data
};

std::ostream& operator<<(ostream& out, const State& obj)
{
    //TODO: output state data
    return out;
}

} // namespace model

//-------------------------------------------------------------------------------
//------------------------------------ View -------------------------------------
//-------------------------------------------------------------------------------

namespace view {

auto readTurnInput(istream& in)
{
    //TODO: Read turn input

    return model::State();
}

void writeOutput(const model::Action& action)
{
    // TODO: Print / return output
}

} // namespace view

//-------------------------------------------------------------------------------
//--------------------------- Artificial Intelligence ---------------------------
//-------------------------------------------------------------------------------

namespace ai {

class MonteCarlo
{
public:
    auto calcMove(model::State state) noexcept
    {
        std::pair<int, model::Action> best = {-1, model::Action()}, current;
        int states                 = 0;
        while (!isTimeout())
        {
            ++states;
            //            debugLog() << "states: " << states << endl;
            current = oneSimulation(state);
            if (current.first > best.first)
            {
                best = current;
            }
        }
        debugLog() << "Simulation end, states: " << states << endl;
        return best.second;
    }

private:
    std::pair<int, model::Action> oneSimulation(model::State state) const noexcept
    {
        // TODO: Perform one simulation, return score and action

        return {0, model::Action()};
    }
};

} // namespace ai

//-------------------------------------------------------------------------------
//------------------------------------ Main -------------------------------------
//-------------------------------------------------------------------------------

#ifndef TESTS

int main()
{
    initializeFibonacci();
    int score(0);

    ai::MonteCarlo monteCarlo;
    std::chrono::duration<double, std::milli> lastTurnTime;
    while (true)
    {
        auto state = view::readTurnInput(std::cin);
        debugLog() << "Last turn time: " << lastTurnTime.count() << " ms"
                   << endl;
        debugLog() << "Score: " << score << endl;
        startTurn();

        auto move = monteCarlo.calcMove(state);

        cout << move << endl;

        lastTurnTime = std::chrono::high_resolution_clock::now() - turnStart;
    }
}

#else

class Test
{
public:
    static bool exampleTest()
    {
        return true;
    }
};

int main()
{
    bool result;
    result = Test::exampleTest();
    cout << "Example test result: " << std::boolalpha << result << endl;
}

#endif
