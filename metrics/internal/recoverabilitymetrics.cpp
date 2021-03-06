#include "recoverabilitymetrics.h"

#include "redmineinstance.h"
#include "charts/drilldownchart.h"
#include "charts/drilldownslice.h"

#include <QDebug>
#include <QHBoxLayout>

#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QLegend>
#include <QtCharts/QPieSeries>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

RecoverabilityMetrics::RecoverabilityMetrics (const QString &prjid, QWidget *parent)
    : QWidget (parent)
{
    if (RedmineInstance::instance ().projects ().contains (prjid) &&
            RedmineInstance::instance ().loadIssues (prjid) &&
            RedmineInstance::instance ().loadAttachments ())
    {
        setWindowTitle (trUtf8 ("Внутренняя метрика восстанавливаемости"));

        QVBoxLayout *vl = new QVBoxLayout ();
        vl->setContentsMargins (4,4,4,4);
        setLayout (vl);

        //-- Восстанавливаемость
        {
            QPieSeries *series = new QPieSeries ();

            //-------------- Число предотвращённых исключений
            series->append (trUtf8 ("Выполненные требования к восстанавл-ти"), 1);

            int A = RedmineInstance::instance ().metric6A (prjid);

            QPieSlice *slice = series->slices ().at (0);
            slice->setValue (A);
            slice->setColor (QColor (147, 112, 219));
            slice->setLabel (trUtf8 (("Выполненные требования к восстанавливаемости - %1")).arg (QString::number (A)));

            {
                QFontMetrics fm (slice->labelFont ());
                if (_maxw < fm.width (trUtf8 (("Выполненные требования к восстанавливаемости - 666"))))
                    _maxw = fm.width (trUtf8 (("Выполненные требования к восстанавливаемости - 666")));
                _maxh += fm.height ();
            }

            //-------------- Число Не предотвращённых исключений
            series->append (trUtf8 ("Не выполненные требования к восстанавливаемости"), 2);

            int B = RedmineInstance::instance ().metric6B (prjid);

            slice = series->slices ().at (1);
            slice->setValue (B - A);
            slice->setLabel (trUtf8 ("Не выполненные требования к восстанавливаемости - %1").arg (QString::number (B - A)));
            slice->setExploded (true);
            slice->setColor (QColor (72, 61, 139));
            slice->setBorderColor (Qt::red);
            slice->setBorderWidth (3);

            {
                QFontMetrics fm (slice->labelFont ());
                if (_maxw < fm.width (trUtf8 ("Не выполненные требования к восстанавливаемости - 666")))
                    _maxw = fm.width (trUtf8 ("Не выполненные требования к восстанавливаемости - 666"));
                _maxh += fm.height ();
            }

            //--------------

            _chart = new QChart ();
            _chart->addSeries (series);
            _chart->setTitle (trUtf8 ("<font size=6><b>Восстанавливаемость</b></font>"));
            _chart->setAnimationOptions (QChart::AllAnimations);
            _chart->legend ()->setVisible (true);
            _chart->legend ()->setAlignment (Qt::AlignRight);
            _chart->setToolTip(trUtf8("Насколько способна система восстановиться до полной работы после некорректного события или запроса?"));

            _chart->legend ()->detachFromChart ();
            _chart->legend ()->setBackgroundVisible (true);
            _chart->legend ()->setAlignment (Qt::AlignLeft);

            QFont f = _chart->legend ()->font ();
            f.setPixelSize (14);
            _chart->legend ()->setFont (f);

            QChartView *chartView = new QChartView (_chart);
            chartView->setRenderHint (QPainter::Antialiasing);
            vl->addWidget (chartView);
        }
    }
}

void RecoverabilityMetrics::resizeEvent (QResizeEvent *event)
{
    (void)event;

    if (!_chart)
        return;

    _chart->legend ()->setGeometry (20, geometry ().bottom () - _maxh - 80, _maxw + 50, _maxh + 30);
}

