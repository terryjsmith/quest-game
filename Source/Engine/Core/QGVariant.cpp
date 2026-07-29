
#include <Core/QGVariant.h>

QGVariant::QGVariant() {
    m_type = 0;
    m_size = 0;
}

QGVariant::~QGVariant() {
    if (m_type == VAR_STRING) {
        free(m_data.str);
    }
}

QGVariant::QGVariant(int32_t value) {
    *this = value;
}

QGVariant::QGVariant(uint32_t value) {
    *this = value;
}

QGVariant::QGVariant(int64_t value) {
    *this = value;
}

QGVariant::QGVariant(uint64_t value) {
    *this = value;
}

QGVariant::QGVariant(bool value) {
    *this = value;
}

QGVariant::QGVariant(float value) {
    *this = value;
}

QGVariant::QGVariant(char* value) {
    *this = value;
}

QGVariant::QGVariant(vector2 value) {
    *this = value;
}

QGVariant::QGVariant(vector3 value) {
    *this = value;
}

QGVariant::QGVariant(vector4 value) {
    *this = value;
}

QGVariant::QGVariant(quaternion value) {
    *this = value;
}

QGVariant::QGVariant(std::string value) {
    *this = value;
}

QGVariant::QGVariant(QGObject* value) {
    *this = value;
}

QGVariant::QGVariant(const QGVariant& value) {
    *this = value;
}

QGVariant& QGVariant::operator =(int32_t rhs) {
    m_type = VAR_INT32;
    m_data.i32 = rhs;
    m_size = sizeof(int32_t);
    return *this;
}

QGVariant& QGVariant::operator =(uint32_t rhs) {
    m_type = VAR_UINT32;
    m_data.ui32 = rhs;
    m_size = sizeof(uint32_t);
    return *this;
}

QGVariant& QGVariant::operator =(int64_t rhs) {
    m_type = VAR_INT64;
    m_data.i64 = rhs;
    m_size = sizeof(int64_t);
    return *this;
}

QGVariant& QGVariant::operator =(uint64_t rhs) {
    m_type = VAR_UINT64;
    m_data.ui64 = rhs;
    m_size = sizeof(uint64_t);
    return *this;
}

QGVariant& QGVariant::operator =(bool rhs) {
    m_type = VAR_BOOL;
    m_data.b = rhs;
    m_size = sizeof(bool);
    return *this;
}

QGVariant& QGVariant::operator =(float rhs) {
    m_type = VAR_FLOAT;
    m_data.f1 = rhs;
    m_size = sizeof(float);
    return *this;
}

QGVariant& QGVariant::operator =(char* rhs) {
    m_type = VAR_STRING;
    int length = strlen(rhs);
    m_data.str = (char*)malloc(length + 1);
    memcpy(m_data.str, rhs, length);
    m_data.str[length] = '\0';
    m_size = length + 1;
    return *this;
}

QGVariant& QGVariant::operator =(vector2 rhs) {
    m_type = VAR_VECTOR2;
    m_data.vec[0] = rhs.x;
    m_data.vec[1] = rhs.y;
    m_size = sizeof(float) * 2;
    return *this;
}

QGVariant& QGVariant::operator =(vector3 rhs) {
    m_type = VAR_VECTOR3;
    m_data.vec[0] = rhs.x;
    m_data.vec[1] = rhs.y;
    m_data.vec[2] = rhs.z;
    m_size = sizeof(float) * 3;
    return *this;
}

QGVariant& QGVariant::operator =(vector4 rhs) {
    m_type = VAR_VECTOR4;
    m_data.vec[0] = rhs.x;
    m_data.vec[1] = rhs.y;
    m_data.vec[2] = rhs.z;
    m_data.vec[3] = rhs.w;
    m_size = sizeof(float) * 4;
    return *this;
}

QGVariant& QGVariant::operator =(quaternion rhs) {
    m_type = VAR_QUATERNION;
    m_data.vec[0] = rhs.x;
    m_data.vec[1] = rhs.y;
    m_data.vec[2] = rhs.z;
    m_data.vec[3] = rhs.w;
    m_size = sizeof(float) * 4;
    return *this;
}

QGVariant& QGVariant::operator =(std::string rhs) {
    m_type = VAR_STRING;
    int length = rhs.length();

    m_data.str = (char*)malloc(length + 1);
    memcpy(m_data.str, rhs.data(), length);
    m_data.str[length] = '\0';
    m_size = length;
    return *this;
}

