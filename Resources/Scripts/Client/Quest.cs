
using System;
using QGEngine;

namespace QuestGame { 
    public class Quest : QGObject {
        public int QuestID = 0;

        public string QuestName = string.Empty;
        public string Description = string.Empty;
        public int RequiredLevel = 0;
    }
}