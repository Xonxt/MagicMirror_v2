using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MagicMirrorNETWrapper
{
    public class cvPoint2f
    {
        public double X;
        public double Y;

        public cvPoint2f()
        {
            this.X = 0;
            this.Y = 0;
        }

        public cvPoint2f(double x, double y)
        {
            this.X = x;
            this.Y = y;
        }
    }
}
