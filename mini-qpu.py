import math
import random

class QPU:
    def __init__(self, num_qubits):
        self.num_qubits = num_qubits
        self.num_terms = 1 << num_qubits
        self._sv = [0.0] * self.num_terms
        self._sv[0] = 1.0

    def cnot(self, target, conditions):
        for t1 in range(self.num_terms):
            if t1 & target and (t1 & conditions) == conditions:
                t0 = t1 ^ target
                a,b = self._sv[t0],self._sv[t1]
                self._sv[t0] = b
                self._sv[t1] = a

    def had(self, target):
        r2 = 1.0 / math.sqrt(2.0)
        for t1 in range(self.num_terms):
            if t1 & target:
                t0 = t1 ^ target
                a,b = self._sv[t0],self._sv[t1]
                self._sv[t0] = (a + b) * r2
                self._sv[t1] = (a - b) * r2

    def phase(self, theta_degrees, conditions):
        rads = math.pi * theta_degrees / 180.0
        rot = math.cos(rads) + 1j*math.sin(rads)
        for t1 in range(self.num_terms):
            if (t1 & conditions) == conditions:
                self._sv[t1] *= rot

    def read(self, target):
        prob = 0.0
        for t1 in range(self.num_terms):
            if t1 & target:
                b = self._sv[t1]
                prob += b.real * b.real + b.imag * b.imag
        if random.random() <= prob:
            result = target
            scale = 1.0 / math.sqrt(1.0 - prob)
        else:
            result = 0
            scale = 1.0 / math.sqrt(prob)
        for t1 in range(self.num_terms):
            if ((t1 ^ result) & target):
                self._sv[t1] = 0.0
            else:
                self._sv[t1] *= scale
        return result


def run_test():
    qc = QPU(2)
    print(qc._sv)
    qc.had(1)
    qc.cnot(2, 1)
    qc.phase(90, 1)
    print(qc._sv)
    print('read',qc.read(1))
    print(qc._sv)
    # for i in range(10):
    #     qc.had(1 << i)
    #     qc.phase(10, 1 << i)
    # print(qc._sv)


run_test()
