#include "sendthread.h"
#include <QDebug>
#include <QThread>
#include <wiringPi.h>

#include<unistd.h>
#define outputPin 15
#define dataReadyPin 23
#define readDataPin 22
#include <QFuture>
#include <QtConcurrent/QtConcurrent>


sendthread::sendthread(Queue<char> *_dataqueue,bool *_dataReady, bool *_receiverReady, bool *_dataRead, QMutex *_mutex2)
{
    mutex2=_mutex2;
    dataqueue=_dataqueue;  //pointer to shared dataqueue
    dataReady=_dataReady;
    //receiverReady=_receiverReady;
    readData=_dataRead;
    pinMode(outputPin,OUTPUT);
    pinMode(dataReadyPin,OUTPUT);
    pinMode(readDataPin,INPUT);

    QFuture<void>future=QtConcurrent::run(this,&sendthread::transmitData);

    //qDebug()<<"send thread";

}


void sendthread::transmitData(){

    while(true){
        if(!tempQueue.isEmpty()){
            bool bits=tempQueue.front();
            tempQueue.dequeue();
            digitalWrite(outputPin,bits);
            qDebug()<<"sending data"<<bits;
            digitalWrite(dataReadyPin,1);
            qDebug()<<digitalRead(24);
            while(digitalRead(readDataPin)==1); //wait until readData become 1
        }
    }

/*
    //receiver is not reading data
        if(*dataRead==0){
            bool bits=tempQueue.dequeue();
            digitalWrite(outputPin,bits);

            mutex2->lock();
            *dataReady=1;
            mutex2->unlock();

            emit pushToQueueSignal();

            mutex2->lock();
            *dataRead=1; //receiver is busy reading data
            mutex2->unlock();

            tempQueue.dequeue();
           // qDebug()<<"sending "<<bits;
            //usleep(600);
        }*/

}

void sendthread::pushToQueue(std::array<bool,28> binarr){
    qDebug()<<"pushing";
    for(int i=0;i<28;i++){
        tempQueue.enqueue(binarr[i]);
    }

}

void sendthread ::sendData(){

        if(*readData==0){
            bool bits=tempQueue.front();
            digitalWrite(outputPin,bits);
            qDebug()<<"sending "<<bits;

            mutex2->lock();
            digitalWrite(dataReadyPin,1);
           // *dataReady=1;
            *readData=1; //receiver is busy reading data
            mutex2->unlock();

          // emit pushToQueueSignal();

            tempQueue.dequeue();
           // qDebug()<<"sending "<<bits;

        }
}

