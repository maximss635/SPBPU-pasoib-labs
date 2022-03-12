#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>

void MainWindow::initUdpView()
{
    ui->tabUdp->setLayout( ui->mainLayoutUdp );
    ui->mainLayoutUdp->addWidget( m_udpDump.get() );
    ui->checkBox_2->setChecked( false );
    ui->checkBox_16->setChecked( false );
    ui->checkBox_17->setChecked( false );
    ui->lineEdit_8->setEnabled( false );
    ui->lineEdit_5->setEnabled( false );
    ui->lineEdit_3->setEnabled( false );

    ui->lineEdit_5->setValidator( m_validatorTwoBytes.get() );
    ui->lineEdit_6->setValidator( m_validatorTwoBytes.get() );
    ui->lineEdit_7->setValidator( m_validatorTwoBytes.get() );
    ui->lineEdit_8->setValidator( m_validatorTwoBytes.get() );

    udphdr& curHeader = GeneratedPackets::instance().m_uUdp.header;

    bindWithVar< uint16_t >( ui->lineEdit_6, curHeader.source, ui->label_6 );
    bindWithVar< uint16_t >( ui->lineEdit_7, curHeader.dest, ui->label_5 );
    bindWithVar< uint16_t >( ui->lineEdit_5, curHeader.len );
    bindWithVar< uint16_t >( ui->lineEdit_8, curHeader.check );

    connect( ui->checkBox_2, &QCheckBox::stateChanged, this,
             [ this ]( int state )
    {
        ui->lineEdit_8->setText( QString::number(
            GeneratedPackets::instance().m_uUdp.header.check ) );

        ui->lineEdit_8->setEnabled( static_cast<bool>( state ) );

        updateUdpView();
    });

    connect( ui->checkBox_16, &QCheckBox::stateChanged, this,
             [ this ]( int state )
    {
        ui->lineEdit_5->setText( QString::number(
            GeneratedPackets::instance().m_uUdp.header.len ) );

        ui->lineEdit_5->setEnabled( static_cast<bool>( state ) );

        updateUdpView();
    });

    updateUdpView();
}

void MainWindow::updateUdpView()
{

    auto& packets = GeneratedPackets::instance();
    auto mainData = ui->textEditData->toPlainText().toStdString();

    if ( !ui->checkBox_16->isChecked() )
    {
        // Update udp len
        packets.m_uUdp.header.len = sizeof( udphdr ) + mainData.size();
        ui->lineEdit_5->setText( QString::number( packets.m_uUdp.header.len ) );

        packets.m_uUdp.header.len = htons( packets.m_uUdp.header.len );

    }

    if ( !ui->checkBox_2->isChecked() )
    {
        qDebug() << "update udp";
        recalcCheckSum( INDEX_TAB_UDP, ui->lineEdit_8 );
    }

    // Encapsulation
    QByteArray udpData = QByteArray(
        ( const char* )packets.m_uUdp.bytes,
        sizeof( udphdr )
    ) ;

    // Adding main data
    udpData += QByteArray(
        ( const char* )mainData.c_str(), mainData.size()
    );

    m_udpDump->setData( udpData );
}
