#ifndef OPENOMF_EXTERNALLIBRARY_NATPMP_MINIUPNPC_HPP
#define OPENOMF_EXTERNALLIBRARY_NATPMP_MINIUPNPC_HPP

#if defined(OMF_HAS_MINIUPNPC)
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/portlistingparse.h>
#include <miniupnpc/upnpcommands.h>
#endif

namespace openomf {
namespace external_library {

/** natpmp / miniupnpc 라이브러리 정적 래퍼 (NAT-PMP·UPnP, 조건부 빌드). */
class NatPmpMiniupnpc {
public:
#if defined(OMF_HAS_MINIUPNPC)
    /** 포트 매핑 목록 조회. */
    static int omf_upnp_get_list_of_port_mappings(const char* a_control_url, const char* a_servicetype,
                                                  const char* a_start_port, const char* a_end_port,
                                                  const char* a_protocol, const char* a_number_of_ports,
                                                  struct PortMappingParserData* a_data_out) {
        return UPNP_GetListOfPortMappings(a_control_url, a_servicetype, a_start_port, a_end_port, a_protocol,
                                         a_number_of_ports, a_data_out);
    }

    /** 포트 매핑 추가. */
    static int omf_upnp_add_port_mapping(const char* a_control_url, const char* a_servicetype,
                                         const char* a_ext_port, const char* a_int_port,
                                         const char* a_int_client, const char* a_desc, const char* a_protocol,
                                         const char* a_remote_host, const char* a_lease_duration) {
        return UPNP_AddPortMapping(a_control_url, a_servicetype, a_ext_port, a_int_port, a_int_client, a_desc,
                                  a_protocol, a_remote_host, a_lease_duration);
    }

    /** 유효한 IGD 검색. */
    static int omf_upnp_get_valid_igd(struct UPNPDev* a_devlist, struct UPNPUrls* a_urls,
                                      struct IGDdatas* a_data, char* a_lan_addr, int a_lan_addr_size) {
        return UPNP_GetValidIGD(a_devlist, a_urls, a_data, a_lan_addr, a_lan_addr_size);
    }

    /** 외부 IP 주소 조회. */
    static int omf_upnp_get_external_ip_address(const char* a_control_url, const char* a_servicetype,
                                                 char* a_ext_ip_addr) {
        return UPNP_GetExternalIPAddress(a_control_url, a_servicetype, a_ext_ip_addr);
    }

    /** 포트 매핑 삭제. */
    static int omf_upnp_delete_port_mapping(const char* a_control_url, const char* a_servicetype,
                                            const char* a_ext_port, const char* a_protocol,
                                            const char* a_remote_host) {
        return UPNP_DeletePortMapping(a_control_url, a_servicetype, a_ext_port, a_protocol, a_remote_host);
    }

    /** 포트 매핑 목록 파서 데이터 해제. */
    static void omf_free_port_listing(struct PortMappingParserData* a_pmp_data) {
        FreePortListing(a_pmp_data);
    }
#endif
};

} // namespace external_library
} // namespace openomf

#endif // OPENOMF_EXTERNALLIBRARY_NATPMP_MINIUPNPC_HPP
