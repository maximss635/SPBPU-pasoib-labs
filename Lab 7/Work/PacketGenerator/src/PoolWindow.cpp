#include <QPushButton>
#include <QMessageBox>
#include <QRadioButton>
#include <QDir>
#include <QDebug>

#include "PoolWindow.h"
#include "ui_PoolWindow.h"

PoolWindow::PoolWindow( QWidget* parent )
    : QDialog( parent )
    , ui( new Ui::PoolWindow() )
{
    ui->setupUi( this );
    setLayout( ui->mainLayoutPool );

    updateLeftTable();

    connect( ui->buttonUpdate, &QPushButton::clicked, this, &PoolWindow::updateLeftTable );
    connect( ui->buttonSend, &QPushButton::clicked, this, &PoolWindow::onButtonSend, Qt::AutoConnection );
    connect( ui->buttonChangeDevice, &QPushButton::clicked, this,
             [ this ]()
    {
        auto* window = new ChangingDeviceWindow( this, m_devices );
        window->exec();

        m_curDeviceIndex = window->getChoice();
        ui->label_4->setText( m_devices[ m_curDeviceIndex ]->getName().c_str() );

        delete window;

        m_sender->setDevice( m_devices[ m_curDeviceIndex ] );

        if ( !m_sender->isDeviceOpened() )
        {
            QMessageBox::warning( this,
                                  "Error",
                                  "Cann't open socket\n"
                                    "Try run programm with 'sudo'");

            ui->buttonSend->setEnabled( false );
        }
        else
        {
            ui->buttonSend->setEnabled( true );
        }
    });

    auto font = ui->label->font();
    font.setPixelSize( 16 );
    font.setBold( true );

    ui->label->setFont( font );
    ui->label_2->setFont( font );
    ui->label->setAlignment( Qt::AlignCenter );
    ui->label_2->setAlignment( Qt::AlignCenter );

    ui->tableLeft->setColumnWidth( 0, width() / 2 - ui->tableLeft->columnWidth( 1 ) - 20 );
    ui->tableRight->setColumnWidth( 0, width() / 6 );
    ui->tableRight->setColumnWidth( 1, width() / 6 );
    ui->tableRight->setColumnWidth( 2, width() / 6 );

    ui->tableLeft->setEditTriggers( QAbstractItemView::NoEditTriggers );

    ui->progressBar->setValue( 0 );

    m_sender.reset( new ThreadSenderPacket() );

    connect( m_sender.get(), &ThreadSenderPacket::packetSent,
             ui->progressBar, &QProgressBar::setValue );

    connect( m_sender.get(),  &QThread::finished, this,
             [ this ]()
    {
        ui->progressBar->setValue( ui->progressBar->maximum() );
        ui->buttonSend->setEnabled( true );
    });

    scanDevices();
}

void PoolWindow::updateLeftTable()
{
    QStringList savedPackets = QDir( "SavePackets/" ).entryList();
    savedPackets.removeOne( "." );
    savedPackets.removeOne( ".." );

    ui->tableLeft->clear();
    ui->tableLeft->setRowCount( savedPackets.size() );

    for ( int i = 0; i < ui->tableLeft->rowCount(); ++i )
    {
        auto btn = makeDynamicButton( "Add to pool", i );

        ui->tableLeft->setItem( i, 0, new QTableWidgetItem( savedPackets[ i ] ) );
        ui->tableLeft->setIndexWidget( ui->tableLeft->model()->index( i, 1 ),
                                        btn );
    }
}

PoolWindow::~PoolWindow()
{
    delete ui;
}

