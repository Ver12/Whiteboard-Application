#ifndef SENDTHREAD_H
#define SENDTHREAD_H
#include "queue.h"
#include <QObject>
#include <QQueue>
#define arraySize 28


class sendthread:public QObject
{
      Q_OBJECT
public:
    sendthread(Queue<char> *_dataqueue, bool *_dataReady, bool *_receiverReady, bool *_dataRead,QMutex *_mutex2);

private:
    QMutex *mutex2;
    Queue<char> *dataqueue;
    bool *dataReady;
    bool *receiverReady;
    bool *readData;
    unsigned int microsecond = 1000;
    QQueue <bool>tempQueue;
    void sendData();

public slots:
    //take in serialised data and push it into queue
    void pushToQueue(std::array<bool,28> binarr);

signals:
    void pushToQueueSignal();

};

#endif // SENDTHREAD_H
