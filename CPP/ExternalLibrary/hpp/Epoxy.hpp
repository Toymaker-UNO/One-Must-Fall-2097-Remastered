#ifndef OPENOMF_EXTERNALLIBRARY_EPOXY_HPP
#define OPENOMF_EXTERNALLIBRARY_EPOXY_HPP

#include <epoxy/gl.h>

namespace openomf {
namespace external_library {

/** epoxy(OpenGL) 라이브러리 정적 래퍼 (OpenGL 3.3). */
class Epoxy {
public:
    /** 이름에 해당하는 GL 문자열 반환. */
    static const GLubyte* omf_gl_get_string(GLenum a_name) { return glGetString(a_name); }

    /** 텍스처 객체 이름 생성. */
    static void omf_gl_gen_textures(GLsizei a_n, GLuint* a_textures) { glGenTextures(a_n, a_textures); }

    /** 텍스처 객체 삭제. */
    static void omf_gl_delete_textures(GLsizei a_n, const GLuint* a_textures) { glDeleteTextures(a_n, a_textures); }

    /** 텍스처 파라미터 설정. */
    static void omf_gl_tex_parameter_i(GLenum a_target, GLenum a_pname, GLint a_param) {
        glTexParameteri(a_target, a_pname, a_param);
    }

    /** 2D 텍스처 이미지 지정. */
    static void omf_gl_tex_image2_d(GLenum a_target, GLint a_level, GLint a_internalformat, GLsizei a_width,
                                   GLsizei a_height, GLint a_border, GLenum a_format, GLenum a_type,
                                   const void* a_pixels) {
        glTexImage2D(a_target, a_level, a_internalformat, a_width, a_height, a_border, a_format, a_type, a_pixels);
    }

    /** 2D 텍스처 서브이미지 지정. */
    static void omf_gl_tex_sub_image2_d(GLenum a_target, GLint a_level, GLint a_xoffset, GLint a_yoffset,
                                        GLsizei a_width, GLsizei a_height, GLenum a_format, GLenum a_type,
                                        const void* a_pixels) {
        glTexSubImage2D(a_target, a_level, a_xoffset, a_yoffset, a_width, a_height, a_format, a_type, a_pixels);
    }

    /** 버퍼 객체 이름 생성. */
    static void omf_gl_gen_buffers(GLsizei a_n, GLuint* a_buffers) { glGenBuffers(a_n, a_buffers); }

    /** 버퍼 객체 삭제. */
    static void omf_gl_delete_buffers(GLsizei a_n, const GLuint* a_buffers) { glDeleteBuffers(a_n, a_buffers); }

    /** 버퍼 객체 데이터 저장. */
    static void omf_gl_buffer_data(GLenum a_target, GLsizeiptr a_size, const void* a_data, GLenum a_usage) {
        glBufferData(a_target, a_size, a_data, a_usage);
    }

    /** 버퍼 서브영역 갱신. */
    static void omf_gl_buffer_sub_data(GLenum a_target, GLintptr a_offset, GLsizeiptr a_size, const void* a_data) {
        glBufferSubData(a_target, a_offset, a_size, a_data);
    }

    /** 버퍼 데이터 영역 포인터 반환. */
    static void* omf_gl_map_buffer_range(GLenum a_target, GLintptr a_offset, GLsizeiptr a_length, GLbitfield a_access) {
        return glMapBufferRange(a_target, a_offset, a_length, a_access);
    }

    /** 매핑된 버퍼 플러시. */
    static void omf_gl_flush_mapped_buffer_range(GLenum a_target, GLintptr a_offset, GLsizeiptr a_length) {
        glFlushMappedBufferRange(a_target, a_offset, a_length);
    }

    /** 버퍼 매핑 해제. */
    static GLboolean omf_gl_unmap_buffer(GLenum a_target) { return glUnmapBuffer(a_target); }

    /** VAO 이름 생성. */
    static void omf_gl_gen_vertex_arrays(GLsizei a_n, GLuint* a_arrays) { glGenVertexArrays(a_n, a_arrays); }

    /** VAO 삭제. */
    static void omf_gl_delete_vertex_arrays(GLsizei a_n, const GLuint* a_arrays) {
        glDeleteVertexArrays(a_n, a_arrays);
    }

    /** VAO 바인딩. */
    static void omf_gl_bind_vertex_array(GLuint a_array) { glBindVertexArray(a_array); }

    /** 버퍼 바인딩. */
    static void omf_gl_bind_buffer(GLenum a_target, GLuint a_buffer) { glBindBuffer(a_target, a_buffer); }

    /** 텍스처 유닛 바인딩. */
    static void omf_gl_bind_texture(GLenum a_target, GLuint a_texture) { glBindTexture(a_target, a_texture); }

