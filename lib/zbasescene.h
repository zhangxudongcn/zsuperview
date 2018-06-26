#ifndef ZBASESCENE_H_
#define ZBASESCENE_H_

#include <QGraphicsScene>
#include <QSharedPointer>
class ZBaseScenePrivate;
class ZBaseScene : public QGraphicsScene {
    Q_OBJECT
public:
    ZBaseScene();
    ~ZBaseScene();
    ZBaseScene* parentScene() const;
    QList<ZBaseScene*> subScenes() const;
    bool isRenderBufferEnabled() const;
    void setRenderBufferEnabled( bool flag );
    ZBaseScene* rootScene();
    void addTo( ZBaseScene *parent_scene );
    void removeFromParent();
    void removeChild( ZBaseScene *child_scene );
    void removeChildren();
protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

    ZBaseScene* prevSceneInRenderQueue() const;
    ZBaseScene* nextSceneInRenderQueue() const;
    ZBaseScene* topSceneInRenderQueue() const;
    ZBaseScene* bottomSceneInRenderQueue() const;
private:
    QSharedPointer<ZBaseScenePrivate> _p_data;
};
#endif /* ZBASESCENE_H_ */
