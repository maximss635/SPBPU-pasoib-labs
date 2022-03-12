#include "MainWindow.h"
#include "ui_MainWindow.h"

void MainWindow::initTcpView()
{
    ui->tabTcp->setLayout( ui->mainLayoutTcp );
    ui->mainLayoutTcp->addWidget( m_tcpDump.get() );
    ui->checkBox_8->setChecked( false );
    ui->lineEdit_11->setEnabled( false );

    ui->lineEdit_9->setValidator( m_validatorTwoBytes.get() );
    ui->lineEdit_10->setValidator( m_validatorTwoBytes.get() );
    ui->lineEdit->setValidator( m_validatorFourBytes.get() );
    ui->lineEdit_9->setValidator( m_validatorFourBytes.get() );
    ui->lineEdit_4->setValidator( m_validatorTwoBytes.get() );
    ui->lineEdit_12->setValidator( m_validatorTwoBytes.get() );
    ui->lineEdit_11->setValidator( m_validatorTwoBytes.get() );
    ui->lineEdit_3->setValidator( m_validatorFourBits.get() );
    ui->lineEdit_2->setValidator( m_validatorTwoBytes.get() );

    GeneratedPackets& packets = GeneratedPackets::instance();
    tcphdr& curHeader = packets.m_uTcp.header;

    bindWithVar< uint16_t >( ui->lineEdit_10, curHeader.dest,    ui->label_9 );
    bindWithVar< uint16_t >( ui->lineEdit_2,  curHeader.source,  ui->label_8 );
    bindWithVar< uint32_t >( ui->lineEdit,    curHeader.seq,     ui->label_2 );
    bindWithVar< uint32_t >( ui->lineEdit_9,  curHeader.ack_seq, ui->label );
    bindWithVar< uint16_t >( ui->lineEdit_4,  curHeader.window,  ui->label_7 );
    bindWithVar< uint16_t >( ui->lineEdit_12, curHeader.urg_ptr, ui->label_10 );
    bindWithVar< uint16_t >( ui->lineEdit_11, curHeader.check );

    // curHeader.doff - 4 bits (12 l)
    connect( ui->lineEdit_3, &QLineEdit::textChanged, this,
             [ this ]( const QString& text )
    {
        GeneratedPackets::instance().m_uTcp.bytes[12] = text.toInt() << 4;
        updateTcpView();
    });

    connect( ui->checkBox_8, &QCheckBox::stateChanged, this,
             [ this, &curHeader ]( int state )
    {
        ui->lineEdit_11->setText( QString::number( curHeader.check ) );

        ui->lineEdit_11->setEnabled( static_cast< bool >( state ) );

        updateTcpView();
    });

    connect( ui->checkBox_17, &QCheckBox::stateChanged, this,
             [ this, &curHeader ]( int state )
    {
        ui->lineEdit_3->setText( QString::number( curHeader.doff ) );

        ui->lineEdit_3->setEnabled( static_cast< bool >( state ) );

        updateTcpView();
    });

    // check box flags
    bindCheckBoxWithBit( ui->checkBox_7, packets.m_uTcp.bytes[ 13 ], 5 );
    bindCheckBoxWithBit( ui->checkBox,   packets.m_uTcp.bytes[ 13 ], 4 );
    bindCheckBoxWithBit( ui->checkBox_3, packets.m_uTcp.bytes[ 13 ], 3 );
    bindCheckBoxWithBit( ui->checkBox_6, packets.m_uTcp.bytes[ 13 ], 2 );
    bindCheckBoxWithBit( ui->checkBox_4, packets.m_uTcp.bytes[ 13 ], 1 );
    bindCheckBoxWithBit( ui->checkBox_5, packets.m_uTcp.bytes[ 13 ], 0 );

    updateTcpView();
}

void MainWindow::updateTcpView()
{
    auto& packets = GeneratedPackets::instance();

    if ( !ui->checkBox_17->isChecked() )
    {
        // Update tcp len
        packets.m_uTcp.header.doff = sizeof( tcphdr ) / 4;
        ui->lineEdit_3->setText( QString::number( packets.m_uTcp.header.doff ) );
    }

    if ( !ui->checkBox_8->isChecked() )
    {
        recalcCheckSum( INDEX_TAB_TCP, ui->lineEdit_11 );
    }

    QByteArray tcpData = QByteArray(
        ( const char* )packets.m_uTcp.bytes,
        sizeof( tcphdr )
    );

    // Adding main data
    auto mainData = ui->textEditData->toPlainText().toStdString();

    tcpData += QByteArray(
        ( const char* )mainData.c_str(), mainData.size()
    );

    m_tcpDump->setData( tcpData );
}
