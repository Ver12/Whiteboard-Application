#include <QtWidgets>

#include "receiverCanvas.h"
#include "receivewindow.h"
#include "commandcode.h"
#include <array>
#include "sendthread.h"

receiverCanvas::receiverCanvas(QWidget *parent)
    : QWidget(parent)

{
    // Roots the widget to the top left even if resized
    setAttribute(Qt::WA_StaticContents);
    myPenWidth = 1;
    myPenColor=Qt::blue;
    history = new QStack<QImage>;

}

void receiverCanvas::receiveData(bool bits){
    qDebug()<<"receiving data";
    receiveBinVec.push_back(bits);
    if(receiveBinVec.size()==28){
        deserialisation(receiveBinVec);
        checkParity();
        receiveBinVec.clear();
    }
}

void receiverCanvas::deserialisation(QVector <bool> binarr){

   // qDebug()<<"deserialising";
    /*
    for(int i=0;i<binarr.size();i++){
        qDebug()<<binarr[i];
    }
    */
    int pt_x=0;
    int pt_y=0;

    QString bit1=binarr[0]==1?"1":"0"; //LSB
    QString bit2=binarr[1]==1?"1":"0";
    QString bit3=binarr[2]==1?"1":"0";

    QString decodedCommand=bit3+bit2+bit1;
    //qDebug()<<decodedCommand;
/*
    for(int i=0;i<21;i++){
         if(i==11){
             qDebug()<<"second num";
         }
        qDebug()<<binarr[i];

    }
    */

   if(decodedCommand=="000"){
      // qDebug()<<"entering start draw command";
       int j=0;
       for(int i=3;i<13;i++){
           pt_x+=binarr[i]*pow(2,j);
           //qDebug()<<"pt_x "<<pt_x;
           j++;
       }
      j=0;
       for(int i=13;i<23;i++){
           pt_y+=binarr[i]*pow(2,j);
           // qDebug()<<"pt_y "<<pt_y;
           j++;
       }
           lastPoint=QPoint(pt_x,pt_y);
         //  qDebug()<<pt_x;
          // qDebug()<<pt_y;
   }


   if(decodedCommand=="001"){
       //qDebug()<<"entering draw line command";
       int j=0;
       for(int i=3;i<13;i++){
           pt_x+=binarr[i]*pow(2,j);
          // qDebug()<<"pt_x "<<pt_x;
           j++;
       }

       j=0;
       for(int i=13;i<23;i++){
           pt_y+=binarr[i]*pow(2,j);
          // qDebug()<<"pt_y "<<pt_y;
           j++;
       }
        QPoint endPoint = QPoint(pt_x,pt_y);
        drawLineTo(endPoint);
       // qDebug()<<pt_x;
       // qDebug()<<pt_y;
   }


   if(decodedCommand=="010"){
      // qDebug()<<"Entering pen width command";
       int newPenWidth=0;
       int j=0;
       for(int i=3;i<9;i++){
           newPenWidth+=binarr[i]*pow(2,j);
          // qDebug()<<"pen width is "<<newPenWidth;
           j++;
       }
       setPenWidth(newPenWidth);
   }

   if(decodedCommand=="011"){
      // qDebug()<<"Entering pen colour command";
       int r=0;
       int g=0;
       int b=0;

       int j=0;
       for(int i=3;i<11;i++){
            r+=binarr[i]*pow(2,j);
            j++;
       }

       j=0;
       for(int i=11;i<19;i++){
           g+=binarr[i]*pow(2,j);
           j++;
       }

       j=0;
       for(int i=19;i<27;i++){
           b+=binarr[i]*pow(2,j);
           j++;
       }
        //qDebug()<<"r value is "<<r;
        //qDebug()<<"g value is "<<g;
        //qDebug()<<"b value is "<<b;
       setPenColor(QColor(r,g,b));
   }

   if(decodedCommand=="100"){
       clearImage();
   }

   if(decodedCommand=="101"){
      // qDebug()<<"unclear command";
       unclear();
   }
}

// QPainter provides functions to draw on the widget
// The QPaintEvent is sent to widgets that need to
// update themselves
void receiverCanvas::paintEvent(QPaintEvent *event)
{
    QPainter painter2(this);
    // Returns the rectangle that needs to be updated
    QRect dirtyRect = event->rect();
    // Draws the rectangle where the image needs to
    // be updated
    painter2.drawImage(dirtyRect, image, dirtyRect);
}

// Resize the image to slightly larger then the main window
// to cut down on the need to resize the image
void receiverCanvas::resizeEvent(QResizeEvent *event)
{
    if (width() > image.width() || height() > image.height()) {
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizeImage(&image, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}

void receiverCanvas::drawLineTo(const QPoint &endPoint)
{
    // Used to draw on the widget
    QPainter painter(&image);

    // Set the current settings for the pen
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));

    // Draw a line from the last registered point to the current
    painter.drawLine(lastPoint, endPoint);

    // Set that the image hasn't been saved
    modified = true;

    int rad = (myPenWidth / 2) + 2;
    // Call to update the rectangular space where we drew
    update(QRect(lastPoint, endPoint).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));
    //emit sendDrawingPos(lastPoint,endPoint);
    // Update the last position where we left off drawing
    lastPoint = endPoint;
}


// When the app is resized create a new image using
// the changes made to the image
void receiverCanvas::resizeImage(QImage *image, const QSize &newSize)
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

// Used to change the pen color
void receiverCanvas::setPenColor(const QColor &newColor)
{
    myPenColor = newColor;
   // qDebug()<<"Pen Color is "<<newColor;
}

// Used to change the pen width
void receiverCanvas::setPenWidth(int newWidth)
{
    myPenWidth = newWidth;
   // qDebug()<<"Pen width is "<<newWidth;
}

void receiverCanvas::clearImage()
{
    history->push(image.copy());
    image.fill(qRgb(255, 255, 255));
    modified = true;
    update();
}

void receiverCanvas::setPen(QColor newPenColor,int newPenWidth){
   myPenColor=newPenColor;
   myPenWidth=newPenWidth;
  // qDebug()<<myPenColor;
  // qDebug()<<myPenWidth;
}


// Save the current image
bool receiverCanvas::saveImage(const QString &fileName, const char *fileFormat)
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

void receiverCanvas::unclear()
{
   //check history count
    if (history->count() > 0)
    {
        image = history->top();
        update();

        history->pop();
    }
}


void receiverCanvas::checkParity(){

    int numberOfOnes=0;

    for(int i=0;i<receiveBinVec.size()-1;i++){
        if(receiveBinVec[i]==1){
            numberOfOnes+=1;
        }
    }
    bool parity = numberOfOnes % 2 == 0 ? 0 :1;
    qDebug()<<"parity bit calculated in receiver is "<<parity;

    if(receiveBinVec.back()==parity){
        qDebug()<<"No parity error";
    }
    else{
        qDebug()<<"There is error in data transmission";
    }

}