QGVariant& QGVariant::operator =(QGObject* rhs) {
    m_type = VAR_OBJECT;
    m_data.obj = rhs;

    // what to do about m_size here?
    return *this;
}

QGVariant& QGVariant::operator =(const QGVariant& rhs) {
    m_type = rhs.m_type;
    m_data = rhs.m_data;
    m_size = rhs.m_size;

    if (m_type == VAR_STRING) {
        int length = strlen(rhs.m_data.str);
        m_data.str = (char*)malloc(length + 1);
        strcpy(m_data.str, rhs.m_data.str);
        m_data.str[length] = '\0';
    }

    return *this;
}

bool QGVariant::operator ==(const QGVariant& rhs) const {
    if (m_type == VAR_FLOAT) {
        if (m_data.f1 == rhs.m_data.f1) {
            return(true);
        }

        int val = static_cast<int>(m_data.f1);
        if (rhs.m_type == VAR_INT32) {
            if (rhs.m_data.i32 == val) {
                return(true);
            }
        }

        if (rhs.m_type == VAR_UINT32) {
            if (rhs.m_data.ui32 == val) {
                return(true);
            }
        }
    }

    if (m_type == VAR_INT32) {
        if (m_data.i32 == rhs.m_data.i32) {
            return(true);
        }

        if (rhs.m_type == VAR_UINT32) {
            if (rhs.m_data.ui32 == m_data.i32) {
                return(true);
            }
        }

        if (rhs.m_type == VAR_FLOAT) {
            float val = static_cast<float>(m_data.i32);
            if (rhs.m_data.f1 == val) {
                return(true);
            }
        }
    }

    if (m_type == VAR_UINT32) {
        if (m_data.ui32 == rhs.m_data.ui32) {
            return(true);
        }

        if (rhs.m_type == VAR_INT32) {
            if (rhs.m_data.i32 == m_data.ui32) {
                return(true);
            }
        }

        if (rhs.m_type == VAR_FLOAT) {
            float val = static_cast<float>(m_data.ui32);
            if (rhs.m_data.f1 == val) {
                return(true);
            }
        }
    }

    if (m_type == VAR_INT64) {
        if (m_data.i64 == rhs.m_data.i64) {
            return(true);
        }
    }

    if (m_type == VAR_UINT32) {
        if (m_data.ui64 == rhs.m_data.ui64) {
            return(true);
        }
    }

    // From here on out, if the types don't match, return false
    if (m_type != rhs.m_type) {
        return(false);
    }

    if (m_type == VAR_BOOL) {
        return(m_data.b == rhs.m_data.b);
    }

    if (m_type == VAR_VECTOR2) {
        if (m_data.vec[0] == rhs.m_data.vec[0] && m_data.vec[1] == rhs.m_data.vec[1]) {
            return(true);
        }
    }

    if (m_type == VAR_VECTOR3) {
        if (m_data.vec[0] == rhs.m_data.vec[0] && m_data.vec[1] == rhs.m_data.vec[1] && m_data.vec[2] == rhs.m_data.vec[2]) {
            return(true);
        }
    }

    if (m_type == VAR_VECTOR4 || m_type == VAR_QUATERNION) {
        if (m_data.vec[0] == rhs.m_data.vec[0] && m_data.vec[1] == rhs.m_data.vec[1] && m_data.vec[2] == rhs.m_data.vec[2]
            && m_data.vec[3] == rhs.m_data.vec[3]) {
            return(true);
        }
    }

    if (m_type == VAR_STRING) {
        if (strcmp(m_data.str, rhs.m_data.str) == 0) {
            return(true);
        }
    }

    if (m_type == VAR_OBJECT) {
        return(m_data.obj == rhs.m_data.obj);
    }

    return(false);
}

bool QGVariant::operator !=(const QGVariant& rhs) const {
    if (rhs == this) return(false);
    return(true);
}

bool QGVariant::operator ==(const int32_t& rhs) const {
    if (m_type == VAR_INT32) {
        if (rhs == m_data.i32)
            return(true);
    }

    if (m_type == VAR_UINT32) {
        if (rhs == m_data.ui32)
            return(true);
    }

    if (m_type == VAR_FLOAT) {
        float f = rhs;
        if (f == m_data.f1)
            return(true);
    }

    return(false);
}

bool QGVariant::operator ==(const uint32_t& rhs) const {
    if (m_type == VAR_INT32) {
        if (rhs == m_data.i32)
            return(true);
    }

    if (m_type == VAR_UINT32) {
        if (rhs == m_data.ui32)
            return(true);
    }

    if (m_type == VAR_FLOAT) {
        float f = rhs;
        if (f == m_data.f1)
            return(true);
    }

    return(false);
}

