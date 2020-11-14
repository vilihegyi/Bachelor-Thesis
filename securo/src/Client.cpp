#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLoggingCategory>
#include <QTimer>
#include <QVariantMap>

#include "Client.h"
#include "Daemon.h"

static QLoggingCategory logClient("Client", QtCriticalMsg);

// BEGINNING OF BLOCKCHAIN KEYS
static const auto cnpKey = QStringLiteral("cnp");
static const auto nameKey = QStringLiteral("name");
static const auto addressKey = QStringLiteral("address");
static const auto emailAddressKey = QStringLiteral("emailAddress");
static const auto voteOptionKey = QStringLiteral("voteOption");
// END OF BLOCKCHAIN KEYS
static const auto dataKey = QStringLiteral("data");
static const auto requestKey = QStringLiteral("request");

/**
    Create a new instance for the newly connected client.
    Also create a new thread on the blockchain to keep up
    the circularity.
*/

Client::Client(QWebSocket *socket, Daemon *parent)
    : QObject(parent), mSocket(socket)
{
    mSocket->setParent(this);
    mParent = parent;

    qCDebug(logClient) << "Client created:" << socket->peerAddress()
                       << "Port" << socket->peerPort();

    connect(mSocket, &QWebSocket::disconnected, [=]() {
        qCDebug(logClient) << "Client disconnected:" << socket->peerAddress()
                           << "Port" << socket->peerPort();
        deleteLater();
    });

    mBlock = new Block();
    mBlockChain = new Blockchain();
    mBlockChain->start();

    connect(mSocket, &QWebSocket::textMessageReceived, this, &Client::request);
    connect(mBlockChain, &Blockchain::miningCompleted, this, &Client::onNotification);
}

/**
    Destroy the previous client and stop their thread.
*/

Client::~Client()
{
    qCDebug(logClient) << "Client destroyed.";
    mBlockChain->stop();
}

/**
    Returns the index of the last block.

    @param  dataIn Get the request and the data it holds through the
            websocket and validate it progressively.
*/

void Client::request(const QString &dataIn)
{
    const auto document = QJsonDocument::fromJson(dataIn.toUtf8());
    if (!document.isObject()) {
        qCCritical(logClient) << "Request is not an object:" << dataIn;
        mSocket->close(QWebSocketProtocol::CloseCodeDatatypeNotSupported);
        return;
    }

    qCDebug(logClient).noquote() << "request:"
                                 << QString::fromUtf8(document.toJson());

    const auto object = document.object();
    if (!object.contains(requestKey)) {
        qCCritical(logClient) << "Object is missing request key: " << object;
        mSocket->close(QWebSocketProtocol::CloseCodeDatatypeNotSupported);
        return;
    }

    const auto value = object.value(requestKey);
    if (!value.isString()) {
        qCCritical(logClient) << "Invalid request-type:" << value;
        mSocket->close(QWebSocketProtocol::CloseCodeDatatypeNotSupported);
        return;
    }

    const auto name = value.toString();
    qDebug() << "Name of request: " << name;

    QVariantMap request = object.toVariantMap();

    if (!request.contains(dataKey)) {
        qCCritical(logClient) << "Request contains no data " << request;
        mSocket->close(QWebSocketProtocol::CloseCodeDatatypeNotSupported);
        return;
    }

    const auto data = request.value(dataKey).toMap();

    if (!data.contains(cnpKey)) {
        qCCritical(logClient) << "Request is missing data, i.e. CNP";
        mSocket->close(QWebSocketProtocol::CloseCodeDatatypeNotSupported);
        return;
    }

    const auto cnp = data.value(cnpKey).toString();

    if (!data.contains(nameKey)) {
        qCCritical(logClient) << "Request is missing data, i.e. name";
        mSocket->close(QWebSocketProtocol::CloseCodeDatatypeNotSupported);
        return;
    }

    const auto fullName = data.value(nameKey).toString();

    if (!data.contains(addressKey)) {
        qCCritical(logClient) << "Request is missing data, i.e. address";
        mSocket->close(QWebSocketProtocol::CloseCodeDatatypeNotSupported);
        return;
    }

    const auto address = data.value(addressKey).toString();

    if (!data.contains(emailAddressKey)) {
        qCCritical(logClient) << "Request is missing data, i.e. email address";
        mSocket->close(QWebSocketProtocol::CloseCodeDatatypeNotSupported);
        return;
    }

    const auto emailAddress = data.value(emailAddressKey).toString();

    if (!data.contains(voteOptionKey)) {
        qCCritical(logClient) << "Request is missing data, i.e. vote option";
        mSocket->close(QWebSocketProtocol::CloseCodeDatatypeNotSupported);
        return;
    }

    const auto voteOption = data.value(voteOptionKey).toString();

    // Everything's set, we can start mining! :)
    qDebug() << "Mining block " << mBlock->getLastIndex() << "...";
    const QString blockData(cnp + "+" + fullName + "+" + address + "+" + emailAddress + "+" + voteOption);
    qDebug() << "Blockdata: " << blockData;
    mBlockChain->add(blockData);
}

/**
    Send through the websocket the result for the received request.

    @param  result QVariantMap containing the result for the
            request received.
*/

void Client::onNotification(const QVariantMap &result)
{
    // Reply to the webapp
    if (mSocket) {
        const auto object = QJsonObject::fromVariantMap(result);
        const auto document = QJsonDocument(object);
        mSocket->sendTextMessage(
            QString(document.toJson(QJsonDocument::Compact)));
    }
}
