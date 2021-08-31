#!/bin/bash

NGINX_VERSION=1.20.1

curl -s -L -O "http://nginx.org/download/nginx-$NGINX_VERSION.tar.gz"
tar xvf nginx-$NGINX_VERSION.tar.gz
