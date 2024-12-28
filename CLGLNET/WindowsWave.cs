using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using OpenTK;
//using OpenTK.Graphics.OpenGL4;
using OpenTK.Graphics.GL;

using OpenTK.Graphics;
using OpenTK.Graphics.OpenGL;
using OpenTK.Input;

using Cloo;
using System;
using System.IO;
using System.Runtime.InteropServices;
using static OpenTK.Graphics.OpenGL.GL;
using OpenTK.Windowing.Desktop;
using OpenTK.Compute.OpenCL;
using System;
using System.IO;
using OpenTK.Windowing.Common;
using OpenTK.Mathematics;
using System.Threading.Tasks.Dataflow;
using Cloo.Bindings;
using OpenTK.Graphics.Wgl;
using static System.Runtime.InteropServices.JavaScript.JSType;
using OpenTK.Graphics.OpenGL;
using Cloo;


namespace CLGLNET
{
    [StructLayout(LayoutKind.Sequential)]
    struct Punkt
    {
        public float m, v, x;
    }

    [StructLayout(LayoutKind.Sequential)]
    struct PunktNormal
    {
        public float x, y, z, nx, ny, nz;
    }

    public class WindowsWave : GameWindow
    {
        int vbo, nbo;
        ComputeContext clContext;
        ComputeCommandQueue queue;
        ComputeKernel kernel;
        ComputeKernel kernelTrujkatow;
        ComputeKernel kernelPrzygotujTrojkaty;
        ComputeBuffer<Punkt> aaBuf, bbBuf;
        ComputeBuffer<PunktNormal> clNbo;
        ComputeBuffer<float> vertexBuffer;

        int N_X = 100;
        int N_Y = 100;
        float dt = 0.01f;
        float w = -2.0f;
        PunktNormal[] punktyINormalne;
        Punkt[] aa;

        private int _vertexBufferObject;
        private int _vertexArrayObject;
        private int _shaderProgram;


        public WindowsWave(int width, int height, string title)
            : base(GameWindowSettings.Default, new NativeWindowSettings() { ClientSize = new OpenTK.Mathematics.Vector2i(width, height), Title = title })
        {
            punktyINormalne = new PunktNormal[N_X * N_Y];

            aa = new Punkt[N_X * N_Y];

            for (int i = 0; i < aa.Length; i++)
            {
                aa[i].m = 1.0f;
            }

        }

        private int _vertexShader;
        private int _fragmentShader;
        private float czas=0.0f;

        protected override void OnLoad()
        {
            base.OnLoad();

            InitOpenGL();

            InitOpenCL();

            // Przykładowe dane

            // Uruchomienie kernela
            RunKernel(true,czas);

            float[] vertices2 = PrzygotujTrojkaty(punktyINormalne);

            PrzygotowanieBufora(vertices2);

            Akcja();
        }

        //void komb()
        //{
        //    // Tworzenie kontekstu OpenCL z kontekstem OpenGL
        //    var properties = new ComputeContextPropertyList(ComputePlatform.Platforms[0]);
        //    properties.Add(new ComputeContextProperty(ComputeContextPropertyName.CL_GL_CONTEXT_KHR, GL.GetCurrentContextHandle()));
        //    properties.Add(new ComputeContextProperty(ComputeContextPropertyName.CL_WGL_HDC_KHR, Wgl.GetCurrentDC()));
        //    var context = new ComputeContext(ComputeDeviceTypes.Gpu, properties, null, IntPtr.Zero);

        //    // Tworzenie bufora OpenGL
        //    int vbo;
        //    GL.GenBuffers(1, out vbo);
        //    GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);
        //    GL.BufferData(BufferTarget.ArrayBuffer, new IntPtr(data.Length * sizeof(float)), data, BufferUsageHint.DynamicDraw);

        //    // Rejestrowanie bufora w OpenCL
        //    IntPtr bufferHandle = CL10.CreateFromGLBuffer(context.Handle, ComputeMemoryFlags.ReadWrite, vbo, out ComputeErrorCode error);
        //    ComputeBuffer<float> clBuffer = new ComputeBuffer<float>(bufferHandle, context, ComputeMemoryFlags.ReadWrite);

