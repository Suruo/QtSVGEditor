#ifndef SVGEDITOR_H_
#define SVGEDITOR_H_

#include <QtWidgets/QMainWindow>
#include "ui_SvgEditor.h"

#include "canvas.h"

class SvgEditor : public QMainWindow
{
    Q_OBJECT

public:
    SvgEditor(QWidget *parent = nullptr);
    ~SvgEditor();
public slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveFileToPng();
protected:
    void setLeftToobar();
    void setTopMenuBar();
    QWidget* DateSetting();
    void parseSvg(QFile* file);
    virtual bool eventFilter(QObject* watched, QEvent* event) override;
private:
    Ui::SvgEditorClass ui;
    Canvas* m_canvas;

};

#endif // !SVGEDITOR_H_