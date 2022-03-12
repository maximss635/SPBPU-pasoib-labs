#include "MainWindow.h"
#include "ui_MainWindow.h"

void MainWindow::initIpView()
{
    ui->tabIp->setLayout( ui->mainLayoutIp );
    ui->mainLayoutIp->addWidget( m_ipDump.get() );
    ui->checkBox_9->setChecked( false );
    ui->checkBox_18->setChecked( false );
    ui->lineEdit_26->setEnabled( false );
    ui->lineEdit_23->setEnabled( false );
    ui->radioButton_2->setChecked( true );

    QList< QLineEdit* > lineEditListIpBytes = {
        ui->lineEdit_13, ui->lineEdit_14,
        ui->lineEdit_15, ui->lineEdit_16,
        ui->lineEdit_17, ui->lineEdit_18,
        ui->lineEdit_19, ui->lineEdit_20
    };

    auto countAddrFrom = [ this ]() -> uint32_t
    {
        return ( ui->lineEdit_16->text().toInt() << 24 ) |
                ( ui->lineEdit_15->text().toInt() << 16 ) |
                ( ui->lineEdit_14->text().toInt() << 8 ) |
                ( ui->lineEdit_13->text().toInt() << 0 ) ;
    };

    auto countAddrTo = [ this ]() -> uint32_t
    {
        return ( ui->lineEdit_20->text().toInt() << 24 ) |
                ( ui->lineEdit_19->text().toInt() << 16 ) |
                ( ui->lineEdit_18->text().toInt() << 8 ) |
                ( ui->lineEdit_17->text().toInt() << 0 ) ;
    };

    int i = 0;
    for ( auto* pLineEdit : lineEditListIpBytes )
    {
        pLineEdit->setValidator( m_validatorOneByte.get() );

        if ( i < 4 )
        {
            connect( pLineEdit, &QLineEdit::textChanged, this,
                     [ this, pLineEdit, lineEditListIpBytes, i, countAddrFrom ]()
            {
                markLabel( ui->lineEdit_13->hasAcceptableInput() &&
                           ui->lineEdit_14->hasAcceptableInput() &&
                           ui->lineEdit_15->hasAcceptableInput() &&
                           ui->lineEdit_16->hasAcceptableInput(),
                           ui->label_12 );

                if ( pLineEdit->text().size() == 3 && i != lineEditListIpBytes.size() - 1 )
                {
                    lineEditListIpBytes[ i + 1 ]->setFocus();
                }

                GeneratedPackets::instance().m_uIp.header.saddr = countAddrFrom();
                updateIpView();
            } );
        }
        else
        {
            connect( pLineEdit, &QLineEdit::textChanged, this,
                     [ this, pLineEdit, lineEditListIpBytes, i, countAddrTo ]()
            {
                markLabel( ui->lineEdit_17->hasAcceptableInput() &&
                           ui->lineEdit_18->hasAcceptableInput() &&
                           ui->lineEdit_19->hasAcceptableInput() &&
                           ui->lineEdit_20->hasAcceptableInput(),
                           ui->label_11 );

                if ( pLineEdit->text().size() == 3 && i != lineEditListIpBytes.size() - 1 )
                {
                    lineEditListIpBytes[ i + 1 ]->setFocus();
                }

                GeneratedPackets::instance().m_uIp.header.daddr = countAddrTo();
                updateIpView();
            } );
        }

        ++i;
    }

    ui->lineEdit_22->setValidator( m_validatorFourBits.get() );
    ui->lineEdit_21->setValidator( new QIntValidator( 5, 15, this ) );
    ui->lineEdit_27->setValidator( new QIntValidator( 0, 7, this ) );
    ui->lineEdit_23->setValidator( m_validatorTwoBytes.get() );
    ui->lineEdit_25->setValidator( m_validatorTwoBytes.get() );
    ui->lineEdit_28->setValidator( new QIntValidator( 0, 8192, this ) );
    ui->lineEdit_24->setValidator( m_validatorOneByte.get() );
    ui->lineEdit_26->setValidator( m_validatorTwoBytes.get() );

    iphdr& curHeader = GeneratedPackets::instance().m_uIp.header;

    // 4-bit vars
    connect( ui->lineEdit_21, &QLineEdit::textChanged, this,
             [ this ]( const QString& text )
    {
        GeneratedPackets::instance().m_uIp.bytes[0] &= 0xf0;
        GeneratedPackets::instance().m_uIp.bytes[0] |= text.toInt() & 0x0f;

        updateView();

        markLabel( ui->lineEdit_21->hasAcceptableInput(), ui->label_20 );

    });
    connect( ui->lineEdit_22, &QLineEdit::textChanged, this,
             [ this ]( const QString& text )
    {
        GeneratedPackets::instance().m_uIp.bytes[0] &= 0x0f;
        GeneratedPackets::instance().m_uIp.bytes[0] |= text.toInt() << 4;

        updateView();

        markLabel( ui->lineEdit_22->hasAcceptableInput(), ui->label_19 );

    });

    markLabel( false, ui->label_20 );
    markLabel( false, ui->label_19 );

    bindWithVar< uint8_t >( ui->lineEdit_27, curHeader.tos, ui->label_24 );
    bindWithVar< uint16_t >( ui->lineEdit_23, curHeader.tot_len );
    bindWithVar< uint16_t >( ui->lineEdit_25, curHeader.id, ui->label_22 );
    bindWithVar< uint16_t >( ui->lineEdit_28, curHeader.frag_off, ui->label_25 );
    bindWithVar< uint8_t >( ui->lineEdit_24, curHeader.ttl, ui->label_23 );
    bindWithVar< uint16_t >( ui->lineEdit_26, curHeader.check );

    bindCheckBoxWithBit( ui->checkBox_10, curHeader.tos, 4 );
    bindCheckBoxWithBit( ui->checkBox_11, curHeader.tos, 3 );
    bindCheckBoxWithBit( ui->checkBox_12, curHeader.tos, 2 );
    bindCheckBoxWithBit( ui->checkBox_13, curHeader.tos, 1 );

    bindCheckBoxWithBit( ui->checkBox_14, curHeader.frag_off, 6 );
    bindCheckBoxWithBit( ui->checkBox_15, curHeader.frag_off, 7 );

    connect( ui->checkBox_9, &QCheckBox::stateChanged, this,
             [ this ]( int state )
    {
        ui->lineEdit_26->setText( QString::number( GeneratedPackets::instance().m_uIp.header.check ) );
        ui->lineEdit_26->setEnabled( static_cast< bool >( state ) );

        updateIpView();
    });

    connect( ui->checkBox_18, &QCheckBox::stateChanged, this,
             [ this ]( int state )
    {
        ui->lineEdit_23->setText( QString::number( GeneratedPackets::instance().m_uIp.header.tot_len ) );
        ui->lineEdit_23->setEnabled( static_cast< bool >( state ) );

        updateIpView();
    });

    connect( ui->radioButton, &QRadioButton::clicked, this, [ this ]()
    {
        GeneratedPackets::instance().m_uIp.header.protocol = GeneratedPackets::PROTO_UDP;
        updateView();
    } );

    connect( ui->radioButton_2, &QRadioButton::clicked, this, [ this ]()
    {
        GeneratedPackets::instance().m_uIp.header.protocol = GeneratedPackets::PROTO_TCP;
        updateView();
    } );

    connect( ui->radioButton_3, &QRadioButton::clicked, this, [ this ]()
    {
        GeneratedPackets::instance().m_uIp.header.protocol = GeneratedPackets::PROTO_ICMP;
        updateView();
    } );

    emit ui->radioButton_2->clicked( true );

    ui->lineEdit_22->setEnabled( false );
    GeneratedPackets::instance().m_uIp.header.version = 4;
    markLabel( true, ui->label_19 );

    auto genIp = []( QLineEdit* p1, QLineEdit* p2, QLineEdit* p3, QLineEdit* p4,
            int i, QPushButton* pButton ) -> int
    {
        static auto deviceList =
                pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();

        int ip = deviceList[ i ]->getIPv4Address().toInt();

        p4->setText( QString::number( ( ip & 0xff000000 ) >> 24 ) );
        p3->setText( QString::number( ( ip & 0x00ff0000 ) >> 16 ) );
        p2->setText( QString::number( ( ip & 0x0000ff00 ) >> 8 ) );
        p1->setText( QString::number( ( ip & 0x000000ff ) >> 0 ) );
        pButton->setText( deviceList[i]->getName().c_str() );

        return ( i + 1 ) % deviceList.size();
    };

    connect( ui->pushButton, &QPushButton::clicked, this,
             [ this, genIp ]()
    {
        static int i = 0;
        i = genIp( ui->lineEdit_13, ui->lineEdit_14, ui->lineEdit_15, ui->lineEdit_16, i, ui->pushButton );
    });
    connect( ui->pushButton_2, &QPushButton::clicked, this,
             [ this, genIp ]()
    {
        static int i = 0;
        i = genIp( ui->lineEdit_17, ui->lineEdit_18, ui->lineEdit_19, ui->lineEdit_20, i, ui->pushButton_2 );
    });

    updateIpView();
}

