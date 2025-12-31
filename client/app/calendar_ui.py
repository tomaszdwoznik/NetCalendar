import tkinter as tk
import calendar
from style import color
from datetime import datetime

class CalendarUI:
    # wstepna konfiguracja okna i czcionek
    def __init__(self, root):
        self.root = root
        self.root.title("NetCalendar")
        
        self.root.geometry("800x600")
        self.root.state('zoomed') 
        self.root.configure(bg=color.style["bg_main"])

        try:
            self.f_header = (color.style["font_family"], 14, "bold")
            self.f_sub = (color.style["font_family"], 10, "bold")
            self.f_norm = (color.style["font_family"], 10)
            self.f_small = ("Consolas", 9)
        except:
            self.f_header = ("Arial", 14, "bold")
            self.f_sub = ("Arial", 10, "bold")
            self.f_norm = ("Arial", 10)
            self.f_small = ("Courier New", 9)

    # glowny setup budowy interfejsu
    def setup_ui(self):
        self._setup_connection_bar()

        main_content = tk.Frame(self.root, bg=color.style["bg_main"], padx=30, pady=30)
        main_content.pack(fill='both', expand=True)

        self._setup_left_column(main_content)
        self._setup_right_column(main_content)

    # tworzenie gornego panelu z polaczeniem do serwera  
    def _setup_connection_bar(self):
        # panel z przyciskami i polem tekstowym
        conn_panel = tk.Frame(self.root, bg=color.style["bg_card"], height=60, padx=20, 
                              highlightbackground=color.style["border"], highlightthickness=1)
        conn_panel.pack(fill='x', side='top', pady=(0, 1))
        
        # linia pod panelem
        tk.Frame(self.root, bg=color.style["highlight"], height=1).pack(fill='x', side='top')

        # pole wprowadzenia adresu IP serwera
        self.ip_entry = tk.Entry(conn_panel, width=20, bg="#F0F2F5", fg=color.style["text_main"], 
                                 relief='flat', font=self.f_norm,
                                 highlightbackground=color.style["border"], highlightthickness=1)
        self.ip_entry.insert(0, "127.0.0.1")
        self.ip_entry.pack(side='left', padx=(0, 15), pady=15, ipady=5)

        # przycisk inicujacy polaczenie z serwerem
        self.btn_connect = tk.Button(conn_panel, text="POŁĄCZ", command=self.handle_connect, 
                                     bg=color.style["btn_connect"], fg=color.style["text_light"], 
                                     activebackground=color.style["highlight"], activeforeground=color.style["text_main"],
                                     relief='flat', font=("Arial", 9, "bold"), cursor="hand2")
        self.btn_connect.pack(side='left', pady=15, ipady=2, ipadx=10)

        # przycisk rozlacz pojawiajacy sie po polaczeniu z serwerem 
        self.btn_disconnect = tk.Button(conn_panel, text="ROZŁĄCZ", command=self.handle_disconnect, 
                                        bg="#e74c3c", fg="white", 
                                        activebackground="#c0392b", activeforeground="white",
                                        relief='flat', font=("Arial", 9, "bold"), cursor="hand2")
        
    # konfiguracja lewej strony interfejsu
    def _setup_left_column(self, parent):
        self.left_card = tk.Frame(parent, bg=color.style["bg_card"], 
                                  highlightbackground=color.style["border"], highlightthickness=1)
        self.left_card.place(relx=0, rely=0, relwidth=0.63, relheight=1.0)

        self._setup_navigation()
        self._setup_calendar_area()
        self._setup_event_list()

    # konfiguracja prawej strony interfejsu
    def _setup_right_column(self, parent):
        self.right_column = tk.Frame(parent, bg=color.style["bg_main"])
        self.right_column.place(relx=0.65, rely=0, relwidth=0.35, relheight=1.0)

        self._setup_add_event_form()
        self._setup_system_logs()
        
    # tworzenie nawigacji (przyciski z nazwa miesiaca)    
    def _setup_navigation(self):
        nav_frame = tk.Frame(self.left_card, bg=color.style["bg_card"], pady=20)
        nav_frame.pack(fill='x')
        
        nav_inner = tk.Frame(nav_frame, bg=color.style["bg_card"])
        nav_inner.pack(expand=True)

        # przycisk poprzedniego miesiaca
        tk.Button(nav_inner, text="<", command=self.prev_month, font=self.f_header, 
                  bg=color.style["bg_card"], fg=color.style["text_muted"], relief='flat', 
                  activebackground=color.style["bg_card"], activeforeground=color.style["btn_primary"], 
                  cursor="hand2").pack(side='left', padx=20)
        
        # miesiac i rok 
        self.month_label = tk.Label(nav_inner, text="", font=self.f_header, bg=color.style["bg_card"], fg=color.style["text_main"])
        self.month_label.pack(side='left', padx=10)
        
        # przycisk nastepnego miesiaca
        tk.Button(nav_inner, text=">", command=self.next_month, font=self.f_header,
                  bg=color.style["bg_card"], fg=color.style["text_muted"], relief='flat', 
                  activebackground=color.style["bg_card"], activeforeground=color.style["btn_primary"], 
                  cursor="hand2").pack(side='left', padx=20)

    # inicjalizacja pustego kontenera dla kalendarza
    def _setup_calendar_area(self):
        self.calendar_container = tk.Frame(self.left_card, bg=color.style["bg_card"], padx=20)
        self.calendar_container.pack(fill='both', expand=True)

        self.calendar_frame = tk.Frame(self.calendar_container, bg=color.style["bg_card"])
        self.calendar_frame.pack(fill='both', expand=True)
    
    # tworzenie listy z wydarzeniami pod kalendarzem 
    def _setup_event_list(self):
        # linia oddzielajaca kalendarz z lista
        tk.Frame(self.left_card, bg=color.style["highlight"], height=1).pack(fill='x', padx=20, pady=10)
        
        list_header = tk.Label(self.left_card, text="NADCHODZĄCE WYDARZENIA", font=self.f_sub, 
                               bg=color.style["bg_card"], fg=color.style["text_muted"], anchor='w')
        list_header.pack(fill='x', padx=25, pady=(5, 0))

        # kontener do wydarzen
        list_container = tk.Frame(self.left_card, bg=color.style["bg_card"], padx=20, pady=10)
        list_container.pack(fill='both', expand=True)

        inner_border_box = tk.Frame(list_container, bg=color.style["bg_card"], 
                                    highlightbackground=color.style["border"], highlightthickness=1)
        inner_border_box.pack(fill='both', expand=True)

        # pasek przewijania
        scrollbar = tk.Scrollbar(inner_border_box, orient="vertical")
        scrollbar.pack(side='right', fill='y')

        # wyswietlenie wydarzen
        self.event_list_text = tk.Text(inner_border_box, state='disabled', bg=color.style["bg_card"], 
                                       fg=color.style["text_main"], font=self.f_norm, borderwidth=0, 
                                       highlightthickness=0, yscrollcommand=scrollbar.set)
        self.event_list_text.pack(side='left', fill='both', expand=True)
        scrollbar.config(command=self.event_list_text.yview)
        
        # tagi uzyte do styliazcji roznych linii tekstu            
        self.event_list_text.tag_configure("date_header", font=self.f_sub, 
                                           foreground=color.style["btn_primary"], spacing1=10, spacing3=5)
        self.event_list_text.tag_configure("event_item", lmargin1=15, lmargin2=15)

    # budowa panelu dodania wydarzenia
    def _setup_add_event_form(self):
        self.add_card = tk.Frame(self.right_column, bg=color.style["bg_card"], padx=20, pady=20,
                                 highlightbackground=color.style["border"], highlightthickness=1)
        self.add_card.pack(fill='x', side='top', pady=(0, 20))

        tk.Label(self.add_card, text="NOWE WYDARZENIE", font=self.f_sub, 
                 bg=color.style["bg_card"], fg=color.style["text_muted"]).pack(anchor='w', pady=(0, 15))

        # etykieta wyswietlajaca date wybrana z kalendarza poprzez klikniecie na konkretny termin
        self.lbl_selected_date = tk.Label(self.add_card, text="Wybierz datę z kalendarza", 
                                          font=self.f_norm, fg=color.style["text_main"], bg=color.style["bg_card"])
        self.lbl_selected_date.pack(anchor='w', pady=(0, 10))

        # pola wprowadzania danych do nowego wydarzenia
        self.time_entry = self._create_styled_input(self.add_card, "GODZINA (HH:MM)")
        self.time_entry.insert(0, "12:00")
        self.title_entry = self._create_styled_input(self.add_card, "NAZWA WYDARZENIA")

        # przycisk wysylajacy komunikat z prosba o dodanie wydarzenia do serwera 
        self.btn_add = tk.Button(self.add_card, text="DODAJ", command=self.handle_add_event, 
                                 bg=color.style["btn_primary"], fg="white", disabledforeground="white",
                                 font=("Arial", 10, "bold"), relief='flat', state='disabled', cursor="hand2", 
                                 activebackground=color.style["highlight"], activeforeground=color.style["text_main"])
        self.btn_add.pack(fill='x', pady=10, ipady=5)

    # panel logow 
    def _setup_system_logs(self):
        self.log_card = tk.Frame(self.right_column, bg=color.style["bg_card"], padx=20, pady=20,
                                 highlightbackground=color.style["border"], highlightthickness=1)
        self.log_card.pack(fill='both', expand=True)

        tk.Label(self.log_card, text="LOGI SYSTEMOWE", font=self.f_sub, 
                 bg=color.style["bg_card"], fg=color.style["text_muted"]).pack(anchor='w', pady=(0, 10))

        # obszar przeznaczony na logi
        self.log_area = tk.Text(self.log_card, state='disabled', bg="#F7F8FA", fg=color.style["text_muted"], 
                                font=self.f_small, padx=10, pady=10, relief="flat", 
                                highlightbackground=color.style["border"], highlightthickness=1)
        self.log_area.pack(fill='both', expand=True)

    # funkcja tworzaca wystylizowana etykiete dla dodania wydarzenia
    def _create_styled_input(self, parent, label_text):
        tk.Label(parent, text=label_text, font=("Arial", 8, "bold"), 
                 bg=color.style["bg_card"], fg=color.style["text_muted"]).pack(anchor='w', pady=(5, 0))
        entry = tk.Entry(parent, bg="#FFFFFF", fg=color.style["text_main"], 
                         relief='flat', font=self.f_norm,
                         highlightbackground=color.style["border"], highlightthickness=1)
        entry.pack(fill='x', pady=(5, 15), ipady=5)
        return entry

    # funkcja rysujaca kalendarz w kontenerze
    def draw_calendar(self, year, month, events, selected_date):
        # czyszczenie kontenera
        for widget in self.calendar_frame.winfo_children():
            widget.destroy()

        # ustawienie nakladki miesiaca i roku
        self.month_label.config(text=f"{calendar.month_name[month]} {year}")

        # iterowanie po dniach tygodnia
        for i in range(7): 
            self.calendar_frame.grid_columnconfigure(i, weight=1)

        # naglowki dni tygodnia
        days = ["MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"]
        for i, day in enumerate(days):
            tk.Label(self.calendar_frame, text=day, font=("Arial", 8, "bold"), 
                     bg=color.style["bg_card"], fg=color.style["text_muted"]).grid(row=0, column=i, sticky="nsew", pady=(0, 10))

        # pobieranie dni miesiaca przy uzyciu biblioteki calendar
        cal = calendar.monthcalendar(year, month)
        for r_idx in range(len(cal)):
            self.calendar_frame.grid_rowconfigure(r_idx + 1, weight=1)

        # iteracja w celu narysowania poszczegolnych dni
        for r, week in enumerate(cal):
            for c, day in enumerate(week):
                if day == 0:
                    tk.Label(self.calendar_frame, bg=color.style["bg_card"]).grid(row=r+1, column=c, sticky="nsew")
                    continue
        
                d_str = f"{year}-{month:02d}-{day:02d}"
                is_selected = (selected_date == d_str)

                # ustawienie kolorystyki                
                bg_color = color.style["selection"] if is_selected else color.style["bg_card"]
                hl_color = color.style["selection_border"] if is_selected else color.style["bg_card"]
                hl_thick = 1 if is_selected else 0

                # kontener dla pojedynczego dnia
                cell = tk.Frame(self.calendar_frame, bg=bg_color, cursor="hand2",
                                highlightbackground=hl_color, highlightthickness=hl_thick)
                cell.grid(row=r+1, column=c, sticky="nsew", padx=2, pady=2)

                # numer dnia 
                lbl_color = "#1877F2" if is_selected else color.style["text_main"]
                lbl = tk.Label(cell, text=str(day), font=("Arial", 12), bg=bg_color, fg=lbl_color)
                lbl.pack(pady=(5, 0))
                
                # rysowanie kropek oznaczajacej ilosc wydarzen wystepujacych w dniu
                dots_frame = tk.Frame(cell, bg=bg_color)
                dots_frame.pack(pady=(2, 5))
                
                # rysowanie max 3 kropek (dla widocznosci)     
                count = sum(1 for e in events if e.get('date') == d_str)
                for _ in range(min(count, 3)): 
                    tk.Frame(dots_frame, bg=color.style["dot_color"], width=6, height=6).pack(side='left', padx=1)

                for w in [cell, lbl, dots_frame]: 
                    w.bind("<Button-1>", lambda e, d=d_str: self.select_day(d))
    
    # aktualizacja widoku wydarzen
    def update_event_list_view(self, events):
        self.event_list_text.config(padx=10, state='normal')
        self.event_list_text.delete('1.0', tk.END)
        self.event_list_text.tag_configure("start", spacing1=8)
        
        if not events:
            self.event_list_text.insert(tk.END, "Brak planów na ten miesiąc.", "start")
        else:
            # sortowanie chronologicznie wydarzen oraz wstawianie ich do kontenera
            sorted_ev = sorted(events, key=lambda x: (x.get('date'), x.get('time')))
            curr = ""
            for ev in sorted_ev:
                if ev.get('date') != curr:                     
                    self.event_list_text.insert(tk.END, f"{ev.get('date')}\n", "date_header")
                    curr = ev.get('date')    
                self.event_list_text.insert(tk.END, f"{ev.get('time')}  |  {ev.get('title')}\n", "event_item")
        
        self.event_list_text.config(state='disabled')

    # aktualizacja logow
    def update_log(self, message):
        self.log_area.config(state='normal')
        ts = datetime.now().strftime("%H:%M:%S")
        self.log_area.insert(tk.END, f"[{ts}] {message}\n")
        self.log_area.see(tk.END)
        self.log_area.config(state='disabled')