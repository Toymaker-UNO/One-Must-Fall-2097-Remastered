#ifndef OPENOMF_UTILS_CONFIG_HPP
#define OPENOMF_UTILS_CONFIG_HPP

#include "Vector.hpp"
#include "Log.hpp"
#include "../ExternalLibrary/hpp/LibConfuse.hpp"
#include <confuse.h>
#include <cstdio>
#include <cstring>

namespace openomf {
namespace utils {

struct ConfigState {
    cfg_t* cfg = nullptr;
    Vector cfg_opts;
    bool cfg_opts_init = false;
};

/** C utils/config 대응. libconfuse는 ExternalLibrary/hpp(LibConfuse) 통해서만 사용. */
class Config {
public:
    Config() = default;

    ~Config()
    {
        conf_close();
        delete m_state;
        m_state = nullptr;
    }

    void conf_addint(char* a_name, int a_default_val)
    {
        conf_ensure_opt_init();
        cfg_opt_t new_opt = CFG_INT(a_name, a_default_val, CFGF_NONE);
        conf_append_opt(&m_state->cfg_opts, &new_opt);
    }

    void conf_addbool(char* a_name, int a_default_val)
    {
        conf_ensure_opt_init();
        cfg_opt_t new_opt = CFG_BOOL(a_name, static_cast<cfg_bool_t>(a_default_val), CFGF_NONE);
        conf_append_opt(&m_state->cfg_opts, &new_opt);
    }

    void conf_addfloat(char* a_name, double a_default_val)
    {
        conf_ensure_opt_init();
        cfg_opt_t new_opt = CFG_FLOAT(a_name, a_default_val, CFGF_NONE);
        conf_append_opt(&m_state->cfg_opts, &new_opt);
    }

    void conf_addstring(char* a_name, char* a_default_val)
    {
        conf_ensure_opt_init();
        cfg_opt_t new_opt = CFG_STR(a_name, a_default_val, CFGF_NONE);
        conf_append_opt(&m_state->cfg_opts, &new_opt);
    }

    int conf_init(const char* a_filename)
    {
        conf_ensure_opt_init();
        int ret = conf_init_internal(a_filename);
        if (ret == 1) {
            Log::log_msg(LogLevel::DEBUG, "Trying to write a default config file.");
            if (conf_write_config(a_filename) != 0) {
                return 1;
            }
            ret = conf_init_internal(a_filename);
            if (ret != 0) {
                openomf::external_library::LibConfuse::omf_cfg_free(m_state->cfg);
                m_state->cfg = nullptr;
                return 1;
            }
        }
        return 0;
    }

    int conf_write_config(const char* a_filename)
    {
        conf_ensure_opt_init();
        FILE* fp = std::fopen(a_filename, "w");
        if (fp) {
            if (m_state->cfg == nullptr) {
                cfg_t* tmp = openomf::external_library::LibConfuse::omf_cfg_init(
                    reinterpret_cast<cfg_opt_t*>(m_state->cfg_opts.data()), 0);
                openomf::external_library::LibConfuse::omf_cfg_print(tmp, fp);
                openomf::external_library::LibConfuse::omf_cfg_free(tmp);
            } else {
                openomf::external_library::LibConfuse::omf_cfg_print(m_state->cfg, fp);
            }
            std::fclose(fp);
            return 0;
        }
        return 1;
    }

    void conf_close()
    {
        if (!m_state) {
            return;
        }
        if (m_state->cfg) {
            openomf::external_library::LibConfuse::omf_cfg_free(m_state->cfg);
            m_state->cfg = nullptr;
        }
        if (m_state->cfg_opts_init) {
            m_state->cfg_opts.vector_free();
            m_state->cfg_opts_init = false;
        }
    }

    int conf_int(const char* a_name) const
    {
        if (!m_state || !m_state->cfg) {
            return 0;
        }
        return static_cast<int>(openomf::external_library::LibConfuse::omf_cfg_getint(m_state->cfg, a_name));
    }

    double conf_float(const char* a_name) const
    {
        if (!m_state || !m_state->cfg) {
            return 0.0;
        }
        return openomf::external_library::LibConfuse::omf_cfg_getfloat(m_state->cfg, a_name);
    }

    int conf_bool(const char* a_name) const
    {
        if (!m_state || !m_state->cfg) {
            return 0;
        }
        return static_cast<int>(openomf::external_library::LibConfuse::omf_cfg_getbool(m_state->cfg, a_name));
    }

    const char* conf_string(const char* a_name) const
    {
        if (!m_state || !m_state->cfg) {
            return "";
        }
        return openomf::external_library::LibConfuse::omf_cfg_getstr(m_state->cfg, a_name);
    }

    void conf_setint(const char* a_name, int a_val)
    {
        if (m_state && m_state->cfg) {
            openomf::external_library::LibConfuse::omf_cfg_setint(m_state->cfg, a_name, static_cast<long>(a_val));
        }
    }

    void conf_setfloat(const char* a_name, double a_val)
    {
        if (m_state && m_state->cfg) {
            openomf::external_library::LibConfuse::omf_cfg_setfloat(m_state->cfg, a_name, a_val);
        }
    }

    void conf_setbool(const char* a_name, int a_val)
    {
        if (m_state && m_state->cfg) {
            openomf::external_library::LibConfuse::omf_cfg_setbool(
                m_state->cfg, a_name, static_cast<cfg_bool_t>(a_val));
        }
    }

    void conf_setstring(const char* a_name, const char* a_val)
    {
        if (m_state && m_state->cfg) {
            openomf::external_library::LibConfuse::omf_cfg_setstr(m_state->cfg, a_name, a_val);
        }
    }

private:
    void conf_ensure_opt_init()
    {
        if (!m_state) {
            m_state = new ConfigState();
        }
        if (!m_state->cfg_opts_init) {
            m_state->cfg_opts.vector_create_with_size(sizeof(cfg_opt_t), 62);
            cfg_opt_t c = CFG_END();
            m_state->cfg_opts.vector_append(&c);
            m_state->cfg_opts_init = true;
        }
    }

    void conf_append_opt(void* a_vec, const void* a_new_opt)
    {
        Vector* vec = static_cast<Vector*>(a_vec);
        unsigned int sz = vec->vector_size();
        cfg_opt_t* end = static_cast<cfg_opt_t*>(vec->vector_get(sz - 1));
        std::memcpy(end, a_new_opt, sizeof(cfg_opt_t));
        cfg_opt_t c = CFG_END();
        vec->vector_append(&c);
    }

    int conf_init_internal(const char* a_filename)
    {
        conf_ensure_opt_init();
        m_state->cfg = openomf::external_library::LibConfuse::omf_cfg_init(
            reinterpret_cast<cfg_opt_t*>(m_state->cfg_opts.data()), CFGF_IGNORE_UNKNOWN);
        int ret = openomf::external_library::LibConfuse::omf_cfg_parse(m_state->cfg, a_filename);
        if (ret == CFG_FILE_ERROR) {
            Log::log_msg(LogLevel::ERR, "Error while attempting to read config file '%s' !", a_filename);
            return 1;
        }
        if (ret == CFG_PARSE_ERROR) {
            Log::log_msg(LogLevel::ERR, "Error while attempting to parse config file '%s' !", a_filename);
            return 1;
        }
        Log::log_msg(LogLevel::INFO, "Configfile read!");
        return 0;
    }

    ConfigState* m_state = nullptr;
};

}  // namespace utils
}  // namespace openomf

#endif
