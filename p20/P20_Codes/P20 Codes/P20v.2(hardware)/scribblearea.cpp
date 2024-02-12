#include <QtWidgets>

#include "scribblearea.h"
#include "mainwindow.h"
#include <iostream>
#include "commandcode.h"
#include "sendthread.h"
#include <QThread>


ScribbleArea::ScribbleArea(QWidget *parent,QQueue<bool> *_tempQueue)
    : QWidget(parent)
{
    tempQueue=_tempQueue;
    // Set defaults for the monitored variables
    modified = false;
    scribbling = false;
    myPenWidth = 1;

    // Roots the widget to the top left even if resized
    setAttribute(Qt::WA_StaticContents);
    history= new QStack <QImage>;

    myPenColor = Qt::blue;

}


void ScribbleArea::serialisation(char commandType,QPoint pt,QColor newPenColor, int newPenWidth){

    std::fill(std::begin(binarr), std::end(binarr), 0);

    switch(commandType){

    case 's':
           binarr[0]=0;
           binarr[1]=0;
           binarr[2]=0;
       break;

       case 'd':
           binarr[0]=1;
           binarr[1]=0;
           binarr[2]=0;
       break;

    case 'w':
        binarr[0]=0;
        binarr[1]=1;
        binarr[2]=0;
        break;

    case 'p':
        binarr[0]=1;
        binarr[1]=1;
        binarr[2]=0;
        break;

    case 'c':
        binarr[0]=0;
        binarr[1]=0;
        binarr[2]=1;
        break;

    case 'u':
       binarr[0]=1;
       binarr[1]=0;
       binarr[2]=1;
       break;

    default:
        qDebug()<<"No matching";
        break;
    }

   // binarr[0]=commandType;

    if((commandType==startDrawCommand) | (commandType==drawLineCommand)){

        int pt_x=pt.x();
        int pt_y=pt.y();
        //if((pt_x <=1000 && pt_x >=0) && (pt_y <=1000 && pt_y >=0)){
        if((pt_x <=1000  ) && (pt_y <=1000 )){
            for(int i=3;i<13;i++){
                   binarr[i]=pt_x%2;
                   //qDebug()<<binarr[i];
                   pt_x/=2;
               }
          //  qDebug()<<"first num";

            for(int i=13;i<23;i++){
                   binarr[i]=pt_y%2;
                   //qDebug()<<binarr[i];
                   pt_y/=2;
               }
            //dataqueue->pushToQueue(binarr);
           // emit sendTest();
            calculateParity();
           // emit sendSerialisedData(binarr);
           pushToQueue(binarr);

        }
    }

    if(commandType==penWidthCommand){
        for(int i=3;i<9;i++){ //6
            binarr[i]=newPenWidth%2;
            newPenWidth/=2;
        }
        calculateParity();
        //emit sendSerialisedData(binarr);
        pushToQueue(binarr);
    }

    if(commandType==clearCommand){
        calculateParity();
        //emit sendSerialisedData(binarr);
       pushToQueue(binarr);
    }

    if(commandType==penColorCommand){
        int r=newPenColor.red();
        int g=newPenColor.green();
        int b=newPenColor.blue();

        for(int i=3;i<11;i++){ //8
            binarr[i]=r%2;
            r/=2;
        }

        for(int i=11;i<19;i++){
            binarr[i]=g%2;
            g/=2;
        }

        for(int i=19;i<27;i++){
            binarr[i]=b%2;
            b/=2;
        }
        calculateParity();
       // emit sendSerialisedData(binarr);
       pushToQueue(binarr);

    }

    if(commandType==unclearCommand){
        //emit sendSerialisedData(binarr);
        pushToQueue(binarr);
    }



}


// Used to load the image and place it in the widget
bool ScribbleArea::openImage(const QString &fileName)
{
    // Holds the image
    QImage loadedImage;

    // If the image wasn't loaded leave this function
    if (!loadedImage.load(fileName))
        return false;

    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    image = loadedImage;
    modified = false;
    update();
    return true;
}

// Save the current image
bool ScribbleArea::saveImage(const QString &fileName, const char *fileFormat)
{
    // Created to hold the image
    QImage visibleImage = image;
    resizeImage(&visibleImage, size());

    if (visibleImage.save(fileName, fileFormat)) {
        modified = false;
        return true;
    } else {
        return false;
    }
}

