#include "element.h"

ElementBase::ElementBase(Type type)
	: m_type(type)
	, m_boundingRect(QRectF())
	, m_path(QPainterPath())
	, m_pen(QPen(Qt::black, 1, Qt::PenStyle::SolidLine))
	, m_brush(QBrush(Qt::transparent))
{

}
ElementBase::ElementBase(const ElementBase& item)
	: m_type(item.getType())
	, m_boundingRect(item.getBoungdingRect())
	, m_path(item.getPath())
	, m_pen(item.getPen())
	, m_brush(item.getBrush())
{
}
ElementBase::ElementBase(Type type, const QRectF& rect, const QPainterPath& path, const QPen& pen, const QBrush& brush)
	: m_type(type)
	, m_boundingRect(rect)
	, m_path(path)
	, m_pen(pen)
	, m_brush(brush)
{
}
Type ElementBase::getType() const
{
	return m_type;
}
const QBrush& ElementBase::getBrush() const
{
	return m_brush;
}
const QPainterPath& ElementBase::getPath() const
{
	return m_path;
}
const QRectF& ElementBase::getBoungdingRect() const
{
	return m_boundingRect;
}
const QPen& ElementBase::getPen() const
{
	return m_pen;
}
void ElementBase::setPen(const QPen& pen)
{
	m_pen = pen;
}
void ElementBase::setBrush(const QBrush& brush)
{
	m_brush = brush;
}

