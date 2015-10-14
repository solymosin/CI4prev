#include <QDesktopWidget>
#include <QtGui>
#include <QTextEdit>
#include <QPrintDialog>
#include <QPrinter>
#include <QFileDialog>
#include <QMessageBox>

//#include <cmath>

#include "gsl/gsl_rng.h"
#include "gsl/gsl_randist.h"
#include "gsl/gsl_cdf.h"
//#include "gsl/gsl_cblas.h"


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "frmsettings.h"
#include "ui_frmsettings.h"
#include "frmabout.h"
#include "ui_frmabout.h"
#include "frmhelp.h"
#include "ui_frmhelp.h"


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*kozepre teszi a nyito kepernyot*/
    QRect available_geom = QDesktopWidget().availableGeometry();
    QRect current_geom =  frameGeometry();

    setGeometry(available_geom.width() / 2 - current_geom.width() / 2,
        available_geom.height() / 2 - current_geom.height() / 2,
        current_geom.width(),
        current_geom.height());
    /********************************/

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));    
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(MenTXT()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
    connect(ui->actionBinom, SIGNAL(triggered()), this, SLOT(binom()) );
    connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(showHelp()));
    connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(printIt()));

    ui->mainToolBar->addAction(ui->actionSave);
    ui->mainToolBar->addAction(ui->actionPrint);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionBinom);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionHelp);

    verzio = "2010-03-03";

    settingClear();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::printIt(){
    #ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    dlg->setWindowTitle(tr("Print Results"));

    if (ui->plainTextEdit->textCursor().hasSelection()){
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    }

    if (dlg->exec() == QDialog::Accepted) {
        ui->plainTextEdit->print(&printer);
    }

    delete dlg;
    #endif
}


void MainWindow::settingClear(){
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CI4prev", "ssconf");
    settings.beginGroup("CIprms");
    settings.setValue("k",     1);
    settings.setValue("n",    10);
    settings.setValue("alpha", 0.95);
    settings.setValue("Sp",    1.0);
    settings.setValue("Se",    1.0);
    settings.setValue("method","Blaker");
    settings.endGroup();
}


void MainWindow::showHelp(){
    frmHelp *dlg = new frmHelp();
    dlg->setWindowTitle("Help");

    if (dlg->exec() == QDialog::Accepted ){

    }
}



