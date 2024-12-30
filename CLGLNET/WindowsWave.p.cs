using OpenTK.Compute.OpenCL;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OpenTK.Graphics.OpenGL;
using System.Runtime.InteropServices;
using OpenTK.Windowing.Common;
using OpenTK.Mathematics;
using OpenTK.Windowing.Desktop;
//using OpenCL.Net;
using OpenTK.Graphics.Wgl;
using ErrorCode = OpenTK.Graphics.OpenGL.ErrorCode;

namespace CLGLNET
{
    public partial class WindowsWave
    {
        private const uint CL_PLATFORM_NAME = 0x0902;
        private const uint CL_PLATFORM_VENDOR = 0x0903;
        private const uint CL_PLATFORM_VERSION = 0x0904;
        private const uint CL_PLATFORM_PROFILE = 0x0900;

        private void CheckGLError(string location)
        {
            ErrorCode error = GL.GetError();
            if (error != ErrorCode.NoError)
            {
                throw new Exception($"OpenGL error at {location}: {error}");
            }
        }

        private static void CheckResult(CLResultCode res)
        {
            if (res != CLResultCode.Success)
            {
                //ViewError(res.ToString(), res.ToString());
                throw new Exception("Error");
            }
        }
        private string GetPlatformInfo(CLPlatform platform, uint paramName)
        {
            CL.GetPlatformInfo(platform, (PlatformInfo)paramName, out byte[] paramValue);
            return System.Text.Encoding.ASCII.GetString(paramValue).TrimEnd('\0');
        }

        private void WyswietlPlatformy(CLPlatform[] platformIds)
        {
            for (int q = 0; q < platformIds.Length; q++)
            {
                Console.WriteLine($"Platforma: {platformIds[q].Handle}");
                string platformName = GetPlatformInfo(platformIds[q], CL_PLATFORM_NAME);
                //string platformVendor = GetPlatformInfo(platformz, CL_PLATFORM_VENDOR);
                //string platformVersion = GetPlatformInfo(platformz, CL_PLATFORM_VERSION);
                //string platformProfile = GetPlatformInfo(platformz, CL_PLATFORM_PROFILE);

                Console.WriteLine($"Platforma[{q}]: {platformName}");
                //Console.WriteLine($"  Vendor: {platformVendor}");
                //Console.WriteLine($"  Wersja: {platformVersion}");
                //Console.WriteLine($"  Profil: {platformProfile}");
            }
        }
    }
}
