using System;

namespace QGEngine
{
    public class Vector3
    {
        public float x;
        public float y;
        public float z;

        public Vector3()
        {
            this.x = 0;
            this.y = 0;
            this.z = 0;
        }

        public Vector3(float x)
        {
            this.x = x;
            this.y = x;
            this.z = x;
        }

        public Vector3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public static Vector3 operator +(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
        }

        public static Vector3 operator -(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
        }

        public static Vector3 operator *(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
        }

        public static Vector3 operator *(Vector3 a, float b)
        {
            return new Vector3(a.x * b, a.y * b, a.z * b);
        }

        public static Vector3 operator *(float b, Vector3 a)
        {
            return new Vector3(a.x * b, a.y * b, a.z * b);
        }

        public static Vector3 operator /(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x / b.x, a.y / b.y, a.z / b.z);
        }

        public static Vector3 operator /(Vector3 a, float b)
        {
            return new Vector3(a.x / b, a.y / b, a.z / b);
        }

        public static Vector3 operator /(float b, Vector3 a)
        {
            return new Vector3(a.x / b, a.y / b, a.z / b);
        }

        public float Length()
        {
            return ((float)Math.Sqrt(this.x * this.x + this.y * this.y + this.z * this.z));
        }

        public void Normalize()
        {
            float length = this.Length();
            if (length == 0) return;

            this.x = this.x / length;
            this.y = this.y / length;
            this.z = this.z / length;
        }
    }
}