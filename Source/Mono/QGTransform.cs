using System;
using System.Runtime.CompilerServices;

namespace QGEngine
{
    public partial class QGTransform : QGObject
    {
        /**
	     * Move from current position (add to position)
	     */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Move(Vector3 amount);

        /**
	     * Rotate around an axis
	     */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Rotate(Vector3 axis, float degrees);

        /**
	     * Scale by a percentage
	     */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Scale(Vector3 amount);

        public Vector3 forward
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
        }

        public Vector3 up
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
        }

        public Vector3 right
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
        }

        public Vector3 position
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
            [MethodImpl(MethodImplOptions.InternalCall)]
            set;
        }

        public Vector3 scaling
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
            [MethodImpl(MethodImplOptions.InternalCall)]
            set;
        }

        public Quaternion rotation
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
            [MethodImpl(MethodImplOptions.InternalCall)]
            set;
        }
    }
}