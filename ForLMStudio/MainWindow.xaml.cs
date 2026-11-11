using Microsoft.Extensions.AI;
using OpenAI;
using System.ClientModel;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Net.Http.Json;
using System.Reflection;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Net.Http.Headers;
using System.Net.Http.Json;

namespace ForLMStudio
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private async void Button_Click(object sender, RoutedEventArgs e)
        {
            //        //var message = new List<ChatMessage>
            //        //{
            //        //    new(ChatRole.System, "Jesteś pomocnym asystentem."),
            //        //    new(ChatRole.User, TBWejscie.Text)
            //        //};

            //        var httpClient = new HttpClient();

            //        //using Microsoft.Extensions.AI;
            //        //using OpenAI;

            //        var openAiClient = new OpenAIClient(
            //            new ApiKeyCredential("lm-studio"));

            //        var chatClient = openAiClient.AsChatClient(
            //            modelId: "llama-3.3-70b-instruct",
            //            endpoint: new Uri("http://192.168.100.5:1234/v1"));

            //        //var request = new
            //        //{
            //        //    //model = "qwen2.5-7b-instruct",
            //        //    model = "llama-3.3-70b-instruct",
            //        //    messages = message   
            //        //};

            //        var request = new
            //        {
            //            model = "llama-3.3-70b-instruct",
            //            messages = new[]
            //{
            //    new
            //    {
            //        role = "system",
            //        content = "Jesteś pomocnym asystentem."
            //    },
            //    new
            //    {
            //        role = "user",
            //        content = TBWejscie.Text
            //    }
            //}
            //        };


            //        var response = await httpClient.PostAsJsonAsync(
            //            "http://192.168.100.5:1234/v1/chat/completions",
            //            request);

            //        var wynik = await response.Content.ReadAsByteArrayAsync();

            //        var st = UTF8Encoding.UTF8.GetString(wynik);

            //        TBWyjscie.Text = UTF8Encoding.UTF8.GetString(wynik);


            var httpClient = new HttpClient();


            ///-------------------
            //IChatProvider provider = new OpenAiCompatibleChatProvider(
            //    httpClient,
            //    endpoint: "http://192.168.100.5:1234/v1",   // LM Studio
            //    model: "llama-3.3-70b-instruct",
            //    apiKey: null,
            //    defaultSystemPrompt: "Jesteś pomocnym asystentem.");

            //var agent = new Agent(provider);

            //var answer = await agent.AskAsync("Napisz krótko czym jest AI.");
            //Console.WriteLine(answer);


            var settings = new ChatProviderSettings
            {
                Endpoint = "http://192.168.100.5:1234/v1",
                Model = "llama-3.3-70b-instruct"
            };

            IChatProvider provider = new OpenAiCompatibleChatProvider(
                new HttpClient(),
                settings.Endpoint,
                settings.Model,
                settings.ApiKey,
                settings.SystemPrompt);

            var agent = new Agent(provider);

            var answer = await agent.AskAsync("Napisz krótko czym jest AI.");
            Console.WriteLine(answer);


            //        IChatProvider provider = new OpenAiCompatibleChatProvider(
            //new HttpClient(),
            //endpoint: "https://api.openai.com/v1",
            //model: "gpt-4o-mini",
            //apiKey: "sk-...",
            //defaultSystemPrompt: "Jesteś pomocnym asystentem.");
        }
    }

    public class ChatCompletionResponse
    {
        public string Id { get; set; }
        public string Model { get; set; }
        public List<Choice> Choices { get; set; }
    }

    public class Choice
    {
        public int Index { get; set; }
        public AssistantMessage Message { get; set; }
        public string Finish_Reason { get; set; }
    }

    public class AssistantMessage
    {
        public string Role { get; set; }
        public string Content { get; set; }
    }

    public interface IChatProvider
    {
        Task<string> SendAsync(string userMessage, string? systemPrompt = null, CancellationToken cancellationToken = default);
    }

 
public sealed class OpenAiCompatibleChatProvider : IChatProvider
    {
        private readonly HttpClient _httpClient;
        private readonly string _endpoint;
        private readonly string _model;
        private readonly string _apiKey;
        private readonly string _defaultSystemPrompt;

        public OpenAiCompatibleChatProvider(
            HttpClient httpClient,
            string endpoint,
            string model,
            string? apiKey = null,
            string defaultSystemPrompt = "Jesteś pomocnym asystentem.")
        {
            _httpClient = httpClient;
            _endpoint = endpoint.TrimEnd('/');
            _model = model;
            _apiKey = apiKey ?? "";
            _defaultSystemPrompt = defaultSystemPrompt;
        }

        public async Task<string> SendAsync(
            string userMessage,
            string? systemPrompt = null,
            CancellationToken cancellationToken = default)
        {
            var requestBody = new
            {
                model = _model,
                messages = new[]
                {
                new { role = "system", content = systemPrompt ?? _defaultSystemPrompt },
                new { role = "user", content = userMessage }
            },
                temperature = 0.7
            };

            using var request = new HttpRequestMessage(HttpMethod.Post, $"{_endpoint}/chat/completions");
            request.Content = JsonContent.Create(requestBody);

            if (!string.IsNullOrWhiteSpace(_apiKey))
            {
                request.Headers.Authorization = new AuthenticationHeaderValue("Bearer", _apiKey);
            }

            using var response = await _httpClient.SendAsync(request, cancellationToken);
            var json = await response.Content.ReadAsStringAsync(cancellationToken);

            if (!response.IsSuccessStatusCode)
            {
                throw new InvalidOperationException($"API error: {(int)response.StatusCode} - {json}");
            }

            using var doc = System.Text.Json.JsonDocument.Parse(json);
            return doc.RootElement
                .GetProperty("choices")[0]
                .GetProperty("message")
                .GetProperty("content")
                .GetString() ?? "";
        }
    }

    public sealed class Agent
    {
        private readonly IChatProvider _provider;

        public Agent(IChatProvider provider)
        {
            _provider = provider;
        }

        public Task<string> AskAsync(string question, string? systemPrompt = null)
        {
            return _provider.SendAsync(question, systemPrompt);
        }
    }

    public sealed class ChatProviderSettings
    {
        public string Endpoint { get; set; } = "";
        public string Model { get; set; } = "";
        public string? ApiKey { get; set; }
        public string SystemPrompt { get; set; } = "Jesteś pomocnym asystentem.";
    }


}