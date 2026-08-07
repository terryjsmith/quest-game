
#ifndef qgtexture2d_h
#define qgtexture2d_h

#include <IO/QGResourceObject.h>
#include <Render/QGRenderDefines.h>

class QUEST_API QGTexture2D : public QGResourceObject {
public:
	enum QGTextureDataType {
		QGTEXTURE_BYTE = 1,
		QGTEXTURE_FLOAT = 2
	};

	QGTexture2D() {
		width = 0;
		height = 0;
		channels = 0;
		type = QGTextureDataType::QGTEXTURE_BYTE;
		m_data = 0;

		minificationFilter = magnificationFilter = TEXTURE_FILTER_LINEAR;
		wrapX = wrapY = false;
	}
	virtual ~QGTexture2D() = default;

	/**
	 * Create from data
	 */
	void Create(int width, int height, int channels, QGTextureDataType type, void* data) {
		this->width = width;
		this->height = height;
		this->channels = channels;
		this->type = type;
		
		int bytesize = (type == QGTEXTURE_BYTE) ? sizeof(unsigned char) : sizeof(float);
		this->m_data = (void*)malloc(width * height * channels * bytesize);
		memcpy(this->m_data, data, width * height * channels * bytesize);
	}

	/**
	 * Use texture
	 */
	virtual void Bind(int slot) {}

	/**
	 * Get data
	 */
	virtual void* Data() { return m_data; }

public:
	int width;
	int height;
	int channels;
	QGTextureDataType type;

	unsigned int minificationFilter, magnificationFilter;
	bool wrapX, wrapY;

protected:
	void* m_data;
};

#endif