bool QGVariant::operator ==(const int64_t& rhs) const {
    if (m_type == VAR_INT64) {
        if (rhs == m_data.i64)
            return(true);
    }

    if (m_type == VAR_UINT64) {
        if (rhs == m_data.ui64)
            return(true);
    }

    return(false);
}

bool QGVariant::operator ==(const uint64_t& rhs) const {
    if (m_type == VAR_INT64) {
        if (rhs == m_data.i64)
            return(true);
    }

    if (m_type == VAR_UINT64) {
        if (rhs == m_data.ui64)
            return(true);
    }

    return(false);
}

bool QGVariant::operator ==(const bool& rhs) const {
    if (m_type == VAR_BOOL) {
        if (rhs == m_data.b)
            return(true);
    }

    return(false);
}

bool QGVariant::operator ==(const float& rhs) const {
    if (m_type == VAR_INT32) {
        if (rhs == (float)m_data.i32)
            return(true);
    }

    if (m_type == VAR_UINT32) {
        if (rhs == (float)m_data.ui32)
            return(true);
    }

    if (m_type == VAR_FLOAT) {
        if (rhs == m_data.f1)
            return(true);
    }

    return(false);
}

bool QGVariant::operator ==(const vector2& rhs) const {
    if (m_type == VAR_VECTOR2) {
        if (rhs.x == m_data.vec[0] && rhs.y == m_data.vec[2])
            return(true);
    }

    return(false);
}

bool QGVariant::operator ==(const vector3& rhs) const {
    if (m_type == VAR_VECTOR3) {
        if (rhs.x == m_data.vec[0] && rhs.y == m_data.vec[1] && rhs.z == m_data.vec[2])
            return(true);
    }

    return(false);
}

bool QGVariant::operator ==(const vector4& rhs) const {
    if (m_type == VAR_VECTOR4) {
        if (rhs.x == m_data.vec[0] && rhs.y == m_data.vec[1] && rhs.z == m_data.vec[2] && rhs.w == m_data.vec[3])
            return(true);
    }

    return(false);
}

bool QGVariant::operator ==(const quaternion& rhs) const {
    if (m_type == VAR_VECTOR4) {
        if (rhs.x == m_data.vec[0] && rhs.y == m_data.vec[1] && rhs.z == m_data.vec[2] && rhs.w == m_data.vec[3])
            return(true);
    }

    return(false);
}

bool QGVariant::operator ==(const QGObject*& rhs) const {
    if (m_type == VAR_OBJECT) {
        if (rhs == m_data.obj)
            return(true);
    }

    return(false);
}

bool QGVariant::IsInt() {
    return(m_type == VAR_INT32 || m_type == VAR_INT64);
}

bool QGVariant::IsUInt() {
    return(m_type == VAR_UINT32 || m_type == VAR_UINT64);
}

bool QGVariant::IsInt64() {
    return(m_type == VAR_INT64);
}

bool QGVariant::IsUInt64() {
    return(m_type == VAR_UINT64);
}

bool QGVariant::IsBool() {
    return(m_type == VAR_BOOL);
}

bool QGVariant::IsFloat() {
    return(m_type == VAR_FLOAT || m_type == VAR_INT32 || m_type == VAR_UINT32);
}

bool QGVariant::IsNumeric() {
    return(m_type == VAR_INT32 || m_type == VAR_UINT32 || m_type == VAR_INT64 || m_type == VAR_UINT64 || m_type == VAR_FLOAT);
}

bool QGVariant::IsVector2() {
    return(m_type == VAR_VECTOR2);
}

bool QGVariant::IsVector3() {
    return(m_type == VAR_VECTOR3);
}

bool QGVariant::IsVector4() {
    return(m_type == VAR_VECTOR4);
}

bool QGVariant::IsQuaternion() {
    return(m_type == VAR_QUATERNION);
}

bool QGVariant::IsString() {
    return(m_type == VAR_STRING);
}

bool QGVariant::IsObject() {
    return(m_type == VAR_OBJECT);
}

bool QGVariant::IsNull() {
    return(m_type == 0);
}

bool QGVariant::IsArray() {
    return(m_type == VAR_ARRAY);
}

