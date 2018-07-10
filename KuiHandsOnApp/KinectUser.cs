using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Media;
using System.IO;

using MagicMirrorNETWrapper;

namespace KuiHandsOnApp
{
    class KinectUser
    {
        private Dictionary<string, SoundPlayer> sounds = new Dictionary<string, SoundPlayer>();

        private string currentlyPlaying = "";

        private UInt64 userId;

        private HandState stateLeft = HandState.Unknown;
        private HandState stateRight = HandState.Unknown;

        private Direction directionLeft = Direction.NONE;
        private Direction directionRight = Direction.NONE;

        public KinectUser(UInt64 id) {
            userId = id;

            loadSounds("audio");
        }

        public bool isStateChanged(HandState newStateL, HandState newStateR) {

            bool diff = false;

            if ((int)newStateL >= 2 && (int)newStateR >= 2)
            {
                diff = (newStateL != stateLeft) || (newStateR != stateRight);

                stateLeft = newStateL;
                stateRight = newStateR;
            }

            return diff;
        }

        public bool changedRight(Direction direction) {
            bool diff = false;

            if (direction != Direction.NONE)
            {
                diff = (direction != directionRight);
                directionRight = direction;
            }            

            return diff;
        }

        public bool changedLeft(Direction direction)
        {
            bool diff = false;

            if (direction != Direction.NONE)
            {
                diff = (direction != directionLeft);
                directionLeft = direction;
            }

            return diff;
        }

        public void playSound(string soundName) {
            try
            {
                //if (currentlyPlaying != "")
                //    sounds[currentlyPlaying].Stop();

                if (sounds.ContainsKey(soundName))
                    sounds[soundName].Play();
            }
            catch (Exception ex) {
               // Console.WriteLine("no such instrument: " + soundName);
            }
        }

        private void loadSounds(string sDir)
        {
            try
            {
                foreach (string d in Directory.GetDirectories(sDir))
                {
                    foreach (string f in Directory.GetFiles(d))
                    {
                        string file = Path.GetFileNameWithoutExtension(f);
                       
                        var player = new SoundPlayer(f);
                        player.Load();
                        sounds.Add(file, player);
                    }
                    loadSounds(d);
                }
            }
            catch (System.Exception excpt)
            {
                Console.WriteLine(excpt.Message);
            }
        }

    }
}
