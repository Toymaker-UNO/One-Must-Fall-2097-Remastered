#ifndef OPENOMF_RESOURCES_IDS_HPP
#define OPENOMF_RESOURCES_IDS_HPP

namespace openomf {
namespace resources {

/** C resources/ids.h resource_id 대응. */
enum class ResourceId : unsigned int {
    BK_INTRO = 0,
    BK_OPENOMF,
    BK_MENU,
    BK_END,
    BK_END1,
    BK_END2,
    BK_CREDITS,
    BK_MECHLAB,
    BK_VS,
    BK_MELEE,
    BK_NEWSROOM,
    BK_ARENA0,
    BK_ARENA1,
    BK_ARENA2,
    BK_ARENA3,
    BK_ARENA4,
    AF_JAGUAR,
    AF_SHADOW,
    AF_THORN,
    AF_PYROS,
    AF_ELECTRA,
    AF_KATANA,
    AF_SHREDDER,
    AF_FLAIL,
    AF_GARGOYLE,
    AF_CHRONOS,
    AF_NOVA,
    PSM_END,
    PSM_MENU,
    PSM_ARENA0,
    PSM_ARENA1,
    PSM_ARENA2,
    PSM_ARENA3,
    PSM_ARENA4,
    DAT_SOUNDS,
    DAT_GRAPHCHR,
    DAT_CHARSMAL,
    DAT_ALTPALS,
    PIC_PLAYERS,
    PCX_NETARENA,
    PCX_NETFONT1,
    PCX_NETFONT2,
    NUMBER_OF_RESOURCES
};

inline const char* get_resource_file(unsigned int id) {
    switch (id) {
        case static_cast<unsigned int>(ResourceId::BK_INTRO):    return "INTRO.BK";
        case static_cast<unsigned int>(ResourceId::BK_OPENOMF):   return "openomf.bk";
        case static_cast<unsigned int>(ResourceId::BK_MENU):      return "MAIN.BK";
        case static_cast<unsigned int>(ResourceId::BK_ARENA0):    return "ARENA0.BK";
        case static_cast<unsigned int>(ResourceId::BK_ARENA1):    return "ARENA1.BK";
        case static_cast<unsigned int>(ResourceId::BK_ARENA2):    return "ARENA2.BK";
        case static_cast<unsigned int>(ResourceId::BK_ARENA3):     return "ARENA3.BK";
        case static_cast<unsigned int>(ResourceId::BK_ARENA4):     return "ARENA4.BK";
        case static_cast<unsigned int>(ResourceId::BK_NEWSROOM):  return "NEWSROOM.BK";
        case static_cast<unsigned int>(ResourceId::BK_END):       return "END.BK";
        case static_cast<unsigned int>(ResourceId::BK_END1):     return "END1.BK";
        case static_cast<unsigned int>(ResourceId::BK_END2):     return "END2.BK";
        case static_cast<unsigned int>(ResourceId::BK_CREDITS):  return "CREDITS.BK";
        case static_cast<unsigned int>(ResourceId::BK_MECHLAB):   return "MECHLAB.BK";
        case static_cast<unsigned int>(ResourceId::BK_MELEE):    return "MELEE.BK";
        case static_cast<unsigned int>(ResourceId::BK_VS):       return "VS.BK";
        case static_cast<unsigned int>(ResourceId::AF_JAGUAR):   return "FIGHTR0.AF";
        case static_cast<unsigned int>(ResourceId::AF_SHADOW):   return "FIGHTR1.AF";
        case static_cast<unsigned int>(ResourceId::AF_THORN):     return "FIGHTR2.AF";
        case static_cast<unsigned int>(ResourceId::AF_PYROS):    return "FIGHTR3.AF";
        case static_cast<unsigned int>(ResourceId::AF_ELECTRA):  return "FIGHTR4.AF";
        case static_cast<unsigned int>(ResourceId::AF_KATANA):   return "FIGHTR5.AF";
        case static_cast<unsigned int>(ResourceId::AF_SHREDDER): return "FIGHTR6.AF";
        case static_cast<unsigned int>(ResourceId::AF_FLAIL):     return "FIGHTR7.AF";
        case static_cast<unsigned int>(ResourceId::AF_GARGOYLE): return "FIGHTR8.AF";
        case static_cast<unsigned int>(ResourceId::AF_CHRONOS):  return "FIGHTR9.AF";
        case static_cast<unsigned int>(ResourceId::AF_NOVA):     return "FIGHTR10.AF";
        case static_cast<unsigned int>(ResourceId::PSM_MENU):    return "MENU.PSM";
        case static_cast<unsigned int>(ResourceId::PSM_END):     return "END.PSM";
        case static_cast<unsigned int>(ResourceId::PSM_ARENA0):  return "ARENA0.PSM";
        case static_cast<unsigned int>(ResourceId::PSM_ARENA1):  return "ARENA1.PSM";
        case static_cast<unsigned int>(ResourceId::PSM_ARENA2):  return "ARENA2.PSM";
        case static_cast<unsigned int>(ResourceId::PSM_ARENA3):  return "ARENA3.PSM";
        case static_cast<unsigned int>(ResourceId::PSM_ARENA4):  return "ARENA4.PSM";
        case static_cast<unsigned int>(ResourceId::DAT_SOUNDS):  return "SOUNDS.DAT";
        case static_cast<unsigned int>(ResourceId::DAT_GRAPHCHR):  return "GRAPHCHR.DAT";
        case static_cast<unsigned int>(ResourceId::DAT_CHARSMAL): return "CHARSMAL.DAT";
        case static_cast<unsigned int>(ResourceId::DAT_ALTPALS):  return "ALTPALS.DAT";
        case static_cast<unsigned int>(ResourceId::PIC_PLAYERS):   return "PLAYERS.PIC";
        case static_cast<unsigned int>(ResourceId::PCX_NETARENA): return "NETARENA.PCX";
        case static_cast<unsigned int>(ResourceId::PCX_NETFONT1): return "NETFONT1.PCX";
        case static_cast<unsigned int>(ResourceId::PCX_NETFONT2): return "NETFONT2.PCX";
        default: break;
    }
    return nullptr;
}

inline const char* get_resource_name(unsigned int id) {
    switch (id) {
        case static_cast<unsigned int>(ResourceId::BK_INTRO):    return "BK_INTRO";
        case static_cast<unsigned int>(ResourceId::BK_OPENOMF): return "BK_OPENOMF";
        case static_cast<unsigned int>(ResourceId::BK_MENU):     return "BK_MAIN";
        case static_cast<unsigned int>(ResourceId::BK_ARENA0):   return "BK_ARENA0";
        case static_cast<unsigned int>(ResourceId::BK_ARENA1):   return "BK_ARENA1";
        case static_cast<unsigned int>(ResourceId::BK_ARENA2):   return "BK_ARENA2";
        case static_cast<unsigned int>(ResourceId::BK_ARENA3):   return "BK_ARENA3";
        case static_cast<unsigned int>(ResourceId::BK_ARENA4):   return "BK_ARENA4";
        case static_cast<unsigned int>(ResourceId::BK_NEWSROOM):  return "BK_NEWSROOM";
        case static_cast<unsigned int>(ResourceId::BK_END):      return "BK_END";
        case static_cast<unsigned int>(ResourceId::BK_END1):     return "BK_END1";
        case static_cast<unsigned int>(ResourceId::BK_END2):     return "BK_END2";
        case static_cast<unsigned int>(ResourceId::BK_CREDITS):   return "BK_CREDITS";
        case static_cast<unsigned int>(ResourceId::BK_MECHLAB):   return "BK_MECHLAB";
        case static_cast<unsigned int>(ResourceId::BK_MELEE):    return "BK_MELEE";
        case static_cast<unsigned int>(ResourceId::BK_VS):       return "BK_VS";
        case static_cast<unsigned int>(ResourceId::AF_JAGUAR):   return "AF_JAGUAR";
        case static_cast<unsigned int>(ResourceId::AF_SHADOW):   return "AF_SHADOW";
        case static_cast<unsigned int>(ResourceId::AF_THORN):     return "AF_THORN";
        case static_cast<unsigned int>(ResourceId::AF_PYROS):    return "AF_PYROS";
        case static_cast<unsigned int>(ResourceId::AF_ELECTRA):  return "AF_ELECTRA";
        case static_cast<unsigned int>(ResourceId::AF_KATANA):   return "AF_KATANA";
        case static_cast<unsigned int>(ResourceId::AF_SHREDDER): return "AF_SHREDDER";
        case static_cast<unsigned int>(ResourceId::AF_FLAIL):     return "AF_FLAIL";
        case static_cast<unsigned int>(ResourceId::AF_GARGOYLE): return "AF_GARGOYLE";
        case static_cast<unsigned int>(ResourceId::AF_CHRONOS):  return "AF_CHRONOS";
        case static_cast<unsigned int>(ResourceId::AF_NOVA):     return "AF_NOVA";
        case static_cast<unsigned int>(ResourceId::PSM_MENU):    return "PSM_MENU";
        case static_cast<unsigned int>(ResourceId::PSM_END):     return "PSM_END";
        case static_cast<unsigned int>(ResourceId::PSM_ARENA0):   return "PSM_ARENA0";
        case static_cast<unsigned int>(ResourceId::PSM_ARENA1):   return "PSM_ARENA1";
        case static_cast<unsigned int>(ResourceId::PSM_ARENA2):   return "PSM_ARENA2";
        case static_cast<unsigned int>(ResourceId::PSM_ARENA3):   return "PSM_ARENA3";
        case static_cast<unsigned int>(ResourceId::PSM_ARENA4):   return "PSM_ARENA4";
        case static_cast<unsigned int>(ResourceId::DAT_SOUNDS):   return "DAT_SOUNDS";
        case static_cast<unsigned int>(ResourceId::DAT_GRAPHCHR): return "DAT_GRAPHCHR";
        case static_cast<unsigned int>(ResourceId::DAT_CHARSMAL): return "DAT_CHARSMAL";
        case static_cast<unsigned int>(ResourceId::DAT_ALTPALS):  return "DAT_ALTPALS";
        case static_cast<unsigned int>(ResourceId::PIC_PLAYERS):  return "PIC_PLAYERS";
        case static_cast<unsigned int>(ResourceId::PCX_NETARENA):  return "PCX_NETARENA";
        default: break;
    }
    return nullptr;
}

inline bool is_arena(unsigned int id) {
    return id >= static_cast<unsigned int>(ResourceId::BK_ARENA0)
        && id <= static_cast<unsigned int>(ResourceId::BK_ARENA4);
}

inline bool is_har(unsigned int id) {
    return id >= static_cast<unsigned int>(ResourceId::AF_JAGUAR)
        && id <= static_cast<unsigned int>(ResourceId::AF_NOVA);
}

inline bool is_music(unsigned int id) {
    switch (id) {
        case static_cast<unsigned int>(ResourceId::PSM_MENU):
        case static_cast<unsigned int>(ResourceId::PSM_END):
        case static_cast<unsigned int>(ResourceId::PSM_ARENA0):
        case static_cast<unsigned int>(ResourceId::PSM_ARENA1):
        case static_cast<unsigned int>(ResourceId::PSM_ARENA2):
        case static_cast<unsigned int>(ResourceId::PSM_ARENA3):
        case static_cast<unsigned int>(ResourceId::PSM_ARENA4):
            return true;
    }
    return false;
}

} // namespace resources
} // namespace openomf

#endif // OPENOMF_RESOURCES_IDS_HPP
