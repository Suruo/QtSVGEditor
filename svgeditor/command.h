#ifndef COMMAND_H_
#define COMMAND_H_

#include <memory>

#include <QPen>
#include <QBrush>
#include <QSize>
#include <QColor>

#include "element.h"

class Canvas;

class Command
{
public:
	Command();
	Command(const Command&) = default;
	Command(Command&&) = default;
	Command& operator=(const Command&) = default;
	Command& operator=(Command&&) = default;
	virtual ~Command() = default;
	virtual void redo() = 0;
	virtual void undo() = 0;
};

class Add :public Command
{
public:
	Add() = default;
	Add(std::vector<std::shared_ptr<Element>>& refer, size_t index);
	Add(const Add&) = default;
	Add(Add&&) = default;
	Add& operator=(const Add&) = default;
	Add& operator=(Add&&) = default;
	~Add() = default;
	virtual void redo() override;
	virtual void undo() override;
private:
	std::vector<std::shared_ptr<Element>>& m_refer;
	std::shared_ptr<Element> m_backup;
	size_t m_index;
};

class Remove :public Command
{
public:
	Remove() = default;
	Remove(std::vector<std::shared_ptr<Element>>& refer, size_t index);
	Remove(const Remove&) = default;
	Remove(Remove&&) = default;
	Remove& operator=(const Remove&) = default;
	Remove& operator=(Remove&&) = default;
	~Remove() = default;
	virtual void redo() override;
	virtual void undo() override;
private:
	std::vector<std::shared_ptr<Element>>& m_refer;
	std::shared_ptr<Element> m_backup;
	size_t m_index;
};

class SwapLayer :public Command
{
public:
	SwapLayer() = default;
	SwapLayer(std::vector<std::shared_ptr<Element>>& refer, size_t index1, size_t index2);
	SwapLayer(const SwapLayer&) = default;
	SwapLayer(SwapLayer&&) = default;
	SwapLayer& operator=(const SwapLayer&) = default;
	SwapLayer& operator=(SwapLayer&&) = default;
	~SwapLayer() = default;
	virtual void redo() override;
	virtual void undo() override;
private:
	std::vector<std::shared_ptr<Element>>& m_refer;
	size_t m_index1;
	size_t m_index2;
};

class ChangePen :public Command
{
public:
	ChangePen() = default;
	ChangePen(std::shared_ptr<Element> item, const QPen& target);
	ChangePen(const ChangePen&) = default;
	ChangePen(ChangePen&&) = default;
	ChangePen& operator=(const ChangePen&) = default;
	ChangePen& operator=(ChangePen&&) = default;
	~ChangePen() = default;
	virtual void redo() override;
	virtual void undo() override;
private:
	std::shared_ptr<Element> m_item;
	QPen m_backup;
	QPen m_target;
};

class ChangeBrush :public Command
{
public:
	ChangeBrush() = default;
	ChangeBrush(std::shared_ptr<Element> item, const QBrush& target);
	ChangeBrush(const ChangeBrush&) = default;
	ChangeBrush(ChangeBrush&&) = default;
	ChangeBrush& operator=(const ChangeBrush&) = default;
	ChangeBrush& operator=(ChangeBrush&&) = default;
	~ChangeBrush() = default;
	virtual void redo() override;
	virtual void undo() override;
private:
	std::shared_ptr<Element> m_item;
	QBrush m_backup;
	QBrush m_target;
};

class ChangeCanvasSize :public Command
{
public:
	ChangeCanvasSize() = default;
	ChangeCanvasSize(Canvas* canvas, const QSize& target);
	ChangeCanvasSize(const ChangeCanvasSize&) = default;
	ChangeCanvasSize(ChangeCanvasSize&&) = default;
	ChangeCanvasSize& operator=(const ChangeCanvasSize&) = default;
	ChangeCanvasSize& operator=(ChangeCanvasSize&&) = default;
	~ChangeCanvasSize() = default;
	virtual void redo() override;
	virtual void undo() override;
private:
	Canvas* m_canvas;
	QSize m_backup;
	QSize m_target;
};

class ChangeScale :public Command
{
public:
	ChangeScale() = default;
	ChangeScale(Canvas * canvas, double target);
	ChangeScale(const ChangeScale&) = default;
	ChangeScale(ChangeScale&&) = default;
	ChangeScale& operator=(const ChangeScale&) = default;
	ChangeScale& operator=(ChangeScale&&) = default;
	~ChangeScale() = default;
	virtual void redo() override;
	virtual void undo() override;
private:
	Canvas* m_canvas;
	double m_backup;
	double m_target;
};

class ChangeBackGroundColor :public Command
{
public:
	ChangeBackGroundColor() = default;
	ChangeBackGroundColor(Canvas * canvas, const QColor& target);
	ChangeBackGroundColor(const ChangeBackGroundColor&) = default;
	ChangeBackGroundColor(ChangeBackGroundColor&&) = default;
	ChangeBackGroundColor& operator=(const ChangeBackGroundColor&) = default;
	ChangeBackGroundColor& operator=(ChangeBackGroundColor&&) = default;
	~ChangeBackGroundColor() = default;
	virtual void redo() override;
	virtual void undo() override;
private:
	Canvas* m_canvas;
	QColor m_backup;
	QColor m_target;
};
#endif // !COMMAND_H_