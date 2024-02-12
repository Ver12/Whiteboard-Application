#include "sendthread.h"
#include <QDebug>
#include <QThread>
#include <wiringPi.h>

#include<unistd.h>
#define outputPin 15


sendthread::sendthread(Queue<char> *_dataqueue,bool *_dataReady, bool *_receiverReady, bool *_dataRead, QMutex *_mutex2)
{
    mutex2=_mutex2;
    dataqueue=_dataqueue;  //pointer to shared dataqueue
    dataReady=_dataReady;
    //receiverReady=_receiverReady;
    readData=_dataRead;
    pinMode(outputPin,OUTPUT);

    //qDebug()<<"send thread";

}


void sendthread::pushToQueue(std::array<bool,28> binarr){

    for(int i=0;i<28;i++){
        tempQueue.enqueue(binarr[i]);
    }
    sendData();

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
    while(tempQueue.size() != 0){
        sendData() ;
    }
}

void sendthread ::sendData(){
    //receiver is not reading data
        if(*readData==0){
            bool bits=tempQueue.front();
            digitalWrite(outputPin,bits);

            mutex2->lock();
            *dataReady=1;
            *readData=1; //receiver is busy reading data
            mutex2->unlock();

           emit pushToQueueSignal();

            tempQueue.dequeue();
           // qDebug()<<"sending "<<bits;
            //usleep(600);
        }
}

