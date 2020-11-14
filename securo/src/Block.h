#ifndef BLOCK_H
#define BLOCK_H

#include <cstdint>
#include <iostream>
#include <time.h>

using namespace std;

class Block
{
public:
    std::string prevHash;

    Block();
    Block(const std::string &);

    std::string getHash();

    void mineBlock(const int);
    int getLastIndex();

private:
    int mNonce;
    std::string mData;
    std::string mHash;
    time_t mTime;

    std::string calculateHash() const;
};

#endif // BLOCK_H