        //    // Synchronizacja i użycie w OpenCL
        //    GL.Finish();
        //    var queue = new ComputeCommandQueue(context, context.Devices[0], ComputeCommandQueueFlags.None);
        //    queue.AcquireGLObjects(new[] { clBuffer }, null);
        //    // Operacje OpenCL na buforze
        //    queue.ReleaseGLObjects(new[] { clBuffer }, null);
        //    queue.Finish();
        //}

        protected override void OnUnload()
        {
            base.OnUnload();

            // Zwolnienie zasobów OpenGL
            GL.DeleteShader(_vertexShader);
            GL.DeleteShader(_fragmentShader);
            GL.DeleteProgram(_shaderProgram);
            GL.DeleteBuffer(_vertexBufferObject);
            GL.DeleteVertexArray(_vertexArrayObject);

            // Zwolnienie zasobów OpenCL
            clNbo.Dispose();
            bbBuf.Dispose();
            aaBuf.Dispose();
            kernelTrujkatow.Dispose();
            kernel.Dispose();
            queue.Dispose();
            clContext.Dispose();
        }

        private float[] PrzygotujTrojkaty(PunktNormal[] wieszcholki)
        {
            List<float> vertices = new List<float>();

            for (int i = 0; i < N_Y - 1; i++)
                for (int j = 0; j < N_X - 1; j++)
                {
                    AddVwrtex(vertices, wieszcholki, i, j);
                    AddVwrtex(vertices, wieszcholki, i, j + 1);
                    AddVwrtex(vertices, wieszcholki, i + 1, j);
                }

            return vertices.ToArray();
        }

        private void AddVwrtex(List<float> vertices, PunktNormal[] wieszcholki, int i, int j)
        {
            vertices.Add(wieszcholki[i * N_Y + j].x - 40f);
            vertices.Add(wieszcholki[i * N_Y + j].y - 40f);
            vertices.Add(wieszcholki[i * N_Y + j].z);
            vertices.Add(wieszcholki[i * N_Y + j].nx);
            vertices.Add(wieszcholki[i * N_Y + j].ny);
            vertices.Add(wieszcholki[i * N_Y + j].nz);
        }

        private void PrzygotowanieBufora(float[] vertices)
        {
            lock (_bufferLock)
            {
                if (_vertexBufferObject == 0)
                {
                    _vertexBufferObject = GL.GenBuffer();
                }
                GL.BindBuffer(BufferTarget.ArrayBuffer, _vertexBufferObject);

                GL.BufferData(BufferTarget.ArrayBuffer, vertices.Length * sizeof(float), vertices, BufferUsageHint.DynamicDraw);

                if (_vertexArrayObject == 0)
                {
                    _vertexArrayObject = GL.GenVertexArray();
                }
                GL.BindVertexArray(_vertexArrayObject);

                GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 6 * sizeof(float), 0);
                GL.EnableVertexAttribArray(0);

                GL.VertexAttribPointer(1, 3, VertexAttribPointerType.Float, false, 6 * sizeof(float), 3 * sizeof(float));
                GL.EnableVertexAttribArray(1);
            }
        }

        protected override void OnRenderFrame(FrameEventArgs e)
        {
            //base.OnRenderFrame(e);

            GL.Clear(ClearBufferMask.ColorBufferBit);

            GL.UseProgram(_shaderProgram);
            GL.BindVertexArray(_vertexArrayObject);
            GL.LoadIdentity();

            // Rotate the image
            Matrix4 rotationMatrixZ = Matrix4.CreateRotationZ(MathHelper.DegreesToRadians(45.0f)); // Rotate 10 degrees around X axis
            Matrix4 rotationMatrixX = Matrix4.CreateRotationX(MathHelper.DegreesToRadians(30.0f)); // Rotate 30 degrees around Y axis
            Matrix4 rotationMatrix = rotationMatrixZ * rotationMatrixX; // Combine rotations

            Matrix4 projection = Matrix4.CreateOrthographic(150.0f, 150.0f, -50.0f, 50.0f);
            GL.UniformMatrix4(GL.GetUniformLocation(_shaderProgram, "projection"), false, ref projection);


            int location = GL.GetUniformLocation(_shaderProgram, "rotationMatrix");
            GL.UniformMatrix4(location, false, ref rotationMatrix);

            GL.DrawArrays(PrimitiveType.Triangles, 0, N_X * N_Y * 2);

            SwapBuffers();
        }

