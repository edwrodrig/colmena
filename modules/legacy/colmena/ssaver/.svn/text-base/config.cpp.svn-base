#include <config.hpp>
#include <QVBoxLayout>
#include <QLabel>

Config::Config ( void ) : QWidget() , colmenaLogo ( ":/data/colmenalogo.jpg" ) , udecLogo ( ":/data/udeclogo.jpg" )
{
	init () ;
}

void Config::init ( void )
{
	QVBoxLayout * layout = new QVBoxLayout ( this ) ;
	layout->setAlignment ( Qt::AlignVCenter ) ;
    QLabel * label1 = new QLabel( this );
    QLabel * label2 = new QLabel( this );
    QLabel * label3 = new QLabel( this );
    QLabel * label4 = new QLabel( this );
	
	label1->setAlignment ( Qt::AlignHCenter ) ;
	label2->setAlignment ( Qt::AlignHCenter ) ;
	label3->setAlignment ( Qt::AlignHCenter ) ;
	label4->setAlignment ( Qt::AlignHCenter ) ;
	
	label1->setPixmap ( colmenaLogo ) ;
	label2->setText ( "Colmena Virtual Cluster" ) ;
	label3->setPixmap ( udecLogo ) ;
	label4->setText ( "Universidad de Concepción (2009)\nhttp://colmena.udec.cl" ) ;
	
    layout->addWidget( label1 ) ;
    layout->addWidget( label2 ) ;
    layout->addWidget( label3 ) ;
    layout->addWidget( label4 ) ;
}
