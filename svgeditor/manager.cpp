#include "manager.h"

#include "command.h"

Manager::Manager() :m_selecteditem(nullptr), m_singleboard(nullptr), m_history(CommandHistory::getInstance())
{
}
std::shared_ptr<Element> Manager::clone(std::shared_ptr<Element> item)
{
	switch (item->getType())
	{
	case Type::Path:
		return std::make_shared<Path>(*item);
		break;
	case Type::Line:
		return std::make_shared<Line>(*item);
		break;
	case Type::Rect:
		return std::make_shared<Rect>(*item);
		break;
	case Type::Ellipse:
		return std::make_shared<Ellipse>(*item);
		break;
	case Type::Pentagon:
		return std::make_shared<Pentagon>(*item);
		break;
	case Type::Hexagon:
		return std::make_shared<Hexagon>(*item);
		break;
	case Type::Star:
		return std::make_shared<Star>(*item);
		break;
	default:
		return nullptr;
		break;
	}
}
void Manager::copy(const QPointF& pos)
{
	m_singleboard = nullptr;
	m_clipboard.clear();
	m_copystart = pos;
	if (m_selecteditem != nullptr)
	{
		m_singleboard = clone(m_selecteditem);
	}
	else
	{
		std::for_each(m_items.begin(), m_items.end(), [this](std::shared_ptr<Element> item)
			{
				if (item != nullptr)
					if (item->isSelected())					
						m_clipboard.push_back(clone(item));					
			});
	}
}
bool Manager::isAnyOneCopied()
{
	return m_singleboard != nullptr || !m_clipboard.empty();
}
void Manager::removeItem()
{
	if (m_selecteditem != nullptr)
	{
		for (size_t i = 0; i < m_items.size(); ++i)
		{
			if (m_items.at(i) == m_selecteditem)
			{
				m_history.addCommand(std::make_shared<Remove>(m_items, i));
				m_items.at(i) = nullptr;
			}
		}
		m_selecteditem = nullptr;
	}
	else
	{
		std::vector<std::shared_ptr<Command>> commands;
		for (size_t i = 0; i < m_items.size(); ++i)
		{
			if (m_items.at(i) != nullptr)
				if(m_items.at(i)->isSelected())
				{
					commands.push_back(std::make_shared<Remove>(m_items, i));
					m_items.at(i) = nullptr;
				}
		}
		if (!commands.empty())
			m_history.addCommands(commands);
	}
}
void Manager::paste(const QPointF& pos)
{
	if (m_singleboard != nullptr)
	{
		std::shared_ptr<Element> cloneptr = clone(m_singleboard);
		cloneptr->translate(m_copystart, pos);
		cloneptr->setSelected(true);
		m_items.push_back(cloneptr);
		m_history.addCommand(std::make_shared<Add>(m_items, m_items.size() - 1));
		m_selecteditem = cloneptr;
	}
	else
	{
		std::vector<std::shared_ptr<Command>> commands;
		std::for_each(m_clipboard.begin(), m_clipboard.end(), [this, &pos, &commands](std::shared_ptr<Element> item)
			{
				std::shared_ptr<Element> cloneptr = clone(item);
				cloneptr->translate(m_copystart, pos);
				cloneptr->setSelected(true);
				m_items.push_back(cloneptr);
				commands.push_back(std::make_shared<Add>(m_items, m_items.size() - 1));
			});
		if (!commands.empty())
			m_history.addCommands(commands);
	}
}
void Manager::upLayer()
{
	if (m_items.size() < 2)
		return;
	size_t i;
	for (i = 0; i < m_items.size(); ++i)
	{
		if (m_items.at(i) == m_selecteditem)
			break;
	}
	if (i + 1 != m_items.size())
	{
		m_history.addCommand(std::make_shared<SwapLayer>(m_items, i, i + 1));
		m_items.at(i).swap(m_items.at(i + 1));	
	}
}
void Manager::downLayer()
{
	if (m_items.size() < 2)
		return;
	size_t i;
	for (i = 0; i < m_items.size(); ++i)
	{
		if (m_items.at(i) == m_selecteditem)
			break;
	}
	if (static_cast<int>(i) - 1 >= 0)
	{
		m_history.addCommand(std::make_shared<SwapLayer>(m_items, i, i - 1));
		m_items.at(i).swap(m_items.at(i - 1));
	}	
}
void Manager::upMost()
{
	int i;
	if (m_items.empty())
		return;
	for (i = 0; i < m_items.size(); ++i)
	{
		if (m_items.at(i) == m_selecteditem)
			break;
	}
	m_history.addCommand(std::make_shared<SwapLayer>(m_items, i, m_items.size() - 1));
	m_items.at(i).swap(m_items.back());
}
void Manager::downMost()
{
	int i;
	if (m_items.empty())
		return;
	for (i = 0; i < m_items.size(); ++i)
	{
		if (m_items.at(i) == m_selecteditem)
			break;
	}
	m_history.addCommand(std::make_shared<SwapLayer>(m_items, i, 0));
	m_items.at(i).swap(m_items.front());
}

