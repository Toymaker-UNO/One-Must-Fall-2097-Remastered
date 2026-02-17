#ifndef OPENOMF_EXTERNALLIBRARY_ENET_HPP
#define OPENOMF_EXTERNALLIBRARY_ENET_HPP

#include <enet/enet.h>
#include <cstddef>

namespace openomf {
namespace external_library {

/** enet 라이브러리 정적 래퍼 (네트워크). */
class Enet {
public:
    /** ENet 전역 초기화. */
    static int omf_enet_initialize() { return enet_initialize(); }

    /** ENet 전역 종료. */
    static void omf_enet_deinitialize() { enet_deinitialize(); }

    /** 호스트 생성. */
    static ENetHost* omf_enet_host_create(const ENetAddress* a_address, std::size_t a_channel_count,
                                          std::size_t a_incoming_bandwidth, enet_uint32 a_outgoing_bandwidth,
                                          enet_uint32 a_packet_throttle) {
        return enet_host_create(a_address, a_channel_count, a_incoming_bandwidth, a_outgoing_bandwidth,
                               a_packet_throttle);
    }

    /** 호스트 파괴. */
    static void omf_enet_host_destroy(ENetHost* a_host) { enet_host_destroy(a_host); }

    /** 원격 호스트로 연결 시도. */
    static ENetPeer* omf_enet_host_connect(ENetHost* a_host, const ENetAddress* a_address, std::size_t a_channel_count,
                                           enet_uint32 a_data) {
        return enet_host_connect(a_host, a_address, a_channel_count, a_data);
    }

    /** 이벤트 대기 및 처리. */
    static int omf_enet_host_service(ENetHost* a_host, ENetEvent* a_event, enet_uint32 a_timeout) {
        return enet_host_service(a_host, a_event, a_timeout);
    }

    /** 송신 버퍼 플러시. */
    static void omf_enet_host_flush(ENetHost* a_host) { enet_host_flush(a_host); }

    /** 소켓 옵션 설정. */
    static int omf_enet_socket_set_option(ENetSocket a_socket, ENetSocketOption a_option, int a_value) {
        return enet_socket_set_option(a_socket, a_option, a_value);
    }

    /** 주소에 호스트명 설정 (DNS 조회). */
    static int omf_enet_address_set_host(ENetAddress* a_address, const char* a_host_name) {
        return enet_address_set_host(a_address, a_host_name);
    }

    /** 패킷 전송. */
    static int omf_enet_peer_send(ENetPeer* a_peer, enet_uint8 a_channel_id, ENetPacket* a_packet) {
        return enet_peer_send(a_peer, a_channel_id, a_packet);
    }

    /** 피어 연결 초기화. */
    static void omf_enet_peer_reset(ENetPeer* a_peer) { enet_peer_reset(a_peer); }

    /** 피어 타임아웃 임계값 설정. */
    static void omf_enet_peer_timeout(ENetPeer* a_peer, enet_uint32 a_timeout_limit, enet_uint32 a_timeout_minimum,
                                      enet_uint32 a_timeout_maximum) {
        enet_peer_timeout(a_peer, a_timeout_limit, a_timeout_minimum, a_timeout_maximum);
    }

    /** 피어 핑 간격 설정. */
    static void omf_enet_peer_ping_interval(ENetPeer* a_peer, enet_uint32 a_interval) {
        enet_peer_ping_interval(a_peer, a_interval);
    }

    /** 피어 지연 연결 해제. */
    static void omf_enet_peer_disconnect_later(ENetPeer* a_peer, enet_uint32 a_data) {
        enet_peer_disconnect_later(a_peer, a_data);
    }

    /** 패킷 생성. */
    static ENetPacket* omf_enet_packet_create(const void* a_data, std::size_t a_data_length, enet_uint32 a_flags) {
        return enet_packet_create(a_data, a_data_length, a_flags);
    }

    /** 패킷 파괴. */
    static void omf_enet_packet_destroy(ENetPacket* a_packet) { enet_packet_destroy(a_packet); }
};

} // namespace external_library
} // namespace openomf

#endif // OPENOMF_EXTERNALLIBRARY_ENET_HPP
