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
    //QGraphicsView::paintEvent(event);
    //return;
    // Set up painter state protection.
    //zxd d->scene->d_func()->painterStateProtection = !(d->optimizationFlags & DontSavePainterState);

    // Determine the exposed region
    QRegion exposedRegion = event->region();
    QRectF exposedSceneRect = mapToScene(exposedRegion.boundingRect()).boundingRect();

    // Set up the painter
    QPainter painter(viewport());

    // draw background
    painter.fillRect( exposedSceneRect, backgroundBrush() );
#if QT_CONFIG(rubberband)
        painter.save();
#endif
    // Set up render hints
    painter.setRenderHints(painter.renderHints(), false);
    painter.setRenderHints(renderHints(), true);

    // Set up viewport transform
    const bool viewTransformed = isTransformed();
    if (viewTransformed)
        painter.setWorldTransform(viewportTransform());
    const QTransform viewTransform = painter.worldTransform();

    // Items
    if (!(optimizationFlags() & IndirectPainting) && false ) {
        qDebug( "call no indirectpainting" );
#if 0 // zxd
        const quint32 oldRectAdjust = d->scene->d_func()->rectAdjust;
        if (d->optimizationFlags & QGraphicsView::DontAdjustForAntialiasing)
            d->scene->d_func()->rectAdjust = 1;
        else
            d->scene->d_func()->rectAdjust = 2;
        d->scene->d_func()->drawItems(&painter, viewTransformed ? &viewTransform : 0,
                                      &d->exposedRegion, viewport());
        d->scene->d_func()->rectAdjust = oldRectAdjust;
        // Make sure the painter's world transform is restored correctly when
        // drawing without painter state protection (DontSavePainterState).
        // We only change the worldTransform() so there's no need to do a full-blown
        // save() and restore(). Also note that we don't have to do this in case of
        // IndirectPainting (the else branch), because in that case we always save()
        // and restore() in QGraphicsScene::drawItems().
        if (!d->scene->d_func()->painterStateProtection)
            painter.setOpacity(1.0);
        painter.setWorldTransform(viewTransform);
#endif
    } else {
#if 0 // zxd
        // Make sure we don't have unpolished items before we draw
        if (!d->scene->d_func()->unpolishedItems.isEmpty())
            d->scene->d_func()->_q_polishItems();
        // We reset updateAll here (after we've issued polish events)
        // so that we can discard update requests coming from polishEvent().
        d->scene->d_func()->updateAll = false;
#endif

        // Find all exposed items
        QRegion adjustedRegion;
        for (const QRect &r : exposedRegion)
            adjustedRegion += r.adjusted(-1, -1, 1, 1);

        QPainterPath path;
        path.addRegion( adjustedRegion );
        const QPainterPath exposedScenePath(mapToScene(path));
        QList<QGraphicsItem*> itemList =  scene()->items(exposedScenePath, Qt::IntersectsItemBoundingRect, Qt::AscendingOrder, viewTransform);
        scene()->render( &painter );
        if (!itemList.isEmpty()) {
#if 0 // zxd
            // Generate the style options.
            const int numItems = itemList.size();
            QGraphicsItem **itemArray = &itemList[0]; // Relies on QList internals, but is perfectly valid.
            QStyleOptionGraphicsItem *styleOptionArray = d->allocStyleOptionsArray(numItems);
            QTransform transform(Qt::Uninitialized);
            for (int i = 0; i < numItems; ++i) {
                QGraphicsItem *item = itemArray[i];
                QGraphicsItemPrivate *itemd = item->d_ptr.data();
                itemd->initStyleOption(&styleOptionArray[i], viewTransform, d->exposedRegion, allItems);
                // Cache the item's area in view coordinates.
                // Note that we have to do this here in case the base class implementation
                // (QGraphicsScene::drawItems) is not called. If it is, we'll do this
                // operation twice, but that's the price one has to pay for using indirect
                // painting :-/.
                const QRectF brect = adjustedItemEffectiveBoundingRect(item);
                if (!itemd->itemIsUntransformable()) {
                    transform = item->sceneTransform();
                    if (viewTransformed)
                        transform *= viewTransform;
                } else {
                    transform = item->deviceTransform(viewTransform);
                }
                itemd->paintedViewBoundingRects.insert(d->viewport, transform.mapRect(brect).toRect());
            }
#endif
            // Draw the items.
            //QStyleOptionGraphicsItem *styleOptionArray;
            //scene()->render(&painter );
            //d->freeStyleOptionsArray(styleOptionArray);
        }
    }

    // Foreground draw nothing
    //drawForeground(&painter, exposedSceneRect);

#if QT_CONFIG(rubberband)
    // Rubberband
        painter.restore();
        if (!rubberBandRect().isEmpty()) {
            painter.restore();
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
#endif

    painter.end();

    // Restore painter state protection.
    //d->scene->d_func()->painterStateProtection = true;
}
