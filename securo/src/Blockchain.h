#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>

#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QQueue>
#include <QThread>
#include <QWaitCondition>

#include "Block.h"
#include "Database.h"

class Blockchain : public QThread
{
    Q_OBJECT

public:
    Blockchain();

    void addBlock(Block);

    void add(const QString &);

    void stop() {
        mRunning = false;
    }

signals:
    void miningCompleted(const QVariantMap &);

protected:
    void run() override;

private:
    int mDifficulty;
    vector<Block> mChain;
    Block *mBlock;
    Database *mDatabase;
    QStringList mDataList;

    Block getLastBlock() const;

    bool mRunning;
    QQueue<QString> mQueue;
    QWaitCondition mWait;
    QMutex mLock;
};


#endif // BLOCKCHAIN_H
