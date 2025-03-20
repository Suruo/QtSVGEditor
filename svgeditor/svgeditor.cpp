#include "svgeditor.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QColorDialog>
#include <QFileDialog>
#include <QPixmap>
#include <QXmlStreamReader>
#include <QTextStream>
#include <QScrollArea>
#include <QEvent>
#include <QKeyEvent>

#include "canvas.h"
#include "element.h"
#include "manager.h"

SvgEditor::SvgEditor(QWidget *parent)
    : QMainWindow(parent), m_canvas(new Canvas(this))
{
    ui.setupUi(this);
    QHBoxLayout* hlayout = new QHBoxLayout(ui.centralWidget);
    QScrollArea* scroll = new QScrollArea(ui.centralWidget);
    scroll->setWidget(m_canvas);
    hlayout->addWidget(scroll);
    setLeftToobar();
    setTopMenuBar();
    hlayout->addWidget(DateSetting());
    resize(1080, 720);
}
void SvgEditor::setLeftToobar()
{
    addToolBar(Qt::LeftToolBarArea, ui.mainToolBar);
    ui.mainToolBar->setMovable(false);
    ui.mainToolBar->setFixedWidth(50);
    QActionGroup* group = new QActionGroup(this);
    group->setExclusive(true);
    Type button[8]{ Type::None, Type::Path, Type::Line, Type::Rect, Type::Ellipse, Type::Pentagon, Type::Hexagon, Type::Star };
    QString iconpath[8]{ ":\\SvgEditor\\Icon\\Mouse.png",":\\SvgEditor\\Icon\\Path.png"
        , ":\\SvgEditor\\Icon\\Line.png", ":\\SvgEditor\\Icon\\Rect.png"
        , ":\\SvgEditor\\Icon\\Ellipse.png", ":\\SvgEditor\\Icon\\Pentagon.png"
        , ":\\SvgEditor\\Icon\\Hexagon.png", ":\\SvgEditor\\Icon\\Star.png" };
    QAction* mouse = new QAction(this);
    mouse->setIcon(QIcon(iconpath[0]));
    mouse->setCheckable(true);
    group->addAction(mouse);
    ui.mainToolBar->addAction(mouse);
    connect(mouse, &QAction::triggered, [button, this]
        {
            this->m_canvas->setItemType(button[0]);
        });
    connect(m_canvas, &Canvas::toTypeNone, [mouse, this]
        {
            emit mouse->triggered();
            mouse->setChecked(true);
        });
    for (int i = 1; i < 8; ++i)
    {
        QAction* action = new QAction(this);
        action->setIcon(QIcon(iconpath[i]));
        action->setCheckable(true);
        group->addAction(action);
        ui.mainToolBar->addAction(action);
        connect(action, &QAction::triggered, [button, this, i]
            {
                this->m_canvas->setItemType(button[i]);
            });
    }
}
void SvgEditor::setTopMenuBar()
{
    QMenu* menu = ui.menuBar->addMenu(QString::fromLocal8Bit("文件"));
    QAction* newfile = menu->addAction(QString::fromLocal8Bit("新建文件"));
    connect(newfile, &QAction::triggered, this, &SvgEditor::newFile);
    newfile->setShortcut(Qt::CTRL + Qt::Key_N);
    QAction* openfile = menu->addAction(QString::fromLocal8Bit("打开SVG"));
    connect(openfile, &QAction::triggered, this, &SvgEditor::openFile);
    openfile->setShortcut(Qt::CTRL + Qt::Key_O);
    QAction* savefile = menu->addAction(QString::fromLocal8Bit("保存"));
    connect(savefile, &QAction::triggered, this, &SvgEditor::saveFile);
    savefile->setShortcut(Qt::CTRL + Qt::Key_S);
    QAction* topng = menu->addAction(QString::fromLocal8Bit("导出PNG"));
    connect(topng, &QAction::triggered, this, &SvgEditor::saveFileToPng);
    topng->setShortcut(Qt::CTRL + Qt::Key_E);
}
QWidget* SvgEditor::DateSetting()
{
    QWidget* data = new QWidget(ui.centralWidget);
    data->setFixedWidth(200);
    QVBoxLayout* datalayout = new QVBoxLayout(data);
    QLabel* name = new QLabel(QString::fromLocal8Bit("参数设置"), data);
    name->setFixedHeight(30);
    datalayout->addWidget(name);

    QWidget* canvasedit = new QWidget(data);
    {
        QGridLayout* canvaslayout = new QGridLayout(canvasedit);
        

        QVBoxLayout* width = new QVBoxLayout(canvasedit);
        QLabel* widthname = new QLabel(QString::fromLocal8Bit("画布宽度"), canvasedit);
        widthname->setFixedHeight(30);
        QLineEdit* widthedit = new QLineEdit(canvasedit);
        widthedit->installEventFilter(this);
        widthedit->setText(QString::number(m_canvas->getWidth()));
        connect(widthedit, &QLineEdit::editingFinished, [this, widthedit]
            {
                m_canvas->setSize(widthedit->text().toInt(), m_canvas->getHeight());
            });
        connect(m_canvas, &Canvas::sizeChange, [widthedit, this]
            {
                widthedit->setText(QString::number(m_canvas->getWidth()));
            });
        width->addWidget(widthname);
        width->addWidget(widthedit);
        canvaslayout->addLayout(width, 0, 0);

        QVBoxLayout* height = new QVBoxLayout(canvasedit);
        QLabel* heightname = new QLabel(QString::fromLocal8Bit("画布高度"), canvasedit);
        heightname->setFixedHeight(30);
        QLineEdit* heightedit = new QLineEdit(canvasedit);
        heightedit->installEventFilter(this);
        heightedit->setText(QString::number(m_canvas->getHeight()));
        connect(heightedit, &QLineEdit::editingFinished, [this, heightedit]
            {
                m_canvas->setSize(m_canvas->getWidth(), heightedit->text().toInt());
            });
        connect(m_canvas, &Canvas::sizeChange, [heightedit, this]
            {
                heightedit->setText(QString::number(m_canvas->getHeight()));
            });
        height->addWidget(heightname);
        height->addWidget(heightedit);
        canvaslayout->addLayout(height, 0, 1);

        QVBoxLayout* scale = new QVBoxLayout(canvasedit);
        QLabel* scalename = new QLabel(QString::fromLocal8Bit("缩放比例"), canvasedit);
        scalename->setFixedHeight(30);
        QLineEdit* scaleedit = new QLineEdit(canvasedit);
        scaleedit->installEventFilter(this);
        scaleedit->setText(QString::number(m_canvas->getScale() * 100));
        connect(scaleedit, &QLineEdit::editingFinished, [this, scaleedit]
            {
                m_canvas->setScale(scaleedit->text().toDouble() / 100);
            });
        connect(m_canvas, &Canvas::sizeChange, [scaleedit, this]
            {
                scaleedit->setText(QString::number(m_canvas->getScale() * 100));
            });
        scale->addWidget(scalename);
        scale->addWidget(scaleedit);
        canvaslayout->addLayout(scale, 1, 0);

        QVBoxLayout* backcolor = new QVBoxLayout(canvasedit);
        QLabel* colorname = new QLabel(QString::fromLocal8Bit("画布颜色"), canvasedit);
        colorname->setFixedHeight(30);
        QPushButton* colorbutton = new QPushButton(canvasedit);
        colorbutton->setStyleSheet("background-color: " + m_canvas->getBackGroundColor().name() + ";");
        connect(colorbutton, &QPushButton::clicked, [colorbutton, this]
            {
                QColor color = QColorDialog::getColor(Qt::white, this, QString::fromLocal8Bit("选择颜色"));
                if (color.isValid())
                {
                    colorbutton->setStyleSheet("background-color: " + color.name() + ";");
                    m_canvas->setBackGroundColor(color);
                }
            });
        connect(m_canvas, &Canvas::backGroundColorChange, [colorbutton, this]
            {
                colorbutton->setStyleSheet("background-color: " + m_canvas->getBackGroundColor().name() + ";");
            });
        backcolor->addWidget(colorname);
        backcolor->addWidget(colorbutton);
        canvaslayout->addLayout(backcolor, 1, 1);
    }
    datalayout->addWidget(canvasedit);
    
    QWidget* itemedit = new QWidget(data);
    {
        QGridLayout* itemlayout = new QGridLayout(itemedit);
        QVBoxLayout* penwidth = new QVBoxLayout(itemedit);
        QLabel* penwidthname = new QLabel(QString::fromLocal8Bit("边框宽度"), itemedit);
        penwidthname->setFixedHeight(30);
        QLineEdit* penwidthedit = new QLineEdit(itemedit);
        penwidthedit->installEventFilter(this);
        connect(penwidthedit, &QLineEdit::editingFinished, [this, penwidthedit]
            {
                m_canvas->getManager()->setSelectedPenWidth(penwidthedit->text().toDouble());
            });
        connect(m_canvas, &Canvas::selectedItemChanged, [penwidthedit](std::shared_ptr<Element> item)
            {
                if (item != nullptr)
                    penwidthedit->setText(QString::number(item->getPen().widthF()));
            });
        penwidth->addWidget(penwidthname);
        penwidth->addWidget(penwidthedit);
        itemlayout->addLayout(penwidth, 0, 0);

        QVBoxLayout* penstyle = new QVBoxLayout(itemedit);
        QLabel* penstylename = new QLabel(QString::fromLocal8Bit("边框样式"), itemedit);
        penstylename->setFixedHeight(30);
        QToolButton* penstylebutton = new QToolButton(itemedit);
        penstylebutton->setText(QString::fromLocal8Bit("—"));
        QMenu* menu = new QMenu(this);
        QAction* solid = new QAction(QString::fromLocal8Bit("—"), this);
        QAction* dash = new QAction(QString::fromLocal8Bit("---"), this);
        QAction* dot = new QAction(QString::fromLocal8Bit("···"), this);
        QAction* dashdot = new QAction(QString::fromLocal8Bit("-·-·-"), this);
        QAction* dashdotdot = new QAction(QString::fromLocal8Bit("-··-··"), this);
        menu->addAction(solid);
        menu->addAction(dash);
        menu->addAction(dot);
        menu->addAction(dashdot);
        menu->addAction(dashdotdot);
        penstylebutton->setMenu(menu);
        penstylebutton->setPopupMode(QToolButton::InstantPopup);
        penstylebutton->setFixedWidth(75);
        connect(solid, &QAction::triggered, [penstylebutton, this]
            {
                m_canvas->getManager()->setSelectedPenStyle(Qt::PenStyle::SolidLine);
                m_canvas->update();
                penstylebutton->setText(QString::fromLocal8Bit("—"));
            });
        connect(dash, &QAction::triggered, [penstylebutton, this]
            {
                m_canvas->getManager()->setSelectedPenStyle(Qt::PenStyle::DashLine);
                m_canvas->update();
                penstylebutton->setText(QString::fromLocal8Bit("---"));
            });
        connect(dot, &QAction::triggered, [penstylebutton, this]
            {
                m_canvas->getManager()->setSelectedPenStyle(Qt::PenStyle::DotLine);
                m_canvas->update();
                penstylebutton->setText(QString::fromLocal8Bit("···"));
            });
        connect(dashdot, &QAction::triggered, [penstylebutton, this]
            {
                m_canvas->getManager()->setSelectedPenStyle(Qt::PenStyle::DashDotLine);
                m_canvas->update();
                penstylebutton->setText(QString::fromLocal8Bit("-·-"));
            });
        connect(dashdotdot, &QAction::triggered, [penstylebutton, this]
            {
                m_canvas->getManager()->setSelectedPenStyle(Qt::PenStyle::DashDotDotLine);
                m_canvas->update();
                penstylebutton->setText(QString::fromLocal8Bit("-··"));
            });
        connect(m_canvas, &Canvas::selectedItemChanged, [penstylebutton, this](std::shared_ptr<Element> item)
            {
                if (item != nullptr)
                {
                    Qt::PenStyle style = item->getPen().style();
                    switch (style)
                    {
                    case Qt::SolidLine:
                        penstylebutton->setText(QString::fromLocal8Bit("—"));
                        break;
                    case Qt::DashLine:
                        penstylebutton->setText(QString::fromLocal8Bit("---"));
                        break;
                    case Qt::DotLine:
                        penstylebutton->setText(QString::fromLocal8Bit("···"));
                        break;
                    case Qt::DashDotLine:
                        penstylebutton->setText(QString::fromLocal8Bit("-·-"));
                        break;
                    case Qt::DashDotDotLine:
                        penstylebutton->setText(QString::fromLocal8Bit("-··"));
                        break;
                    default:
                        break;
                    }
                }
            });
        penstyle->addWidget(penstylename);
        penstyle->addWidget(penstylebutton);
        itemlayout->addLayout(penstyle, 0, 1);

        QVBoxLayout* pencolor = new QVBoxLayout(itemedit);
        QLabel* pencolorname = new QLabel(QString::fromLocal8Bit("边框颜色"), itemedit);
        pencolorname->setFixedHeight(30);
        QPushButton* pencolorbutton = new QPushButton(itemedit);
        connect(pencolorbutton, &QPushButton::clicked, [pencolorbutton, this]
            {
                QColor color = QColorDialog::getColor(Qt::black, this, QString::fromLocal8Bit("选择颜色"));
                if (color.isValid())
                {
                    pencolorbutton->setStyleSheet("background-color: " + color.name() + ";");
                    m_canvas->getManager()->setSelectedPenColor(color);
                }
            });
        connect(m_canvas, &Canvas::selectedItemChanged, [pencolorbutton, this](std::shared_ptr<Element> item)
            {
                if (item != nullptr)
                    pencolorbutton->setStyleSheet("background-color: " + item->getPen().color().name() + ";");
            });
        pencolor->addWidget(pencolorname);
        pencolor->addWidget(pencolorbutton);
        itemlayout->addLayout(pencolor, 1, 0);

        QVBoxLayout* fillcolor = new QVBoxLayout(itemedit);
        QLabel* fillcolorname = new QLabel(QString::fromLocal8Bit("填充颜色"), itemedit);
        fillcolorname->setFixedHeight(30);
        QPushButton* fillcolorbutton = new QPushButton(itemedit);
        connect(fillcolorbutton, &QPushButton::clicked, [fillcolorbutton, this]
            {
                QColor color = QColorDialog::getColor(Qt::white, this, QString::fromLocal8Bit("选择颜色"));
                if (color.isValid())
                {
                    fillcolorbutton->setStyleSheet("background-color: " + color.name() + ";");
                    m_canvas->getManager()->setSelectedBrushColor(color);
                }
            });
        connect(m_canvas, &Canvas::selectedItemChanged, [fillcolorbutton, this](std::shared_ptr<Element> item)
            {
                if (item != nullptr)
                {
                    if (item->getBrush().color() != Qt::transparent)
                        fillcolorbutton->setStyleSheet("background-color: " + item->getBrush().color().name() + ";");
                    else
                        fillcolorbutton->setStyleSheet("");
                }
            });
        fillcolor->addWidget(fillcolorname);
        fillcolor->addWidget(fillcolorbutton);
        itemlayout->addLayout(fillcolor, 1, 1);
    }
    itemedit->setVisible(false);
    datalayout->addWidget(itemedit);
    connect(m_canvas, &Canvas::selectedItemChanged, [itemedit](std::shared_ptr<Element> item)
        {
            if (item == nullptr)
            {
                itemedit->setVisible(false);
            }
            else
            {
                itemedit->setVisible(true);
            }
        });
    datalayout->addStretch();
    return data;
}
void SvgEditor::newFile()
{
    m_canvas->reset();
}
void SvgEditor::openFile()
{
    QString filename = QFileDialog::getOpenFileName(this
        , QString::fromLocal8Bit("打开文件")
        , QDir::currentPath()
        , QString::fromLocal8Bit("(*.svg)"));
    if (filename.isEmpty())
        return;
    newFile();
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    parseSvg(&file);
    file.close();
}
void SvgEditor::parseSvg(QFile* file)
{
    QXmlStreamReader reader(file);
    while (!reader.atEnd() && !reader.hasError())
    {
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartElement)
        {
            QRectF rect;
            QPainterPath path;
            Type type = Type::None;
            if (reader.name() == "svg")
            {
                m_canvas->setSize(reader.attributes().value("width").toString().toInt()
                    , reader.attributes().value("height").toString().toInt());
            }
            if (reader.name() == "line")
            {
                qreal x1 = reader.attributes().value("x1").toString().toDouble();
                qreal y1 = reader.attributes().value("y1").toString().toDouble();
                qreal x2 = reader.attributes().value("x2").toString().toDouble();
                qreal y2 = reader.attributes().value("y2").toString().toDouble();
                rect.setTopLeft(QPointF(x1, y1));
                rect.setBottomRight(QPointF(x2, y2));
                type = Type::Line;
            }
            if (reader.name() == "path")
            {
                QString pathstr = reader.attributes().value("d").toString().toUpper();
                pathstr.remove(" ");
                int n = pathstr.indexOf("L");
                QString str = pathstr.mid(1, n - 1);
                int i = str.indexOf(",");
                path.moveTo(QPointF(str.mid(0, i).toDouble(), str.mid(i + 1).toDouble()));
                pathstr.remove(0, n + 1);
                while (pathstr.indexOf("L") != -1)
                {
                    n = pathstr.indexOf("L");
                    str = pathstr.left(n);
                    i = str.indexOf(",");
                    path.lineTo(QPointF(str.mid(0, i).toDouble(), str.mid(i + 1).toDouble()));
                    pathstr.remove(0, n + 1);
                }
                if (pathstr.indexOf("Z") != -1)
                {
                    pathstr.remove("Z");
                    i = pathstr.indexOf(",");
                    path.lineTo(QPointF(pathstr.mid(0, i).toDouble(), pathstr.mid(i + 1).toDouble()));
                    path.closeSubpath();
                }
                else
                {
                    i = pathstr.indexOf(",");
                    path.lineTo(QPointF(pathstr.mid(0, i).toDouble(), pathstr.mid(i + 1).toDouble()));
                }
                rect = path.boundingRect();
                type = Type::Path;
            }
            if (reader.name() == "ellipse")
            {
                qreal cx = reader.attributes().value("cx").toString().toDouble();
                qreal cy = reader.attributes().value("cy").toString().toDouble();
                qreal rx = reader.attributes().value("rx").toString().toDouble();
                qreal ry = reader.attributes().value("ry").toString().toDouble();
                rect = QRectF(cx - rx, cy - ry, 2 * rx, 2 * ry);
                type = Type::Ellipse;
            }
            if (reader.name() == "rect")
            {
                qreal x = reader.attributes().value("x").toString().toDouble();
                qreal y = reader.attributes().value("y").toString().toDouble();
                qreal width = reader.attributes().value("width").toString().toDouble();
                qreal height = reader.attributes().value("height").toString().toDouble();
                rect = QRectF(x, y, width, height);
                type = Type::Rect;
            }
            if (type != Type::None)
            {
                QString stroke = reader.attributes().value("stroke").toString();
                qreal strokewidth = reader.attributes().value("stroke-width").toString().toDouble();
                QString strokedasharray = reader.attributes().value("stroke-dasharray").toString();
                Qt::PenStyle style = Qt::PenStyle::SolidLine;
                if (strokedasharray == "10,5")
                    style = Qt::PenStyle::DashLine;
                if (strokedasharray == "1,5")
                    style = Qt::PenStyle::DotLine;
                if (strokedasharray == "10,5,1,5")
                    style = Qt::PenStyle::DashDotLine;
                if (strokedasharray == "10,5,1,5,1,5")
                    style = Qt::PenStyle::DashDotDotLine;
                QPen pen(QColor(stroke), strokewidth, style);
                QBrush brush = QBrush(QColor(reader.attributes().value("fill").toString()));
                m_canvas->getManager()->createItem(type, rect, path, pen, brush);
            }
        }
    }
}
void SvgEditor::saveFile()
{
    QString filename = QFileDialog::getSaveFileName(this
        , QString::fromLocal8Bit("保存文件")
        , QDir::currentPath()
        , QString::fromLocal8Bit("(*.svg)"));
    if (filename.isEmpty())
        return;
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out << QString::fromStdString(m_canvas->toSvgText());
    file.close();
}
void SvgEditor::saveFileToPng()
{
    QString filename = QFileDialog::getSaveFileName(this
        , QString::fromLocal8Bit("保存文件")
        , QDir::currentPath()
        , QString::fromLocal8Bit("(*.png)"));
    if (filename.isEmpty())
        return;
    QPixmap pixmap(m_canvas->getWidth(), m_canvas->getHeight());
    QPainter painter(&pixmap);
    painter.scale(1 / m_canvas->getScale(), 1 / m_canvas->getScale());
    m_canvas->getManager()->cancelSelected();
    m_canvas->render(&painter);
    pixmap.save(filename);
}
bool SvgEditor::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyevent = static_cast<QKeyEvent*>(event);
        switch (keyevent->key())
        {
        case Qt::Key_0:
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:
        case Qt::Key_Backspace:
            return false;
        default:
            return true;
            break;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}
SvgEditor::~SvgEditor()
{}
