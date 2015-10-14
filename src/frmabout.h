#ifndef FRMABOUT_H
#define FRMABOUT_H

#include <QDialog>

namespace Ui {
    class frmAbout;
}

class frmAbout : public QDialog {
    Q_OBJECT
public:
    frmAbout(QWidget *parent = 0);
    ~frmAbout();
    Ui::frmAbout *ui;

protected:
    void changeEvent(QEvent *e);

};

#endif // FRMABOUT_H
