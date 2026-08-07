using System;
using System.Runtime.CompilerServices;
using System.Collections.Generic;

namespace QGEngine
{
    public partial class QGCameraComponent : QGComponent
    {
        public QGTransform transform
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
        }

        public float fov
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
            [MethodImpl(MethodImplOptions.InternalCall)]
            set;
        }
    }
}