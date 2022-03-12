#include <QMessageBox>
#include <QString>
#include <QDebug>

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "CheckSum.h"

MainWindow::MainWindow( QWidget* parent )
    : QMainWindow( parent )
    , ui( new Ui::MainWindow )
{
    s_pMainWindow = this;

    ui->setupUi( this );
    setWindowTitle( "Packet generator" );
    resize( 700, 500 );

    ui->centralwidget->setLayout( ui->mainLayout );
    ui->mainTabWidget->setTabText( INDEX_TAB_DATA, "Data" );
    ui->mainTabWidget->setTabText( INDEX_TAB_TCP, "TCP" );
    ui->mainTabWidget->setTabText( INDEX_TAB_UDP, "UDP" );
    ui->mainTabWidget->setTabText( INDEX_TAB_ICMP, "ICMP" );
    ui->mainTabWidget->setTabText( INDEX_TAB_IP, "IP" );

    ui->tabData->setLayout( ui->mainLayoutData );

    initTcpView();
    initUdpView();
    initIpView();
    initIcmpView();

    connect( ui->buttonSave, &QPushButton::clicked, this, &MainWindow::onButtonSave );
    connect( ui->buttonPool, &QPushButton::clicked, this, &MainWindow::onButtonPool );

    for ( int i = 0; i < ui->mainTabWidget->count(); ++i )
        m_labelsInTabs.append( QList< QLabel* >() );

    m_labelsInTabs[ INDEX_TAB_TCP ] = {
        ui->label_7, ui->label,
        ui->label_2, ui->label_7,
        ui->label_9, ui->label_10
    };

    m_labelsInTabs[ INDEX_TAB_UDP ] = {
        ui->label_5, ui->label_6
    };

    m_labelsInTabs[ INDEX_TAB_IP ] = {
        ui->label_11, ui->label_12,
        ui->label_19, ui->label_20,
        ui->label_24, ui->label_22,
        ui->label_25, ui->label_23
    };

    m_labelsInTabs[ INDEX_TAB_ICMP ] = {
        ui->label_53,
        ui->label_54,
        ui->label_55,
        ui->label_56,
    };

    connect( ui->mainTabWidget, &QTabWidget::currentChanged, this,
             [ this ]( int index )
    {
        ui->buttonSave->setEnabled( index == MainWindow::INDEX_TAB_IP );
        updateView();
    });

    ui->buttonSave->setEnabled( ui->mainTabWidget->currentIndex() == INDEX_TAB_IP );
}

MainWindow::~MainWindow()
{
    delete ui;
}

QByteArray MainWindow::getCurrentDump() const
{
    switch( ui->mainTabWidget->currentIndex() )
    {
    case INDEX_TAB_ICMP:
        return m_icmpDump->data();

    case INDEX_TAB_TCP:
        return m_tcpDump->data();

    case INDEX_TAB_UDP:
        return m_udpDump->data();

    case INDEX_TAB_IP:
        return m_ipDump->data();

    default:
        return nullptr;
    }
}

void MainWindow::markLabel( bool cond, QLabel* pLabel )
{
    if ( pLabel->text().toStdString().substr( 0, 3 ) != "   " &&
         pLabel->text().toStdString().substr( 0, 3 ) != " * " )
    {
        pLabel->setText( "   " + pLabel->text() );
    }

    if ( cond )
    {
        if ( pLabel->text().at( 1 ) == '*' )
        {
            auto oldText = pLabel->text().toStdString();
            auto newText = "   " + oldText.erase( 0, 3 );
            pLabel->setText( newText.c_str() );

            pLabel->setStyleSheet( "QLabel { background-color : white; color : black; }" );
        }

    }
    else
    {
        if ( pLabel->text().at( 1 ) == ' ' )
        {
            auto oldText = pLabel->text().toStdString();
            auto newText = " * " + oldText.erase( 0, 3 );
            pLabel->setText( newText.c_str() );
        }

        pLabel->setStyleSheet( "QLabel { background-color : white; color : red; }" );
    }
}

void MainWindow::onButtonSave()
{
    QString errorField = checkValidateErrors();

    if ( errorField.isEmpty() )
    {
        this->setEnabled( false );
        m_savingWindow->setEnabled( true );
        m_savingWindow->show();
    }
    else
    {
        errorField = errorField.toStdString().erase( 0, 3 ).c_str();
        std::string::size_type pos = errorField.toStdString().find( "(" );
        if ( pos != std::string::npos )
        {
            errorField = errorField.toStdString().erase( pos ).c_str();
        }

        if ( errorField.back() == ' ' )
        {
            errorField = errorField.toStdString().erase( errorField.size() - 1 ).c_str();
        }

        QMessageBox::warning( this, "Error",
                              "Some fields have incorrect values:\n\t\"" +
                              errorField + "\"", "Ok" );
    }
}

