#!/bin/bash
echo "статус"
netstat -tulpn |grep slapd

echo "активные соединения"
sockstat|grep :389
