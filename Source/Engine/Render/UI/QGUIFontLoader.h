
#ifndef qguifontloader_h
#define qguifontloader_h

#include <Render/UI/QGUIFont.h>
#include <IO/QGResourceLoader.h>

class QUEST_API QGUIFontLoader : public QGResourceLoader {
public:
	QGUIFontLoader() = default;
	~QGUIFontLoader() = default;

	QGResourceObject* LoadResource(QGResource* resource, std::string type);
};

#endif