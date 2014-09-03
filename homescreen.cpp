#include "homescreen.h"
#include "ui_homescreen.h"
#include"masterpasswordform.h"
#include"opendatabaseform.h"
#include "newentryform.h"
#include "database.h"
#include "databasesettingsform.h"
#include "changemasterkeyform.h"
#include "groups.h"
#include "about.h"
#include <QMessageBox>
#include <QtSql>

HomeScreen::HomeScreen(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HomeScreen),dbPtr(new Database),locked(false),currentDbID("None"),currentGroupName("Default"),
    currentKeyID("None"),currentPassID("None"),currentDbName("None")
{
    ui->setupUi(this);
    //set default values  
    ui->treeWidget_groups->setColumnCount(1);
    ui->treeWidget_groups->setHeaderLabel("Groups");       
    LockWorkspace();
}

HomeScreen::~HomeScreen()
{
    delete ui;
}
void HomeScreen::setDatabaseID(QString id)
{
  currentDbID=id;
}
void HomeScreen::setDatabaseName(QString name)
{
    currentDbName=name;
    AddRoot(currentDbName);//Adds the current database to group tree
    qDebug()<<currentDbName;
}
void HomeScreen::AddRoot(QString name)
{
    QTreeWidgetItem *group=new QTreeWidgetItem(ui->treeWidget_groups);
    group->setText(0,name);
    QSqlQuery getGroups;
    getGroups.prepare("select groupName from groups where dbID='"+currentDbID+"'");
    if(getGroups.exec())
    {
        while(getGroups.next())
        {
            AddChild(group,getGroups.value(0).toString());//add each group of the database as a child in the tree
        }
    }

}
void HomeScreen::AddChild(QTreeWidgetItem *parent,QString name)
{
    QTreeWidgetItem *child=new QTreeWidgetItem();
    child->setText(0,name);
    parent->addChild(child);
}
void HomeScreen::RemoveRoot()
{
    ui->treeWidget_groups->clear();
}
void HomeScreen::RefreshTree()
{
    ui->treeWidget_groups->clear();
    AddRoot(currentDbName);
}

QString HomeScreen::getDatabaseID()
{
    return currentDbID;
}
QString HomeScreen::getGroupID()
{
    QSqlQuery id;
    id.prepare("select groupID from groups where dbID='"+currentDbID+"' and groupName='"+currentGroupName+"'");
    if(id.exec())
    {
        while(id.next())
        {
            QString gID=id.value(0).toString();
            return gID;
        }
    }
    else
        QMessageBox::warning(this,tr("Error"),id.lastError().text());
        return "None" ;

}

