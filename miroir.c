#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int lireligne(int fd, char *buffer, int size)
{
  ssize_t nbread = read(fd, buffer, size);
  if (nbread == -1)
  {
    return -1;
  }

  int i;
  for (i = 0; i < nbread; i++)
  {
    if (buffer[i] == '\n')
    {
      i++;
      break;
    }
  }
  lseek(fd, i - nbread, SEEK_CUR);
  return i;
}

void intensite(int fd_in, int fd_out, int delta)
{
  int nbread;
  unsigned char *buffer = malloc(4096 * sizeof(unsigned char)); // buffer de lecture

  for (int i = 0; i < 3; i++)
  {
    nbread = lireligne(fd_in, buffer, 4096);
    write(fd_out, buffer, nbread);
    if (buffer[0] == '#')
    {
      i -= 1;
    }
  }

  // Fin de la lecture de l'en-tête
  nbread = 1;
  while (nbread != 0)
  {
    nbread = read(fd_in, buffer, 4096);
    for (int i = 0; i < nbread; i++)
    {
      if (buffer[i] + delta > 255)
      {
        buffer[i] = 255;
      }
      else if (buffer[i] + delta < 0)
      {
        buffer[i] = 0;
      }
      else
      {
        buffer[i] = buffer[i] + delta;
      }
    }
    write(fd_out, buffer, nbread);
  }
}

void miroir_vertical(int fd_in, int fd_out) {
  int nbread;
  int width, height;
  unsigned char *buffer = malloc(4096 * sizeof(unsigned char));    // buffer de lecture

  for(int i = 0; i < 3; i++) {
    nbread = lireligne(fd_in, buffer, 4096);
    write(fd_out, buffer, nbread);
    if (buffer[0] == '#') {
      i -= 1;
    } else if (i == 1) {
      char *p = buffer;
      width = (int) strtol(p, &p, 10);
      height = (int) strtol(p, &p, 10);
    }
  }

  printf("%d %d", width, height);
  // fin de lecture de l'en-tête
  unsigned char *bufferligne = malloc(width * sizeof(unsigned char));
  unsigned char *bufferinverse = malloc(width * sizeof(unsigned char));

  for(int i = 0 ;i < height; i++){
    read(fd_in, bufferligne, width);
 
    for (int j = 0; j < width; j++){
      bufferinverse[j] = bufferligne[width-1-j];
    }
    write(fd_out, bufferinverse, width);
  }
}

void miroir_horizontal(int fd_in, int fd_out) {
  int nbread;
  int width, height;
  int bytesPerPixel = 1;
  unsigned char *buffer = malloc(4096 * sizeof(unsigned char));    // buffer de lecture

  for(int i = 0; i < 3; i++) {
    nbread = lireligne(fd_in, buffer, 4096);
    write(fd_out, buffer, nbread);
    if (buffer[0] == '#') {
      i -= 1;
    } else if (i == 0 && buffer[0] == 'P' && buffer[1] == '6') {
      bytesPerPixel = 3;
    } else if (i == 1) {
      char *p = buffer;
      width = (int) strtol(p, &p, 10);
      height = (int) strtol(p, &p, 10);
    }
  }

  // fin de lecture de l'en-tête
  unsigned char *bufferligne = malloc(width * bytesPerPixel);
  unsigned char *bufferinverse = malloc(width * bytesPerPixel);

  for(int i = 0 ;i < height; i++){
    read(fd_in, bufferligne, width * bytesPerPixel);
 
    for (int j = 0; j < width; j++){
      for (int k = 0; k < bytesPerPixel; k++) {
        bufferinverse[bytesPerPixel * j + k] = bufferligne[width - bytesPerPixel * (j + 1) + k];
      }
    }
    write(fd_out, bufferinverse, width);
  }

}

int main(int argc, char **argv)
{
  int fd_in;  // descripteur de fichier du fichier ouvert en lecture
  int fd_out; // descripteur de fichier du fichier ouvert en écriture

  fd_in = open(argv[1], O_RDONLY);
  if (fd_in < 0)
  {
    perror(argv[1]);
    exit(1);
  }

  fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd_out < 0)
  {
    perror(argv[2]);
    exit(1);
  }

  //int delta = atoi(argv[3]);
  //intensite(fd_in, fd_out, delta);

  //miroir_vertical(fd_in, fd_out);
  miroir_horizontal(fd_in, fd_out);

  return 0;
}