#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define FONT_LOCATION 0x50
#define RAM_SIZE 4096
#define PGM_START 0x200
#define PGM_MAX RAM_SIZE-PGM_START
#define STACK_SIZE 16

typedef struct op
{
  uint8_t n1:4;
  uint8_t n2:4;
  uint8_t n3:4;
  uint8_t n4:4;
} op_t;

uint8_t mem[RAM_SIZE];		// main memory
uint8_t display[64][32];
uint8_t dt;			// delay timer
uint8_t st;			// sound timer
uint8_t sp;			// stack pointer

uint16_t v[16];			// variable register
uint16_t pc;			// program counter
uint16_t i;			// index register
uint16_t stack[STACK_SIZE];

uint8_t font[] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0,	// 0
  0x20, 0x60, 0x20, 0x20, 0x70,	// 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0,	// 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0,	// 3
  0x90, 0x90, 0xF0, 0x10, 0x10,	// 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0,	// 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0,	// 6
  0xF0, 0x10, 0x20, 0x40, 0x40,	// 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0,	// 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0,	// 9
  0xF0, 0x90, 0xF0, 0x90, 0x90,	// A
  0xE0, 0x90, 0xE0, 0x90, 0xE0,	// B
  0xF0, 0x80, 0x80, 0x80, 0xF0,	// C
  0xE0, 0x90, 0x90, 0x90, 0xE0,	// D
  0xF0, 0x80, 0xF0, 0x80, 0xF0,	// E
  0xF0, 0x80, 0xF0, 0x80, 0x80	// F
};

void load_program ();
void loop_program ();

int
main ()
{

  memcpy (&mem[FONT_LOCATION], font, sizeof (font));
  sp = 0;
  pc = PGM_START;

  load_program ();
  loop_program ();
  return 0;
}

void
load_program ()
{
  int fd;
  int bytes_read;
  char *fname = "chip8.ch8";
  struct stat sb;

  if (stat (fname, &sb) == -1)
    {
      perror (fname);
      exit (errno);
    }
  if (sb.st_size > PGM_MAX)
    {
      errno = EFBIG;
      perror (fname);
      exit (errno);
    }
  fd = open (fname, O_RDONLY);
  if (fd == -1)
    {
      perror (fname);
      exit (errno);
    }
  if (read (fd, &mem[PGM_START], sb.st_size) == -1)
    {
      perror (fname);
      exit (errno);
    }
  printf ("%s loaded\n", fname);
  close (fd);
}

void
inst_0 (op_t op)
{
  switch (op.n1)
    {
    case 0x00e0:
      printf ("CLS\n");
      break;
    case 0x00ee:
      printf ("RET\n");
      break;
    default:
      printf ("SYS %3x\n", op.n1);
    }
}

void
loop_program ()
{
  op_t *op;
  while (1)
    {
      op = (op_t *) (mem + pc);
      pc += 2;
      switch (op->n1)
	{
	case 0x0:
	  inst_0 (*op);
	default:
	  printf ("%4x\n", op->n1);
	}
      sleep (1);
    }
}
