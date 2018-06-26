#include "chip.h"
#include "mainwindow.h"
#include "view.h"
#include <zbasescene.h>

#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    populateScene();

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

    qDebug( "width = %f, height = %f", scene->sceneRect().width(), scene->sceneRect().height() );
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
    qDebug( "width = %f, height = %f", scene->sceneRect().width(), scene->sceneRect().height() );
}