std::string QGVariant::ToString() {
    std::string output;
    int length = 0;
    switch (m_type) {
    case VAR_INT32:
        output = std::to_string(m_data.i32);
        break;
    case VAR_UINT32:
        output = std::to_string(m_data.ui32);
        break;
    case VAR_UINT64:
        output = std::to_string(m_data.ui64);
        break;
    case VAR_INT64:
        output = std::to_string(m_data.i64);
        break;
    case VAR_BOOL:
        output = m_data.b ? "true" : "false";
        break;
    case VAR_FLOAT:
        output = std::to_string(m_data.f1);
        break;
    case VAR_VECTOR2:
        output.resize(50);
        length = sprintf((char*)output.data(), "(%.2f, %.2f)", m_data.vec[0], m_data.vec[1]);
        output.resize(length);
        break;
    case VAR_VECTOR3:
        output.resize(50);
        length = sprintf((char*)output.data(), "(%.2f, %.2f, %.2f)", m_data.vec[0], m_data.vec[1], m_data.vec[2]);
        output.resize(length);
        break;
    case VAR_VECTOR4:
        output.resize(50);
        length = sprintf((char*)output.data(), "(%.2f, %.2f, %.2f, %.2f)", m_data.vec[0], m_data.vec[1], m_data.vec[2], m_data.vec[3]);
        output.resize(length);
        break;
    case VAR_QUATERNION:
        output.resize(50);
        length = sprintf((char*)output.data(), "(%.2f, %.2f, %.2f, %.2f)", m_data.vec[0], m_data.vec[1], m_data.vec[2], m_data.vec[3]);
        output.resize(length);
        break;
    case VAR_STRING:
        output = m_data.str;
        break;
    case VAR_OBJECT:
        break;
    default:
        break;
    }

    return(output);
}

void QGVariant::FromString(std::string value, int type) {
    switch (type) {
    case VAR_INT32:
        m_data.i32 = std::stoi(value);
        m_type = VAR_INT32;
        break;
    case VAR_UINT32:
        m_data.ui32 = (uint32_t)std::stoul(value);
        m_type = VAR_UINT32;
        break;
    case VAR_UINT64:
        m_data.i64 = (uint64_t)std::stoull(value);
        m_type = VAR_UINT64;
        break;
    case VAR_INT64:
        m_data.i64 = (uint64_t)std::stoll(value);
        m_type = VAR_INT64;
        break;
    case VAR_BOOL:
        m_data.b = value.compare("true") == 0 ? true : false;
        m_type = VAR_BOOL;
        break;
    case VAR_FLOAT:
        m_data.f1 = std::stof(value);
        m_type = VAR_FLOAT;
        break;
    case VAR_VECTOR2:
        sscanf(value.c_str(), "(%f, %f)", &m_data.vec[0], &m_data.vec[1]);
        m_type = VAR_VECTOR2;
        break;
    case VAR_VECTOR3:
        sscanf(value.c_str(), "(%f, %f, %f)", &m_data.vec[0], &m_data.vec[1], &m_data.vec[2]);
        m_type = VAR_VECTOR3;
        break;
    case VAR_VECTOR4:
        sscanf(value.c_str(), "(%f, %f, %f, %f)", &m_data.vec[0], &m_data.vec[1], &m_data.vec[2], &m_data.vec[3]);
        m_type = VAR_VECTOR4;
        break;
    case VAR_QUATERNION:
        sscanf(value.c_str(), "(%f, %f, %f, %f)", &m_data.vec[3], &m_data.vec[0], &m_data.vec[1], &m_data.vec[2]);
        m_type = VAR_QUATERNION;
        break;
    case VAR_STRING:
        m_data.str = (char*)malloc(value.size() + 1);
        strcpy(m_data.str, value.c_str());
        m_data.str[value.size()] = '\0';
        m_type = VAR_STRING;
        break;
    case VAR_OBJECT:
        // NOt sure if we'll get back to this
        break;
    default:
        break;
    }
}

int32_t QGVariant::AsInt() {
    if (m_type == VAR_INT32) {
        return m_data.i32;
    }

    if (m_type == VAR_FLOAT) {
        return((int)m_data.f1);
    }

    if (m_type == VAR_STRING) {
        std::string strval = m_data.str;
        free(m_data.str);

        this->FromString(strval, VAR_INT32);
        return m_data.i32;
    }

    return(0);
}

uint32_t QGVariant::AsUInt() {
    if (m_type == VAR_UINT32) {
        return m_data.ui32;
    }

    if (m_type == VAR_FLOAT) {
        return((uint32_t)m_data.f1);
    }

    if (m_type == VAR_STRING) {
        std::string strval = m_data.str;
        free(m_data.str);

        this->FromString(strval, VAR_UINT32);
        return m_data.ui32;
    }

    return(0);
}

