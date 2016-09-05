# UART-communication
A small library allowing the user to read from and write to an UART port.
Assumes that the package that we are trying to receive divided into small packets that have short time between them.
When that time expires, the package is considered complete.
The code is commented so there should not be any ambiguities.
Still not sure about the flags for the port but, in my case, it is working fine.
