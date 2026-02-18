#ifndef OPENOMF_RESOURCES_PATHMANAGER_HPP
#define OPENOMF_RESOURCES_PATHMANAGER_HPP

#include "Ids.hpp"
#include <string>
#include <vector>
#include <cstddef>

namespace openomf {
namespace resources {

/** C resources/pathmanager.h 대응 (최소 이식). SDL/플랫폼 초기화 없이 리소스 베이스만 설정. */
namespace PathManager {

#if defined(_WIN32) || defined(WIN32)
constexpr char path_sep = '\\';
#else
constexpr char path_sep = '/';
#endif

inline static std::string s_base;
inline static std::vector<std::string> s_paths;

/** 리소스 루트 디렉터리 설정. 경로 끝에 sep 유무 무관. 설정 후 get_resource_path(id) 사용 가능. */
inline void set_resource_base(const char* base) {
    s_base.clear();
    s_paths.clear();
    if (!base) return;
    s_base = base;
    if (!s_base.empty() && s_base.back() != path_sep && s_base.back() != '/')
        s_base += path_sep;
    s_paths.resize(static_cast<unsigned int>(ResourceId::NUMBER_OF_RESOURCES));
    for (size_t i = 0; i < s_paths.size(); i++) {
        const char* file = get_resource_file(static_cast<unsigned int>(i));
        if (file)
            s_paths[i] = s_base + file;
        else
            s_paths[i].clear();
    }
}

/** id에 해당하는 리소스 전체 경로. set_resource_base 호출 후에만 유효. */
inline const char* get_resource_path(unsigned int id) {
    if (id >= s_paths.size()) return nullptr;
    if (s_paths[id].empty()) return nullptr;
    return s_paths[id].c_str();
}

/** 로컬 리소스 루트 경로 (set_resource_base 로 설정한 값, 끝에 sep 포함). */
inline const char* get_resource_base() {
    return s_base.empty() ? nullptr : s_base.c_str();
}

} // namespace PathManager
} // namespace resources
} // namespace openomf

#endif // OPENOMF_RESOURCES_PATHMANAGER_HPP
