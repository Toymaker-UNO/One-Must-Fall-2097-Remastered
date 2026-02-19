#ifndef OPENOMF_GAME_FIGHTSTATS_HPP
#define OPENOMF_GAME_FIGHTSTATS_HPP

#include <cstdint>

namespace openomf {
namespace game {

/** C formats/pilot.h sd_pilot 전방 선언. (Formats에 Pilot 이식 전까지 불투명 포인터로 사용) */
struct SdPilot;

/** C game/protos/fight_stats.h fight_finisher. */
enum class FightFinisher {
    FINISH_NONE = 0,
    FINISH_SCRAP,
    FINISH_DESTRUCTION,
};

constexpr int SOLD_BUF_SIZE = 24;

/** C game/protos/fight_stats.h fight_stats_t. */
struct FightStats {
    int winner = 0;
    unsigned plug_text = 0;
    char sold[SOLD_BUF_SIZE] = {};

    int winnings = 0;
    int bonuses = 0;
    int repair_cost = 0;
    int profit = 0;
    int hp = 0;
    int max_hp = 0;

    FightFinisher finish = FightFinisher::FINISH_NONE;

    SdPilot* challenger = nullptr;

    unsigned hits_landed[2] = {};
    float average_damage[2] = {};
    unsigned total_attacks[2] = {};
    unsigned hit_miss_ratio[2] = {};
};

constexpr unsigned PLUG_TEXT_START = 587;
constexpr int PLUG_ENHANCEMENT = 0;
constexpr int PLUG_FORFEIT = 1;
constexpr int PLUG_LOSE = 2;
constexpr int PLUG_WIN = 7;
constexpr int PLUG_WIN_OK = 10;
constexpr int PLUG_WIN_BIG = 13;
constexpr int PLUG_WARNING = 16;
constexpr int PLUG_KICK_OUT = 17;
constexpr int PLUG_SOLD_UPGRADE = 18;

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_FIGHTSTATS_HPP
