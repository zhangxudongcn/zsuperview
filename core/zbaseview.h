#ifndef ZBASEVIEW_H_
#define ZBASEVIEW_H_
#include <QGraphicsView>
class ZBaseScene;
class ZBaseView : public QGraphicsView
{
    Q_OBJECT
public:
    ZBaseView( QWidget *parent = nullptr );
    ~ZBaseView();
    
    // make follow base class function to virtual function
    virtual void setScene(QGraphicsScene *scene );
protected:
    void paintEvent( QPaintEvent *event ) override;
    void scrollContentsBy(int dx, int dy) override;

};
#endif /*ZBASEVIEW_H_*/

