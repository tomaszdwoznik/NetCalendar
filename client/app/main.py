import tkinter as tk
from tkinter import messagebox
import json
from datetime import datetime
from calendar_ui import CalendarUI
from network_client import NetworkClient

class CalendarApp(CalendarUI):
    def __init__(self, root):
        # wywolanie nadrzednej klasy (CalendarUI)
        super().__init__(root)
        
        # inicjalizacja polaczenia i sieciowego daty 
        self.network = NetworkClient()
        self.current_year = datetime.now().year
        self.current_month = datetime.now().month
        self.selected_date = None
        self.events_data = [] 

        # budowa interfejsu
        self.setup_ui()
        self.refresh_view()

    # odswieza kalendarz i uaktualnia wydarzenia
    def refresh_view(self):
        self.draw_calendar(self.current_year, self.current_month, self.events_data, self.selected_date)
        self.update_event_list_view(self.events_data)

    # wysyla zapytanie o wydarzenia z danego miesiaca
    def fetch_events(self):
        if self.network.is_connected:
            date_query = f"{self.current_year}-{self.current_month:02d}"
            self.network.send_json({"action": "get_events", "date": date_query})

    # obsluguje dane otryzmane od serwera
    def on_message_received(self, data):
        try:
            parsed = json.loads(data)
            if isinstance(parsed, list):
                self.events_data = parsed
                self.root.after(0, self.refresh_view)
                return
        except json.JSONDecodeError:
            pass

        # aktualizacja logow        
        self.update_log(f"{data}")
        # gdy dodano wydarzenie odswiez liste wydarzen
        if data.strip() == "Dodano wydarzenie.":
            self.fetch_events()

    # inicjalizacja polaczenia SSL z serwerem 
    def handle_connect(self):
        ip = self.ip_entry.get()
        if self.network.connect(ip, 8888, self.on_message_received):
            self.btn_connect.pack_forget()
            self.btn_disconnect.pack(side='left', pady=15, ipady=2, ipadx=10)
            
            self.update_log("Połączono.")
            self.fetch_events()
        else:
            messagebox.showerror("Błąd", "Brak połączenia.")

    # funkcja obslugujaca rozlaczenie polaczenia z serwerem
    def handle_disconnect(self):
        if self.network.is_connected:
            self.network.disconnect()
            
            self.btn_disconnect.pack_forget()
            self.btn_connect.pack(side='left', pady=15, ipady=2, ipadx=10)
            
            self.btn_add.config(state='disabled')
            
            self.update_log("Rozłączono pomyślnie.")
            self.events_data = []
            self.refresh_view()

    # wyslanie zapytanie o dodaniu wydarzenia do serwera 
    def handle_add_event(self):
        time, title = self.time_entry.get(), self.title_entry.get()
        if not time or not title or not self.selected_date: return
        self.network.send_json({"action": "add_event", "date": self.selected_date, "time": time, "title": title})
        self.update_log(f"Dodawanie: {title}")

    # wybranie konkretnego dnia w celu dodania wydarzenia
    def select_day(self, date_str):
        self.selected_date = date_str
        self.lbl_selected_date.config(text=date_str)
        if self.network.is_connected: 
            self.btn_add.config(state='normal')
        self.refresh_view()

    # wyswietlenie poprzedniego miesiaca
    def prev_month(self):
        self.current_month = 12 if self.current_month == 1 else self.current_month - 1
        if self.current_month == 12: self.current_year -= 1
        self.refresh_view()
        self.fetch_events()

    # wyswietlenie nastepnego miesiaca
    def next_month(self):
        self.current_month = 1 if self.current_month == 12 else self.current_month + 1
        if self.current_month == 1: self.current_year += 1
        self.refresh_view()
        self.fetch_events()

# inicjalizacja startowa - start okna oraz petli programu
if __name__ == "__main__":
    root = tk.Tk()
    app = CalendarApp(root)
    root.mainloop()