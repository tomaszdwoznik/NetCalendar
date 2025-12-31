#include <stdio.h>
#include <stdlib.h>
#include "../include/h/calendar.h"
#include "../include/h/cJSON.h"
#include "../include/h/common_utils.h"
#include "../include/h/network_utils.h"
#include "../include/h/ssl_methods.h"
int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Użycie: %s <port>\n", argv[0]);
		return -1;
	}

	cJSON *db = load_database();
	int sfd, cfd, fdmax, rc, i;
	struct sockaddr_in saddr, caddr;
	socklen_t slt = sizeof(caddr);
	fd_set main_rmask, main_wmask, rmask, wmask;
	SSL_CTX *ctx = init_ssl_context("../resources/server.crt", "../resources/server.key");
	if (ctx == NULL)
	{
		fprintf(stderr, "Nie udało się zainicjować SSL\n");
		return -1;
	}

	if (setup_addr(&saddr, argv[1]) == 0)
	{
		sfd = make_listen_socket(&saddr);
		if (sfd != -1)
		{
			init_selector(sfd, &main_rmask, &main_wmask, &fdmax);
			printf("Serwer uruchomiony na porcie %s\n", argv[1]);
		}
		else
		{
			return -1;
		}
	}

	while (1)
	{
		rmask = main_rmask;
		wmask = main_wmask;

		rc = select(fdmax + 1, &rmask, &wmask, NULL, NULL);

		if (rc < 0)
		{
			perror("Błąd select");
			break;
		}

		if (FD_ISSET(sfd, &rmask))
		{
			cfd = accept(sfd, (struct sockaddr *)&caddr, &slt);
			if (cfd > 0)
			{
				states[cfd].ssl = SSL_new(ctx);
				SSL_set_fd(states[cfd].ssl, cfd);

				if (SSL_accept(states[cfd].ssl) <= 0)
				{
					ERR_print_errors_fp(stderr);
					close_ssl_connection(cfd, &main_rmask);
				}
				else
				{
					setNonBlock(cfd);
					FD_SET(cfd, &main_rmask);
					if (cfd > fdmax)
					{
						fdmax = cfd;
					}
					states[cfd].state = 1;
					printf("Nowe połączenie SSL na FD: %d\n", cfd);
				}
			}
			else
			{
				perror("Błąd accept");
			}
		}

		for (i = 0; i <= fdmax; i++)
		{
			if (i != sfd && FD_ISSET(i, &rmask))
			{
				char buf[1024];
				int bytes = ssl_buf_read(states[i].ssl, buf, sizeof(buf) - 1);
				if (bytes == -2)
				{
					continue;
				}
				if (bytes <= 0)
				{
					printf("Klient %d rozłączony.\n", i);
					close_ssl_connection(i, &main_rmask);
				}
				else
				{
					buf[bytes] = '\0';
					cJSON *json = cJSON_Parse(buf);
					if (json == NULL)
					{
						printf("Niepoprawny format JSON od klienta %d\n", i);
					}
					else
					{
						cJSON *action = cJSON_GetObjectItemCaseSensitive(json, "action");
						cJSON *json = cJSON_Parse(buf);

						if (strcmp(action->valuestring, "get_events") == 0)
						{
							cJSON *date_param = cJSON_GetObjectItemCaseSensitive(json, "date");

							if (!is_valid_date(date_param->valuestring))
							{
								char *error_msg = "Niepoprawna data";
								SSL_write(states[i].ssl, error_msg, strlen(error_msg));
							}
							else
							{
								cJSON *events = get_events_by_date(date_param->valuestring, db);
								char *out = cJSON_PrintUnformatted(events);
								SSL_write(states[i].ssl, out, strlen(out));
								free(out);
							}
						}

						else if (strcmp(action->valuestring, "add_event") == 0)
						{
							cJSON *date_param = cJSON_GetObjectItemCaseSensitive(json, "date");
							cJSON *time_param = cJSON_GetObjectItemCaseSensitive(json, "time");
							cJSON *title_param = cJSON_GetObjectItemCaseSensitive(json, "title");

							if (date_param && cJSON_IsString(date_param) && time_param && cJSON_IsString(time_param))
							{
								if (!is_valid_date(date_param->valuestring))
								{
									char *error_msg = "Niepoprawna data [YYYY-MM-HH]\n";
									SSL_write(states[i].ssl, error_msg, strlen(error_msg));
								}
								else if (!cJSON_IsString(time_param) || !is_valid_time(time_param->valuestring))
								{
									char *error_msg = "Niepoprawna godzina [HH:MM]\n";
									SSL_write(states[i].ssl, error_msg, strlen(error_msg));
								}
								else if (is_event_duplicate(db, date_param->valuestring, time_param->valuestring))
								{
									char *error_msg = "Ten termin jest już zajęty.\n";
									SSL_write(states[i].ssl, error_msg, strlen(error_msg));
								}
								else
								{

									add_event_to_db(date_param->valuestring, time_param->valuestring, title_param->valuestring, db);
								}
							}
						}
						else
						{
							char *error_msg = "Niepoprawna akcja ['get_events', 'add_event']";
							SSL_write(states[i].ssl, error_msg, strlen(error_msg));
						}
					}
				}
			}
		}
	}
	cJSON_Delete(db);
	SSL_CTX_free(ctx);
	return 0;
}