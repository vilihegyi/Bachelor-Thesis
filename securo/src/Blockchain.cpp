#include <QLoggingCategory>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "Blockchain.h"

static QLoggingCategory logBlockchain("Blockchain", QtCriticalMsg);

static const auto idKey = QStringLiteral("id");
static const auto resultKey = QStringLiteral("result");
static const auto statusKey = QStringLiteral("status");
static const auto failureKey = QStringLiteral("failure");
static const auto successKey = QStringLiteral("success");

/**
    Create a connection to the database, create the
    Genesis block of the blockchain and set the
    difficulty of calculation to 4 (zeroes to get).
*/

Blockchain::Blockchain() : QThread(nullptr)
{
    mDatabase = new Database();
    mBlock = new Block();
    mChain.emplace_back(Block("Genesis Block"));
    mDifficulty = 4;
}

/**
    Returns the index of the last block.

    @param  newBlock The new block to be added
            to the blockchain.
*/

void Blockchain::addBlock(Block newBlock)
{
    newBlock.prevHash = getLastBlock().getHash();
    newBlock.mineBlock(mDifficulty);
    mChain.push_back(newBlock);

    QString hash = QString::fromStdString(newBlock.getHash());
    QVariantMap map;

    map.insert(idKey, mBlock->getLastIndex());
    QSqlQuery insertQuery;
    if (mDatabase->checkForPreviousVoting(mDataList[0])) {
        map.insert(statusKey, failureKey);
        map.insert(resultKey, "You cannot vote twice!");
        qCCritical(logBlockchain) << "Error: User has already voted, with the CNP: " << mDataList[0];
    } else if (!insertQuery.exec("INSERT INTO blockchainVoting.votes (CNP, Name, Address, EmailAddress, VoteOption, Hash)"
                         "VALUES ('" + mDataList[0] + "', '" + mDataList[1] + "', '" + mDataList[2] + "', '" + mDataList[3] + "', '" + mDataList[4] + "', '" + hash + "')")) {
            map.insert(statusKey, failureKey);
            map.insert(resultKey, insertQuery.lastError().text());
            qCCritical(logBlockchain) << "Error: " << insertQuery.lastError();
    } else {
        map.insert(statusKey, successKey);
        map.insert(resultKey, hash);
    }

    emit miningCompleted(map);
}

/**
    Get the last block of the blockchain.

    @return Returns last block of the blockchain.
*/

Block Blockchain::getLastBlock() const
{
    return mChain.back();
}

/**
    Add a string of data to the block from the blockchain 
    which represents the voters details and chosen option.

    @param  data Data to be added to the blockchain.
*/

void Blockchain::add(const QString &data)
{
    mDataList = data.split("+");
    mWait.wakeOne();
    QMutexLocker locker(&mLock);
    mQueue.enqueue(data);
}

/**
    Keep running the blockchain's blocks on a separate threads
    to keep the circularity of the blockchain. This helps each
    voter cast their vote after the previous one is done and not
    only the one with the most calculating power will have their
    vote validated and added to the blockchain.
*/

void Blockchain::run()
{
    mRunning = true;

    // Handling the circular hashing
    while (mRunning) {
        QMutexLocker locker(&mLock);
        if (mQueue.isEmpty()) {
            qCDebug(logBlockchain) << "Is empty!";
            mWait.wait(locker.mutex());
        } else {
            addBlock(Block(mQueue.dequeue().toUtf8().constData()));
        }
    }
}
