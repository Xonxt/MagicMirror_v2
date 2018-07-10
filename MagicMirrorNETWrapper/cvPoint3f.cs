using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MagicMirrorNETWrapper
{
    public class cvPoint3f
    {
        public double X;
        public double Y;
        public double Z;

        public cvPoint3f()
        {
            this.X = this.Y = this.Z = 0;
        }

        public cvPoint3f(double x, double y, double z)
        {
            this.X = x;
            this.Y = y;
            this.Z = z;
        }
    }
}
