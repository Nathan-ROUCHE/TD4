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
    int fd_in;  // descripteur de fichier du fichier ouvert en lecture
    int fd_out; // descripteur de fichier du fichier ouvert en écriture
    int nbread;
    unsigned char *buffer = malloc(4096 * sizeof(unsigned char));    // buffer de lecture

    fd_in = open(argv[1], O_RDONLY);
    if (fd_in < 0) {
      perror(argv[1]);
      exit(1);
    }

    fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out < 0) {
      perror(argv[2]);
      exit(1);
    }

    for(int i = 0; i < 3; i++) {
      nbread = lireligne(fd_in, buffer, 4096);
      write(fd_out, buffer, nbread);
      if (buffer[0] == '#') {
        i -= 1;
      }
    }

    // Fin de la lecture de l'en-tête
    int delta = atoi(argv[3]);
    nbread = 1;
    while (nbread != 0) {
      nbread = read(fd_in, buffer, 4096);
      for (int i = 0; i < nbread; i++) {
        if (buffer[i] + delta > 255) {
          buffer[i] = 255;
        } else if (buffer[i] + delta < 0) {
          buffer[i] = 0;
        } else {
          buffer[i] = buffer[i] + delta;
        }
      }
      write(fd_out, buffer, nbread);
    }


    return 0;
}