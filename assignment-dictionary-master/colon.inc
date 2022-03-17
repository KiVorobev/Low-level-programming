%define ARG 0

%macro colon 2

%2: 
dq ARG
db %1, 0

val%2:

%define ARG %2

%endmacro