Element::Element(Type type, const QPointF& pos)
	: ElementBase(type)
	, m_edge(Edge::BottomRight)
	, m_selected(false)
{
	m_boundingRect = QRectF(pos.x(), pos.y(), 1, 1);
}
Element::Element(const ElementBase& item)
	: ElementBase(item)
	, m_edge(Edge::NoEdge)
	, m_selected(false)
{
}
std::string Element::toSvgPenAndBrushAttribute() const
{
	std::string attribute;
	attribute += ("stroke=\"" + m_pen.color().name().toStdString() + "\" ");
	attribute += ("stroke-width=\"" + std::to_string(m_pen.widthF()) + "\" ");
	switch (m_pen.style())
	{
	case Qt::PenStyle::DashLine:
		attribute += ("stroke-dasharray=\"10,5\" ");
		break;
	case Qt::PenStyle::DotLine:
		attribute += ("stroke-dasharray=\"1,5\" ");
		break;
	case Qt::PenStyle::DashDotLine:
		attribute += ("stroke-dasharray=\"10,5,1,5\" ");
		break;
	case Qt::PenStyle::DashDotDotLine:
		attribute += ("stroke-dasharray=\"10,5,1,5,1,5\" ");
		break;
	default:
		break;
	}
	if (m_brush.color() != Qt::transparent)
		attribute += ("fill=\"" + m_brush.color().name().toStdString() + "\" ");
	else
		attribute += ("fill=\"transparent\" ");
	return attribute;
}
void Element::setSelected(bool selected)
{
	m_selected = selected;
	if (!m_selected)
		m_edge = Edge::NoEdge;
}
bool Element::isSelected() const
{
	return m_selected;
}
bool Element::isPosIn(const QPointF& point) const
{
	return m_edge != Edge::NoEdge ? m_boundingRect.normalized().adjusted(-5, -5, 5, 5).contains(point) : m_boundingRect.normalized().contains(point);
}
Edge Element::recognizeMousePos(const QPointF& pos)
{
	double top = m_boundingRect.top();
	double bottom = m_boundingRect.bottom();
	double left = m_boundingRect.left();
	double right = m_boundingRect.right();
	double x = pos.x();
	double y = pos.y();
	if (qAbs(x - right) <= 5 && qAbs(y - bottom) <= 5)
	{
		m_edge = Edge::BottomRight;
		return m_edge;
	}
	if (qAbs(x - left) <= 5 && qAbs(y - top) <= 5)
	{
		m_edge = Edge::TopLeft;
		return m_edge;
	}
	if (qAbs(x - left) <= 5 && qAbs(y - bottom) <= 5)
	{
		m_edge = Edge::BottomLeft;
		return m_edge;
	}
	if (qAbs(x - right) <= 5 && qAbs(y - top) <= 5)
	{
		m_edge = Edge::TopRight;
		return m_edge;
	}
	if (qAbs(x - left) <= 5 && qAbs(y - top) + qAbs(y - bottom) == qAbs(m_boundingRect.height()))
	{
		m_edge = Edge::LeftEdge;
		return m_edge;
	}
	if (qAbs(y - top) <= 5 && qAbs(x - left) + qAbs(x - right) == qAbs(m_boundingRect.width()))
	{
		m_edge = Edge::TopEdge;
		return m_edge;
	}
	if (qAbs(x - right) <= 5 && qAbs(y - top) + qAbs(y - bottom) == qAbs(m_boundingRect.height()))
	{
		m_edge = Edge::RightEdge;
		return m_edge;
	}
	if (qAbs(y - bottom) <= 5 && qAbs(x - left) + qAbs(x - right) == qAbs(m_boundingRect.width()))
	{
		m_edge = Edge::BottomEdge;
		return m_edge;
	}
	m_edge = Edge::NoEdge;
	return m_edge;
}
void Element::drawShape(const QPointF& pos)
{
	changeShape(Edge::BottomRight, pos);
}
void Element::changeShape(Edge edge, const QPointF& pos)
{
	switch (edge)
	{
	case Edge::TopLeft:
		m_boundingRect.setTopLeft(pos);
		break;
	case Edge::TopEdge:
		m_boundingRect.setTop(pos.y());
		break;
	case Edge::TopRight:
		m_boundingRect.setTopRight(pos);
		break;
	case Edge::RightEdge:
		m_boundingRect.setRight(pos.x());
		break;
	case Edge::BottomRight:
		m_boundingRect.setBottomRight(pos);
		break;
	case Edge::BottomEdge:
		m_boundingRect.setBottom(pos.y());
		break;
	case Edge::BottomLeft:
		m_boundingRect.setBottomLeft(pos);
		break;
	case Edge::LeftEdge:
		m_boundingRect.setLeft(pos.x());
		break;
	default:
		break;
	}
	updatePath();
}
void Element::translate(const QPointF& start, const QPointF& end)
{
	if (m_edge == Edge::NoEdge)
	{
		m_path.translate(end.x() - start.x(), end.y() - start.y());
		m_boundingRect.translate(end.x() - start.x(), end.y() - start.y());
	}
}

Path::Path(const QPointF& pos) :Element(Type::Path, pos)
{
	m_points.push_back(pos);
}
Path::Path(const ElementBase& element) : Element(element)
{
	for (int i = 0; i < element.getPath().elementCount(); ++i)
		m_points.push_back(element.getPath().elementAt(i));
	updatePath();
}
void Path::drawShape(const QPointF& pos)
{
	m_points.push_back(pos);
	updatePath();
	m_boundingRect = m_path.boundingRect();
}
void Path::changeShape(Edge edge, const QPointF& pos)
{
	double oldleft = m_boundingRect.left();
	double oldwidth = m_boundingRect.width();
	double oldtop = m_boundingRect.top();
	double oldheight = m_boundingRect.height();
	Element::changeShape(edge, pos);
	if (m_boundingRect.width() == 0)
		m_boundingRect.setWidth(1);
	if (m_boundingRect.height() == 0)
		m_boundingRect.setHeight(1);
	double newleft = m_boundingRect.left();
	double newwidth = m_boundingRect.width();
	double newtop = m_boundingRect.top();
	double newheight = m_boundingRect.height();
	for (QPointF& iter : m_points)
	{
		iter.setX((iter.x() - oldleft) * newwidth / oldwidth + newleft);
		iter.setY((iter.y() - oldtop) * newheight / oldheight + newtop);
	}
	updatePath();
}
void Path::translate(const QPointF& start, const QPointF& end)
{
	Element::translate(start, end);
	for (QPointF& iter : m_points)
	{
		iter.setX(iter.x() + end.x() - start.x());
		iter.setY(iter.y() + end.y() - start.y());
	}
}
void Path::updatePath()
{
	m_path = QPainterPath();
	m_path.moveTo(m_points[0]);
	std::for_each(m_points.begin() + 1, m_points.end(), [this](const QPointF& point)
		{
			m_path.lineTo(point);
		});
}
const std::vector<QPointF>& Path::getPoints() const
{
	return m_points;
}
std::string Path::toSvgElement() const
{
	std::string path = "<path d=\"M";
	path += (std::to_string(m_path.elementAt(0).x) + "," + std::to_string(m_path.elementAt(0).y));
	for (int i = 0; i < m_path.elementCount(); ++i)
	{
		path += ("L" + std::to_string(m_path.elementAt(i).x))
			+ "," + std::to_string((m_path.elementAt(i).y));
	}
	path += "\" ";
	path += toSvgPenAndBrushAttribute();
	path += "/>";
	return path;
}

