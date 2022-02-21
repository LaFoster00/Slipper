using System.Runtime.InteropServices;

namespace SlipperEditor{
    public unsafe class Program
    {
        public const string EngineDll = @"../../../Engine/libSlipper_Engine";
        
        [DllImport(EngineDll, CallingConvention = CallingConvention.Cdecl)]
        public static extern void say_hello(int* array);

        static void Main(string[] args)
        {
            Console.WriteLine(File.Exists(EngineDll));
            say_hello(null);
        }
    }
}