// Used to change the pen color
void ScribbleArea::setPenColor(const QColor &newColor)
{
    myPenColor = newColor;
    //emit sendPenColor(myPenColor);
    serialisation(penColorCommand,QPoint(0,0),myPenColor,1);
}

// Used to change the pen width
void ScribbleArea::setPenWidth(int newWidth)
{
    myPenWidth = newWidth;
    //emit sendPenWidth(myPenWidth);
    serialisation(penWidthCommand,QPoint(0,0),Qt::blue,newWidth);

}

// Color the image area with white
void ScribbleArea::clearImage()
{
    history->push(image.copy());
    image.fill(qRgb(255, 255, 255));
    modified = true;
    update();
    serialisation(clearCommand,QPoint(0,0),Qt::blue,1);
   // emit sendClear();
}

// If a mouse button is pressed check if it was the
// left button and if so store the current position
// Set that we are currently drawing
void ScribbleArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        serialisation(startDrawCommand,lastPoint,Qt::blue,1);
        scribbling = true;
    }

}

// When the mouse moves if the left button is clicked
// we call the drawline function which draws a line
// from the last position to the current
void ScribbleArea::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && scribbling){

       // qDebug()<<"Inside mouse event "<<event->pos();
        serialisation(drawLineCommand,event->pos(),Qt::blue,1);
        drawLineTo(lastPoint,event->pos());

    }

}

// If the button is released we set variables to stop drawing
void ScribbleArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling) {
       // qDebug()<<"Inside mouse release event "<<lastPoint;
       // qDebug()<<"Inside mouse release event "<<event->pos();
        serialisation(drawLineCommand,event->pos(),Qt::blue,1);
        drawLineTo(lastPoint,event->pos());
        scribbling = false;
    }
}

// QPainter provides functions to draw on the widget
// The QPaintEvent is sent to widgets that need to
// update themselves
void ScribbleArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    // Returns the rectangle that needs to be updated
    QRect dirtyRect = event->rect();
    // Draws the rectangle where the image needs to
    // be updated
    painter.drawImage(dirtyRect, image, dirtyRect);
}

// Resize the image to slightly larger then the main window
// to cut down on the need to resize the image
void ScribbleArea::resizeEvent(QResizeEvent *event)
{
    if (width() > image.width() || height() > image.height()) {
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizeImage(&image, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}

void ScribbleArea::drawLineTo(const QPoint &startPt,const QPoint &endPoint)
{
    // Used to draw on the widget
    QPainter painter(&image);

    // Set the current settings for the pen
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));

    // Draw a line from the last registered point to the current
    painter.drawLine(startPt, endPoint);

    // Set that the image hasn't been saved
    modified = true;

    int rad = (myPenWidth / 2) + 2;
    // Call to update the rectangular space where we drew
    update(QRect(startPt, endPoint).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));

   // emit sendDrawingPos(lastPoint,endPoint);
    // Update the last position where we left off drawing
    lastPoint = endPoint;
}

// When the app is resized create a new image using
// the changes made to the image
void ScribbleArea::resizeImage(QImage *image, const QSize &newSize)
{
    // Check if we need to redraw the image
    if (image->size() == newSize)
        return;

    // Create a new image to display and fill it with white
    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));

    // Draw the image
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

void ScribbleArea::unclear()
{

   //check history count
    if (history->count() > 0)
    {
       // clearImage();
        image = history->top();
        update();

        history->pop();
        serialisation(unclearCommand,QPoint(0,0),Qt::blue,1);
    }
}


void ScribbleArea::calculateParity(){

    int numberOfOnes=0;
    for(int i=0;i<27;i++){
        if(binarr[i]==1){
            numberOfOnes+=1;
        }
    }

    bool parityBit=numberOfOnes % 2 == 0 ? 0 :1;
    binarr.back()=parityBit; //set last bit to parity bit
   // qDebug()<<"parity bit send is "<< parityBit;
}

void ScribbleArea::pushToQueue(std::array<bool,28>binarr){

    for(int i=0;i<28;i++){
        tempQueue->enqueue(binarr[i]);

    }

}

