
using System;
using QGEngine;

namespace QuestGame
{
    public class QuestPlayer : QGScript
    {
        private float moveSpeed = 0.0f;
        private float turnSpeed = 0.0f;
        private float moveModifier = 1.0f;
        private float turnModifier = 20.0f;

        private QGCameraComponent camera = null;
        private float cameraDistance = -5.0f;

        public override void Initialize()
        {
            QGEventSystem.Subscribe<QGInputCommand>(InputCommandCallback, this.gameObject);
            this.camera = this.gameObject.GetComponent<QGCameraComponent>();
        }

        public void InputCommandCallback(QGEvent ev, QGObject obj)
        {
            QGInputCommand command = (QGInputCommand)ev;
            if (command.command == "MOVE")
            {
                Console.WriteLine("Setting move speed for player ID " + this.gameObject.id.ToString() + " to " + command.state.ToString());
                moveSpeed = command.state;
            }

            if (command.command == "TURN")
            {
                Console.WriteLine("Setting turn speed for player ID " + this.gameObject.id.ToString() + " to " + command.state.ToString());
                turnSpeed = command.state;
            }
        }

        public override void Update(float delta)
        {
            // Update entity position
            this.gameObject.transform.Move(delta * moveSpeed * moveModifier * this.gameObject.transform.forward);
            this.gameObject.transform.Rotate(this.gameObject.transform.up, delta * turnSpeed * turnModifier);

            Vector3 playerPosition = this.gameObject.transform.position;
            Console.WriteLine("Game object position: (" + playerPosition.x.ToString() + "," + playerPosition.y.ToString() + "," + playerPosition.z.ToString() + ")");

            // Update camera position
            this.camera.transform.position = playerPosition + (this.gameObject.transform.forward * cameraDistance);

            // Update rotation to match look vector
            this.camera.transform.rotation = this.gameObject.transform.rotation;

            Vector3 cameraPosition = this.camera.transform.position;
            Console.WriteLine("Camera object position: (" + cameraPosition.x.ToString() + "," + cameraPosition.y.ToString() + "," + cameraPosition.z.ToString() + ")");
        }
    }
}