#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int jeu(int socket_player_1, int socket_player_2) {
  char response[10];
  sprintf(response, "%d %d", 3, 0);
  send(socket_player_1, response, sizeof(response), 0);
  send(socket_player_2, response, sizeof(response), 0);

  char request[10];
  char line_ship[10];
  char column_ship[10];

  recv(socket_player_1, request, sizeof(request), 0);
  line_ship[0] = request[0];
  column_ship[0] = request[2];

  int line_ship_player_1 = atoi(line_ship);
  int column_ship_player_1 = atoi(column_ship);

  recv(socket_player_2, request, sizeof(request), 0);
  line_ship[0] = request[0];
  column_ship[0] = request[2];

  int line_ship_player_2 = atoi(line_ship);
  int column_ship_player_2 = atoi(column_ship);

  int res = 0;

  while (res != 2) {
    sprintf(response, "%d %d", 0, 1);

    send(socket_player_1, response, sizeof(response), 0); // tu peux jouer

    recv(socket_player_1, request, sizeof(request), 0); // recu du tour

    line_ship[0] = request[0];
    column_ship[0] = request[2];
    int line_to_hit = atoi(line_ship);
    int column_to_hit = atoi(column_ship);

    if (line_to_hit == line_ship_player_2 && column_to_hit == column_ship_player_2) {
      sprintf(response, "%d %d", 1, 2);
      send(socket_player_1, response, sizeof(response), 0);
      sprintf(response, "%d %d", 0, 2);
      send(socket_player_1, response, sizeof(response), 0);

      return 0;
    }

    else {
      sprintf(response, "%d %d", 1, 1);
      send(socket_player_1, response, sizeof(response), 0);
      sprintf(response, "%d %d", 0, 1);
      send(socket_player_2, response, sizeof(response), 0);
    }

    sprintf(response, "%d %d", 0, 1);

    send(socket_player_2, response, sizeof(response), 0); // tu peux jouer

    recv(socket_player_2, request, sizeof(request), 0); // recu du tour

    line_ship[0] = request[0];
    column_ship[0] = request[2];
    line_to_hit = atoi(line_ship);
    column_to_hit = atoi(column_ship);

    if (line_to_hit == line_ship_player_1 && column_to_hit == column_ship_player_1) {
      sprintf(response, "%d %d", 0, 2);
      send(socket_player_1, response, sizeof(response), 0);
      sprintf(response, "%d %d", 1, 2);
      send(socket_player_2, response, sizeof(response), 0);

      return 0;
    }

    else {
      sprintf(response, "%d %d", 0, 1);
      send(socket_player_1, response, sizeof(response), 0);
      sprintf(response, "%d %d", 1, 1);
      send(socket_player_2, response, sizeof(response), 0);
    }
  }
}

int main() {
  int main_socket = socket(AF_INET, SOCK_STREAM, 0);

  if (main_socket < 0) {
    perror("Error socket creation.");
    return EXIT_FAILURE;
  }

  else {
    printf("Socket creation OK...\n");
  }

  struct sockaddr_in local_addr;

  u_short port = 5555;

  memset(&local_addr, 0, sizeof(local_addr));
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(port);
  local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int return_bind = bind(main_socket, (struct sockaddr *) &local_addr, sizeof(local_addr));

  if (return_bind == -1) {
    perror("Error binding");
    return EXIT_FAILURE;
  }

  else {
    printf("Binding OK...\n");
  }

  int return_listen = (main_socket, 15);

  if (return_listen == -1) {
    perror("Listening error.\n");
    return EXIT_FAILURE;
  }

  else {
    printf("Listening OK...\n");
  }

  int socket_player_1;
  int socket_player_2;

  struct sockaddr_in client_addr_1;
  struct sockaddr_in client_addr_2;

  char response[10];
  socklen_t client_1_len = sizeof(client_addr_1);
  socklen_t client_2_len = sizeof(client_addr_2);

  while (1) {
    socket_player_1 = accept(main_socket, (struct sockaddr *)&client_addr_1, &client_1_len);

    if (socket_player_1 < 0) {
      perror("Error accepting connection\n.");
      close(main_socket);
      return EXIT_FAILURE;
    }

    else {
      printf("Accepting connextion OK...\n");
    }

    sprintf(response, "%d %d", 3, 1);

    send(socket_player_1, response, sizeof(response), 0);

    socket_player_2 = accept(main_socket, (struct sockaddr *)&client_addr_2, &client_2_len);

    switch(fork()) {
      case -1:
        perror("Error creation child process.\n");
        close(socket_player_1);
        close(socket_player_2);
        close(main_socket);
        exit(1);

      case 0:

    }
  }
}
