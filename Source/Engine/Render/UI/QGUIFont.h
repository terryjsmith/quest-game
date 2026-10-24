
#ifndef qguifont_h
#define qguifont_h

#include <ft2build.h>
#include FT_FREETYPE_H

#include <IO/QGResourceObject.h>

class QUEST_API QGUIFontLoader;

class QUEST_API QGUIFont : public QGResourceObject {
public:
	QGUIFont() : m_face(0) {}
	~QGUIFont() = default;

	friend class QGUIFontLoader;

	FT_Face GetFace() { return m_face; }

protected:
	static FT_Library m_library;
	FT_Face m_face;
};

#endif