#include <zbasescene.h>
#include <QList>
#include <QImage>
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>
class ZBaseScenePrivate {
public:
    ZBaseScenePrivate()
    {
        _parent_scene = nullptr;
        _double_buffer = nullptr;
        _is_double_buffer_painted = false;
    }
    ~ZBaseScenePrivate() {
        if ( _double_buffer ) {
            delete _double_buffer;
        }
    }
    ZBaseScene *_parent_scene;
    QList<ZBaseScene*> _children;
    QImage *_double_buffer;
    bool _is_double_buffer_painted;
};
ZBaseScene::ZBaseScene() : QGraphicsScene( 0 ), _p_data( new ZBaseScenePrivate() )
{
}

ZBaseScene::~ZBaseScene()
{
    removeFromParent();
    delete _p_data;
}

ZBaseScene* ZBaseScene::parentScene() const
{
    return _p_data->_parent_scene;
}

QList<ZBaseScene*> ZBaseScene::children() const
{
    return _p_data->_children;
}

bool ZBaseScene::isDoubleBufferEnabled() const
{
    return _p_data->_double_buffer != nullptr;
}

void ZBaseScene::setDoubleBufferEnabled( bool flag )
{
    if ( flag == true ) {
        if ( _p_data->_double_buffer == nullptr ) {
            QDesktopWidget *desktop_widget = QApplication::desktop();
            int screen_count = QGuiApplication::screens().size();
            if ( desktop_widget->isVirtualDesktop() ) {
                screen_count = 1;
            }
            int width = 0;
            int height = 0;
            for (int index = 0; index < screen_count; index++) {
                width += desktop_widget->availableGeometry().width();
                height += desktop_widget->availableGeometry().height();
            }
            _p_data->_double_buffer = new QImage( width, height, QImage::Format_ARGB32_Premultiplied);
            qDebug( "double buffer size is %d, %d", width, height );
            _p_data->_is_double_buffer_painted = false;
        }
    } else { // flag = false
        if ( _p_data->_double_buffer != nullptr ) {
            delete _p_data->_double_buffer;
            _p_data->_double_buffer = nullptr;
            _p_data->_is_double_buffer_painted = false;
        }
    }
}


bool ZBaseScene::isDoubleBufferPainted() const
{
    return _p_data->_is_double_buffer_painted;
}

ZBaseScene* ZBaseScene::rootScene()
{
    ZBaseScene *root_scene = this;
    while ( root_scene->parentScene() != nullptr ) {
        root_scene = root_scene->parentScene();
    }
    return root_scene;
}

void ZBaseScene::addChild( ZBaseScene *child_scene )
{
    if ( child_scene != nullptr && child_scene != this ) {
        child_scene->removeFromParent();
        child_scene->_p_data->_parent_scene = this;
        _p_data->_children.append( child_scene );
        child_scene->setSceneRect(sceneRect());
        // emit signal
    }
}

void ZBaseScene::removeFromParent()
{
    if ( parentScene() ) {
        parentScene()->_p_data->_children.removeOne( this );
        _p_data->_parent_scene = nullptr;
        // emit signal
    }
}

void ZBaseScene::removeChild( ZBaseScene *child_scene )
{
    if ( _p_data->_children.contains( child_scene ) ) {
        child_scene->removeFromParent();
    }
}

void ZBaseScene::removeChildren()
{
    for ( auto &scene : children() ) {
        scene->removeFromParent();
    }
}

QList<ZBaseScene*> ZBaseScene::renderList() const
{
    QList<ZBaseScene*> render_list;
    for ( auto &child : children() ) {
        render_list += child->renderList();
    }
    render_list.append( const_cast<ZBaseScene*>(this) );
    return render_list;
}

void ZBaseScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    if ( this == rootScene() ) {
        QGraphicsScene::drawBackground(painter,rect);
    }
    // draw nothing
}

void ZBaseScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    if ( this == rootScene() ) {
        QGraphicsScene::drawForeground(painter,rect);
    }
}

void ZBaseScene::drawScene( QPainter *painter, const QRect &viewport_exposed_rect, const QRectF &scene_exposed_rect)
{
    if ( isDoubleBufferEnabled() ) {
        painter->drawImage( viewport_exposed_rect, *_p_data->_double_buffer, viewport_exposed_rect, Qt::NoFormatConversion );
    } else {
        render(painter, viewport_exposed_rect, scene_exposed_rect );
    }
}

bool ZBaseScene::event(QEvent *event)
{
    if ( this == rootScene() ) {
        QList<ZBaseScene*> render_list = renderList();
        render_list.removeLast(); // remove root scene for avoiding loop forever
        for ( auto &layer : render_list ) {
            if( layer->event( event ) == true ) {
                return true;
            }
        }
        return rootScene()->QGraphicsScene::event( event );
    } else {
        return QGraphicsScene::event( event );
    }
    return false;
}


void ZBaseScene::renderDoubleBuffer( const QPainter &painter, const QRect &viewport_exposed_rect, const QRectF &scene_exposed_rect )
{
    if (isDoubleBufferEnabled()) {
        QPainter buffer_painter( _p_data->_double_buffer );
        buffer_painter.setRenderHints(buffer_painter.renderHints(), false);
        buffer_painter.setRenderHints(painter.renderHints(), true);
        // erase buffer
        for ( int y_index = viewport_exposed_rect.top(); y_index <= viewport_exposed_rect.bottom(); y_index++ ) {
            memset( _p_data->_double_buffer->scanLine(y_index) + viewport_exposed_rect.left() * sizeof(QRgb), 0, sizeof(QRgb) * viewport_exposed_rect.width() );
        }
        // render scene
        render( &buffer_painter, viewport_exposed_rect, scene_exposed_rect );
        _p_data->_is_double_buffer_painted = true;
    }
}


void ZBaseScene::renderDoubleBuffer( const QPainter &painter, const QVector<QRect> viewport_exposed_rects, const QVector<QRectF> &scene_exposed_rects )
{
    if (isDoubleBufferEnabled()) {
        for ( int r_index = 0; r_index < viewport_exposed_rects.size(); r_index++ ) {
            renderDoubleBuffer(painter, viewport_exposed_rects[r_index], scene_exposed_rects[r_index] );
        }
    }
}

void ZBaseScene::translateDoubleBuffer( int dx, int dy, const QRect &valid_rect )
{
    if ( isDoubleBufferEnabled() == true ) {
        if ( abs( dx ) < valid_rect.width() && abs( dy ) < valid_rect.height()) {
            QPainter painter(_p_data->_double_buffer);
            painter.drawImage(QPoint( dx, dy ), *(_p_data->_double_buffer), valid_rect );
        }
    }
}

ZBaseScene* ZBaseScene::prevSceneInRenderList() const
{
    return nullptr;
}
ZBaseScene* ZBaseScene::nextSceneInRenderList() const
{
    return nullptr;
}
ZBaseScene* ZBaseScene::topSceneInRenderList() const
{
    return nullptr;
}
ZBaseScene* ZBaseScene::bottomSceneInRenderList() const
{
    return nullptr;
}
