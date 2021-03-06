#include "projectswidget.h"
#include "projectpassportdialog.h"
#include "projectcodemetricsdialog.h"

#include <QFile>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMenu>

ProjectsWidget::ProjectsWidget (QWidget *parent)
    : QWidget (parent)
    , _view (nullptr)
    , _model (nullptr)
{
    QVBoxLayout *vbl = new QVBoxLayout ();
    vbl->setContentsMargins (4, 4, 4, 4);
    setLayout (vbl);

    _view = new QTreeView (this);
    _view->header ()->hide ();
    _view->setContextMenuPolicy (Qt::CustomContextMenu);
    _view->setSelectionMode (QAbstractItemView::SingleSelection);
    _view->setEditTriggers (QAbstractItemView::NoEditTriggers);

    _model = new QStandardItemModel (0, 1);
    _view->setModel (_model);

    connect (_view, SIGNAL(customContextMenuRequested(QPoint)),
             this, SLOT(slotCustomContextMenu(QPoint)));
    connect (_view->selectionModel (), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
             SLOT(slotSelectionChanged(QItemSelection,QItemSelection)));
    vbl->addWidget (_view);
}

QString ProjectsWidget::selectedProject () const
{
    if (_currentIndex.isValid ())
        return _currentIndex.data (Qt::UserRole + 1).toString ();
    else
        return QString::null;
}

void ProjectsWidget::slotInitProjectsList ()
{
    if (RedmineInstance::instance ().loadIssueCategories ()) {
        qDebug () << "[ProjectsWidget][slotInitProjectsList] Issue categories loaded";
    } else {
        qCritical () << "[ProjectsWidget][slotInitProjectsList] Could not load issue categories";
        return;
    }

    if (RedmineInstance::instance ().loadIssueStatuses ()) {
        qDebug () << "[ProjectsWidget][slotInitProjectsList] Issue statuses loaded";
    } else {
        qCritical () << "[ProjectsWidget][slotInitProjectsList] Could not load issue statuses";
        return;
    }

    if (RedmineInstance::instance ().loadTrackers ()) {
        qDebug () << "[ProjectsWidget][slotInitProjectsList] Trackers loaded";
    } else {
        qCritical () << "[ProjectsWidget][slotInitProjectsList] Could not load trackers";
        return;
    }

    if (RedmineInstance::instance ().loadProjects ()) {
        qDebug () << "[ProjectsWidget][slotInitProjectsList] Projects loaded";
    } else {
        qCritical () << "[ProjectsWidget][slotInitProjectsList] Could not load projects list";
        return;
    }

    const QMap<QString, QSharedPointer<RedmineProject> > projects =
            RedmineInstance::instance ().projects ();

    _model->removeRows (0, _model->rowCount ());
    _model->setRowCount (0);

    QMapIterator<QString, QSharedPointer<RedmineProject>> iter (projects);
    while (iter.hasNext ()) {
        iter.next ();
        if (iter.value ()->_parent_id == "0")
        {
            QStandardItem *item = new QStandardItem (iter.value ()->_name);
            item->setData (iter.value ()->_id, Qt::UserRole + 1);
            deepTreeRun (item, projects);

            _model->appendRow (item);
        }
    }

    _view->expandAll ();

    //    QMapIterator<QString, QSharedPointer<RedmineProject> > iter
    //            (projects);

    //    while (iter.hasNext ()) {
    //        iter.next ();
    //        qDebug () << iter.value ()->_id << iter.value ()->_parent_id;
    //    }

    //    QSqlDatabase db = QSqlDatabase::database ("yuliya");
    //    if (!db.isValid () || !db.isOpen ())
    //        return;

    //    QSqlQuery query (db);
    //    if (!query.exec ("select * from projects;"))
    //    {
    //        qCritical () << "[ProjectsWidget][slotInitProjectsList] Connection error";
    //        qCritical () << "[ProjectsWidget][slotInitProjectsList]"
    //                     << query.lastError ().databaseText ();
    //        qCritical () << "[ProjectsWidget][slotInitProjectsList]" <<
    //                        query.lastError ().driverText ();
    //        qCritical () << "[ProjectsWidget][slotInitProjectsList]" <<
    //                        query.lastError ().text ();

    //        return;
    //    }

    //    QFile f ("/home/comrade/projects.list");
    //    if (!f.open (QIODevice::ReadWrite | QIODevice::Truncate))
    //        return;

    //    int count = query.record ().count ();
    //    for (int i = 0; i < count; ++i) {
    //        f.write (query.record ().fieldName (i).toUtf8 ());
    //        f.write ("\n");
    //    }
    //    f.close ();
}

void ProjectsWidget::slotCustomContextMenu (const QPoint &pos)
{
    QModelIndex index = _view->indexAt (pos);
    if (!index.isValid ())
        return;

    QString prjid = index.data (Qt::UserRole + 1).toString ();
    if (prjid.isEmpty ())
        return;

    QMenu menu;
    menu.addAction (trUtf8 ("Паспорт проекта"));
    menu.addAction (trUtf8 ("Настроки кода"));
    QAction *act = menu.exec (_view->viewport ()->mapToGlobal (pos));
    if (!act) return;

    if (act->text () == trUtf8 ("Паспорт проекта"))
    {
        if (!RedmineInstance::instance ().loadIssues (prjid)) {
            qCritical () << "[ProjectsWidget][slotCustomContextMenu]";
            return;
        } else {
            auto *dlg = new ProjectPassportDialog (prjid, this);
            dlg->exec ();
        }
    }
    else if (act->text () == trUtf8 ("Настроки кода"))
    {
        auto *dlg = new ProjectCodeMetricsDialog (prjid, this);
        dlg->exec ();
    }
}

void ProjectsWidget::slotSelectionChanged (const QItemSelection &selected,
                                           const QItemSelection &deselected)
{
    (void)deselected;

    if (selected.isEmpty ())
        _currentIndex = QModelIndex ();
    else
        _currentIndex = selected.indexes ()[0];

    emit projectSelected (_currentIndex.data (Qt::UserRole + 1).toString ());
}

void ProjectsWidget::showEvent (QShowEvent */*ev*/)
{
    QSqlDatabase db = QSqlDatabase::database ("yuliya");
    if (!db.isValid () || !db.isOpen ())
        return;

    QTimer::singleShot (0, this, SLOT(slotInitProjectsList()));
}

void ProjectsWidget::deepTreeRun (QStandardItem *item,
                                  const QMap<QString, QSharedPointer<RedmineProject> > &prjs)
{
    if (prjs.isEmpty ())
        return;

    QMapIterator<QString, QSharedPointer<RedmineProject>> iter (prjs);
    while (iter.hasNext ())
    {
        iter.next ();
        if (iter.value ()->_parent_id == item->data (Qt::UserRole + 1).toString ())
        {
            QStandardItem *child = new QStandardItem (iter.value ()->_name);
            child->setData (iter.value ()->_id, Qt::UserRole + 1);
            deepTreeRun (child, prjs);

            item->appendRow (child);
        }
    }
}
