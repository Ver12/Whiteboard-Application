#include "receivethread.h"
#include <wiringPi.h>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

#define inputPin 16
#define dataReadyPin 24
#define readDataPin 27

receivethread::receivethread(Queue<char> *_dataqueue,bool *_dataReady, bool *_receiverReady, bool *_dataRead, QMutex *_mutex2)
{
    dataqueue=_dataqueue;  //pointer to shared dataqueue
    dataReady=_dataReady;
    receiverReady=_receiverReady;
    readData=_dataRead;
    mutex2=_mutex2;
    pinMode(inputPin,INPUT);
    pinMode(dataReadyPin,INPUT);
    pinMode(readDataPin,OUTPUT);
    digitalWrite(readDataPin,0);

    //qDebug()<<"Receive thread";
   QFuture<void>future=QtConcurrent::run(this,&receivethread::pullFromQueue);

}

void receivethread::pullFromQueue(){

while(true){
     qDebug()<<"reading pins";

     while( digitalRead(dataReadyPin)==0); //wait for dataReady

     bool bits=digitalRead(inputPin);
     qDebug()<<"receiving "<<bits;
     emit sendReceiveData(bits);

     //*readData=0; //receiver finish reading data //digitalWrite(readDataPin,0);
     digitalWrite(readDataPin,0);
}



            //qDebug()<<"pull from queue";
            //bool bits=dataqueue->getTopElement(); //digitalRead
            //qDebug()<<"Pull from queue "<<bits;
           // dataqueue->removeFromQueue(); //remove first element


}


