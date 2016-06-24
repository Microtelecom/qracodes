g++ -march=native -pthread -O3 -DFTZ_ENABLE -DQRA_DEBUG -I../qracodes-mt/  \
*.c                                                                        \
../qracodes-mt/normrnd.c                                                   \
../qracodes-mt/npfwht.c                                                    \
../qracodes-mt/pdmath.c                                                    \
../qracodes-mt/qra12_63_64_irr_b.c                                         \
../qracodes-mt/qra13_64_64_irr_e.c                                         \
../qracodes-mt/qracodes.c                                                  \
-lpthread -o qra65

