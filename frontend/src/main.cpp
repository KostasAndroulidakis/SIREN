/**
 * @file main.cpp
 * @brief Qt6 unoRadar Frontend Application
 * @author unoRadar Project
 * @date 2025
 *
 * Military-grade Qt6 application with WebSocket client for real-time radar data
 */

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtGui/QPalette>
#include <QtWebSockets/QWebSocket>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QStatusBar>
#include <QtCore/QTimer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QDebug>
#include <QtGui/QFont>

// Military-grade constants
namespace RadarConstants {
    const QString BACKEND_URL = "ws://localhost:8080";
    const int RECONNECT_INTERVAL_MS = 5000;
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
}

/**
 * @brief Main radar application window
 */
class RadarWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RadarWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
        , webSocket(new QWebSocket)
        , dataDisplay(new QTextEdit)
        , reconnectTimer(new QTimer(this))
    {
        setupUI();
        setupWebSocket();
        connectToBackend();
    }

    ~RadarWindow()
    {
        webSocket->deleteLater();
    }

private slots:
    void onConnected()
    {
        qDebug() << "Connected to unoRadar backend";
        statusBar()->showMessage("Connected to Backend", 2000);
        reconnectTimer->stop();
    }

    void onDisconnected()
    {
        qDebug() << "Disconnected from backend";
        statusBar()->showMessage("Disconnected - Attempting Reconnect", 0);
        reconnectTimer->start(RadarConstants::RECONNECT_INTERVAL_MS);
    }

    void onMessageReceived(const QString &message)
    {
        // Parse JSON radar data
        QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
        if (!doc.isNull()) {
            QJsonObject obj = doc.object();
            double angle = obj["angle"].toDouble();
            double distance = obj["distance"].toDouble();

            QString displayText = QString("Angle: %1° - Distance: %2cm")
                .arg(angle, 0, 'f', 1)
                .arg(distance, 0, 'f', 1);

            dataDisplay->append(displayText);

            // Keep only last 100 lines
            QStringList lines = dataDisplay->toPlainText().split('\n');
            if (lines.size() > 100) {
                lines = lines.mid(lines.size() - 100);
                dataDisplay->setPlainText(lines.join('\n'));
            }
        }
    }

    void onError(QAbstractSocket::SocketError error)
    {
        qDebug() << "WebSocket error:" << error;
        statusBar()->showMessage("Connection Error", 0);
    }

    void attemptReconnect()
    {
        qDebug() << "Attempting to reconnect to backend...";
        connectToBackend();
    }

private:
    void setupUI()
    {
        setWindowTitle("unoRadar - Military-Grade Radar System");
        resize(RadarConstants::WINDOW_WIDTH, RadarConstants::WINDOW_HEIGHT);

        // Military theme
        QPalette palette = this->palette();
        palette.setColor(QPalette::Window, Qt::black);
        palette.setColor(QPalette::WindowText, Qt::green);
        palette.setColor(QPalette::Base, Qt::black);
        palette.setColor(QPalette::Text, Qt::green);
        setPalette(palette);

        // Central widget
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        // Layout
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);

        // Data display
        dataDisplay->setReadOnly(true);
        dataDisplay->setFont(QFont("Consolas", 10));
        dataDisplay->setStyleSheet("background-color: black; color: green;");
        layout->addWidget(dataDisplay);

        // Status bar
        statusBar()->showMessage("Starting unoRadar Frontend...", 2000);
    }

    void setupWebSocket()
    {
        connect(webSocket, &QWebSocket::connected, this, &RadarWindow::onConnected);
        connect(webSocket, &QWebSocket::disconnected, this, &RadarWindow::onDisconnected);
        connect(webSocket, &QWebSocket::textMessageReceived, this, &RadarWindow::onMessageReceived);
        connect(webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
                this, &RadarWindow::onError);

        connect(reconnectTimer, &QTimer::timeout, this, &RadarWindow::attemptReconnect);
    }

    void connectToBackend()
    {
        qDebug() << "Connecting to:" << RadarConstants::BACKEND_URL;
        webSocket->open(QUrl(RadarConstants::BACKEND_URL));
    }

private:
    QWebSocket *webSocket;
    QTextEdit *dataDisplay;
    QTimer *reconnectTimer;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    RadarWindow window;
    window.show();

    return app.exec();
}

#include "main.moc"