void MainWindow::updateIpView()
{
    GeneratedPackets& packets = GeneratedPackets::instance();

    if ( !ui->checkBox_9->isChecked() )
    {
        recalcCheckSum( INDEX_TAB_IP, ui->lineEdit_26 );
    }

    if ( !ui->checkBox_18->isChecked() )
    {
        // Update ip len
        packets.m_uIp.header.tot_len = sizeof( iphdr ) + ui->textEditData->toPlainText().size();
        switch ( packets.m_uIp.header.protocol )
        {
        case GeneratedPackets::PROTO_ICMP:
            packets.m_uIp.header.tot_len += sizeof( icmphdr );
            break;

        case GeneratedPackets::PROTO_TCP:
            packets.m_uIp.header.tot_len += sizeof( tcphdr );
            break;

        case GeneratedPackets::PROTO_UDP:
            packets.m_uIp.header.tot_len += sizeof( udphdr );
            break;
        }

        ui->lineEdit_23->setText( QString::number( packets.m_uIp.header.tot_len ) );

        packets.m_uIp.header.tot_len = htons( packets.m_uIp.header.tot_len );
    }

    QByteArray ipData( ( const char* )packets.m_uIp.bytes, sizeof( iphdr ) );

    // Incapsulation
    switch ( packets.m_uIp.header.protocol )
    {
    case GeneratedPackets::PROTO_ICMP:
        ipData += QByteArray( ( const char* )packets.m_uIcmp.bytes, sizeof( icmphdr ) );
        break;

    case GeneratedPackets::PROTO_TCP:
        ipData += QByteArray( ( const char* )packets.m_uTcp.bytes, sizeof( tcphdr ) );
        break;

    case GeneratedPackets::PROTO_UDP:
        ipData += QByteArray( ( const char* )packets.m_uUdp.bytes, sizeof( udphdr ) );
        break;
    }

    // Adding main data
    auto mainData = ui->textEditData->toPlainText().toStdString();

    ipData += QByteArray(
        ( const char* )mainData.c_str(), mainData.size()
    );

    m_ipDump->setData( ipData );
};
