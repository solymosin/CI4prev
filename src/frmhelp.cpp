#include "frmhelp.h"
#include "ui_frmhelp.h"

frmHelp::frmHelp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmHelp)
{
    ui->setupUi(this);
}

frmHelp::~frmHelp()
{
    delete ui;
}

void frmHelp::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
