#include <zbaseview.h>
#include <zbasescene.h>
#include <QPaintEvent>
#include <QStyleOptionRubberBand>
#include <QStyle>

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

void ZBaseView::paintEvent( QPaintEvent *event )
{
    if ( scene() == nullptr ) {
        QGraphicsView::paintEvent(event);
        return;
    }
    qDebug( "call paint event" );
    //QGraphicsView::paintEvent(event);
    //return;
    // Set up painter state protection.
    //zxd d->scene->d_func()->painterStateProtection = !(d->optimizationFlags & DontSavePainterState);

    // Determine the exposed region
    QRegion exposedRegion = event->region();
    QRect viewportExposedRect = exposedRegion.boundingRect();
    QRectF sceneExposedRect = mapToScene( viewportExposedRect ).boundingRect();

    // Set up the painter
    QPainter painter(viewport());

    // draw background
    painter.fillRect( viewportExposedRect, backgroundBrush() );


    painter.save(); // save for rubberband
    // Set up render hints
    painter.setRenderHints(painter.renderHints(), false);
    painter.setRenderHints(renderHints(), true);
    scene()->render( &painter, viewportExposedRect, sceneExposedRect );
    // Rubberband
    painter.restore(); // restore for rubberband
    

    if (!rubberBandRect().isEmpty()) {
        QStyleOptionRubberBand option;
        option.initFrom(viewport());
        option.rect = rubberBandRect();
        option.shape = QRubberBand::Rectangle;

        QStyleHintReturnMask mask;
        if (viewport()->style()->styleHint(QStyle::SH_RubberBand_Mask, &option, viewport(), &mask)) {
            // painter clipping for masked rubberbands
            painter.setClipRegion(mask.region, Qt::IntersectClip);
        }

        viewport()->style()->drawControl(QStyle::CE_RubberBand, &option, &painter, viewport());
    }

    // Restore painter state protection.
    //d->scene->d_func()->painterStateProtection = true;
}
