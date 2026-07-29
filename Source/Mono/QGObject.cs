using System;
using System.Runtime.CompilerServices;

namespace QGEngine
{
    public partial class QGObject
    {
        protected internal IntPtr ptr;

        public QGObject()
        {
            if (ptr == IntPtr.Zero)
                ptr = internal_QGObject_Ctor(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static IntPtr internal_QGObject_Ctor(QGObject obj);
    }
}