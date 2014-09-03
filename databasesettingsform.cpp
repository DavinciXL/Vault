#include "databasesettingsform.h"
#include "ui_databasesettingsform.h"

DatabaseSettingsForm::DatabaseSettingsForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseSettingsForm),edit(false)
{
    ui->setupUi(this);
}

DatabaseSettingsForm::~DatabaseSettingsForm()
{
    delete ui;
}
void DatabaseSettingsForm::setMasterKey(QString key)
{
    newDatabase.SetMasterKey(key);
}
void DatabaseSettingsForm::setMainFormReference(HomeScreen *mainForm)
{
    main=mainForm;
}
void DatabaseSettingsForm::setEdit(QString id)
{
    edit=true;
    dbID=id;
}

void DatabaseSettingsForm::on_buttonBox_Response_accepted()
{
    if(edit)//changing details of currently open database
    {
        main->RemoveRoot();
        newDatabase.SetDbName(ui->lineEdit_dbName->text());
        newDatabase.SetDescription(ui->lineEdit_description->text());
        newDatabase.SetUserName(ui->lineEdit_userName->text());
        newDatabase.SetDbID(dbID);
        newDatabase.EditSettings();
        main->setDatabaseName(newDatabase.getDbName());
        this->close();
    }
    else//entering details for a new database
    {
        newDatabase.SetDbName(ui->lineEdit_dbName->text());
        newDatabase.SetDescription(ui->lineEdit_description->text());
        newDatabase.SetUserName(ui->lineEdit_userName->text());
        newDatabase.SaveSettings();
        main->setDatabaseID(newDatabase.getDbID());
        main->setDatabaseName(newDatabase.getDbName());
        main->UnlockWorkspace();
        this->close();
    }
}

void DatabaseSettingsForm::on_buttonBox_Response_rejected()
{
    this->close();
}