    /** 프레임버퍼 바인딩. */
    static void omf_gl_bind_framebuffer(GLenum a_target, GLuint a_framebuffer) {
        glBindFramebuffer(a_target, a_framebuffer);
    }

    /** UBO 바인딩 슬롯에 바인딩. */
    static void omf_gl_bind_buffer_base(GLenum a_target, GLuint a_index, GLuint a_buffer) {
        glBindBufferBase(a_target, a_index, a_buffer);
    }

    /** 활성 텍스처 유닛 선택. */
    static void omf_gl_active_texture(GLenum a_texture) { glActiveTexture(a_texture); }

    /** 버텍스 속성 포인터 설정. */
    static void omf_gl_vertex_attrib_pointer(GLuint a_index, GLint a_size, GLenum a_type, GLboolean a_normalized,
                                             GLsizei a_stride, const void* a_pointer) {
        glVertexAttribPointer(a_index, a_size, a_type, a_normalized, a_stride, a_pointer);
    }

    /** 버텍스 속성 정수 포인터 설정. */
    static void omf_gl_vertex_attrib_i_pointer(GLuint a_index, GLint a_size, GLenum a_type, GLsizei a_stride,
                                               const void* a_pointer) {
        glVertexAttribIPointer(a_index, a_size, a_type, a_stride, a_pointer);
    }

    /** 버텍스 속성 배열 활성화. */
    static void omf_gl_enable_vertex_attrib_array(GLuint a_index) { glEnableVertexAttribArray(a_index); }

    /** 프레임버퍼 객체 이름 생성. */
    static void omf_gl_gen_framebuffers(GLsizei a_n, GLuint* a_framebuffers) {
        glGenFramebuffers(a_n, a_framebuffers);
    }

    /** FBO 삭제. */
    static void omf_gl_delete_framebuffers(GLsizei a_n, const GLuint* a_framebuffers) {
        glDeleteFramebuffers(a_n, a_framebuffers);
    }

    /** FBO에 텍스처 첨부. */
    static void omf_gl_framebuffer_texture2_d(GLenum a_target, GLenum a_attachment, GLenum a_textarget,
                                              GLuint a_texture, GLint a_level) {
        glFramebufferTexture2D(a_target, a_attachment, a_textarget, a_texture, a_level);
    }

    /** FBO 완전성 검사. */
    static GLenum omf_gl_check_framebuffer_status(GLenum a_target) {
        return glCheckFramebufferStatus(a_target);
    }

    /** 셰이더 객체 생성. */
    static GLuint omf_gl_create_shader(GLenum a_type) { return glCreateShader(a_type); }

    /** 셰이더 소스 설정. */
    static void omf_gl_shader_source(GLuint a_shader, GLsizei a_count, const GLchar* const* a_string,
                                     const GLint* a_length) {
        glShaderSource(a_shader, a_count, a_string, a_length);
    }

    /** 셰이더 컴파일. */
    static void omf_gl_compile_shader(GLuint a_shader) { glCompileShader(a_shader); }

    /** 셰이더 파라미터 조회. */
    static void omf_gl_get_shader_iv(GLuint a_shader, GLenum a_pname, GLint* a_params) {
        glGetShaderiv(a_shader, a_pname, a_params);
    }

    /** 셰이더 정보 로그 반환. */
    static void omf_gl_get_shader_info_log(GLuint a_shader, GLsizei a_buf_size, GLsizei* a_length, GLchar* a_info_log) {
        glGetShaderInfoLog(a_shader, a_buf_size, a_length, a_info_log);
    }

    /** 셰이더 객체 여부. */
    static GLboolean omf_gl_is_shader(GLuint a_shader) { return glIsShader(a_shader); }

    /** 셰이더 객체 삭제. */
    static void omf_gl_delete_shader(GLuint a_shader) { glDeleteShader(a_shader); }

    /** 프로그램 객체 생성. */
    static GLuint omf_gl_create_program() { return glCreateProgram(); }

    /** 프로그램에 셰이더 첨부. */
    static void omf_gl_attach_shader(GLuint a_program, GLuint a_shader) { glAttachShader(a_program, a_shader); }

    /** 프로그램 링크. */
    static void omf_gl_link_program(GLuint a_program) { glLinkProgram(a_program); }

    /** 프로그램 파라미터 조회. */
    static void omf_gl_get_program_iv(GLuint a_program, GLenum a_pname, GLint* a_params) {
        glGetProgramiv(a_program, a_pname, a_params);
    }

    /** 프로그램에 첨부된 셰이더 목록 반환. */
    static void omf_gl_get_attached_shaders(GLuint a_program, GLsizei a_max_count, GLsizei* a_count,
                                            GLuint* a_shaders) {
        glGetAttachedShaders(a_program, a_max_count, a_count, a_shaders);
    }

