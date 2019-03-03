#include <zbaseview.h>
#include <zbasescene.h>
#include <QPaintEvent>
#include <QStyleOptionRubberBand>
#include <QStyle>
#include <omp.h>
ZBaseView::ZBaseView( QWidget *parent ) : QGraphicsView( parent )
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
    qDebug( "call paint event: %d", event->region().rectCount() );
    //QGraphicsView::paintEvent(event);return;
    

    // Set up the painter
    QPainter painter(viewport());

    // draw background
    //drawBackground(&painter, viewportExposedRect );
    //painter.fillRect( viewportExposedRect, backgroundBrush() );


    painter.save(); // save for rubberband
    // Set up render hints
    painter.setRenderHints(painter.renderHints(), false);
    painter.setRenderHints(renderHints(), true);

    QList<QRect> viewport_exposed_rects;
    QList<QRectF> scene_exposed_rects;
    for ( auto rect_iter = event->region().begin(); rect_iter < event->region().end(); rect_iter++ ) {
        viewport_exposed_rects.push_back( *rect_iter );
        qDebug( "paint rect : %d, %d, %d, %d", rect_iter->left(), rect_iter->top(), rect_iter->width(), rect_iter->height() );
        scene_exposed_rects.push_back( mapToScene( *rect_iter ).boundingRect() );
    }

    // render double buffer
    QList<ZBaseScene *> render_list = ((ZBaseScene*)scene())->renderList();
    QPointF scene_point = mapToScene( QPoint(0,0));
    #pragma omp parallel for
    for ( int scene_index = 0; scene_index < render_list.size(); scene_index++ ) {
        qDebug("i = %d, I am Thread %d", scene_index, omp_get_thread_num());
        render_list[scene_index]->renderDoubleBuffer( painter, viewport_exposed_rects, scene_exposed_rects, transform(), scene_point, viewport()->rect() );
    }

    for ( int r_index = 0; r_index < viewport_exposed_rects.size(); r_index++ ) { // openmp 不支持 for( auto &item : container )格式
        for ( auto scene_iter = render_list.rbegin(); scene_iter != render_list.rend(); scene_iter++ ) {
            (*scene_iter)->drawScene( &painter, viewport_exposed_rects[r_index], scene_exposed_rects[r_index] );
        }
    }

    //scene()->render(&painter, viewportExposedRect, sceneExposedRect);
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

void ZBaseView::scrollContentsBy(int dx, int dy)
{
    QList<ZBaseScene *> render_list = ((ZBaseScene*)scene())->renderList();
    // get update viewport rects and scene rects

    for ( auto &scene : render_list ) {
        scene->translateDoubleBuffer(dx, dy, viewport()->rect() );
    }
    QGraphicsView::scrollContentsBy(dx, dy);
}
