// See https://aka.ms/new-console-template for more information
using System.ComponentModel.DataAnnotations;
using System;
using System.Collections.Generic;
using System.Linq;
//using Tensorflow;
////using Tensorflow;
//using static Tensorflow.Binding;
//using static Tensorflow.KerasApi;
//using Tensorflow;
//using Tensorflow.NumPy;
//using System.ServiceModel.Channels;
//using Tensorflow;
using Tensorflow.Keras;
using Tensorflow;
//using Tensorflow.Keras.Engine;
//using Tensorflow.Keras.Engine;
using Keras.Datasets;
using Keras.Layers;
using Keras.Models;
using Keras.Utils;
using Numpy;

Console.WriteLine("Hello, World!");
// Zdefiniuj zmienne wejściowe i wyjściowe
//static void Main(string[] args)
{
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

    // Stwórz graf obliczeniowy TensorFlow
    //Następnie, zamiast korzystania z klasy TFGraph, używaj klasy Model z Tensorflow.Keras, która pozwala na tworzenie sieci neuronowych za pomocą warstw.

    using (var model = new Sequential())
    {

        //W celu utworzenia warstwy ukrytej i wyjściowej, użyj metody Add na obiekcie modelu, przekazując odpowiednią klasę warstwy i jej parametry.

        model.Add(new Dense(2, activation: "sigmoid", input_shape: 2));
        model.Add(new Dense(1, activation: "sigmoid"));

        //Następnie konieczne będzie skonfigurowanie modelu przed jego trenowaniem, a następnie wykonanie trenowania przy użyciu metody Fit:

        model.Compile(optimizer: "sgd", loss: "mean_squared_error");
        model.Fit(inputData, outputData, epochs: 1000);

        //Na koniec, aby wykonać predykcję na modelu wykorzystać metodę Predict

        model.Predict(new float[,] { { 0, 1 } });

        //Console.WriteLine("Predykcja dla wejścia [0, 1]: " + prediction.get);

        //// Zdefiniuj zmienne wejściowe i wyjściowe w grafie
        //var input = graph.Placeholder(TFDataType.Float, new TFOutput[] { 2 });
        //var output = graph.Placeholder(TFDataType.Float, new TFOutput[] { 1 });

        //// Zdefiniuj warstwę ukrytą
        //var hidden = graph.Sigmoid(graph.Dense(input, 2, "hidden_layer"));

        //// Zdefiniuj warstwę wyjściową
        //var logits = graph.Dense(hidden, 1, "output_layer");

        //// Zdefiniuj funkcję straty i optymalizator
        //var loss = graph.MeanSquaredError(logits, output);
        //var optimizer = graph.GradientDescentOptimizer(0.5f);

        //// Zdefiniuj operację trenowania
        //var train = graph.Minimize(optimizer, loss);

        //// Stwórz sesję TensorFlow i inicjalizuj zmienne
        //using (var session = new TFSession(graph))
        //{
        //    session.GetRunner().AddInput(input, inputData).AddInput(output, outputData).Fetch(loss).Run();

        //    // Trenuj model wielokrotnie
        //    for (int i = 0; i < 1000; i++)
        //    {
        //        session.GetRunner().AddInput(input, inputData).AddInput(output, outputData).Fetch(loss).Run();
        //    }

        //    // Użyj modelu do predykcji
        //    var prediction = session.GetRunner().AddInput(input, new float[1, 2] { { 0, 1 } }).Fetch(logits).Run()[0];
        //    Console.WriteLine("Predykcja dla wejścia [0, 1]: " + prediction.GetValue());
        //}
    }
}
