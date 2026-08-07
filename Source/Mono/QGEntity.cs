using System;
using System.Runtime.CompilerServices;

namespace QGEngine
{
    public partial class QGEntity : QGObject
    {
        public T GetComponent<T>()
        {
            foreach (QGComponent component in components)
            {
                if (component is T comp)
                {
                    return (comp);
                }
            }

            return (default(T));
        }

        private QGComponent[] components
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
        }

        public uint id
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
            [MethodImpl(MethodImplOptions.InternalCall)]
            set;
        }

        public string name
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
            [MethodImpl(MethodImplOptions.InternalCall)]
            set;
        }

        public QGTransform transform
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
        }
    }
}