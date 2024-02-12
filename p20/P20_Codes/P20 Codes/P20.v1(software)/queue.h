#ifndef QUEUE_H
#define QUEUE_H

#include <QMutex>
#include <QQueue>
#include <QDebug>

template <class T>
class Queue
{
public:
    bool *dataReady;
    Queue(QMutex *_mutex,bool *_dataReady){
        mutex=_mutex;
        dataReady=_dataReady;
    };

    void pushToQueue(T pushedData){
        // Lock the thread so queue can't be modified by other threads
        mutex->lock();

        //qDebug() << "Pushed to Queue";

        serialisedQueue.enqueue(pushedData);
        *dataReady=1; //data is pushed to queue and is ready to be read

        mutex->unlock();

    };

    char getTopElement(){
        //qDebug()<<serialisedQueue.front();
        return serialisedQueue.front();
    }

    void removeFromQueue(){
        mutex->lock();
        serialisedQueue.dequeue();

        if(isEmpty()){
            *dataReady=0; //if queue is empty then dataReady =0;
        }

        mutex->unlock();
    }

    bool isEmpty(){
        return serialisedQueue.isEmpty();
    }




private:
    QMutex *mutex;
    QQueue<T> serialisedQueue;

};


#endif // QUEUE_H
