using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Text;
using System.Runtime.InteropServices;

namespace MagicMirrorNETWrapper
{
    public class PoseRecognizer : IDisposable
    {
        // the c++ wrapper
        protected unsafe class Native
        {
            [DllImport("kernel32.dll", EntryPoint = "RtlMoveMemory", SetLastError = false)]
            public static extern void CopyMemory(IntPtr Destination, IntPtr Source, int Length);

            // constuctor, creates an object
            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_Create(ref IntPtr newInstance);

            // destructor, destroys the object
            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_Destroy(IntPtr instance);

            // initialize
            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_initialize(IntPtr instance, out bool result);

            // process next frame
            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_processNextFrame(IntPtr instance, out bool result);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getRgbFrame(IntPtr instance, byte* outputImage);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getSkeletonImage(IntPtr instance, byte* outputImage);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getDepthMap(IntPtr instance, byte* outputImage);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_userPresent(IntPtr instance, out bool result);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getJointRealworld(IntPtr instance, [MarshalAs(UnmanagedType.I4)] int jointNumber, out double x, out double y, out double z, out bool result);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getJointScene(IntPtr instance, [MarshalAs(UnmanagedType.I4)] int jointNumber, out double x, out double y, out double z, out bool result);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getPose(IntPtr instance, out int poseIndex, out bool result);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getPoseName(IntPtr instance, StringBuilder poseName, out bool result);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_addNewPose(IntPtr instance, [MarshalAs(UnmanagedType.LPStr)] string poseName, double dof);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_saveTrainingDataByName(IntPtr instance, [MarshalAs(UnmanagedType.LPStr)] string poseName);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_saveTrainingDataByIdx(IntPtr instance, [MarshalAs(UnmanagedType.I4)] int poseIndex);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getPoseList(IntPtr instance,
                [MarshalAs(UnmanagedType.LPArray)] out string[] poseList, out int size);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_expectPoseByName(IntPtr instance, [MarshalAs(UnmanagedType.LPStr)] string poseName, out double result);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_expectPoseByIdx(IntPtr instance, [MarshalAs(UnmanagedType.I4)] int poseIndex, out double result);

            // additional functions for the specifis user:
            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_expectUserPoseByName(IntPtr instance, [MarshalAs(UnmanagedType.U8)] UInt64 userId, 
                [MarshalAs(UnmanagedType.LPStr)] string poseName, out double result);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_expectUserPoseByIdx(IntPtr instance, [MarshalAs(UnmanagedType.U8)] UInt64 userId, 
                [MarshalAs(UnmanagedType.I4)] int poseIndex, out double result);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getUserJointRealworld(IntPtr instance, [MarshalAs(UnmanagedType.U8)] UInt64 userId, 
                [MarshalAs(UnmanagedType.I4)] int jointNumber, out double x, out double y, out double z, out bool result);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getUserJointScene(IntPtr instance, [MarshalAs(UnmanagedType.U8)] UInt64 userId, 
                [MarshalAs(UnmanagedType.I4)] int jointNumber, out double x, out double y, out double z, out bool result);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getUserPose(IntPtr instance, [MarshalAs(UnmanagedType.U8)] UInt64 userId, out int poseIndex, out bool result);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getUserPoseName(IntPtr instance, [MarshalAs(UnmanagedType.U8)] UInt64 userId, 
                StringBuilder poseName, out bool result);

            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getUserIdList(IntPtr instance, out int amount, ref IntPtr list);
            //public static extern bool PoseRecognizer_getUserIdList(IntPtr instance, out int amount, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] out UInt64[] list);

            // GET THE MOVEMENT DIRECTION OF THE SPECIFIED HAND FOR SPECIFIED USER
            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getUserHandDirection(IntPtr instance, [MarshalAs(UnmanagedType.U8)] UInt64 userId, [MarshalAs(UnmanagedType.I4)] int handNumber, out int direction, out bool result);

            // CHECK IF SPECIFIED HAND FOR SPECIFIED USER CHANGED DIRECTION
            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_userHandChangedDirection(IntPtr instance, [MarshalAs(UnmanagedType.U8)] UInt64 userId, [MarshalAs(UnmanagedType.I4)] int handNumber, out bool isChanged, out bool result);

            // RETURN STATE OF SPECIFIED HAND FOR SPECIFIED USER 
            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getUserHandState(IntPtr instance, [MarshalAs(UnmanagedType.U8)] UInt64 userId, [MarshalAs(UnmanagedType.I4)] int handNumber, out int handState, out bool result);

