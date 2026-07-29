using System;
using System.Runtime.CompilerServices;
using System.Collections.Generic;

namespace QGEngine
{
    // Define the blueprint for the callback function
    public delegate void QGEventHandlerCallback(QGEvent ev, QGObject obj);

    public partial class QGEventSystem
    {
        public static void Subscribe<T>(QGEventHandlerCallback func, QGObject obj = null)
        {
            RegisteredCallback callback = new RegisteredCallback();
            callback.type = typeof(T);
            callback.callback = func;
            callback.obj = obj;

            m_callbacks.Add(callback);
        }

        public static void Publish(QGEvent ev, QGObject obj)
        {
            // Send to subscribers
            foreach (RegisteredCallback callback in m_callbacks)
            {
                if (callback.type == ev.GetType())
                {
                    if (callback.obj == null || callback.obj == obj)
                    {
                        callback.callback.Invoke(ev, obj);
                    }
                }
            }
        }

        public class RegisteredCallback
        {
            public Type type;
            public QGEventHandlerCallback callback;
            public QGObject obj;
        }

        protected static List<RegisteredCallback> m_callbacks = new List<RegisteredCallback>();
    }
}