void MainWindow::binom(){
    frmSettings *dlg = new frmSettings();

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "RJsoft", "ssconf");

    settings.beginGroup("CIprms");
    if(settings.contains("k")){
        dlg->m_ui->spinBox->setValue(settings.value("k").toInt());
    } else {
        dlg->m_ui->spinBox->setValue(0);
    }
    if(settings.contains("n")){
        dlg->m_ui->spinBox_2->setValue(settings.value("n").toInt());
    } else {
        dlg->m_ui->spinBox_2->setValue(0);
    }
    if(settings.contains("alpha")){
        dlg->m_ui->doubleSpinBox_3->setValue(settings.value("alpha").toDouble());
    } else {
        dlg->m_ui->doubleSpinBox_3->setValue(0.95);
    }
    if(settings.contains("Sp")){
        dlg->m_ui->doubleSpinBox_2->setValue(settings.value("Sp").toDouble());
    } else {
        dlg->m_ui->doubleSpinBox_2->setValue(1.0);
    }
    if(settings.contains("Se")){
        dlg->m_ui->doubleSpinBox->setValue(settings.value("Se").toDouble());
    } else {
        dlg->m_ui->doubleSpinBox->setValue(1.0);
    }
    if(settings.contains("method")){
        dlg->m_ui->comboBox->setCurrentIndex(dlg->m_ui->comboBox->findText(settings.value("method").toString(), Qt::MatchExactly));
    } else {
        dlg->m_ui->comboBox->setCurrentIndex(dlg->m_ui->comboBox->findText("Blaker", Qt::MatchExactly));
    }
    settings.endGroup();

    if (dlg->exec() == QDialog::Accepted ){

        QApplication::setOverrideCursor( Qt::WaitCursor );

        unsigned int k = dlg->m_ui->spinBox->value();
        unsigned int n = dlg->m_ui->spinBox_2->value();
        double conf    = dlg->m_ui->doubleSpinBox_3->value();
        double alpha   = 1-conf;
        double Se = dlg->m_ui->doubleSpinBox->value();
        double Sp = dlg->m_ui->doubleSpinBox_2->value();

        settings.beginGroup("CIprms");
        settings.setValue("k",     k);
        settings.setValue("n",     n);
        settings.setValue("alpha", dlg->m_ui->doubleSpinBox_3->value());
        settings.setValue("Sp",    Sp);
        settings.setValue("Se",    Se);
        settings.setValue("method",dlg->m_ui->comboBox->currentText());
        settings.endGroup();

        double pbal = 0, pjobb=1, sum=0;
        double prev = (double)k/(double)n;

        ui->plainTextEdit->appendPlainText(
            QString("Number of test positives:   %1  out of   %2\nSensitivity: %3     Specificity: %4     Confidence level: %5     Method: %6\n")
            .arg(k).arg(n).arg(Se).arg(Sp).arg(conf)
            .arg(dlg->m_ui->comboBox->currentText())
            );


        if (dlg->m_ui->comboBox->currentText()=="Sterne"){

        // ----------------------------------------------------------
        // -------------------- S T E R N E -------------------------
        // ----------------------------------------------------------

            //bal
            int z =0;
            for(double p=0.0001; p<=0.9999; p = p+0.0001){
                sum=0;
                double kp = gsl_ran_binomial_pdf(k, p, n);
                for(unsigned int x=0; x!=n+1; x++){
                    double xp = gsl_ran_binomial_pdf(x, p, n);
                    if (xp<=kp){
                        sum = sum+xp;
                    }
                }
                if (sum<alpha){
                    pbal = p;
    //                ui->plainTextEdit->appendPlainText(QString("sum: %1; p: %2").arg(sum).arg(p));
                } else {
                    break;
                }
                z++;
            }

            //jobb
            z =0;
            for(double p=0.9999; p>=0.0001; p = p-0.0001){
                sum=0;
                double kp = gsl_ran_binomial_pdf(k, p, n);
                for(unsigned int x=0; x!=n+1; x++){
                    double xp = gsl_ran_binomial_pdf(x, p, n);
                    if (xp<=kp){
                        sum = sum+xp;
                    }
                }
                if (sum<alpha){
                    pjobb = p;
                } else {
                    break;
                }
                z++;
            }

        } else if (dlg->m_ui->comboBox->currentText()=="Blaker"){

        // ----------------------------------------------------------
        // -------------------- B L A K E R -------------------------
        // ----------------------------------------------------------

            //bal
            int z =0;
            for(double p=0.0001; p<=0.9999; p = p+0.0001){
                sum=0;
                double kp = gsl_cdf_binomial_P(k, p, n);
                        // ui->plainTextEdit->appendPlainText(QString("k %1 p %2 n %3 kp %4\n").arg(k).arg(p).arg(n).arg(kp));
                double kp2 = gsl_cdf_binomial_Q(k, p, n) + gsl_ran_binomial_pdf(k, p, n);
                        // ui->plainTextEdit->appendPlainText(QString("k %1 p %2 n %3 kp2 %4\n").arg(k).arg(p).arg(n).arg(kp2));

                if (kp2 < kp) kp=kp2;
                for(unsigned int x=0; x!=n+1; x++){
                    double xp = gsl_cdf_binomial_P(x, p, n);
                           // ui->plainTextEdit->appendPlainText(QString("x %1 p %2 n %3 kp %4\n").arg(x).arg(p).arg(n).arg(xp));
                    double xp2 = gsl_cdf_binomial_Q(x, p, n) + gsl_ran_binomial_pdf(x, p, n);
                           // ui->plainTextEdit->appendPlainText(QString("x %1 p %2 n %3 kp2 %4\n").arg(x).arg(p).arg(n).arg(xp2));
                    if (xp2 < xp) xp=xp2;
                    double xxp = gsl_ran_binomial_pdf(x, p, n);
                            // ui->plainTextEdit->appendPlainText(QString("x %1 p %2 n %3 xxp %4\n").arg(x).arg(p).arg(n).arg(xxp));
                    if (xp<=kp){
                        sum = sum+xxp;
                    }
                            // ui->plainTextEdit->appendPlainText(QString("p %1 sum %2\n").arg(p).arg(sum));

                }
                if (sum<alpha){
                    pbal = p;
     //              ui->plainTextEdit->appendPlainText(QString("sum: %1; p: %2").arg(sum).arg(p));
                } else {
                    break;
                }
                z++;
            }

            //jobb
            z =0;
            for(double p=0.9999; p>=0.0001; p = p-0.0001){
                sum=0;
                double kp = gsl_cdf_binomial_P(k, p, n);
                double kp2 = gsl_cdf_binomial_Q(k, p, n) + gsl_ran_binomial_pdf(k, p, n);
                if (kp2 < kp) kp=kp2;
                for(unsigned int x=0; x!=n+1; x++){
                    double xp = gsl_cdf_binomial_P(x, p, n);
                    double xp2 = gsl_cdf_binomial_Q(x, p, n) + gsl_ran_binomial_pdf(x, p, n);
                    if (xp2 < xp) xp=xp2;
                    double xxp = gsl_ran_binomial_pdf(x, p, n);
                    if (xp<=kp){
                        sum = sum+xxp;
                    }
                }
                 // ui->plainTextEdit->appendPlainText(QString("p %1 sum %2\n").arg(p).arg(sum));
                if (sum<alpha){
                    pjobb = p;
                } else {
                    break;
                }
                z++;
            }
        } else if (dlg->m_ui->comboBox->currentText()=="Wilson"){

        // ----------------------------------------------------------
        // -------------------- W I L S O N -------------------------
        // ----------------------------------------------------------

            double zkrit = 1.96;
            double plusminus = zkrit * pow(pow(zkrit, 2.) + 4. * k * (n-k) / n, .5);
            double nevezo = 2.*(n + pow(zkrit, 2.));

            pbal = (2.*k + pow(zkrit, 2) - plusminus)/nevezo;
            pjobb = (2.*k + pow(zkrit, 2) + plusminus)/nevezo;

        } else if (dlg->m_ui->comboBox->currentText()=="Clopper-Pearson"){

        // ----------------------------------------------------------
        // ------------- C L O P P E R   P E A R S O N --------------
        // ----------------------------------------------------------

            //bal
            int z =0;
            for(double p=0.0001; p<=0.9999; p = p+0.0001){
                double kp = gsl_cdf_binomial_Q(k, p, n) + gsl_ran_binomial_pdf(k, p, n);
    //                ui->plainTextEdit->appendPlainText(QString("kp: %1; p: %2").arg(kp).arg(p));
                if (kp<0.5*alpha){
                    pbal = p;
              } else {
                    break;
                }
                z++;
            }

            //jobb
            z =0;
            for(double p=0.9999; p>=0.0001; p = p-0.0001){
                double kp = gsl_cdf_binomial_P(k, p, n);
                if (kp<0.5*alpha){
                    pjobb = p;
                } else {
                    break;
                }
                z++;
            }

        }

        kiir(pbal, pjobb, prev, Sp, Se);

        QApplication::restoreOverrideCursor();
    }    
}