            // CHECK IF SPECIFIED HAND FOR SPECIFIED USER IS MOVING
            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_isUserHandMoving(IntPtr instance, [MarshalAs(UnmanagedType.U8)] UInt64 userId, [MarshalAs(UnmanagedType.I4)] int handNumber, out bool isMoving, out bool result);

            // RETURN MOVEMENT SPEED OF SPECIFIED HAND FOR SPECIFIED USER 
            [DllImport("MagicMirrorAPI.dll", CallingConvention = CallingConvention.StdCall)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool PoseRecognizer_getUserHandSpeed(IntPtr instance, [MarshalAs(UnmanagedType.U8)] UInt64 userId, [MarshalAs(UnmanagedType.I4)] int handNumber, out double handSpeed, out bool result);
        }

        // our C++ object instance
        private IntPtr instance = IntPtr.Zero;

        // constructor
        public PoseRecognizer()
        {
            if (!Native.PoseRecognizer_Create(ref instance))
                throw new InvalidOperationException("Internal error: cannot create instance of PoseRecognizer!");
        }

        // destructor
        public void Dispose()
        {
            if (!Native.PoseRecognizer_Destroy(instance))
                throw new InvalidOperationException("Internal error: cannot destroy instance of PoseRecognizer!");

            instance = IntPtr.Zero;
        }

        /// <summary>
        /// initialize the Pose Recognizer
        /// </summary>
        /// <returns>TRUE if initialization was successful</returns>
        public bool initialize()
        {
            bool result;

            if (!Native.PoseRecognizer_initialize(instance, out result))
                throw new InvalidOperationException("Internal error: cannot call PoseRecognizer.initialize()!");

            return result;
        }

        /// <summary>
        /// Get the next frame from Kinect and process it
        /// </summary>
        /// <returns>return TRUE if everything executed properly</returns>
        public bool processNextFrame()
        {
            bool result;

            if (!Native.PoseRecognizer_processNextFrame(instance, out result))
                throw new InvalidOperationException("Internal error: cannot call PoseRecognizer.processNextFrame()!");

            return result;
        }

        /// <summary>
        /// Checks whether there's a user recognized and tracked in the frame
        /// </summary>
        /// <returns>TRUE if at least one user is present and tracked</returns>
        public bool userPresent()
        {
            bool result;

            if (!Native.PoseRecognizer_userPresent(instance, out result))
                throw new InvalidOperationException("Internal error: cannot call PoseRecognizer.userPresent()!");

            return result;
        }

        /// <summary>
        /// Gets the "in scene" coordinated of the specified joint. The coordinated are for the 640x480
        /// depth image
        /// </summary>
        /// <param name="joint">Name of the joint from the KinectJoint enum</param>
        /// <returns>Coordinates of the joint</returns>
        public PointF getJointScene(KinectJoint joint)
        {
            double x, y, z;
            bool result;

            if (!Native.PoseRecognizer_getJointScene(instance, (int)joint, out x, out y, out z, out result))
                throw new NullReferenceException("Error while trying to read joint coordinates!");

            if (result)
                return new PointF((float)x, (float)y);
            else
                return new PointF(0, 0);
        }

        /// <summary>
        /// Gets the real world coordinates of the specified joint
        /// </summary>
        /// <param name="joint">Name of the joint</param>
        /// <returns>Coordinates of a joint as 3D-point</returns>
        public cvPoint3f getJointRealworld(KinectJoint joint)
        {
            double x, y, z;
            bool result;

            if (!Native.PoseRecognizer_getJointRealworld(instance, (int)joint, out x, out y, out z, out result))
                throw new NullReferenceException("Error while trying to read joint coordinates!");

            if (result)
                return new cvPoint3f(x, y, z);
            else
                return new cvPoint3f(0, 0, 0);
        }

