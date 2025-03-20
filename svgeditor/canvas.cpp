#include "canvas.h"

#include <QMenu>
#include <QPainter>
#include <QPalette>
#include <QShortCut>

#include "command.h"

Canvas::Canvas(QWidget* parent = Q_NULLPTR)
	: QWidget(parent)
	, m_manager(std::make_shared<Manager>())
	, m_pressed(false)
	, m_creating(false)
	, m_resizing(false)
	, m_moving(false)
	, m_movestart(QPointF())
	, m_type(Type::None)
	, m_edge(Edge::NoEdge)
	, m_scale(1)
	, m_history(CommandHistory::getInstance())
	, m_rubberband(new QRubberBand(QRubberBand::Rectangle, this))
{
	setMouseTracking(true);
	setFixedSize(700, 500);
	setAutoFillBackground(true);
	setPalette(QPalette(QPalette::Window, Qt::white));
}
Canvas::~Canvas()
{
}
void Canvas::setItemType(Type type)
{
	m_type = type;
	if (m_type == Type::None)
		setCursor(Qt::ArrowCursor);
}
void Canvas::setSize(int width, int height)
{
	m_history.addCommand(std::make_shared<ChangeCanvasSize>(this, QSize(width, height)));
	setFixedSize(static_cast<int>(width * m_scale), static_cast<int>(height * m_scale));
}
void Canvas::setScale(double scale)
{
	if (scale != 0)
	{
		m_history.addCommand(std::make_shared<ChangeScale>(this, scale));
		setFixedSize(static_cast<int>(size().width() * scale / m_scale), static_cast<int>(size().height() * scale / m_scale));
		m_scale = scale;
		update();
	}
}
void Canvas::setBackGroundColor(const QColor& color)
{
	m_history.addCommand(std::make_shared<ChangeBackGroundColor>(this, color));
	setPalette(QPalette(QPalette::Window, color));
}
int Canvas::getWidth() const
{
	return static_cast<int>(size().width() / m_scale);
}
int Canvas::getHeight() const
{
	return static_cast<int>(size().height() / m_scale);
}
double Canvas::getScale() const
{
	return m_scale;
}
double& Canvas::getScaleRefer()
{
	return m_scale;
}
const QColor& Canvas::getBackGroundColor() const
{
	return palette().color(QPalette::Active, QPalette::Window);
}
std::shared_ptr<Element> Canvas::getSelectedItem() const
{
	return m_manager->getSelectedItem();
}
std::shared_ptr<Manager> Canvas::getManager() const
{
	return m_manager;
}
void Canvas::reset()
{
	m_manager = std::make_shared<Manager>();
	m_history.clearAll();
	setFixedSize(700, 500);
	setPalette(QPalette(QPalette::Window, Qt::white));
	emit backGroundColorChange();
	emit sizeChange();
	emit selectedItemChanged(nullptr);
	update();
}
std::string Canvas::toSvgText() const
{
	if (getBackGroundColor() != Qt::white)
	{
		std::string backgroud = "\t<rect width=\"100%\" height=\"100%\" fill = \"" + getBackGroundColor().name().toStdString() + "\"/>\n";
		return "<svg width=\"" + std::to_string(getWidth()) + "\" height=\"" + std::to_string(getHeight())
			+ "\" xmlns=\"http://www.w3.org/2000/svg\">\n" + backgroud + m_manager->toSvgElements() + "</svg>";
	}
	else
	{
		return "<svg width=\"" + std::to_string(getWidth()) + "\" height=\"" + std::to_string(getHeight())
			+ "\" xmlns=\"http://www.w3.org/2000/svg\">\n" + m_manager->toSvgElements() + "</svg>";
	}
}
	

