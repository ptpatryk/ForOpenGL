Oto porównanie podstawowych instrukcji w **C#** i ich odpowiedników w **Python**:

***

### **1. Deklaracja zmiennej**

*   **C#:**
    ```csharp
    int x = 10;
    string name = "Jan";
    ```
*   **Python:**
    ```python
    x = 10
    name = "Jan"
    ```

*(Python nie wymaga typu – jest dynamicznie typowany.)*

***

### **2. Instrukcja warunkowa `if`**

*   **C#:**
    ```csharp
    if (x > 5)
    {
        Console.WriteLine("Większe niż 5");
    }
    else
    {
        Console.WriteLine("Mniejsze lub równe 5");
    }
    ```
*   **Python:**
    ```python
    if x > 5:
        print("Większe niż 5")
    else:
        print("Mniejsze lub równe 5")
    ```

***

### **3. Pętla `for`**

*   **C#:**
    ```csharp
    for (int i = 0; i < 5; i++)
    {
        Console.WriteLine(i);
    }
    ```
*   **Python:**
    ```python
    for i in range(5):
        print(i)
    ```

***

### **4. Pętla `while`**

*   **C#:**
    ```csharp
    int i = 0;
    while (i < 5)
    {
        Console.WriteLine(i);
        i++;
    }
    ```
*   **Python:**
    ```python
    i = 0
    while i < 5:
        print(i)
        i += 1
    ```

***

### **5. Funkcja**

*   **C#:**
    ```csharp
    int Dodaj(int a, int b)
    {
        return a + b;
    }
    ```
*   **Python:**
    ```python
    def dodaj(a, b):
        return a + b
    ```

***

### **6. Klasa**

*   **C#:**
    ```csharp
    class Osoba
    {
        public string Imie { get; set; }
        public Osoba(string imie)
        {
            Imie = imie;
        }
    }
    ```
*   **Python:**
    ```python
    class Osoba:
        def __init__(self, imie):
            self.imie = imie
    ```

***

### **7. Obsługa wyjątków**

*   **C#:**
    ```csharp
    try
    {
        int x = int.Parse("abc");
    }
    catch (Exception ex)
    {
        Console.WriteLine(ex.Message);
    }
    ```
*   **Python:**
    ```python
    try:
        x = int("abc")
    except Exception as ex:
        print(ex)
    ```

***

Chcesz, żebym przygotował **pełną tabelę porównawczą** z najczęściej używanymi konstrukcjami (np. `switch`, `foreach`, `using`, `lambda`, LINQ vs list comprehensions) w formacie **Markdown** albo **Excel**? Mogę też zrobić **wizualny schemat** pokazujący różnice w składni. Jak wolisz?
