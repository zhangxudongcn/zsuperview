#include <zbaseview.h>
#include <zbasescene.h>

ZBaseView::ZBaseView( QWidget *parent ) : QGraphicsView( parent ), _p_data( nullptr )
{
}

ZBaseView::~ZBaseView()
{
}

void ZBaseView::setScene(QGraphicsScene *scene)
{
    ZBaseScene *z_scene = dynamic_cast<ZBaseScene*>( scene );
    if ( z_scene ) {
        if ( z_scene->parentScene() == nullptr ) {
            QGraphicsView::setScene( scene );
        }
    }
}
