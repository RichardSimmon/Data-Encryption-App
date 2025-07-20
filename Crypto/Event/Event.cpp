// Event/Event.cpp
#include "Event.h"
#include "Dialog.h"
#include "IO.h"
#include "Crypto.h"
#include "InputHandler.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <gmp.h>
#include <algorithm>

EventHandler::EventHandler(Ui::MainWindow *ui) : m_ui(ui)
{
    connectEvents();
}

void EventHandler::connectEvents()
{
    connect(m_ui->BtInp, &QPushButton::clicked, this, &EventHandler::on_BtInp_Clicked);
    connect(m_ui->Encryption, &QPushButton::clicked, this, &EventHandler::on_Encryption_Clicked);
    connect(m_ui->Decryption, &QPushButton::clicked, this, &::EventHandler::on_Encryption_Clicked);
}

// Handle file browsing
void EventHandler::on_BtInp_Clicked()
{
    QString filePath = QFileDialog::getOpenFileName(nullptr, "Open File", "", "All Files (*)");
    if (!filePath.isEmpty())
        m_ui->TFInpPath->setText(filePath);
}

// Handle encryption logic: read → encrypt → write → display remainders
void EventHandler::on_Encryption_Clicked()
{
    QString InpPath = m_ui->TFInpPath->text();
    QFileInfo fileInfo(InpPath);
    QString OutpPath = fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + ".encr";

    mpz_t num;
    mpz_init(num);

    IO::readFileToBigInteger(InpPath.toStdString(), num);
    std::vector<std::string> modStrs, divStrs;
    for (QLineEdit *qLine : InputHandler::getDiv(m_ui))
        divStrs.push_back(qLine->text().toStdString());

    Crypto::encrypt(num, divStrs, modStrs);

    IO::writeBigIntegerToFile(OutpPath.toStdString(), num);

    QVector<QLineEdit *> modFields = InputHandler::getMod(m_ui);

    int limit = std::min(static_cast<int>(modFields.size()), static_cast<int>(modStrs.size()));
    for (int i = 0; i < limit; ++i)
    {
        modFields[i]->setText(QString::fromStdString(modStrs[i]));
    }

    Dialog::showSuccess(nullptr, "Encryption completed successfully.");
    mpz_clear(num);
}

// Handle decryption logic: read → reverse math → write
void EventHandler::on_Decryption_Clicked()
{
    QString InpPath = m_ui->TFInpPath->text();
    QFileInfo fileInfo(InpPath);
    QString OutpPath = fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + ".decr";

    mpz_t num;
    mpz_init(num);

    IO::readFileToBigInteger(InpPath.toStdString(), num);

    std::vector<std::string> divStrs, modStrs;
    for (QLineEdit *qLine : InputHandler::getDiv(m_ui))
        divStrs.push_back(qLine->text().toStdString());

    for (QLineEdit *qLine : InputHandler::getMod(m_ui))
        modStrs.push_back(qLine->text().toStdString());

    Crypto::decrypt(num, divStrs, modStrs);

    IO::writeBigIntegerToFile(OutpPath.toStdString(), num);

    Dialog::showSuccess(nullptr, "Decryption completed successfully.");
    mpz_clear(num);
}
