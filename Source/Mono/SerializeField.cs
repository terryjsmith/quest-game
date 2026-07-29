using System;
using System.Runtime.CompilerServices;

namespace QGEngine
{
    [System.AttributeUsage(System.AttributeTargets.Field)]
    public class SerializeField : System.Attribute
    {
        public SerializeField() { }
    }
}