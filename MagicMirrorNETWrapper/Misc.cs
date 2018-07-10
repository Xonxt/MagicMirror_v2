using System;

namespace MagicMirrorNETWrapper
{
    /// <summary>
    /// Specifies LEFT or RIGHT hand for functions
    /// </summary>
    public enum Hand {
        /// <summary>
        /// LEFT HAND
        /// </summary>
        LEFT = 0,
        /// <summary>
        /// RIGHT HAND
        /// </summary>
        RIGHT = 1
    }

    /// <summary>
    /// The state of the specified hand, as seen by the Kinect device
    /// </summary>
    public enum HandState {
        /// <summary>
        /// State unknown
        /// </summary>
        Unknown = 0,
        /// <summary>
        /// Hand is not being tracked
        /// </summary>
        NotTracked = 1,
        /// <summary>
        /// Hand is an open palm
        /// </summary>
        Open = 2,
        /// <summary>
        /// Hand is closed into a fist
        /// </summary>
        Closed = 3,
        /// <summary>
        /// Fingers are looped into a lasso shape
        /// </summary>
        Lasso = 4
    }

    /// <summary>
    /// Movement directions for the hands
    /// </summary>
    public enum Direction {
        /// <summary>
        /// Hand is not moving
        /// </summary>
        NONE = 0,
        /// <summary>
        /// Hand is moving to the left
        /// </summary>
        LEFT,
        /// <summary>
        /// Hand is moving to the right
        /// </summary>
        RIGHT,
        /// <summary>
        /// Hand is moving downwards
        /// </summary>
        DOWN,
        /// <summary>
        /// Hand is moving upwards
        /// </summary>
        UP,
        /// <summary>
        /// Hand is moving forwards from the user
        /// </summary>
        FORWARDS,
        /// <summary>
        /// Hand is moving backwards to the user
        /// </summary>
        BACKWARDS
    }

    /// <summary>
    /// The identifiers for all the Kinect joints in the skeleton
    /// </summary>
    public enum KinectJoint
    {
        SpineBase = 0,
        SpineMid = 1,
        Torso = 1,
        Neck = 2,
        Head = 3,
        LeftShoulder = 4,
        LeftElbow = 5,
        LeftWrist = 6,
        LeftHand = 7,
        RightShoulder = 8,
        RightElbow = 9,
        RightWrist = 10,
        RightHand = 11,
        LeftHip = 12,
        LeftKnee = 13,
        LeftAnkle = 14,
        LeftFoot = 15,
        RightHip = 16,
        RightKnee = 17,
        RightAnkle = 18,
        RightFoot = 19,
        SpineShoulder = 20,
        HandTipLeft = 21,
        ThumbLeft = 22,
        HandTipRight = 23,
        ThumbRight = 24
    }

    /// <summary>
    /// A small class for various useful utilities
    /// </summary>
    public class Util
    {
        /// <summary>
        /// Log something into a text file
        /// </summary>
        /// <param name="message">The string message to log into the file</param>
        public static void Log(String message)
        {
            using (System.IO.StreamWriter file = new System.IO.StreamWriter(@"log.txt", true))
            {
                file.WriteLine(DateTime.Now.ToShortTimeString() + " ## " + message);
            }
        }
    }
}
