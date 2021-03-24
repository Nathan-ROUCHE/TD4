#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int lireligne(int fd, char *buffer, int size) {
	ssize_t nbread = read(fd, buffer, size);
	if (nbread == -1) {
		return -1;
	}

	int i;
	for (i = 0; i < nbread; i++) {
		if (buffer[i] == '\n') {
			i++;
			break;
		}
	}
	lseek(fd, i - nbread, SEEK_CUR);
	return i;
}


int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Pas assez d'arguments.\n");
    exit(1);
  }


  int fd_in = open(argv[1], O_RDONLY);  // descripteur de fichier du fichier ouvert en lecture
  
  if (fd_in < 0) {
    perror(argv[1]);
    exit(1);
  }


  int fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644); // descripteur de fichier du fichier ouvert en écriture
  
  if (fd_out < 0) {
    perror(argv[2]);
    exit(1);
  }


  int nbread;
  unsigned char *buffer = malloc(4096 * sizeof(unsigned char));    // buffer de lecture

  // Entete
  for(int i = 0; i < 3; i++) {
    nbread = lireligne(fd_in, buffer, 4096);
    write(fd_out, buffer, nbread);
    if (buffer[0] == '#') {
      i -= 1;
    }
  }

  // Couleurs
  nbread = 1;
  while (nbread != 0) {
    nbread = read(fd_in, buffer, 4096);
    for (int i = 0; i < nbread; i++) {
      buffer[i] = 255 - buffer[i];
    }
    write(fd_out, buffer, nbread);
  }

  close(fd_in);
  close(fd_out);

  return 0;
}

/*int main(int argc, char **argv) {
  int fd_out = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd_out < 0) {
    perror(argv[1]);
    exit(1);
  }

  for(int i = 0; i < 1234; i++) {
    write(fd_out, "bonjour", 7);
  }

  close(fd_out);
  return 0;
}*/