#include "receivewindow.h"
#include <QDebug>
#include <QtWidgets>


receivewindow::receivewindow()

{

    canvas=new receiverCanvas(this);
    setCentralWidget(canvas);

    setFixedSize(1000,1000);
    createActions();
    createMenus();


}

// Create the menubar
void receivewindow::createMenus()
{
    // Create Save As option and the list of file types
    saveAsMenu = new QMenu(tr("&Save As"), this);
    foreach (QAction *action, saveAsActs)
        saveAsMenu->addAction(action);

    // Attach all actions to File
    fileMenu = new QMenu(tr("&File"), this);

    fileMenu->addMenu(saveAsMenu);
    //fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    // Add menu items to the menubar
    menuBar()->addMenu(fileMenu);
}

// Define menu actions that call functions
void receivewindow::createActions()
{

    // Get a list of the supported file formats
    // QImageWriter is used to write images to files
    foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
        QString text = tr("%1...").arg(QString(format).toUpper());

        // Create an action for each file format
        QAction *action = new QAction(text, this);

        // Set an action for each file format
        action->setData(format);

        // When clicked call MainWindow::save()
        connect(action, SIGNAL(triggered()), this, SLOT(save()));

        // Attach each file format option menu item to Save As
        saveAsActs.append(action);
    }
/*
    // Create print action and tie to MainWindow::print()
    printAct = new QAction(tr("&Print..."), this);
    connect(printAct, SIGNAL(triggered()), scribbleArea, SLOT(print()));
*/
    // Create exit action and tie to MainWindow::close()
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

// Called when the user clicks Save As in the menu
void receivewindow::save()
{
    // A QAction represents the action of the user clicking
    QAction *action = qobject_cast<QAction *>(sender());

    // Stores the array of bytes of the users data
    QByteArray fileFormat = action->data().toByteArray();

    // Pass it to be saved
    saveFile(fileFormat);
}

bool receivewindow::saveFile(const QByteArray &fileFormat)
{
    // Define path, name and default file type
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;

    // Get selected file from dialog
    // Add the proper file formats and extensions
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                               initialPath,
                               tr("%1 Files (*.%2);;All Files (*)")
                               .arg(QString::fromLatin1(fileFormat.toUpper()))
                               .arg(QString::fromLatin1(fileFormat)));

    // If no file do nothing
    if (fileName.isEmpty()) {
        return false;
    } else {

        // Call for the file to be saved
        return canvas->saveImage(fileName, fileFormat.constData());
    }
}
/*
void receivewindow::deserialisation(std::vector<std::vector<bool>>binarr){
    qDebug()<<"deserialising";
    int point1_x=0;
    int point1_y=0;
    int point2_x=0;
    int point2_y=0;

    std::vector<bool> pt1_x,pt1_y,pt2_x,pt2_y;
    pt1_x=binarr[0];
    pt1_y=binarr[1];
    pt2_x=binarr[2];
    pt2_y=binarr[3];

    for(int i=0;i<pt1_x.size();i++){
        point1_x+=pt1_x[i]*pow(2,i);
    }

    for(int i=0;i<pt1_y.size();i++){
        point1_y+=pt1_y[i]*pow(2,i);
    }
    for(int i=0;i<pt2_x.size();i++){
        point2_x+=pt2_x[i]*pow(2,i);
    }
    for(int i=0;i<pt2_y.size();i++){
        point2_y+=pt2_y[i]*pow(2,i);
    }
    qDebug()<<"Point 1 is "<<point1_x<<" "<<point1_y;
    qDebug()<<"Point 2 is "<<point2_x<<" "<<point2_y;
    emit sendReceiveDrawing(QPoint(point1_x,point1_y),QPoint(point2_x,point2_y));

}

void receivewindow::receiveDrawingSlot(QPoint p1,QPoint p2){
   canvas->drawLineTo(p1,p2);
}


void receivewindow::receivePenInfo(QColor newColor,int newPenWidth){
    canvas->setPenColor(newColor);
    canvas->setPenWidth(newPenWidth);
}

void receivewindow::receiveClear(){
   canvas->clearImage();
}
*/



