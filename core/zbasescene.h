#ifndef ZBASESCENE_H_
#define ZBASESCENE_H_

#include <QGraphicsScene>

class ZBaseScenePrivate;
class ZBaseScene : public QGraphicsScene {
    friend class ZBaseView;
    Q_OBJECT
public:
    ZBaseScene();
    ~ZBaseScene();
    ZBaseScene* parentScene() const;
    bool isSceneVisible() const;
    void setSceneVisible( bool flag );
    QList<ZBaseScene*> children() const;
    ZBaseScene* rootScene();
    void addChild( ZBaseScene *child_scene );

    void removeChild( ZBaseScene *child_scene);
    void removeChildren();

    void swapChild( ZBaseScene *child_1, ZBaseScene *child_2 );
    void moveChildUp( ZBaseScene *child_scene );
    void moveChildDown( ZBaseScene *child_scene );
    void moveChildTop( ZBaseScene *child_scene );
    void moveChildBottom( ZBaseScene *child_scene );
                      

    // make follow base class function to virtual function
    virtual void setSceneRect(const QRectF &rect);
    virtual void render(QPainter *painter, const QRectF &target = QRectF(), const QRectF &source = QRectF(), Qt::AspectRatioMode aspectRatioMode = Qt::KeepAspectRatio);
protected:
    QList<ZBaseScene*> renderList() const;
    void removeFromParent();
    bool event(QEvent *event) override;

    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;
    void drawScene( QPainter *painter, const QRect &viewport_exposed_rect, const QRectF &scene_exposed_rect);

    virtual void renderDoubleBuffer( const QPainter &painter, const QList<QRect> v_exposed_rects, const QList<QRectF> &s_exposed_rects,
                                     const QTransform &transform, const QPointF &scene_point, const QRect &viewport_rect );
    virtual void translateDoubleBuffer( int dx, int dy, const QRect &viewport_rect );
protected slots:
    void selfChangedSlot( const QList<QRectF> &region );
private:
    ZBaseScenePrivate *_p_data;
};
#endif /* ZBASESCENE_H_ */