void Manager::addItem(Type type, const QPointF& pos)
{
	switch (type)
	{
	case Type::Path:
		m_items.push_back(std::make_shared<Path>(pos));
		break;
	case Type::Line:
		m_items.push_back(std::make_shared<Line>(pos));
		break;
	case Type::Rect:
		m_items.push_back(std::make_shared<Rect>(pos));
		break;
	case Type::Ellipse:
		m_items.push_back(std::make_shared<Ellipse>(pos));
		break;
	case Type::Pentagon:
		m_items.push_back(std::make_shared<Pentagon>(pos));
		break;
	case Type::Hexagon:
		m_items.push_back(std::make_shared<Hexagon>(pos));
		break;
	case Type::Star:
		m_items.push_back(std::make_shared<Star>(pos));
		break;
	default:
		break;
	}
	m_selecteditem = m_items.back();
	m_history.addCommand(std::make_shared<Add>(m_items, m_items.size() - 1));
	m_selecteditem->setSelected(true);
}
void Manager::createItem(Type type, const QRectF& rect, const QPainterPath& path, const QPen& pen, const QBrush& brush)
{
	switch (type)
	{
	case Type::Path:
		m_items.push_back(std::make_shared<Path>(ElementBase(type, rect, path, pen, brush)));
		break;
	case Type::Line:
		m_items.push_back(std::make_shared<Line>(ElementBase(type, rect, path, pen, brush)));
		break;
	case Type::Rect:
		m_items.push_back(std::make_shared<Rect>(ElementBase(type, rect, path, pen, brush)));
		break;
	case Type::Ellipse:
		m_items.push_back(std::make_shared<Ellipse>(ElementBase(type, rect, path, pen, brush)));
		break;
	case Type::Pentagon:
		m_items.push_back(std::make_shared<Pentagon>(ElementBase(type, rect, path, pen, brush)));
		break;
	case Type::Hexagon:
		m_items.push_back(std::make_shared<Hexagon>(ElementBase(type, rect, path, pen, brush)));
		break;
	case Type::Star:
		m_items.push_back(std::make_shared<Star>(ElementBase(type, rect, path, pen, brush)));
		break;
	default:
		break;
	}
}
void Manager::setSelectedPenWidth(double width)
{
	if (m_selecteditem != nullptr)
	{
		QPen pen = m_selecteditem->getPen();
		pen.setWidthF(width);
		m_history.addCommand(std::make_shared<ChangePen>(m_selecteditem, pen));
		m_selecteditem->setPen(pen);
	}
}
void Manager::setSelectedPenColor(const QColor& color)
{
	if (m_selecteditem != nullptr)
	{
		QPen pen = m_selecteditem->getPen();
		pen.setColor(color);
		m_history.addCommand(std::make_shared<ChangePen>(m_selecteditem, pen));
		m_selecteditem->setPen(pen);
	}
}
void Manager::setSelectedPenStyle(Qt::PenStyle style)
{
	if (m_selecteditem != nullptr)
	{
		QPen pen = m_selecteditem->getPen();
		pen.setStyle(style);
		m_history.addCommand(std::make_shared<ChangePen>(m_selecteditem, pen));
		m_selecteditem->setPen(pen);
	}
}
void Manager::setSelectedBrushColor(const QColor& color)
{
	if (m_selecteditem != nullptr)
	{
		m_history.addCommand(std::make_shared<ChangeBrush>(m_selecteditem, QBrush(color)));
		m_selecteditem->setBrush(QBrush(color));
	}
}
void Manager::paint(QPainter* painter)
{
	std::for_each(m_items.begin(), m_items.end(), [painter](std::shared_ptr<Element> item)
		{
			if (item != nullptr)
			{
				painter->save();
				painter->setRenderHint(QPainter::Antialiasing);
				painter->setPen(item->getPen());
				painter->setBrush(item->getBrush());
				painter->drawPath(item->getPath());
				if (item->isSelected())
				{
					painter->setPen(QPen(Qt::blue, 1, Qt::PenStyle::DashLine));
					painter->setBrush(Qt::transparent);
					painter->drawRect(item->getBoungdingRect());
				}
				painter->restore();
			}	
		});
}
bool Manager::isItemAt(const QPointF& pos) const
{
	return std::any_of(m_items.rbegin(), m_items.rend(), [&pos](std::shared_ptr<Element> item)
		{
			if (item == nullptr)
				return false;
			else
				return item->isPosIn(pos);
		});
}
void Manager::selectItemAt(const QPointF& pos)
{
	if (m_selecteditem != nullptr)
		if (m_selecteditem->isPosIn(pos))
			return;
	cancelSelected();
	for (auto iter = m_items.rbegin(); iter != m_items.rend(); ++iter)
	{
		if (*iter != nullptr)
			if ((*iter)->isPosIn(pos))
			{
				(*iter)->setSelected(true);
				m_selecteditem = *iter;
				return;
			}
	}

}
std::shared_ptr<Element> Manager::getSelectedItem() const
{
	return m_selecteditem;
}
void Manager::selectItems(const QRectF& rect)
{
	cancelSelected();
	std::for_each(m_items.rbegin(), m_items.rend(), [&rect](std::shared_ptr<Element> item)
		{
			if (item != nullptr)
				if (rect.intersects(item->getBoungdingRect()))
					item->setSelected(true);		
		});
}
void Manager::selectAll()
{
	cancelSelected();
	std::for_each(m_items.begin(), m_items.end(), [](std::shared_ptr<Element> item)
		{	
			if (item != nullptr)
				item->setSelected(true);
		});
}
void Manager::cancelSelected()
{
	std::for_each(m_items.begin(), m_items.end(), [](std::shared_ptr<Element> item)
		{
			if (item != nullptr)
				item->setSelected(false);
		});
	m_selecteditem = nullptr;
}
bool Manager::isOnlyOneSelected() const
{
	return m_selecteditem != nullptr;
}
bool Manager::isAnyOneSelected() const
{
	return std::any_of(m_items.begin(), m_items.end(), [](std::shared_ptr<Element> item)
		{
			if (item == nullptr)
				return false;
			else
				return item->isSelected();
		});
}
void Manager::moveItem(const QPointF& start, const QPointF& end)
{
	if (m_selecteditem != nullptr)
		m_selecteditem->translate(start, end);
	else
		std::for_each(m_items.begin(), m_items.end(), [&](std::shared_ptr<Element> item)
			{
				if (item != nullptr)
					if(item->isSelected())
						item->translate(start, end);
			});
}
Edge Manager::recognizeMousePos(const QPointF& pos)
{
	
	return m_selecteditem != nullptr ? m_selecteditem->recognizeMousePos(pos) : Edge::NoEdge;
}
void Manager::drawItemShape(const QPointF& pos)
{
	if (m_selecteditem != nullptr)
		m_selecteditem->drawShape(pos);
}
void Manager::changeItemShape(Edge edge, const QPointF& pos)
{
	if (m_selecteditem != nullptr)
		m_selecteditem->changeShape(edge, pos);
}
std::string Manager::toSvgElements() const
{
	std::string elements;
	std::for_each(m_items.begin(), m_items.end(), [&elements](std::shared_ptr<Element> iter)
		{
			elements += ("\t" + iter->toSvgElement() + "\n");
		});
	return elements;
}