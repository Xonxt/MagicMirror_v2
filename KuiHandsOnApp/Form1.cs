using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Media;

using MagicMirrorNETWrapper;
using System.Collections;

using System.Runtime.InteropServices;

namespace KuiHandsOnApp
{
    enum ImageType
    {
        Skeleton = 0,
        Depth = 1,
        Rgb = 2
    }

    public partial class Form1 : Form
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool AllocConsole();

        private PoseRecognizer poseRecognizer;
        private Timer timer;
        private bool initialized = false;
        private int gapX = 0, gapXX = 0, gapY = 0;

        private Dictionary<UInt64, KinectUser> userList = new Dictionary<ulong, KinectUser>();

        // audio sounds
        //private Dictionary<string, byte[]> sounds = new Dictionary<string, byte[]>();
        private Dictionary<string, SoundPlayer> sounds = new Dictionary<string, SoundPlayer>();       

        private ImageType toggleType = ImageType.Skeleton;

        public Form1()
        {
            InitializeComponent();
            
            timer = new Timer();
            timer.Interval = 1;
            timer.Tick += Timer_Tick;

            poseRecognizer = new PoseRecognizer();

            try
            {
                if (poseRecognizer.initialize())
                {
                    initialized = true;
                    timer.Start();
                }
            }
            catch
            {
                MessageBox.Show("Couln't initialize pose recognizer!");
            }

            updateList();
        }

        private void updateList()
        {
            comboBox1.Items.Clear();

            var files = Directory.GetFiles("instruments/", "*.txt");

            foreach (var file in files)
            {

                using (System.IO.StreamReader reader = new StreamReader(file))
                {
                    string name = reader.ReadLine();

                    comboBox1.Items.Add(name);
                }

            }
        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            if (!initialized)
                return;

            bool nextFrame = false;

            try
            {
                nextFrame = poseRecognizer.processNextFrame();
            }
            catch
            {
                MessageBox.Show("next frame exception");
                timer.Stop();
                return;
            }

            if (nextFrame)
            {
                Bitmap frame = null;
                try
                {
                    //frame = (toggleRgb) ? poseRecognizer.getSkeletonImage() : poseRecognizer.getDepthMap();
                    switch (toggleType)
                    {
                        case ImageType.Skeleton:
                            frame = poseRecognizer.getSkeletonImage();
                            break;
                        case ImageType.Rgb:
                            frame = poseRecognizer.getRgbFrame();
                            //frame = poseRecognizer.getSkeletonImage();
                            break;
                        case ImageType.Depth:
                            frame = poseRecognizer.getDepthMap();
                            break;
                        default:
                            break;
                    }

                }
                catch
                {
                    MessageBox.Show("get frame exception");
                }

                bool userPresent = false;
                try
                {
                    userPresent = poseRecognizer.userPresent();
                }
                catch
                {
                    MessageBox.Show("user present exception");
                }

                label2.Text = "Users present: " +  userList.Count;

                if (userPresent)
                {
                    // populate the list of currently seen users:
                    UInt64[] list = poseRecognizer.getUserList();
                    updateUsers(list);

                    foreach (UInt64 user in list)
                    {
                        playMusic(user);
                    }
                    

                    try
                    {
                        // recognize the currect pose out of all available poses
                        label5AllPoses.Text = poseRecognizer.getPoseName();

                        // recognize one specified pose and get the similarity score in range [0..1]:
                        if (comboBox1.Text != "")
                        {
                            string poseName = comboBox1.Text;
                            // "expectPose(String poseName)" will return the similarity score for the pose                            
                            label3.Text = poseName.ElementAt<char>(0) + ": " + poseRecognizer.expectPose(poseName);
                        }                        

                        // see how the joints for the user are extracted:
                        if (toggleType == ImageType.Skeleton)
                            drawSkeleton(ref frame);
                    }
                    catch (NullReferenceException ex)
                    {
                        MessageBox.Show("get pose exception");
                    }
                }

                pictureBox1.Image = frame as Image;
            }

            timer.Start();
        }

