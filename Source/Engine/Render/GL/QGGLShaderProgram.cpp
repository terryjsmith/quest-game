
#ifdef USE_OPENGL

#include <Render/GL/QGGLShaderProgram.h>
#include <Render/GL/QGGL.h>

void QGGLShaderProgram::Bind() {
	if (m_program == 0) {
		QGASSERT(m_program == 0, "Program already initialized.");

		std::string vshader = (char*)vertexShader->resource->Data();
		std::string fshader = (char*)fragmentShader->resource->Data();

		// Create shaders
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Load vertex shader
		const char* strptr = vshader.c_str();
		glShaderSource(vertexShader, 1, &strptr, NULL);

		// Compile
		GL_CHECK(glCompileShader(vertexShader));

		// Check for compilation errors
		int compiled = 0, length;
		GL_CHECK(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled));
		if (compiled == 0) {
			GL_CHECK(glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &length));

			std::string errstr;
			errstr.resize(length);

			GL_CHECK(glGetShaderInfoLog(vertexShader, length, &length, (char*)errstr.c_str()));
			QGASSERT(false, "Error compiling shader source.");
		}

		// Load fragment shader source
		strptr = fshader.c_str();
		glShaderSource(fragmentShader, 1, &strptr, NULL);

		// Compile
		GL_CHECK(glCompileShader(fragmentShader));

		// Check for compilation errors
		GL_CHECK(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled));
		if (compiled == 0) {
			GL_CHECK(glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &length));

			std::string errstr;
			errstr.resize(length);

			GL_CHECK(glGetShaderInfoLog(fragmentShader, length, &length, (char*)errstr.c_str()));
			QGASSERT(false, "Error compiling shader source.");
		}

		// Load tesselation shaders
		unsigned int tessControlShader = 0;
		if (tesselationControlShader) {
			// Load shader source
			std::string tcsshader = (char*)tesselationControlShader->resource->Data();
			strptr = tcsshader.c_str();

			tessControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
			glShaderSource(tessControlShader, 1, &strptr, NULL);

			// Compile
			GL_CHECK(glCompileShader(tessControlShader));

			// Check for compilation errors
			GL_CHECK(glGetShaderiv(tessControlShader, GL_COMPILE_STATUS, &compiled));
			if (compiled == 0) {
				GL_CHECK(glGetShaderiv(tessControlShader, GL_INFO_LOG_LENGTH, &length));

				std::string errstr;
				errstr.resize(length);

				GL_CHECK(glGetShaderInfoLog(tessControlShader, length, &length, (char*)errstr.c_str()));
				QGASSERT(false, "Error compiling shader source.");
			}
		}

		unsigned int tessEvalShader = 0;
		if (tesselationEvalShader) {
			// Load shader source
			std::string tesshader = (char*)tesselationEvalShader->resource->Data();
			strptr = tesshader.c_str();

			tessEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
			glShaderSource(tessEvalShader, 1, &strptr, NULL);

			// Compile
			GL_CHECK(glCompileShader(tessEvalShader));

			// Check for compilation errors
			GL_CHECK(glGetShaderiv(tessEvalShader, GL_COMPILE_STATUS, &compiled));
			if (compiled == 0) {
				GL_CHECK(glGetShaderiv(tessEvalShader, GL_INFO_LOG_LENGTH, &length));

				std::string errstr;
				errstr.resize(length);

				GL_CHECK(glGetShaderInfoLog(tessEvalShader, length, &length, (char*)errstr.c_str()));
				QGASSERT(false, "Error compiling shader source.");
			}
		}

		// If that worked, create a program object
		m_program = glCreateProgram();

		// Attach shaders
		glAttachShader(m_program, vertexShader);
		glAttachShader(m_program, fragmentShader);
		if (tesselationControlShader) glAttachShader(m_program, tessControlShader);
		if (tesselationEvalShader) glAttachShader(m_program, tessEvalShader);

		GL_CHECK(glLinkProgram(m_program));

		// Attempt to compile / link program
		GL_CHECK(glGetProgramiv(m_program, GL_LINK_STATUS, &compiled));
		if (compiled == 0) {
			glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);

			std::string errstr;
			errstr.resize(length);
			glGetProgramInfoLog(m_program, length, &length, (char*)errstr.c_str());

			// ... and throw an error
			QGASSERT(false, "Error compiling shader program.");
		}

		// We have a compiled program, build the variable caches
		GL_CHECK(glUseProgram(m_program));

		// Find out how many variables we have of each type
		GLint activeAttribs = 0;
		GLint activeUniforms = 0;
		GL_CHECK(glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES, &activeAttribs));
		GL_CHECK(glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &activeUniforms));

		// Start with the uniform variables
		for (int i = 0; i < activeUniforms; i++) {
			int size = 0;
			int length = 0;
			unsigned int type = 0;

			char name[256];
			memset(name, 0, 256);

			// Get the name of the uniform from the program
			GL_CHECK(glGetActiveUniform(m_program, i, 256, &length, &size, &type, name));

			name[length] = '\0';

			// Then tell the program to go get and store its location
			m_uniforms[name] = glGetUniformLocation(m_program, name);
		}

		return;
	}

	// We have a compiled program, build the variable caches
	GL_CHECK(glUseProgram(m_program));
}