void MainWindow::onButtonPool()
{
    m_poolWindow.reset( new PoolWindow( this ) );
    m_poolWindow->show();
}

QString MainWindow::checkValidateErrors()
{
    for ( auto* pLabel : m_labelsInTabs[ ui->mainTabWidget->currentIndex() ] )
    {
        if ( pLabel->text()[ 1 ] == '*' )
        {
            if ( pLabel == ui->label_55 || pLabel == ui->label_56 )
            {
                if ( GeneratedPackets::instance().m_uIcmp.header.type == 0 ||
                     GeneratedPackets::instance().m_uIcmp.header.type == 8 )
                {
                    continue;
                }
            }

            return pLabel->text();
        }
    }

    return QString();
}

void MainWindow::updateView()
{
    updateTcpView();
    updateUdpView();
    updateIcmpView();
    updateIpView();
}

void MainWindow::recalcCheckSum( int tab, QLineEdit* pLineEdit )
{
    auto& packets = GeneratedPackets::instance();

    QByteArray mainData(
        ui->textEditData->toPlainText().toStdString().c_str(),
        ui->textEditData->toPlainText().toStdString().size());

    qDebug() << "addr:" << packets.m_uIp.header.daddr;

    QByteArray pseudoData( 12, '\0' );
    pseudoData[ 1 ] = ( packets.m_uIp.header.saddr >> 24 ) & 0xff;
    pseudoData[ 0 ] = ( packets.m_uIp.header.saddr >> 16 ) & 0xff;
    pseudoData[ 3 ] = ( packets.m_uIp.header.saddr >> 8 ) & 0xff;
    pseudoData[ 2 ] = ( packets.m_uIp.header.saddr >> 0 ) & 0xff;
    pseudoData[ 5 ] = ( packets.m_uIp.header.daddr >> 24 ) & 0xff;
    pseudoData[ 4 ] = ( packets.m_uIp.header.daddr >> 16 ) & 0xff;
    pseudoData[ 7 ] = ( packets.m_uIp.header.daddr >> 8 ) & 0xff;
    pseudoData[ 6 ] = ( packets.m_uIp.header.daddr >> 0 ) & 0xff;
    pseudoData[ 9 ] = packets.m_uIp.header.protocol;

    qDebug() << "5:" << pseudoData[5];

    if ( tab == INDEX_TAB_TCP )
        pseudoData[ 11 ] = sizeof( tcphdr ) + mainData.size();
    else if ( tab == INDEX_TAB_UDP )
        pseudoData[ 11 ] = sizeof( udphdr )+ mainData.size();

    if ( mainData.size() % 2 != 0 )
    {
        mainData.append( (char)0 );
    }

    switch ( tab )
    {
    case INDEX_TAB_TCP:
        packets.m_uTcp.header.check = 0;

        packets.m_uTcp.header.check = CheckSum::calcCheckSum(
            QByteArray(
                ( const char* )packets.m_uTcp.bytes,
                sizeof( tcphdr )
        ) + pseudoData + mainData );

        pLineEdit->setText( QString::number( packets.m_uTcp.header.check ) );

        break;

    case INDEX_TAB_UDP:
        qDebug() << "1";
        packets.m_uUdp.header.check = 0;

        packets.m_uUdp.header.check = CheckSum::calcCheckSum(
            QByteArray(
                ( const char* )packets.m_uUdp.bytes,
                sizeof( udphdr )
        ) + pseudoData + mainData );

        pLineEdit->setText( QString::number( packets.m_uUdp.header.check ) );

        break;

    case INDEX_TAB_ICMP:
        packets.m_uIcmp.header.checksum = 0;

        packets.m_uIcmp.header.checksum = CheckSum::calcCheckSum(
            QByteArray(
                ( const char* )packets.m_uIcmp.bytes,
                sizeof( icmphdr )
        ) + mainData );

        pLineEdit->setText( QString::number( packets.m_uIcmp.header.checksum ) );

        break;

    case INDEX_TAB_IP:
        packets.m_uIp.header.check = 0;

        packets.m_uIp.header.check = CheckSum::calcCheckSum(
            QByteArray(
                ( const char* )packets.m_uIp.bytes,
                sizeof( iphdr )
        ));

        pLineEdit->setText( QString::number( packets.m_uIp.header.check ) );

        break;
    }

    pseudoData.clear();
}
