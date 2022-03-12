#include "ChangingDeviceWindow.h"
#include "ui_ChangingDeviceWindow.h"

ChangingDeviceWindow::ChangingDeviceWindow( QWidget* parent, Devices& devices)
    : QDialog( parent )
    , ui( new Ui::ChangingDeviceWindow )
{
    ui->setupUi(this);

    setFixedSize( 200, devices.size() * 30 + 30 );
    setWindowTitle( "Change device" );
    setLayout( ui->gridLayout );
    ui->gridLayout->addWidget( ui->pushButton );

    for ( Devices::size_type i = 0; i < devices.size(); ++i )
    {
        auto* button = new QRadioButton( this );
        button->setText( devices[ i ]->getName().c_str() );
        m_buttonList.append( button );
        ui->verticalLayout->addWidget( button );
    }

    m_buttonList[ 0 ]->setChecked( true );

    connect( ui->pushButton, &QPushButton::clicked, this,
             [ this ]()
    {
        int i;
        for ( i = 0; i < m_buttonList.size(); ++i )
        {
            if ( m_buttonList[ i ]->isChecked() )
            {
                break;
            }
        }

        m_choice = i;

        close();
    });
}

ChangingDeviceWindow::~ChangingDeviceWindow()
{
    for ( auto* button : m_buttonList )
    {
        delete button;
    }

    delete ui;
}