void Canvas::selectAll()
{
	m_manager->selectAll();
}
void Canvas::copy(const QPointF& pos)
{
	m_manager->copy(pos);
}
void Canvas::cut(const QPointF& pos)
{
	m_manager->copy(pos);
	m_manager->removeItem();
}
void Canvas::paste(const QPointF& pos)
{
	m_manager->paste(pos);
}
void Canvas::replace(const QPointF& pos)
{
	m_manager->removeItem();
	m_manager->paste(pos);
}
void Canvas::deleteItem()
{
	m_manager->removeItem();
}
void Canvas::upLayer()
{
	m_manager->upLayer();
}
void Canvas::downLayer()
{
	m_manager->downLayer();
}
void Canvas::upMost()
{
	m_manager->upMost();
}
void Canvas::downMost()
{
	m_manager->downMost();
}
void Canvas::undo()
{
	m_history.undo();
}
void Canvas::redo()
{
	m_history.redo();
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
		leftButtonPressed(event->localPos() / m_scale);
	update();
	emit selectedItemChanged(m_manager->getSelectedItem());
	return QWidget::mousePressEvent(event);
}
void Canvas::mouseMoveEvent(QMouseEvent* event)
{
	mouseMoving(event->localPos() / m_scale);
	update();
	return QWidget::mouseMoveEvent(event);
}
void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
	m_pressed = false;
	m_creating = false;
	m_resizing = false;
	m_moving = false;
	m_rubberband->hide();
	return QWidget::mouseReleaseEvent(event);
}
void Canvas::leftButtonPressed(const QPointF& pos)
{
	m_pressed = true;
	if (m_type != Type::None)
	{
		m_manager->cancelSelected();
		m_manager->addItem(m_type, pos);
		m_creating = true;
	}
	else if (!m_manager->isItemAt(pos))
	{
		m_manager->cancelSelected();
		m_movestart = pos;
		m_rubberband->setGeometry(QRect((m_movestart * m_scale).toPoint(), QSize()));
		m_rubberband->show();
	}
	else if (!m_manager->isAnyOneSelected() || !m_manager->isOnlyOneSelected())
	{
		if (!m_manager->isAnyOneSelected())
			m_manager->selectItemAt(pos);
		m_moving = true;
		m_movestart = pos;
	}
	else
	{
		m_manager->selectItemAt(pos);
		if (m_edge != Edge::NoEdge)
		{
			m_resizing = true;
		}
		else
		{
			m_moving = true;
			m_movestart = pos;
		}
	}
}
void Canvas::mouseMoving(const QPointF& pos)
{
	if (!m_pressed)
	{
		if (m_type == Type::None)
		{
			m_edge = m_manager->recognizeMousePos(pos);
			changeCursor(m_edge);
		}
		else
		{
			setCursor(Qt::CrossCursor);
		}
	}
	else if (m_creating)
	{
		m_manager->drawItemShape(pos);
	}
	else if (m_resizing)
	{
		m_manager->changeItemShape(m_edge, pos);
	}
	else if (m_moving)
	{
		m_manager->moveItem(m_movestart, pos);
		m_movestart = pos;
	}
	else if (m_rubberband->isVisible())
	{
		m_rubberband->setGeometry(QRect((m_movestart * m_scale).toPoint(), (pos * m_scale).toPoint()).normalized());
		m_manager->selectItems(QRectF(m_movestart, pos));
	}
}
void Canvas::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.scale(m_scale, m_scale);
	m_manager->paint(&painter);
}
void Canvas::contextMenuEvent(QContextMenuEvent* event)
{
	if (m_type == Type::None)
		setRightButtonMenu(event);
	else
		emit toTypeNone();
	update();
	return QWidget::contextMenuEvent(event);
}
void Canvas::setRightButtonMenu(QContextMenuEvent* event)
{
	QPointF pos(event->pos().x() / m_scale, event->pos().y() / m_scale);
	QMenu menu(this);
	QAction* selectall = menu.addAction(QString::fromLocal8Bit("全选"));
	connect(selectall, &QAction::triggered, this, &Canvas::selectAll);
	selectall->setShortcut(Qt::Key_A);

	QAction* undo = menu.addAction(QString::fromLocal8Bit("撤销(undo)"));
	connect(undo, &QAction::triggered, this, &Canvas::undo);
	undo->setShortcut(Qt::Key_Z);

	QAction* redo = menu.addAction(QString::fromLocal8Bit("重做(redo)"));
	connect(redo, &QAction::triggered, this, &Canvas::redo);
	redo->setShortcut(Qt::Key_Y);

	QAction* cut = menu.addAction(QString::fromLocal8Bit("剪切"));
	connect(cut, &QAction::triggered, [this, &pos]
		{
			this->cut(pos);
		});
	cut->setShortcut(Qt::Key_X);

	QAction* copy = menu.addAction(QString::fromLocal8Bit("复制"));
	connect(copy, &QAction::triggered, [this, &pos]
		{
			this->copy(pos);
		});
	copy->setShortcut(Qt::Key_C);

	QAction* paste = menu.addAction(QString::fromLocal8Bit("粘贴"));
	connect(paste, &QAction::triggered, [this, &pos]
		{
			this->paste(pos);
		});
	paste->setShortcut(Qt::Key_V);

	QAction* replace = menu.addAction(QString::fromLocal8Bit("复写"));
	connect(replace, &QAction::triggered, [this, &pos]
		{
			this->replace(pos);
		});
	replace->setShortcut(Qt::Key_D);

	QAction* deleteitem = menu.addAction(QString::fromLocal8Bit("删除"));
	connect(deleteitem, &QAction::triggered, this, &Canvas::deleteItem);
	deleteitem->setShortcut(Qt::Key_Backspace);

	QAction* up = menu.addAction(QString::fromLocal8Bit("置上一层"));
	connect(up, &QAction::triggered, this, &Canvas::upLayer);
	up->setShortcut(Qt::Key_Up);

	QAction* down = menu.addAction(QString::fromLocal8Bit("置下一层"));
	connect(down, &QAction::triggered, this, &Canvas::downLayer);
	down->setShortcut(Qt::Key_Down);

	QAction* left = menu.addAction(QString::fromLocal8Bit("置最前"));
	connect(left, &QAction::triggered, this, &Canvas::upMost);
	left->setShortcut(Qt::Key_Left);

	QAction* right = menu.addAction(QString::fromLocal8Bit("置最后"));
	connect(right, &QAction::triggered, this, &Canvas::downMost);
	right->setShortcut(Qt::Key_Right);

	if (!m_manager->isItemAt(pos))
	{
		m_manager->cancelSelected();
		cut->setEnabled(false);
		copy->setEnabled(false);
		replace->setEnabled(false);
		deleteitem->setEnabled(false);
		up->setEnabled(false);
		down->setEnabled(false);
		left->setEnabled(false);
		right->setEnabled(false);
	}
	else if (m_manager->isOnlyOneSelected() || !m_manager->isAnyOneSelected())
	{
		m_manager->selectItemAt(pos);
	}
	else
	{
		up->setEnabled(false);
		down->setEnabled(false);
		left->setEnabled(false);
		right->setEnabled(false);
	}
	if (!m_manager->isAnyOneCopied())
	{
		paste->setEnabled(false);
		replace->setEnabled(false);
	}
	emit selectedItemChanged(m_manager->getSelectedItem());
	update();
	menu.exec(event->globalPos());
}
void Canvas::changeCursor(Edge edge)
{
	switch (edge)
	{
	case Edge::NoEdge:
		setCursor(Qt::ArrowCursor);
		break;
	case Edge::LeftEdge:
		setCursor(Qt::SizeHorCursor);
		break;
	case Edge::TopLeft:
		setCursor(Qt::SizeFDiagCursor);
		break;
	case Edge::TopEdge:
		setCursor(Qt::SizeVerCursor);
		break;
	case Edge::TopRight:
		setCursor(Qt::SizeBDiagCursor);
		break;
	case Edge::RightEdge:
		setCursor(Qt::SizeHorCursor);
		break;
	case Edge::BottomRight:
		setCursor(Qt::SizeFDiagCursor);
		break;
	case Edge::BottomEdge:
		setCursor(Qt::SizeVerCursor);
		break;
	case Edge::BottomLeft:
		setCursor(Qt::SizeBDiagCursor);
		break;
	default:
		break;
	}
}