#ifndef FRMHELP_H
#define FRMHELP_H

#include <QDialog>

namespace Ui {
    class frmHelp;
}

class frmHelp : public QDialog {
    Q_OBJECT
public:
    frmHelp(QWidget *parent = 0);
    ~frmHelp();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::frmHelp *ui;
};

#endif // FRMHELP_H
