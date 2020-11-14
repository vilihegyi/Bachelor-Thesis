#include <sstream>

#include <QDebug>
#include <QFile>
#include <QLoggingCategory>

#include "Block.h"
#include "SHA256.h"

static int mIndex;
static QLoggingCategory logBlock("Block", QtCriticalMsg);

/**
    Initialize the index of the block as 0.
*/

Block::Block()
{
    mIndex = 0;
}

/**
    Initialize the time to a nullptr and the nonce of the
    blockchain to -1 (to be first).

    @param sDataIn String data received from the front-end.
*/

Block::Block(const std::string &sDataIn) : mData(sDataIn)
{
    mNonce = -1;
    mTime = time(nullptr);
}

/**
    Returns the hash of the current block.

    @return The hash of the current block.
*/

std::string Block::getHash()
{
    return mHash;
}

/**
    Returns the index of the last block.

    @return Returns the index of the last block.
*/

int Block::getLastIndex()
{
    return mIndex;
}

/**
    "Mine" the block of data.

    @param  difficulty Difficulty (number of zeroes)
            we should have before we can declare the
            block "mined".
*/

void Block::mineBlock(const int difficulty)
{
    // Initialize a char array where we fill
    // each position with a 0 and the last
    // position is the terminator ('\0') which
    // indicates the end of the char array.
    // The length of the char array is determinated
    // by the level of difficulty, which means if
    // the difficulty is set to 9, we will have a
    // char array looking like this as a string:
    // "000000000" (nine pieces of zeros)
    char *cstr = new char[difficulty + 1];
    for (int i = 0; i < difficulty; ++i) {
        cstr[i] = '0';
    }
    cstr[difficulty] = '\0';

    std::string str(cstr);

    // We keep hashing until the first 'difficulty' digits
    // are equal to 0. So if we set it to 1, if the first
    // character is 0, we stop the hashing and the block is
    // so saidly "mined". Increase the difficulty and you have
    // to keep hashing until you have enough zeros.

    // TO-DO: Write to a file for later calls
    do {
        mNonce++;
        mHash = calculateHash();
    } while (mHash.substr(0, difficulty) != str);

    qCDebug(logBlock) << "Block mined: " << QString::fromStdString(mHash);
    mIndex++;

    delete[] cstr;
}

/**
    Calculate the hash to validate the data.

    @return Returns a string encrypted with the
            SHA256 hashing algorithm which is
            valid for the index, current time,
            received data, nonce and previous
            hash of the block.
*/

inline std::string Block::calculateHash() const
{
    std::stringstream ss;
    ss << mIndex << mTime << mData << mNonce << prevHash;

    return sha256(ss.str());
}
