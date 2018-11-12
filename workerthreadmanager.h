#ifndef WORKERTHREADMANAGER_H
#define WORKERTHREADMANAGER_H

#include "workerthread.h"

#define NUM_OF_WORKERTHREADS  4

class WorkerThreadManager:public QObject
{
    Q_OBJECT
public:
    WorkerThreadManager();
    ~WorkerThreadManager();

public slots:

    void processRPCs( QString cmd);

private:
    QMap<int, WorkerThread*> workerThreads;
    bool running;
};

#endif // WORKERTHREADMANAGER_H