        unsafe void InitOpenGL()
        {
            #region tu mam problem miało być to do dzielenia zasobów między OpenCL i OpenGL

            GL.GenBuffers(1, out nbo);
            GL.BindBuffer(BufferTarget.ArrayBuffer, nbo);
            //GL.BufferData(BufferTarget.ArrayBuffer, sizeof(PunktNormal) * N_X * N_Y, IntPtr.Zero, BufferUsageHint.DynamicDraw);
            GL.BufferData(BufferTarget.ArrayBuffer, sizeof(float) * N_X * N_Y * 18, IntPtr.Zero, BufferUsageHint.DynamicDraw);

            #endregion

            #region przygotawanie schaderów

            string vertexShaderSource = File.ReadAllText("vertex_shader.glsl");

            string fragmentShaderSource = File.ReadAllText("fragment_shader.glsl");

            _vertexShader = GL.CreateShader(ShaderType.VertexShader);
            GL.ShaderSource(_vertexShader, vertexShaderSource);
            GL.CompileShader(_vertexShader);

            _fragmentShader = GL.CreateShader(ShaderType.FragmentShader);
            GL.ShaderSource(_fragmentShader, fragmentShaderSource);
            GL.CompileShader(_fragmentShader);

            _shaderProgram = GL.CreateProgram();
            GL.AttachShader(_shaderProgram, _vertexShader);
            GL.AttachShader(_shaderProgram, _fragmentShader);
            GL.LinkProgram(_shaderProgram);

            #endregion
        }

        unsafe void InitOpenCL()
        {
            var platform = ComputePlatform.Platforms[0];
            var devices = platform.Devices;

            clContext = new ComputeContext(devices, new ComputeContextPropertyList(platform), null, IntPtr.Zero);
            queue = new ComputeCommandQueue(clContext, devices[0], ComputeCommandQueueFlags.None);

            string kernelSource = File.ReadAllText("kernel.cl");
            var program = new ComputeProgram(clContext, kernelSource);
            program.Build(devices, null, null, IntPtr.Zero);
            kernel = program.CreateKernel("obliczWspolrzedne");
            kernelTrujkatow = program.CreateKernel("obliczNormalne");
            kernelPrzygotujTrojkaty = program.CreateKernel("przygotujTrojkaty");

            aaBuf = new ComputeBuffer<Punkt>(clContext, ComputeMemoryFlags.ReadWrite | ComputeMemoryFlags.CopyHostPointer, new Punkt[N_X * N_Y]);
            bbBuf = new ComputeBuffer<Punkt>(clContext, ComputeMemoryFlags.ReadWrite, N_X * N_Y);
            //clNbo = new ComputeBuffer<PunktNormal>(clContext, ComputeMemoryFlags.WriteOnly, N_X * N_Y);
            clNbo = new ComputeBuffer<PunktNormal>(clContext, ComputeMemoryFlags.ReadWrite, N_X * N_Y);

            //vertexBuffer = ComputeBuffer<float>.CreateFromGLBuffer<float>(clContext, ComputeMemoryFlags.ReadWrite, 1);// nbo);
            vertexBuffer = new ComputeBuffer<float>(clContext, ComputeMemoryFlags.ReadWrite, N_X * N_Y*4*18);



            queue.WriteToBuffer(aa, aaBuf, true, null);
        }

       
        void UpdateSinglePoint(int index, Punkt newValue)
        {
            queue.WriteToBuffer(new Punkt[] { newValue }, aaBuf, true, new IntPtr(index * Marshal.SizeOf(typeof(Punkt))), 0, 1, null);
            // Update the value in the local array
            //punktyINormalne[index].x = newValue.x;
            //punktyINormalne[index].y = newValue.y;
            //punktyINormalne[index].z = 5;

            // Write the updated value to the OpenCL buffer
            //queue.WriteToBuffer(new Punkt[] { newValue }, aaBuf, true, new IntPtr(index * Marshal.SizeOf(typeof(Punkt))), null);
        }

