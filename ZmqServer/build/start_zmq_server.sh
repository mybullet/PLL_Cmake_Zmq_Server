#!/bin/bash

# Wait for MySQL server port to be available
while ! nc -z mysql-server 3306; do
  echo "Waiting for MySQL server to start..."
  sleep 1
done

# Start ZMQ server
./ZmqServer
