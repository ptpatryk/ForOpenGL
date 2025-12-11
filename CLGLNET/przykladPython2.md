Świetnie — poniżej masz **pełne, praktyczne zestawienie** najważniejszych konstrukcji **C# → Python**, z krótkimi objaśnieniami i przykładami. Ułożyłem je tematycznie, żeby szybko znaleźć odpowiedniki. (Jeśli chcesz, mogę z tego wygenerować plik **Excel** lub **PDF** z czytelną tabelą—powiedz tylko, jaki format wolisz.)

***

## 1) Zmienne i typy

**Deklaracja i przypisanie**

```csharp
int x = 10;
string s = "tekst";
var y = 3.14; // wnioskowanie typu
```

```python
x = 10
s = "tekst"
y = 3.14  # typ wynika z wartości
```

**Stałe**

```csharp
const double Pi = 3.1415;
readonly int MaxCount = 100;
```

```python
PI = 3.1415  # umownie wielkie litery; brak "const" w języku
```

**Null / None**

```csharp
string? name = null;
if (name == null) { ... }
```

```python
name = None
if name is None:
    ...
```

***

## 2) Operatory

**Arytmetyczne / porównania**

```csharp
a + b; a - b; a * b; a / b; a % b;
a == b; a != b; a > b; a <= b;
```

```python
a + b; a - b; a * b; a / b; a % b
a == b; a != b; a > b; a <= b
```

**Logiczne**

```csharp
bool ok = a && b || !c;
```

```python
ok = (a and b) or (not c)
```

**Przypisanie z operacją**

```csharp
x += 1; x -= 1; x *= 2; x /= 2;
```

```python
x += 1; x -= 1; x *= 2; x /= 2
```

***

## 3) Warunki (`if` / `else if` / `else`)

```csharp
if (x > 10) { ... }
else if (x == 10) { ... }
else { ... }
```

```python
if x > 10:
    ...
elif x == 10:
    ...
else:
    ...
```

**Operator trójargumentowy**

```csharp
var msg = (x > 0) ? "plus" : "nie-plus";
```

```python
msg = "plus" if x > 0 else "nie-plus"
```

***

## 4) Pętle

**`for` (licznik)**

```csharp
for (int i = 0; i < 5; i++)
{
    Console.WriteLine(i);
}
```

```python
for i in range(5):
    print(i)
```

**`foreach` / iteracja po kolekcji**

```csharp
foreach (var item in list)
{
    Console.WriteLine(item);
}
```

```python
for item in my_list:
    print(item)
```

**`while`**

```csharp
int i = 0;
while (i < 5) { i++; }
```

```python
i = 0
while i < 5:
    i += 1
```

**`break` / `continue`**

```csharp
foreach (var x in list)
{
    if (x == 0) continue;
    if (x < 0) break;
}
```

```python
for x in my_list:
    if x == 0:
        continue
    if x < 0:
        break
```

***

## 5) Przełączanie: `switch` vs `match` (Python 3.10+)

```csharp
switch (status)
{
    case 200: Console.WriteLine("OK"); break;
    case 404: Console.WriteLine("Not Found"); break;
    default:  Console.WriteLine("Other"); break;
}
```

```python
match status:
    case 200:
        print("OK")
    case 404:
        print("Not Found")
    case _:
        print("Other")
```

***

## 6) Funkcje, parametry, zwroty

```csharp
int Add(int a, int b) => a + b;
void Log(string msg = "domyślne") { Console.WriteLine(msg); }
```

```python
def add(a, b):
    return a + b

def log(msg="domyślne"):
    print(msg)
```

**Parametry nazwane / zmienne**

```csharp
void Print(string title, params int[] values) { ... }
Print(title: "Raport", values: new[] {1,2,3});
```

```python
def print_values(title, *values):
    ...
print_values(title="Raport", *[1, 2, 3])
```

**Funkcje anonimowe (lambda)**

```csharp
Func<int,int,int> add = (a,b) => a + b;
```

```python
add = lambda a, b: a + b
```

***

## 7) Klasy, właściwości, konstruktor

