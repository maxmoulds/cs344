#!/usr/bin/env bash


pathappend() {
  for ARG in "$@"
  do
    if [ -d "$ARG" ] && [[ ":$PATH:" != *":$ARG:"* ]]; then
        PATH="${PATH:+"$PATH:"}$ARG"
    fi
  done
}

# Compile script for opt program
#
# export PATH=$(pwd):$PATH
_OTP_ENC=xotp_enc
_KEYGEN=xkeygen

gcc -o $_KEYGEN keygen.c
gcc -o $_OTP_ENC otp_enc.c

#pathappend #"$(pwd)/$_KEYGEN" "$(pwd)/$_OPT_ENC"
#export PATH=$PATH:.

#exit
