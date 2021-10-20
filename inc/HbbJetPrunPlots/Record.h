#ifndef RECORD_H
#define RECORD_H

class Record {
    public:
        Record(float prun_lev, int np_num, float cpu_time_m0, float cpu_time_m1=0., float cpu_time_m3=0.);
        float getPrunLev() const {return _prun_lev;}
        int getNpNum() const {return _np_num;}
        float getCpuTimeM0() const {return _cpu_time_m0;}
        float getCpuTimeM0h() const {return _cpu_time_m0 / 60;}
        float getCpuTimeM1() const {return _cpu_time_m1;}
        float getCpuTimeM1h() const {return _cpu_time_m1 / 60;}
        float getCpuTimeM3() const {return _cpu_time_m3;}
        float getCpuTimeM3h() const {return _cpu_time_m3 / 60;}
    private:
        // Level of pruning
        float _prun_lev;
        // Number of retained nuisance parameters
        int _np_num;
        // CPU time (in minutes) for a minos 0 fit
        float _cpu_time_m0;
        // CPU time (in minutes) for a minos 1 fit
        float _cpu_time_m1;
        // CPU time (in minutes) for a minos 3 fit
        float _cpu_time_m3;
};

#endif