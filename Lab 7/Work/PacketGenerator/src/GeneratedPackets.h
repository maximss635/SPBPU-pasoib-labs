#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>
#include <net/ethernet.h>

#include <stdint.h>
#include <QByteArray>

class GeneratedPackets
{
public:
    union
    {
        uint8_t bytes[ sizeof( udphdr ) ];
        udphdr header;
    } m_uUdp;

    union
    {
        uint8_t bytes[ sizeof( tcphdr ) ];
        tcphdr header;
    } m_uTcp;

    union
    {
        uint8_t bytes[ sizeof( iphdr ) ];
        iphdr header;
    } m_uIp;

    union
    {
        uint8_t bytes[ sizeof( icmphdr ) ];
        icmphdr header;
    } m_uIcmp;

    union
    {
        uint8_t bytes[ sizeof( ether_header ) ];
        ether_header header;
    } m_uEth;

    static GeneratedPackets& instance()
    {
        static GeneratedPackets packets;
        packets.m_uEth.header.ether_type = 8; // ipv4

        return packets;
    }

    static constexpr int PROTO_ICMP = 1;
    static constexpr int PROTO_TCP = 6;
    static constexpr int PROTO_UDP = 17;
};
