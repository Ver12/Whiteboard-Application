#ifndef RECEIVETHREAD_H
#define RECEIVETHREAD_H
#include "queue.h"
#include <array>

class receivethread :public QObject
{
    Q_OBJECT
public:
    receivethread(Queue<char> *_dataqueue,bool *_dataReady, bool *_receiverReady, bool *_dataRead,QMutex *_mutex2);
     void pullFromQueue();

private:
    QMutex *mutex2;
    Queue<char> *dataqueue;

    bool *dataReady;
    bool *receiverReady;
    bool *readData;





/*
private slots:
    //take in serialised data and push it into queue
    void pullFromQueue();
*/
signals:
    void sendReceiveData(bool);
};

#endif // RECEIVETHREAD_H