void HomeScreen::loadPasswords(QString id)
{
    //load all the passwords to the tableView
    QSqlQuery loadPass;
    QSqlQueryModel *load=new QSqlQueryModel();
    loadPass.prepare("select title,userName,password,notes,url,creationTime,modificationTime,passID from passwords where dbID='"+id+"'");
    loadPass.exec();
    load->setQuery(loadPass);
    load->setHeaderData(0,Qt::Horizontal,QObject::tr("Title"));
    load->setHeaderData(1,Qt::Horizontal,QObject::tr("User Name"));
    load->setHeaderData(2,Qt::Horizontal,QObject::tr("Password"));
    load->setHeaderData(3,Qt::Horizontal,QObject::tr("Notes"));
    load->setHeaderData(4,Qt::Horizontal,QObject::tr("URL"));
    load->setHeaderData(5,Qt::Horizontal,QObject::tr("Creation Time"));
    load->setHeaderData(6,Qt::Horizontal,QObject::tr("Modification Time"));
    load->setHeaderData(7,Qt::Horizontal,QObject::tr("Hidden"));
    ui->tableView_info->setModel(load);
    ui->tableView_info->hideColumn(7);

}
void HomeScreen::RefreshPasswords()
{
    QSqlQuery loadPass;
    QSqlQueryModel *load=new QSqlQueryModel();
    QString id=getGroupID();
    loadPass.prepare("select title,userName,password,notes,url,creationTime,modificationTime,passID from passwords where dbID='"+currentDbID+"' and groupID='"+id+"'");

    loadPass.exec();
    load->setQuery(loadPass);
    load->setHeaderData(0,Qt::Horizontal,QObject::tr("Title"));
    load->setHeaderData(1,Qt::Horizontal,QObject::tr("User Name"));
    load->setHeaderData(2,Qt::Horizontal,QObject::tr("Password"));
    load->setHeaderData(3,Qt::Horizontal,QObject::tr("Notes"));
    load->setHeaderData(4,Qt::Horizontal,QObject::tr("URL"));
    load->setHeaderData(5,Qt::Horizontal,QObject::tr("Creation Time"));
    load->setHeaderData(6,Qt::Horizontal,QObject::tr("Modification Time"));
    load->setHeaderData(7,Qt::Horizontal,QObject::tr("Hidden"));

    ui->tableView_info->setModel(load);
    ui->tableView_info->hideColumn(7);
}
void HomeScreen::DeletePassword(QString id)
{
    QSqlQuery del;
    del.prepare("delete from passwords where passID='"+id+"'");
    if(del.exec())
    {
        qDebug()<<"Password deleted...";
        RefreshPasswords();
    }
    else
    {
        qDebug()<<del.lastError().text();
    }
}
void HomeScreen::loadKeys(QString id)
{
    //load all the serial/product keys to the tableView
    QSqlQuery loadKey;
    QSqlQueryModel *load=new QSqlQueryModel();
    loadKey.prepare("select title,serialKey,notes,keyID from serials where dbID='"+id+"'");
    loadKey.exec();
    load->setQuery(loadKey);
    load->setHeaderData(0,Qt::Horizontal,QObject::tr("Title"));
    load->setHeaderData(1,Qt::Horizontal,QObject::tr("Serial/Product Key"));
    load->setHeaderData(2,Qt::Horizontal,QObject::tr("Notes"));
    load->setHeaderData(3,Qt::Horizontal,QObject::tr("Hidden"));

     ui->tableView_keyInfo->setModel(load);
     ui->tableView_keyInfo->hideColumn(3);
}
void HomeScreen::RefreshKeys()
{
    QSqlQuery loadKey;
    QSqlQueryModel *load=new QSqlQueryModel();
    QString id=getGroupID();
    loadKey.prepare("select title,serialKey,notes,keyID from serials where dbID='"+currentDbID+"'and groupID='"+id+"'");
    loadKey.exec();
    load->setQuery(loadKey);
    load->setHeaderData(0,Qt::Horizontal,QObject::tr("Title"));
    load->setHeaderData(1,Qt::Horizontal,QObject::tr("Serial/Product Key"));
    load->setHeaderData(2,Qt::Horizontal,QObject::tr("Notes"));
    load->setHeaderData(3,Qt::Horizontal,QObject::tr("Hidden"));

     ui->tableView_keyInfo->setModel(load);
     ui->tableView_keyInfo->hideColumn(3);
}
void HomeScreen::DeleteKey(QString id)
{
    QSqlQuery del;
    del.prepare("delete from serials where keyID='"+id+"'");
    if(del.exec())
    {
        qDebug()<<"Key deleted...";
        RefreshKeys();
    }
    else
    {
        qDebug()<<del.lastError().text();
    }
}
void HomeScreen::DeleteGroup(QString id)
{
    QSqlQuery delPasswords,delKeys,delGroup;
    delPasswords.prepare("delete from passwords where groupID='"+id+"'");
    delKeys.prepare("delete serials where groupID='"+id+"'");
    delGroup.prepare("delete from groups where groupID='"+id+"'");
    if(delPasswords.exec())
    {
        if(delKeys.exec())
        {
            if(delGroup.exec())
            {
                qDebug()<<"Group deleted....";
                RefreshTree();

            }
            else
                qDebug()<<"Error="<<delGroup.lastError().text();
        }
        else
            qDebug()<<"Error="<<delKeys.lastError().text();
    }
    else
        qDebug()<<"Error="<<delPasswords.lastError().text();
}

void HomeScreen::UnlockWorkspace()
{
    ui->tableView_info->setVisible(true);
    ui->tableView_keyInfo->setVisible(true);
    ui->treeWidget_groups->setVisible(true);
    ui->actionDatabase_Settings->setEnabled(true);
    ui->actionChange_Master_Key->setEnabled(true);
    ui->actionLock_Workspace->setEnabled(true);
    ui->actionAdd_Group->setEnabled(true);
    ui->actionAdd_New_Entry->setEnabled(true);
    locked=false;
    ui->actionLock_Workspace->setText("Lock Workspace");


}
void HomeScreen::LockWorkspace()
{
    ui->tableView_info->setVisible(false);
    ui->tableView_keyInfo->setVisible(false);
    ui->treeWidget_groups->setVisible(false);
    ui->actionDatabase_Settings->setEnabled(false);
    ui->actionChange_Master_Key->setEnabled(false);
    ui->actionLock_Workspace->setEnabled(false);
    ui->actionAdd_Group->setEnabled(false);
    ui->actionAdd_New_Entry->setEnabled(false);
    this->ui->actionDelete_Entry->setDisabled(true);
    this->ui->actionEdit_View_Entry->setDisabled(true);
    this->ui->actionDuplicate_Entry->setDisabled(true);
    this->ui->actionEdit_Group->setDisabled(true);
    this->ui->actionDelete_Group->setDisabled(true);

    if(locked)
    {
        ui->actionLock_Workspace->setEnabled(true);
        ui->actionLock_Workspace->setText("Unlock Workspace");
    }
}

