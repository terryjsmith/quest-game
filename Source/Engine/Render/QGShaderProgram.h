
#ifndef qgshaderprogram_h
#define qgshaderprogram_h

#include <Render/QGShader.h>

class QUEST_API QGShaderProgram : public QGObject {
public:
	QGShaderProgram() {
		vertexShader = 0;
		fragmentShader = 0;
		geometryShader = 0;
		tesselationControlShader = 0;
		tesselationEvalShader = 0;
	}

	virtual ~QGShaderProgram() = default;

	/**
	 * Bind / use shader program
	 */
	virtual void Bind() {}

	/**
	* Variable setting functions
	*/
	virtual void Set(std::string name, float value) {}
	virtual void Set(std::string name, vector2 value) {}
	virtual void Set(std::string name, vector3 value) {}
	virtual void Set(std::string name, vector4 value) {}
	virtual void Set(std::string name, int value) {}
	virtual void Set(std::string name, unsigned int value) {}
	virtual void Set(std::string name, matrix4 value) {}
	virtual void Set(std::string name, matrix3 value) {}

public:
	QGShader* vertexShader;
	QGShader* fragmentShader;
	QGShader* geometryShader;
	QGShader* tesselationControlShader;
	QGShader* tesselationEvalShader;
};

#endif