        private readonly object _syncLock = new object();

        async Task Akcja()
        {
            bool kt = false;
            while (true)
            {
                lock (_syncLock)
                {
                    kt = RunKernel(kt, czas);
                    czas += dt;

                    float[] vertices2 = PrzygotujTrojkaty(punktyINormalne);

                    PrzygotowanieBufora(vertices2);
                }

                await Task.Delay(1000);
            }
        }

        //async Task Akcja()
        //{
        //    bool kt = false;
        //    while (true)
        //    {
        //        kt = RunKernelUpdate(kt, czas);
        //        czas += dt;

        //        float[] vertices2 = PrzygotujTrojkaty(punktyINormalne);

        //        PrzygotowanieBufora(vertices2);

        //        Thread.Sleep(300);


        //    }

        //}

        object _bufferLock = new object();

        unsafe bool RunKernel(bool kt, float czas)
        {
            var b1 = kt ? aaBuf : bbBuf;
            var b2 = kt ? bbBuf : aaBuf;

            lock (_bufferLock)
            {
                //if (czas < 4 * 3.14)
                //{
                //    int index = N_Y * 50 + 50;
                //    aa[index].v = (float)Math.Sin(czas);
                //    aa[index].x = punktyINormalne[index].z; //w tablicy aa x odpowiada z
                //    queue.WriteToBuffer(new Punkt[] { aa[index] }, b1, true, new IntPtr(index * Marshal.SizeOf(typeof(Punkt))), 0, 1, null);
                //}

                kernel.SetMemoryArgument(0, b1);
                kernel.SetMemoryArgument(1, b2);
                kernel.SetValueArgument(2, dt);
                kernel.SetValueArgument(3, w);
                kernel.SetValueArgument(4, N_X);
                kernel.SetValueArgument(5, N_Y);

                //do wzbudzenia fal
                kernel.SetValueArgument(6, (float)Math.Sin(czas));
                kernel.SetValueArgument(7, czas);

                kernelTrujkatow.SetMemoryArgument(0, b2);
                kernelTrujkatow.SetMemoryArgument(1, clNbo);
                kernelTrujkatow.SetValueArgument(2, N_X);
                kernelTrujkatow.SetValueArgument(3, N_Y);

                GL.Finish();
                
                /*
                var queue = new ComputeCommandQueue(context, context.Devices[0], ComputeCommandQueueFlags.None);
                queue.AcquireGLObjects(new[] { clBuffer }, null);
                // Operacje OpenCL na buforze
                queue.ReleaseGLObjects(new[] { clBuffer }, null);
                queue.Finish();
                */
                queue.AcquireGLObjects(new[] { vertexBuffer }, null);

                var globalWorkSize = new long[] { N_X, N_Y };

                queue.Execute(kernel, null, globalWorkSize, null, null);
                queue.Execute(kernelTrujkatow, null, globalWorkSize, null, null);

                kernelPrzygotujTrojkaty.SetMemoryArgument(0, clNbo);
                kernelPrzygotujTrojkaty.SetMemoryArgument(1, vertexBuffer);
                kernelPrzygotujTrojkaty.SetValueArgument(2, N_X);
                kernelPrzygotujTrojkaty.SetValueArgument(3, N_Y);

                queue.Execute(kernelPrzygotujTrojkaty, null, globalWorkSize, null, null);

                //queue.ReadFromBuffer(vertexBuffer, ref punktyINormalne, true, null);


                //punktyINormalne = new PunktNormal[N_X * N_Y];

                //queue.ReadFromBuffer(clNbo, ref punktyINormalne, true, null);

                queue.ReleaseGLObjects(new[] { vertexBuffer }, null);
                queue.Finish();
                
            }
            return !kt;
        }
        private void CheckGLError(string location)
        {
            ErrorCode error = GL.GetError();
            if (error != ErrorCode.NoError)
            {
                throw new Exception($"OpenGL error at {location}: {error}");
            }
        }
        /////////////////////////////////////////////////////////////////////////////////////////////
        ///


    }
}
