
#include <Render/UI/QGUIScreen.h>

void QGUIScreen::Redraw() {
	for (auto it = m_children.begin(); it != m_children.end(); it++) {
		this->RecursiveRedraw(*it);
	}
}

void QGUIScreen::RecursiveRedraw(QGUIControl* node) {
	node->Redraw();

	auto children = node->Children();
	for (auto it = children.begin(); it != children.end(); it++) {
		this->RecursiveRedraw(*it);
	}
}