
#ifndef qgglshaderprogram_h
#define qgglshaderprogram_h

#include <Render/QGShaderProgram.h>

class QUEST_API QGGLShaderProgram : public QGShaderProgram {
public:
	QGGLShaderProgram() : m_program(0) {}
	~QGGLShaderProgram() = default;

	/**
	 * Bind / use shader program
	 */
	void Bind();

	/**
	* Variable setting functions
	*/
	void Set(std::string name, float value);
	void Set(std::string name, vector2 value);
	void Set(std::string name, vector3 value);
	void Set(std::string name, vector4 value);
	void Set(std::string name, int value);
	void Set(std::string name, unsigned int value);
	void Set(std::string name, matrix4 value);
	void Set(std::string name, matrix3 value);

protected:
	unsigned int m_program;

	// Cache of program variables
	std::map<std::string, int> m_uniforms;
};

#endif