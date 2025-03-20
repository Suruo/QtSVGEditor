#include "command.h"

#include "canvas.h"

Command::Command()
{
}

Add::Add(std::vector<std::shared_ptr<Element>>& refer, size_t index) :m_refer(refer), m_backup(refer.at(index)), m_index(index)
{
}
void Add::redo()
{
	m_refer.at(m_index) = m_backup;
}
void Add::undo()
{
	m_refer.at(m_index) = nullptr;
}

Remove::Remove(std::vector<std::shared_ptr<Element>>& refer, size_t index) :m_refer(refer), m_backup(refer.at(index)), m_index(index)
{
}
void Remove::redo()
{
	m_refer.at(m_index) = nullptr;
}
void Remove::undo()
{
	m_refer.at(m_index) = m_backup;
}

SwapLayer::SwapLayer(std::vector<std::shared_ptr<Element>>& refer, size_t index1, size_t index2)
	: m_refer(refer)
	, m_index1(index1)
	, m_index2(index2)
{
}
void SwapLayer::redo()
{
	m_refer.at(m_index1).swap(m_refer.at(m_index2));
}
void SwapLayer::undo()
{
	m_refer.at(m_index1).swap(m_refer.at(m_index2));
}

ChangePen::ChangePen(std::shared_ptr<Element> item, const QPen& target) :m_item(item), m_backup(item->getPen()), m_target(target)
{
}
void ChangePen::redo()
{
	m_item->setPen(m_target);
}
void ChangePen::undo()
{
	m_item->setPen(m_backup);
}

ChangeBrush::ChangeBrush(std::shared_ptr<Element> item, const QBrush& target) :m_item(item), m_backup(item->getBrush()), m_target(target)
{
}
void ChangeBrush::redo()
{
	m_item->setBrush(m_target);
}
void ChangeBrush::undo()
{
	m_item->setBrush(m_backup);
}

ChangeCanvasSize::ChangeCanvasSize(Canvas* canvas, const QSize& target)
	: m_canvas(canvas)
	, m_backup(canvas->getWidth(), canvas->getHeight())
	, m_target(target)
{
}
void ChangeCanvasSize::redo()
{
	m_canvas->setFixedSize(static_cast<int>(m_target.width() * m_canvas->getScale())
		, static_cast<int>(m_target.height() * m_canvas->getScale()));
	emit m_canvas->sizeChange();
}
void ChangeCanvasSize::undo()
{
	m_canvas->setFixedSize(static_cast<int>(m_backup.width() * m_canvas->getScale())
		, static_cast<int>(m_backup.height() * m_canvas->getScale()));
	emit m_canvas->sizeChange();
}

ChangeScale::ChangeScale(Canvas* canvas, double target) :m_canvas(canvas), m_backup(m_canvas->getScale()), m_target(target)
{
}
void ChangeScale::redo()
{
	m_canvas->setFixedSize(static_cast<int>(m_canvas->getWidth() * m_target), static_cast<int>(m_canvas->getHeight() * m_target));
	m_canvas->getScaleRefer() = m_target;
	emit m_canvas->sizeChange();
	m_canvas->update();
}
void ChangeScale::undo()
{
	m_canvas->setFixedSize(static_cast<int>(m_canvas->getWidth() * m_backup), static_cast<int>(m_canvas->getHeight() * m_backup));
	m_canvas->getScaleRefer() = m_backup;
	emit m_canvas->sizeChange();
	m_canvas->update();
}

ChangeBackGroundColor::ChangeBackGroundColor(Canvas* canvas, const QColor& target)
	: m_canvas(canvas)
	, m_backup(canvas->getBackGroundColor())
	, m_target(target)
{
}
void ChangeBackGroundColor::redo()
{
	m_canvas->setPalette(m_target);
	emit m_canvas->backGroundColorChange();
}
void ChangeBackGroundColor::undo()
{
	m_canvas->setPalette(m_backup);
	emit m_canvas->backGroundColorChange();
}