#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <nginx.h>
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static char *nginx_status_exporter(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t  nginx_status_exporter_commands[] = {

  { ngx_string("stub_status_exporter"),
    NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
    nginx_status_exporter,
    0,
    0,
    NULL },

    ngx_null_command
};


static ngx_http_module_t  nginx_status_exporter_module_ctx = {
  NULL,                          /* preconfiguration */
  NULL,                          /* postconfiguration */
  NULL,                          /* create main configuration */
  NULL,                          /* init main configuration */
  NULL,                          /* create server configuration */
  NULL,                          /* merge server configuration */
  NULL,                          /* create location configuration */
  NULL                           /* merge location configuration */
};


ngx_module_t nginx_status_exporter_module = {
  NGX_MODULE_V1,
  &nginx_status_exporter_module_ctx, /* module context */
  nginx_status_exporter_commands,   /* module directives */
  NGX_HTTP_MODULE,               /* module type */
  NULL,                          /* init master */
  NULL,                          /* init module */
  NULL,                          /* init process */
  NULL,                          /* init thread */
  NULL,                          /* exit thread */
  NULL,                          /* exit process */
  NULL,                          /* exit master */
  NGX_MODULE_V1_PADDING
};

const char templ[] = ""
  "# HELP nginx_up Status of the last metric scrape\n"
  "# TYPE nginx_up gauge\n"
  "nginx_up %uA\n"
  "# HELP nginxexporter_build_info Exporter build information\n"
  "# TYPE nginxexporter_build_info gauge\n"
  "nginx_status_exporter_build_info{gitCommit=\"\",version=\"0.2\"} %uA\n"
  "# HELP nginx_connections_accepted Accepted client connections\n"
  "# TYPE nginx_connections_accepted counter\n"
  "nginx_connections_accepted %uA\n"
  "# HELP nginx_connections_active Active client connections\n"
  "# TYPE nginx_connections_active gauge\n"
  "nginx_connections_active %uA\n"
  "# HELP nginx_connections_handled Handled client connections\n"
  "# TYPE nginx_connections_handled counter\n"
  "nginx_connections_handled %uA\n"
  "# HELP nginx_connections_reading Connections where NGINX is reading the request header\n"
  "# TYPE nginx_connections_reading gauge\n"
  "nginx_connections_reading %uA\n"
  "# HELP nginx_connections_waiting Idle client connections\n"
  "# TYPE nginx_connections_waiting gauge\n"
  "nginx_connections_waiting %uA\n"
  "# HELP nginx_connections_writing Connections where NGINX is writing the response back to the client\n"
  "# TYPE nginx_connections_writing gauge\n"
  "nginx_connections_writing %uA\n"
  "# HELP nginx_http_requests_total Total http requests\n"
  "# TYPE nginx_http_requests_total counter\n"
  "nginx_http_requests_total %uA\n";

static ngx_int_t nginx_status_exporter_handler(ngx_http_request_t *r)
{
  size_t             size;
  ngx_int_t          rc;
  ngx_buf_t         *b;
  ngx_chain_t        out;
  ngx_atomic_int_t   ap, hn, ac, rq, rd, wr, wa;

  if (r->method != NGX_HTTP_GET && r->method != NGX_HTTP_HEAD) {
    return NGX_HTTP_NOT_ALLOWED;
  }

  rc = ngx_http_discard_request_body(r);

  if (rc != NGX_OK) {
    return rc;
  }

  ngx_str_set(&r->headers_out.content_type, "text/plain; version=0.0.4");

  if (r->method == NGX_HTTP_HEAD) {
    r->headers_out.status = NGX_HTTP_OK;

    rc = ngx_http_send_header(r);

    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
      return rc;
    }
  }

  size = sizeof(templ) + NGX_ATOMIC_T_LEN * 9;

  b = ngx_create_temp_buf(r->pool, size);
  if (b == NULL) {
    return NGX_HTTP_INTERNAL_SERVER_ERROR;
  }

  out.buf = b;
  out.next = NULL;

  ap = *ngx_stat_accepted;
  hn = *ngx_stat_handled;
  ac = *ngx_stat_active;
  rq = *ngx_stat_requests;
  rd = *ngx_stat_reading;
  wr = *ngx_stat_writing;
  wa = *ngx_stat_waiting;

  b->last = ngx_sprintf(
    b->last,
    templ,
    1, 1, ap, ac, hn, rd, wa, wr, rq
  );

  r->headers_out.status = NGX_HTTP_OK;
  r->headers_out.content_length_n = b->last - b->pos;

  b->last_buf = (r == r->main) ? 1 : 0;

  rc = ngx_http_send_header(r);

  if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
    return rc;
  }

  return ngx_http_output_filter(r, &out);
}


static char *nginx_status_exporter(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
  ngx_http_core_loc_conf_t  *clcf;

  clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
  clcf->handler = nginx_status_exporter_handler;

  return NGX_CONF_OK;
}
