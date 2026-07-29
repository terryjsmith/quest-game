using System;
using System.Runtime.CompilerServices;

namespace QGEngine
{
    public partial class QGScript : QGComponent
    {
        // Declare our overridable functions
        public virtual void Initialize() { }
        public virtual void Update(float delta) { }
        public virtual void FixedUpdate(float delta) { }
        public virtual void Destroy() { }
    }
}