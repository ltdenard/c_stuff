#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

/*
openssl req -newkey rsa:2048 -nodes -keyout key.pem -x509 -days 365 -out cert.pem
gcc -lssl -lcrypto -o seerver seerver.c
gcc seerver.c -o seerver -I/usr/local/ssl/include -L/usr/local/ssl/lib -lssl -lcrypto
*/
int create_socket(int port)
{
    int s;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
	perror("Unable to create socket");
	exit(EXIT_FAILURE);
    }

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
	perror("Unable to bind");
	exit(EXIT_FAILURE);
    }

    if (listen(s, 1) < 0) {
	perror("Unable to listen");
	exit(EXIT_FAILURE);
    }

    return s;
}

void init_openssl()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl()
{
    EVP_cleanup();
}

SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
	perror("Unable to create SSL context");
	ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx)
{
    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{
    int sock;
    SSL_CTX *ctx;

    init_openssl();
    ctx = create_context();

    configure_context(ctx);

    sock = create_socket(4433);

    /* Handle connections */
    while(1) {
        struct sockaddr_in addr;
        uint len = sizeof(addr);
        SSL *ssl;
        char charBuffer[1024] = "";

        int client = accept(sock, (struct sockaddr*)&addr, &len);
        if (client < 0) {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);

        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
        }
        else {
            SSL_read(ssl, (char *)charBuffer, 1024);
            if (charBuffer > 0)
            {
                /* printf("%d\n", strlen(charBuffer)); */
                printf("%s\n", charBuffer);
                time_t t = time(NULL);
                struct tm *tm = localtime(&t);
                char s[64];
                strftime(s, sizeof(s), "%a, %d %b %Y %H:%M:%S %Z", tm);
                char reply[] = "HTTP/2 200\n";
                strcat(reply, "date: ");
                strcat(reply, s);
                strcat(reply, "\n");
                strcat(reply, "server: nginx\n");
                strcat(reply, "content-type: text/html; charset=utf-8\n");
                strcat(reply, "x-frame-options: SAMEORIGIN\n");
                strcat(reply, "strict-transport-security: max-age=63072000; includeSubDomains; preload\n");
                strcat(reply, "x-frame-options: DENY\n");
                strcat(reply, "x-content-type-options: nosniff\n");
                strcat(reply, "x-xss-protection: 1; mode=block\n");
                strcat(reply, "x-robots-tag: none\n");
                strcat(reply, "\n\n");
                strcat(reply, "Hello World");
                SSL_write(ssl, reply, strlen(reply));
            }
        }

        SSL_free(ssl);
        close(client);
    }

    close(sock);
    SSL_CTX_free(ctx);
    cleanup_openssl();
}
