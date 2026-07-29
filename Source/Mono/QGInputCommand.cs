using System;
using System.Runtime.CompilerServices;
using System.Collections.Generic;

namespace QGEngine
{
    public partial class QGInputCommand : QGEvent
    {
        public string command
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
        }

        public int type
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
        }

        public float state
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
        }
    }
}