```csharp
class Person
{
    public string Name { get; set; }
    public Person(string name) { Name = name; }
}
```

```python
class Person:
    def __init__(self, name):
        self.name = name
```

**Właściwości z logiką**

```csharp
class Temperature
{
    private double celsius;
    public double Celsius
    {
        get => celsius;
        set => celsius = value;
    }
}
```

```python
class Temperature:
    def __init__(self):
        self._celsius = 0

    @property
    def celsius(self):
        return self._celsius

    @celsius.setter
    def celsius(self, value):
        self._celsius = value
```

**Statyczne człony**

```csharp
class App
{
    public static int Count { get; set; }
    public static void Inc() => Count++;
}
```

```python
class App:
    count = 0  # atrybut klasy

    @classmethod
    def inc(cls):
        cls.count += 1
```

***

## 8) Dziedziczenie, interfejsy, klasy abstrakcyjne

```csharp
interface IRun { void Run(); }
abstract class Animal { public abstract void Speak(); }
class Dog : Animal, IRun { public override void Speak() { ... } public void Run() { ... } }
```

```python
from abc import ABC, abstractmethod

class RunProtocol(ABC):
    @abstractmethod
    def run(self):
        ...

class Animal(ABC):
    @abstractmethod
    def speak(self):
        ...

class Dog(Animal, RunProtocol):
    def speak(self):
        ...
    def run(self):
        ...
```

*(Python nie ma „interfejsów” wprost; używa ABC/`protocols` z `typing_extensions`.)*

***

## 9) Moduły / przestrzenie nazw

```csharp
namespace MyApp.Core { class Util { } }
using MyApp.Core;
```

```python
# pliki i foldery to moduły/pakiety
# utils.py
class Util: ...
# użycie:
from utils import Util
```

***

## 10) Zarządzanie zasobami: `using` vs `with`

```csharp
using (var fs = File.OpenRead(path))
{
    // ...
}
```

```python
with open(path, "rb") as f:
    # ...
```

***

## 11) Wyjątki

```csharp
try
{
    var x = int.Parse("abc");
}
catch (FormatException ex)
{
    Console.WriteLine(ex.Message);
}
catch (Exception ex)
{
    // ogólny
}
```

```python
try:
    x = int("abc")
except ValueError as ex:
    print(ex)
except Exception as ex:
    ...
```

**Rzucanie**

```csharp
throw new InvalidOperationException("Błąd");
```

```python
raise RuntimeError("Błąd")
```

***

## 12) Kolekcje: lista/słownik/zestaw/tupla

```csharp
var list = new List<int>{1,2,3};
var dict = new Dictionary<string,int>{{"a",1},{"b",2}};
var set  = new HashSet<int>{1,2,2}; // {1,2}
var tup  = (1, "a");
```

```python
lst = [1, 2, 3]
dct = {"a": 1, "b": 2}
st  = {1, 2, 2}  # {1, 2}
tup = (1, "a")
```

**Comprehensions vs LINQ**

```csharp
var evens = nums.Where(n => n%2==0).Select(n => n*n).ToList();
```

```python
evens = [n*n for n in nums if n % 2 == 0]
```

***

## 13) Stringi i formatowanie

```csharp
var name = "Jan";
var s = $"Witaj, {name}!";                 // interpolacja
var s2 = string.Format("Witaj, {0}!", name);
```

```python
name = "Jan"
s  = f"Witaj, {name}!"                     # f-string
s2 = "Witaj, {}!".format(name)
```

**Metody**

```csharp
" Ala ".Trim().ToUpper();
```

```python
" Ala ".strip().upper()
```

***

## 14) Pliki, JSON, Regex

**Pliki**

```csharp
var text = File.ReadAllText(path);
File.WriteAllText(path, "data");
```

```python
from pathlib import Path
text = Path(path).read_text(encoding="utf-8")
Path(path).write_text("data", encoding="utf-8")
```

**JSON**

```csharp
using System.Text.Json;
var json = JsonSerializer.Serialize(obj);
var obj2 = JsonSerializer.Deserialize<MyType>(json);
```