    /** 프로그램 객체 삭제. */
    static void omf_gl_delete_program(GLuint a_program) { glDeleteProgram(a_program); }

    /** 프로그램 정보 로그 반환. */
    static void omf_gl_get_program_info_log(GLuint a_program, GLsizei a_buf_size, GLsizei* a_length,
                                           GLchar* a_info_log) {
        glGetProgramInfoLog(a_program, a_buf_size, a_length, a_info_log);
    }

    /** 프로그램 객체 여부. */
    static GLboolean omf_gl_is_program(GLuint a_program) { return glIsProgram(a_program); }

    /** 유니폼 위치 반환. */
    static GLint omf_gl_get_uniform_location(GLuint a_program, const GLchar* a_name) {
        return glGetUniformLocation(a_program, a_name);
    }

    /** 유니폼 블록 인덱스 반환. */
    static GLuint omf_gl_get_uniform_block_index(GLuint a_program, const GLchar* a_name) {
        return glGetUniformBlockIndex(a_program, a_name);
    }

    /** 4x4 행렬 유니폼 설정. */
    static void omf_gl_uniform_matrix4fv(GLint a_location, GLsizei a_count, GLboolean a_transpose,
                                         const GLfloat* a_value) {
        glUniformMatrix4fv(a_location, a_count, a_transpose, a_value);
    }

    /** 1i 유니폼 설정. */
    static void omf_gl_uniform1i(GLint a_location, GLint a_v0) { glUniform1i(a_location, a_v0); }

    /** 1ui 유니폼 설정. */
    static void omf_gl_uniform1ui(GLint a_location, GLuint a_v0) { glUniform1ui(a_location, a_v0); }

    /** 유니폼 블록 바인딩 슬롯 지정. */
    static void omf_gl_uniform_block_binding(GLuint a_program, GLuint a_uniform_block_index,
                                            GLuint a_uniform_block_binding) {
        glUniformBlockBinding(a_program, a_uniform_block_index, a_uniform_block_binding);
    }

    /** 프로그램 사용. */
    static void omf_gl_use_program(GLuint a_program) { glUseProgram(a_program); }

    /** 클리어 색상 설정. */
    static void omf_gl_clear_color(GLfloat a_red, GLfloat a_green, GLfloat a_blue, GLfloat a_alpha) {
        glClearColor(a_red, a_green, a_blue, a_alpha);
    }

    /** 버퍼 비트 클리어. */
    static void omf_gl_clear(GLbitfield a_mask) { glClear(a_mask); }

    /** 뷰포트 설정. */
    static void omf_gl_viewport(GLint a_x, GLint a_y, GLsizei a_width, GLsizei a_height) {
        glViewport(a_x, a_y, a_width, a_height);
    }

    /** 프리미티브 배열 렌더링. */
    static void omf_gl_draw_arrays(GLenum a_mode, GLint a_first, GLsizei a_count) {
        glDrawArrays(a_mode, a_first, a_count);
    }

    /** 여러 프리미티브 배열 렌더링. */
    static void omf_gl_multi_draw_arrays(GLenum a_mode, const GLint* a_first, const GLsizei* a_count,
                                         GLsizei a_drawcount) {
        glMultiDrawArrays(a_mode, a_first, a_count, a_drawcount);
    }

    /** 픽셀 읽기. */
    static void omf_gl_read_pixels(GLint a_x, GLint a_y, GLsizei a_width, GLsizei a_height, GLenum a_format,
                                   GLenum a_type, void* a_pixels) {
        glReadPixels(a_x, a_y, a_width, a_height, a_format, a_type, a_pixels);
    }

    /** 픽셀 저장/언팩 정렬 설정. */
    static void omf_gl_pixel_store_i(GLenum a_pname, GLint a_param) { glPixelStorei(a_pname, a_param); }

    /** 컬러 버퍼 쓰기 마스크. */
    static void omf_gl_color_mask(GLboolean a_red, GLboolean a_green, GLboolean a_blue, GLboolean a_alpha) {
        glColorMask(a_red, a_green, a_blue, a_alpha);
    }

    /** GL 기능 활성화. */
    static void omf_gl_enable(GLenum a_cap) { glEnable(a_cap); }

    /** GL 기능 비활성화. */
    static void omf_gl_disable(GLenum a_cap) { glDisable(a_cap); }

    /** 블렌드 연산 설정. */
    static void omf_gl_blend_equation(GLenum a_mode) { glBlendEquation(a_mode); }
};

} // namespace external_library
} // namespace openomf

#endif // OPENOMF_EXTERNALLIBRARY_EPOXY_HPP