void HomeScreen::on_actionNew_Database_triggered()
{
    MasterPasswordForm masterForm;
    masterForm.setMainFormReference(this);//set the master password form to reference the MainWindow
    masterForm.exec();//open master password form
    masterForm.setModal(true);
    this->ui->actionDelete_Entry->setDisabled(true);
    this->ui->actionEdit_View_Entry->setDisabled(true);
    this->ui->actionDuplicate_Entry->setDisabled(true);
    this->ui->actionEdit_Group->setDisabled(true);
    this->ui->actionDelete_Group->setDisabled(true);
}

void HomeScreen::on_actionOpen_Database_triggered()
{

    OpenDatabaseForm openDB;
    openDB.setMainFormReference(this);//set the open database form to reference the MainWindow
    openDB.setModal(true);
    openDB.exec();//show the open database form
    this->ui->actionDelete_Entry->setDisabled(true);
    this->ui->actionEdit_View_Entry->setDisabled(true);
    this->ui->actionDuplicate_Entry->setDisabled(true);
    this->ui->actionEdit_Group->setDisabled(true);
    this->ui->actionDelete_Group->setDisabled(true);
    if(currentDbID=="None")
    {
        //no database opened
        //do nothing
    }
    else
    {
        loadPasswords(currentDbID);//load all the database's passwords
        loadKeys(currentDbID);//load all the database's serial/product keys
    }

}

void HomeScreen::on_actionEdit_View_Entry_triggered()
{
    //validate current passID and keyID
    NewEntryForm edit;
    edit.setMainFormReference(this);
    if(currentPassID=="None")
    {
        edit.setKeyEditFeatures(currentKeyID);//set the Edit Entry form to edit currently selected key
    }
    else
    {
        edit.setEditFeatures(currentPassID);//set the Edit Entry form to edit currently selected password
    }
    edit.exec();//open the Edit Entry Form
}

void HomeScreen::on_actionAdd_New_Entry_triggered()
{
    //Add new password/key entry
    NewEntryForm entry;
    entry.setDatabaseID(currentDbID);
    entry.setGroupID(currentGroupName);
    entry.setMainFormReference(this);
    entry.setModal(true);
    entry.exec();

}


void HomeScreen::on_actionExit_triggered()
{
    QMessageBox::information(this,tr("Vault"),tr("Thank you for using Vault Password Safe"));
    dbPtr->connectionClose();//close database connection
    this->close();//exit program
}

void HomeScreen::on_actionChange_Master_Key_triggered()
{
        ChangeMasterKeyForm form;//Form to change the database's master key
        form.SetDbID(currentDbID);
        form.exec();
}

void HomeScreen::on_actionAdd_Group_triggered()
{
    //Open form to add a new Group
    Groups newGroup;
    newGroup.setMainFormReference(this);
    newGroup.setDbID(currentDbID);
    newGroup.exec();
}

void HomeScreen::on_treeWidget_groups_itemClicked(QTreeWidgetItem *item, int column)
{
    currentGroupName=item->data(column,0).toString();
    if(currentGroupName==currentDbName)
    {
        currentGroupName="Default";
    }
    else
    {
        //Only show passwords and keys for currently selected group
        this->ui->actionEdit_Group->setDisabled(false);
        this->ui->actionDelete_Group->setDisabled(false);
        RefreshPasswords();
        RefreshKeys();
    }

    qDebug()<<currentGroupName;
}

void HomeScreen::on_actionEdit_Group_triggered()
{
    Groups editForm;
    editForm.setMainFormReference(this);
    editForm.setEditFeatures(currentGroupName,currentDbID);//set Edit Form to edit currently selected group
    editForm.exec();
}

void HomeScreen::on_tableView_info_doubleClicked(const QModelIndex &index)
{
    //allow editing for currently selected password
   this->ui->actionDelete_Entry->setDisabled(false);
   this->ui->actionEdit_View_Entry->setDisabled(false);
   this->ui->actionDuplicate_Entry->setDisabled(false);
   int col= index.column();
   int row=index.row();
   currentPassID=index.sibling(row,7).data(0).toString();
   if(col==0)//if 1st column selected open EditForm
   {
     NewEntryForm edit;
     edit.setMainFormReference(this);
     edit.setEditFeatures(currentPassID);
     edit.exec();
   }
   else//copy the text to the clipboard for pasting
   {
       QClipboard *clipboard=QApplication::clipboard();
       clipboard->setText(index.data(0).toString(),QClipboard::Clipboard);
   }
   qDebug()<<QString::number(col);


}

