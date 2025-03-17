#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProcess>
#include <QMovie>
#include <QPalette>
#include <QTextEdit>
#include <QFile>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QTemporaryFile>

class MainWindow : public QWidget {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QWidget(parent) {
        // Set up the main window
        setWindowTitle("Apex KLGE Minimal v1.0");
        setFixedSize(800, 600); // Larger window size

        // Set background color
        QPalette pal = palette();
        pal.setColor(QPalette::Window, QColor("#00568f"));
        setPalette(pal);

        // Main layout
        QVBoxLayout *layout = new QVBoxLayout(this);

        // Group for image and text using a single QLabel with HTML
        QLabel *gifTextLabel = new QLabel(this);
        gifTextLabel->setText(QString(
            "<div style='text-align: center;'>"
            "<img src=':/images/apex.gif'><br/>" // Display the GIF
            "<span style='color: gold; font-size: 20px; font-weight: bold; margin: 0; padding: 0;'>"
            "Welcome To Apex KLGE Minimal v1.0"
            "</span>"
            "</div>"
        ));
        gifTextLabel->setAlignment(Qt::AlignCenter);

        layout->addWidget(gifTextLabel);

        // Buttons
        QPushButton *calamaresButton = new QPushButton("Install With Calamares", this);
        calamaresButton->setStyleSheet("background-color: #00568f; color: gold; font-size: 16px;");
        connect(calamaresButton, &QPushButton::clicked, this, &MainWindow::runCalamares);
        layout->addWidget(calamaresButton);

        QPushButton *apexInstallerButton = new QPushButton("Install With Apex Arch Installer", this);
        apexInstallerButton->setStyleSheet("background-color: #00568f; color: gold; font-size: 16px;");
        connect(apexInstallerButton, &QPushButton::clicked, this, &MainWindow::runApexInstaller);
        layout->addWidget(apexInstallerButton);

        // ChangeLog button
        QPushButton *changeLogButton = new QPushButton("ChangeLog", this);
        changeLogButton->setStyleSheet("background-color: #00568f; color: gold; font-size: 16px;");
        connect(changeLogButton, &QPushButton::clicked, this, &MainWindow::downloadAndShowChangeLog);
        layout->addWidget(changeLogButton);

        setLayout(layout);

        // Initialize network manager
        networkManager = new QNetworkAccessManager(this);
    }

private slots:
    void runCalamares() {
        QProcess::startDetached("pkexec calamares");
    }

    void runApexInstaller() {
        QProcess::startDetached("ApexInstaller");
    }

    void downloadAndShowChangeLog() {
        // URL to download the ChangeLog
        QUrl url("https://github.com/claudemods/ApexKLGE-Minimal/releases/download/v1.0/Minimal-ChangeLog");

        // Start the download
        QNetworkReply *reply = networkManager->get(QNetworkRequest(url));
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() == QNetworkReply::NoError) {
                // Save the downloaded file temporarily
                QTemporaryFile tempFile;
                if (tempFile.open()) {
                    tempFile.write(reply->readAll());
                    tempFile.close();

                    // Open and display the file
                    showChangeLog(tempFile.fileName());
                } else {
                    QMessageBox::warning(this, "Error", "Could not create temporary file.");
                }
            } else {
                QMessageBox::warning(this, "Error", "Failed to download ChangeLog: " + reply->errorString());
            }
            reply->deleteLater();
        });
    }

    void showChangeLog(const QString &filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Error", "Could not open changelog file.");
            return;
        }

        QTextStream in(&file);
        QString changeLogText = in.readAll();
        file.close();

        // Create a QTextEdit to display the changelog
        QTextEdit *textEdit = new QTextEdit();
        textEdit->setPlainText(changeLogText);
        textEdit->setReadOnly(true);
        textEdit->setStyleSheet("background-color: #00568f; color: gold; font-size: 14px;");

        // Create a layout and add the QTextEdit
        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(textEdit);

        // Create a new window for the changelog
        QWidget *changeLogWindow = new QWidget();
        changeLogWindow->setWindowTitle("ChangeLog");
        changeLogWindow->setLayout(layout);
        changeLogWindow->resize(1000, 600); // Wider window (1000x600)

        // Set the background color of the window
        QPalette pal = changeLogWindow->palette();
        pal.setColor(QPalette::Window, QColor("#00568f"));
        changeLogWindow->setPalette(pal);

        changeLogWindow->show();
    }

private:
    QNetworkAccessManager *networkManager;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}

#include "main.moc"
