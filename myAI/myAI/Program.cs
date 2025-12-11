// See https://aka.ms/new-console-template for more information
using System.ComponentModel.DataAnnotations;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Diagnostics;
using System.IO;
using System.Text;
using Tensorflow.Keras;
using Tensorflow;
using Keras.Datasets;
using Keras.Layers;
using Keras.Models;
using Keras.Utils;
using Numpy;

Console.WriteLine("Hello, World!");

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
