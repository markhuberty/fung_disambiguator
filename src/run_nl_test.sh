#!/bin/bash

eCONFIG=../config/nl_engine_config.txt
bCONFIG=../config/nl_blocking_config.txt

./exedisambig $eCONFIG $bCONFIG > $1
