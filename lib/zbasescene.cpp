#include <zbasescene.h>
#include <QList>
#include <QImage>
#include <QDesktopWidget>
#include <QApplication>
class ZBaseScenePrivate {
public:
    ZBaseScenePrivate()
    {
        _parent_scene = nullptr;
        _render_buffer = nullptr;
    }
    ZBaseScene *_parent_scene;
    QList<ZBaseScene*> _sub_scenes;
    QImage *_render_buffer;
};
ZBaseScene::ZBaseScene() : QGraphicsScene( 0 ), _p_data( new ZBaseScenePrivate() )
{
}

ZBaseScene::~ZBaseScene()
{
    removeFromParent();
}


ZBaseScene* ZBaseScene::parentScene() const
{
    return _p_data->_parent_scene;
}

QList<ZBaseScene*> ZBaseScene::subScenes() const
{
    return _p_data->_sub_scenes;
}


bool ZBaseScene::isRenderBufferEnabled() const
{
    return _p_data->_render_buffer != nullptr;
}

void ZBaseScene::setRenderBufferEnabled( bool flag )
{
    if ( flag == true ) {
        if ( _p_data->_render_buffer == nullptr ) {
            QDesktopWidget *desktop_widget = QApplication::desktop();
            int screen_count = desktop_widget->screenCount();
            if ( desktop_widget->isVirtualDesktop() ) {
                screen_count = 1;
            }
            int width = 0;
            int height = 0;
            for (int index = 0; index < screen_count; index++) {
                width += desktop_widget->availableGeometry().width();
                height += desktop_widget->availableGeometry().height();
            }
            _p_data->_render_buffer = new QImage(QSize( width, height ), QImage::Format_ARGB32_Premultiplied);
        }
    } else { // flag = false
        if ( _p_data->_render_buffer != nullptr ) {
            delete _p_data->_render_buffer;
            _p_data->_render_buffer = nullptr;
        }
    }
}

ZBaseScene* ZBaseScene::rootScene()
{
    ZBaseScene *root_scene = this;
    while ( root_scene->parentScene() != nullptr ) {
        root_scene = root_scene->parentScene();
    }
    return root_scene;
}

void ZBaseScene::addTo( ZBaseScene *parent_scene )
{
    removeFromParent();
    _p_data->_parent_scene = parent_scene;
    if ( parent_scene ) {
        // emit signal
    }
}

void ZBaseScene::removeFromParent()
{
    if ( parentScene() ) {
        parentScene()->_p_data->_sub_scenes.removeOne( this );
        _p_data->_parent_scene = nullptr;
    }
}

void ZBaseScene::removeChild( ZBaseScene *child_scene )
{
    if ( _p_data->_sub_scenes.contains( child_scene ) ) {
        child_scene->removeFromParent();
    }
}

void ZBaseScene::removeChildren()
{

}

void ZBaseScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    if ( this == rootScene() ) {
        // draw background
    }
}

void ZBaseScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    // draw nothing
}

void ZBaseScene::drawItems(QPainter *painter, int numItems,
                           QGraphicsItem *items[],
                           const QStyleOptionGraphicsItem options[],
                           QWidget *widget)
{
    QGraphicsScene::drawItems(painter,numItems,items,options,widget);
}
