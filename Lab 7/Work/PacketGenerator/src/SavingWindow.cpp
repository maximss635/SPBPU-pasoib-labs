#include <QMessageBox>
#include <QCloseEvent>
#include <QFile>
#include <QDir>

#include "SavingWindow.h"
#include "ui_SavingWindow.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

SavingWindow::SavingWindow( QWidget* parent )
    : QDialog( parent )
    , ui( new Ui::SavingWindow() )
{
    ui->setupUi( this );

    setWindowTitle( "Saving" );
    setLayout( ui->mainLayout );
    setFixedSize( 250, 80 );

    connect( this, &QDialog::finished, this, &SavingWindow::onFinish );
}

SavingWindow::~SavingWindow()
{
    delete ui;
}

void SavingWindow::onFinish( int result )
{
    if ( 1 == result )
    {
        if ( !savePacket() )
        {
            show();
        }
        else
        {
            MainWindow::instance().setEnabled( true );
        }
    }
    else
    {
        MainWindow::instance().setEnabled( true );
    }
}

bool SavingWindow::savePacket()
{
    if ( !QDir( PATH_SAVE ).exists() )
    {
        QDir().mkdir( PATH_SAVE );
    }

    if ( ui->lineEdit->text().isEmpty() )
    {
        //QMessageBox::warning( this, "Error", "Filename is empty", "Ok" );
        return false;
    }

    auto pathSave = PATH_SAVE + ui->lineEdit->text() + ".bin";
    if ( QFile::exists( pathSave ) )
    {
        QMessageBox::warning( this, "Error", "Already exists: " + pathSave, "Ok" );
        return false;
    }

    QFile fileSave( pathSave );
    fileSave.open( QIODevice::WriteOnly );

    QByteArray data = MainWindow::instance().getCurrentDump();

    // Ether incapsulation
    data = QByteArray(
        ( const char* )GeneratedPackets::instance().m_uEth.bytes, sizeof( ether_header )
    ) + data;

    fileSave.write( data );

    fileSave.close();

    return true;
}