int64_t QGVariant::AsInt64() {
    if (m_type == VAR_INT64) {
        return m_data.i64;
    }

    if (m_type == VAR_UINT64) {
        return((int64_t)m_data.ui64);
    }

    if (m_type == VAR_INT32) {
        return((int64_t)m_data.i32);
    }

    if (m_type == VAR_UINT32) {
        return((int64_t)m_data.ui32);
    }

    if (m_type == VAR_FLOAT) {
        return((int64_t)m_data.f1);
    }

    if (m_type == VAR_STRING) {
        std::string strval = m_data.str;
        free(m_data.str);

        this->FromString(strval, VAR_INT64);
        return m_data.i64;
    }

    return(0);
}

uint64_t QGVariant::AsUInt64() {
    if (m_type == VAR_INT64) {
        return m_data.ui64;
    }

    if (m_type == VAR_UINT64) {
        return((uint64_t)m_data.ui64);
    }

    if (m_type == VAR_INT32) {
        return((uint64_t)m_data.i32);
    }

    if (m_type == VAR_UINT32) {
        return((uint64_t)m_data.ui32);
    }

    if (m_type == VAR_FLOAT) {
        return((uint64_t)m_data.f1);
    }

    if (m_type == VAR_STRING) {
        std::string strval = m_data.str;
        free(m_data.str);

        this->FromString(strval, VAR_UINT64);
        return m_data.ui64;
    }

    return(0);
}

bool QGVariant::AsBool() {
    if (m_type == VAR_BOOL) {
        return(m_data.b);
    }

    if (m_type == VAR_STRING) {
        std::string strval = m_data.str;
        free(m_data.str);

        this->FromString(strval, VAR_BOOL);
        return m_data.b;
    }

    return(false);
}

float QGVariant::AsFloat() {
    if (m_type == VAR_FLOAT) {
        return(m_data.f1);
    }

    if (m_type == VAR_INT32) {
        return((float)m_data.i32);
    }

    if (m_type == VAR_UINT32) {
        return((float)m_data.ui32);
    }

    if (m_type == VAR_STRING) {
        std::string strval = m_data.str;
        free(m_data.str);

        this->FromString(strval, VAR_FLOAT);
        return m_data.f1;
    }

    return(0.0f);
}

vector2 QGVariant::AsVector2() {
    if (m_type == VAR_VECTOR2) {
        return vector2(m_data.vec[0], m_data.vec[1]);
    }

    if (m_type == VAR_STRING) {
        std::string strval = m_data.str;
        free(m_data.str);

        this->FromString(strval, VAR_VECTOR2);
        return vector2(m_data.vec[0], m_data.vec[1]);
    }

    return(vector2(0, 0));
}

vector3 QGVariant::AsVector3() {
    if (m_type == VAR_VECTOR3) {
        return vector3(m_data.vec[0], m_data.vec[1], m_data.vec[2]);
    }

    if (m_type == VAR_STRING) {
        std::string strval = m_data.str;
        free(m_data.str);

        this->FromString(strval, VAR_VECTOR3);
        return vector3(m_data.vec[0], m_data.vec[1], m_data.vec[2]);
    }

    return(vector3(0, 0, 0));
}

vector4 QGVariant::AsVector4() {
    if (m_type == VAR_VECTOR4) {
        return vector4(m_data.vec[0], m_data.vec[1], m_data.vec[2], m_data.vec[3]);
    }

    if (m_type == VAR_STRING) {
        std::string strval = m_data.str;
        free(m_data.str);

        this->FromString(strval, VAR_VECTOR4);
        return vector4(m_data.vec[0], m_data.vec[1], m_data.vec[2], m_data.vec[3]);
    }

    return(vector4(0, 0, 0, 0));
}

quaternion QGVariant::AsQuaternion() {
    if (m_type == VAR_QUATERNION) {
        return quaternion(m_data.vec[3], m_data.vec[0], m_data.vec[1], m_data.vec[2]);
    }

    if (m_type == VAR_STRING) {
        std::string strval = m_data.str;
        free(m_data.str);

        this->FromString(strval, VAR_QUATERNION);
        return quaternion(m_data.vec[3], m_data.vec[0], m_data.vec[1], m_data.vec[2]);
    }

    return(quaternion(1, 0, 0, 0));
}

std::string QGVariant::AsString() {
    if (m_type == VAR_STRING) {
        return(m_data.str);
    }

    return(std::string(""));
}

