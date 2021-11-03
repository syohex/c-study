#include <uv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>

static uv_loop_t *loop;
struct Request {
    uv_write_t req;
    char *buf;
};

static void WriteCallback(uv_write_t *req, int status) {
    struct Request *r = (struct Request *)req;
    free(r->buf);
    free(r);
}

static void ReadCallback(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    if (nread < 0) {
        if (nread == UV_EOF) {
            printf("closed\n");
            return;
        }

        free(buf->base);
        return;
    }

    struct Request *req = malloc(sizeof(struct Request));
    if (req == NULL) {
        perror("malloc");
        exit(1);
    }

    req->buf = malloc(nread);
    if (req->buf == NULL) {
        perror("malloc");
        exit(1);
    }

    for (ssize_t i = 0; i < nread; ++i) {
        req->buf[i] = toupper(buf->base[i]);
    }
    free(buf->base);

    uv_buf_t uvbuf;
    uvbuf.base = req->buf;
    uvbuf.len = nread;

    uv_write(&req->req, stream, &uvbuf, 1, WriteCallback);
}

static void AllocBuffer(uv_handle_t *handle, size_t size, uv_buf_t *buf) {
    *buf = uv_buf_init((char *)malloc(size), size);
}

static void ConnectionCallback(uv_stream_t *stream, int status) {
    if (status < 0) {
        printf("## connection error=%d\n", status);
        return;
    }

    uv_tcp_t *client = malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);
    uv_accept(stream, (uv_stream_t *)client);
    uv_read_start((uv_stream_t *)client, AllocBuffer, ReadCallback);
}

int main(void) {
    loop = uv_default_loop();
    if (loop == NULL) {
        perror("uv_default_loop");
        return 1;
    }

    struct sockaddr_in addr;
    uv_ip4_addr("127.0.0.1", 9999, &addr);

    uv_tcp_t server;
    uv_tcp_init(loop, &server);
    uv_tcp_bind(&server, (struct sockaddr *)&addr, 0);
    uv_listen((uv_stream_t *)&server, 128, ConnectionCallback);
    uv_run(loop, UV_RUN_DEFAULT);
    return 0;
}