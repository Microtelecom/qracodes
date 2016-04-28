g++ -O2 -DFTZ_ENABLE -DQRA_DEBUG -DTEST_WER_AWGN *.c -o qracodesawgn  && ./qracodesawgn
g++ -O2 -DFTZ_ENABLE -DQRA_DEBUG -DTEST_WER_RAYLEIGH *.c -o qracodesrayleigh  && ./qracodesrayleigh