QGObject* QGVariant::AsObject() {
    if (m_type == VAR_OBJECT) {
        return m_data.obj;
    }

    return(0);
}

QGVariant* QGVariant::AsVariant() {
    return(this);
}

void* QGVariant::GetPtr() {
    switch (m_type) {
    case VAR_INT32:
        return(&m_data.i32);
    case VAR_UINT32:
        return(&m_data.ui32);
    case VAR_UINT64:
        return(&m_data.i64);
    case VAR_INT64:
        return(&m_data.i64);
    case VAR_BOOL:
        return(&m_data.b);
    case VAR_FLOAT:
        return(&m_data.f1);
    case VAR_VECTOR2:
        return(&m_data.f1);
    case VAR_VECTOR3:
        return(&m_data.f1);
    case VAR_VECTOR4:
        return(&m_data.f1);
    case VAR_QUATERNION:
        return(&m_data.f1);
    case VAR_STRING:
        return(m_data.str);
    case VAR_OBJECT:
        return(&m_data.obj);
    default:
        break;
    }

    return(0);
}

void QGVariant::FromBytes(void* bytes, int type, int length) {
    m_type = type;

    if (type == VAR_INT32) {
        int32_t ui = 0;
        memcpy(&ui, bytes, sizeof(int32_t));
        *this = ui;
        return;
    }

    if (type == VAR_UINT32) {
        uint32_t ui = 0;
        memcpy(&ui, bytes, sizeof(uint32_t));
        *this = ui;
        return;
    }

    if (type == VAR_INT64) {
        int64_t ui = 0;
        memcpy(&ui, bytes, sizeof(int64_t));
        *this = ui;
        return;
    }

    if (type == VAR_UINT64) {
        uint64_t ui = 0;
        memcpy(&ui, bytes, sizeof(uint64_t));
        *this = ui;
        return;
    }

    if (type == VAR_UINT32) {
        uint64_t ui = 0;
        memcpy(&ui, bytes, sizeof(uint64_t));
        *this = ui;
        return;
    }

    if (type == VAR_BOOL) {
        bool ui = 0;
        memcpy(&ui, bytes, sizeof(bool));
        *this = ui;
        return;
    }

    if (type == VAR_FLOAT) {
        float ui = 0;
        memcpy(&ui, bytes, sizeof(float));
        *this = ui;
        return;
    }

    if (type == VAR_VECTOR2) {
        vector2 ui;
        unsigned char* ptr = (unsigned char*)bytes;

        int offset = 0;
        memcpy(&ui.x, ptr, sizeof(float));

        offset += sizeof(float);
        memcpy(&ui.y, ptr + offset, sizeof(float));

        *this = ui;
        return;
    }

    if (type == VAR_VECTOR3) {
        vector3 ui;
        unsigned char* ptr = (unsigned char*)bytes;

        int offset = 0;
        memcpy(&ui.x, ptr, sizeof(float));

        offset += sizeof(float);
        memcpy(&ui.y, ptr + offset, sizeof(float));

        offset += sizeof(float);
        memcpy(&ui.z, ptr + offset, sizeof(float));

        *this = ui;
        return;
    }

    if (type == VAR_VECTOR4) {
        vector4 ui;
        unsigned char* ptr = (unsigned char*)bytes;

        int offset = 0;
        memcpy(&ui.x, ptr, sizeof(float));

        offset += sizeof(float);
        memcpy(&ui.y, ptr + offset, sizeof(float));

        offset += sizeof(float);
        memcpy(&ui.z, ptr + offset, sizeof(float));

        offset += sizeof(float);
        memcpy(&ui.w, ptr + offset, sizeof(float));

        *this = ui;
        return;
    }

    if (type == VAR_QUATERNION) {
        quaternion ui;
        unsigned char* ptr = (unsigned char*)bytes;

        int offset = 0;
        memcpy(&ui.x, ptr, sizeof(float));

        offset += sizeof(float);
        memcpy(&ui.y, ptr + offset, sizeof(float));

        offset += sizeof(float);
        memcpy(&ui.z, ptr + offset, sizeof(float));

        offset += sizeof(float);
        memcpy(&ui.w, ptr + offset, sizeof(float));

        *this = ui;
        return;
    }

    if (type == VAR_STRING) {
        char* ui = new char[length + 1];
        memcpy(ui, bytes, length);
        ui[length] = '\0';
        *this = ui;
        free(ui);
        return;
    }

    QGASSERT(false, "Unrecognized variant type.");
}