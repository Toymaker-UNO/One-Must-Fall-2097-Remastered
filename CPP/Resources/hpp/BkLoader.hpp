#ifndef OPENOMF_RESOURCES_BKLOADER_HPP
#define OPENOMF_RESOURCES_BKLOADER_HPP

#include "Formats/hpp/Bk.hpp"
#include "Formats/hpp/Error.hpp"

namespace openomf {
namespace resources {

/** C resources/bk_loader: Formats BkFile 로드. */
inline formats::SdError load_bk_file(const char* filename, formats::BkFile* out) {
    if (!filename || !out) return formats::SdError::INVALID_INPUT;
    if (formats::bk_create(out) != formats::SdError::SUCCESS) return formats::SdError::OUT_OF_MEMORY;
    formats::SdError err = formats::bk_load(out, filename);
    if (err != formats::SdError::SUCCESS) {
        formats::bk_free(out);
        return err;
    }
    return formats::SdError::SUCCESS;
}

} // namespace resources
} // namespace openomf

#endif // OPENOMF_RESOURCES_BKLOADER_HPP
