#ifndef DATABASE_H
#define DATABASE_H

#include <QVariantMap>

class Database
{
public:
    Database();

    // [HV] to be implemented soon...
    QVariantMap countVotes(QString &);
    QVariantMap countAllVotes();
    QVariantMap requestVoters();
    bool checkForPreviousVoting(QString &);
};

#endif // DATABASE_H
