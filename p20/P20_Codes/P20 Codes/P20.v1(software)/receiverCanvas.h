#ifndef RECEIVERCANVAS_H
#define RECEIVERCANVAS_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QPainter>
#include <QPaintDevice>
#include <QThread>
#include "receivethread.h"

#include <QVector>

class receiverCanvas : public QWidget
{
    // Declares our class as a QObject which is the base class
    // for all Qt objects
    // QObjects handle events
    Q_OBJECT

public:
    receiverCanvas(QWidget *parent = 0);

    void setPenColor(const QColor &newColor);
    void setPenWidth(int newWidth);
    void clearImage();
    void drawLineTo(const QPoint &endPoint);
    void deserialisation(QVector <bool> binarr);
    bool saveImage(const QString &fileName, const char *fileFormat);
    void checkParity();
     void unclear();


protected:

    // Updates the scribble area where we are painting
    void paintEvent(QPaintEvent *event) override;

    // Makes sure the area we are drawing on remains
    // as large as the widget
    void resizeEvent(QResizeEvent *event) override;


private:
    //void drawLineTo(const QPoint &endPoint);
    void resizeImage(QImage *image, const QSize &newSize);


    // Will be marked true or false depending on if
    // we have saved after a change
    bool modified;

    // Marked true or false depending on if the user
    // is drawing
    bool scribbling;

    // Holds the current pen width & color
    int myPenWidth;
    QColor myPenColor;

    QPoint lastPoint;

    // Stores the image being drawn
    QImage image=QImage(1000,1000,QImage::Format_ARGB32); //to solve qpainter not active issue

    QThread *receiveThread;
    receivethread *worker;
    QVector<bool>receiveBinVec;
    QStack <QImage> *history;


    //std::array<char,25> receiveBinArr;

private slots:
    void setPen(QColor newPenColor,int newPenWidth);
    void receiveData(bool bits);
signals:
    void startReading();

    //void receiveDrawing(QPoint pt1,QPoint pt2);
    //void drawLineTo(const QPoint &startPt,const QPoint &endPoint);
    //void setReceivePenInfo(QColor newColor,int newPenWidth);
};


#endif // RECEIVERCANVAS_H
