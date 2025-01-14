//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using Tensorflow;
//using Tensorflow.Keras.Layers;
//using Tensorflow.Keras;
//using Tensorflow.Keras.Engine;

//namespace AIFramework
//{
//    internal class Program
//    {

//        static void Main(string[] args)
//        {
//            // Zdefiniuj zmienne wejściowe i wyjściowe
//            // Zdefiniuj zmienne wejściowe i wyjściowe
//            var inputData = new float[4, 2]
//            {
//                { 0, 0 },
//                { 0, 1 },
//                { 1, 0 },
//                { 1, 1 }
//            };
//            var outputData = new float[4, 1]
//            {
//                { 0 },
//                { 1 },
//                { 1 },
//                { 0 }
//            };

//            var model = new Sequential();
//            model.Add(new Dense(units: 2, input_shape: new[] { 2 }, activation: "sigmoid"));
//            model.Add(new Dense(units: 1, activation: "sigmoid"));
//            //Konfiguracja modelu
//            model.Compile(optimizer: "sgd", loss: "mean_squared_error");

//            var ddd = new Tensorflow.Keras.ArgsDefinition.DenseArgs();

//            ddd.InputShape = new[] { 2 };
//            ddd.Activation = ;

//            //Stwórz model perceptronu
//            var model = new Sequential(new List<Layer>() {
//                new Dense(ddd),
//                new Dense(1, activation: "sigmoid")
//            });

//            //    using (var graph = new TFGraph())
//            //    {
//            //        // Zdefiniuj zmienne wejściowe i wyjściowe w grafie
//            //        var input = graph.Placeholder(TFDataType.Float, new TFOutput[] { 2 });
//            //        var output = graph.Placeholder(TFDataType.Float, new TFOutput[] { 1 });

//            //        // Zdefiniuj warstwę ukrytą
//            //        var hidden = graph.Sigmoid(graph.Dense(input, 2, "hidden_layer"));

//            //        // Zdefiniuj warstwę wyjściową
//            //        var logits = graph.Dense(hidden, 1, "output_layer");

//            //        // Zdefiniuj funkcję straty i optymalizator
//            //        var loss = graph.MeanSquaredError(logits, output);
//            //        var optimizer = graph.GradientDescentOptimizer(0.5f);

//            //        // Zdefiniuj operację trenowania
//            //        var train = graph.Minimize(optimizer, loss);

//            //        // Stwórz sesję TensorFlow i inicjalizuj zmienne
//            //        using (var session = new TFSession(graph))
//            //        {
//            //            session.GetRunner().AddInput(input, inputData).AddInput(output, outputData).Fetch(loss).Run();

//            //            // Trenuj model wielokrotnie
//            //            for (int i = 0; i < 1000; i++)
//            //            {
//            //                session.GetRunner().AddInput(input, inputData).AddInput(output, outputData).Fetch(loss).Run();
//            //            }

//            //            // Użyj modelu do predykcji
//            //            var prediction = session.GetRunner().AddInput(input, new float[1, 2] { { 0, 1 } }).Fetch(logits).Run()[0];
//            //            Console.WriteLine("Predykcja dla wejścia [0, 1]: " + prediction.GetValue());
//            //        }
//            //    }
//            //}

//        }
//    }

    using System;
using System.Collections.Generic;
//using Tensorflow;
//using Tensorflow.Keras;
//using Tensorflow.Keras.ArgsDefinition;
//using Tensorflow.Keras.Engine;
//using Tensorflow.Keras.Layers;
//using Tensorflow.Operations.Activation;
//using Tensorflow.Operations.Losses;

using Keras.Datasets;
using Keras.Layers;
using Keras.Models;
using Keras.Utils;
using Numpy;

using System.IO;
using System.Linq;
using Tensorflow;
using TensorFlow;
//using TensorFlow;

namespace TensorFlowExample
{
    class Program
    {
        static void Main(string[] args)
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
            
            //wykomentowałem kod legasy, bo coś się pozmieniało i się nie kompilował - brakuje metod, itp.
            /*
            // Stwórz graf obliczeniowy TensorFlow
            using (var graph = new TFGraph())
            {
                // Zdefiniuj zmienne wejściowe i wyjściowe w grafie
                var input = graph.Placeholder(TFDataType.Float, new TFOutput[] { 2 });
                var output = graph.Placeholder(TFDataType.Float, new TFOutput[] { 1 });

                // Zdefiniuj warstwę ukrytą
                var hidden = graph.Sigmoid(graph.Dense(input, 2, "hidden_layer"));

                // Zdefiniuj warstwę wyjściową
                var logits = graph.Dense(hidden, 1, "output_layer");

                // Zdefiniuj funkcję straty i optymalizator
                var loss = graph.MeanSquaredError(logits, output);
                var optimizer = graph.GradientDescentOptimizer(0.5f);

                // Zdefiniuj operację trenowania
                var train = graph.Minimize(optimizer, loss);

                // Stwórz sesję TensorFlow i inicjalizuj zmienne
                using (var session = new TFSession(graph))
                {
                    session.GetRunner().AddInput(input, inputData).AddInput(output, outputData).Fetch(loss).Run();

                    // Trenuj model wielokrotnie
                    for (int i = 0; i < 1000; i++)
                    {
                        session.GetRunner().AddInput(input, inputData).AddInput(output, outputData).Fetch(loss).Run();
                    }

                    // Użyj modelu do predykcji
                    var prediction = session.GetRunner().AddInput(input, new float[1, 2] { { 0, 1 } }).Fetch(logits).Run()[0];
                    Console.WriteLine("Predykcja dla wejścia [0, 1]: " + prediction.GetValue());
                }
            }
            */
            //KerasClass ff=new KerasClass();
            //ff.TrainModel();

