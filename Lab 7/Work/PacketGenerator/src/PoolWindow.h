#pragma once

#include <QDialog>
#include <QThread>

#include "ChangingDeviceWindow.h"

namespace Ui {
class PoolWindow;
}

typedef QList< QPair< QByteArray, QPair< int, int > > > ModelPackets;

class ThreadSenderPacket : public QThread
{
    Q_OBJECT

public:
    ThreadSenderPacket();

    void run() override;

    bool isDeviceOpened() const
    {
        return m_isOpened;
    }

    void setModel( ModelPackets model )
    {
        m_model = model;
    }

    void setDevice( pcpp::PcapLiveDevice* device );

signals:
    void packetSent( int progress );

private:
    ModelPackets m_model;
    pcpp::PcapLiveDevice* m_pDev;
    bool m_isOpened;
};

// ============================================================

class PoolWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PoolWindow( QWidget* parent = nullptr );
    ~PoolWindow();

private:
    void scanDevices();
    QWidget* makeDynamicButton( const QString& text, int i );

private slots:
    void updateLeftTable();
    void onButtonAddToPool( int i );
    void onButtonSend();

private:
    Ui::PoolWindow* ui;
    std::unique_ptr< ThreadSenderPacket > m_sender;

    Devices m_devices;
    int m_curDeviceIndex = 0;
};
