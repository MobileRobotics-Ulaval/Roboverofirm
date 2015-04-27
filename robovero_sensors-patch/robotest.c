#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <poll.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // ignore break signal
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf("error %d setting term attributes", errno);
}

void sendCom(int fd, char *buf)
{
  int ret = write (fd, buf, strlen(buf));           // send carrier return
  usleep(100*1000);
  printf("(%i):%i\t%s",strlen(buf),ret,buf);
}

int main (int argc, char* argv[])
{
    struct termios if_mode;
    char*          robovero_dev;
    char* buf;
    int            serial_if;
    int            error;

    if(argc < 2)
    {
        printf(
               "Syntax is: robocomm [device file]\n"
               "Example: robocomm /dev/ttyACM0\n"
              );

        return -1;
    }


    robovero_dev = argv[1];
    serial_if = open (robovero_dev, O_RDWR | O_NOCTTY | O_SYNC);
    if (serial_if < 0)
    {
	    printf("error %d opening %s: %s", errno, robovero_dev, strerror (errno));
	    return -1;
    }

    set_interface_attribs (serial_if, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking (serial_if, 0);                // set no blocking

    sendCom(serial_if,"\r\n");
    sendCom(serial_if,"promptOff\r\n");
    sendCom(serial_if,"resetConfig\r\n");
    sendCom(serial_if,"roboveroConfig\r\n");
    usleep(5000*1000);
    sendCom(serial_if,"heartbeatOff\r\n");
    usleep(5000*1000);
    sendCom(serial_if,"heartbeatOn\r\n");
    tcflush(serial_if,TCIOFLUSH);
    
    close(serial_if);
    return 0;
}
