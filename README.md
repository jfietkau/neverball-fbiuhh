# Neverball-FBIUHH

## Introduction

*Neverball-FBIUHH* is a special version of *Neverball* suited for hacking away
at the control scheme. It exposes some internal variables to input from a TCP
socket, thereby allowing arbitrary software (or hardware) to control the game
locally or over the network.

This patch was initially written in 2011 for the *Interaction Design* undergrad
course at the University of Hamburg, Germany, and reused in the years
afterwards. The intention is to allow rapid prototyping of new *Neverball*
controllers based on a wide variety of software and hardware platforms.

In conjunction with the patch, two example clients (written in C and Java,
respectively) are provided.

## Installation

Just apply the patch to the *Neverball* 1.5.4 source code. On Ubuntu 13.04 this
should work:

```bash
sudo apt-get install build-essential libjpeg8-dev libphysfs-dev libsdl1.2-dev libsdl-ttf2.0-dev libvorbis-dev gettext
wget http://neverball.org/neverball-1.5.4.tar.gz
tar -xzf neverball-1.5.4.tar.gz
cd neverball-1.5.4
patch -p1 -i ../neverball-fbiuhh.patch
make LDFLAGS="-lX11 -lm"
```

### Client Architecture

The patch creates a tiny client/server interface, where *Neverball* listens on
a TCP port for incoming data, which is then interpreted in chunks of 4
little-endian signed shorts (8 bytes). Each one is mapped in that order to x
tilt, z tilt, camera rotation and camera setting.

The client must connect to a Neverball-FBIUHH instance (default: port 33333)
and regularly send updated values. Ideally, they should be sent once for every
frame of the game (i.e. about 60 times per second on modern machines). Fewer
packets may result in stuttering, but more packets may congest the TCP stream
and cause delayed reactions, so you should err on the side of fewer packets in
most cases.

Example clients written in C (using SDL) and Java (using AWT/Swing) are
provided.

### Author's Remarks

This patch has never actually been sent to *Neverball*'s regular maintainers.
That is because it was written in utter disregard of its existing architecture.
The only thing it does is add a network loop and expose a few internal
variables, driving a large wedge into what the original developers may have had
in mind about separation of concerns and modularity.

I hope that this may anyway be useful to people outside of our institution.
Have fun!