        private void playMusic(UInt64 user) {

            if (!userList.ContainsKey(user)) {
                return;

            }

            string instrument = poseRecognizer.getPoseName(user).ToLower();

            var LeftHand = new {
                state = poseRecognizer.getHandState(user, Hand.LEFT),
                speed = poseRecognizer.getHandMovementSpeed(user, Hand.LEFT),
                direction = poseRecognizer.getHandDirection(user, Hand.LEFT),
                changedDirection = poseRecognizer.handChangedDirection(user, Hand.LEFT),
                isMoving = poseRecognizer.isHandMoving(user, Hand.LEFT)
            };

            var RightHand = new
            {
                state = poseRecognizer.getHandState(user, Hand.RIGHT),
                speed = poseRecognizer.getHandMovementSpeed(user, Hand.RIGHT),
                direction = poseRecognizer.getHandDirection(user, Hand.RIGHT),
                changedDirection = poseRecognizer.handChangedDirection(user, Hand.RIGHT),
                isMoving = poseRecognizer.isHandMoving(user, Hand.RIGHT)
            };

            //if (userList[user].changedRight(RightHand.direction))
            //    Console.WriteLine(Enum.GetName(typeof(Direction), RightHand.direction));

            string soundName = String.Copy(instrument);

            // go instrument by instrument:
            if (instrument == "flute" || instrument == "clarinet")
            {
                // both hands closed
                if (userList[user].isStateChanged(LeftHand.state, RightHand.state))
                {
                    if ((int)LeftHand.state >= (int)HandState.Closed && (int)RightHand.state >= (int)HandState.Closed)
                    {
                        soundName += "_A";
                    }
                    else if ((int)LeftHand.state <= (int)HandState.Open && (int)RightHand.state >= (int)HandState.Closed)
                    {
                        soundName += "_B";
                    }
                    else if ((int)LeftHand.state >= (int)HandState.Closed && (int)RightHand.state <= (int)HandState.Open)
                    {
                        soundName += "_C";
                    }
                    else if ((int)LeftHand.state <= (int)HandState.Open && (int)RightHand.state <= (int)HandState.Open)
                    {
                        soundName += "_D";
                    }

                    userList[user].playSound(soundName);
                }

            }
            else if (instrument == "violin" || instrument == "cello")
            {
                if (userList[user].changedRight(RightHand.direction))
                {
                    if (RightHand.direction == Direction.RIGHT || RightHand.direction == Direction.BACKWARDS)
                        soundName += "_right";
                    else if (RightHand.direction == Direction.LEFT || RightHand.direction == Direction.FORWARDS)
                        soundName += "_left";

                    double slow = 0.45, medium = 0.85;

                    if (RightHand.speed <= slow)
                        soundName += "_slow";
                    else if (RightHand.speed > slow && RightHand.speed <= medium)
                        soundName += "_medium";
                    else if (RightHand.speed > medium)
                        soundName += "_fast";
                    else
                        soundName += "_slow";

                    userList[user].playSound(soundName);
                }
            }
            else if (instrument == "drums")
            {
                if (userList[user].changedRight(RightHand.direction))
                {
                    if (RightHand.direction == Direction.DOWN)
                    {
                        userList[user].playSound(soundName + "_fast");
                    }
                }

                if (userList[user].changedLeft(LeftHand.direction))
                {
                    if (LeftHand.direction == Direction.DOWN)
                    {
                        userList[user].playSound(soundName + "_medium");
                    }
                }
            }
            else if (instrument == "guitar")
            {
                if (userList[user].changedRight(RightHand.direction))
                {
                    //Console.WriteLine(Enum.GetName(typeof(Direction), RightHand.direction));

                    if (RightHand.direction == Direction.UP)
                    {
                        userList[user].playSound("guitar_up");
                    }
                    else if (RightHand.direction == Direction.DOWN)
                    {
                        userList[user].playSound("guitar_down");
                    }


                }
            }
            else if (instrument == "conductor")
            {
                if (RightHand.changedDirection && !RightHand.isMoving)
                {

                    userList[user].playSound("conductor");
                }
            }
            
            
        }

        private void updateUsers(UInt64[] list) {

            foreach (var user in list) {

                if (!userList.ContainsKey(user)) {
                    userList.Add(user, new KinectUser(user));
                }

            }
            
            var it = userList.GetEnumerator();

            try
            {
                while (it.MoveNext())
                {
                    if (!list.Contains<UInt64>(it.Current.Key))
                    {
                        //userList.Remove(it.Current.Key);
                        userList.Clear();
                        break;

                    }
                }
            }
            catch (Exception ex) {
                Console.WriteLine("error with user removal");
            }
        }

