using System.Runtime.InteropServices;

namespace SlipperEditor{
    public class Program
    {
        public const string EngineDll = @"../../../../Slipper_Engine/build/libSlipper_Engine.so";
        

        [DllImport(EngineDll, CallingConvention = CallingConvention.Cdecl)]
        public static extern void say_hello();

        static void Main(string[] args)
        {
            Console.WriteLine(File.Exists(EngineDll));
            say_hello();
        }
    }
}