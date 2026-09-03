
#ifndef qgvariant_h
#define qgvariant_h

#include <Core/QGObject.h>

/**
 * A variant generic data storage
 */
class QUEST_API QGVariant {
public:
    QGVariant();
    virtual ~QGVariant();

    // Supported types
    enum Type {
        VAR_INT32 = 1,  // 1
        VAR_UINT32,     // 2
        VAR_INT64,      // 3
        VAR_UINT64,     // 4
        VAR_BOOL,       // 5
        VAR_FLOAT,      // 6
        VAR_VECTOR2,    // 7
        VAR_VECTOR3,    // 8
        VAR_VECTOR4,    // 9
        VAR_QUATERNION, // 10
        VAR_STRING,     // 11
        VAR_OBJECT,     // 12
        VAR_ARRAY,      // 13
        VAR_VOID,       // 14
        VAR_VARIANT,    // 15
        VAR_LASTTYPE    // 16
    };

    struct Value {
        union {
            int32_t i32;
            uint32_t ui32;
            int64_t i64;
            uint64_t ui64;
            bool b;
            float f1;
            char* str;
            void* ptr;
            QGObject* obj;
            float vec[4];
        };
    };

    /**
     * Constructors
     */
    QGVariant(int32_t value);
    QGVariant(uint32_t value);
    QGVariant(int64_t value);
    QGVariant(uint64_t value);
    QGVariant(bool value);
    QGVariant(float value);
    QGVariant(char* value);
    QGVariant(vector2 value);
    QGVariant(vector3 value);
    QGVariant(vector4 value);
    QGVariant(quaternion value);
    QGVariant(std::string value);
    QGVariant(QGObject* value);
    QGVariant(const QGVariant& value);
    template<typename T> QGVariant(std::vector<T> list) {
        *this = list;
    }

    /**
     * Set operators
     */
    QGVariant& operator =(int32_t rhs);
    QGVariant& operator =(uint32_t rhs);
    QGVariant& operator =(int64_t rhs);
    QGVariant& operator =(uint64_t rhs);
    QGVariant& operator =(bool rhs);
    QGVariant& operator =(float rhs);
    QGVariant& operator =(char* rhs);
    QGVariant& operator =(vector2 rhs);
    QGVariant& operator =(vector3 rhs);
    QGVariant& operator =(vector4 rhs);
    QGVariant& operator =(quaternion rhs);
    QGVariant& operator =(std::string rhs);
    QGVariant& operator =(QGObject* rhs);
    QGVariant& operator =(const QGVariant& rhs);

    bool operator ==(const QGVariant& rhs) const;
    bool operator ==(const int32_t& rhs) const;
    bool operator ==(const uint32_t& rhs) const;
    bool operator ==(const int64_t& rhs) const;
    bool operator ==(const uint64_t& rhs) const;
    bool operator ==(const bool& rhs) const;
    bool operator ==(const float& rhs) const;
    bool operator ==(const vector2& rhs) const;
    bool operator ==(const vector3& rhs) const;
    bool operator ==(const vector4& rhs) const;
    bool operator ==(const quaternion& rhs) const;
    bool operator ==(const QGObject*& rhs) const;

    bool operator !=(const QGVariant& rhs) const;

    /**
     * Checkers
     */
    bool IsInt();
    bool IsUInt();
    bool IsInt64();
    bool IsUInt64();
    bool IsBool();
    bool IsFloat();
    bool IsNumeric();
    bool IsVector2();
    bool IsVector3();
    bool IsVector4();
    bool IsQuaternion();
    bool IsString();
    bool IsObject();
    bool IsNull();
    bool IsArray();
    bool IsVariant() { return true; }

    /**
     * Convert value to string
     */
    std::string ToString();

    /**
     * Convert value from string
     */
    void FromString(std::string value, int type);

    /**
     * From binary data
     */
    void FromBytes(void* bytes, int type, int length = 0);

    /**
     * Get operators
     */
    int32_t AsInt();
    uint32_t AsUInt();
    int64_t AsInt64();
    uint64_t AsUInt64();
    bool AsBool();
    float AsFloat();
    vector2 AsVector2();
    vector3 AsVector3();
    vector4 AsVector4();
    quaternion AsQuaternion();
    std::string AsString();
    QGObject* AsObject();
    QGVariant* AsVariant();
    template<class T> T AsObject() {
        if (m_type == VAR_OBJECT) {
            return(dynamic_cast<T>(m_data.obj));
        }

        return(0);
    }

    /**
     * Get raw value
     */
    void* GetPtr();

    /**
     * Get type
     */
    int Type() { return m_type; }

    /**
     * Get binary size
     */
    uint32_t Size() { return m_size; }

protected:
    // Type
    int m_type;

    // Size in bytes (or size of array)
    uint32_t m_size;

    // Value data
    Value m_data;
};

#endif