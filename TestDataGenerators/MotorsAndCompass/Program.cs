using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace MotorsAndCompass
{
    class Program
    {
        static void Main(string[] args)
        {
            Random r = new Random();
            double StartHeading = (0.5 - r.NextDouble())/2;
            double lastHeading = StartHeading;
            double headingChange = (0.5 - r.NextDouble()) / 100;
            int maxIter = 800 + r.Next(10)*200;
            int mFL = -100;
            int mFR = -100;
            int mBL = -100;
            int mBR = -100;
            using (StreamWriter sw = new StreamWriter("MotorCalibrationData.txt", false)) 
            {
                sw.WriteLine("Time\tHeading\tmFLt\tmFR\tmBL\tmBR");
                for (int i = 1; i < maxIter + (maxIter / 200); i++)
                {
                    if (i % (maxIter / 200) == 0)
                    {
                        mFL++;
                        mFR++;
                        mBL++;
                        mBR++;
                        headingChange = headingChange/2 + (0.5 - r.NextDouble()) / 100;
                        if (mFL == 0)
                            headingChange *= -1;
                    }
                    sw.WriteLine(string.Format("{0}\t{1:0.0000}\t{2}\t{3}\t{4}\t{5}", i, lastHeading, mFL, mFR, mBL, mBR));
                    if (Math.Abs(mFL) > 2)
                        lastHeading += headingChange + (0.5 - r.NextDouble())/200;
                }
            }
        }
    }
}
