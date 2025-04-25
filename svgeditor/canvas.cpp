#include "canvas.h"

#include <QMenu>
#include <QPainter>
#include <QPalette>
#include <QShortCut>

#include "command.h"

Canvas::Canvas(QWidget* parent = Q_NULLPTR)
	: QWidget(parent)
	, m_manager(std::make_shared<Manager>())
	, m_isPressed(false)
	, m_isCreating(false)
	, m_isResizing(false)
	, m_isMoving(false)
	, m_moveStartPos(QPointF())
	, m_type(Type::None)
	, m_edge(Edge::NoEdge)
	, m_scale(1)
	, m_history(CommandHistory::getInstance())
	, m_rubberBand(new QRubberBand(QRubberBand::Rectangle, this))
{
	setMouseTracking(true);
	setFixedSize(1600, 900);
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
	m_isPressed = false;
	m_isCreating = false;
	m_isResizing = false;
	m_isMoving = false;
	m_rubberBand->hide();
	return QWidget::mouseReleaseEvent(event);
}
void Canvas::leftButtonPressed(const QPointF& pos)
{
	m_isPressed = true;
	if (m_type != Type::None)
	{
		m_manager->cancelSelected();
		m_manager->addItem(m_type, pos);
		m_isCreating = true;
	}
	else if (!m_manager->isItemAt(pos))
	{
		m_manager->cancelSelected();
		m_moveStartPos = pos;
		m_rubberBand->setGeometry(QRect((m_moveStartPos * m_scale).toPoint(), QSize()));
		m_rubberBand->show();
	}
	else if (!m_manager->isAnyOneSelected() || !m_manager->isOnlyOneSelected())
	{
		if (!m_manager->isAnyOneSelected())
			m_manager->selectItemAt(pos);
		m_isMoving = true;
		m_moveStartPos = pos;
	}
	else
	{
		m_manager->selectItemAt(pos);
		if (m_edge != Edge::NoEdge)
		{
			m_isResizing = true;
		}
		else
		{
			m_isMoving = true;
			m_moveStartPos = pos;
		}
	}
}
void Canvas::mouseMoving(const QPointF& pos)
{
	if (!m_isPressed)
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
	else if (m_isCreating)
	{
		m_manager->drawItemShape(pos);
	}
	else if (m_isResizing)
	{
		m_manager->changeItemShape(m_edge, pos);
	}
	else if (m_isMoving)
	{
		m_manager->moveItem(m_moveStartPos, pos);
		m_moveStartPos = pos;
	}
	else if (m_rubberBand->isVisible())
	{
		m_rubberBand->setGeometry(QRect((m_moveStartPos * m_scale).toPoint(), (pos * m_scale).toPoint()).normalized());
		m_manager->selectItems(QRectF(m_moveStartPos, pos));
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
	QMenu menu;
	QAction* selectAll = menu.addAction(QString::fromLocal8Bit("全选"));
	connect(selectAll, &QAction::triggered, this, &Canvas::selectAll);

	QAction* undo = menu.addAction(QString::fromLocal8Bit("撤销(undo)"));
	connect(undo, &QAction::triggered, this, &Canvas::undo);

	QAction* redo = menu.addAction(QString::fromLocal8Bit("重做(redo)"));
	connect(redo, &QAction::triggered, this, &Canvas::redo);

	QAction* cut = menu.addAction(QString::fromLocal8Bit("剪切"));
	connect(cut, &QAction::triggered, [this, &pos]
		{
			this->cut(pos);
		});

	QAction* copy = menu.addAction(QString::fromLocal8Bit("复制"));
	connect(copy, &QAction::triggered, [this, &pos]
		{
			this->copy(pos);
		});

	QAction* paste = menu.addAction(QString::fromLocal8Bit("粘贴"));
	connect(paste, &QAction::triggered, [this, &pos]
		{
			this->paste(pos);
		});

	QAction* replace = menu.addAction(QString::fromLocal8Bit("复写"));
	connect(replace, &QAction::triggered, [this, &pos]
		{
			this->replace(pos);
		});

	QAction* deleteItem = menu.addAction(QString::fromLocal8Bit("删除"));
	connect(deleteItem, &QAction::triggered, this, &Canvas::deleteItem);

	QAction* up = menu.addAction(QString::fromLocal8Bit("置上一层"));
	connect(up, &QAction::triggered, this, &Canvas::upLayer);

	QAction* down = menu.addAction(QString::fromLocal8Bit("置下一层"));
	connect(down, &QAction::triggered, this, &Canvas::downLayer);

	QAction* left = menu.addAction(QString::fromLocal8Bit("置最前"));
	connect(left, &QAction::triggered, this, &Canvas::upMost);

	QAction* right = menu.addAction(QString::fromLocal8Bit("置最后"));
	connect(right, &QAction::triggered, this, &Canvas::downMost);

	if (!m_manager->isItemAt(pos))
	{
		m_manager->cancelSelected();
		cut->setEnabled(false);
		copy->setEnabled(false);
		replace->setEnabled(false);
		deleteItem->setEnabled(false);
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