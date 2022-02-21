using System.Runtime.InteropServices;

namespace SlipperEditor{
    public unsafe class Program
    {
#if DEBUG
        public const string EngineDll = @"../../../Engine/Debug/libSlipper_Engine";
#else
        public const string EngineDll = @"../../../Engine/Release/libSlipper_Engine";
#endif

        [DllImport(EngineDll, CallingConvention = CallingConvention.Cdecl)]
        public static extern void say_hello(int* array);

        static void Main(string[] args)
        {
            Console.WriteLine(File.Exists(EngineDll));
            say_hello(null);
        }
    }
}