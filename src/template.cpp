#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <random>
#include <unordered_set>
#include <vector>

using std::begin;
using std::cerr;
using std::cin;
using std::cout;
using std::end;
using std::endl;
using std::istream;
using std::ostream;
using std::string;
using std::vector;

#define DEBUG
#pragma GCC optimize "-O3"

//-------------------------------------------------------------------------------
//--------------------------------- Configuration -------------------------------
//-------------------------------------------------------------------------------

constexpr int TIMEOUT    = 95;
constexpr int MAP_SIZE   = 154;
constexpr int MAP_PLAYER = 4;
constexpr int POD_COST   = 20;

//-------------------------------------------------------------------------------
//----------------------------------- Constants ---------------------------------
//-------------------------------------------------------------------------------

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

//-------------------------------------------------------------------------------
//----------------------------------- Model -------------------------------------
//-------------------------------------------------------------------------------

namespace model {

int playerCount; // Number of players
int myId;        // My ID

class Map
{
public:
    using Index   = int;
    using Indexes = std::vector<int>;

    // TODO: This is actually state, so it can evolve (e.g. in case of some simulation)
    struct Cell
    {
        static constexpr int NEUTRAL = -1;

        explicit Cell() noexcept { std::fill(begin(pods), end(pods), 0); }

        int owner = NEUTRAL;
        int pods[MAP_PLAYER];
    };

public:
    const Indexes& getNeighbors(Index a) const noexcept;
    Cell& getCell(Index a) noexcept;
    auto getPlatinumSource(Index a) const noexcept;

    void addLink(Index a, Index b) noexcept;
    void setPlatinumSource(Index a, int value) noexcept;

private:
    int platinumSource[MAP_SIZE];
    Indexes neighbors[MAP_SIZE];
    Cell cells[MAP_SIZE];
};

Map map;

const Map::Indexes& Map::getNeighbors(Index a) const noexcept
{
    assert((a % MAP_SIZE) == a);
    return neighbors[a];
}

Map::Cell& Map::getCell(Index a) noexcept
{
    assert((a % MAP_SIZE) == a);
    return cells[a];
}

auto Map::getPlatinumSource(Index a) const noexcept
{
    assert((a % MAP_SIZE) == a);
    return platinumSource[a];
}

void Map::addLink(Index a, Index b) noexcept
{
    assert((a % MAP_SIZE) == a);
    assert((b % MAP_SIZE) == b);
    neighbors[a].push_back(b);
    neighbors[b].push_back(a);
}

void Map::setPlatinumSource(Index a, int value) noexcept
{
    assert((a % MAP_SIZE) == a);
    platinumSource[a] = value;
}

class Player
{
public:
    int platinum;
    std::vector<int> pods;
};

class State
{
public:
    Player me;
};

} // namespace model

//-------------------------------------------------------------------------------
//------------------------------------ View -------------------------------------
//-------------------------------------------------------------------------------

namespace view {

void readInitializationInput(istream& in)
{
    int player_count; // the amount of players (2 to 4)
    int zoneCount;    // the amount of zones on the map
    int linkCount;    // the amount of links between all zones
    cin >> model::playerCount >> model::myId >> zoneCount >> linkCount;
    debugLog() << "zoneCount: " << zoneCount << "\n";
    cin.ignore();
    for (int i = 0; i < zoneCount; i++)
    {
        int zoneId;         // this zone's ID (between 0 and zoneCount-1)
        int platinumSource; // the amount of Platinum this zone can provide per game turn
        cin >> zoneId >> platinumSource;
        debugLog() << "zoneId: " << zoneId << std::endl;
        model::map.setPlatinumSource(zoneId, platinumSource);
        cin.ignore();
    }
    for (int i = 0; i < linkCount; i++)
    {
        int zone1;
        int zone2;
        cin >> zone1 >> zone2;
        model::map.addLink(zone1, zone2);
        cin.ignore();
    }
}

auto readTurnInput(istream& in)
{
    model::State state;

    int platinum; // my available Platinum
    cin >> platinum;
    state.me.platinum = platinum;
    cin.ignore();
    for (int i = 0; i < MAP_SIZE; i++)
    {
        int zoneId;  // this zone's ID
        int ownerId; // the player who owns this zone (-1 otherwise)
        int podsP0;  // player 0's PODs on this zone
        int podsP1;  // player 1's PODs on this zone
        int podsP2;  // player 2's PODs on this zone (always 0 for a two
                     // player game)
        int podsP3;  // player 3's PODs on this zone (always 0 for a two or
                     // three player game)
        cin >> zoneId >> ownerId >> podsP0 >> podsP1 >> podsP2 >> podsP3;
        cin.ignore();
        auto& cell   = model::map.getCell(zoneId);
        cell.owner   = ownerId;
        cell.pods[0] = podsP0;
        cell.pods[1] = podsP1;
        cell.pods[2] = podsP2;
        cell.pods[3] = podsP3;
    }

    return state;
}

} // namespace view

//-------------------------------------------------------------------------------
//--------------------------- Artificial Intelligence ---------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//------------------------------------ Main -------------------------------------
//-------------------------------------------------------------------------------

#ifndef TESTS

int main()
{
    view::readInitializationInput(cin);

    // game loop
    while (1)
    {
        auto state = view::readTurnInput(cin);

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        // first line for movement commands, second line for POD purchase (see the protocol in the
        // statement for details)
        cout << "WAIT" << endl;
        //        cout << "1 73" << endl;

        while (state.me.platinum >= POD_COST)
        {
            int best      = randomNumber(0, MAP_SIZE - 1);
            int bestValue = 0;
            for (int i = 0; i < MAP_SIZE; ++i)
            {
                if (model::map.getPlatinumSource(i) > bestValue
                    && model::map.getCell(i).owner == model::Map::Cell::NEUTRAL)
                {
                    best      = i;
                    bestValue = model::map.getPlatinumSource(i);
                }
            }

            state.me.platinum -= POD_COST;
            cout << "1 " << best << " ";
            model::map.getCell(best).owner = model::myId;
        }
        cout << endl;
    }
}

#else

#endif
