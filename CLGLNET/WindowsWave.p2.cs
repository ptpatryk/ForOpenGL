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
        [DllImport("OpenCL.dll", EntryPoint = "clEnqueueReleaseGLObjects")]
        private static extern int clEnqueueReleaseGLObjects(IntPtr commandQueue, uint numObjects, IntPtr[] memObjects, uint numEventsInWaitList, IntPtr[] eventWaitList, out IntPtr @event);

        [DllImport("OpenCL.dll", EntryPoint = "clEnqueueAcquireGLObjects")]
        private static extern int clEnqueueAcquireGLObjects(IntPtr commandQueue, uint numObjects, IntPtr[] memObjects, uint numEventsInWaitList, IntPtr[] eventWaitList, out IntPtr @event);

        [DllImport("OpenCL.dll", EntryPoint = "clCreateFromGLBuffer")]
        private static extern IntPtr clCreateFromGLBuffer(IntPtr context, ulong flags, uint bufobj, out int errcode_ret);

        [DllImport("opengl32.dll")]
        private static extern IntPtr wglGetCurrentContext();

        [DllImport("opengl32.dll")]
        private static extern IntPtr wglGetCurrentDC();


        private CLResultCode EnqueueAcquireGLObjects(CLCommandQueue queue, uint numObjects, CLBuffer[] memObjects, uint numEventsInWaitList, CLEvent[]? eventWaitList, out CLEvent @event)
        {
            IntPtr[] memObjectHandles = new IntPtr[memObjects.Length];
            for (int i = 0; i < memObjects.Length; i++)
            {
                memObjectHandles[i] = memObjects[i].Handle;
            }

            //IntPtr[] eventWaitListHandles = Array.Empty<IntPtr>();
            IntPtr[] eventWaitListHandles = null;
            if (eventWaitList != null)
            {
                eventWaitListHandles = new IntPtr[eventWaitList.Length];
                for (int i = 0; i < eventWaitList.Length; i++)
                {
                    eventWaitListHandles[i] = eventWaitList[i].Handle;
                }
            }

            IntPtr eventHandle;
            CLResultCode result = (CLResultCode)clEnqueueAcquireGLObjects(queue.Handle, numObjects, memObjectHandles, numEventsInWaitList, eventWaitListHandles, out eventHandle);
            @event = new CLEvent(eventHandle);
            return result;
        }

        private CLResultCode EnqueueReleaseGLObjects(CLCommandQueue queue, uint numObjects, CLBuffer[] memObjects, uint numEventsInWaitList, CLEvent[]? eventWaitList, out CLEvent @event)
        {
            IntPtr[] memObjectHandles = new IntPtr[memObjects.Length];
            for (int i = 0; i < memObjects.Length; i++)
            {
                memObjectHandles[i] = memObjects[i].Handle;
            }

            //IntPtr[] eventWaitListHandles = Array.Empty<IntPtr>();
            IntPtr[] eventWaitListHandles = null;
            if (eventWaitList != null)
            {
                eventWaitListHandles = new IntPtr[eventWaitList.Length];
                for (int i = 0; i < eventWaitList.Length; i++)
                {
                    eventWaitListHandles[i] = eventWaitList[i].Handle;
                }
            }

            IntPtr eventHandle;
            CLResultCode result = (CLResultCode)clEnqueueReleaseGLObjects(queue.Handle, numObjects, memObjectHandles, numEventsInWaitList, eventWaitListHandles, out eventHandle);
            @event = new CLEvent(eventHandle);
            return result;
        }

        private unsafe static CLBuffer CreateFromGLBuffer(CLContext context, MemoryFlags flags, int buffer, out CLResultCode resultCode)
        {
            IntPtr clBufferPtr = clCreateFromGLBuffer(context.Handle, (ulong)flags, (uint)buffer, out int errcode);
            resultCode = (CLResultCode)errcode;

            if (resultCode != CLResultCode.Success)
            {
                Console.WriteLine($"Error creating buffer from GL buffer: {resultCode}");
                // Możesz dodać dodatkowe logowanie lub obsługę błędów tutaj
            }

            return new CLBuffer(clBufferPtr);
        }

    }
}
