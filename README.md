# NetCalendar 

### 1. Temat zadania
System współdzielonego kalendarza sieciowego

### 2. Opis protokołu komunikacyjnego
Komunikacja w systemie opiera się na architekturze klient-serwer z wykorzystaniem gniazd i przesyłania danych w formacie JSON. Komunikacja odbywa się przy użyciu protokołu TCP, a połączenie pomiędzy klientem i serwerem jest szyfrowane za pomocą biblioteki OpenSSL.

### 3. Opis implementacji
Współbieżny serwer został zaimplementowany w języku C dla środowiska Linux.
Klient został zaimplementowany w języku Python przy użyciu biblioteki graficznej Tkinter dla środowiska Windows. 

#### Opis plików źródłowych
##### SERWER
- `main.c`: Główna pętla serwera do jednoczesnej obsługi wielu klientów, zarządzanie sesjami SSL oraz obsługi żądań JSON.
- `calendar.c`: Implementacja operacji na bazie danych JSON.
- `network_utils.c`: Konfiguracja gniazd sieciowych.
- `ssl_methods.c`: Inicjalizacja kontekstu OpenSSL wraz z certyfikatami oraz obsługa zaszyfrowanego odczytu i zapisu danych.
- `common_utils.c`: Zarządzanie tablicą stanów użytkowników.
- `cJSON.c`: Zewnętrzna biblioteka do przetwarzania danych w formacie JSON w języku C.
##### KLIENT
- `main.py`: Główna klasa aplikacji klienta; odpowiada m.in. za przetwarzanie komunikatów sieciowych, obsługę błędów oraz zarządzanie stanem aplikacji.
- `calendar_ui.py`: Odpowiada za warstwę wizualną klienta.
- `style.py`: Plik zawierający paletę kolorów stosowaną w `calendar_ui.py`
- `network_client.py`: Plik odpowiedzialny za połączenie TCP, wysyłanie danych JSON oraz odbieranie komunikatów z serwera.

### 4. Sposób uruchomienia i obsługi
#### SERWER
1. W folderze `NetCalendar` wpisać w terminalu `make`.
2. Uruchomić w terminalu skompilowany plik `./server_app`.

#### KLIENT
1. Uruchomić skrypt `run_client.bat`.
