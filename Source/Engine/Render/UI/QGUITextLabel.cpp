
#include <Render/UI/QGUITextLabel.h>

void QGUITextLabel::Redraw() {
	if (m_dirty == false) return;
	if (m_bytes) free(m_bytes);

	// Get font face
	FT_Face face = m_font->GetFace();

	// Set font size
	int error = FT_Set_Char_Size(
		face,				  /* handle to face object         */
		0,					  /* char_width in 1/64 of points  */
		m_fontSize * 64,	  /* char_height in 1/64 of points */
		0,	   	 			  /* horizontal device resolution  */
		0); 				  /* vertical device resolution    */

	// For each character...
	int finalWidth, maxUp, maxDown;
	finalWidth = maxUp = maxDown = 0;
	for (int i = 0; i < m_text.length(); i++) {
		// Get character
		char c = m_text[i];

		// Get character index into font
		int glyph_index = FT_Get_Char_Index(face, c);

		// Load character
		int error = FT_Load_Glyph(
			face,          /* handle to face object */
			glyph_index,   /* glyph index           */
			FT_LOAD_DEFAULT);

		// Render
		error = FT_Render_Glyph(face->glyph,    /* glyph slot  */
			FT_RENDER_MODE_NORMAL);				/* render mode */

		// Get glyph
		FT_GlyphSlot glyph = face->glyph;

		// Determine total height and total width, first just add width (in 1/64s of a pixel)
		finalWidth += glyph->advance.x >> 6;

		int up = glyph->metrics.horiBearingY >> 6;
		maxUp = std::max(up, maxUp);

		int down = (glyph->metrics.height - glyph->metrics.horiBearingY) >> 6;
		maxDown = std::max(down, maxDown);
	}

	// Compute final image height
	int finalHeight = maxUp + maxDown;

	// Set as fixed width and height
	m_fixedWidth = finalWidth;
	m_fixedHeight = finalHeight;

	// Create our texture
	m_bytes = (unsigned char*)malloc(finalWidth * finalHeight * 4);
	memset(m_bytes, 0, finalWidth * finalHeight * 4);

	// Copy our characters over
	int nextX = 0;
	for (int i = 0; i < m_text.length(); i++) {
		// Get character
		char c = m_text[i];

		// Get character index into font
		int glyph_index = FT_Get_Char_Index(face, c);

		// Load character
		int error = FT_Load_Glyph(
			face,          /* handle to face object */
			glyph_index,   /* glyph index           */
			FT_LOAD_DEFAULT);

		// Render
		error = FT_Render_Glyph(face->glyph,    /* glyph slot  */
			FT_RENDER_MODE_NORMAL);				/* render mode */

		// Get glyph
		FT_GlyphSlot glyph = face->glyph;

		// Calculate start Y
		int startY = maxUp - glyph->bitmap_top;
		int startX = nextX + glyph->bitmap_left;
		for (int y = 0; y < glyph->bitmap.rows; y++) {
			for (int x = 0; x < glyph->bitmap.width; x++) {
				int srcOffset = (y * glyph->bitmap.width) + x;
				int destOffset = (((startY + y) * finalWidth) + (startX + x)) * 4;
				m_bytes[destOffset + 0] = m_color.r;
				m_bytes[destOffset + 1] = m_color.g;
				m_bytes[destOffset + 2] = m_color.b;
				m_bytes[destOffset + 3] = glyph->bitmap.buffer[srcOffset];
			}
		}

		// Increment offset X
		nextX += glyph->advance.x >> 6;
	}

	m_dirty = false;
}