Line::Line(const QPointF& pos) :Element(Type::Line, pos)
{
}
Line::Line(const ElementBase& element) : Element(element)
{
	updatePath();
}
void Line::updatePath()
{
	m_path = QPainterPath();
	m_path.moveTo(m_boundingRect.topLeft());
	m_path.lineTo(m_boundingRect.bottomRight());
}
std::string Line::toSvgElement() const
{
	std::string line = "<line ";
	line += ("x1=\"" + std::to_string(m_boundingRect.topLeft().x()) + "\" ");
	line += ("y1=\"" + std::to_string(m_boundingRect.topLeft().y()) + "\" ");
	line += ("x2=\"" + std::to_string(m_boundingRect.bottomRight().x()) + "\" ");
	line += ("y2=\"" + std::to_string(m_boundingRect.bottomRight().y()) + "\" ");
	line += toSvgPenAndBrushAttribute();
	line += "/>";
	return line;
}

Rect::Rect(const QPointF& pos) : Element(Type::Rect, pos)
{
}
Rect::Rect(const ElementBase& element) : Element(element)
{
	updatePath();
}
void Rect::updatePath()
{
	m_path = QPainterPath();
	m_path.addRect(m_boundingRect);
}
std::string Rect::toSvgElement() const
{
	std::string rect = "<rect ";
	rect += ("x=\"" + std::to_string(m_boundingRect.normalized().topLeft().x()) + "\" ");
	rect += ("y=\"" + std::to_string(m_boundingRect.normalized().topLeft().y()) + "\" ");
	rect += ("width=\"" + std::to_string(m_boundingRect.normalized().width()) + "\" ");
	rect += ("height=\"" + std::to_string(m_boundingRect.normalized().height()) + "\" ");
	rect += toSvgPenAndBrushAttribute();
	rect += "/>";
	return rect;
}

Ellipse::Ellipse(const QPointF& pos) : Element(Type::Ellipse, pos)
{
}
Ellipse::Ellipse(const ElementBase& element) : Element(element)
{
	updatePath();
}
void Ellipse::updatePath()
{
	m_path = QPainterPath();
	m_path.addEllipse(m_boundingRect);
}
std::string Ellipse::toSvgElement() const
{
	std::string ellipse = "<ellipse ";
	ellipse += ("cx=\"" + std::to_string(m_boundingRect.normalized().topLeft().x() + m_boundingRect.normalized().width() / 2) + "\" ");
	ellipse += ("cy=\"" + std::to_string(m_boundingRect.normalized().topLeft().y() + m_boundingRect.normalized().height() / 2) + "\" ");
	ellipse += ("rx=\"" + std::to_string(m_boundingRect.normalized().width() / 2) + "\" ");
	ellipse += ("ry=\"" + std::to_string(m_boundingRect.normalized().height() / 2) + "\" ");
	ellipse += toSvgPenAndBrushAttribute();
	ellipse += "/>";
	return ellipse;
}

