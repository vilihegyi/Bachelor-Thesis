#include <QDebug>
#include <QLoggingCategory>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "Database.h"

static QLoggingCategory logDatabase("Database", QtCriticalMsg);

/**
    Create the connection to the database with the credentials.
*/

Database::Database()
{
    // [HV] Could be a good idea not to store these hardcoded...
    // Also, maybe create the database in case it doesn't exist?!
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("password");
    db.setDatabaseName("blockchainVoting");

    if (!db.open()) {
        qCCritical(logDatabase) << "Failed to open the database!";
    }

    qCDebug(logDatabase) << "The database connection has been successfully established!";
}

/**
    TO-DO:
    Returns the number of votes for a given candidate.

    @param candidate The name of the candidate.
    @return The number of appearances for the candidate.
            In case no candidate is found we return an
            empty QVariantMap.
*/

QVariantMap Database::countVotes(QString &)
{
    return QVariantMap();
}

/**
    TO-DO:
    Returns the number of votes for all candidates.

    @return QVariantMap with all candidates and number
            of votes they have received.
*/

QVariantMap Database::countAllVotes()
{
    QSqlQuery optionsQuery;
    optionsQuery.exec("SELECT VoteOption FROM blockchainVoting.votes");
    while (optionsQuery.next()) {
        // TO-DO
        //QSqlQuery query;
        //query.exec("SELECT VoteOption, ROUND ((LENGTH(VoteOption) - LENGTH(REPLACE(VoteOption, "Popescu Ionescu", ""))) / LENGTH(' + ')) AS Count FROM votes");
    }

    return QVariantMap();
}

/**
    TO-DO:
    Returns the current candidates for the vote.

    @return QVariantMap with all candidates taking
            part in the election.
*/

QVariantMap Database::requestVoters()
{
    return QVariantMap();
}

/**
    Returns the number of votes for all candidates.

    @param votersCNP The CNP of the voter.
    @return True in case the CNP appears in the database
            already, otherwise false.
*/

bool Database::checkForPreviousVoting(QString &votersCNP)
{
    QSqlQuery duplicateCheckQuery;

    if (!duplicateCheckQuery.exec("SELECT Name, CNP, COUNT(*) FROM blockchainVoting.votes WHERE CNP='"
        + votersCNP + "' GROUP BY Name, CNP HAVING COUNT(*) > 0")) {
        return false;
    }

    return duplicateCheckQuery.next();
}
