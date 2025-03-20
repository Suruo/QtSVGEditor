#ifndef ELEMENT_H_
#define ELEMENT_H_

#include <vector>
#include <string>

#include <QRectF>
#include <QPointF>
#include <QPainterPath>
#include <QPen>
#include <QBrush>

enum class Type { None, Path, Line, Rect, Ellipse, Pentagon, Hexagon, Star };
enum class Edge { NoEdge, LeftEdge, TopLeft, TopEdge, TopRight, RightEdge, BottomRight, BottomEdge, BottomLeft };

class ElementBase
{
public:
	ElementBase() = default;
	explicit ElementBase(Type type);
	ElementBase(const ElementBase& item);
	ElementBase(Type type, const QRectF& rect, const QPainterPath& path, const QPen& pen, const QBrush& brush);
	ElementBase(ElementBase&&) = default;
	ElementBase& operator=(const ElementBase&) = default;
	ElementBase& operator=(ElementBase&&) = default;
	virtual ~ElementBase() = default;
	Type getType() const;
	const QRectF& getBoungdingRect() const;
	const QPainterPath& getPath() const;
	const QPen& getPen() const;
	const QBrush& getBrush() const;
	void setPen(const QPen& pen);
	void setBrush(const QBrush& brush);
protected:
	Type m_type;
	QRectF m_boundingrect;
	QPainterPath m_path;
	QPen m_pen;
	QBrush m_brush;
};

class Element:public ElementBase
{
public:
	Element() = default;
	Element(Type type, const QPointF& pos);
	Element(const ElementBase& item);
	Element(const Element&) = default;
	Element(Element&&) = default;
	Element& operator=(const Element&) = default;
	Element& operator=(Element&&) = default;
	~Element() = default;
	virtual void updatePath() = 0;
	virtual std::string toSvgElement() const = 0;
	std::string toSvgPenAndBrushAttribute() const;
	void setSelected(bool selected);
	bool isSelected() const;
	bool isPosIn(const QPointF& point) const;
	Edge recognizeMousePos(const QPointF& pos);
	virtual void drawShape(const QPointF& pos);
	virtual void changeShape(Edge edge, const QPointF& pos);
	virtual void translate(const QPointF& start, const QPointF& end);
protected:
	bool m_selected;
	Edge m_edge;
};

class Path :public Element
{
public:
	Path() = default;
    explicit Path(const QPointF& pos);
	Path(const ElementBase& element);
	Path(const Path&) = default;
	Path(Path&&) = default;
	Path& operator=(const Path&) = default;
	Path& operator=(Path&&) = default;
	~Path() = default;
	virtual void drawShape(const QPointF& pos) override;
	virtual void changeShape(Edge edge, const QPointF& pos) override;
	virtual void updatePath() override;
	virtual void translate(const QPointF& start, const QPointF& end) override;
	const std::vector<QPointF>& getPoints() const;
	virtual std::string toSvgElement() const override;
private:
	std::vector<QPointF> m_points;
};

class Line :public Element
{
public:
	Line() = default;
	explicit Line(const QPointF& pos);
	Line(const ElementBase& element);
	Line(const Line&) = default;
	Line(Line&&) = default;
	Line& operator=(const Line&) = default;
	Line& operator=(Line&&) = default;
	~Line() = default;
	virtual void updatePath() override;
	virtual std::string toSvgElement() const override;
};

class Rect :public Element
{
public:
	Rect() = default;
	explicit Rect(const QPointF& pos);
	Rect(const ElementBase& element);
	Rect(const Rect&) = default;
	Rect(Rect&&) = default;
	Rect& operator=(const Rect&) = default;
	Rect& operator=(Rect&&) = default;
	~Rect() = default;
	virtual void updatePath() override;
	virtual std::string toSvgElement() const override;
};

class Ellipse :public Element
{
public:
	Ellipse() = default;
	explicit Ellipse(const QPointF& pos);
	Ellipse(const ElementBase& element);
	Ellipse(const Ellipse&) = default;
	Ellipse(Ellipse&&) = default;
	Ellipse& operator=(const Ellipse&) = default;
	Ellipse& operator=(Ellipse&&) = default;
	~Ellipse() = default;
	virtual void updatePath() override;
	virtual std::string toSvgElement() const override;
};

class Pentagon :public Element
{
public:
	Pentagon() = default;
	explicit Pentagon(const QPointF& pos);
	Pentagon(const ElementBase& element);
	Pentagon(const Pentagon&) = default;
	Pentagon(Pentagon&&) = default;
	Pentagon& operator=(const Pentagon&) = default;
	Pentagon& operator=(Pentagon&&) = default;
	~Pentagon() = default;
	virtual void updatePath() override;
	virtual std::string toSvgElement() const override;
};

class Hexagon :public Element
{
public:
	Hexagon() = default;
	explicit Hexagon(const QPointF& pos);
	Hexagon(const ElementBase& element);
	Hexagon(const Hexagon&) = default;
	Hexagon(Hexagon&&) = default;
	Hexagon& operator=(const Hexagon&) = default;
	Hexagon& operator=(Hexagon&&) = default;
	~Hexagon() = default;
	virtual void updatePath() override;
	virtual std::string toSvgElement() const override;
};

class Star :public Element
{
public:
	Star() = default;
	explicit Star(const QPointF& pos);
	Star(const ElementBase& element);
	Star(const Star&) = default;
	Star(Star&&) = default;
	Star& operator=(const Star&) = default;
	Star& operator=(Star&&) = default;
	~Star() = default;
	virtual void updatePath() override;
	virtual std::string toSvgElement() const override;
};
#endif // !ELEMENT_H_