Pentagon::Pentagon(const QPointF& pos) : Element(Type::Pentagon, pos)
{
}
Pentagon::Pentagon(const ElementBase& element) : Element(element)
{
	updatePath();
}
void Pentagon::updatePath()
{
	m_path = QPainterPath();
	double x1 = m_boundingRect.topLeft().x();
	double y1 = m_boundingRect.topLeft().y();
	double x2 = m_boundingRect.bottomRight().x();
	double y2 = m_boundingRect.bottomRight().y();
	m_path.moveTo((x1 + x2) / 2, y1);
	m_path.lineTo(x2, y1 + (y2 - y1) * 7 / 18);
	m_path.lineTo(x1 + (x2 - x1) * 15.4 / 19, y2);
	m_path.lineTo(x1 + (x2 - x1) * 3.6 / 19, y2);
	m_path.lineTo(x1, y1 + (y2 - y1) * 7 / 18);
	m_path.closeSubpath();
}
std::string Pentagon::toSvgElement() const
{
	std::string pentagon = "<path d=";
	double x1 = m_boundingRect.topLeft().x();
	double y1 = m_boundingRect.topLeft().y();
	double x2 = m_boundingRect.bottomRight().x();
	double y2 = m_boundingRect.bottomRight().y();
	pentagon += ("\"M" + std::to_string((x1 + x2) / 2) + "," + std::to_string(y1));
	pentagon += ("L" + std::to_string(x2) + "," + std::to_string(y1 + (y2 - y1) * 7 / 18));
	pentagon += ("L" + std::to_string(x1 + (x2 - x1) * 15.4 / 19) + "," + std::to_string(y2));
	pentagon += ("L" + std::to_string(x1 + (x2 - x1) * 3.6 / 19) + "," + std::to_string(y2));
	pentagon += ("L" + std::to_string(x1) + "," + std::to_string(y1 + (y2 - y1) * 7 / 18) + "Z\" ");
	pentagon += toSvgPenAndBrushAttribute();
	pentagon += "/>";
	return pentagon;
}

Hexagon::Hexagon(const QPointF& pos) : Element(Type::Hexagon, pos)
{
}
Hexagon::Hexagon(const ElementBase& element) : Element(element)
{
	updatePath();
}
void Hexagon::updatePath()
{
	m_path = QPainterPath();
	double x1 = m_boundingRect.topLeft().x();
	double y1 = m_boundingRect.topLeft().y();
	double x2 = m_boundingRect.bottomRight().x();
	double y2 = m_boundingRect.bottomRight().y();
	m_path.moveTo(x1 + (x2 - x1) / 4, y1);
	m_path.lineTo(x1 + 3 * (x2 - x1) / 4, y1);
	m_path.lineTo(x2, (y1 + y2) / 2);
	m_path.lineTo(x1 + 3 * (x2 - x1) / 4, y2);
	m_path.lineTo(x1 + (x2 - x1) / 4, y2);
	m_path.lineTo(x1, (y1 + y2) / 2);
	m_path.closeSubpath();
}
std::string Hexagon::toSvgElement() const
{
	std::string hexagon = "<path d=";
	double x1 = m_boundingRect.topLeft().x();
	double y1 = m_boundingRect.topLeft().y();
	double x2 = m_boundingRect.bottomRight().x();
	double y2 = m_boundingRect.bottomRight().y();
	hexagon += ("\"M" + std::to_string(x1 + (x2 - x1) / 4) + "," + std::to_string(y1));
	hexagon += ("L" + std::to_string(x1 + 3 * (x2 - x1) / 4) + "," + std::to_string(y1));
	hexagon += ("L" + std::to_string(x2) + "," + std::to_string((y1 + y2) / 2));
	hexagon += ("L" + std::to_string(x1 + 3 * (x2 - x1) / 4) + "," + std::to_string(y2));
	hexagon += ("L" + std::to_string(x1 + (x2 - x1) / 4) + "," + std::to_string(y2));
	hexagon += ("L" + std::to_string(x1) + "," + std::to_string((y1 + y2) / 2) + "Z\" ");
	hexagon += toSvgPenAndBrushAttribute();
	hexagon += "/>";
	return hexagon;
}

