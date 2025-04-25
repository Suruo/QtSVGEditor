#ifndef CANVAS_H_
#define CANVAS_H_

#include <memory>

#include <QColor>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPointF>
#include <QRubberBand>
#include <QWidget>

#include "commandhistory.h"
#include "element.h"
#include "manager.h"

class Canvas :public QWidget
{
	Q_OBJECT
public:
	Canvas(QWidget* parent);
	~Canvas();
	void setItemType(Type type);
	void setSize(int width, int height);
	void setScale(double scale);
	void setBackGroundColor(const QColor& color);
	int getWidth() const;
	int getHeight() const;
	double getScale() const;
	double& getScaleRefer();
	const QColor& getBackGroundColor() const;
	std::shared_ptr<Element> getSelectedItem() const;
	std::shared_ptr<Manager> getManager() const;
	void reset();
	std::string toSvgText() const;
public slots:
	void selectAll();
	void copy(const QPointF& pos);
	void cut(const QPointF& pos);
	void paste(const QPointF& pos);
	void replace(const QPointF& pos);
	void deleteItem();
	void upLayer();
	void downLayer();
	void upMost();
	void downMost();
	void undo();
	void redo();
signals:
	void backGroundColorChange();
	void sizeChange();
	void toTypeNone();
	void selectedItemChanged(std::shared_ptr<Element> item);
protected:
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void contextMenuEvent(QContextMenuEvent* event) override;
	void leftButtonPressed(const QPointF& pos);
	void mouseMoving(const QPointF& pos);
	void setRightButtonMenu(QContextMenuEvent* event);
	void changeCursor(Edge edge);
private:
	std::shared_ptr<Manager> m_manager;
	bool m_isPressed;
	bool m_isCreating;
	bool m_isResizing;
	bool m_isMoving;
	QPointF m_moveStartPos;
	Type m_type;
	Edge m_edge;
	double m_scale;
	CommandHistory& m_history;
	QRubberBand* m_rubberBand;
};

#endif // !CANVAS_H_

