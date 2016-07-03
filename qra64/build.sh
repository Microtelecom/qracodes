g++ -march=native -pthread -O3 -I../qracodes/                           \
*.c                                                                     \
../qracodes/normrnd.c                                                   \
../qracodes/npfwht.c                                                    \
../qracodes/pdmath.c                                                    \
../qracodes/qra12_63_64_irr_b.c                                         \
../qracodes/qra13_64_64_irr_e.c                                         \
../qracodes/qracodes.c                                                  \
-lpthread -o qra64

