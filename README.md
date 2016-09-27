# Profiling RDMA Demo Performance
1. Add ```-pg``` to ```CFLAG```.
2. Run ```/test 1``` at server, and ```/test``` at client.
3. ```gprof test gmon.out > test.gprof```
4. ```./gprof2dot.py ../wc-sep-test/test.gprof | dot -Tpng -o output.png``` to generate visualization.
