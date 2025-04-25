#include "svgeditor.h"

#include <QColorDialog>
#include <QEvent>
#include <QFileDialog>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QTextStream>
#include <QToolButton>
#include <QXmlStreamReader>

#include "canvas.h"
#include "element.h"
#include "manager.h"

SvgEditor::SvgEditor(QWidget* parent)
	: QMainWindow(parent), m_canvas(new Canvas(this))
{
	ui.setupUi(this);
	QHBoxLayout* hlayout = new QHBoxLayout(ui.centralWidget);
	QScrollArea* scroll = new QScrollArea(ui.centralWidget);
	scroll->setWidget(m_canvas);
	hlayout->addWidget(scroll);
	setLeftToobar();
	setTopMenuBar();
	hlayout->addWidget(getDatePanel());
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
QWidget* SvgEditor::getDatePanel()
{
	QWidget* data = new QWidget(ui.centralWidget);
	data->setFixedWidth(200);
	QVBoxLayout* dataLayout = new QVBoxLayout(data);
	QLabel* name = new QLabel(QString::fromLocal8Bit("参数设置"), data);
	name->setFixedHeight(30);
	dataLayout->addWidget(name);

	QWidget* canvasEdit = new QWidget(data);
	{
		QGridLayout* canvasLayout = new QGridLayout(canvasEdit);


		QVBoxLayout* width = new QVBoxLayout(canvasEdit);
		QLabel* widthName = new QLabel(QString::fromLocal8Bit("画布宽度"), canvasEdit);
		widthName->setFixedHeight(30);
		QLineEdit* widthEdit = new QLineEdit(canvasEdit);
		widthEdit->installEventFilter(this);
		widthEdit->setText(QString::number(m_canvas->getWidth()));
		connect(widthEdit, &QLineEdit::editingFinished, [this, widthEdit]
			{
				m_canvas->setSize(widthEdit->text().toInt(), m_canvas->getHeight());
			});
			connect(m_canvas, &Canvas::sizeChange, [widthEdit, this]
				{
					widthEdit->setText(QString::number(m_canvas->getWidth()));
				});
			width->addWidget(widthName);
			width->addWidget(widthEdit);
			canvasLayout->addLayout(width, 0, 0);

			QVBoxLayout* height = new QVBoxLayout(canvasEdit);
			QLabel* heightName = new QLabel(QString::fromLocal8Bit("画布高度"), canvasEdit);
			heightName->setFixedHeight(30);
			QLineEdit* heightEdit = new QLineEdit(canvasEdit);
			heightEdit->installEventFilter(this);
			heightEdit->setText(QString::number(m_canvas->getHeight()));
			connect(heightEdit, &QLineEdit::editingFinished, [this, heightEdit]
				{
					m_canvas->setSize(m_canvas->getWidth(), heightEdit->text().toInt());
				});
				connect(m_canvas, &Canvas::sizeChange, [heightEdit, this]
					{
						heightEdit->setText(QString::number(m_canvas->getHeight()));
					});
				height->addWidget(heightName);
				height->addWidget(heightEdit);
				canvasLayout->addLayout(height, 0, 1);

				QVBoxLayout* scale = new QVBoxLayout(canvasEdit);
				QLabel* scaleName = new QLabel(QString::fromLocal8Bit("缩放比例"), canvasEdit);
				scaleName->setFixedHeight(30);
				QLineEdit* scaleEdit = new QLineEdit(canvasEdit);
				scaleEdit->installEventFilter(this);
				scaleEdit->setText(QString::number(m_canvas->getScale() * 100));
				connect(scaleEdit, &QLineEdit::editingFinished, [this, scaleEdit]
					{
						m_canvas->setScale(scaleEdit->text().toDouble() / 100);
					});
					connect(m_canvas, &Canvas::sizeChange, [scaleEdit, this]
						{
							scaleEdit->setText(QString::number(m_canvas->getScale() * 100));
						});
					scale->addWidget(scaleName);
					scale->addWidget(scaleEdit);
					canvasLayout->addLayout(scale, 1, 0);

					QVBoxLayout* backgroundColor = new QVBoxLayout(canvasEdit);
					QLabel* colorName = new QLabel(QString::fromLocal8Bit("画布颜色"), canvasEdit);
					colorName->setFixedHeight(30);
					QPushButton* colorButton = new QPushButton(canvasEdit);
					colorButton->setStyleSheet("background-color: " + m_canvas->getBackGroundColor().name() + ";");
					connect(colorButton, &QPushButton::clicked, [colorButton, this]
						{
							QColor color = QColorDialog::getColor(Qt::white, this, QString::fromLocal8Bit("选择颜色"));
							if (color.isValid())
							{
								colorButton->setStyleSheet("background-color: " + color.name() + ";");
								m_canvas->setBackGroundColor(color);
							}
						});
					connect(m_canvas, &Canvas::backGroundColorChange, [colorButton, this]
						{
							colorButton->setStyleSheet("background-color: " + m_canvas->getBackGroundColor().name() + ";");
						});
					backgroundColor->addWidget(colorName);
					backgroundColor->addWidget(colorButton);
					canvasLayout->addLayout(backgroundColor, 1, 1);
	}
	dataLayout->addWidget(canvasEdit);

	QWidget* itemEdit = new QWidget(data);
	{
		QGridLayout* itemLayout = new QGridLayout(itemEdit);
		QVBoxLayout* penWidth = new QVBoxLayout(itemEdit);
		QLabel* penWidthName = new QLabel(QString::fromLocal8Bit("边框宽度"), itemEdit);
		penWidthName->setFixedHeight(30);
		QLineEdit* penWidthEdit = new QLineEdit(itemEdit);
		penWidthEdit->installEventFilter(this);
		connect(penWidthEdit, &QLineEdit::editingFinished, [this, penWidthEdit]
			{
				m_canvas->getManager()->setSelectedPenWidth(penWidthEdit->text().toDouble());
			});
			connect(m_canvas, &Canvas::selectedItemChanged, [penWidthEdit](std::shared_ptr<Element> item)
				{
					if (item != nullptr)
						penWidthEdit->setText(QString::number(item->getPen().widthF()));
				});
			penWidth->addWidget(penWidthName);
			penWidth->addWidget(penWidthEdit);
			itemLayout->addLayout(penWidth, 0, 0);

			QVBoxLayout* penStyle = new QVBoxLayout(itemEdit);
			QLabel* penStyleName = new QLabel(QString::fromLocal8Bit("边框样式"), itemEdit);
			penStyleName->setFixedHeight(30);
			QToolButton* penStyleButton = new QToolButton(itemEdit);
			penStyleButton->setText(QString::fromLocal8Bit("—"));
			QMenu* menu = new QMenu(this);
			QAction* solid = new QAction(QString::fromLocal8Bit("—"), this);
			QAction* dash = new QAction(QString::fromLocal8Bit("---"), this);
			QAction* dot = new QAction(QString::fromLocal8Bit("···"), this);
			QAction* dashDot = new QAction(QString::fromLocal8Bit("-·-·-"), this);
			QAction* dashDotDot = new QAction(QString::fromLocal8Bit("-··-··"), this);
			menu->addAction(solid);
			menu->addAction(dash);
			menu->addAction(dot);
			menu->addAction(dashDot);
			menu->addAction(dashDotDot);
			penStyleButton->setMenu(menu);
			penStyleButton->setPopupMode(QToolButton::InstantPopup);
			penStyleButton->setFixedWidth(75);
			connect(solid, &QAction::triggered, [penStyleButton, this]
				{
					m_canvas->getManager()->setSelectedPenStyle(Qt::PenStyle::SolidLine);
					m_canvas->update();
					penStyleButton->setText(QString::fromLocal8Bit("—"));
				});
			connect(dash, &QAction::triggered, [penStyleButton, this]
				{
					m_canvas->getManager()->setSelectedPenStyle(Qt::PenStyle::DashLine);
					m_canvas->update();
					penStyleButton->setText(QString::fromLocal8Bit("---"));
				});
			connect(dot, &QAction::triggered, [penStyleButton, this]
				{
					m_canvas->getManager()->setSelectedPenStyle(Qt::PenStyle::DotLine);
					m_canvas->update();
					penStyleButton->setText(QString::fromLocal8Bit("···"));
				});
			connect(dashDot, &QAction::triggered, [penStyleButton, this]
				{
					m_canvas->getManager()->setSelectedPenStyle(Qt::PenStyle::DashDotLine);
					m_canvas->update();
					penStyleButton->setText(QString::fromLocal8Bit("-·-"));
				});
			connect(dashDotDot, &QAction::triggered, [penStyleButton, this]
				{
					m_canvas->getManager()->setSelectedPenStyle(Qt::PenStyle::DashDotDotLine);
					m_canvas->update();
					penStyleButton->setText(QString::fromLocal8Bit("-··"));
				});
			connect(m_canvas, &Canvas::selectedItemChanged, [penStyleButton, this](std::shared_ptr<Element> item)
				{
					if (item != nullptr)
					{
						Qt::PenStyle style = item->getPen().style();
						switch (style)
						{
						case Qt::SolidLine:
							penStyleButton->setText(QString::fromLocal8Bit("—"));
							break;
						case Qt::DashLine:
							penStyleButton->setText(QString::fromLocal8Bit("---"));
							break;
						case Qt::DotLine:
							penStyleButton->setText(QString::fromLocal8Bit("···"));
							break;
						case Qt::DashDotLine:
							penStyleButton->setText(QString::fromLocal8Bit("-·-"));
							break;
						case Qt::DashDotDotLine:
							penStyleButton->setText(QString::fromLocal8Bit("-··"));
							break;
						default:
							break;
						}
					}
				});
			penStyle->addWidget(penStyleName);
			penStyle->addWidget(penStyleButton);
			itemLayout->addLayout(penStyle, 0, 1);

			QVBoxLayout* penColor = new QVBoxLayout(itemEdit);
			QLabel* penColorName = new QLabel(QString::fromLocal8Bit("边框颜色"), itemEdit);
			penColorName->setFixedHeight(30);
			QPushButton* penColorButton = new QPushButton(itemEdit);
			connect(penColorButton, &QPushButton::clicked, [penColorButton, this]
				{
					QColor color = QColorDialog::getColor(Qt::black, this, QString::fromLocal8Bit("选择颜色"));
					if (color.isValid())
					{
						penColorButton->setStyleSheet("background-color: " + color.name() + ";");
						m_canvas->getManager()->setSelectedPenColor(color);
					}
				});
			connect(m_canvas, &Canvas::selectedItemChanged, [penColorButton, this](std::shared_ptr<Element> item)
				{
					if (item != nullptr)
						penColorButton->setStyleSheet("background-color: " + item->getPen().color().name() + ";");
				});
			penColor->addWidget(penColorName);
			penColor->addWidget(penColorButton);
			itemLayout->addLayout(penColor, 1, 0);

			QVBoxLayout* fillColor = new QVBoxLayout(itemEdit);
			QLabel* fillColorName = new QLabel(QString::fromLocal8Bit("填充颜色"), itemEdit);
			fillColorName->setFixedHeight(30);
			QPushButton* fillColorButton = new QPushButton(itemEdit);
			connect(fillColorButton, &QPushButton::clicked, [fillColorButton, this]
				{
					QColor color = QColorDialog::getColor(Qt::white, this, QString::fromLocal8Bit("选择颜色"));
					if (color.isValid())
					{
						fillColorButton->setStyleSheet("background-color: " + color.name() + ";");
						m_canvas->getManager()->setSelectedBrushColor(color);
					}
				});
			connect(m_canvas, &Canvas::selectedItemChanged, [fillColorButton, this](std::shared_ptr<Element> item)
				{
					if (item != nullptr)
					{
						if (item->getBrush().color() != Qt::transparent)
							fillColorButton->setStyleSheet("background-color: " + item->getBrush().color().name() + ";");
						else
							fillColorButton->setStyleSheet("");
					}
				});
			fillColor->addWidget(fillColorName);
			fillColor->addWidget(fillColorButton);
			itemLayout->addLayout(fillColor, 1, 1);
	}
	itemEdit->setVisible(false);
	dataLayout->addWidget(itemEdit);
	connect(m_canvas, &Canvas::selectedItemChanged, [itemEdit](std::shared_ptr<Element> item)
		{
			if (item == nullptr)
			{
				itemEdit->setVisible(false);
			}
			else
			{
				itemEdit->setVisible(true);
			}
		});
	dataLayout->addStretch();
	return data;
}
void SvgEditor::newFile()
{
	m_canvas->reset();
}
void SvgEditor::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this
		, QString::fromLocal8Bit("打开文件")
		, QDir::currentPath()
		, QString::fromLocal8Bit("(*.svg)"));
	if (fileName.isEmpty())
		return;
	newFile();
	QFile file(fileName);
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
				QString pathStr = reader.attributes().value("d").toString().toUpper();
				pathStr.remove(" ");
				int n = pathStr.indexOf("L");
				QString str = pathStr.mid(1, n - 1);
				int i = str.indexOf(",");
				path.moveTo(QPointF(str.mid(0, i).toDouble(), str.mid(i + 1).toDouble()));
				pathStr.remove(0, n + 1);
				while (pathStr.indexOf("L") != -1)
				{
					n = pathStr.indexOf("L");
					str = pathStr.left(n);
					i = str.indexOf(",");
					path.lineTo(QPointF(str.mid(0, i).toDouble(), str.mid(i + 1).toDouble()));
					pathStr.remove(0, n + 1);
				}
				if (pathStr.indexOf("Z") != -1)
				{
					pathStr.remove("Z");
					i = pathStr.indexOf(",");
					path.lineTo(QPointF(pathStr.mid(0, i).toDouble(), pathStr.mid(i + 1).toDouble()));
					path.closeSubpath();
				}
				else
				{
					i = pathStr.indexOf(",");
					path.lineTo(QPointF(pathStr.mid(0, i).toDouble(), pathStr.mid(i + 1).toDouble()));
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
				qreal strokeWidth = reader.attributes().value("stroke-width").toString().toDouble();
				QString strokeDashArray = reader.attributes().value("stroke-dasharray").toString();
				Qt::PenStyle style = Qt::PenStyle::SolidLine;
				if (strokeDashArray == "10,5")
					style = Qt::PenStyle::DashLine;
				if (strokeDashArray == "1,5")
					style = Qt::PenStyle::DotLine;
				if (strokeDashArray == "10,5,1,5")
					style = Qt::PenStyle::DashDotLine;
				if (strokeDashArray == "10,5,1,5,1,5")
					style = Qt::PenStyle::DashDotDotLine;
				QPen pen(QColor(stroke), strokeWidth, style);
				QBrush brush = QBrush(QColor(reader.attributes().value("fill").toString()));
				m_canvas->getManager()->createItem(type, rect, path, pen, brush);
			}
		}
	}
}
void SvgEditor::saveFile()
{
	QString fileName = QFileDialog::getSaveFileName(this
		, QString::fromLocal8Bit("保存文件")
		, QDir::currentPath()
		, QString::fromLocal8Bit("(*.svg)"));
	if (fileName.isEmpty())
		return;
	QFile file(fileName);
	file.open(QIODevice::WriteOnly);
	QTextStream out(&file);
	out << QString::fromStdString(m_canvas->toSvgText());
	file.close();
}
void SvgEditor::saveFileToPng()
{
	QString fileName = QFileDialog::getSaveFileName(this
		, QString::fromLocal8Bit("保存文件")
		, QDir::currentPath()
		, QString::fromLocal8Bit("(*.png)"));
	if (fileName.isEmpty())
		return;
	QPixmap pixMap(m_canvas->getWidth(), m_canvas->getHeight());
	QPainter painter(&pixMap);
	painter.scale(1 / m_canvas->getScale(), 1 / m_canvas->getScale());
	m_canvas->getManager()->cancelSelected();
	m_canvas->render(&painter);
	pixMap.save(fileName);
}
bool SvgEditor::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		switch (keyEvent->key())
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
{
}
