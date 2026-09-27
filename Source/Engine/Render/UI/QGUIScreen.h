
#ifndef qguiscreen_h
#define qguiscreen_h

#include <Render/UI/QGUIControl.h>

class QUEST_API QGUIScreen : public QGUIControl {
public:
	QGUIScreen() = default;
	~QGUIScreen() = default;

	void Redraw();

protected:
	void RecursiveRedraw(QGUIControl* node);
};

#endif