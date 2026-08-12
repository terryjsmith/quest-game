using System;
using System.Runtime.CompilerServices;

namespace QGEngine
{
    public partial class QGComponent : QGObject
    {
        public virtual void Serialize() { }
        public virtual void Deserialize() { }

        // Game object we are currently attached to
        public QGEntity gameObject
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
        }
    }
}