            // Stwórz graf obliczeniowy TensorFlow
            //Następnie, zamiast korzystania z klasy TFGraph, używaj klasy Model z Tensorflow.Keras, która pozwala na tworzenie sieci neuronowych za pomocą warstw.

            //using (var model = new Sequential())
            //{

            //    //W celu utworzenia warstwy ukrytej i wyjściowej, użyj metody Add na obiekcie modelu, przekazując odpowiednią klasę warstwy i jej parametry.

            //    model.Add(new Dense(2, activation: "sigmoid", input_shape: 2));
            //    model.Add(new Dense(1, activation: "sigmoid"));

            //    //Następnie konieczne będzie skonfigurowanie modelu przed jego trenowaniem, a następnie wykonanie trenowania przy użyciu metody Fit:

            //    model.Compile(optimizer: "sgd", loss: "mean_squared_error");
            //    model.Fit(inputData, outputData, epochs: 1000);

            //    //Na koniec, aby wykonać predykcję na modelu wykorzystać metodę Predict

            //    var prediction = model.Predict(new float[,] { { 0, 1 } });

            //    //Console.WriteLine("Predykcja dla wejścia [0, 1]: " + prediction.get);

            //    //// Zdefiniuj zmienne wejściowe i wyjściowe w grafie
            //    //var input = graph.Placeholder(TFDataType.Float, new TFOutput[] { 2 });
            //    //var output = graph.Placeholder(TFDataType.Float, new TFOutput[] { 1 });

            //    //// Zdefiniuj warstwę ukrytą
            //    //var hidden = graph.Sigmoid(graph.Dense(input, 2, "hidden_layer"));

            //    //// Zdefiniuj warstwę wyjściową
            //    //var logits = graph.Dense(hidden, 1, "output_layer");

            //    //// Zdefiniuj funkcję straty i optymalizator
            //    //var loss = graph.MeanSquaredError(logits, output);
            //    //var optimizer = graph.GradientDescentOptimizer(0.5f);

            //    //// Zdefiniuj operację trenowania
            //    //var train = graph.Minimize(optimizer, loss);

            //    //// Stwórz sesję TensorFlow i inicjalizuj zmienne
            //    //using (var session = new TFSession(graph))
            //    //{
            //    //    session.GetRunner().AddInput(input, inputData).AddInput(output, outputData).Fetch(loss).Run();

            //    //    // Trenuj model wielokrotnie
            //    //    for (int i = 0; i < 1000; i++)
            //    //    {
            //    //        session.GetRunner().AddInput(input, inputData).AddInput(output, outputData).Fetch(loss).Run();
            //    //    }

            //    //    // Użyj modelu do predykcji
            //    //    var prediction = session.GetRunner().AddInput(input, new float[1, 2] { { 0, 1 } }).Fetch(logits).Run()[0];
            //    //    Console.WriteLine("Predykcja dla wejścia [0, 1]: " + prediction.GetValue());
            //    //}
            //}
        }


        // Zdefiniuj zmienne wejściowe i wyjściowe
        //    var inputData = new float[4, 2]
        //    {
        //        { 0, 0 },
        //        { 0, 1 },
        //        { 1, 0 },
        //        { 1, 1 }
        //    };
        //    var outputData = new float[4, 1]
        //    {
        //        { 0 },
        //        { 1 },
        //        { 1 },
        //        { 0 }
        //    };

        //    NeuralNetArgs argsx;

        //    //Stwórz model perceptronu
        //    DenseArgs da1 = new DenseArgs()
        //    {
        //        Units = 2,
        //        InputShape = new[] { 2 },
        //        Activation = activations.relu
        //    };