        /// <summary>
        /// Gets the current RGB frame from Kinect
        /// </summary>
        /// <returns></returns>
        public Bitmap getRgbFrame()
        {
            Bitmap outputImage;
            bool success = false;
            BitmapData data0;

            // Util.Log("getting RGB file...");

            outputImage = new Bitmap(1920, 1080, PixelFormat.Format24bppRgb);
            Rectangle r = new Rectangle(Point.Empty, new Size(outputImage.Width, outputImage.Height));

            unsafe
            {
                byte* scan0;

                data0 = outputImage.LockBits(r, ImageLockMode.ReadWrite, outputImage.PixelFormat);
                scan0 = (byte*)data0.Scan0.ToPointer();

                success = Native.PoseRecognizer_getRgbFrame(instance, scan0);

                // Util.Log("RGB frame received!");

                outputImage.UnlockBits(data0);
            }

            if (success)
            {
                //outputImage.Save(DateTime.Now.ToShortTimeString() + "_pic.jpg");
                return outputImage;
            }
            else
            {
                outputImage.Dispose();
                throw new InvalidOperationException("Internal error: cannot call PoseRecognizer.getRgbFrame()!");
            }

        }

        /// <summary>
        /// Gets the Indexed image with the Skeleton from Kinect
        /// </summary>
        /// <returns></returns>
        public Bitmap getSkeletonImage()
        {
            Bitmap outputImage;
            bool success = false;
            BitmapData data0;

            outputImage = new Bitmap(512, 424, PixelFormat.Format24bppRgb);
            Rectangle r = new Rectangle(Point.Empty, new Size(outputImage.Width, outputImage.Height));

            unsafe
            {
                byte* scan0;

                data0 = outputImage.LockBits(r, ImageLockMode.ReadWrite, outputImage.PixelFormat);
                scan0 = (byte*)data0.Scan0.ToPointer();

                success = Native.PoseRecognizer_getSkeletonImage(instance, scan0);

                outputImage.UnlockBits(data0);
            }

            if (success)
            {
                return outputImage;
            }
            else
            {
                outputImage.Dispose();
                throw new InvalidOperationException("Internal error: cannot call PoseRecognizer.getSkeletonImage()!");
            }

        }

        /// <summary>
        /// Gets the depth map from Kinect
        /// </summary>
        /// <returns></returns>
        public Bitmap getDepthMap()
        {
            Bitmap outputImage;
            bool success = false;
            BitmapData data0;

            outputImage = new Bitmap(512, 424, PixelFormat.Format8bppIndexed);
            Rectangle r = new Rectangle(Point.Empty, new Size(outputImage.Width, outputImage.Height));

            unsafe
            {
                byte* scan0;

                data0 = outputImage.LockBits(r, ImageLockMode.ReadWrite, outputImage.PixelFormat);
                scan0 = (byte*)data0.Scan0.ToPointer();

                success = Native.PoseRecognizer_getDepthMap(instance, scan0);

                outputImage.UnlockBits(data0);
            }

            if (success)
            {
                return outputImage;
            }
            else
            {
                outputImage.Dispose();
                throw new InvalidOperationException("Internal error: cannot call getDepthMap()!");
            }
        }

        /// <summary>
        /// Returns the name of the current pose as a string.
        /// </summary>
        /// <returns>The name of the pose</returns>
        public string getPoseName()
        {
            bool result;

            StringBuilder sb = new StringBuilder(24);

            if (!Native.PoseRecognizer_getPoseName(instance, sb, out result))
                throw new NullReferenceException("Error while trying to get pose!");

            if (result)
                return sb.ToString();
            else
                return "";
        }

        /// <summary>
        /// Adds a new pose to the sytem
        /// </summary>
        /// <param name="poseName">Name of the pose as a string</param>
        /// <param name="dof">Degree of freedom for the pose recognition. Basically, how much freedom is given to the recognition
        /// algorithm when trying to recognize this particular pose. The bigger the value, the more precise 
        /// the user needs to be when emulating the pose. The value of 0.8-0.85 should be enough </param>
        public void addNewPose(string poseName, double dof = 0.8)
        {
            if (!Native.PoseRecognizer_addNewPose(instance, poseName, dof))
                throw new NullReferenceException("Error while trying to get pose!");
        }

        /// <summary>
        /// Save the current joint positions as a new training sample for the specified pose
        /// </summary>
        /// <param name="poseName">String name of the pose, for which we want to save the training sample</param>
        public void saveTrainingSample(string poseName)
        {
            if (!Native.PoseRecognizer_saveTrainingDataByName(instance, poseName))
                throw new NullReferenceException("Error while trying to save pose!");
        }

