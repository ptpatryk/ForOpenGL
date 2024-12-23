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
        //int vbo, nbo;
        //CLContext clContext;
        //CLCommandQueue queue;
        //CLKernel kernel;
        //CLBuffer aaBuf, bbBuf, clNbo;

        int vbo, nbo;
        ComputeContext clContext;
        ComputeCommandQueue queue;
        ComputeKernel kernel;
        ComputeKernel kernelTrujkatow;
        ComputeBuffer<Punkt> aaBuf, bbBuf;
        ComputeBuffer<PunktNormal> clNbo;

        int N_X = 100;
        int N_Y = 100;
        float dt = 0.01f;
        float w = -2.0f;

        private int _vertexBufferObject;
        private int _vertexArrayObject;
        private int _shaderProgram;

        public WindowsWave(int width, int height, string title)
            : base(GameWindowSettings.Default, new NativeWindowSettings() { Size = new OpenTK.Mathematics.Vector2i(width, height), Title = title })
        {

        }


        protected override void OnLoad()
        {
            base.OnLoad();

            //InitOpenGL(); - to było do metody 1

            InitOpenCL();

            // Przykładowe dane
            Punkt[] aa = new Punkt[N_X * N_Y];

            for (int i = 0; i < aa.Length; i++)
            {
                aa[i].m = 1.0f;
            }

            // Uruchomienie kernela
            var wieszcholki = RunKernel(aa);

            float[] vertices2 = PrzygotujTrojkaty(wieszcholki);

            #region rysowania statycznie trójkątów (metoda 2)

            float[] vertices = {
    // Trójkąt 1
    -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
   -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
    // Trójkąt 2
    -1.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
    1.0f,  -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
};



            PrzygotowanieBufora(vertices2);



            string vertexShaderSource = File.ReadAllText("vertex_shader.glsl");

            string fragmentShaderSource = File.ReadAllText("fragment_shader.glsl");

            int vertexShader = GL.CreateShader(ShaderType.VertexShader);
            GL.ShaderSource(vertexShader, vertexShaderSource);
            GL.CompileShader(vertexShader);

            int fragmentShader = GL.CreateShader(ShaderType.FragmentShader);
            GL.ShaderSource(fragmentShader, fragmentShaderSource);
            GL.CompileShader(fragmentShader);

            _shaderProgram = GL.CreateProgram();
            GL.AttachShader(_shaderProgram, vertexShader);
            GL.AttachShader(_shaderProgram, fragmentShader);
            GL.LinkProgram(_shaderProgram);

            //GL.Viewport(0, 0, 100, 100);
            //GL.MatrixMode(MatrixMode.Projection);
            //GL.LoadIdentity();
            //GL.Ortho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);


            //GL.Rotate(10.3, 0, 1, 0);


            GL.DeleteShader(vertexShader);
            GL.DeleteShader(fragmentShader);


            #endregion
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
            vertices.Add(wieszcholki[i * N_Y + j].x-40f);
            vertices.Add(wieszcholki[i * N_Y + j].y-40f);
            vertices.Add(wieszcholki[i * N_Y + j].z);
            vertices.Add(wieszcholki[i * N_Y + j].nx);
            vertices.Add(wieszcholki[i * N_Y + j].ny);
            vertices.Add(wieszcholki[i * N_Y + j].nz);
        }

        private void PrzygotowanieBufora(float[] vertices)
        {
            _vertexBufferObject = GL.GenBuffer();
            GL.BindBuffer(BufferTarget.ArrayBuffer, _vertexBufferObject);
            GL.BufferData(BufferTarget.ArrayBuffer, vertices.Length * sizeof(float), vertices, BufferUsageHint.StaticDraw);

            _vertexArrayObject = GL.GenVertexArray();
            GL.BindVertexArray(_vertexArrayObject);

            GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 6 * sizeof(float), 0);
            GL.EnableVertexAttribArray(0);

            GL.VertexAttribPointer(1, 3, VertexAttribPointerType.Float, false, 6 * sizeof(float), 3 * sizeof(float));
            GL.EnableVertexAttribArray(1);
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

            GL.DrawArrays(PrimitiveType.Triangles, 0, N_X * N_Y*2);

            SwapBuffers();
        }

        unsafe void InitOpenGL()
        {
            GL.GenBuffers(1, out nbo);
            GL.BindBuffer(BufferTarget.ArrayBuffer, nbo);
            GL.BufferData(BufferTarget.ArrayBuffer, sizeof(PunktNormal) * N_X * N_Y, IntPtr.Zero, BufferUsageHint.DynamicDraw);
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

            aaBuf = new ComputeBuffer<Punkt>(clContext, ComputeMemoryFlags.ReadOnly | ComputeMemoryFlags.CopyHostPointer, new Punkt[N_X * N_Y]);
            bbBuf = new ComputeBuffer<Punkt>(clContext, ComputeMemoryFlags.ReadWrite, N_X * N_Y);
            clNbo = new ComputeBuffer<PunktNormal>(clContext, ComputeMemoryFlags.WriteOnly, N_X * N_Y);
        }

        unsafe PunktNormal[] RunKernel(Punkt[] aa)
        {
            queue.WriteToBuffer(aa, aaBuf, true, null);

            kernel.SetMemoryArgument(0, aaBuf);
            kernel.SetMemoryArgument(1, bbBuf);
            kernel.SetValueArgument(2, dt);
            kernel.SetValueArgument(3, w);
            kernel.SetValueArgument(4, N_X);
            kernel.SetValueArgument(5, N_Y);

            kernelTrujkatow.SetMemoryArgument(0, bbBuf);
            kernelTrujkatow.SetMemoryArgument(1, clNbo);
            kernelTrujkatow.SetValueArgument(2, N_X);
            kernelTrujkatow.SetValueArgument(3, N_Y);

            GL.Finish();
            //queue.AcquireGLObjects(new[] { clNbo }, null);

            var globalWorkSize = new long[] { N_X, N_Y };

            queue.Execute(kernel, null, globalWorkSize, null, null);
            queue.Execute(kernelTrujkatow, null, globalWorkSize, null, null);

            PunktNormal[] normals = new PunktNormal[N_X * N_Y]; // Initialize the array
            queue.ReadFromBuffer(clNbo, ref normals, true, null);

            //queue.ReleaseGLObjects(new[] { clNbo }, null);
            queue.Finish();

            return normals;

            //Render();
        }

        unsafe void Render()
        {



            #region renderowanie ze zmiennej vertix 

            //GL.Clear(ClearBufferMask.ColorBufferBit);




            //GL.UseProgram(_shaderProgram);
            //GL.BindVertexArray(_vertexArrayObject);
            //GL.DrawArrays(PrimitiveType.Triangles, 0, 600);//rysuje 6 wierzchołków (2 trójkąty)






            #endregion

            #region próba rysowania trójkątów z płata - do poprawy

            //GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

            //GL.BindBuffer(BufferTarget.ArrayBuffer, nbo);
            //GL.VertexAttribPointer(0, 6, VertexAttribPointerType.Float, false, sizeof(PunktNormal), IntPtr.Zero);
            //GL.EnableVertexAttribArray(0);

            //GL.DrawArrays(PrimitiveType.Triangles, 0, N_X * N_Y);

            #endregion



            // Ustawienie kamery pod pewnym kątem
            // With the following code
            //Matrix4 projection = Matrix4.CreateOrthographic(100.0f, 100.0f, -1.0f, 1.0f);
            //GL.UniformMatrix4(GL.GetUniformLocation(_shaderProgram, "projection"), false, ref projection);

            // Replace the following lines in the Render method
            // GL.MatrixMode(MatrixMode.Modelview);
            // GL.LoadMatrix(ref modelview);

            // With the following code
            // GL.UniformMatrix4(GL.GetUniformLocation(_shaderProgram, "modelview"), false, ref modelview);

        }
    }
}
