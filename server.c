#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <strings.h>
#include <getopt.h>
#include <unistd.h>

/* TODO:
 Check for root, getuid()
 Display pid
 Attach BPF filter
 Variable length message buffer
 Send file contents */

static const char *short_options = "hb:n:p:";
static const struct option long_options[] = {
        {"help",                no_argument,		NULL, 'h'},
        {"port",                required_argument,      NULL, 'p'},
        {"bind",                required_argument,      NULL, 'b'},
        {"num",                 required_argument,      NULL, 'n'},
};

usage(void)
{
	puts("--Simple TCP Server--\n\n"
	"        To play with socket programming\n\n"
	"Options:\n\n"
	" -h|--help		Display help\n"
	" -p|--port		Port to listen on\n"
	" -n|--num		Listen for number of connections\n"
	" -b|--bind		Address to listen on (def: all)\n");
}

int main(int argc, char **argv)
{
	int sock, cli, server_addr, port, len, 
	    sent, opt, opt_index, bind_all_ip = 1, num = 1;
	struct sockaddr_in server, client;
	char mesg[]= "Test";

	while ((opt = getopt_long(argc, argv, short_options,
				long_options, &opt_index)) != EOF) {

		switch (opt) {
		case 'h':
			usage();
			exit(0);
		case 'b':
			bind_all_ip = 0;
			server_addr = optarg;
			break;
		case 'n':
			num = atoi(optarg);
			break;
		case 'p':
			port = atoi(optarg);
			break;
		default:
			usage();
			exit(1);
		}
	}

	if (argc <= 1)
	{
		printf("No options given, try ``--help''\n", argc);
		exit(1);
	}

	printf("PID: %d", (int) getpid());

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("ERROR: creating socket");
		exit(-1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	//inet_pton(AF_INET, server_addr, &(server.sin_addr.s_addr));
	server.sin_addr.s_addr = htons(server_addr);
	//server.sin_addr.s_addr = inet_addr(server_addr);
	bzero(&server.sin_zero, 8);

	if (bind_all_ip == 1)
		server.sin_addr.s_addr = INADDR_ANY;
	else
		server.sin_addr.s_addr = htons(server_addr);

	len = sizeof(struct sockaddr_in);

	if((bind(sock, (struct sockaddr *)&server, len)) == -1)
	{
		perror("ERROR: binding to socket");
		exit(-1);
	}

	/*if((listen(sock, 5)) == -1)
	{
		perror("ERROR: listening on socket");
		exit(-1);
	}*/

	listen(sock, num);

	while(num > 0)
	{
		num--;

		if((cli = accept(sock, (struct sockaddr *)&client, &len)) == -1)
		{
			perror("ERROR: accepting on socket");
			exit(-1);
		}

		if((sent = send(cli, mesg, strlen(mesg), 0)) == -1)
		{
			perror("ERROR: sending on socket");
		}

		printf("Sent %d bytes to client %s",
			sent, inet_ntoa(client.sin_addr));

		close(cli);
	}

}
