#include "chip.h"
#include "mainwindow.h"
#include "view.h"
#include <zbasescene.h>

#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    populateScene();
    qDebug( " child num = %d", scene->children().size());
    qDebug(" render list num = %d", scene->renderList().size());

    View *view = new View("Top left view");
    view->view()->setScene( scene );
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(view);
    setLayout(layout);
    setWindowTitle(tr("Chip Example"));
}

void MainWindow::populateScene()
{
    scene = new ZBaseScene();

    QImage image(":/qt4logo.png");

    // Populate scene
    int xx = 0;
    int nitems = 0;
    for (int i = -11000; i < 11000; i += 110) {
        ++xx;
        int yy = 0;
        for (int j = -7000; j < 7000; j += 70) {
            ++yy;
            qreal x = (i + 11000) / 22000.0;
            qreal y = (j + 7000) / 14000.0;

            QColor color(image.pixel(int(image.width() * x), int(image.height() * y)));
            QGraphicsItem *item = new Chip(color, xx, yy);
            item->setPos(QPointF(i, j));
            scene->addItem(item);

            ++nitems;
        }
    }
    qDebug( "x0: %f, y0: %f, width = %f, height = %f", scene->sceneRect().x(), scene->sceneRect().y(), scene->sceneRect().width(), scene->sceneRect().height() );
    scene->setBackgroundBrush( Qt::white );
    scene->setDoubleBufferEnabled(true );

    QRectF scene_rect = scene->sceneRect();
    top_scene = new ZBaseScene();
    top_scene->addLine( scene_rect.left(), scene_rect.top(), scene_rect.right(), scene_rect.bottom(), QPen( Qt::red, 10 ) );
    for ( int i = 0; i < 10000; i++ ) {
        top_scene->addLine( random() % int( scene_rect.width() ) - scene_rect.width()/2, random() % int( scene_rect.height() ) - scene_rect.height()/2, 
                            random() % int( scene_rect.width() )- scene_rect.width()/2, random() % int( scene_rect.height() ) - scene_rect.height()/2, QPen(Qt::red, 10));
    }
    top_scene->setDoubleBufferEnabled(true );
    scene->addChild(  top_scene );
    tmp_scene = new ZBaseScene();
    tmp_scene->setDoubleBufferEnabled(true );
    //for ( int i = 0; i < 10000; i++ ) {
    //    tmp_scene->addLine( random() % int( scene_rect.width() ) - scene_rect.width()/2, random() % int( scene_rect.height() ) - scene_rect.height()/2, 
    //                        random() % int( scene_rect.width() )- scene_rect.width()/2, random() % int( scene_rect.height() ) - scene_rect.height()/2, QPen(Qt::blue, 10));
    //}
    top_scene->addChild(  tmp_scene );

}
