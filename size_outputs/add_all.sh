#!/bin/bash

awk '{ sum += $1 } END { print sum }' output_tx_20.txt >> tx_out.txt
awk '{ sum += $1 } END { print sum }' output_rx_20.txt >> rx_out.txt