void PoolWindow::scanDevices()
{
    m_devices = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();
    ui->label_4->setText( m_devices[ m_curDeviceIndex ]->getName().c_str() );

    m_sender->setDevice( m_devices[ m_curDeviceIndex ] );

    if ( !m_sender->isDeviceOpened() )
    {
        QMessageBox::warning( this,
                              "Error",
                              "Cann't open socket\n"
                                "Try run programm with 'sudo'");

        ui->buttonSend->setEnabled( false );
    }
    else
    {
        ui->buttonSend->setEnabled( true );
    }

    for ( auto* pDevice : m_devices )
    {
        qDebug() << "Device: " << pDevice->getName().c_str()
                 << "(" << pDevice->getIPv4Address().toString().c_str() << ")"
                 << pDevice->getMacAddress().toString().c_str();
    }
}

QWidget* PoolWindow::makeDynamicButton( const QString& text, int i )
{
    QWidget* wgt = new QWidget();
    QBoxLayout* l = new QHBoxLayout();
    QPushButton* btn = new QPushButton( text );

    l->setMargin( 0 );
    l->addWidget( btn );
    l->addStretch();
    wgt->setLayout( l );

    connect( btn, &QPushButton::clicked, this, [ this, i ]()
    {
        onButtonAddToPool( i );
    } );

    return wgt;
}

void PoolWindow::onButtonAddToPool( int i )
{
    QString packetName = ui->tableLeft->item( i, 0 )->text();

    auto row = ui->tableRight->rowCount();

    if ( row > 0 && ui->tableRight->item( row - 1, 0 )->text() == packetName )
    {
        ui->tableRight->setItem( row - 1, 1, new QTableWidgetItem(
            QString::number( ui->tableRight->item( row - 1, 1 )->text().toInt() + 1 )
        ) );
    }
    else
    {
        ui->tableRight->setRowCount( row + 1 );

        ui->tableRight->setItem( row, 0, new QTableWidgetItem( packetName ) );
        ui->tableRight->setItem( row, 1, new QTableWidgetItem( "1" ) );
        ui->tableRight->setItem( row, 2, new QTableWidgetItem( "0" ) );
    }
}

void PoolWindow::onButtonSend()
{
    ModelPackets model;

    for ( int i = 0; i < ui->tableRight->rowCount(); ++i )
    {
        QString packetName = "SavePackets/" + ui->tableRight->item( i, 0 )->text();

        QFile file( packetName );
        file.open( QIODevice::OpenModeFlag::ReadOnly );

        QByteArray data = file.readAll();
        int count = ui->tableRight->item( i, 1 )->text().toInt();
        int delay = ui->tableRight->item( i, 2 )->text().toInt();

        model.append( QPair( data, QPair( count, delay ) ) );
        file.close();
    }

    int total = 0;
    for ( int i = 0; i < ui->tableRight->rowCount(); ++i )
    {
        total += ui->tableRight->item( i, 1 )->text().toInt();
    }

    ui->progressBar->setMaximum( total );

    ui->buttonSend->setEnabled( false );
    m_sender->setModel( model );
    m_sender->start();
}

// ====================================

ThreadSenderPacket::ThreadSenderPacket()
    : QThread()
{
}

void ThreadSenderPacket::run()
{
    int k = 0;
    for ( int i = 0; i < m_model.size(); ++i )
    {
        auto& curPacketInfo = m_model[i];
        auto count = curPacketInfo.second.first;
        auto delay = curPacketInfo.second.second;

        pcpp::RawPacket curPacket( ( const uchar* )curPacketInfo.first.data(),
                                   curPacketInfo.first.size(),
                                   timespec(),
                                   false);

        for ( int j = 0; j < count; ++j, ++k )
        {
            if ( delay > 0 )
            {
                msleep( delay );
            }

            m_pDev->sendPacket( curPacket );

            qDebug() << curPacketInfo.first.size()
                     << "bytes sent, delay=" << delay
                     << ", interface =" << m_pDev->getName().c_str();

            emit packetSent( k );
        }
    }
}

void ThreadSenderPacket::setDevice( pcpp::PcapLiveDevice* device )
{
    if ( !isRunning() )
    {
        m_pDev = device;
        m_isOpened = m_pDev->open();
    }
}