        //    var model = new Sequential(new List<Layer>() {
        //        new Dense(2, input_shape: new[] { 2 }, activation: "sigmoid"),
        //        new Dense(1, activation: "sigmoid")
        //    });
        //    //Konfiguracja modelu
        //    model.compile("sgd", "mean_squared_error", null);
        //    //Trenowanie modelu
        //    model.fit(inputData, outputData, epochs: 1000);
        //    //Testowanie modelu
        //    var prediction = model.predict(new float[,] { { 0, 1 } });
        //    //Console.WriteLine("Predykcja dla wejścia
        //}
    }
    //https://www.codeproject.com/Articles/5286497/Starting-with-Keras-NET-in-Csharp-Train-Your-First
    //class KerasClass
    //{
    //    public void TrainModel()
    //    {

    //        int batch_size = 1000;   //Size of the batches per epoch
    //        int num_classes = 10;    //We got 10 outputs since 
    //                                 //we can predict 10 different labels seen on the 
    //                                 //dataset: https://github.com/zalandoresearch/fashion-mnist#labels
    //        int epochs = 30;         //Amount on trainingperiods, 
    //                                 //I figure it out that the maximum is something about 
    //                                 //700 epochs, after this it won't increase the 
    //                                 //accuracy siginificantly

    //        // input image dimensions
    //        int img_rows = 28, img_cols = 28;

    //        // the data, split between train and test sets
    //        var ((x_train, y_train), (x_test, y_test)) =
    //                                  FashionMNIST.LoadData(); // Load the datasets from 
    //                                                           // fashion MNIST, Keras.Net implement this directly

    //        x_train.reshape(-1, img_rows, img_cols).astype(np.float32); //ByteArray needs 
    //                                                                    //to be reshaped to fit the dimmensions of the y arrays

    //        y_train = Util.ToCategorical(y_train, num_classes); //here, you modify the 
    //                                                            //forecast data to 10 outputs
    //                                                            //as we have 10 different 
    //                                                            //labels to predict (see the 
    //                                                            //Labels on the Dataset)
    //        y_test = Util.ToCategorical(y_test, num_classes);   //same for the test data 
    //                                                            //[hint: you can change this 
    //                                                            //in example you want to 
    //                                                            //make just a binary 
    //                                                            //crossentropy as you just 
    //                                                            //want to figure, i.e., if 
    //                                                            //this is a angleboot or not

    //        var model = new Sequential();

    //        model.Add(new Dense(100, 784, "sigmoid")); //hidden dense layer, with 100 neurons, 
    //                                                   //you have 28*28 pixel which make 
    //                                                   //784 'inputs', and sigmoid function 
    //                                                   //as activationfunction
    //        model.Add(new Dense(10, null, "sigmoid")); //Ouputlayer with 10 outputs,...
    //        model.Compile(optimizer: "sgd", loss: "categorical_crossentropy",
    //            metrics: new string[] { "accuracy" }); //we have a crossentropy as prediction 
    //                                                   //and want to see as well the 
    //                                                   //accuracy metric.

    //        var X_train = x_train.reshape(60000, 784); //this is actually very important. 
    //                                                   //C# works with pointers, 
    //                                                   //so if you have to reshape (again) 
    //                                                   //the function for the correct 
    //                                                   //processing, you need to write this 
    //                                                   //to a different var
    //        var X_test = x_test.reshape(10000, 784);

    //        model.Fit(X_train, y_train, batch_size, epochs, 1); //now, we set the data to 
    //                                                            //the model with all the 
    //                                                            //arguments (x and y data, 
    //                                                            //batch size...the '1' is 
    //                                                            //just verbose=1

    //        Console.WriteLine("---------------------");
    //        Console.WriteLine(X_train.shape);
    //        Console.WriteLine(X_test.shape);
    //        Console.WriteLine(y_train[0]);
    //        Console.WriteLine(y_train[1]);       //some outputs...you can play with them

    //        var y_train_pred = model.Predict(X_train);       //prediction on the train data
    //        Console.WriteLine(y_train_pred);

    //        model.Evaluate(X_test.reshape(-1, 784), y_test); //-1 tells the code that 
    //                                                         //it can figure out the size of 
    //                                                         //the array by itself
    //    }

    //    private byte[] openDatas(string path, int skip)      //just the open Data function. 
    //                                                         //As I mentioned, I did not work 
    //                                                         //with unzip stuff, you have 
    //                                                         //to unzip the data before 
    //                                                         //by yourself
    //    {
    //        var file = File.ReadAllBytes(path).Skip(skip).ToArray();
    //        return file;
    //    }

    //    //Hint: First, I was working by opening the data locally and 
    //    //I wanted to figure it out how to present data to the arrays. 
    //    //So you can use something like this and call this within the TrainModel() function:
    //    //x_train = openDatas(@"PATH\OF\YOUR\DATAS\train-images-idx3-ubyte", 16);
    //    //y_train = openDatas(@"PATH\OF\YOUR\DATAS\train-labels-idx1-ubyte", 8);
    //    //x_test = openDatas(@"PATH\OF\YOUR\DATAS\t10k-images-idx3-ubyte", 16);
    //    //y_test = openDatas(@"PATH\OF\YOUR\DATAS\t10k-labels-idx1-ubyte", 8);
    //}

}