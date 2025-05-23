#ifndef MANAGER_H_
#define MANAGER_H_

#include <memory>
#include <string>
#include <vector>

#include <QColor>
#include <QPainter>
#include <QPointF>
#include <QRectF>

#include "commandhistory.h"
#include "element.h"

class Manager
{
public:
	Manager();
	Manager(const Manager&) = default;
	Manager(Manager&&) = default;
	Manager& operator=(const Manager&) = default;
	Manager& operator=(Manager&&) = default;
	~Manager() = default;

	std::shared_ptr<Element> clone(std::shared_ptr<Element> item);
	void copy(const QPointF& pos);
	bool isAnyOneCopied();
	void removeItem();
	void paste(const QPointF& pos);
	void upLayer();
	void downLayer();
	void upMost();
	void downMost();

	void setSelectedPenWidth(double width);
	void setSelectedPenColor(const QColor& color);
	void setSelectedPenStyle(Qt::PenStyle style);
	void setSelectedBrushColor(const QColor& color);

	void addItem(Type type, const QPointF& pos);
	void createItem(Type type, const QRectF& rect, const QPainterPath& path, const QPen& pen, const QBrush& brush);
	bool isItemAt(const QPointF& pos) const;
	void selectItemAt(const QPointF& pos);
	std::shared_ptr<Element> getSelectedItem() const;
	void selectItems(const QRectF& rect);
	void selectAll();
	void cancelSelected();
	bool isOnlyOneSelected() const;
	bool isAnyOneSelected() const;
	void paint(QPainter* painter);
	void moveItem(const QPointF& start, const QPointF& end);
	Edge recognizeMousePos(const QPointF& pos);
	void drawItemShape(const QPointF& pos);
	void changeItemShape(Edge edge, const QPointF& pos);
	std::string toSvgElements() const;
private:
	std::vector<std::shared_ptr<Element>> m_items;
	std::shared_ptr<Element> m_selectedItem;
	std::vector<std::shared_ptr<Element>> m_clipBoard;
	std::shared_ptr<Element> m_singleBoard;
	CommandHistory& m_history;
	QPointF m_copyStartPos;
};

#endif // !MANAGER_H_

