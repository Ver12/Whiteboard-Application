#ifndef RECEIVEWINDOW_H
#define RECEIVEWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPoint>
#include <QPainter>
#include <QRect>
#include <QPaintEvent>
#include "receiverCanvas.h"
#include "vector"
#include <QMenu>
#include <QAction>

class receivewindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit receivewindow();
    QPoint lastPoint;
    QPoint endpoint;
    receiverCanvas *canvas;

private:
    void createMenus();
    void createActions();

    QMenu *saveAsMenu;
    QMenu *fileMenu;
    // Actions tied to specific file formats
    QList<QAction *> saveAsActs;
    QAction *exitAct;

    bool saveFile(const QByteArray &fileFormat);


public slots:
    void save();
   // void receiveDrawingSlot(QPoint p1,QPoint p2);
   // void receiveDrawingSlot(std::vector<std::vector<bool>>binarr);
   // void deserialisation(std::vector<std::vector<bool>>binarr);
   // void receivePenInfo(QColor newColor,int newPenWidth);
   // void receiveClear();


signals:
    void startReading();
   // void sendReceiveDrawing(QPoint,QPoint);
    //void setReceivePenInfo(QColor,int);
    //void setClear();

};

#endif // RECEIVEWINDOW_H
