#pragma once

#include <QMainWindow>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>

#include <netinet/in.h>
#include <memory>

#include "MemoryViewer.h"
#include "GeneratedPackets.h"
#include "SavingWindow.h"
#include "PoolWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    inline static MainWindow* s_pMainWindow = nullptr;

public:
    static MainWindow& instance()
    {
        return* s_pMainWindow;
    }

public:
    MainWindow( QWidget* parent = nullptr );
    ~MainWindow();

    QByteArray getCurrentDump() const;

public:
    static constexpr int INDEX_TAB_DATA = 0;
    static constexpr int INDEX_TAB_TCP = 1;
    static constexpr int INDEX_TAB_UDP = 2;
    static constexpr int INDEX_TAB_ICMP = 3;
    static constexpr int INDEX_TAB_IP = 4;

private:
    void initTcpView();
    void initUdpView();
    void initIcmpView();
    void initIpView();

    void updateView();
    void updateTcpView();
    void updateUdpView();
    void updateIcmpView();
    void updateIpView();

    void recalcCheckSum( int tab, QLineEdit* pLineEdit );

    QString checkValidateErrors();
    void markLabel( bool cond, QLabel* pLabel );

    void onButtonSave();
    void onButtonPool();

    template< typename T >
    void bindCheckBoxWithBit( QCheckBox* pCheckBox, T& byte, int bit )
    {
        connect( pCheckBox, &QCheckBox::stateChanged, this,
                 [ this, &byte, bit ]( int state )
        {
            if ( state )
            {
                byte |= ( 1 << bit );
            }
            else
            {
                byte &= ~( 1 << bit );
            }

            updateView();
        } );
    }

    template< typename T >
    void bindWithVar( QLineEdit* pLineEdit, T& var, QLabel* pLabel = nullptr )
    {
        connect( pLineEdit, &QLineEdit::textChanged, this,
                 [ this, &var, pLineEdit, pLabel ]( const QString& s )
        {
            onLineEditChanged( s, var, pLineEdit, pLabel );
        } );

        if ( pLabel )
            markLabel( false, pLabel );
    }

    template< typename T >
    void onLineEditChanged( const QString& newText,
                            T& bintVar,
                            QLineEdit* pLineEdit,
                            QLabel* pLabel  )
    {
        bintVar = newText.toInt();

        if ( sizeof( T ) == sizeof( uint16_t ) )
        {
            bintVar = htons( bintVar );
        }
        else if ( sizeof( T ) == sizeof( uint32_t ) )
        {
            bintVar = htonl( bintVar );
        }

        if ( pLabel )
            markLabel( pLineEdit->hasAcceptableInput(), pLabel );

        updateView();
    }

private:
    Ui::MainWindow* ui;

    std::unique_ptr< QWidget > m_savingWindow = std::make_unique< SavingWindow >( this );
    std::unique_ptr< QWidget > m_poolWindow;

    // Validators to line edit
    std::unique_ptr< QIntValidator > m_validatorFourBits = std::make_unique< QIntValidator >( 1, 15, this );
    std::unique_ptr< QIntValidator > m_validatorOneByte = std::make_unique< QIntValidator >( 0, 255, this );
    std::unique_ptr< QIntValidator > m_validatorTwoBytes = std::make_unique< QIntValidator >( 1, 65535, this );
    std::unique_ptr< QIntValidator > m_validatorFourBytes = std::make_unique< QIntValidator >( this );

    // Hex views
    std::unique_ptr< MemoryViewer > m_tcpDump = std::make_unique< MemoryViewer >( this );
    std::unique_ptr< MemoryViewer > m_udpDump = std::make_unique< MemoryViewer >( this );
    std::unique_ptr< MemoryViewer > m_icmpDump = std::make_unique< MemoryViewer >( this );
    std::unique_ptr< MemoryViewer > m_ipDump = std::make_unique< MemoryViewer >( this );

    QList< QList< QLabel* > > m_labelsInTabs;

};