        /// <summary>
        /// Save the current joint positions as a new training sample for the specified pose
        /// </summary>
        /// <param name="poseIndex">ID of the pose, for which we want to save the training sample</param>
        public void saveTrainingSample(int poseIndex)
        {
            if (!Native.PoseRecognizer_saveTrainingDataByIdx(instance, poseIndex))
                throw new NullReferenceException("Error while trying to save pose!");
        }

        /// <summary>
        /// Detemine if the user is trying to do the specified pose and return the likeness score
        /// </summary>
        /// <param name="poseName">Name of the pose</param>
        /// <returns>The score from 0 to 1 as to how well is the user imitating the pose</returns>
        public double expectPose(string poseName)
        {
            double result = 0.0;

            if (!Native.PoseRecognizer_expectPoseByName(instance, poseName, out result))
                throw new NullReferenceException("Error while estimating a pose!");

            return result;
        }

        /// <summary>
        /// Detemine if the user is trying to do the specified pose and return the likeness score
        /// </summary>
        /// <param name="poseIndex">ID of the pose</param>
        /// <returns>The score from 0 to 1 as to how well is the user imitating the pose</returns>
        public double expectPose(int poseIndex)
        {
            double result = 0.0;

            if (!Native.PoseRecognizer_expectPoseByIdx(instance, poseIndex, out result))
                throw new NullReferenceException("Error while estimating a pose!");

            return result;
        }

        //////////////// Specific User related functions:       

        /// <summary>
        /// Get a list of user IDs for every currently tracked user
        /// </summary>
        /// <returns>A list of IDs</returns>
        public unsafe UInt64[] getUserList() {
            int amount = 0;
            IntPtr listRef = IntPtr.Zero;

            if ( !Native.PoseRecognizer_getUserIdList(instance, out amount, ref listRef) )
                throw new NullReferenceException("Error while getting the list of users!");

            //long[] llist = new long[amount];
            // Util.Log("# size: " + amount + "ptr: " + listRef.ToString());
            //Marshal.Copy(listRef, llist, 0, amount);

            UInt64[] list = new UInt64[amount];           

            fixed (UInt64* ptrDest = &list[0]) {
                Native.CopyMemory((IntPtr)ptrDest, listRef, amount * 8);
            }

            return list;
        }

        /// <summary>
        /// Detemine if the user is trying to do the specified pose and return the likeness score
        /// </summary>
        /// <param name="userId">The Id of the specific user</param>
        /// <param name="poseName">Name of the pose</param>
        /// <returns>The score from 0 to 1 as to how well is the user imitating the pose</returns>
        public double expectPose(UInt64 userId, string poseName)
        {
            double result = 0.0;

            if (!Native.PoseRecognizer_expectUserPoseByName(instance, userId, poseName, out result))
                throw new NullReferenceException("Error while estimating a pose!");

            return result;
        }

        /// <summary>
        /// Detemine if the user is trying to do the specified pose and return the likeness score
        /// </summary>
        /// <param name="userId">The ID of the specific user</param>
        /// <param name="poseIndex">ID of the pose</param>
        /// <returns>The score from 0 to 1 as to how well is the user imitating the pose</returns>
        public double expectPose(UInt64 userId, int poseIndex)
        {
            double result = 0.0;

            if (!Native.PoseRecognizer_expectUserPoseByIdx(instance, userId, poseIndex, out result))
                throw new NullReferenceException("Error while estimating a pose!");

            return result;
        }

        /// <summary>
        /// Returns the name of the current pose as a string.
        /// </summary>
        /// <param name="userId">The ID of the specific user</param>
        /// <returns>The name of the pose</returns>
        public string getPoseName(UInt64 userId)
        {
            bool result;

            StringBuilder sb = new StringBuilder(24);
            
            if (!Native.PoseRecognizer_getUserPoseName(instance, userId, sb, out result))
                throw new NullReferenceException("Error while trying to get pose!");
            
            if (result)
                return sb.ToString();
            else
                return "";
        }

        /// <summary>
        /// Gets the "in scene" coordinated of the specified joint. The coordinated are for the 640x480
        /// depth image
        /// </summary>
        /// <param name="userId">The ID of the specific user</param>
        /// <param name="joint">Name of the joint from the KinectJoint enum</param>
        /// <returns>Coordinates of the joint</returns>
        public PointF getJointScene(UInt64 userId, KinectJoint joint)
        {
            double x, y, z;
            bool result;

            if (!Native.PoseRecognizer_getUserJointScene(instance, userId, (int)joint, out x, out y, out z, out result))
                throw new NullReferenceException("Error while trying to read joint coordinates!");

            if (result)
                return new PointF((float)x, (float)y);
            else
                return new PointF(0, 0);
        }

