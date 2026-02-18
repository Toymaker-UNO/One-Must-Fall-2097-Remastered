#ifndef OPENOMF_RESOURCES_AFLOADER_HPP
#define OPENOMF_RESOURCES_AFLOADER_HPP

#include "hpp/Ids.hpp"
#include "hpp/PathManager.hpp"
#include "Formats/hpp/Af.hpp"
#include "Formats/hpp/Error.hpp"

namespace openomf {
namespace resources {

/** C resources/af_loader: 리소스 id로 AF 파일 경로를 얻어 Formats AfFile 로드. PathManager::set_resource_base 호출 후 사용. */
inline formats::SdError load_af_file(unsigned int id, formats::AfFile* out) {
    if (!out) return formats::SdError::INVALID_INPUT;
    const char* path = PathManager::get_resource_path(id);
    if (!path) return formats::SdError::FILE_OPEN_ERROR;
    if (formats::af_create(out) != formats::SdError::SUCCESS) return formats::SdError::OUT_OF_MEMORY;
    formats::SdError err = formats::af_load(out, path);
    if (err != formats::SdError::SUCCESS) {
        formats::af_free(out);
        return err;
    }
    return formats::SdError::SUCCESS;
}

} // namespace resources
} // namespace openomf

#endif // OPENOMF_RESOURCES_AFLOADER_HPP
