#include "transferconfirmdialog.h"
#include "ui_transferconfirmdialog.h"
#include "tic.h"
#include "debug_log.h"
#include "transferpage.h"

TransferConfirmDialog::TransferConfirmDialog(QString address, QString amount, QString fee, QString remark, QWidget *parent) :
    QDialog(parent),
    address(address),
    amount(amount),
    fee(fee),
    remark(remark),
    yesOrNo(false),
    ui(new Ui::TransferConfirmDialog)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);

    setParent(Fry::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color: rgb(45,47,51);border:1px groove rgb(54,56,60);}");

    TransferPage *p = (TransferPage*)parent;
    QComboBox *asset_combo = p->getAssetCombo();

    ui->addressLabel->setText(address);
    if (asset_combo->currentText() == "TV") {
        ui->amountLabel->setText("<body><B>" + amount + "</B>" + " TV</body>");
        ui->feeLabel->setText("<body><font color=#409AFF>" + fee + "</font>" + " TV</body>");
    }
    else if (asset_combo->currentText() == "ACS") {
        ui->amountLabel->setText("<body><B>" + amount + "</B>" + " ACS</body>");
        ui->feeLabel->setText("<body><font color=#409AFF>" + fee + "</font>" + " TV</body>");
    }
    else if (asset_combo->currentText() == "3DC") {
        ui->amountLabel->setText("<body><B>" + amount + "</B>" + " 3DC</body>");
        ui->feeLabel->setText("<body><font color=#409AFF>" + fee + "</font>" + " TV</body>");
    }
    else if (asset_combo->currentText() == "NBJ") {
        ui->amountLabel->setText("<body><B>" + amount + "</B>" + " NBJ</body>");
        ui->feeLabel->setText("<body><font color=#409AFF>" + fee + "</font>" + " TV</body>");
    }
    else if (asset_combo->currentText() == "NOT") {
        ui->amountLabel->setText("<body><B>" + amount + "</B>" + " NOT</body>");
        ui->feeLabel->setText("<body><font color=#409AFF>" + fee + "</font>" + " TV</body>");
    }

    ui->remarkLabel->setText(remark);
    ui->okBtn->setText(tr("Ok"));
    ui->cancelBtn->setText(tr("Cancel"));

    ui->pwdLineEdit->setStyleSheet("background:rgb(37,39,43);color:rgb(190,190,190);border:none;");
    ui->pwdLineEdit->setPlaceholderText(tr("Enter login password"));
    ui->pwdLineEdit->setTextMargins(8, 0, 0, 0);
    ui->pwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit->setFocus();

    if (amount.toDouble() < 1000) {
        ui->pwdLabel->hide();
        ui->pwdLineEdit->hide();
    } else {
        ui->okBtn->setEnabled(false);
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

TransferConfirmDialog::~TransferConfirmDialog()
{
    delete ui;
}

bool TransferConfirmDialog::pop()
{
    ui->pwdLineEdit->grabKeyboard();
    move(0,0);
    exec();
    return yesOrNo;
}

void TransferConfirmDialog::on_okBtn_clicked()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    if (amount.toDouble() < 1000) {
        yesOrNo = true;
        close();
    } else {
        if (ui->pwdLineEdit->text().isEmpty()) return;

        QString str = "wallet_check_passphrase " + ui->pwdLineEdit->text() + "\n";
        Fry::getInstance()->write( str);
        QString result = Fry::getInstance()->read();

        if (result.mid(0, 4) == "true") {
            yesOrNo = true;
            close();
        } else if (result.mid(0, 5) == "false") {
            ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr("Wrong password!") + "</font></body>" );
            return;
        } else if (result.mid(0, 5) == "20015") {
            ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr("At least 8 letters!") + "</font></body>" );
            return;
        }
    }
    DLOG_QT_WALLET_FUNCTION_END;
}

void TransferConfirmDialog::on_cancelBtn_clicked()
{
    yesOrNo = false;
    close();
}

void TransferConfirmDialog::on_pwdLineEdit_textChanged(const QString &arg1)
{
    if (arg1.indexOf(' ') > -1) {
        ui->pwdLineEdit->setText(ui->pwdLineEdit->text().remove(' '));
        return;
    }

    if (arg1.isEmpty()) {
        ui->okBtn->setEnabled(false);
    } else {
        ui->okBtn->setEnabled(true);
    }
}
