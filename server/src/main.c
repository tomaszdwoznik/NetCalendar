#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/h/cJSON.h"
#include "../include/h/calendar.h"
#include "../include/h/common_utils.h"
#include "../include/h/network_utils.h"
#include "../include/h/ssl_methods.h"

int main(int argc, char* argv[]) {
  // wczytanie bazy z calendar.json oraz inicjaizacja kontekstu SSL 
  cJSON* db = load_database();
  SSL_CTX* ctx =
      init_ssl_context("server/resources/server.crt", "server/resources/server.key");
  if (ctx == NULL) {
    fprintf(stderr, "Nie udało się zainicjować SSL\n");
    return -1;
  }

  struct sockaddr_in saddr;
  int sfd = -1;
  int fdmax = 0;
  fd_set main_rmask, main_wmask;

  // konfiguracja adresow i utworzenie gniazda
  if (setup_addr(&saddr) == 0) {
    sfd = make_listen_socket(&saddr);
    if (sfd != -1) {
      // ustawienia do funkcji select()
      init_selector(sfd, &main_rmask, &main_wmask, &fdmax);
      printf("Serwer uruchomiony na porcie %d\n", ntohs(saddr.sin_port));
    } else {
      return -1;
    }
  }

  // glowna petla serwera
  while (1) {
    fd_set rmask = main_rmask;
    fd_set wmask = main_wmask;

    // oczekiwanie na klientow
    int rc = select(fdmax + 1, &rmask, &wmask, NULL, NULL);
    if (rc < 0) {
      perror("Błąd select");
      break;
    }

    // obsluga nowych polaczen przychodzacych
    if (FD_ISSET(sfd, &rmask)) {
      struct sockaddr_in caddr;
      socklen_t slt = sizeof(caddr);
      int cfd = accept(sfd, (struct sockaddr*)&caddr, &slt);

      // tworzenie nowej struktury dla polaczenia z klientem
      if (cfd > 0) {
        states[cfd].ssl = SSL_new(ctx);
        SSL_set_fd(states[cfd].ssl, cfd);

        // 'handshake' SSL
        if (SSL_accept(states[cfd].ssl) <= 0) {
          ERR_print_errors_fp(stderr);
          close_ssl_connection(cfd, &main_rmask);
        } else {
          // ustawienie gniazda w trybie nieblokujacym oraz ustawienie maski gotowej do odczytu
          setNonBlock(cfd);
          FD_SET(cfd, &main_rmask);
          if (cfd > fdmax) {
            fdmax = cfd;
          }
          states[cfd].state = 1;
          printf("Nowe połączenie SSL na FD: %d\n", cfd);
        }
      } else {
        perror("Błąd accept");
      }
    }

    // sprawdzanie po deskryptorach, czy sa dane do odczytu
    for (int i = 0; i <= fdmax; i++) {
      if (i != sfd && FD_ISSET(i, &rmask)) {
        char buf[1024];
        int bytes = ssl_buf_read(states[i].ssl, buf, sizeof(buf) - 1);

        if (bytes == -2) continue;
        
        // gdy polaczenie z klientem zostaje utracone zamykamy polaczenie SSL
        if (bytes <= 0) {
          printf("Klient %d rozłączony.\n", i);
          close_ssl_connection(i, &main_rmask);
        } else {
          // weryfikacja danych od klienta (poprawny json oraz jego struktura)
          buf[bytes] = '\0';
          cJSON* json = cJSON_Parse(buf);
          if (json == NULL) {
            printf("Niepoprawny format JSON od klienta %d\n", i);
            continue;
          }

          cJSON* action = cJSON_GetObjectItemCaseSensitive(json, "action");
          if (!action || !cJSON_IsString(action)) {
            char* err = "Brak akcji w żądaniu";
            SSL_write(states[i].ssl, err, strlen(err));
            cJSON_Delete(json);
            continue;
          }

          // obsluga pobrania wydarzen 
          if (strcmp(action->valuestring, "get_events") == 0) {
            cJSON* date_param = cJSON_GetObjectItemCaseSensitive(json, "date");
            if (date_param && is_valid_date(date_param->valuestring)) {
              cJSON* events = get_events_by_date(date_param->valuestring, db);
              char* out = cJSON_PrintUnformatted(events);
              SSL_write(states[i].ssl, out, strlen(out));
              free(out);
              cJSON_Delete(events);
            } else {
              char* error_msg = "Niepoprawna data";
              SSL_write(states[i].ssl, error_msg, strlen(error_msg));
            }
          } else if (strcmp(action->valuestring, "add_event") == 0) {
            // obsluga dodania wydarzenia 
            cJSON* date_p = cJSON_GetObjectItemCaseSensitive(json, "date");
            cJSON* time_p = cJSON_GetObjectItemCaseSensitive(json, "time");
            cJSON* title_p = cJSON_GetObjectItemCaseSensitive(json, "title");

            // walidacja danych podanych przez klienta
            if (date_p && cJSON_IsString(date_p) && time_p &&
                cJSON_IsString(time_p)) {
              if (!is_valid_date(date_p->valuestring)) {
                char* msg = "Niepoprawna data. (format [YYYY-MM-HH])\n";
                SSL_write(states[i].ssl, msg, strlen(msg));
              } else if (!is_valid_time(time_p->valuestring)) {
                char* msg = "Niepoprawna godzina. (format [HH:MM])\n";
                SSL_write(states[i].ssl, msg, strlen(msg));
              } else if (is_event_duplicate(db, date_p->valuestring,
                                            time_p->valuestring)) {
                // weryfikacja dostepnosci terminu oraz dodanie wydarzenia
                char* msg = "Ten termin jest już zajęty.\n";
                SSL_write(states[i].ssl, msg, strlen(msg));
              } else {
                add_event_to_db(date_p->valuestring, time_p->valuestring,
                                title_p->valuestring, db);
                char* success_msg = "Dodano wydarzenie.";
                SSL_write(states[i].ssl, success_msg, strlen(success_msg));

                // broadcast informujacy klientow, ze dodano nowe wydarzenie
                char month_buffer[8];
                strncpy(month_buffer, date_p->valuestring, 7);
                month_buffer[7] = '\0';

                cJSON* events = get_events_by_date(month_buffer, db);
                char* json_output = cJSON_PrintUnformatted(events);

                for (int j = 0; j <= fdmax; j++) {
                  if (j != sfd && FD_ISSET(j, &main_rmask)) {
                    if (states[j].ssl != NULL) {
                      SSL_write(states[j].ssl, json_output, strlen(json_output));
                    }
                  }
                }
                free(json_output);
                cJSON_Delete(events);
              }
            }
          } else {
            char* error_msg = "Niepoprawna akcja ['get_events', 'add_event']";
            SSL_write(states[i].ssl, error_msg, strlen(error_msg));
          }
          cJSON_Delete(json);
        }
      }
    }
  }

  // zwolnienie zasobow przed zamknieciem programu
  cJSON_Delete(db);
  SSL_CTX_free(ctx);
  return 0;
}