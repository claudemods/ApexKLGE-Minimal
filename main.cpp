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
#include <QDebug>

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

        // GIF and text at the top
        QLabel *gifTextLabel = new QLabel(this);
        gifTextLabel->setText(QString(
            "<div style='text-align: center;'>"
            "<img src=':/images/apex.gif'><br/>" // Display the GIF
            "<span style='color: #24ffff; font-size: 20px; font-weight: bold; margin: 0; padding: 0;'>"
            "Welcome To Apex KLGE Minimal v1.0"
            "</span>"
            "</div>"
        ));
        gifTextLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter); // Align to the top and center horizontally

        // Add the GIF and text label to the layout
        layout->addWidget(gifTextLabel);

        // Add a stretch to push the logo to the center
        layout->addStretch();

        // Centered logo
        QLabel *logoLabel = new QLabel(this);
        QPixmap logoPixmap(":/images/logo.png"); // Load the logo image
        logoLabel->setPixmap(logoPixmap.scaled(200, 200, Qt::KeepAspectRatio)); // Make the logo smaller
        logoLabel->setAlignment(Qt::AlignCenter); // Center the logo

        // Add the logo to the layout
        layout->addWidget(logoLabel);

        // Add another stretch to push the buttons to the bottom
        layout->addStretch();

        // Buttons
        QPushButton *calamaresButton = new QPushButton("Install With Calamares", this);
        calamaresButton->setStyleSheet("background-color: #00568f; color: #24ffff; font-size: 16px;");
        connect(calamaresButton, &QPushButton::clicked, this, &MainWindow::runCalamares);
        layout->addWidget(calamaresButton);

        QPushButton *apexInstallerButton = new QPushButton("Install With Apex Arch Installer", this);
        apexInstallerButton->setStyleSheet("background-color: #00568f; color: #24ffff; font-size: 16px;");
        connect(apexInstallerButton, &QPushButton::clicked, this, &MainWindow::runApexInstaller);
        layout->addWidget(apexInstallerButton);

        // ChangeLog button
        QPushButton *changeLogButton = new QPushButton("ChangeLog", this);
        changeLogButton->setStyleSheet("background-color: #00568f; color: #24ffff; font-size: 16px;");
        connect(changeLogButton, &QPushButton::clicked, this, &MainWindow::downloadAndShowChangeLog);
        layout->addWidget(changeLogButton);

        setLayout(layout);

        // Initialize network manager
        networkManager = new QNetworkAccessManager(this);
    }

private slots:
    void runCalamares() {
        QProcess *process = new QProcess(this);
        connect(process, QOverload<QProcess::ProcessError>::of(&QProcess::errorOccurred), this, [this, process](QProcess::ProcessError error) {
            qDebug() << "Error occurred:" << error;
            QMessageBox::warning(this, "Error", "Failed to start Calamares: " + process->errorString());
            process->deleteLater();
        });

        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
            qDebug() << "Calamares finished with exit code:" << exitCode;
            process->deleteLater();
        });

        process->start("pkexec", QStringList() << "calamares");
    }

    void runApexInstaller() {
        QProcess *process = new QProcess(this);
        connect(process, QOverload<QProcess::ProcessError>::of(&QProcess::errorOccurred), this, [this, process](QProcess::ProcessError error) {
            qDebug() << "Error occurred:" << error;
            QMessageBox::warning(this, "Error", "Failed to start Apex Installer: " + process->errorString());
            process->deleteLater();
        });

        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
            qDebug() << "Apex Installer finished with exit code:" << exitCode;
            process->deleteLater();
        });

        process->start("ApexInstaller");
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
        textEdit->setStyleSheet("background-color: #00568f; color: #24ffff; font-size: 14px;");

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
