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
_OTP_ENC_D=xotp_enc_d
_OTP_DEC=xotp_dec
_OTP_DEC_D=xotp_dec_d

gcc -o $_KEYGEN keygen.c
gcc -o $_OTP_ENC otp_enc.c
gcc -o $_OTP_ENC_D otp_enc_d.c
gcc -o $_OTP_DEC otp_dec.c
gcc -o $_OTP_DEC_D otp_dec_d.c


#pathappend #"$(pwd)/$_KEYGEN" "$(pwd)/$_OPT_ENC"
#export PATH=$PATH:.
#export PATH=$PATH:$(pwd)

#echo $PATH
#echo $PATH:$(pwd)

#newpath=$PATH:$(pwd)

#PATH=$newpath

#export PATH
#source ~/.bashrc
#exit

#exec env PATH=$newpath bash

#kill -TERM $$
#exit 1 

