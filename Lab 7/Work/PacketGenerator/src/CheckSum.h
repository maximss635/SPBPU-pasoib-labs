#pragma once

#include <stdint.h>
#include <linux/icmp.h>
#include <linux/ip.h>
#include <netinet/in.h>
#include <QByteArray>
#include <fstream>
#include <QDebug>
#include <QString>

class CheckSum
{
public:
    static uint16_t calcCheckSum( const QByteArray buf )
    {
        qDebug() <<"calcCheckSum";

        uint32_t res = 0;

        for ( uint16_t* p = ( uint16_t* )buf.data();
              p < ( uint16_t* )buf.data() + buf.size() / 2;
              ++p )
        {
            res += htons( *p );
            qDebug() << "0x"<<QString::number(htons( *p ), 16);
        }

        while ( res & 0xffff0000 )
        {
            uint16_t t = res >> 16;
            res &= 0x0000ffff;
            res += t;
        }

        return ~htons( ( uint16_t )( res & 0xffff ) );
    }

private:
    inline static std::ofstream m_logger;
};


