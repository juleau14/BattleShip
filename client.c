#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define N 10


void afficher(int grid[N][N]) {
  for (int i = 0; i < N; i++) {
    printf("  1 2 3 4 5 6 7 8 9 10 \n");
  }

  for (int i = 0; i < N; i++) {
    printf("%d |", (i+1));
    for (int j = 0; j < N; j++) {
      printf("%d|", grid[i][j]);
    }
    printf("\n");
  }
}


int jeu(int socket_desc) {

  int waiting_room;

  char response[10];      // response = ['type requete', ' ', 'valeur requete']
  char response_type_c[10];
  char response_value_c[10];

  int response_type;
  int response_value;


  int return_recv = recv(socket_desc, response, sizeof(response), 0);

  if (return_recv == -1) {
    perror("Error reception.\n");
    close(socket_desc);
    return EXIT_FAILURE;
  }

  else {
    printf("Receive response OK...\n");
  }

  response_type_c[0] = response[0];
  response_type = atoi(response_type_c);
  response_value_c[0] = response[2];
  response_value = atoi(response_value_c);

  if (response_type == 3) {
    waiting_room = response_value;
  }

  else {
    perror("Excpected to receive waiting room indication.\n");
    close(socket);
    return EXIT_FAILURE;
  }

  while (waiting_room) {
    printf("Vous êtes connecté. En attente d'un autre joueur...\n");
    recv(socket_desc, response, sizeof(response), 0);
    response_value_c[0] = response[2];
    waiting_room = atoi(response_value_c);
  }

  printf("La partie commence !\n");

  /* INITIALISION OF THE GRIDS */
  int my_grid[N][N];
  int ennemy_grid[N][N];

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      my_grid[i][j] = 0;
      ennemy_grid[i][j] = 0;
    }
  }

  int line_ship;
  int column_ship;

  printf("Veuillez entrer la ligne de votre bateau :\n");
  scanf("%d\n", &line_ship);
  printf("Veuillez entrer la colonne de votre bateau :\n");
  scanf("%d\n", &column_ship);

  char ship_position_request[10];
  sprintf(ship_position_request, "%d %d", line_ship, column_ship);

  int return_send = send(socket_desc, ship_position_request, sizeof(ship_position_request), 0);

  if (return_send == -1) {
    perror("Error sending position ship request.\n");
    close(socket_desc);
    return EXIT_FAILURE;
  }

  else {
    printf("Send informations to the server OK...\n");
  }

  int res = 0;
  int turn;

  char request_shot[10];

  int line_to_hit;
  int column_to_hit;

  while (res != 2) {
    printf("Waiting for opponent or server.\n");
    int return_recv = recv(socket_desc, response, sizeof(response), 0);

    if (return_recv == -1) {
      perror("Reception error.\n");
      close(socket_desc);
      return EXIT_FAILURE;
    }

    else {
      printf("Response received succesfuly.\n");
    }

    response_type_c[0] = response[0];
    response_value_c[0] = response[2];

    response_type = atoi(response_type_c);
    response_value = atoi(response_value_c);

    if (response_type == 0) {
      turn = response_value;
    }

    else {
      perror("Bad request type (expected turn indication).\n");
      close(socket_desc);
      return EXIT_FAILURE;
    }

    if (turn == 2) {
      printf("Le joueur ennemi a trouvé votre bateau, vous avez perdu.\n");
      res = 2;
      close(socket_desc);
      return EXIT_SUCCESS;
    }

    else {
      printf("C'est votre tour.\n");
      printf("--- GRILLE ENNEMIE ---\n\n");
      afficher(ennemy_grid);
      printf("\n\n");
      printf("Veuillez entrer la ligne de la case à frapper :\n");
      scanf("%d\n", &line_to_hit);
      printf("Veuillez entrer la colonne de la case à frapper :\n");
      scanf("%d\n", &column_to_hit);

      sprintf(request_shot, "%d %d", line_to_hit, column_to_hit);

      int return_send = send(socket_desc, request_shot, sizeof(request_shot), 0);

      if (return_send == -1) {
        perror("Sending error.\n");
        close(socket_desc);
        return EXIT_FAILURE;
      }

      else {
        printf("Request sending OK...\n");
      }

      return_recv = recv(socket_desc, response, sizeof(response), 0);

      if (return_recv == -1) {
        perror("Reception error.\n");
        close(socket_desc);
        return EXIT_FAILURE;
      }

      else {
        printf("Response received succesfuly.\n");
      }

      response_type_c[0] = response[0];
      response_value_c[0] = response[2];

      response_type = atoi(response_type_c);
      response_value = atoi(response_value_c);

      if (response_type == 1) {
        res = response_value;
      }

      else {
        perror("Bad request type (expected result).\n");
        close(socket_desc);
        return EXIT_FAILURE;
      }

      if (res == 1) {
        printf("Raté pour cette fois !\n");
        ennemy_grid[line_to_hit][column_to_hit] = 1;
      }

      else if(res == 2) {
        printf("Bravo, vous avez gagné !");
        close(socket_desc);
        return EXIT_SUCCESS;
      }

    } // end else (le joueur ennemi n'a pas trouvé)

  } // end while (personne n'a gagné, la partie continue)
} // end jeu


int main() {

  int socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc == -1) {
    perror("Error socket creation\n");
    return EXIT_FAILURE;
  }

  else {
    printf("Socket creation OK...\n");
  }

  u_short port = 5555;
  char ip[20] = "212.71.239.103";

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  inet_pton(AF_INET, ip, &(server_addr.sin_addr));    // inet python converti le string ip en bytes et l'injecte dans la struct du serveur

  int return_connect = connect(socket_desc, (struct sockaddr *) &server_addr, sizeof(server_addr));

  if (return_connect < 0) {
    perror("Error connection socket.\n");
    return EXIT_FAILURE;
  }

  else {
    printf("Socket connection... OK\n");

    jeu(socket_desc);
  }


}  // end main
