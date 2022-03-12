#include <QList>

#include "MainWindow.h"
#include "ui_MainWindow.h"

void MainWindow::initIcmpView()
{
    ui->lineEdit_59->setEnabled( false );
    ui->tabIcmp->setLayout( ui->mainLayoutIcmp );
    ui->mainLayoutIcmp->addWidget( m_icmpDump.get() );

    ui->lineEdit_60->setValidator( m_validatorTwoBytes.get() );
    ui->lineEdit_61->setValidator( m_validatorTwoBytes.get() );
    ui->lineEdit_57->setValidator( m_validatorOneByte.get() );
    ui->lineEdit_58->setValidator( m_validatorOneByte.get() );
    ui->lineEdit_59->setValidator( m_validatorTwoBytes.get() );

    icmphdr& curHeader = GeneratedPackets::instance().m_uIcmp.header;

    connect( ui->checkBox_31, &QCheckBox::stateChanged, this,
             [ this, &curHeader ]( int state )
    {
        ui->lineEdit_59->setEnabled( static_cast< bool >( state ) );
        ui->lineEdit_59->setText( QString::number( curHeader.checksum ) );
        updateIcmpView();
    });

    QList< QWidget* > pingWidgets = {
        ui->label_55, ui->label_56,
        ui->lineEdit_60, ui->lineEdit_61
    };

    for ( auto w : pingWidgets )
    {
        w->setEnabled( false );
    }

    bindWithVar< uint16_t >(ui->lineEdit_61, curHeader.un.echo.id, ui->label_56 );
    bindWithVar< uint8_t >(ui->lineEdit_58, curHeader.code, ui->label_54 );
    bindWithVar< uint16_t >(ui->lineEdit_60, curHeader.un.echo.sequence, ui->label_55 );
    bindWithVar< uint16_t >(ui->lineEdit_59, curHeader.checksum );

    markLabel( false, ui->label_53 );

    connect( ui->lineEdit_57, &QLineEdit::textChanged, this,
             [ this, pingWidgets ]( const QString& text )
    {
        GeneratedPackets::instance().m_uIcmp.header.type = text.toInt();
        markLabel( ui->lineEdit_57->hasAcceptableInput(), ui->label_53 );
        updateView();

        bool isPing = ( GeneratedPackets::instance().m_uIcmp.header.type == 0
             || GeneratedPackets::instance().m_uIcmp.header.type == 8 );

        for ( auto w : pingWidgets )
        {
            w->setEnabled( isPing );
        }
    });

    updateIcmpView();
}

void MainWindow::updateIcmpView()
{
    if ( !ui->checkBox_31->isChecked() )
    {
        recalcCheckSum( INDEX_TAB_ICMP, ui->lineEdit_59 );
    }

    // Encapsulation
    auto& packets = GeneratedPackets::instance();

    QByteArray icmpData(
        ( const char* )packets.m_uIcmp.bytes,
        sizeof( icmphdr )
    );

    auto mainData = ui->textEditData->toPlainText().toStdString();

    icmpData += QByteArray(
        ( const char* )mainData.c_str(), mainData.size()
    );

    m_icmpDump->setData( icmpData );
}
