#ifndef GROUPS_H
#define GROUPS_H

#include <QDialog>
#include <QtSql>
#include "homescreen.h"

namespace Ui {
class Groups;
}

class Groups : public QDialog
{
    Q_OBJECT

public:
    explicit Groups(QWidget *parent = 0);
    void setMainFormReference(HomeScreen*);
    void setGroupID(QString);
    void setDbID(QString);
    ~Groups();

private slots:
    void on_buttonBox_Response_accepted();

private:
    Ui::Groups *ui;
    HomeScreen* main;
    QString groupID;
    QString dbID;
};

#endif // GROUPS_H
