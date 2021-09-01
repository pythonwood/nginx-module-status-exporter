# nginx-module-status-exporter

export stub status metrics by nginx itself. for prometheus grafana dashboards/12708.

This is an nginx module that provides the same information as the standard <a href="http://nginx.org/en/docs/http/ngx_http_stub_status_module.html">stub_status module</a> but in a format recognized by the <a href="http://prometheus.io">prometheus</a> time-series database.

copy from <https://github.com/mhowlett/ngx_stub_status_prometheus>, format metrics as <https://github.com/nginxinc/nginx-prometheus-exporter>.

Use Grafana Dashboard <https://grafana.com/grafana/dashboards/12708>

## Usage

Place the stub_status_exporter directive in a /metrics location context, as expected by prometheus.
Here is a complete, minimalistic configuration:

    events {
    }

    http {
      server {
        listen 80;
        location /metrics {
          stub_status_exporter;
        }
      }
    }

Metrics for Prometheus

    # HELP nginx_up Status of the last metric scrape
    # TYPE nginx_up gauge
    nginx_up 1
    # HELP nginxexporter_build_info Exporter build information
    # TYPE nginxexporter_build_info gauge
    nginx_status_exporter_build_info{gitCommit="",version="0.2"} 1
    # HELP nginx_connections_accepted Accepted client connections
    # TYPE nginx_connections_accepted counter
    nginx_connections_accepted 149
    # HELP nginx_connections_active Active client connections
    # TYPE nginx_connections_active gauge
    nginx_connections_active 3
    # HELP nginx_connections_handled Handled client connections
    # TYPE nginx_connections_handled counter
    nginx_connections_handled 149
    # HELP nginx_connections_reading Connections where NGINX is reading the request header
    # TYPE nginx_connections_reading gauge
    nginx_connections_reading 0
    # HELP nginx_connections_waiting Idle client connections
    # TYPE nginx_connections_waiting gauge
    nginx_connections_waiting 2
    # HELP nginx_connections_writing Connections where NGINX is writing the response back to the client
    # TYPE nginx_connections_writing gauge
    nginx_connections_writing 1
    # HELP nginx_http_requests_total Total http requests
    # TYPE nginx_http_requests_total counter
    nginx_http_requests_total 9359

In order to prevent public access, you can use the auto and deny directives, or https and basic auth as described here: http://prometheus.io/docs/operating/configuration/#scrape-configurations-scrape_config

Unlike stub_status, stub_status_exporter does not expose any variables corresponding to the status information.
If you need these, you can use the stub_status module alongside the prometheus status module.
Note that the stub_status directive can be placed in a server context for this purpose (does not need to be placed in location context).

Misc notes:

1. The stub_status_exporter directive takes no parameters.
2. The statistics reported are impacted by requests to the metrics endpoint by the prometheus server.
3. The status information exposed relates to the entire nginx server, not the server context in which it is contained.
