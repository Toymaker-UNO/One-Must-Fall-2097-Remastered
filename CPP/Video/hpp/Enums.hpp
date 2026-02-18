#ifndef OPENOMF_VIDEO_ENUMS_HPP
#define OPENOMF_VIDEO_ENUMS_HPP

namespace openomf {
namespace video {

/** C video/enums.h renderer_options */
enum RendererOptions : unsigned int {
    SPRITE_REMAP     = 0x01,
    SPRITE_SHADOW    = 0x02,
    SPRITE_INDEX_ADD  = 0x04,
    SPRITE_HAR_QUIRKS = 0x08,
    SPRITE_DARK_TINT  = 0x10,
};

/** C video/enums.h renderer_framebuffer_options */
enum RendererFramebufferOptions : unsigned int {
    FBUFOPT_CREDITS = 0x01,
};

/** C video/enums.h video_flip_mode */
enum VideoFlipMode : unsigned int {
    FLIP_NONE       = 0,
    FLIP_HORIZONTAL = 0x1,
    FLIP_VERTICAL   = 0x2,
};

} // namespace video
} // namespace openomf

#endif // OPENOMF_VIDEO_ENUMS_HPP
