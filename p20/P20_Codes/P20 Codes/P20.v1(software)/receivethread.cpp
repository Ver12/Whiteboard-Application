#include "receivethread.h"
#include <wiringPi.h>

#define inputPin 16

receivethread::receivethread(Queue<char> *_dataqueue,bool *_dataReady, bool *_receiverReady, bool *_dataRead, QMutex *_mutex2)
{
    dataqueue=_dataqueue;  //pointer to shared dataqueue
    dataReady=_dataReady;
    receiverReady=_receiverReady;
    readData=_dataRead;
    mutex2=_mutex2;
    pinMode(inputPin,INPUT);//27
    //qDebug()<<"Receive thread";
}

void receivethread::pullFromQueue(){

   // qDebug()<<"reading pins";
    //if data is ready to be read

       // qDebug()<<"reading pins";
        if(*dataReady==1){
            //qDebug()<<"pull from queue";
            //bool bits=dataqueue->getTopElement(); //digitalRead
            //qDebug()<<"Pull from queue "<<bits;
           // dataqueue->removeFromQueue(); //remove first element

            mutex2->lock();
            bool bits=digitalRead(inputPin);
           // qDebug()<<"receiving "<<bits;
            emit sendReceiveData(bits);

            *readData=0; //receiver finish reading data
            *dataReady=0;
            mutex2->unlock();

        }

}

