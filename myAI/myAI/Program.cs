// See https://aka.ms/new-console-template for more information
using Keras.Datasets;
using Keras.Layers;
using Keras.Models;
using Keras.Utils;
using Numpy;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using Tensorflow;
using Tensorflow.Keras;
using TorchSharp;
using static TorchSharp.torch;

//Console.WriteLine("Hello, World!");

using System;

        // ===== Konfiguracja urządzenia (CPU/CUDA) =====
        var device = torch.cuda.is_available() ? torch.CUDA : torch.CPU;
        Console.WriteLine($"Uruchamiam na: {(device.type == DeviceType.CUDA ? "CUDA" : "CPU")}");

        // Opcjonalnie: deterministyczność / seed
        torch.random.manual_seed(1234);

        // ===== Dane XOR =====
        // input: 4 próbki po 2 cechy
        var xData = new float[] {
            0f, 0f,
            0f, 1f,
            1f, 0f,
            1f, 1f
        };
        var yData = new float[] {
            0f,
            1f,
            1f,
            0f
        };

        // Tworzymy tensory o kształtach [4,2] i [4,1]
        var x = torch.tensor(xData, new long[] { 4, 2 }, dtype: ScalarType.Float32).to(device);
        var y = torch.tensor(yData, new long[] { 4, 1 }, dtype: ScalarType.Float32).to(device);

        // ===== Definicja modelu (Sequential) =====
        var model = torch.nn.Sequential(
            ("fc1", torch.nn.Linear(2, 2)),     // Dense(2, activation='sigmoid', input_shape=2)
            ("sig1", torch.nn.Sigmoid()),
            ("fc2", torch.nn.Linear(2, 1)),     // Dense(1, activation='sigmoid')
            ("sig2", torch.nn.Sigmoid())
        ).to(device);

        // ===== Strata i optymalizator =====
        var lossFn = torch.nn.MSELoss();       // mean_squared_error
        var optimizer = torch.optim.SGD(model.parameters(), learningRate: 0.1f);

        // ===== Trening =====
        model.train();
        int epochs = 1000;

        for (int epoch = 1; epoch <= epochs; epoch++)
        {
            // forward
            using var yhat = model.forward(x);
            using var loss = lossFn.forward(yhat, y);

            // backward + update
            optimizer.zero_grad();
            loss.backward();
            optimizer.step();

            // log co 100 epok
            if (epoch % 100 == 0)
            {
                // ToSingle() działa dla skalara; dla tensora 1x1 można też użyć .ToArray<float>()[0]
                Console.WriteLine($"Epoka {epoch}, loss = {loss.ToSingle()}");
            }
        }

// ===== Predykcja =====
/*
model.eval();
using var sample = torch.tensor(new float[] { 0f, 1f }, new long[] { 1, 2 }, dtype: ScalarType.Float32).to(device);
using var pred = model.forward(sample);

// ekstrakcja wartości z tensora [1,1]
var predVal = pred[0,0].ToString();
Console.WriteLine($"Predykcja dla [0,1] = {predVal:F4}");

Console.WriteLine("Predykcja wykonana.");
*/


model.eval();
using var sample = torch.tensor(new float[] { 0f, 1f }, new long[] { 1, 2 }, dtype: ScalarType.Float32).to(device);
using var pred = model.forward(sample);

// tensor [1,1] → pobierz element [0,0] i zrób skalar
float predVal = pred[0, 0].ToSingle();
Console.WriteLine($"Predykcja dla [0,1] = {predVal:F4}");



void UzycieBibliotekiWymagajacejPythona()
{
    // USTAWIENIE PYTHONNET PRZED INICJACJĄ pythonnet/Keras
    EnsurePythonForPythonNet();

    // Zdefiniuj zmienne wejściowe i wyjściowe
    var inputData = new float[4, 2]
    {
            { 0, 0 },
            { 0, 1 },
            { 1, 0 },
            { 1, 1 }
    };
    var outputData = new float[4, 1]
    {
            { 0 },
            { 1 },
            { 1 },
            { 0 }
    };

    // Stwórz graf obliczeniowy TensorFlow (Keras.NET)
    using (var model = new Sequential())
    {
        model.Add(new Dense(2, activation: "sigmoid", input_shape: 2));
        model.Add(new Dense(1, activation: "sigmoid"));

        model.Compile(optimizer: "sgd", loss: "mean_squared_error");
        model.Fit(inputData, outputData, epochs: 1000);

        var prediction = model.Predict(new float[,] { { 0, 1 } });
        Console.WriteLine("Predykcja wykonana.");
    }
}

static void EnsurePythonForPythonNet()
{
    // Jeśli użytkownik już ustawił PYTHONNET_PYDLL, sprawdzamy czy plik istnieje
    try
    {
        var existing = Environment.GetEnvironmentVariable("PYTHONNET_PYDLL");
        if (!string.IsNullOrEmpty(existing) && File.Exists(existing))
        {
            Console.WriteLine($"PYTHONNET_PYDLL już ustawione: {existing}");
            return;
        }

        // Spróbuj znaleźć python.exe przez 'where python'
        string pythonExe = null;
        try
        {
            var psi = new ProcessStartInfo("where", "python")
            {
                RedirectStandardOutput = true,
                UseShellExecute = false,
                CreateNoWindow = true,
                StandardOutputEncoding = Encoding.UTF8
            };
            using var p = Process.Start(psi);
            var output = p.StandardOutput.ReadToEnd();
            p.WaitForExit(1000);
            var lines = output.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
            if (lines.Length > 0)
                pythonExe = lines[0].Trim();
        }
        catch
        {
            // ignoruj błąd where
        }

        if (!string.IsNullOrEmpty(pythonExe) && File.Exists(pythonExe))
        {
            var dir = Path.GetDirectoryName(pythonExe);
            // Szukamy plików python*.dll obok python.exe lub w katalogu nadrzędnym
            var candidates = new List<string>();
            try
            {
                candidates.AddRange(Directory.GetFiles(dir, "python*.dll", SearchOption.TopDirectoryOnly));
                var parent = Path.GetDirectoryName(dir);
                if (!string.IsNullOrEmpty(parent))
                    candidates.AddRange(Directory.GetFiles(parent, "python*.dll", SearchOption.TopDirectoryOnly));
            }
            catch { }

            if (candidates.Count > 0)
            {
                // wybierz pierwszy pasujący (można dopracować wybór)
                var dll = candidates.OrderByDescending(p => p).First();
                Environment.SetEnvironmentVariable("PYTHONNET_PYDLL", dll);
                var path = Environment.GetEnvironmentVariable("PATH") ?? string.Empty;
                if (!path.Split(';', StringSplitOptions.RemoveEmptyEntries).Any(p => string.Equals(p, dir, StringComparison.OrdinalIgnoreCase)))
                {
                    Environment.SetEnvironmentVariable("PATH", dir + ";" + path);
                }
                Console.WriteLine($"Ustawiono PYTHONNET_PYDLL = {dll}");
                return;
            }
        }

        Console.WriteLine("Nie wykryto odpowiedniego pliku python*.dll obok python.exe.");
        Console.WriteLine("Zalecane: zainstaluj oficjalnego CPythona x64 3.8 i dodaj katalog instalacji do PATH,");
        Console.WriteLine("lub ustaw ręcznie zmienną środowiskową PYTHONNET_PYDLL na pełną ścieżkę do python38.dll.");
    }
    catch (Exception ex)
    {
        Console.WriteLine("Błąd podczas próby ustawienia PYTHONNET_PYDLL: " + ex.Message);
    }
}
