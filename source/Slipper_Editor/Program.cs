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
        public static extern int main();

        static void Main(string[] args)
        {
            Console.WriteLine(File.Exists(EngineDll));
            main();
        }
    }
}