using CLGLNET;

class Program
{
    //[DllImport("nvapi64.dll", EntryPoint = "NvAPI_Initialize", CallingConvention = CallingConvention.Cdecl)]
    //public static extern int NvAPI_Initialize();

    static void Main(string[] args)
    {
       // NvAPI_Initialize();
        using (var game = new WindowsWave(1600, 1200, "OpenGL with OpenCL"))
        {
            game.Run();
        }
    }

}

