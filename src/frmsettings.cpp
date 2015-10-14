#include <QMessageBox>
#include <QDoubleValidator>


#include "frmsettings.h"
#include "ui_frmsettings.h"


frmSettings::frmSettings(QWidget *parent): QDialog(parent), m_ui(new Ui::frmSettings)
{
    m_ui->setupUi(this);

    m_ui->comboBox->addItem("Blaker");
    m_ui->comboBox->addItem("Sterne");
    m_ui->comboBox->addItem("Clopper-Pearson");
    m_ui->comboBox->addItem("Wilson");

    connect(m_ui->pushButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_ui->pushButton_2, SIGNAL(clicked()), this, SLOT(kontroll()));
    connect(m_ui->spinBox_2, SIGNAL(valueChanged(int)), this, SLOT(nval()));

}

void frmSettings::nval(){
    if (m_ui->spinBox_2->value()>1000){
        m_ui->comboBox->clear();
        m_ui->comboBox->addItem("Wilson");
    } else {
        m_ui->comboBox->clear();
        m_ui->comboBox->addItem("Blaker");
        m_ui->comboBox->addItem("Sterne");
        m_ui->comboBox->addItem("Clopper-Pearson");
        m_ui->comboBox->addItem("Wilson");
    }
}


void frmSettings::kontroll(){    
    if(m_ui->spinBox->value()>m_ui->spinBox_2->value()){
        QMessageBox::information(0,
            tr("Control"),
            tr("k must be less than or equal to n"),
            QMessageBox::Ok);
    } else {
        if(m_ui->spinBox_2->value() == 0){
            QMessageBox::information(0,
                tr("Control"),
                tr("n must be greater than 0"),
                QMessageBox::Ok);
        } else {
            accept();
        }
    }
}


frmSettings::~frmSettings()
{
    delete m_ui;
}

void frmSettings::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
