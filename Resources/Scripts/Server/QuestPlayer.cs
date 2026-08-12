
using System;
using QGEngine;

namespace QuestGame { 
    public class QuestPlayer : QGScript {
        private float moveSpeed = 0.0f;
        private float turnSpeed = 0.0f;

        private float moveModifier = 1.0f;
        private float turnModifier = 30.0f;

        public override void Initialize() {
            QGEventSystem.Subscribe<QGInputCommand>(InputCommandCallback, this.gameObject);
        }

        public void InputCommandCallback(QGEvent ev, QGObject obj) {
            QGInputCommand command = (QGInputCommand)ev;
            if(command.command == "MOVE")
            {
                Console.WriteLine("Setting move speed for player ID " + this.gameObject.id.ToString() + " to " + command.state.ToString());
                moveSpeed = command.state;
            }

            if(command.command == "TURN")
            {
                Console.WriteLine("Setting turn speed for player ID " + this.gameObject.id.ToString() + " to " + command.state.ToString());
                turnSpeed = command.state; 
            }
        }

        public override void Update(float delta) {
            this.gameObject.transform.Move(delta * moveSpeed * moveModifier * this.gameObject.transform.forward);
            this.gameObject.transform.Rotate(this.gameObject.transform.up, delta * turnSpeed * turnModifier);
        }
    }
}