```python
import json
json_str = json.dumps(obj, ensure_ascii=False)
obj2 = json.loads(json_str)
```

**Regex**

```csharp
var m = Regex.Match(s, @"\d+");
if (m.Success) Console.WriteLine(m.Value);
```

```python
import re
m = re.search(r"\d+", s)
if m:
    print(m.group())
```

***

## 15) Asynchroniczność (`async/await`)

```csharp
async Task<string> GetAsync()
{
    var s = await http.GetStringAsync(url);
    return s;
}
```

```python
import asyncio
import aiohttp

async def get_async(url):
    async with aiohttp.ClientSession() as session:
        async with session.get(url) as resp:
            return await resp.text()
```

**Uruchomienie pętli**

```csharp
var result = await GetAsync();
```

```python
result = asyncio.run(get_async(url))
```

***

## 16) Rzutowanie i typowanie ogólne

**Generics**

```csharp
List<T> list = new List<T>();
```

**Python typing**

```python
from typing import List, TypeVar
T = TypeVar("T")
def f(xs: List[T]) -> T: ...
```

**Pattern matching (Python) vs `is`/`switch` (C#)**

```csharp
if (obj is string s) { ... }
```

```python
match obj:
    case str(s):
        ...
```

***

## 17) Atrybuty vs dekoratory

```csharp
[Obsolete("użyj NewMethod")]
void OldMethod() { ... }
```

```python
def obsolete(func):
    def wrapper(*a, **kw):
        print("użyj new_method")
        return func(*a, **kw)
    return wrapper

@obsolete
def old_method(): ...
```

***

## 18) Zdarzenia i delegaty vs callbacki

```csharp
public event EventHandler OnChanged;
OnChanged?.Invoke(this, EventArgs.Empty);
```

```python
def on_changed():
    ...

callbacks = []
callbacks.append(on_changed)
for cb in callbacks:
    cb()
```

***

## 19) Różnice w składni i stylu

*   **Bloki**: C# używa `{ }`, Python **wymaga wcięć** (4 spacje).
*   **Komentarze**: `//` i `/* */` vs w Pythonie `#` oraz docstring `""" ... """`.
*   **Importy**: `using Namespace;` vs `import moduł` / `from moduł import nazwa`.
*   **Prawda/fałsz**: `true/false` vs `True/False`.
*   **Null**: `null` vs `None`.

***

## 20) Krótkie odpowiedniki „na szybko”

*   `foreach` ⇒ `for x in kolekcja:`
*   `using (IDisposable)` ⇒ `with resource:`
*   `switch` ⇒ `match` (3.10+), albo słownik funkcji:
    ```python
    actions = {200: lambda: print("OK"), 404: lambda: print("NF")}
    actions.get(status, lambda: print("Other"))()
    ```
*   LINQ `Where/Select` ⇒ list/dict/set comprehensions lub `filter/map`
*   `??` / `?.` (C#) ⇒ w Pythonie zwykle `x if x is not None else y`, oraz bezpieczny dostęp przez `get`/`try-except`

***

## 21) Mini-praktyka: odpowiedniki LINQ

```csharp
var result = persons
    .Where(p => p.Age >= 18)
    .OrderBy(p => p.Name)
    .Select(p => p.Name)
    .ToList();
```

```python
result = sorted(
    [p.name for p in persons if p.age >= 18],
    key=lambda n: n
)
```

***

## 22) Zarządzanie pakietami

*   **C#**: NuGet (`<PackageReference ... />`)
*   **Python**: `pip`, `pyproject.toml`, `requirements.txt`, wirtualne środowiska (`venv`)

***

### Chcesz plik?

Mogę przygotować **arkusz Excel** z czytelną tabelą „C# → Python” (grupy: kontrola przepływu, kolekcje, wyjątki, async, IO, stringi itd.). Jeśli wolisz — wygeneruję **PDF** lub **Markdown** z indeksowanym spisem treści.  
**Daj znać, jaki format wybierasz (Excel/PDF/Markdown) i czy dodać przykłady zaawansowane (np. `async` z `CancellationToken` → `asyncio` cancel, `IEnumerable` → generatory `yield`).**
