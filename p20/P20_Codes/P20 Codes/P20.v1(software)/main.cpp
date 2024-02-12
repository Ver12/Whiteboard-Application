#include "mainwindow.h"
#include "receivewindow.h"
#include "queue.h"

#include <QApplication>
#include <iostream>
#include <QMutex>
#include <QThread>
#include "sendthread.h"
#include "receivethread.h"
#include "scribblearea.h"
#include <wiringPi.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<std::array<bool,28>>("std::array<bool,28>");
    wiringPiSetup();
    QMutex mutex; //share mutex
    QMutex mutex2;

    //toggle boolean variables for communication
    bool dataReady=0;
    bool receiverReady=1;
    bool dataRead=0;

    Queue<char> dataqueue(&mutex,&dataReady); //share dataqueue

    //setup send and receive window
    MainWindow sender;
    receivewindow receiver;
    sender.setWindowTitle("Send Window");
    receiver.setWindowTitle("Receive Window");

    //worker threads
    sendthread *send=new sendthread(&dataqueue,&dataReady,&receiverReady,&dataRead,&mutex2); //pass queue into send thread
    receivethread *receive = new receivethread(&dataqueue,&dataReady,&receiverReady,&dataRead,&mutex2);

    QThread *sendThread=new QThread;
    QThread *receiveThread=new QThread;

   // move to threads
    send->moveToThread(sendThread);
    QObject::connect(sendThread, &QThread::finished,send, &QObject::deleteLater);
    if(send->thread() == sendThread){
        qDebug()<<"success";
    }

    receive->moveToThread(receiveThread);
    QObject::connect(receiveThread, &QThread::finished,receive, &QObject::deleteLater);

    sendThread->start();
    receiveThread->start();

    if( sendThread->isRunning() ) {
        qDebug() << "Thread a is running" ;
      }
      if( receiveThread->isRunning() ) {
        qDebug() << "Thread b is running";
      }

   // QObject::connect(receiver.canvas,SIGNAL(startReading()),receive,SLOT(pullFromQueue()));
   // QObject::connect(receiver.canvas,SIGNAL(startReading()),receiver.canvas,SLOT(testData()));
    QObject::connect(sender.scribbleArea,SIGNAL(sendSerialisedData(std::array<bool,28>)),send,SLOT(pushToQueue(std::array<bool,28>)));

    QObject::connect(send,SIGNAL(pushToQueueSignal()),receive,SLOT(pullFromQueue()));
    QObject::connect(receive,SIGNAL(sendReceiveData(bool)),receiver.canvas,SLOT(receiveData(bool)));

    sender.show();
    receiver.show();
    return a.exec();
   // sendThread.quit();
    //receiveThread.quit();

}