void QGGLShaderProgram::Set(std::string name, float value) {
	auto uit = m_uniforms.find(name);
	if (uit == m_uniforms.end()) {
		QGASSERT(false, "Bad shader variable name.");
		return;
	}

	GL_CHECK(glUniform1f(uit->second, value));
}

void QGGLShaderProgram::Set(std::string name, vector3 value) {
	auto uit = m_uniforms.find(name);
	if (uit == m_uniforms.end()) {
		QGASSERT(false, "Bad shader variable name.");
		return;
	}

	GL_CHECK(glUniform3fv(uit->second, 1, &(value)[0]));
}

void QGGLShaderProgram::Set(std::string name, vector2 value) {
	auto uit = m_uniforms.find(name);
	if (uit == m_uniforms.end()) {
		QGASSERT(false, "Bad shader variable name.");
		return;
	}

	GL_CHECK(glUniform2fv(uit->second, 1, &(value)[0]));
}

void QGGLShaderProgram::Set(std::string name, vector4 value) {
	auto uit = m_uniforms.find(name);
	if (uit == m_uniforms.end()) {
		QGASSERT(false, "Bad shader variable name.");
		return;
	}

	GL_CHECK(glUniform4fv(uit->second, 1, &(value)[0]));
}

void QGGLShaderProgram::Set(std::string name, int value) {
	auto uit = m_uniforms.find(name);
	if (uit == m_uniforms.end()) {
		QGASSERT(false, "Bad shader variable name.");
		return;
	}

	GL_CHECK(glUniform1i(uit->second, value));
}

void QGGLShaderProgram::Set(std::string name, unsigned int value) {
	auto uit = m_uniforms.find(name);
	if (uit == m_uniforms.end()) {
		QGASSERT(false, "Bad shader variable name.");
		return;
	}

	GL_CHECK(glUniform1ui(uit->second, value));
}

void QGGLShaderProgram::Set(std::string name, matrix4 value) {
	auto uit = m_uniforms.find(name);
	if (uit == m_uniforms.end()) {
		QGASSERT(false, "Bad shader variable name.");
		return;
	}

	GL_CHECK(glUniformMatrix4fv(uit->second, 1, GL_FALSE, &(value)[0][0]));
}

void QGGLShaderProgram::Set(std::string name, matrix3 value) {
	auto uit = m_uniforms.find(name);
	if (uit == m_uniforms.end()) {
		//QGASSERT(false, "Bad shader variable name.");
		return;
	}

	GL_CHECK(glUniformMatrix3fv(uit->second, 1, GL_FALSE, &(value)[0][0]));
}

#endif