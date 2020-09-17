// To build this: gcc mini-qpu.cpp -lstdc++
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <complex>
typedef std::complex<float> complex;

class QPU {
public:
    QPU(int _num_qubits) {
        num_qubits = _num_qubits;
        num_terms = 1 << num_qubits;
        _sv.resize(num_terms);
        _sv[0] = 1.0;
    }
    ~QPU() {}

    void cnot(int target, int conditions) {
        for (int t1 = 0; t1 < num_terms; ++t1) {
            if (t1 & target && (t1 & conditions) == conditions) {
                int t0 = t1 ^ target;
                complex a = _sv[t0];
                complex b = _sv[t1];
                _sv[t0] = b;
                _sv[t1] = a;
            }
        }
    }

    void had(int target) {
        float r2 = 1.0 / sqrt(2.0);
        for (int t1 = 0; t1 < num_terms; ++t1) {
            if (t1 & target) {
                int t0 = t1 ^ target;
                complex a = _sv[t0];
                complex b = _sv[t1];
                _sv[t0] = (a + b) * r2;
                _sv[t1] = (a - b) * r2;
            }
        }
    }

    void phase(float theta_degrees, int conditions) {
        float rads = M_PI * theta_degrees / 180.0;
        complex rot = complex(cos(rads), sin(rads));
        for (int t1 = 0; t1 < num_terms; ++t1) {
            if ((t1 & conditions) == conditions)
                _sv[t1] *= rot;
        }
    }

    int read(int target) {
        float prob = 0.0;
        for (int t1 = 0; t1 < num_terms; ++t1) {
            if (t1 & target) {
                complex b = _sv[t1];
                prob += b.real() * b.real() + b.imag() * b.imag();
            }
        }
        float scale;
        int result;
        if (((float) rand() / (RAND_MAX)) <= prob) {
            result = target;
            scale = 1.0 / sqrt(1.0 - prob);
        } else {
            result = 0;
            scale = 1.0 / sqrt(prob);
        }
        for (int t1 = 0; t1 < num_terms; ++t1) {
            if ((t1 ^ result) & target)
                _sv[t1] = 0.0;
            else
                _sv[t1] *= scale;
        }
        return result;
    }

    void print_sv() {
        for (int t1 = 0; t1 < num_terms; ++t1)
            printf("(%f,%f), ", _sv[t1].real(), _sv[t1].imag());
        printf("\n");
    }

    int num_qubits;
    int num_terms;
protected:
    std::vector<complex> _sv;
};



int main() {
    srand(time(NULL));

    QPU qc(2);
    qc.print_sv();
    qc.had(1);
    qc.cnot(2, 1);
    qc.phase(90, 1);
    qc.print_sv();
    printf("read %d\n", qc.read(1));
    qc.print_sv();
}