Star::Star(const QPointF& pos) : Element(Type::Star, pos)
{
}
Star::Star(const ElementBase& element) : Element(element)
{
	updatePath();
}
void Star::updatePath()
{
	m_path = QPainterPath();
	double x1 = m_boundingRect.topLeft().x();
	double y1 = m_boundingRect.topLeft().y();
	double x2 = m_boundingRect.bottomRight().x();
	double y2 = m_boundingRect.bottomRight().y();
	m_path.moveTo((x1 + x2) / 2, y1);
	m_path.lineTo(x1 + (x2 - x1) * 11.7 / 19, y1 + (y2 - y1) * 7 / 18);
	m_path.lineTo(x2, y1 + (y2 - y1) * 7 / 18);
	m_path.lineTo(x1 + (x2 - x1) * 13.1 / 19, y1 + (y2 - y1) * 11.2 / 18);
	m_path.lineTo(x1 + (x2 - x1) * 15.4 / 19, y2);
	m_path.lineTo((x1 + x2) / 2, y1 + (y2 - y1) * 13 / 18);
	m_path.lineTo(x1 + (x2 - x1) * 3.6 / 19, y2);
	m_path.lineTo(x1 + (x2 - x1) * 5.9 / 19, y1 + (y2 - y1) * 11.2 / 18);
	m_path.lineTo(x1, y1 + (y2 - y1) * 7 / 18);
	m_path.lineTo(x1 + (x2 - x1) * 7.3 / 19, y1 + (y2 - y1) * 7 / 18);
	m_path.closeSubpath();
}
std::string Star::toSvgElement() const
{
	std::string star = "<path d=";
	double x1 = m_boundingRect.topLeft().x();
	double y1 = m_boundingRect.topLeft().y();
	double x2 = m_boundingRect.bottomRight().x();
	double y2 = m_boundingRect.bottomRight().y();
	star += ("\"M" + std::to_string((x1 + x2) / 2) + "," + std::to_string(y1));
	star += ("L" + std::to_string(x1 + (x2 - x1) * 11.7 / 19) + "," + std::to_string(y1 + (y2 - y1) * 7 / 18));
	star += ("L" + std::to_string(x2) + "," + std::to_string(y1 + (y2 - y1) * 7 / 18));
	star += ("L" + std::to_string(x1 + (x2 - x1) * 13.1 / 19) + "," + std::to_string(y1 + (y2 - y1) * 11.2 / 18));
	star += ("L" + std::to_string(x1 + (x2 - x1) * 15.4 / 19) + "," + std::to_string(y2));
	star += ("L" + std::to_string((x1 + x2) / 2) + "," + std::to_string(y1 + (y2 - y1) * 13 / 18));
	star += ("L" + std::to_string(x1 + (x2 - x1) * 3.6 / 19) + "," + std::to_string(y2));
	star += ("L" + std::to_string(x1 + (x2 - x1) * 5.9 / 19) + "," + std::to_string(y1 + (y2 - y1) * 11.2 / 18));
	star += ("L" + std::to_string(x1) + "," + std::to_string(y1 + (y2 - y1) * 7 / 18));
	star += ("L" + std::to_string(x1 + (x2 - x1) * 7.3 / 19) + "," + std::to_string(y1 + (y2 - y1) * 7 / 18) + "Z\" ");
	star += toSvgPenAndBrushAttribute();
	star += "/>";
	return star;
}