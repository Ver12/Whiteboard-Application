#include "mainwindow.h"
#include "receivewindow.h"
#include "queue.h"

#include <QApplication>
#include <iostream>
#include <QMutex>
#include <QThread>

#include "scribblearea.h"
#include <wiringPi.h>
#include <pthread.h>
#include <unistd.h>
#define outputPin 15
#define inputPin 16
/*
#define dataReadyOutPin 23
#define readDataInPin 26
#define dataReadyInPin 22
#define readDataOutPin 27
*/
#define clockPin 0
#define clockInputPin1 2
#define clockInputPin2 3

QQueue<bool>tempQueue;
// Getting the mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

MainWindow *sender;
receivewindow *receiver;
bool prevclk=1;


void *sendThread(void *){

    while(true){
        //qDebug()<<"send thread";
        //qDebug()<<tempQueue.front();
        if(digitalRead(clockInputPin1)==0 && prevclk==1){

        if(tempQueue.size()>0){
           pthread_mutex_lock(&mutex);
           // qDebug()<<"From send thread "<<tempQueue.front();
            bool bits=tempQueue.front();
            tempQueue.dequeue();
            // Get the mutex unlocked
            pthread_mutex_unlock(&mutex);
            digitalWrite(outputPin,bits);
            qDebug()<<"From send thread "<<bits;
            prevclk=0;
        }
           // qDebug()<<"Ok";

        }
    }

/*
            digitalWrite(outputPin,bits);
            digitalWrite(dataReadyOutPin,1);
            qDebug()<<"from send";
            while(!digitalRead(readDataInPin));
            qDebug()<<"from send2";
            digitalWrite(dataReadyOutPin,0);
            while(digitalRead(readDataInPin));
            qDebug()<<"From send3"; */
            /*
            usleep(300);
            //digitalWrite(dataReadyOutPin,0);
            digitalWrite(outputPin,bits);
           qDebug()<<"from send "<<bits;
            digitalWrite(dataReadyOutPin,1);
             //qDebug()<<"from send2";

            while(digitalRead(readDataInPin)==0);
            digitalWrite(dataReadyOutPin,0);
            */
    pthread_exit(NULL);
}

void *receiveThread(void *){

    while(true){
       // qDebug()<<"receive thread";
/*
        while(digitalRead(dataReadyInPin));
        digitalWrite(readDataOutPin,0);
        while(!digitalRead(dataReadyInPin));
        bool bits=digitalRead(inputPin);
        receiver->canvas->receiveData(bits);

       digitalWrite(readDataOutPin,1); */
      // digitalWrite(readDataOutPin,0);
/*
       while(digitalRead(dataReadyInPin)==0);

       bool bits=digitalRead(inputPin);
      // qDebug()<<"receive thread";
       receiver->canvas->receiveData(bits);
       digitalWrite(readDataOutPin,1);
       usleep(200);
       digitalWrite(readDataOutPin,0);*/

       if(digitalRead(clockInputPin2)==1 &&prevclk==0){
           bool bits=digitalRead(inputPin);
           qDebug()<<"receive thread";
           receiver->canvas->receiveData(bits);
           prevclk=1;
          // digitalWrite(readDataOutPin,1);
          // usleep(200);
          // digitalWrite(readDataOutPin,0);
       }

    }
    pthread_exit(NULL);
}

void *clocktick(void *){
    while(true){
        digitalWrite(clockPin,0);
        usleep(35);
        digitalWrite(clockPin,1);
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<std::array<bool,28>>("std::array<bool,28>");
  qDebug()<<"setting up";
    wiringPiSetup();
    pinMode(outputPin,OUTPUT);
    pinMode(inputPin,INPUT);
    /*
    pinMode(readDataInPin,INPUT);
    pinMode(readDataOutPin,OUTPUT);
    pinMode(dataReadyInPin,INPUT);
    pinMode(dataReadyOutPin,OUTPUT);
    
    pullUpDnControl(readDataInPin, PUD_DOWN); //0
    pullUpDnControl(dataReadyInPin, PUD_DOWN); //0
    */
    pullUpDnControl(inputPin, PUD_DOWN); //0
    pullUpDnControl(clockInputPin1, PUD_DOWN);
    pullUpDnControl(clockInputPin2, PUD_DOWN);

    //clock
    pinMode(clockPin,OUTPUT);
    pinMode(clockInputPin1,INPUT);
    pinMode(clockInputPin2,INPUT);



    //setup send and receive window
  //  MainWindow sender;
   // receivewindow receiver;

    sender=new MainWindow(&tempQueue);
    receiver=new receivewindow;
    sender->setWindowTitle("Send Window");
    receiver->setWindowTitle("Receive Window");


   // QObject::connect(sender->scribbleArea,SIGNAL(sendSerialisedData(std::array<bool,28>)),sender->scribbleArea,SLOT(pushToQueue(std::array<bool,28>)));

    sender->show();
    receiver->show();
    /*

    MainWindow sender;
    receivewindow receiver;
    sender.setWindowTitle("Send Window");
    receiver.setWindowTitle("Receive Window");

    sender.scribbleArea->tempQueue=&tempQueue;
    QObject::connect(sender.scribbleArea,SIGNAL(sendSerialisedData(std::array<bool,28>)),sender.scribbleArea,SLOT(pushToQueue(std::array<bool,28>)));

    sender.show();
    receiver.show();
    */
    pthread_t send;
    pthread_t receive;
    pthread_t clock;


    int fail=pthread_create(&send,NULL,sendThread,NULL);
    if(fail){
        qDebug()<<" fail to create send thread";
        exit(1);
    }
    else{
        qDebug()<<"create send thread";
    }

    int fail2=pthread_create(&receive,NULL,receiveThread,NULL);
    if(fail2){
        qDebug()<<" fail to create receive thread";
        exit(1);
    }
    else{
        qDebug()<<"create receive thread";
    }

    int fail3=pthread_create(&clock,NULL,clocktick,NULL);
    if(fail3){
        qDebug()<<" fail to create clock thread";
        exit(1);
    }
    else{
        qDebug()<<"create clock thread";
    }



    return a.exec();

}
