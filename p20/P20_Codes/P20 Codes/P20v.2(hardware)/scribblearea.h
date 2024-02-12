#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <vector>
#include "queue.h"
#include <QMutex>
#include <QThread>
#include "sendthread.h"
#include <QQueue>
#define arraySize 28

class ScribbleArea : public QWidget
{
    // Declares our class as a QObject which is the base class
    // for all Qt objects
    // QObjects handle events
    Q_OBJECT

public:
    ScribbleArea(QWidget *parent ,QQueue <bool> *_tempQueue);

   // Queue<std::array<char,25>>*dataqueue;
    QMutex *mutex;
    QQueue<bool> *tempQueue;

    // Handles all events
    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName, const char *fileFormat);
    void setPenColor(const QColor &newColor);
    void setPenWidth(int newWidth);


    //void serialisation(char commandType,QPoint pt=QPoint(0,0),QColor newPenColor=Qt::blue,int newPenWidth=1);

    void serialisation(char commandType,QPoint pt,QColor newPenColor, int newPenWidth);
    // Has the image been modified since last save
    bool isModified() const { return modified; }
    QColor penColor() const { return myPenColor; }
    int penWidth() const { return myPenWidth; }
    void pushToQueue(std::array<bool,28>);

private:
    //std::vector<std::vector<bool>>binarr;
    std::array<bool,arraySize> binarr;

private slots:
    void clearImage();
    void unclear();



protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    // Updates the scribble area where we are painting
    void paintEvent(QPaintEvent *event) override;

    // Makes sure the area we are drawing on remains
    // as large as the widget
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawLineTo(const QPoint &startPt,const QPoint &endPoint);
    void resizeImage(QImage *image, const QSize &newSize);
    void calculateParity();

    // Will be marked true or false depending on if
    // we have saved after a change
    bool modified;

    // Marked true or false depending on if the user
    // is drawing
    bool scribbling;

    // Holds the current pen width & color
    int myPenWidth;
    QColor myPenColor;

    // Stores the image being drawn
    QImage image;
    QStack <QImage> *history;

    // Stores the location at the current mouse event
    QPoint lastPoint;

signals:

    void sendSerialisedData(std::array<bool,28>);
    void sendClear();
    void sendPenColor(QColor);
    void sendPenWidth(int);
    void sendTest();
};

#endif
