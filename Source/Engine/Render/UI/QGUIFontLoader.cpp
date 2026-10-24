
#include <Render/UI/QGUIFontLoader.h>

QGResourceObject* QGUIFontLoader::LoadResource(QGResource* resource, std::string type) {
	// Create a font
	QGUIFont* font = new QGUIFont();
	if (font->m_library == 0) {
		// Initialize library
		int error = FT_Init_FreeType(&font->m_library);
		QGASSERT(error == 0, "Unable to initialize freetype library.");
		return(0);
	}

	int error = FT_New_Memory_Face(font->m_library,
		resource->Data(),   /* first byte in memory */
		resource->filesize, /* size in bytes        */
		0,					/* face_index           */
		&font->m_face);
	QGASSERT(error == 0, "Unable to load font from memory: %s", resource->filename.c_str());

	return(font);
}