void HomeScreen::on_actionDelete_Entry_triggered()
{
   int ret= QMessageBox::warning(this,tr("Delete"),tr("Are You Sure You want to permanently delete the selected entry?\n"),QMessageBox::Yes|QMessageBox::Cancel,QMessageBox::Cancel);
   if (ret==QMessageBox::Yes)
   {
       if(currentKeyID=="None")
       {
           DeletePassword(currentPassID);
       }
       else
       {
           DeleteKey(currentKeyID);
       }

   }
   if(ret==QMessageBox::Cancel)
   {
       qDebug()<<"Cancel pressed";
   }
}

void HomeScreen::on_tableView_info_clicked(const QModelIndex &index)
{
    this->ui->actionDelete_Entry->setDisabled(false);
    this->ui->actionEdit_View_Entry->setDisabled(false);
    this->ui->actionDuplicate_Entry->setDisabled(false);
    int row=index.row();
    currentPassID=index.sibling(row,7).data(0).toString();//set currentPassID to that of the currently selected record in the table
}

void HomeScreen::on_tableView_keyInfo_doubleClicked(const QModelIndex &index)
{
    this->ui->actionDelete_Entry->setDisabled(false);
    this->ui->actionEdit_View_Entry->setDisabled(false);
    this->ui->actionDuplicate_Entry->setDisabled(false);
    int col= index.column();
    int row=index.row();
    currentKeyID=index.sibling(row,3).data(0).toString();
    if(col==0)//open EditForm to edit the Key's Information
    {
      NewEntryForm edit;
      edit.setMainFormReference(this);
      edit.setKeyEditFeatures(currentKeyID);
      edit.exec();
    }
    else//copy selected text to the clipboard
    {
        QClipboard *clipboard=QApplication::clipboard();
        clipboard->setText(index.data(0).toString(),QClipboard::Clipboard);
    }
    qDebug()<<QString::number(col);
}

void HomeScreen::on_tableView_keyInfo_clicked(const QModelIndex &index)
{
    int row=index.row();
    currentKeyID=index.sibling(row,3).data(0).toString();//set currentKeyID to that of the currently selected record in the table
    this->ui->actionDelete_Entry->setDisabled(false);
    this->ui->actionEdit_View_Entry->setDisabled(false);
    this->ui->actionDuplicate_Entry->setDisabled(false);
}

void HomeScreen::on_tabWidget_tabBarClicked(int index)
{
    //reset values every time the tabBar is clicked
    if(index==0)
    {
        currentKeyID="None";
    }
    else
    {
        currentPassID="None";
    }
    this->ui->actionEdit_View_Entry->setDisabled(true);
    this->ui->actionDelete_Entry->setDisabled(true);
    this->ui->actionDuplicate_Entry->setDisabled(true);
}

void HomeScreen::on_actionLock_Workspace_triggered()
{
    //Hides all info on screen and prevents use till the user opens the database again
    locked=true;
    LockWorkspace();
    OpenDatabaseForm lock;
    lock.setLockFeatures(currentDbID);
    lock.setMainFormReference(this);
    lock.setModal(true);
    lock.exec();
    if(locked==true)
    this->showMinimized();

}


void HomeScreen::on_actionDatabase_Settings_triggered()
{
    //Change the details for the database
    DatabaseSettingsForm edit;
    edit.setMainFormReference(this);
    edit.setEdit(currentDbID);
    edit.exec();
}



void HomeScreen::on_actionDelete_Group_triggered()
{
    int ret= QMessageBox::warning(this,tr("Delete"),tr("Are You Sure You want to permanently delete the selected group?\nDoing so will also delete the entries in that group."),QMessageBox::Yes|QMessageBox::Cancel,QMessageBox::Cancel);
    if (ret==QMessageBox::Yes)
    {
       QSqlQuery getID;
       getID.prepare("select groupID from groups where groupName='"+currentGroupName+"' and dbID='"+currentDbID+"'");
       if(getID.exec())
       {
           while(getID.next())
           {
               DeleteGroup(getID.value(0).toString());
           }
       }

    }
    if(ret==QMessageBox::Cancel)
    {
        qDebug()<<"Cancel pressed";
    }
}

void HomeScreen::on_actionAbout_Vault_triggered()
{
    About about;
    about.setModal(true);
    about.exec();
}
