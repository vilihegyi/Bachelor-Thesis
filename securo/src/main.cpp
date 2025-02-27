#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QTextCodec>

#include "Daemon.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption port(
        {QStringLiteral("p"), QStringLiteral("port")},
        QStringLiteral("Port to listen on (default: 8080)."),
        QStringLiteral("port"));
    parser.addOption(port);
    parser.process(app);

    auto daemon = new Daemon(&app);

    if (parser.isSet(port)) {
        bool ok;
        auto customPort = parser.value(port).toInt(&ok, 10);
        if (!ok) {
            qCritical() << "Invalid argument to port:" << parser.value(port);
            return EXIT_FAILURE;
        }
        daemon->setPort(customPort);
    }

    QTimer::singleShot(0, daemon, &Daemon::start);

    return app.exec();
}