        /// <summary>
        /// An examle of how the joint coordinated for the user can be extracted and used for something
        /// </summary>
        /// <param name="image"></param>
        private void drawSkeleton(ref Bitmap image)
        {

            List<PointF> points = new List<PointF>();

            try
            {
                points.Add(poseRecognizer.getJointScene(KinectJoint.Head));
                points.Add(poseRecognizer.getJointScene(KinectJoint.Neck));
                points.Add(poseRecognizer.getJointScene(KinectJoint.RightShoulder));
                points.Add(poseRecognizer.getJointScene(KinectJoint.RightElbow));
                points.Add(poseRecognizer.getJointScene(KinectJoint.RightHand));
                points.Add(poseRecognizer.getJointScene(KinectJoint.LeftShoulder));
                points.Add(poseRecognizer.getJointScene(KinectJoint.LeftElbow));
                points.Add(poseRecognizer.getJointScene(KinectJoint.LeftHand));

                points.Add(poseRecognizer.getJointScene(KinectJoint.Torso));
                points.Add(poseRecognizer.getJointScene(KinectJoint.LeftHip));
                points.Add(poseRecognizer.getJointScene(KinectJoint.RightHip));
                points.Add(poseRecognizer.getJointScene(KinectJoint.LeftKnee));
                points.Add(poseRecognizer.getJointScene(KinectJoint.RightKnee));
                points.Add(poseRecognizer.getJointScene(KinectJoint.LeftFoot));
                points.Add(poseRecognizer.getJointScene(KinectJoint.RightFoot));

            }
            catch
            {
               // MessageBox.Show("get point exception");
            }

            try
            {
                using (Graphics g = Graphics.FromImage(image))
                {
                    g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

                    using (Pen pen = new Pen(Color.Blue, 5))
                    {
                        g.DrawLine(pen, points[0], points[1]);
                        g.DrawLine(pen, points[1], points[2]);
                        g.DrawLine(pen, points[2], points[3]);
                        g.DrawLine(pen, points[3], points[4]);
                        g.DrawLine(pen, points[1], points[5]);
                        g.DrawLine(pen, points[5], points[6]);
                        g.DrawLine(pen, points[6], points[7]);

                        g.DrawLine(pen, points[1], points[8]);
                        g.DrawLine(pen, points[8], points[9]);
                        g.DrawLine(pen, points[8], points[10]);
                        g.DrawLine(pen, points[9], points[11]);
                        g.DrawLine(pen, points[10], points[12]);
                        g.DrawLine(pen, points[11], points[13]);
                        g.DrawLine(pen, points[12], points[14]);
                    }

                    using (Pen pen = new Pen(Color.Red, 7))
                    {
                        foreach (var point in points)
                        {
                            g.DrawEllipse(pen, new RectangleF(point, new SizeF(3, 3)));
                        }
                    }
                }
            }
            catch
            {
                //MessageBox.Show("drawing exception");
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {   
            toggleType++;

            if ((int)toggleType > 2)
                toggleType = 0;

            switch (toggleType)
            {
                case ImageType.Skeleton:
                    button3.Text = "Skeleton";
                    break;
                case ImageType.Rgb:
                    button3.Text = "RGB";
                    break;
                case ImageType.Depth:
                    button3.Text = "Depth";
                    break;
                default:
                    break;
            }
        }

        private void buttonAddPose_Click(object sender, EventArgs e)
        {
            if (textBox1.Text != "" && initialized)
            {
                poseRecognizer.addNewPose(textBox1.Text);
                MessageBox.Show("Pose '" + textBox1.Text + "' added!");
                updateList();
            }
        }

        private void Form1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.B && initialized)
            {
                poseRecognizer.saveTrainingSample(textBox1.Text);
            }
        }

        private void Form1_Resize(object sender, EventArgs e)
        { // 925; 572
            if ((gapXX + gapX) == 0)
                return;

            panel1.Left = this.Width - panel1.Width - gapXX;
            pictureBox1.Width = panel1.Left - gapX - pictureBox1.Left;
            pictureBox1.Height = this.Height - pictureBox1.Top - gapY;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            gapX = panel1.Left - (pictureBox1.Left + pictureBox1.Width);
            gapXX = this.Width - panel1.Right;
            gapY = this.Height - (pictureBox1.Top + pictureBox1.Height);

           // AllocConsole();
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            if (textBox1.Text != "" && initialized)
                poseRecognizer.saveTrainingSample(textBox1.Text);
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            textBox1.Text = comboBox1.Text;
        }

        private Rectangle oldBoundsWindow, oldBoundsPictureBox;

        // Double-click on the Picture box
        private void pictureBox1_DoubleClick(object sender, EventArgs e)
        {
            if (WindowState == FormWindowState.Normal)
            {
                oldBoundsWindow = this.Bounds;
                oldBoundsPictureBox = pictureBox1.Bounds;

                this.WindowState = FormWindowState.Maximized;
                FormBorderStyle = FormBorderStyle.None;

                TopMost = true;
                Left = Top = 0;
                Width = Screen.PrimaryScreen.WorkingArea.Width;
                Height = Screen.PrimaryScreen.WorkingArea.Height;

                panel1.Visible = false;

                pictureBox1.Left = pictureBox1.Top = 0;
                pictureBox1.Width = this.Width;
                pictureBox1.Height = this.Height;
            }
            else
            {
                WindowState = FormWindowState.Normal;
                FormBorderStyle = FormBorderStyle.Sizable;
                TopMost = false;

                panel1.Visible = true;

                this.Bounds = oldBoundsWindow;
                pictureBox1.Bounds = oldBoundsPictureBox;
            }




        }
      

        
    }
}