void MainWindow::kiir(double pbal, double pjobb, double prev, double Sp, double Se){
    double adjPrev = trafo(prev, Sp, Se);
    double balvp = trafo(pbal, Sp, Se);
    double jobbvp = trafo(pjobb, Sp, Se);
    QString s,lb,ub,lc,uc,pr;

    pr.sprintf("%.4f", prev);
    lc.sprintf("%.4f", pbal);
    uc.sprintf("%.4f", pjobb);
    ui->plainTextEdit->appendPlainText(QString("    Observed test prevalence:         %1    CI: (%2 , %3)").arg(pr).arg(lc).arg(uc));

    s.sprintf("%.4f", adjPrev);
    lb.sprintf("%.4f", balvp);
    ub.sprintf("%.4f", jobbvp);
    ui->plainTextEdit->appendPlainText(QString("    Prevalence adjusted for Se/Sp:  %1    CI: (%2 , %3)\n\n").arg(s).arg(lb).arg(ub));
}


double MainWindow::trafo(double pont, double Sp, double Se){
    double res = (pont + Sp-1)/(Se+Sp-1);
    if (res < 0) res=0;
    if (res > 1) res=1;
    return res;
}

bool MainWindow::MenTXT()
{    
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Text files (*.txt)"));
    if (fileName.isEmpty())
        return false;

    QFileInfo fi(fileName);
    if (fi.completeSuffix()==""){
        fileName.append(".txt");
    }

    QFile file(fileName);    
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
        tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << ui->plainTextEdit->toPlainText();
    QApplication::restoreOverrideCursor();
    return true;
 }


void MainWindow::showAbout(){
    frmAbout *dlg = new frmAbout();

    QString about = QString("CI for prevalence, version %1\n"
    "\n"
    "Designed and programmed by Jenő Reiczigel and Norbert Solymosi\n"
    "    Szent István University, Faculty of Veterinary Science,\n"
    "    Budapest, Hungary\n"
    "\n"
    "Bug reports and comments to solymosi.norbert@gmail.com\n").arg(verzio);

    dlg->ui->plainTextEdit->appendPlainText(about);
    dlg->ui->plainTextEdit->moveCursor(QTextCursor::Start,QTextCursor::MoveAnchor);

    if (dlg->exec() == QDialog::Accepted ){

    }
}


