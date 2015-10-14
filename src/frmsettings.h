#ifndef FRMSETTINGS_H
#define FRMSETTINGS_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
    class frmSettings;
}

class frmSettings : public QDialog {
    Q_OBJECT
public:
    frmSettings(QWidget *parent = 0);
    ~frmSettings();
    Ui::frmSettings *m_ui;

protected:
    void changeEvent(QEvent *e);

private slots:
    void kontroll();
    void nval();
};

#endif // FRMSETTINGS_H