        /// <summary>
        /// Gets the real world coordinates of the specified joint
        /// </summary>
        /// <param name="userId">The ID of the specific user</param>
        /// <param name="joint">Name of the joint</param>
        /// <returns>Coordinates of a joint as 3D-point</returns>
        public cvPoint3f getJointRealworld(UInt64 userId, KinectJoint joint)
        {
            double x, y, z;
            bool result;

            if (!Native.PoseRecognizer_getUserJointRealworld(instance, userId, (int)joint, out x, out y, out z, out result))
                throw new NullReferenceException("Error while trying to read joint coordinates!");

            if (result)
                return new cvPoint3f(x, y, z);
            else
                return new cvPoint3f(0, 0, 0);
        }

        /// <summary>
        /// Returns the movement direction for the specified hand of the specified user as an enum type {NONE, LEFT, RIGHT, UP, etc.}
        /// </summary>
        /// <param name="userId">The ID of the user</param>
        /// <param name="hand">The specified hand (Hand.LEFT or Hand.RIGHT)</param>
        /// <returns>The direction as on of the options from the enum 'Direction'</returns>
        public Direction getHandDirection(UInt64 userId, Hand hand) {
            int dir = 0;
            bool result;

            if (!Native.PoseRecognizer_getUserHandDirection(instance, userId, (int)hand, out dir, out result))
                throw new NullReferenceException("Error while trying to get hand movement direction!");

            if (result)
                return (Direction)dir;
            else
                return Direction.NONE;
        }

        /// <summary>
        /// Check if the user's hand movement direction briefly changed!
        /// </summary>
        /// <param name="userId">The ID of the user</param>
        /// <param name="hand">The specified hand (Hand.LEFT or Hand.RIGHT)</param>
        /// <returns>Briefly returns TRUE if the hand stopped moving or changed movement direction</returns>
        public bool handChangedDirection(UInt64 userId, Hand hand) {
            bool directionChanged;
            bool result;

            if (!Native.PoseRecognizer_userHandChangedDirection(instance, userId, (int)hand, out directionChanged, out result))
                throw new NullReferenceException("Error while trying to check if hand changed movement direction!");

            if (result)
                return directionChanged;
            else
                return false;
        }

        /// <summary>
        /// Get the state for the specified hand (untracked, closed, open, etc.)
        /// </summary>
        /// <param name="userId">The ID of the user</param>
        /// <param name="hand">The specified hand (Hand.LEFT or Hand.RIGHT)</param>
        /// <returns>Returns the state</returns>
        public HandState getHandState(UInt64 userId, Hand hand) {
            bool result;
            int state;

            if (!Native.PoseRecognizer_getUserHandState(instance, userId, (int)hand, out state, out result))
                throw new NullReferenceException("Error while trying to check if hand changed movement direction!");

            if (result)
                return (HandState)state;
            else
                return HandState.Unknown;
        }

        /// <summary>
        /// Check if the hand is currently moving or not
        /// </summary>
        /// <param name="userId">The ID of the user</param>
        /// <param name="hand">The specified hand (Hand.LEFT or Hand.RIGHT)</param>
        /// <returns></returns>
        public bool isHandMoving(UInt64 userId, Hand hand)
        {
            bool isMoving;
            bool result;

            if (!Native.PoseRecognizer_isUserHandMoving(instance, userId, (int)hand, out isMoving, out result))
                throw new NullReferenceException("Error while trying to check if the hand is moving!");

            if (result)
                return isMoving;
            else
                return false;
        }

        /// <summary>
        /// Get the hand's movement speed in m/sec
        /// </summary>
        /// <param name="userId">The ID of the user</param>
        /// <param name="hand">The specified hand (Hand.LEFT or Hand.RIGHT)</param>
        /// <returns></returns>
        public double getHandMovementSpeed(UInt64 userId, Hand hand) {
            double speed = 0;
            bool result;

            if (!Native.PoseRecognizer_getUserHandSpeed(instance, userId, (int)hand, out speed, out result))
                throw new NullReferenceException("Error while trying to get hand movement speed!");

            if (result)
                return speed;
            else
                return Double.NegativeInfinity;
        }
    }
}

